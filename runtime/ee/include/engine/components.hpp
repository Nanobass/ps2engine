//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        components.hpp
//
// Description: Game Object Basic Components
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

#include <string>
#include <vector>

//========================================
// PS2SDK Includes
//========================================

//========================================
// Project Includes
//========================================

#include <core/math.hpp>
#include <core/memory.hpp>

#include <engine/camera.hpp>
#include <engine/texturemanager.hpp>
#include <engine/lightingmanager.hpp>
#include <engine/modelmanager.hpp>
#include <engine/material.hpp>

namespace pse
{

struct id_component
{
    uuid mId;
    id_component() = default;
    id_component(const id_component&) = default;
};

struct tag_component
{
    std::string mTag;
    tag_component() = default;
    tag_component(const tag_component&) = default;
    tag_component(const std::string& tag) : mTag(tag) {}
};

struct transform_component
{
    math::vec3 mTranslation = { 0.0f, 0.0f, 0.0f };
    math::vec3 mRotation = { 0.0f, 0.0f, 0.0f };
    math::vec3 mScale = { 1.0f, 1.0f, 1.0f };

    transform_component() = default;
    transform_component(const transform_component&) = default;
    transform_component(const math::vec3& translation)
        : mTranslation(translation) {}

    math::mat4 get_transform() const
    {
        return math::transformation(mTranslation, mRotation, mScale);
    }
};

struct camera_component
{
    scene_camera mCamera;
    bool mPrimary = true;

    camera_component() = default;
    camera_component(const camera_component&) = default;
    camera_component(bool primary) 
        : mPrimary(primary) {}
};

struct sprite_renderer_component
{
    texture_ptr mTexture;
    math::color mColor = {1.0F, 1.0F, 1.0F, 1.0F};
    math::texel mOffset = {0.0F, 0.0F};
    math::texel mScale = {1.0F, 1.0F};
    bool mVisible = true;
    
    sprite_renderer_component() = default;
    sprite_renderer_component(const sprite_renderer_component&) = default;
    sprite_renderer_component(texture_ptr texture) : mTexture(texture) {}
};


struct mesh_renderer_component
{
    mesh_ptr mMesh;

    mesh_renderer_component() = default;
    mesh_renderer_component(const mesh_renderer_component&) = default;
    mesh_renderer_component(mesh_ptr mesh) 
        : mMesh(mesh) {}

};

struct light_component
{
    enum light_type { kPosition, kDirectional, kSpot };
    light_ptr mLight = nullptr;
    light_type mLightType = kDirectional;

    light_component() = default;
    light_component(const light_component&) = default;
    light_component(light_ptr light, light_type lightType = kDirectional) 
        : mLight(light), mLightType(lightType) {}
};
    
} // namespace pse
