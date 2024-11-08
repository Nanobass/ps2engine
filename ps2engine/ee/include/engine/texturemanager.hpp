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

/* ps2gl */
#include <GL/ps2gl.h>
#include <GL/gl.h>

//========================================
// Project Includes
//========================================

/* ps2memory */
#include <ps2memory.hpp>

/* ps2glu */
#include <ps2glu.hpp>

/* ps2math */
#include <ps2math.hpp>


namespace Engine
{

namespace PS2
{

void GetOpenGLFormatAndType(GS::tPSM psm, GLenum& format, GLenum& type)
{
    switch(psm)
    {
        case GS::kPsm32: 
            format = GL_RGBA;
            type = GL_UNSIGNED_BYTE;
        break;
        case GS::kPsm24: 
            format = GL_RGB;
            type = GL_UNSIGNED_BYTE;
        break;
        case GS::kPsm16:
            format = GL_RGBA;
            type = GL_UNSIGNED_SHORT_5_5_5_1;
        break;
        case GS::kPsm8:
            format = GL_COLOR_INDEX;
            type = GL_UNSIGNED_BYTE;
        break;
        default:
        break;
    }
}
    
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
    buffer<uint8_t> mData;

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
    name mName;
    TextureBuffer* mCore = nullptr;
    TextureBuffer* mClutBuffer = nullptr;
    GLuint mGLName;

    Texture(name name, TextureBuffer core) : mName(name), mGLName(0)
    {
        mCore = new TextureBuffer(std::move(core));
        glGenTextures(1, &mGLName);
        Upload();
        operator<<(std::cout << "Texture Created: ") << std::endl;
    }

    Texture(name name, TextureBuffer core, TextureBuffer clut) : mName(name)
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

    void Upload()
    {
        glBindTexture(GL_TEXTURE_2D, mGLName);
        glTexImage2D(GL_TEXTURE_2D, 0 /* mipmap */, GL_RGBA /* ignored */, mCore->mWidth, mCore->mHeight, 0 /* border */, mCore->mFormat, mCore->mType, mCore->mData.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
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
        // this is a temporary arrangement, later texture pages will dynamically move these boundries
        
        // 64x32
        pglAddGsMemSlot(240,  1,   GS::kPsm32);
        pglAddGsMemSlot(241,  1,   GS::kPsm32);
        pglAddGsMemSlot(242,  1,   GS::kPsm32);
        pglAddGsMemSlot(243,  1,   GS::kPsm32);
        // 64x64
        pglAddGsMemSlot(244,  2,   GS::kPsm32);
        pglAddGsMemSlot(246,  2,   GS::kPsm32);
        pglAddGsMemSlot(248,  2,   GS::kPsm32);
        pglAddGsMemSlot(250,  2,   GS::kPsm32);
        pglAddGsMemSlot(252,  2,   GS::kPsm32);
        pglAddGsMemSlot(254,  2,   GS::kPsm32);
        // 128x128
        pglAddGsMemSlot(256,  8,   GS::kPsm32);
        pglAddGsMemSlot(264,  8,   GS::kPsm32);
        pglAddGsMemSlot(272,  8,   GS::kPsm32);
        pglAddGsMemSlot(280,  8,   GS::kPsm32);
        pglAddGsMemSlot(288,  8,   GS::kPsm32);
        pglAddGsMemSlot(296,  8,   GS::kPsm32);
        pglAddGsMemSlot(304,  8,   GS::kPsm32);
        pglAddGsMemSlot(312,  8,   GS::kPsm32);
        // 256x256
        pglAddGsMemSlot(320,  32,  GS::kPsm32);
        pglAddGsMemSlot(352,  32,  GS::kPsm32);
        // 512x256
        pglAddGsMemSlot(384,  64,  GS::kPsm32);
        pglAddGsMemSlot(448,  64,  GS::kPsm32);
        
        glEnable(GL_TEXTURE_2D); // kinda needed
        glEnable(GL_BLEND); // free on ps2 iirc
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // no one uses the rest
    }

    ~TextureManager()
    {
        mTextures.clear();
    }

    Texture* LoadGsTexture(const name& name, const std::string& path)
    {
        std::ifstream resource(path, std::ios_base::in);
        GsTextureHeader header;
        resource.read((char*) &header, sizeof(GsTextureHeader));

        GLenum format, type;
        PS2::GetOpenGLFormatAndType((GS::tPSM) header.mPsm, format, type);
        uint8_t bpp = 0;
        if(header.mPsm == GS_PSM_4) bpp = 4;
        if(header.mPsm == GS_PSM_8) bpp = 8;
        if(header.mPsm == GS_PSM_16) bpp = 16;
        if(header.mPsm == GS_PSM_24) bpp = 24;
        if(header.mPsm == GS_PSM_32) bpp = 32;

        TextureBuffer core(header.mWidth, header.mHeight, bpp, format, type);
        resource.read((char*) core.mData.data(), core.mData.size());

        if( header.mPsm == GS_PSM_8 || header.mPsm == GS_PSM_4 )
        {
            PS2::GetOpenGLFormatAndType((GS::tPSM) header.mPsm, format, type);
            if(header.mClutPsm == GS_PSM_16) bpp = 16;
            if(header.mClutPsm == GS_PSM_24) bpp = 24;
            if(header.mClutPsm == GS_PSM_32) bpp = 32;
            TextureBuffer clut(header.mClutWidth, header.mClutHeight, bpp, format, type);
            resource.read((char*) clut.mData.data(), clut.mData.size());
            return CreateTexture(name, std::move(core), std::move(clut));
        } else {
            return CreateTexture(name, std::move(core));
        }
    }

    Texture* CreateTexture(const name& name, TextureBuffer core)
    {
        auto texture = std::make_unique<Texture>(name, std::move(core));
        Texture* pointer = texture.get();
        mTextures[name.mID] = std::move(texture);
        return pointer;
    }

    Texture* CreateTexture(const name& name, TextureBuffer core, TextureBuffer clut)
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
