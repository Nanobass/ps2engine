//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        ps2glu.hpp
//
// Description: PS2 GL Utility Library
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

/* ps2gl */
#include <GL/ps2gl.h>
#include <GL/gl.h>

/* standard library */
#include <cstddef>
#include <cstdint>

/* system */
#include <osd_config.h>

//========================================
// Project Includes
//========================================

/* ps2math */
#include <ps2math.hpp>

namespace ps2
{
inline float GetSystemAspectRatio()
{
    switch (configGetTvScreenType())
    {
    case TV_SCREEN_43: return 4.0F / 3.0F;
    case TV_SCREEN_169: return 16.0F / 9.0F;
    case TV_SCREEN_FULL: return 1.0F; // should i ingore this one???
    default: return 4.0F / 3.0F; // we should never get here!!!
    }
}
}

inline void gluClearColor(const pse::math::color& color) 
{
    glClearColor(color.r, color.g, color.b, color.a);
}

inline void gluLoadMatrix(const pse::math::mat4& matrix) 
{ 
    glLoadMatrixf(matrix.matrix); 
}

inline void gluMultMatrix(const pse::math::mat4& matrix) 
{ 
    glMultMatrixf(matrix.matrix); 
}

inline void gluPushMatrix(const pse::math::mat4& matrix) 
{ 
    glPushMatrix();
    glMultMatrixf(matrix.matrix); 
}

inline void gluPerspective(float fov, float near, float far, float aspect) 
{
    float h = 2.0f * near * (float)tanf(fov * 3.141592654f / 180.0f / 2.0f);
    float w = h * aspect;
    glFrustum(-w / 2.0f, w / 2.0f, -h / 2.0f, h / 2.0f, near, far);
}

inline void gluLookAt(pse::math::vec4 position, pse::math::vec4 target, pse::math::vec4 up)
{
    pse::math::mat4 view = pse::math::lookAt(position, target, up);
    gluMultMatrix(view);
}

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}