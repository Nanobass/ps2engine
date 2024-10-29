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

#include <vector>
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

enum TextureFormat
{
    TEXTURE_RGBA_8_8_8_8,
    TEXTURE_RGB_8_8_8,
    TEXTURE_RGBA_5_5_5_1,
    TEXTURE_RGBA_4_4_4_4,
    TEXTURE_INDEX_8,
    TEXTURE_INDEX_4,
    TEXTURE_INVALID = -1
};

namespace GL
{
void GetOpenGLTextureFormatAndType(TextureFormat texformat, GLenum& format, GLenum& type)
{
    switch (texformat)
        {
        case TEXTURE_RGBA_8_8_8_8: 
            format = GL_RGBA;
            type = GL_UNSIGNED_INT_8_8_8_8;
        break;
        case TEXTURE_RGB_8_8_8: 
            format = GL_RGB;
            type = GL_UNSIGNED_BYTE;
        break;
        case TEXTURE_RGBA_5_5_5_1: 
            format = GL_RGBA;
            type = GL_UNSIGNED_SHORT_5_5_5_1;
        break;
        case TEXTURE_RGBA_4_4_4_4:
            format = GL_RGBA;
            type = GL_UNSIGNED_SHORT_4_4_4_4;
        break;
        case TEXTURE_INDEX_8:  
            format = GL_COLOR_INDEX;
            type = GL_UNSIGNED_BYTE;
        break;
        case TEXTURE_INDEX_4:
            format = GL_COLOR_INDEX;
            type = GL_UNSIGNED_SHORT_4_4_4_4; // is this right????, no it's not!!! ps2gl has no 4 bit textures (yet)
        break;
        case TEXTURE_INVALID: break;
    }
}
} // namespace opengl

namespace PS2
{

TextureFormat GetTextureFormat(GS::tPSM psm)
{
    switch(psm)
    {
        case GS::kPsm32: 
            return TEXTURE_RGBA_8_8_8_8;
        case GS::kPsm24: 
            return TEXTURE_RGB_8_8_8;
        case GS::kPsm16: 
        case GS::kPsm16s: 
            return TEXTURE_RGBA_5_5_5_1;
        case GS::kPsm8: 
        case GS::kPsm8h: 
            return TEXTURE_INDEX_8;
        case GS::kPsm4: 
        case GS::kPsm4hh: 
        case GS::kPsm4hl: 
            return TEXTURE_INDEX_4;
        default: 
            return TEXTURE_INVALID;
    }
}
    
} // namespace PS2

struct TextureBuffer
{
    static uint32_t GetSize(uint16_t width, uint16_t height, TextureFormat psm)
    {
        switch(psm)
        {
        case TEXTURE_RGBA_8_8_8_8: 
            return width * height * 4;
        case TEXTURE_RGB_8_8_8: 
            return width * height * 3;
        case TEXTURE_RGBA_5_5_5_1: 
        case TEXTURE_RGBA_4_4_4_4:
            return width * height * 2;
        case TEXTURE_INDEX_8:
            return width * height;
        case TEXTURE_INDEX_4:
            return width * height * 4 / 8;
        default: return 0;
        }
    }

    uint16_t mWidth = 0;
    uint16_t mHeight = 0;
    TextureFormat mFormat = TEXTURE_INVALID;
    buffer<uint8_t> mData;

    TextureBuffer(uint16_t width, uint16_t height, TextureFormat format)
        :   mWidth(width),
            mHeight(height),
            mFormat(format),
            mData(GetSize(), GMA_GRAPHICS) // store textures in graphics
    {}

    TextureBuffer(const TextureBuffer&) = delete;
    TextureBuffer& operator=(const TextureBuffer&) = delete;

    TextureBuffer(TextureBuffer&& other) noexcept
        : mWidth(std::exchange(other.mWidth, 0)),
          mHeight(std::exchange(other.mHeight, 0)),
          mFormat(std::exchange(other.mFormat, TEXTURE_INVALID)),
          mData(std::move(other.mData))
    {}

    uint32_t GetSize() 
    {
        return GetSize(mWidth, mHeight, mFormat);
    }
};

struct Texture 
{
    uint32_t mName;
    TextureBuffer* mCore = nullptr;
    TextureBuffer* mClutBuffer = nullptr;
    GLuint mGLName;

