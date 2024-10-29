//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        renderer.hpp
//
// Description: Master Renderer
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
#include <fstream>
#include <utility>
#include <memory>
#include <string.h>
#include <iostream>
#include <algorithm>

/* ps2sdk */
#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <stdio.h>
#include <graph.h>
#include <gs_psm.h>
#include <osd_config.h>

/* ps2gl */
#include <GL/ps2gl.h>
#include <GL/gl.h>

/* ps2stuff */
#include <ps2s/gs.h>

//========================================
// Project Includes
//========================================

/* engine */
#include <engine/texturemanager.hpp>
#include <engine/textrenderer.hpp>

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
        mProjectionMatrix = Math::Projection(mFieldOfView, 1.0F, 1.0F, 1.0F, mAspectRatio, mNearPlane, mFarPlane);
        mViewMatrix = Math::LookAt(mPosition, mTarget, mUp);
        Camera::Apply();
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
        Math::Vec4 scale = Math::Vec4(1.0F / mScreenWidth * 2.0F, 1.0F / mScreenHeight * 2.0F, 1.0F);
        mProjectionMatrix = Math::Scale(scale);
        mViewMatrix = Math::Mat4();
        Camera::Apply();
    }
};

struct MasterRenderer {

    std::unique_ptr<TextureManager> mTextureManager;
    int mWidth, mHeight;
    float mAspectRatio;

    bool mFirstFrame = true;

    /**
     * initialize everything
     */
    MasterRenderer()
    {
        *(GIF::Registers::ctrl) = 1; // OSDSYS leaves path 3 busy, so fix that
        pglInit(64 * 1024, 1000); // initialize ps2gl, small immediate buffers because we don't use immediate mode, nevermind buffers need to be biiiig

        mWidth = 640;
        mHeight = 512;
        mAspectRatio = ps2::GetSystemAspectRatio();

        graph_set_mode(GRAPH_MODE_INTERLACED, GRAPH_MODE_PAL, GRAPH_MODE_FIELD, GRAPH_ENABLE);

        pgl_slot_handle_t frame_slot_0 = pglAddGsMemSlot(  0, 80, GS_PSM_24);
        pgl_area_handle_t frame_area_0 = pglCreateGsMemArea(mWidth, mHeight, GS_PSM_24);
        pglLockGsMemSlot(frame_slot_0);
        pglBindGsMemAreaToSlot(frame_area_0, frame_slot_0);

        pgl_slot_handle_t frame_slot_1 = pglAddGsMemSlot( 80, 80, GS_PSM_24);
        pgl_area_handle_t frame_area_1 = pglCreateGsMemArea(mWidth, mHeight, GS_PSM_24);
        pglLockGsMemSlot(frame_slot_1);
        pglBindGsMemAreaToSlot(frame_area_1, frame_slot_1);

        pgl_slot_handle_t depth_slot   = pglAddGsMemSlot(160, 80, GS_PSMZ_24);
        pgl_area_handle_t depth_area   = pglCreateGsMemArea(mWidth, mHeight, GS_PSMZ_24);
        pglLockGsMemSlot(depth_slot);
        pglBindGsMemAreaToSlot(depth_area, depth_slot);

        pglSetDrawBuffers(PGL_INTERLACED, frame_area_0, frame_area_1, depth_area);
        pglSetDisplayBuffers(PGL_INTERLACED, frame_area_0, frame_area_1);

        mTextureManager = std::make_unique<TextureManager>(240, 512);

        gluClearColor(Math::Color(0.1F, 0.1F, 0.1F));
        glClearDepth(1.0F);
        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glDisable(GL_CULL_FACE);

        glEnable(GL_RESCALE_NORMAL);

        glEnable(GL_LIGHTING);
        gluAmbientLight(Math::Color(1.0F, 1.0F, 1.0F));

        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
    }  

    ~MasterRenderer()
    {
        // delete the texture manager first
        mTextureManager.reset();
        
        pglFinish();
    }

    void BeginFrame() 
    {
        pglBeginGeometry();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } 

    void EndFrame()
    {
        pglEndGeometry();
        if(!mFirstFrame) pglFinishRenderingGeometry(PGL_DONT_FORCE_IMMEDIATE_STOP);
        else mFirstFrame = false;
        pglWaitForVSync();
        pglSwapBuffers();
        pglRenderGeometry();
    }

};
    
} // namespace Engine
