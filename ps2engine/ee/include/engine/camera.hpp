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

/* standard library */
#include <cstddef>
#include <cstdint>

/* ps2gl */
#include <GL/ps2gl.h>
#include <GL/gl.h>

/* ps2stuff */
#include <ps2s/gs.h>

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

struct Camera {
    Math::Mat4 mProjectionMatrix;
    Math::Mat4 mViewMatrix;

    virtual void Apply() 
    {
        glMatrixMode(GL_PROJECTION);
        gluLoadMatrix(mProjectionMatrix);
        glMatrixMode(GL_MODELVIEW);
        gluLoadMatrix(mViewMatrix);
    }
};

struct PerspectiveCamera : public Camera {
    Math::Vec4 mPosition, mTarget, mUp = Math::Vec4(0.0F, 1.0F, 0.0F);

    float mNearPlane, mFarPlane;
    float mFieldOfView;
    float mAspectRatio;

    PerspectiveCamera(float fov, float near, float far, float aspect)
        :   mNearPlane(near)
        ,   mFarPlane(far)
        ,   mFieldOfView(fov)
        ,   mAspectRatio(aspect)
    {}

    void Apply() 
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

struct OrthographicCamera : public Camera {
    Math::Vec4 mPosition;

    float mScreenWidth, mScreenHeight;
    float mWorldWidth, mWorldHeight;
    float mAspectRatio;

    OrthographicCamera(float width, float height, float aspect)
        :   mWorldWidth(width)
        ,   mWorldHeight(height)
        ,   mAspectRatio(aspect)
    {}

    void Apply() 
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
    
} // namespace Engine
