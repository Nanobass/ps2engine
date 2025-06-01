//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        texturemanager.hpp
//
// Description: Texture Manager
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

/* standard library */
#include <memory>
#include <map>
#include <cassert>
#include <fstream>
#include <iostream>

/* ps2gl */
#include <GL/ps2gl.h>
#include <GL/gl.h>

/* ps2stuff */
#include <ps2s/gs.h>

//========================================
// Project Includes
//========================================

/* ps2memory */
#include <ps2memory.hpp>

/* ps2glu */
#include <ps2glu.hpp>

/* ps2math */
#include <ps2math.hpp>


namespace pse
{

namespace PS2
{

void GetOpenGLFormatAndType(GS::tPSM psm, GLenum& format, GLenum& type);
    
} // namespace PS2

struct TextureBuffer
{
    static uint32_t GetSize(uint16_t width, uint16_t height, uint8_t bpp)
    {
        return width * height * bpp / 8;
    }

    uint16_t mWidth = 0;
    uint16_t mHeight = 0;
    uint8_t mBpp = 0;
    GLenum mFormat = GL_INVALID_ENUM;
    GLenum mType = GL_INVALID_ENUM;
    memory::buffer<uint8_t> mData;

    TextureBuffer(uint16_t width, uint16_t height, uint8_t bpp, GLenum format, GLenum type)
        :   mWidth(width),
            mHeight(height),
            mBpp(bpp),
            mFormat(format),
            mType(type),
            mData(GetSize())
    {}

    TextureBuffer(const TextureBuffer&) = delete;
    TextureBuffer& operator=(const TextureBuffer&) = delete;

    TextureBuffer(TextureBuffer&& other) noexcept
        : mWidth(std::exchange(other.mWidth, 0)),
          mHeight(std::exchange(other.mHeight, 0)),
          mBpp(std::exchange(other.mBpp, 0)),
          mFormat(std::exchange(other.mFormat, GL_INVALID_ENUM)),
          mType(std::exchange(other.mType, GL_INVALID_ENUM)),
          mData(std::move(other.mData))
    {}

    uint32_t GetSize() 
    {
        return GetSize(mWidth, mHeight, mBpp);
    }
};

struct Texture 
{
    memory::name mName;
    TextureBuffer* mCore = nullptr;
    TextureBuffer* mClutBuffer = nullptr;
    GLuint mGLName;

    Texture(memory::name name, TextureBuffer core) : mName(name), mGLName(0)
    {
        mCore = new TextureBuffer(std::move(core));
        glGenTextures(1, &mGLName);
        Upload();
        operator<<(std::cout << "Texture Created: ") << std::endl;
    }

    Texture(memory::name name, TextureBuffer core, TextureBuffer clut) : mName(name)
    {
        mCore = new TextureBuffer(std::move(core));
        mClutBuffer = new TextureBuffer(std::move(clut));
        glGenTextures(1, &mGLName);
        Upload();
        operator<<(std::cout << "Texture Created: ") << std::endl;
    }

    virtual ~Texture()
    {
        operator<<(std::cout << "Texture Deleted: ") << std::endl;
        if(mCore) delete mCore;
        if(mClutBuffer) delete mClutBuffer;
        if(mGLName != 0) glDeleteTextures(1, &mGLName);
    }

    void SetFilter(GLenum filter)
    {
        glBindTexture(GL_TEXTURE_2D, mGLName);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Upload()
    {
        glBindTexture(GL_TEXTURE_2D, mGLName);
        glTexImage2D(GL_TEXTURE_2D, 0 /* mipmap */, GL_RGBA /* ignored */, mCore->mWidth, mCore->mHeight, 0 /* border */, mCore->mFormat, mCore->mType, mCore->mData.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Bind()
    {
        glBindTexture(GL_TEXTURE_2D, mGLName);
        if(mClutBuffer) glColorTable(GL_COLOR_TABLE, GL_RGBA, 256, GL_RGBA, GL_UNSIGNED_INT, mClutBuffer->mData.data());
    }

    int GetWidth() const { return mCore->mWidth; }
    int GetHeight() const { return mCore->mHeight; }

    std::ostream& operator<<(std::ostream& os)
    {
        os << "Texture ID=\"" << mName.mStringName << "\" (" << mName.mID << ")" << " GL=" << mGLName << " " << mCore->mWidth << "x" << mCore->mHeight; 
        return os;
    }

};

struct GsTextureHeader {
    static const uint32_t MAGIC = 'G' | ('T' << 8) | ('E' << 16) | ('X' << 24);
    uint32_t mMagic;
    uint16_t mWidth;
    uint16_t mHeight;
    uint8_t mPsm;
    uint16_t mClutWidth;
    uint16_t mClutHeight;
    uint8_t mClutPsm;
    uint8_t mComponents;
    uint8_t mFunction;
} __attribute__ ((packed));

struct TextureManager
{

    // gotta love c++
    std::map<uint32_t, std::unique_ptr<Texture>> mTextures;

    TextureManager(uint32_t vramStart, uint32_t vramEnd)
    {
        std::cout << "Initializing TextureManager: vramStart=" << vramStart << ", vramEnd=" << vramEnd << std::endl;
        InitializeGsMemory(vramStart, vramEnd);
        
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND); // free on ps2 iirc
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    }

    ~TextureManager()
    {
        mTextures.clear();
    }

    void InitializeGsMemory(uint32_t vramStart, uint32_t vramEnd);

    Texture* LoadTexture(const memory::name& name, const std::string& path)
    {

        if(ends_with(path, "gs"))
        {
            return LoadGsTexture(name, path);
        }
        if(ends_with(path, "png"))
        {
            return LoadPNG(name, path);
        }
        throw std::exception();
    }

    Texture* LoadGsTexture(const memory::name& name, const std::string& path);

    Texture* LoadPNG(const memory::name& name, const std::string& path);

    Texture* CreateTexture(const memory::name& name, TextureBuffer core)
    {
        auto texture = std::make_unique<Texture>(name, std::move(core));
        Texture* pointer = texture.get();
        mTextures[name.mID] = std::move(texture);
        return pointer;
    }

    Texture* CreateTexture(const memory::name& name, TextureBuffer core, TextureBuffer clut)
    {
        auto texture = std::make_unique<Texture>(name, std::move(core), std::move(clut));
        Texture* pointer = texture.get();
        mTextures[name.mID] = std::move(texture);
        return pointer;
    }

    Texture* FindTexture(const uint32_t& name)
    {
        auto it = mTextures.find(name);
        return it->second.get();
    }

    void DeleteTexture(const uint32_t& name)
    {
        auto it = mTextures.find(name);
        mTextures.erase(it);
    }

};
    
} // namespace engine
