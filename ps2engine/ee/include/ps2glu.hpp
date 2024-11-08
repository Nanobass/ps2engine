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

//========================================
// Project Includes
//========================================

/* ps2math */
#include <ps2math.hpp>

namespace ps2
{
float GetSystemAspectRatio()
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

static inline Math::Color g_AmbientLight;

void gluClearColor(const Math::Color& color) 
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void gluLoadMatrix(const Math::Mat4& matrix) 
{ 
    glLoadMatrixf(matrix.matrix); 
}

void gluMultMatrix(const Math::Mat4& matrix) 
{ 
    glMultMatrixf(matrix.matrix); 
}

void gluPerspective(float fov, float near, float far, float aspect) 
{
    float h = 2.0f * near * (float)tanf(fov * 3.141592654f / 180.0f / 2.0f);
    float w = h * aspect;
    glFrustum(-w / 2.0f, w / 2.0f, -h / 2.0f, h / 2.0f, near, far);
}

void gluLookAt(Math::Vec4 position, Math::Vec4 target, Math::Vec4 up)
{
    Math::Mat4 view = Math::LookAt(position, target, up);
    gluMultMatrix(view);
}