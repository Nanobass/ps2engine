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

/* engine */
#include <engine/camera.hpp>
#include <engine/texturemanager.hpp>

namespace pse
{

struct light
{
    light(GLenum glLightIndex) : mGLLightIndex(glLightIndex), mAllocated(false) {}

    GLenum mGLLightIndex;
    bool mAllocated, mLocked, mEnabled;
    union
    {
        math::vec4 mPosition;
        math::vec4 mDirection;
    };
    math::color mAmbient, mDiffuse, mSpecular;
    
    bool is_enabled() { return mEnabled; }
    void set_enabled(bool enable)
    {
        if(enable) glEnable(mGLLightIndex);
        else glDisable(mGLLightIndex);
        mEnabled = enable;
    }

    bool is_free() { return !mAllocated && !mLocked; }
    void free(bool force = false) 
    { 
        if(!mLocked || force) 
        {
            set_enabled(false); 
            mAllocated = false;
            mLocked = false;
        }
    }
    
    void apply(perspective_camera& camera)
    {
        if(mEnabled) glEnable(mGLLightIndex);
        else glDisable(mGLLightIndex);
        glPushMatrix();
        gluLoadMatrix(camera.mViewMatrix);
        glLightfv(mGLLightIndex, GL_POSITION, mPosition.vector);
        glLightfv(mGLLightIndex, GL_AMBIENT, mAmbient.vector);
        glLightfv(mGLLightIndex, GL_DIFFUSE, mDiffuse.vector);
        glLightfv(mGLLightIndex, GL_SPECULAR, mSpecular.vector);
        glPopMatrix();
    }

};

struct material
{
    math::color mAmbient, mDiffuse, mSpecular, mEmission;
    float mShininess;
    GLenum mColorMaterial;
    
    material(const math::color& ambient, const math::color& diffuse, const math::color& specular = math::color(1,1,1), const math::color& emission = math::color(0,0,0), float shininess = 0.0F)
        : mAmbient(ambient), mDiffuse(diffuse), mSpecular(specular), mEmission(emission), mShininess(0.0F), mColorMaterial(0)
    {}

    material(const math::color& specular = math::color(1,1,1), const math::color& emission = math::color(0,0,0), float shininess = 0.0F)
        : mAmbient(0, 0, 0), mDiffuse(0, 0, 0), mSpecular(specular), mEmission(emission), mShininess(shininess), mColorMaterial(GL_DIFFUSE)
    {}

    void apply()
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT, mAmbient.vector);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mDiffuse.vector);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mSpecular.vector);
        glMaterialfv(GL_FRONT, GL_EMISSION, mEmission.vector);
        glMaterialf(GL_FRONT, GL_SHININESS, mShininess);
        if(mColorMaterial) 
        {
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT, mColorMaterial);
        } else glDisable(GL_COLOR_MATERIAL);
    }

};

enum class lighting_mode
{
    kDisabled, kColor, kLighting
};

struct lighting_manager
{
    lighting_mode mLightingMode = lighting_mode::kDisabled;
    material* mCurrentMaterial = nullptr;
    std::array<light, 8> mLights { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7 };
    math::color mGlobalAmbient = math::color(0.2F, 0.2F, 0.2F);

    lighting_manager()
    {
        log::out(log::kInfo) << "initializing lighting manager: number_of_lights=" << mLights.size() << std::endl;
    }

    lighting_mode get_lighting_mode() { return mLightingMode; }
    void set_lighting_mode(lighting_mode mode)
    {
        switch(mode)
        {
        case lighting_mode::kDisabled:
            glDisable(GL_LIGHTING);
        break;
        case lighting_mode::kColor:
        break;
        case lighting_mode::kLighting:
            glEnable(GL_LIGHTING);
        break;
        }
        mLightingMode = mode;
    }

    material* get_material() { return mCurrentMaterial; }
    void set_material(material* material)
    {
        if(material) material->apply();
        mCurrentMaterial = material;
    }

    light* allocate_light(bool lock = false)
    {
        for(auto& light : mLights)
            if(light.is_free())
            {
                light.mAllocated = true;
                light.mLocked = lock;
                return &light;
            }
        return nullptr;
    }

    void do_lighting(perspective_camera& camera)
    {
        for(auto& light : mLights)
            light.apply(camera);
    }

};
    
} // namespace pse
