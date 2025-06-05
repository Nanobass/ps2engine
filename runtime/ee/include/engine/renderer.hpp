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
#include <memory>

//========================================
// Project Includes
//========================================

/* engine */
#include <engine/texturemanager.hpp>
#include <engine/lighting.hpp>
#include <engine/camera.hpp>

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

struct render_manager {

    bool mFirstFrame = true;

    int mWidth, mHeight;
    float mAspectRatio;

    std::unique_ptr<texture_manager> mTextureManager;
    std::unique_ptr<lighting_manager> mLightingManager;

    /**
     * initialize everything
     */
    render_manager(size_t immediateVertexBufferSize = 4 * 1024, size_t immediateDmaPacketSize = 128)
    {
        log::out(log::kInfo) << "initializing ps2gl" << std::endl;
        *(GIF::Registers::ctrl) = 1; // OSDSYS leaves path 3 busy, so fix that
    	SetGsCrt(1 /* interlaced */, 2 /* ntsc */, 1 /* frame */);
        pglInit(immediateVertexBufferSize, immediateDmaPacketSize);

        mWidth = 640;
        mHeight = 224;
        mAspectRatio = ps2::GetSystemAspectRatio();

        log::out(log::kInfo) << "initializing buffers: NTSC, double-buffered, interlaced, " << mWidth << "x" << mHeight << ", aspect ratio=" << mAspectRatio << std::endl;
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

        mLightingManager = std::make_unique<lighting_manager>();
        mTextureManager = std::make_unique<texture_manager>(240, 512);
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_RESCALE_NORMAL);
    }  

    ~render_manager()
    {
        mTextureManager.reset();
        mLightingManager.reset();
        pglFinish();
    }

    void begin_frame() 
    {
        pglBeginGeometry();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    } 

    void end_frame()
    {
        pglEndGeometry();
        if(!mFirstFrame) pglFinishRenderingGeometry(PGL_DONT_FORCE_IMMEDIATE_STOP);
        else mFirstFrame = false;
        pglWaitForVSync();
        pglSwapBuffers();
        pglRenderGeometry();
    }

};
    
} // namespace pse
