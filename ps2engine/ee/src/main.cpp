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

/* engine */
#include <engine/texturemanager.hpp>
#include <engine/renderer.hpp>

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
    Engine::PerspectiveCamera camera(70.0F, 1.0F, 4000.0F, g_MasterRenderer->mAspectRatio);
    Engine::OrthographicCamera hud(640.0F, 480.0F, g_MasterRenderer->mAspectRatio);

    std::unique_ptr<Engine::TextRenderer> g_TextRenderer = std::make_unique<Engine::TextRenderer>(g_MasterRenderer->mTextureManager.get());
    Engine::Font* font = g_TextRenderer->LoadFont(0, "emotion.fnt", "emotion.gs", 32.0F, 39.0F);

    Math::Vec4 cameraRotation(0, 0, 0);
    camera.mPosition = Math::Vec3(0, 0, 1.5F);
    camera.mTarget = Math::Vec3(0, 0, 0);

    Engine::Texture* tex = g_MasterRenderer->mTextureManager->LoadGsTexture(1, "ps2lua_banner.gs");

    gluSetLight(0, GLU_LIGHT_DIRECTIONAL, Math::Color(1.0F, 1.0F, 1.0F), Math::Vec3(0, 0, 0), Math::Vec3(0, 0, 1));

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

        g_MasterRenderer->BeginFrame();
        
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);

        camera.Apply();
        glBindTexture(GL_TEXTURE_2D, tex->mGLName);
        glBegin(GL_QUADS);
        {
            glTexCoord2f(0, 1);
            glVertex3f(0, 0, 0);

            glTexCoord2f(0, 0);
            glVertex3f(0, 1, 0);

            glTexCoord2f(1, 0);
            glVertex3f(1, 1, 0);

            glTexCoord2f(1, 1);
            glVertex3f(1, 0, 0);
        }
        glEnd();

        hud.Apply();
        glTranslatef(-hud.mScreenWidth / 2.0F, hud.mScreenHeight / 2.0F, 0.0F);
        glScalef(72.0F, 72.0F, 1.0F);
        glTranslatef(0.0F,  -font->mLineHeight, 0.0F);
        g_TextRenderer->DrawString(font, "/usr/local/ps2dev/ee/lib/gcc/mips64r5900el-ps2-elf/11.3.0/../../../../mips64r5900el-ps2-elf/bin/ld: /home/nanobass/ps2/ps2engine/ps2gl//libps2gl.a(scei.vo): warning: linking abicalls files with non-abicalls files");

        g_MasterRenderer->EndFrame();
    } while (!bCloseRequested);

    g_TextRenderer.reset();
    g_MasterRenderer.reset();

    return 0;
}
