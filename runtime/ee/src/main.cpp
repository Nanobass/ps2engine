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
#include <sstream>
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
#include <GL/ps2glu.hpp>

/* ps2stuff */
#include <ps2s/gs.h>

//========================================
// Project Includes
//========================================

#include <core/log.hpp>
#include <core/math.hpp>

#include <engine/memory/memory.hpp>
#include <engine/memory/metrics.hpp>

#include <engine/renderer.hpp>
#include <engine/renderer/font.hpp>
#include <engine/renderer/skybox.hpp>
#include <engine/renderer/basic.hpp>

#include <engine/scene.hpp>
#include <engine/gameobject.hpp>
#include <engine/components.hpp>

#include <ps2pad.hpp>

std::unique_ptr<pse::render_manager> g_RenderManager = nullptr;
std::unique_ptr<pse::text_renderer> g_TextRenderer = nullptr;
std::unique_ptr<pse::skybox_renderer> g_SkyboxRenderer = nullptr;
std::unique_ptr<pse::BasicRenderer> g_BasicRenderer = nullptr;
std::unique_ptr<pse::orthographic_camera> g_HudCamera = nullptr;
pse::font* g_DefaultFont = nullptr;
pse::scene* g_Scene = nullptr;

int main(int argc, char const *argv[]) 
{
    pse::initialize_memory_system();
    pse::memory::set_tracking(true, 0, "initialiation");

    SifLoadModule("host0:/irx/sio2man.irx", 0, NULL);
    SifLoadModule("host0:/irx/padman.irx", 0, NULL);
    padInit(0);
    Input::Pad::PadDevice pad;
    pad.Open(0, 0);

    bool bCloseRequested = false;

    g_RenderManager = std::make_unique<pse::render_manager>();
    g_TextRenderer = std::make_unique<pse::text_renderer>(g_RenderManager->mTextureManager.get());
    g_SkyboxRenderer = std::make_unique<pse::skybox_renderer>(g_RenderManager->mTextureManager.get(), g_RenderManager->mLightingManager.get());
    g_BasicRenderer = std::make_unique<pse::BasicRenderer>(g_RenderManager->mTextureManager.get());
    g_HudCamera = std::make_unique<pse::orthographic_camera>(640.0F, 480.0F, g_RenderManager->mAspectRatio);
    g_DefaultFont = g_TextRenderer->load_font("emotion", "emotion.fnt", "emotion.gs", 32.0F, 39.0F);

    g_Scene = new(pse::GME_OBJECT) pse::scene();
    pse::game_object gmCamera = g_Scene->create_entity("Camera");
    pse::game_object gmModel = g_Scene->create_entity("Model");
    for(int i = 0; i < 32; i++)
    {
        g_Scene->create_entity("GameObject");
    }
    
    g_SkyboxRenderer->load_skybox("sky.gs");
    g_SkyboxRenderer->mSunLight->mDiffuse = pse::math::color(1.0F, 1.0F, 1.0F);
    g_SkyboxRenderer->mSunLight->mAmbient = pse::math::color(0.05F, 0.05F, 0.05F);

    g_RenderManager->mLightingManager->set_lighting_mode(pse::lighting_mode::kLighting);

    pse::perspective_camera camera(40.0F, 1.0F, 100.0F, g_RenderManager->mAspectRatio);
    pse::math::vec4 cameraRotation(0, 0, 0);
    camera.mPosition = pse::math::vec4(0, 0, 5);
    camera.mTarget = pse::math::vec4(0, 0, 0);

    std::string metrics = "";
    int frameCounter = 0;

    pse::material * material = new pse::material();
    g_RenderManager->mLightingManager->set_material(material);

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

        g_RenderManager->begin_frame();
        g_SkyboxRenderer->render_sky(camera);
        g_RenderManager->mLightingManager->do_lighting(camera);

        g_HudCamera->apply();
        glPushMatrix();
        glTranslatef(-g_HudCamera->mScreenWidth / 2.0F, g_HudCamera->mScreenHeight / 2.0F, 0.0F);
        glScalef(24.0F, 24.0F, 1.0F);
        glTranslatef(0.0F, -1.0F, 0.0F);
        g_TextRenderer->draw_string(g_DefaultFont, metrics);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-g_HudCamera->mScreenWidth / 2.0F, -g_HudCamera->mScreenHeight / 2.0F, 0.0F);
        glScalef(24.0F, 24.0F, 1.0F);
        glTranslatef(0.0F, 1.0F, 0.0F);
        g_TextRenderer->draw_string(g_DefaultFont, debug_info.str());
        glPopMatrix();
 
        g_RenderManager->end_frame();
    } while (!bCloseRequested);

    g_BasicRenderer.reset();
    g_SkyboxRenderer.reset();
    g_TextRenderer.reset();
    g_RenderManager.reset();

    return 0;
}