//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        main.cpp
//
// Description: Main File
//
//=============================================================================

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

/* ps2memory */
#include <memory/memory.hpp>

/* ps2glu */
#include <ps2glu.hpp>

/* ps2math */
#include <ps2math.hpp>

/* ps2pad */
#include <ps2pad.hpp>

using namespace Input;

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

int main(int argc, char const *argv[])
{
    *(GIF::Registers::ctrl) = 1; // OSDSYS leaves path 3 busy, so fix that
    pglInit(64 * 1024, 1000); // initialize ps2gl, small immediate buffers because we don't use immediate mode, nevermind buffers need to be biiiig

    int mWidth = 640;
    int mHeight = 512;
    graph_set_mode(GRAPH_MODE_INTERLACED, GRAPH_MODE_PAL, GRAPH_MODE_FIELD, GRAPH_ENABLE);
    pgl_slot_handle_t frame_slot_0 = pglAddGsMemSlot(  0, 80, GS_PSM_24);
    pgl_slot_handle_t frame_slot_1 = pglAddGsMemSlot( 80, 80, GS_PSM_24);
    pgl_slot_handle_t depth_slot   = pglAddGsMemSlot(160, 80, GS_PSMZ_24);
    pglLockGsMemSlot(frame_slot_0);
    pglLockGsMemSlot(frame_slot_1);
    pglLockGsMemSlot(depth_slot);
    pgl_area_handle_t frame_area_0 = pglCreateGsMemArea(mWidth, mHeight, GS_PSM_24);
    pgl_area_handle_t frame_area_1 = pglCreateGsMemArea(mWidth, mHeight, GS_PSM_24);
    pgl_area_handle_t depth_area   = pglCreateGsMemArea(mWidth, mHeight, GS_PSMZ_24);
    pglBindGsMemAreaToSlot(frame_area_0, frame_slot_0);
    pglBindGsMemAreaToSlot(frame_area_1, frame_slot_1);
    pglBindGsMemAreaToSlot(depth_area, depth_slot);
    pglSetDrawBuffers(PGL_INTERLACED, frame_area_0, frame_area_1, depth_area);
    pglSetDisplayBuffers(PGL_INTERLACED, frame_area_0, frame_area_1);

    // TODO implement texture pages
    // 8 bit textures mangled into the frame and depth buffers
    // 128x128
    pglAddGsMemSlot(0,    8,   GS::kPsm8h);
    pglAddGsMemSlot(8,    8,   GS::kPsm8h);
    pglAddGsMemSlot(16,   8,   GS::kPsm8h);
    pglAddGsMemSlot(24,   8,   GS::kPsm8h);
    // 256x128
    pglAddGsMemSlot(32,   16,  GS::kPsm8h);
    // 256x256
    pglAddGsMemSlot(48,   32,  GS::kPsm8h);
    pglAddGsMemSlot(80,   32,  GS::kPsm8h);
    // 512x256
    pglAddGsMemSlot(112,  64,  GS::kPsm8h);
    pglAddGsMemSlot(176,  64,  GS::kPsm8h);

    // fill the rest with 32 bit textures
    // 64x32
    pglAddGsMemSlot(240,  1,   GS::kPsm32);
    pglAddGsMemSlot(241,  1,   GS::kPsm32);
    pglAddGsMemSlot(242,  1,   GS::kPsm32);
    pglAddGsMemSlot(243,  1,   GS::kPsm32);
    // 64x64
    pglAddGsMemSlot(244,  2,   GS::kPsm32);
    pglAddGsMemSlot(246,  2,   GS::kPsm32);
    pglAddGsMemSlot(248,  2,   GS::kPsm32);
    pglAddGsMemSlot(250,  2,   GS::kPsm32);
    pglAddGsMemSlot(252,  2,   GS::kPsm32);
    pglAddGsMemSlot(254,  2,   GS::kPsm32);
    // 128x128
    pglAddGsMemSlot(256,  8,   GS::kPsm32);
    pglAddGsMemSlot(264,  8,   GS::kPsm32);
    pglAddGsMemSlot(272,  8,   GS::kPsm32);
    pglAddGsMemSlot(280,  8,   GS::kPsm32);
    pglAddGsMemSlot(288,  8,   GS::kPsm32);
    pglAddGsMemSlot(296,  8,   GS::kPsm32);
    pglAddGsMemSlot(304,  8,   GS::kPsm32);
    pglAddGsMemSlot(312,  8,   GS::kPsm32);
    // 256x256
    pglAddGsMemSlot(320,  32,  GS::kPsm32);
    pglAddGsMemSlot(352,  32,  GS::kPsm32);
    // 512x256
    pglAddGsMemSlot(384,  64,  GS::kPsm32);
    pglAddGsMemSlot(448,  64,  GS::kPsm32);
    
    pglPrintGsMemAllocation();
    float mAspectRatio = ps2::GetSystemAspectRatio();

    SifLoadModule("host0:/irx/sio2man.irx", 0, NULL);
    SifLoadModule("host0:/irx/padman.irx", 0, NULL);
    padInit(0);
    Pad::PadDevice pad;
    pad.Open(0, 0);

    bool mFirstFrame = true;

    gluClearColor(Math::Color(0.1F, 0.1F, 0.1F));
    glClearDepth(1.0F);
    gluAmbientLight(Math::Color(1.0F, 1.0F, 1.0F));
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_RESCALE_NORMAL);
    glDisable(GL_CULL_FACE);
    glViewport(0, 0, 640, 512);
    gluPerspective(70.0F, 1.0F, 4000.0F, mAspectRatio);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);

    Math::Vec4 cameraRotation(0, 0, 0);
    Math::Camera camera;
    camera.mPosition = Math::Vec3(0, 0, 1.5F);
    camera.mLookAt = Math::Vec3(0, 0, 0);
    camera.mUp = Math::Vec3(0, 1, 0);

    do {
        pad.Poll();

        cameraRotation.x += -pad.GetInput(Pad::RightY).mInputValue * 0.02f * 2;
        cameraRotation.y += -pad.GetInput(Pad::RightX).mInputValue * 0.02f * 2;

        // Move Camera
        Math::Mat4 matrix = Math::RotationY(cameraRotation.y) * Math::RotationX(cameraRotation.x);
        camera.mPosition += matrix * Math::Vec4(0, 0, -1) * -pad.GetInput(Pad::LeftY).mInputValue * 0.02f * 15;
        camera.mPosition += matrix * Math::Vec4(1, 0, 0) * pad.GetInput(Pad::LeftX).mInputValue * 0.02f * 15;
        camera.mPosition += matrix * Math::Vec4(0, 1, 0) * (-pad.GetInput(Pad::L2).mInputValue + pad.GetInput(Pad::R2).mInputValue) * 0.02f * 15;
        camera.mLookAt = camera.mPosition + matrix * Math::Vec4(0, 0, -1);

        pglBeginGeometry();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gluLoadMatrix(camera.GetViewMatrix());
        gluSetLight(0, GLU_LIGHT_DIRECTIONAL, Math::Color(1.0F, 1.0F, 1.0F), Math::Vec3(0, 0, 0), Math::Vec3(0, 0, 1));
        
        glBegin(GL_QUADS);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 1, 0);
        glVertex3f(1, 1, 0);
        glVertex3f(1, 0, 0);
        glEnd();

        pglEndGeometry();
        if(!mFirstFrame) pglFinishRenderingGeometry(PGL_DONT_FORCE_IMMEDIATE_STOP);
        else mFirstFrame = false;
        pglWaitForVSync();
        pglSwapBuffers();
        pglRenderGeometry();
    } while (1);

    pglFinish();

    return 0;
}
