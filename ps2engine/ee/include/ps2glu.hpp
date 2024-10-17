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

//========================================
// System Includes
//========================================

/* ps2gl */
#include <GL/ps2gl.h>
#include <GL/gl.h>

//========================================
// Project Includes
//========================================

/* ps2math */
#include <ps2math.hpp>

enum LightType
{
    GLU_LIGHT_DIRECTIONAL, GLU_LIGHT_POINT
};

static inline Math::Color g_AmbientLight;

void gluClearColor(const Math::Color& color) 
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void gluAmbientLight(const Math::Color& color)
{
    g_AmbientLight = color;
    for(uint32_t GL_LIGHTi = GL_LIGHT0; GL_LIGHTi <= GL_LIGHT7; GL_LIGHTi++)
    {
        glLightfv(GL_LIGHTi, GL_AMBIENT, color.vector);
    }
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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float h = 2.0f * near * (float)tanf(fov * 3.141592654f / 180.0f / 2.0f);
    float w = h * aspect;
    glFrustum(-w / 2.0f, w / 2.0f, -h / 2.0f, h / 2.0f, near, far);
}

void gluSetLight(int handle, LightType type, Math::Color color, Math::Vec3 position, Math::Vec3 direction)
    {
        int GL_LIGHTi = GL_LIGHT0 + handle;
        glEnable(GL_LIGHTi);
        glLightfv(GL_LIGHTi, GL_AMBIENT, g_AmbientLight.vector);
        glLightfv(GL_LIGHTi, GL_DIFFUSE, color.vector);
        switch (type)
        {
        case GLU_LIGHT_DIRECTIONAL:
        {
            Math::Vec4 _direction = Math::Vec4(direction, 0.0F);
            glLightfv(GL_LIGHTi, GL_POSITION, _direction.vector);
        }
        break;
        case GLU_LIGHT_POINT:
        {
            Math::Vec4 _position = Math::Vec4(position, 1.0F);
            glLightfv(GL_LIGHTi, GL_POSITION, _position.vector);
        }
        break;
        }
    }