    Texture(uint32_t name, TextureBuffer core) : mName(name), mGLName(0)
    {
        mCore = new TextureBuffer(std::move(core));
        glGenTextures(1, &mGLName);
        Upload();
        operator<<(std::cout << "Texture Created: ") << std::endl;
    }

    Texture(uint32_t name, TextureBuffer core, TextureBuffer clut) : mName(name)
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
        GLenum format, type;
        GL::GetOpenGLTextureFormatAndType(mCore->mFormat, format, type);
        glBindTexture(GL_TEXTURE_2D, mGLName);
        glTexImage2D(GL_TEXTURE_2D, 0 /* mipmap */, GL_RGBA /* ignored */, mCore->mWidth, mCore->mHeight, 0 /* border */, format, type, mCore->mData.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        if(mClutBuffer)
        {
            uint32_t width = 0;
            if(mCore->mFormat == TEXTURE_INDEX_8) width = 256;
            else if(mCore->mFormat == TEXTURE_INDEX_4) width = 16;
            GLenum format, type;
            GL::GetOpenGLTextureFormatAndType(mClutBuffer->mFormat, format, type);
            glColorTable(GL_COLOR_TABLE, GL_RGBA, width, format, type, mClutBuffer->mData.data());
        }
    }

    TextureFormat GetFormat() { return mCore->mFormat; }
    uint32_t GetWidth() { return mCore->mWidth; }
    virtual uint32_t GetHeight() { return mCore->mHeight; }
    virtual uint8_t GetNumberOfMipMaps() { return 1; }

    std::ostream& operator<<(std::ostream& os)
    {
        os << "Texture ID=" << mName << " GL=" << mGLName << " " << mCore->mWidth << "x" << mCore->mHeight << " "; 
        switch (mCore->mFormat)
        {
        case TEXTURE_RGBA_8_8_8_8:
            os << "32-Bit";
        break;
        case TEXTURE_RGB_8_8_8:
            os << "24-Bit";
        break;
        case TEXTURE_RGBA_5_5_5_1:
            os << "16-Bit";
        break;
        case TEXTURE_INDEX_8:
            os << "8-Bit";
        break;
        case TEXTURE_INDEX_4:
            os << "4-Bit";
        break;
        default: break;
        }
        if(mClutBuffer && (mCore->mFormat == TEXTURE_INDEX_8 || mCore->mFormat == TEXTURE_INDEX_4))
        {
            switch (mClutBuffer->mFormat)
            {
                case TEXTURE_RGBA_8_8_8_8:
                    os << " (32-Bit)";
                break;
                case TEXTURE_RGB_8_8_8:
                    os << " (24-Bit)";
                break;
                case TEXTURE_RGBA_5_5_5_1:
                    os << " (16-Bit)";
                break;
                default: break;
            }
        }
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
    }

    ~TextureManager()
    {
        mTextures.clear();
    }

    Texture* LoadGsTexture(const uint32_t& name, const std::string& path)
    {
        std::ifstream resource(path, std::ios_base::in);
        GsTextureHeader header;
        resource.read((char*) &header, sizeof(GsTextureHeader));

        TextureBuffer core(header.mWidth, header.mHeight, PS2::GetTextureFormat((GS::tPSM) header.mPsm));
        stream::read_buf(resource, core.mData);

        if( header.mPsm == GS_PSM_8 || header.mPsm == GS_PSM_4 )
        {
            TextureBuffer clut(header.mClutWidth, header.mClutHeight, PS2::GetTextureFormat((GS::tPSM) header.mClutPsm));
            stream::read_buf(resource, clut.mData);
            return CreateTexture(name, std::move(core), std::move(clut));
        } else {
            return CreateTexture(name, std::move(core));
        }
    }

    Texture* CreateTexture(const uint32_t& name, TextureBuffer core)
    {
        auto texture = std::make_unique<Texture>(name, std::move(core));
        Texture* pointer = texture.get();
        mTextures[name] = std::move(texture);
        return pointer;
    }

    Texture* CreateTexture(const uint32_t& name, TextureBuffer core, TextureBuffer clut)
    {
        auto texture = std::make_unique<Texture>(name, std::move(core), std::move(clut));
        Texture* pointer = texture.get();
        mTextures[name] = std::move(texture);
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
