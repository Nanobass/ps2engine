//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        lighting.hpp
//
// Description: 
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

/* standard library */
#include <array>

//========================================
// Project Includes
//========================================

/* ps2gl */
#include <GL/gl.h>
#include <GL/ps2gl.h>
#include <GL/ps2glu.hpp>

/* core */
#include <core/log.hpp>
#include <core/math.hpp>

namespace pse
{

struct light
{
    static void deleter(light* light)
    {
        log::out(log::kDebug) << "deallocated light " << (light->mGLLightIndex - GL_LIGHT0) << std::endl;
        light->mAllocated = false;
        light->set_enabled(false);
    }

    light(GLenum glLightIndex) : mGLLightIndex(glLightIndex) {}

    GLenum mGLLightIndex;
    bool mEnabled, mAllocated;
    math::color mAmbient, mDiffuse, mSpecular;
    
    bool is_enabled() { return mEnabled; }
    void set_enabled(bool enable)
    {
        if(enable) glEnable(mGLLightIndex);
        else glDisable(mGLLightIndex);
        mEnabled = enable;
    }

    void set_position(const math::vec4& position)
    {
        glLightfv(mGLLightIndex, GL_POSITION, position.vector);
    }
    
    void apply()
    {
        glLightfv(mGLLightIndex, GL_AMBIENT, mAmbient.vector);
        glLightfv(mGLLightIndex, GL_DIFFUSE, mDiffuse.vector);
        glLightfv(mGLLightIndex, GL_SPECULAR, mSpecular.vector);
    }

};

struct lighting_manager
{
    bool mLightingEnabled = false;
    std::array<light, 8> mLights { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7 };
    math::color mGlobalAmbient = math::color(0.2F, 0.2F, 0.2F, 1.0F);

    lighting_manager()
    {
        log::out(log::kInfo) << "initializing lighting manager: number_of_lights=" << mLights.size() << std::endl;
    }

    bool is_lighting_enabled() { return mLightingEnabled; }
    void set_lighting_enabled(bool enabled) 
    { 
        mLightingEnabled = enabled;
        if(mLightingEnabled) glEnable(GL_LIGHTING);
        else glDisable(GL_LIGHTING);
    }

    std::shared_ptr<light> allocate_light()
    {
        for(auto& l : mLights)
            if(!l.mAllocated)
            {
                l.mAllocated = true;
                log::out(log::kDebug) << "allocated light " << (l.mGLLightIndex - GL_LIGHT0) << std::endl;
                return std::shared_ptr<light>(&l, light::deleter);
            }
        return nullptr;
    }

};
    
} // namespace pse
