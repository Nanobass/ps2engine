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
#include <memory>
#include <map>
#include <cassert>
#include <fstream>
#include <iostream>
#include <array>

/* ps2gl */
#include <GL/ps2gl.h>
#include <GL/gl.h>

/* ps2stuff */
#include <ps2s/gs.h>

//========================================
// Project Includes
//========================================

/* ps2memory */
#include <core/memory.hpp>

/* ps2glu */
#include <ps2glu.hpp>

/* ps2math */
#include <core/math.hpp>

/* engine */
#include <engine/texturemanager.hpp>
#include <engine/camera.hpp>

#include <core/log.hpp>

namespace pse
{

struct Light
{
    Light(GLenum glLightIndex) : mGLLightIndex(glLightIndex), mAllocated(false) {}

    GLenum mGLLightIndex;
    bool mAllocated, mLocked, mEnabled;
    union
    {
        pse::math::vec4 mPosition;
        pse::math::vec4 mDirection;
    };
    pse::math::color mAmbient, mDiffuse, mSpecular;
    
    bool IsEnabled() { return mEnabled; }
    void SetEnabled(bool enable)
    {
        if(enable) glEnable(mGLLightIndex);
        else glDisable(mGLLightIndex);
        mEnabled = enable;
    }

    bool IsFree() { return !mAllocated && !mLocked; }
    void Free(bool force = false) 
    { 
        if(!mLocked || force) 
        {
            SetEnabled(false); 
            mAllocated = false;
            mLocked = false;
        }
    }
    
    void Apply(PerspectiveCamera& camera)
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

struct Material
{
    pse::math::color mAmbient, mDiffuse, mSpecular, mEmission;
    float mShininess;
    GLenum mColorMaterial;
    
    Material(const pse::math::color& ambient, const pse::math::color& diffuse, const pse::math::color& specular = pse::math::color(1,1,1), const pse::math::color& emission = pse::math::color(0,0,0), float shininess = 0.0F)
        : mAmbient(ambient), mDiffuse(diffuse), mSpecular(specular), mEmission(emission), mShininess(0.0F), mColorMaterial(0)
    {}

    Material(const pse::math::color& specular = pse::math::color(1,1,1), const pse::math::color& emission = pse::math::color(0,0,0), float shininess = 0.0F)
        : mAmbient(0, 0, 0), mDiffuse(0, 0, 0), mSpecular(specular), mEmission(emission), mShininess(shininess), mColorMaterial(GL_DIFFUSE)
    {}

    void Apply()
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

enum class LightingMode
{
    Disabled, Color, Lighting
};

struct LightingManager
{
    LightingMode mLightingMode = LightingMode::Disabled;
    Material* mCurrentMaterial = nullptr;
    std::array<Light, 8> mLights { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7 };
    pse::math::color mGlobalAmbient = pse::math::color(0.2F, 0.2F, 0.2F);

    LightingManager()
    {
        log::out(log::kInfo) << "Initializing LightingManager: NumberOfLights=" << mLights.size() << std::endl;
    }

    LightingMode GetLightingMode() { return mLightingMode; }
    void SetLightingMode(LightingMode mode)
    {
        switch(mode)
        {
        case LightingMode::Disabled:
            glDisable(GL_LIGHTING);
        break;
        case LightingMode::Color:
        break;
        case LightingMode::Lighting:
            glEnable(GL_LIGHTING);
        break;
        }
        mLightingMode = mode;
    }

    Material* GetMaterial() { return mCurrentMaterial; }
    void SetMaterial(Material* material)
    {
        if(material) material->Apply();
        mCurrentMaterial = material;
    }

    Light* AllocateLight(bool lock = false)
    {
        for(auto& light : mLights)
            if(light.IsFree())
            {
                light.mAllocated = true;
                light.mLocked = lock;
                return &light;
            }
        return nullptr;
    }

    void DoLighting(PerspectiveCamera& camera)
    {
        for(auto& light : mLights)
            light.Apply(camera);
    }

};
    
} // namespace pse
