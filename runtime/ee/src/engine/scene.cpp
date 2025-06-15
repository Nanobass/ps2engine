//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        scene.cpp
//
// Description: scene
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
#include <engine/components.hpp>

//========================================
// Definitions
//========================================

namespace pse
{

game_object scene::create_entity(const std::string& name)
{
    return create_entity_with_uuid(uuid(), name);
}

game_object scene::create_entity_with_uuid(uuid uuid, const std::string& name)
{
    memory::push_allocator(GME_OBJECT);
    game_object entity = { mRegistry.create(), this };
    entity.add_component<id_component>(uuid);
    entity.add_component<transform_component>();
    auto& tag = entity.add_component<tag_component>();
    tag.mTag = name.empty() ? "game_object" : name;
    mEntityMap[uuid] = entity;
    memory::pop_allocator();
    return entity;
}

void scene::destroy_entity(game_object entity)
{
    memory::push_allocator(GME_OBJECT);
    mEntityMap.erase(entity.get_uuid());
    mRegistry.destroy(entity);
    memory::pop_allocator();
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

game_object scene::get_entity_by_uuid(uuid uuid)
{
    if (mEntityMap.find(uuid) != mEntityMap.end())
        return { mEntityMap.at(uuid), this };
    return {};
}

game_object scene::get_primary_camera_entity()
{
    auto view = mRegistry.view<camera_component>();
    for (auto entity : view)
    {
        const auto& camera = view.get<camera_component>(entity);
        if (camera.mPrimary)
            return game_object{entity, this};
    }
    return {};
}

void scene::render()
{
    transform_component* trCamera = nullptr;
    camera_component* gcCamera = nullptr;

    {
        auto view = mRegistry.view<transform_component, camera_component>();
        for(auto entity : view)
        {
            auto [transform, camera] = view.get<transform_component, camera_component>(entity);
            if(camera.mPrimary)
            {
                trCamera = &transform;
                gcCamera = &camera;
                break;
            }
        }
    }

    if(trCamera && gcCamera)
    {
        // TODO: optimize!!!
        glMatrixMode(GL_PROJECTION);
        gcCamera->mCamera.set_viewport(mRenderManager->mWidth, mRenderManager->mHeight, mRenderManager->mAspectRatio);
        gcCamera->mCamera.apply_projection();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glScalef(1.0F / trCamera->mScale.x, 1.0F / trCamera->mScale.y, 1.0F / trCamera->mScale.z);
        glRotatef(-trCamera->mRotation.x, 1.0F, 0.0F, 0.0F);
        glRotatef(-trCamera->mRotation.y, 0.0F, 1.0F, 0.0F);
        glRotatef(-trCamera->mRotation.z, 0.0F, 0.0F, 1.0F);
        glTranslatef(-trCamera->mTranslation.x, -trCamera->mTranslation.y, -trCamera->mTranslation.z);
        //math::mat4 viewMatrix = trCamera->get_transform();
        //pglLoadMatrixCombined(viewMatrix.inverted(), viewMatrix);
    }
    else log::out(log::kError) << "no camera present" << std::endl;
    
    mRenderManager->mLightingManager->set_lighting_enabled(false);
    
    if(trCamera && gcCamera)
    {
        auto view = mRegistry.view<transform_component, skybox_component>();
        for(auto& entity : view)
        {
            auto [transform, skybox] = view.get<transform_component, skybox_component>(entity);
            if(!skybox.mSkyTexture) continue;
            mRenderManager->mSkyboxRenderer->set_sky_texture(skybox.mSkyTexture);
            mRenderManager->mSkyboxRenderer->render_sky(trCamera->mTranslation);
        }
    } else glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mRenderManager->mLightingManager->set_lighting_enabled(true);

    {
        auto view = mRegistry.view<transform_component, light_component>();
        for(auto& entity : view)
        {
            auto [transform, light] = view.get<transform_component, light_component>(entity);
            if(!light.mLight) continue;
            switch(light.mLightType)
            {
                case light_component::kPosition:
                    light.mLight->set_position(math::vec4(-transform.mTranslation, 1.0F));
                break;
                case light_component::kDirectional:
                {
                    math::mat4 matrix = math::rotation(transform.mRotation);
                    math::vec4 direction = matrix * math::vec4(0.0F, 0.0F, 1.0F, 0.0F);
                    light.mLight->set_position(math::vec4(direction, 0.0F));
                }
                break;
                default:
                break;
            }
            light.mLight->set_enabled(true);
            light.mLight->apply();
        }
    }

    {
        auto view = mRegistry.view<transform_component, mesh_renderer_component>();
        for(auto& entity : view)
        {
            auto [transform, mesh] = view.get<transform_component, mesh_renderer_component>(entity);
            if(!mesh.mMesh) continue;
            glPushMatrix();
            pglMultMatrix(transform.get_transform());
            for(auto& sub : mesh.mMesh->mSubMeshes)
            {
                sub.draw_arrays();
            }
            glPopMatrix();
        }
    }

    {
        auto view = mRegistry.view<transform_component, sprite_renderer_component>();
        for(auto& entity : view)
        {
            auto [transform, sprite] = view.get<transform_component, sprite_renderer_component>(entity);
            if(!sprite.mTexture) continue;
            glPushMatrix();
            pglMultMatrix(transform.get_transform());
            mRenderManager->mSpriteRenderer->render_sprite(sprite.mTexture, sprite.mOffset, sprite.mScale, sprite.mColor);
            glPopMatrix();
        }
    }

}
    
} // namespace pse
