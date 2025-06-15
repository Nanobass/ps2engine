//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        camera.hpp
//
// Description: Cameras
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

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

namespace pse
{

class camera {
public:
    camera() = default;
    camera(const math::mat4& projection)
        : mProjectionMatrix(projection) {}
    virtual ~camera() = default;

    virtual const math::mat4& apply_projection() = 0;
    const math::mat4& get_projection() const { return mProjectionMatrix; }
protected:
		math::mat4 mProjectionMatrix = math::mat4();
};

class scene_camera : camera {
public:
    enum projection_mode { kOrthographic, kPerspective };

    float mScreenWidth, mScreenHeight;
    float mAspectRatio;

    float mWorldWidth = 640.0F, mWorldHeight = 480.0F;
    float mNearPlane = 1.0F, mFarPlane = 100.0F;
    float mFieldOfView = 40.0F;
    
    projection_mode mMode = kPerspective;

    scene_camera() = default;
    scene_camera(const scene_camera&) = default;

    void set_viewport(float width, float height, float aspect) 
    {
        mScreenWidth = width;
        mScreenWidth = height;
        mAspectRatio = aspect;
    }

    void set_perspective(float fov, float near, float far)
    {
        mFieldOfView = fov;
        mNearPlane = near;
        mFarPlane = far;
        set_projection_mode(kPerspective);
    }

    void set_orthographic(float worldWidth, float worldHeight, float near, float far)
    {
        mWorldWidth = worldWidth;
        mWorldHeight = worldHeight;
        mNearPlane = near;
        mFarPlane = far;
        set_projection_mode(kOrthographic);
    }
    
    void set_projection_mode(projection_mode mode)
    {
        mMode = mode;
        apply_projection();
    }

    static void gluPerspective(float fov, float near, float far, float aspect) 
    {
        float h = 2.0f * near * (float)tanf(fov * 3.141592654f / 180.0f / 2.0f);
        float w = h * aspect;
        glFrustum(-w / 2.0f, w / 2.0f, -h / 2.0f, h / 2.0f, near, far);
    }

    const math::mat4& apply_projection()
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        switch(mMode)
        {
            case kOrthographic: glOrtho(-mWorldWidth / 2.0F, mWorldWidth / 2.0F, -mWorldHeight / 2.0F, mWorldHeight / 2.0F, -1.0F, 1.0F); break;
            case kPerspective: gluPerspective(mFieldOfView, mNearPlane, mFarPlane, mAspectRatio); break;
        }
        glGetFloatv(GL_PROJECTION_MATRIX, mProjectionMatrix.matrix);
        return mProjectionMatrix;
    }

};
    
} // namespace pse
