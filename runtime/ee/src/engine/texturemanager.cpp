//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        texturemanager.cpp
//
// Description: 
//
//=============================================================================

#include <engine/texturemanager.hpp>

//========================================
// System Includes
//========================================

/* libpng */
#include <png.h>

/* ps2sdk */
#include <gs_psm.h>

/* standard library */
#include <fstream>

//========================================
// Project Includes
//========================================

namespace pse
{

void texture_manager::initialize_gs_memory(uint32_t vramStart, uint32_t vramEnd)
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

    pglPrintGsMemAllocation();
}

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

texture_ptr texture_manager::load_gs_texture(const memory::resource_id& id, const std::string& path)
{
    std::ifstream resource(path, std::ios_base::in);
    GsTextureHeader header;
    resource.read((char*) &header, sizeof(GsTextureHeader));

    assert(header.mMagic == GsTextureHeader::MAGIC);

    GLenum format = 0, type = 0;
    PS2::GetOpenGLFormatAndType((GS::tPSM) header.mPsm, format, type);
    uint8_t bpp = 0;
    if(header.mPsm == GS_PSM_4) bpp = 4;
    if(header.mPsm == GS_PSM_8) bpp = 8;
    if(header.mPsm == GS_PSM_16) bpp = 16;
    if(header.mPsm == GS_PSM_24) bpp = 24;
    if(header.mPsm == GS_PSM_32) bpp = 32;

    texture_buffer core(header.mWidth, header.mHeight, bpp, format, type);
    resource.read((char*) core.mData.data(), core.mData.size());

    if( header.mPsm == GS_PSM_8 || header.mPsm == GS_PSM_4 )
    {
        PS2::GetOpenGLFormatAndType((GS::tPSM) header.mPsm, format, type);
        if(header.mClutPsm == GS_PSM_16) bpp = 16;
        if(header.mClutPsm == GS_PSM_24) bpp = 24;
        if(header.mClutPsm == GS_PSM_32) bpp = 32;
        texture_buffer clut(header.mClutWidth, header.mClutHeight, bpp, format, type);
        resource.read((char*) clut.mData.data(), clut.mData.size());
        return create_texture(id, core, clut);
    } else {
        return create_texture(id, core);
    }
}

} // namespace pse
