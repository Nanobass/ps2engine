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

#include <ps2gl/metrics.h>

/* ps2stuff */
#include <ps2s/gs.h>

//========================================
// Project Includes
//========================================

/* engine */
#include <engine/renderer.hpp>
#include <engine/renderer/fragment.hpp>

/* ps2memory */
#include <ps2memory.hpp>

/* ps2glu */
#include <ps2glu.hpp>

/* ps2math */
#include <ps2math.hpp>

/* ps2pad */
#include <ps2pad.hpp>

using namespace Input;

int main(int argc, char const *argv[])
{
    SifLoadModule("host0:/irx/sio2man.irx", 0, NULL);
    SifLoadModule("host0:/irx/padman.irx", 0, NULL);
    padInit(0);
    Pad::PadDevice pad;
    pad.Open(0, 0);

    bool bCloseRequested = false;

    std::unique_ptr<Engine::MasterRenderer> g_MasterRenderer = std::make_unique<Engine::MasterRenderer>();
    Engine::PerspectiveCamera camera(40.0F, 1.0F, 100.0F, g_MasterRenderer->mAspectRatio);
    Engine::OrthographicCamera hud(640.0F, 480.0F, g_MasterRenderer->mAspectRatio);

    Engine::Font* font = g_MasterRenderer->mTextRenderer->LoadFont(0, "emotion.fnt", "emotion.gs", 32.0F, 39.0F);
    g_MasterRenderer->mSkyboxRenderer->LoadSkybox("sky.gs");

    Math::Vec4 cameraRotation(0, 0, 0);
    camera.mPosition = Math::Vec3(0, 0, 5);
    camera.mTarget = Math::Vec3(0, 0, 0);

    std::string metrics = "";
    int frameCounter = 0;

    Engine::FragmentRenderer fragmentRenderer;
    fragmentRenderer.mGeometryAllocator = g_MasterRenderer->mGeometryAllocator.get();
    fragmentRenderer.mTextureManager = g_MasterRenderer->mTextureManager.get();

    Engine::Texture* kekw = g_MasterRenderer->mTextureManager->LoadGsTexture("kekw", "kekw.gs");
    Engine::Material* material = fragmentRenderer.CreateMaterial("default", color(0.1F, 0.1F, 0.1F));
    Engine::Geometry* geom = fragmentRenderer.CreateGeometry("quad", GL_QUADS, true, 1);
    
    Engine::GeometryFragment& fragment = fragmentRenderer.AddFragment(geom, 0, 4);

    fragment.mVertices[0] = vec3(-0.5F, -0.5F, 0.0F);
    fragment.mTextureCoordinates[0] = vec2(0.0F, 1.0F);
    fragment.mNormals[0] = vec3(0.0F, 0.0F, 1.0F);
    fragment.mColors[0] = vec4(1.0F, 0.0F, 0.0F);

    fragment.mVertices[1] = vec3(-0.5F, 0.5F, 0.0F);
    fragment.mTextureCoordinates[1] = vec2(0.0F, 0.0F);
    fragment.mNormals[1] = vec3(0.0F, 0.0F, 1.0F);
    fragment.mColors[1] = vec4(0.0F, 1.0F, 0.0F);

    fragment.mVertices[2] = vec3(0.5F, 0.5F, 0.0F);
    fragment.mTextureCoordinates[2] = vec2(1.0F, 0.0F);
    fragment.mNormals[2] = vec3(0.0F, 0.0F, 1.0F);
    fragment.mColors[2] = vec4(0.0F, 0.0F, 1.0F);

    fragment.mVertices[3] = vec3(0.5F, -0.5F, 0.0F);
    fragment.mTextureCoordinates[3] = vec2(1.0F, 1.0F);
    fragment.mNormals[3] = vec3(0.0F, 0.0F, 1.0F);
    fragment.mColors[3] = vec4(1.0F, 1.0F, 1.0F);
    
    do {
        pad.Poll();
        
        if(pad.InputChanged(Pad::Square) && pad.GetInput(Pad::Square).mPressed) pglPrintGsMemAllocation();
        if(pad.InputChanged(Pad::Circle) && pad.GetInput(Pad::Circle).mPressed) bCloseRequested = true;

        cameraRotation.x += -pad.GetInput(Pad::RightY).mInputValue * 0.02f * 2;
        cameraRotation.y += -pad.GetInput(Pad::RightX).mInputValue * 0.02f * 2;
        
        Math::Mat4 matrix = Math::RotationY(cameraRotation.y) * Math::RotationX(cameraRotation.x);
        camera.mPosition += matrix * Math::Vec4(0, 0, -1) * -pad.GetInput(Pad::LeftY).mInputValue * 0.02f * 15;
        camera.mPosition += matrix * Math::Vec4(1, 0, 0) * pad.GetInput(Pad::LeftX).mInputValue * 0.02f * 15;
        camera.mPosition += matrix * Math::Vec4(0, 1, 0) * (-pad.GetInput(Pad::L2).mInputValue + pad.GetInput(Pad::R2).mInputValue) * 0.02f * 15;
        camera.mTarget = camera.mPosition + matrix * Math::Vec4(0, 0, -1);

        if(frameCounter <= 0)
        {
            std::stringstream ss;
            ss << "Metrics per 30 frames" << std::endl;
            ss << "  " << "Renderer Uploads: " << pglGetMetric(kMetricsRendererUpload) << std::endl;
            ss << "  " << "Texture Uploads: " << pglGetMetric(kMetricsTextureUploadCount) << std::endl;
            ss << "  " << "Texture Binds: " << pglGetMetric(kMetricsBindTexture) << std::endl;
            ss << "  " << "CLUT Uploads: " << pglGetMetric(kMetricsClutUploadCount) << std::endl;
            metrics = ss.str();
            pglResetMetrics();
            frameCounter = 30;
        }
        frameCounter--;

        g_MasterRenderer->BeginFrame();
        g_MasterRenderer->mSkyboxRenderer->RenderSky(camera);
        g_MasterRenderer->mSkyboxRenderer->DoLighting();

        material->Bind();
        kekw->Bind();
        geom->Draw();

        hud.Apply();
        glTranslatef(-hud.mScreenWidth / 2.0F, hud.mScreenHeight / 2.0F, 0.0F);
        glScalef(24.0F, 24.0F, 1.0F);
        glTranslatef(0.0F, -1.0F, 0.0F);
        g_MasterRenderer->mTextRenderer->DrawString(font, metrics);

        g_MasterRenderer->EndFrame();
    } while (!bCloseRequested);

    g_MasterRenderer.reset();

    return 0;
}
