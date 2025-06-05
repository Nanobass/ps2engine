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

struct skybox_renderer
{
    texture_manager* mTextureManager;
    lighting_manager* mLightingManager;

    texture* mSkyTexture = nullptr;
    light* mSunLight = nullptr;

    GLuint mList = 0;

    skybox_renderer(texture_manager* textureManager, lighting_manager* lightingManager) 
        :   mTextureManager(textureManager)
        ,   mLightingManager(lightingManager)
    {
        mSunLight = mLightingManager->allocate_light(true);
        mSunLight->mPosition = pse::math::vec4(1.0F, 1.0F, 1.0F, 0.0F);
        mSunLight->mAmbient = pse::math::color(0.0F, 0.0F, 0.0F, 1.0F);
        mSunLight->mDiffuse = pse::math::color(1.0F, 1.0F, 1.0F, 1.0F);
        mSunLight->mSpecular = pse::math::color(1.0F, 1.0F, 1.0F, 1.0F);
        mSunLight->set_enabled(true);
    }

    ~skybox_renderer()
    {
        mSunLight->free(true);
        if(mList || mSkyTexture) unload_skybox();
    }

    void load_skybox(const std::string& img, float size = 16.0F, int divisions = 16);

    void unload_skybox();

    void render_sky(perspective_camera& camera, bool render = true);

};
    
} // namespace pse
