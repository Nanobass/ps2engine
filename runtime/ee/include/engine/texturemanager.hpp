//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        texturemanager.hpp
//
// Description: texture Manager
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

/* standard library */
#include <map>
#include <memory>

//========================================
// Project Includes
//========================================

/* core */
#include <core/log.hpp>
#include <core/math.hpp>
#include <core/memory.hpp>

/* ps2gl */
#include <GL/gl.h>
#include <GL/ps2gl.h>
#include <GL/ps2glu.hpp>

/* ps2stuff */
#include <ps2s/gs.h>


namespace pse
{

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

struct texture_buffer
{
    static uint32_t get_size(uint16_t width, uint16_t height, uint8_t bpp)
    {
        return width * height * bpp / 8;
    }

    uint16_t mWidth = 0;
    uint16_t mHeight = 0;
    uint8_t mBpp = 0;
    GLenum mFormat = GL_INVALID_ENUM;
    GLenum mType = GL_INVALID_ENUM;
    memory::buffer<uint8_t> mData;

    texture_buffer(uint16_t width, uint16_t height, uint8_t bpp, GLenum format, GLenum type)
        :   mWidth(width),
            mHeight(height),
            mBpp(bpp),
            mFormat(format),
            mType(type),
            mData(get_size())
    {}

    texture_buffer(const texture_buffer&) = delete;
    texture_buffer& operator=(const texture_buffer&) = delete;

    texture_buffer(texture_buffer&& other) noexcept
        : mWidth(std::exchange(other.mWidth, 0)),
          mHeight(std::exchange(other.mHeight, 0)),
          mBpp(std::exchange(other.mBpp, 0)),
          mFormat(std::exchange(other.mFormat, GL_INVALID_ENUM)),
          mType(std::exchange(other.mType, GL_INVALID_ENUM)),
          mData(std::move(other.mData))
    {}

    uint32_t get_size() 
    {
        return get_size(mWidth, mHeight, mBpp);
    }
};

struct texture 
{
    memory::name mName;
    texture_buffer* mCore = nullptr;
    texture_buffer* mClutBuffer = nullptr;
    GLuint mGLName;

    texture(memory::name name, texture_buffer core) : mName(name), mGLName(0)
    {
        mCore = new texture_buffer(std::move(core));
        glGenTextures(1, &mGLName);
        upload();
        operator<<(log::out(log::kInfo) << "texture created: ") << std::endl;
    }

    texture(memory::name name, texture_buffer core, texture_buffer clut) : mName(name)
    {
        mCore = new texture_buffer(std::move(core));
        mClutBuffer = new texture_buffer(std::move(clut));
        glGenTextures(1, &mGLName);
        upload();
        operator<<(log::out(log::kInfo) << "texture created: ") << std::endl;
    }

    virtual ~texture()
    {
        operator<<(log::out(log::kInfo) << "texture deleted: ") << std::endl;
        if(mCore) delete mCore;
        if(mClutBuffer) delete mClutBuffer;
        if(mGLName != 0) glDeleteTextures(1, &mGLName);
    }

    void set_filter(GLenum filter)
    {
        glBindTexture(GL_TEXTURE_2D, mGLName);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void upload()
    {
        glBindTexture(GL_TEXTURE_2D, mGLName);
        glTexImage2D(GL_TEXTURE_2D, 0 /* mipmap */, GL_RGBA /* ignored */, mCore->mWidth, mCore->mHeight, 0 /* border */, mCore->mFormat, mCore->mType, mCore->mData.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void bind()
    {
        glBindTexture(GL_TEXTURE_2D, mGLName);
        if(mClutBuffer) glColorTable(GL_COLOR_TABLE, GL_RGBA, 256, GL_RGBA, GL_UNSIGNED_INT, mClutBuffer->mData.data());
    }

    int get_width() const { return mCore->mWidth; }
    int get_height() const { return mCore->mHeight; }

    std::ostream& operator<<(std::ostream& os)
    {
        os << "texture uuid=\"" << mName.mName << "\" (" << (uint32_t) mName.mUuid << ")" << " GL=" << mGLName << " " << mCore->mWidth << "x" << mCore->mHeight; 
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

struct texture_manager
{

    // gotta love c++
    std::map<uuid, std::unique_ptr<texture>> mTextures;

    texture_manager(uint32_t vramStart, uint32_t vramEnd)
    {
        log::out(log::kInfo) << "initializing texture manager: vramStart=" << vramStart << ", vramEnd=" << vramEnd << std::endl;
        initialize_gs_memory(vramStart, vramEnd);
        
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND); // free on ps2 iirc
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    }

    ~texture_manager()
    {
        mTextures.clear();
    }

    void initialize_gs_memory(uint32_t vramStart, uint32_t vramEnd);

    texture* load_texture(const memory::name& name, const std::string& path)
    {

        if(ends_with(path, "gs"))
        {
            return load_gs_texture(name, path);
        }
        if(ends_with(path, "png"))
        {
            return load_png(name, path);
        }
        throw std::exception();
    }

    texture* load_gs_texture(const memory::name& name, const std::string& path);

    texture* load_png(const memory::name& name, const std::string& path);

    texture* create_texture(const memory::name& name, texture_buffer core)
    {
        auto tex = std::make_unique<texture>(name, std::move(core));
        texture* pointer = tex.get();
        mTextures[name.mUuid] = std::move(tex);
        return pointer;
    }

    texture* create_texture(const memory::name& name, texture_buffer core, texture_buffer clut)
    {
        auto tex = std::make_unique<texture>(name, std::move(core), std::move(clut));
        texture* pointer = tex.get();
        mTextures[name.mUuid] = std::move(tex);
        return pointer;
    }

    texture* find_texture(const uuid& uuid)
    {
        auto it = mTextures.find(uuid);
        return it->second.get();
    }

    void delete_texture(const uuid& uuid)
    {
        auto it = mTextures.find(uuid);
        mTextures.erase(it);
    }

};
    
} // namespace engine
