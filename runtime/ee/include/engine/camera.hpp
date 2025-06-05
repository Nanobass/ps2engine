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
#include <GL/ps2glu.hpp>

namespace pse
{

struct camera {
    pse::math::mat4 mProjectionMatrix;
    pse::math::mat4 mViewMatrix;

    virtual void apply() 
    {
        glMatrixMode(GL_PROJECTION);
        gluLoadMatrix(mProjectionMatrix);
        glMatrixMode(GL_MODELVIEW);
        gluLoadMatrix(mViewMatrix);
    }
};

struct perspective_camera : public camera {
    pse::math::vec4 mPosition, mTarget, mUp = pse::math::vec4(0.0F, 1.0F, 0.0F);

    float mNearPlane, mFarPlane;
    float mFieldOfView;
    float mAspectRatio;

    perspective_camera(float fov, float near, float far, float aspect)
        :   mNearPlane(near)
        ,   mFarPlane(far)
        ,   mFieldOfView(fov)
        ,   mAspectRatio(aspect)
    {}

    void apply() 
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(mFieldOfView, mNearPlane, mFarPlane, mAspectRatio);
        glGetFloatv(GL_PROJECTION_MATRIX, mProjectionMatrix.matrix);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(mPosition, mTarget, mUp);
        glGetFloatv(GL_MODELVIEW_MATRIX, mViewMatrix.matrix);
    }
};

struct orthographic_camera : public camera {
    pse::math::vec4 mPosition;

    float mScreenWidth, mScreenHeight;
    float mWorldWidth, mWorldHeight;
    float mAspectRatio;

    orthographic_camera(float width, float height, float aspect)
        :   mWorldWidth(width)
        ,   mWorldHeight(height)
        ,   mAspectRatio(aspect)
    {}

    void apply() 
    {
        mScreenWidth = mWorldWidth * mAspectRatio;
        mScreenHeight = mWorldHeight;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-mScreenWidth / 2.0F, mScreenWidth / 2.0F, -mScreenHeight / 2.0F, mScreenHeight / 2.0F, -1.0F, 1.0F);
        glGetFloatv(GL_PROJECTION_MATRIX, mProjectionMatrix.matrix);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glGetFloatv(GL_MODELVIEW_MATRIX, mViewMatrix.matrix);
    }
};
    
} // namespace pse
