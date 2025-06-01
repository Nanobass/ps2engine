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
#include <chrono>

/* ps2sdk */
#include <debug.h>
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

#include <engine/memory/memory.hpp>
#include <engine/memory/metrics.hpp>

#include <ps2glu.hpp>
#include <ps2math.hpp>
#include <ps2pad.hpp>

#include <singletons.hpp>

#include <terrain.hpp>
#include <noise.h>

int main(int argc, char const *argv[]) 
{
    pse::initialize_memory_system();
    pse::memory::set_tracking(true, 0);

    SifLoadModule("host0:/irx/sio2man.irx", 0, NULL);
    SifLoadModule("host0:/irx/padman.irx", 0, NULL);
    padInit(0);
    Input::Pad::PadDevice pad;
    pad.Open(0, 0);

    bool bCloseRequested = false;

    g_RenderManager = std::make_unique<pse::RenderManager>();
    g_TextRenderer = std::make_unique<pse::TextRenderer>(g_RenderManager->mTextureManager.get());
    g_SkyboxRenderer = std::make_unique<pse::SkyboxRenderer>(g_RenderManager->mTextureManager.get(), g_RenderManager->mLightingManager.get());
    g_BasicRenderer = std::make_unique<pse::BasicRenderer>(g_RenderManager->mTextureManager.get());
    g_HudCamera = std::make_unique<pse::OrthographicCamera>(640.0F, 480.0F, g_RenderManager->mAspectRatio);
    g_DefaultFont = g_TextRenderer->LoadFont("emotion", "emotion.fnt", "emotion.gs", 32.0F, 39.0F);
    
    g_SkyboxRenderer->LoadSkybox("sky.gs");
    g_SkyboxRenderer->mSunLight->mDiffuse = pse::math::color(1.0F, 1.0F, 1.0F);
    g_SkyboxRenderer->mSunLight->mAmbient = pse::math::color(0.05F, 0.05F, 0.05F);

    g_RenderManager->mLightingManager->SetLightingMode(pse::LightingMode::Lighting);

    pse::PerspectiveCamera camera(40.0F, 1.0F, 100.0F, g_RenderManager->mAspectRatio);
    pse::math::vec4 cameraRotation(0, 0, 0);
    camera.mPosition = pse::math::vec3(0, 0, 5);
    camera.mTarget = pse::math::vec3(0, 0, 0);

    std::string metrics = "";
    int frameCounter = 0;

    pse::Material* material = new pse::Material();
    g_RenderManager->mLightingManager->SetMaterial(material);

    pse::memory::set_tracking(true, 0, "Terrain Generation");

    CTerrain* terrain = new CTerrain();

    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            for(int k = 0; k < 4; k++)
            {
                makeChunk(terrain, i, j, k);
            }
        }
    }

    pse::memory::set_tracking(true, 0, "Terrain Mesh Generation");

    for(auto& chunk : terrain->chunks())
    {
        chunk.GenerateMesh();
    }

    pse::memory::set_tracking(false);
    do {
        pad.Poll();
        
        if(pad.InputChanged(Input::Pad::Square) && pad.GetInput(Input::Pad::Square).mPressed) pglPrintGsMemAllocation();
        if(pad.InputChanged(Input::Pad::Circle) && pad.GetInput(Input::Pad::Circle).mPressed) bCloseRequested = true;
        if(pad.InputChanged(Input::Pad::Triangle) && pad.GetInput(Input::Pad::Triangle).mPressed) std::terminate();
        if(pad.InputChanged(Input::Pad::Cross) && pad.GetInput(Input::Pad::Cross).mPressed) pse::memory::print_statistics();

        cameraRotation.x += -pad.GetInput(Input::Pad::RightY).mInputValue * 0.02f * 2;
        cameraRotation.y += -pad.GetInput(Input::Pad::RightX).mInputValue * 0.02f * 2;

        pse::math::mat4 matrix = pse::math::rotationY(cameraRotation.y) * pse::math::rotationX(cameraRotation.x);
        camera.mPosition += matrix * pse::math::vec4(0, 0, -1) * -pad.GetInput(Input::Pad::LeftY).mInputValue * 0.02f * 15;
        camera.mPosition += matrix * pse::math::vec4(1, 0, 0) * pad.GetInput(Input::Pad::LeftX).mInputValue * 0.02f * 15;
        camera.mPosition += matrix * pse::math::vec4(0, 1, 0) * (-pad.GetInput(Input::Pad::L2).mInputValue + pad.GetInput(Input::Pad::R2).mInputValue) * 0.02f * 15;
        camera.mTarget = camera.mPosition + matrix * pse::math::vec4(0, 0, -1);

        if(frameCounter <= 0)
        {
            metrics = pse::metrics::generate_metrics();
            frameCounter = 30;
        }
        frameCounter--;

        std::stringstream debug_info;
        debug_info << "Camera Position: " << camera.mPosition << std::endl;
        debug_info << "Camera Rotation: " << cameraRotation << std::endl;

        g_RenderManager->BeginFrame();
        g_SkyboxRenderer->RenderSky(camera);
        g_RenderManager->mLightingManager->DoLighting(camera);
        
        camera.Apply();
        for(auto& chunk : terrain->chunks())
        {
            glCallList(chunk.mesh().mList);
        }

        g_HudCamera->Apply();
        glPushMatrix();
        glTranslatef(-g_HudCamera->mScreenWidth / 2.0F, g_HudCamera->mScreenHeight / 2.0F, 0.0F);
        glScalef(24.0F, 24.0F, 1.0F);
        glTranslatef(0.0F, -1.0F, 0.0F);
        g_TextRenderer->DrawString(g_DefaultFont, metrics);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-g_HudCamera->mScreenWidth / 2.0F, -g_HudCamera->mScreenHeight / 2.0F, 0.0F);
        glScalef(24.0F, 24.0F, 1.0F);
        glTranslatef(0.0F, 1.0F, 0.0F);
        g_TextRenderer->DrawString(g_DefaultFont, debug_info.str());
        glPopMatrix();
 
        g_RenderManager->EndFrame();
    } while (!bCloseRequested);

    g_BasicRenderer.reset();
    g_SkyboxRenderer.reset();
    g_TextRenderer.reset();
    g_RenderManager.reset();

    return 0;
}