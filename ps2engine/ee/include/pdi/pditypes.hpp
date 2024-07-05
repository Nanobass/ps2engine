//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        pditypes.hpp
//
// Description: Platform Driver Interface Types
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

/* standard library */
#include <math.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <list>

//========================================
// Project Includes
//========================================

/* ps2math */
#include <ps2math.hpp>

namespace pdi
{

struct DisplayMode
{
    uint32_t width, height;
    uint32_t bpp;
};

enum MatrixMode
{
    PDI_MATRIX_MODELVIEW, PDI_MATRIX_PROJECTION, PDI_MATRIX_TEXTURE
};

enum LightType
{
    PDI_LIGHT_DIRECTIONAL, PDI_LIGHT_POINT, PDI_LIGHT_SPOT
};

enum CullMode
{
    PDI_CULL_OFF, PDI_CULL_BACK, PDI_CULL_FRONT
};

enum CompareMode
{
    PDI_NEVER, PDI_ALWAYS, PDI_LESS, PDI_LESSEQUAL, PDI_GREATER, PDI_GREATEREQUAL, PDI_EQUAL, PDI_NOTEQUAL
};

enum BufferMask
{
    PDI_COLOR_BUFFER = 1, PDI_DEPTH_BUFFER = 2
};

enum FillMode
{
    PDI_FILL, PDI_LINE, PDI_POINT
};

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

enum PrimType
{
    PRIM_TRIANGLES, PRIM_TRIANGLE_STRIP,
    PRIM_QUADS, PRIM_QUAD_STRIP,
    PRIM_LINES, PRIM_LINE_STRIP,
    PRIM_POINTS
};

} // namespace pdi
