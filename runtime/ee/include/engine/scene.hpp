//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        scene.hpp
//
// Description: Scene
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

#include <core/math.hpp>
#include <core/memory.hpp>

#include <engine/components.hpp>
#include <engine/renderer.hpp>

#include <entt/entity/registry.hpp>

namespace pse
{

class game_object;

class scene
{
public:
    scene(render_manager* renderManager) : mRenderManager(renderManager) {}
    ~scene() 
    {
        mEntityMap.clear();
        mRegistry.clear();
    }
    
    game_object create_entity(const std::string& name = std::string());
    game_object create_entity_with_uuid(uuid uuid, const std::string& name = std::string());
    void destroy_entity(game_object entity);
    game_object duplicate_entity(game_object entity);
    game_object find_entity_by_name(std::string_view name);
    game_object get_entity_by_uuid(uuid uuid);
    game_object get_primary_camera_entity();

    template<typename... Components>
    auto get_all_entities_with() { return mRegistry.view<Components...>(); }

    void render();

private:

    render_manager* mRenderManager;

    entt::registry mRegistry;
    std::unordered_map<uuid, entt::entity> mEntityMap;

    friend class game_object;
};


    
} // namespace pse
