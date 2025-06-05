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

//========================================
// PS2SDK Includes
//========================================

//========================================
// Project Includes
//========================================

#include <core/math.hpp>
#include <core/memory.hpp>

#include <engine/camera.hpp>

namespace pse
{

struct id_component
{
    pse::uuid mId;
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
    pse::math::vec4 mTranslation = { 0.0f, 0.0f, 0.0f, 1.0f };
    pse::math::vec4 mRotation = { 0.0f, 0.0f, 0.0f, 0.0f };
    pse::math::vec4 mScale = { 1.0f, 1.0f, 1.0f, 1.0f };

    transform_component() = default;
    transform_component(const transform_component&) = default;
    transform_component(const pse::math::vec3& translation)
        : mTranslation(translation, 1.0F) {}

    pse::math::mat4 get_transform() const
    {
        return pse::math::transformation(mTranslation, mRotation, mScale);
    }
};

enum camera_mode
{
    kOrthographic, kPerspective
};

struct camera_component
{

    bool primary = false;
    camera_mode mode = kPerspective;
    orthographic_camera orthographic = orthographic_camera(1.0F, 1.0F, 1.0F);
    perspective_camera perspective = perspective_camera(90.0F, 1.0F, 4095.0F, 1.0F);

    camera_component() = default;
    camera_component(const camera_component&) = default;
};

struct mesh_renderer_component
{
    pse::uuid mMeshId;

    mesh_renderer_component() = default;
    mesh_renderer_component(const mesh_renderer_component&) = default;

    void* mRuntimeData = nullptr;

};
    
} // namespace pse
