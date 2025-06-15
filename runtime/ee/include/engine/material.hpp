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
#include <core/memory.hpp>

namespace pse
{

struct material
{
    memory::resource_id mId;
    math::color mAmbient, mDiffuse, mSpecular, mEmission;
    float mShininess;
    GLenum mColorMaterial;
    
    material(const memory::resource_id& id, const math::color& ambient, const math::color& diffuse, const math::color& specular, const math::color& emission, float shininess, GLenum colorMaterial)
        : mId(id), mAmbient(ambient), mDiffuse(diffuse), mSpecular(specular), mEmission(emission), mShininess(shininess), mColorMaterial(colorMaterial)
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

using material_ptr = std::shared_ptr<material>;

class material_manager;

struct material_deleter
{
    material_manager* mMaterialManager;
    inline void operator()(material* m);
};

class material_manager
{
public:
    material_manager() {}

    ~material_manager()
    {
        mMaterials.clear();
    }

    material_ptr create_material(const memory::resource_id& id, const math::color& ambient, const math::color& diffuse, const math::color& specular, const math::color& emission, float shininess, GLenum colorMaterial)
    {
        material_ptr m = material_ptr(new material(id, ambient, diffuse, specular, emission, shininess, colorMaterial), material_deleter{this});
        mMaterials[id.mUuid] = m;
        return m;
    }

    material_ptr find_material(const uuid& uuid)
    {
        auto it = mMaterials.find(uuid);
        return it->second.lock();
    }

    void delete_material(const uuid& uuid)
    {
        auto it = mMaterials.find(uuid);
        if(it->second.expired()) mMaterials.erase(it);
    }

private:
    // gotta love c++
    std::map<uuid, std::weak_ptr<material>> mMaterials;
};

inline void material_deleter::operator()(material* m) 
{ 
    mMaterialManager->delete_material(m->mId.mUuid);
    delete m;
}
    
} // namespace pse
