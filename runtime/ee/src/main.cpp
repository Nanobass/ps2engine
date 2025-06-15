//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        main.cpp
//
// Description: Main File
//
//=============================================================================

//========================================
// System Includes
//========================================

/* standard library */
#include <math.h>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <utility>
#include <memory>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <chrono>

/* ps2sdk */
#include <debug.h>
#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <stdio.h>
#include <graph.h>
#include <gs_psm.h>
#include <osd_config.h>

/* ps2gl */
#include <GL/ps2gl.h>
#include <GL/gl.h>
#include <GL/ps2glu.hpp>

/* ps2stuff */
#include <ps2s/gs.h>

//========================================
// Project Includes
//========================================

#include <core/log.hpp>
#include <core/math.hpp>

#include <engine/memory/memory.hpp>
#include <engine/memory/metrics.hpp>

#include <engine/renderer.hpp>
#include <engine/scene.hpp>
#include <engine/gameobject.hpp>
#include <engine/components.hpp>

#include <engine/input/input.hpp>
#include <engine/input/padman.hpp>

#include <engine/modelmanager.hpp>

#include <ps2pad.hpp>

std::unique_ptr<pse::render_manager> g_RenderManager = nullptr;
std::unique_ptr<pse::input::input_manager> g_InputManager = nullptr;

pse::font_ptr g_DefaultFont;

pse::scene* g_Scene = nullptr;

#include <tiny_obj_loader.h>

