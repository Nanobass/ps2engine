//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        scene.cpp
//
// Description: Scene
//
//=============================================================================

#include <engine/scene.hpp>

//========================================
// System Includes
//========================================

//========================================
// PS2SDK Includes
//========================================

//========================================
// Project Includes
//========================================

#include <engine/memory/memory.hpp>

#include <engine/gameobject.hpp>

//========================================
// Definitions
//========================================

namespace pse
{

game_object scene::create_entity(const std::string& name)
{
    return create_entity_with_uuid(pse::uuid(), name);
}

game_object scene::create_entity_with_uuid(pse::uuid uuid, const std::string& name)
{
    pse::memory::push_allocator(pse::GME_OBJECT);
    game_object entity = { mRegistry.create(), this };
    entity.add_component<id_component>(uuid);
    entity.add_component<transform_component>();
    auto& tag = entity.add_component<tag_component>();
    tag.mTag = name.empty() ? "game_object" : name;
    mEntityMap[uuid] = entity;
    pse::memory::pop_allocator();
    return entity;
}

void scene::destroy_entity(game_object entity)
{
    pse::memory::push_allocator(pse::GME_OBJECT);
    mEntityMap.erase(entity.get_uuid());
    mRegistry.destroy(entity);
    pse::memory::pop_allocator();
}

game_object scene::find_entity_by_name(std::string_view name)
{
    auto view = mRegistry.view<tag_component>();
    for (auto entity : view)
    {
        const tag_component& tc = view.get<tag_component>(entity);
        if (tc.mTag == name)
            return game_object{ entity, this };
    }
    return {};
}

game_object scene::get_entity_by_uuid(pse::uuid uuid)
{
    if (mEntityMap.find(uuid) != mEntityMap.end())
        return { mEntityMap.at(uuid), this };
    return {};
}
    
} // namespace pse
