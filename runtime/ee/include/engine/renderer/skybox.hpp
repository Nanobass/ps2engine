//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        skybox.hpp
//
// Description: Skybox Renderer
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
#include <exception>
#include <sstream>
#include <regex>

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
#include <engine/lighting.hpp>
#include <engine/camera.hpp>

/* ps2memory */
#include <core/memory.hpp>

/* ps2glu */
#include <ps2glu.hpp>

/* ps2math */
#include <core/math.hpp>

namespace pse
{

struct SkyboxRenderer
{
    TextureManager* mTextureManager;
    LightingManager* mLightingManager;

    Texture* mSkyTexture = nullptr;
    Light* mSunLight = nullptr;

    GLuint mList = 0;

    SkyboxRenderer(TextureManager* textureManager, LightingManager* lightingManager) 
        :   mTextureManager(textureManager)
        ,   mLightingManager(lightingManager)
    {
        mSunLight = mLightingManager->AllocateLight(true);
        mSunLight->mPosition = pse::math::vec4(1.0F, 1.0F, 1.0F, 0.0F);
        mSunLight->mAmbient = pse::math::color(0.0F, 0.0F, 0.0F, 1.0F);
        mSunLight->mDiffuse = pse::math::color(1.0F, 1.0F, 1.0F, 1.0F);
        mSunLight->mSpecular = pse::math::color(1.0F, 1.0F, 1.0F, 1.0F);
        mSunLight->SetEnabled(true);
    }

    ~SkyboxRenderer()
    {
        mSunLight->Free(true);
        if(mList || mSkyTexture) UnloadSkybox();
    }

    void LoadSkybox(const std::string& img, float size = 16.0F, int divisions = 16);

    void UnloadSkybox();

    void RenderSky(PerspectiveCamera& camera, bool render = true);

};
    
} // namespace pse
