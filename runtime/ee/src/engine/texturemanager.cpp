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

texture* texture_manager::load_gs_texture(const memory::name& name, const std::string& path)
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
        return create_texture(name, std::move(core), std::move(clut));
    } else {
        return create_texture(name, std::move(core));
    }
}

texture* texture_manager::load_png(const memory::name& name, const std::string& path)
{
    FILE* file = fopen(path.c_str(), "r");
    if (!file) {
        throw std::runtime_error("failed to open file");
    }

    png_byte header[8];
    fread(header, sizeof(png_byte), 8, file);
    if (png_sig_cmp(header, 0, 8)) {
        fclose(file);
        throw std::runtime_error("file is not a png");
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        fclose(file);
        throw std::runtime_error("failed to create png read struct");
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, nullptr, nullptr);
        fclose(file);
        throw std::runtime_error("failed to create png info struct");
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, nullptr);
        fclose(file);
        throw std::runtime_error("png read error");
    }

    png_uint_32 width, height;
    int bit_depth, color_type;

    png_init_io(png, file);
    png_set_sig_bytes(png, 8);
    png_read_info(png, info);
    png_get_IHDR(png, info, &width, &height, &bit_depth, &color_type, nullptr, nullptr, nullptr);
    png_read_update_info(png, info);
    size_t row_bytes = png_get_rowbytes(png, info);

    int bpp = row_bytes * 8 / width;

    try {
        GLenum format = (color_type == PNG_COLOR_TYPE_RGBA) ? GL_RGBA : GL_RGB;
        GLenum type = GL_UNSIGNED_BYTE;
        texture_buffer core(width, height, bpp, format, type);
        memory::buffer<png_bytep> row_pointers(height);

        for (size_t i = 0; i < height; i++) row_pointers[i] = core.mData.data() + i * row_bytes;
        png_read_image(png, row_pointers.data());
        png_destroy_read_struct(&png, &info, nullptr);
        fclose(file);

        return create_texture(std::move(name), std::move(core));
    } catch(const std::exception& e)
    {
        png_destroy_read_struct(&png, &info, nullptr);
        fclose(file);
        throw e;
    }   
}

} // namespace pse
