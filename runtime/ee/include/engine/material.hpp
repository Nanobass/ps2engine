//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        material.hpp
//
// Description: material struct
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

//========================================
// PS2SDK Includes
//========================================

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

struct material
{
    math::color mAmbient, mDiffuse, mSpecular, mEmission;
    float mShininess;
    GLenum mColorMaterial;
    
    material(const math::color& ambient, const math::color& diffuse, const math::color& specular = math::color(1,1,1), const math::color& emission = math::color(0,0,0), float shininess = 0.0F)
        : mAmbient(ambient), mDiffuse(diffuse), mSpecular(specular), mEmission(emission), mShininess(shininess), mColorMaterial(0)
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
    
} // namespace pse