pse::mesh_ptr load_obj(const pse::memory::resource_id& id, const std::string& objFile, const std::string& dir)
{
    pse::mesh_ptr mesh = g_RenderManager->mModelManager->create_mesh(id);
    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warnings;
    std::string errors;
    tinyobj::LoadObj(&attributes, &shapes, &materials, &warnings, &errors, objFile.c_str(), dir.c_str());

    std::map<std::string, std::pair<pse::material_ptr, pse::texture_ptr>> lut;

    for(auto& material : materials)
    {
        pse::math::color ambient = pse::math::color(material.ambient[0], material.ambient[1], material.ambient[2]);
        pse::math::color diffuse = pse::math::color(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        pse::math::color specular = pse::math::color(material.specular[0], material.specular[1], material.specular[2]); 
        pse::math::color emmision = pse::math::color(material.emission[0], material.emission[1], material.emission[2]);
        pse::material_ptr mat = g_RenderManager->mMaterialManager->create_material(id.mName + material.name, ambient, diffuse, specular, emmision, material.shininess, 0);
        pse::texture_ptr tex = nullptr;
        if(!material.diffuse_texname.empty())
        {
            tex = g_RenderManager->mTextureManager->load_gs_texture(dir + "/" + material.diffuse_texname, dir + "/" + material.diffuse_texname);
        }
        lut[material.name] = std::pair<pse::material_ptr, pse::texture_ptr>(mat, tex);
    }

    printf("alloc\n");

    for(auto& shape : shapes)
    {
        if(shape.mesh.material_ids.empty()) continue;
        std::pair<pse::material_ptr, pse::texture_ptr> mat = lut[materials[shape.mesh.material_ids[0]].name];
        pse::sub_mesh& sub = mesh->mSubMeshes.emplace_back(mat.first, mat.second, GL_TRIANGLES, shape.mesh.indices.size(), shape.mesh.indices.size());
        printf("sub %s\n", materials[shape.mesh.material_ids[0]].name.c_str());
        int j = 0;
        for(auto& i : shape.mesh.indices)
        {
            printf("%d\n", j);
            pse::math::vec3 position = {
                attributes.vertices[i.vertex_index * 3],
                attributes.vertices[i.vertex_index * 3 + 1],
                attributes.vertices[i.vertex_index * 3 + 2]
            };
            pse::math::vec3 normal = {
                attributes.normals[i.normal_index * 3],
                attributes.normals[i.normal_index * 3 + 1],
                -attributes.normals[i.normal_index * 3 + 2]
            };
            pse::math::vec2 texCoord = {
                attributes.texcoords[i.texcoord_index * 2],
                1.0F - attributes.texcoords[i.texcoord_index * 2 + 1],
            };
            sub.mVertexBuffer[j] = position;
            sub.mNormalBuffer[j] = normal;
            sub.mTexCoordBuffer[j] = texCoord;
            j++;
        }
    }
    return mesh;
}

int main(int argc, char const *argv[]) 
{
    pse::initialize_memory_system();
    pse::memory::set_tracking(false, 0, "initialiation");

    SifLoadModule("host0:/irx/sio2man.irx", 0, NULL);
    SifLoadModule("host0:/irx/padman.irx", 0, NULL);
    padInit(0);
    Input::Pad::PadDevice pad;
    pad.Open(0, 0);

    bool bCloseRequested = false;

    g_RenderManager = std::make_unique<pse::render_manager>();
    g_InputManager = std::make_unique<pse::input::input_manager>();

    {
        pse::texture_ptr fontTex = g_RenderManager->mTextureManager->load_gs_texture("emotion.gs", "emotion.gs");
        g_DefaultFont = g_RenderManager->mTextRenderer->load_font("emotion.fnt", "emotion.fnt", fontTex, 32.0F, 39.0F);
    }
    
    /* create scene */

    g_Scene = new(pse::GME_OBJECT) pse::scene(g_RenderManager.get());

    pse::game_object goCamera = g_Scene->create_entity("Camera");
    pse::camera_component& gcCamera = goCamera.add_component<pse::camera_component>(true);
    pse::transform_component& trCamera = goCamera.get_component<pse::transform_component>();
    gcCamera.mCamera.set_perspective(40.0F, 1.0F, 4095.0F);
    trCamera.mTranslation.set(0, 0, 5);

    pse::game_object goLight = g_Scene->create_entity("Sun");
    pse::transform_component& trLight = goLight.get_component<pse::transform_component>();
    pse::light_component& lcLight = goLight.add_component<pse::light_component>(g_RenderManager->mLightingManager->allocate_light(), pse::light_component::kDirectional);
    trLight.mRotation.set(3.1415f / 4.0f, 3.1415f / 4.0f, 0);
    lcLight.mLight->mAmbient.set(0.0F, 0.0F, 0.0F, 1.0F);
    lcLight.mLight->mDiffuse.set(1.0F, 1.0F, 1.0F, 1.0F);
    lcLight.mLight->mSpecular.set(0.0F, 0.0F, 0.0F, 1.0F);

    pse::game_object goPlayer = g_Scene->create_entity("Player");
    goPlayer.add_component<pse::mesh_renderer_component>(load_obj("LVL0.obj", "LVL0.obj", ""));

    /* metrics stuff */

    std::string metrics = "";
    int frameCounter = 0;

    do {
        pad.Poll();
        //g_InputManager->poll_controllers();
        
        if(pad.InputChanged(Input::Pad::Square) && pad.GetInput(Input::Pad::Square).mPressed) pglPrintGsMemAllocation();
        if(pad.InputChanged(Input::Pad::Circle) && pad.GetInput(Input::Pad::Circle).mPressed) bCloseRequested = true;
        if(pad.InputChanged(Input::Pad::Triangle) && pad.GetInput(Input::Pad::Triangle).mPressed) std::terminate();
        if(pad.InputChanged(Input::Pad::Cross) && pad.GetInput(Input::Pad::Cross).mPressed) pse::memory::print_statistics();
 
        trCamera.mRotation.x += -pad.GetInput(Input::Pad::RightY).mInputValue * 0.02f * 2;
        trCamera.mRotation.y += -pad.GetInput(Input::Pad::RightX).mInputValue * 0.02f * 2;

        pse::math::mat4 matrix = pse::math::rotationY(trCamera.mRotation.y) * pse::math::rotationX(trCamera.mRotation.x);
        trCamera.mTranslation += matrix * pse::math::vec4(0, 0, 1) * pad.GetInput(Input::Pad::LeftY).mInputValue * 0.02f * 15;
        trCamera.mTranslation += matrix * pse::math::vec4(1, 0, 0) * pad.GetInput(Input::Pad::LeftX).mInputValue * 0.02f * 15;
        trCamera.mTranslation += matrix * pse::math::vec4(0, 1, 0) * (-pad.GetInput(Input::Pad::L2).mInputValue + pad.GetInput(Input::Pad::R2).mInputValue) * 0.02f * 15;

        /* generate debug text */
        if(frameCounter <= 0)
        {
            metrics = pse::metrics::generate_metrics();
            frameCounter = 30;
        }
        frameCounter--;

        std::stringstream debug_info;
        debug_info << "Camera Position: " << trCamera.mTranslation << std::endl;
        debug_info << "Camera Rotation: " << trCamera.mRotation << std::endl;

        /* rendering loop */
        g_RenderManager->begin_frame();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glEnable(GL_RESCALE_NORMAL);
        
        glEnable(GL_LIGHTING);

        /* render scene */
        g_Scene->render();
       
        /* draw debug text */
        {
            glDisable(GL_LIGHTING);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(-320.0F * g_RenderManager->mAspectRatio, 320.0F * g_RenderManager->mAspectRatio, -240.0F, 240.0F, -1.0F, 1.0F);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glPushMatrix();
            glTranslatef(-320.0F * g_RenderManager->mAspectRatio, 240.0F, 0.0F);
            glScalef(24.0F, 24.0F, 1.0F);
            glTranslatef(0.0F, -1.0F, 0.0F);
            g_RenderManager->mTextRenderer->draw_string(g_DefaultFont, metrics);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(-320.0F * g_RenderManager->mAspectRatio, -240.0F, 0.0F);
            glScalef(24.0F, 24.0F, 1.0F);
            glTranslatef(0.0F, 1.0F, 0.0F);
            g_RenderManager->mTextRenderer->draw_string(g_DefaultFont, debug_info.str());
            glPopMatrix();
        }
 
        g_RenderManager->end_frame();
    } while (!bCloseRequested);

    delete g_Scene;

    g_DefaultFont = nullptr;
    
    g_InputManager.reset();
    g_RenderManager.reset();

    pse::memory::print_statistics();

    return 0;
}