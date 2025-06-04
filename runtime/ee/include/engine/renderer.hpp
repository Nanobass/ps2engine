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
#include <cstddef>
#include <cstdint>
#include <memory>

/* ps2sdk */
#include <graph.h>

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
#include <engine/lighting.hpp>
#include <engine/camera.hpp>

/* ps2memory */
#include <core/memory.hpp>

/* ps2glu */
#include <ps2glu.hpp>

/* ps2math */
#include <core/math.hpp>
#include <core/log.hpp>

namespace pse
{

struct RenderManager {

    bool mFirstFrame = true;

    int mWidth, mHeight;
    float mAspectRatio;

    std::unique_ptr<TextureManager> mTextureManager;
    std::unique_ptr<LightingManager> mLightingManager;

    /**
     * initialize everything
     */
    RenderManager(size_t immediateVertexBufferSize = 4 * 1024, size_t immediateDmaPacketSize = 128)
    {
        log::out(log::kInfo) << "Initializing PS2GL" << std::endl;
        *(GIF::Registers::ctrl) = 1; // OSDSYS leaves path 3 busy, so fix that
    	SetGsCrt(1 /* interlaced */, 2 /* ntsc */, 1 /* frame */);
        pglInit(immediateVertexBufferSize, immediateDmaPacketSize);

        mWidth = 640;
        mHeight = 224;
        mAspectRatio = ps2::GetSystemAspectRatio();

        log::out(log::kInfo) << "Initializing Buffers: NTSC, Double-Buffered, Interlaced, " << mWidth << "x" << mHeight << ", AspectRatio=" << mAspectRatio << std::endl;
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

        mLightingManager = std::make_unique<LightingManager>();
        mTextureManager = std::make_unique<TextureManager>(240, 512);
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_RESCALE_NORMAL);
    }  

    ~RenderManager()
    {
        mTextureManager.reset();
        mLightingManager.reset();
        pglFinish();
    }

    void BeginFrame() 
    {
        pglBeginGeometry();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
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
    
} // namespace pse
