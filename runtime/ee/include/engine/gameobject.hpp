//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        gameobject.hpp
//
// Description: Game Object
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

#include <engine/memory/memory.hpp>

#include <engine/scene.hpp>
#include <engine/components.hpp>

#include <entt/entity/registry.hpp>

namespace pse
{

class game_object
{
public:
    game_object() = default;
    game_object(entt::entity handle, scene* scene) : mEntityHandle(handle), mScene(scene) {}
    game_object(const game_object& other) = default;

    template<typename T, typename... Args>
    T& add_component(Args&&... args)
    {
        mErrorIf(has_component<T>(), "game_object already has component");
        pse::memory::push_allocator(pse::GME_OBJECT);
        T& component = mScene->mRegistry.emplace<T>(mEntityHandle, std::forward<Args>(args)...);
        pse::memory::pop_allocator();
        //mScene->OnComponentAdded<T>(*this, component);
        return component;
    }

    template<typename T, typename... Args>
    T& add_or_replace_component(Args&&... args)
    {
        
        pse::memory::push_allocator(pse::GME_OBJECT);
        T& component = mScene->mRegistry.emplace_or_replace<T>(mEntityHandle, std::forward<Args>(args)...);
        pse::memory::pop_allocator();
        //mScene->OnComponentAdded<T>(*this, component);
        return component;
    }

    template<typename T>
    T& get_component()
    {
        mErrorIf(!has_component<T>(), "game_object does not have component");
        return mScene->mRegistry.get<T>(mEntityHandle);
    }

    template<typename T>
    bool has_component()
    {
        return mScene->mRegistry.any_of<T>(mEntityHandle);
    }

    template<typename T>
    void remove_component()
    {
        mErrorIf(!has_component<T>(), "game_object does not have component");
        pse::memory::push_allocator(pse::GME_OBJECT);
        mScene->mRegistry.remove<T>(mEntityHandle);
        pse::memory::pop_allocator();
    }

    operator bool() const { return mEntityHandle != entt::null; }
    operator entt::entity() const { return mEntityHandle; }
    operator uint32_t() const { return (uint32_t)mEntityHandle; }

    uint32_t get_uuid() { return get_component<id_component>().mId; }
    const std::string& get_name() { return get_component<tag_component>().mTag; }

    bool operator==(const game_object& other) const { return mEntityHandle == other.mEntityHandle && mScene == other.mScene; }
    bool operator!=(const game_object& other) const { return !(*this == other); }

private:
    entt::entity mEntityHandle{ entt::null };
    scene* mScene = nullptr;
};
    
} // namespace pse
