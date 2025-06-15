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

/* core */
#include <core/log.hpp>
#include <core/math.hpp>

namespace pse
{

struct light
{

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

using light_ptr = std::shared_ptr<light>;

struct lighting_manager;

struct light_deleter
{
    lighting_manager* mLightingManager;
    inline void operator()(light* light);
};

struct lighting_manager
{
    std::array<light, 8> mLights { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7 };
    bool mLightingEnabled;
    math::color mGlobalAmbient;

    lighting_manager()
    {
        log::out(log::kInfo) << "initializing lighting manager: number_of_lights=" << mLights.size() << std::endl;
        set_lighting_enabled(false);
        set_global_ambient(math::color(0.2F, 0.2F, 0.2F, 1.0F));
    }

    ~lighting_manager()
    {
        log::out(log::kInfo) << "terminate lighting manager" << std::endl;
    }

    bool is_lighting_enabled() { return mLightingEnabled; }
    void set_lighting_enabled(bool enabled) 
    { 
        mLightingEnabled = enabled;
        if(mLightingEnabled) glEnable(GL_LIGHTING);
        else glDisable(GL_LIGHTING);
    }

    math::color get_global_ambient() { return mGlobalAmbient; }
    void set_global_ambient(const math::color& color)
    {
        mGlobalAmbient = color;
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, mGlobalAmbient.vector);
    }

    light_ptr allocate_light()
    {
        for(auto& l : mLights)
            if(!l.mAllocated)
            {
                l.mAllocated = true;
                log::out(log::kDebug) << "allocated light " << (l.mGLLightIndex - GL_LIGHT0) << std::endl;
                return light_ptr(&l, light_deleter{this});
            }
        return nullptr;
    }

    void delete_light(light* light)
    {
        log::out(log::kDebug) << "deallocated light " << (light->mGLLightIndex - GL_LIGHT0) << std::endl;
        light->mAllocated = false;
        light->set_enabled(false);
    }

};

inline void light_deleter::operator()(light* light)
{
    mLightingManager->delete_light(light);
}
    
} // namespace pse
