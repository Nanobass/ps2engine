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
#include <engine/lightingmanager.hpp>
#include <engine/camera.hpp>

/* core */
#include <core/log.hpp>
#include <core/math.hpp>
#include <core/memory.hpp>

/* ps2gl */
#include <GL/gl.h>
#include <GL/ps2gl.h>

namespace pse
{

struct skybox_renderer
{
    texture_ptr mSkyTexture = nullptr;
    GLuint mDList = 0;

    skybox_renderer()
    {
        load_skybox();
        log::out(log::kInfo) << "skybox renderer initialized" << std::endl;
    }

    ~skybox_renderer()
    {
        if(mDList) glDeleteLists(mDList, 1);
        log::out(log::kInfo) << "skybox renderer terminated" << std::endl;
    }

    void set_sky_texture(const texture_ptr& texture);

    void load_skybox(float size = 16.0F, int divisions = 16);

    void unload_skybox();

    void render_sky(const math::vec3& cameraPosition);

};
    
} // namespace pse
