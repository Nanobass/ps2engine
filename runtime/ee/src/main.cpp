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

#include <ps2pad.hpp>

std::unique_ptr<pse::render_manager> g_RenderManager = nullptr;

struct orthographic_camera {
    pse::math::mat4 mProjectionMatrix;
    pse::math::mat4 mViewMatrix;

    pse::math::vec4 mPosition;

    float mScreenWidth, mScreenHeight;
    float mWorldWidth, mWorldHeight;
    float mAspectRatio;

    orthographic_camera(float width, float height, float aspect)
        :   mWorldWidth(width)
        ,   mWorldHeight(height)
        ,   mAspectRatio(aspect)
    {}

    void apply() 
    {
        mScreenWidth = mWorldWidth * mAspectRatio;
        mScreenHeight = mWorldHeight;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-mScreenWidth / 2.0F, mScreenWidth / 2.0F, -mScreenHeight / 2.0F, mScreenHeight / 2.0F, -1.0F, 1.0F);
        glGetFloatv(GL_PROJECTION_MATRIX, mProjectionMatrix.matrix);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glGetFloatv(GL_MODELVIEW_MATRIX, mViewMatrix.matrix);
    }
};

#include "tiny_obj_loader.h"

pse::mesh* load_obj(const std::string& objFile, const std::string& mtlDir)
{
    pse::mesh* mesh = new pse::mesh;
    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warnings;
    std::string errors;
    tinyobj::LoadObj(&attributes, &shapes, &materials, &warnings, &errors, objFile.c_str(), mtlDir.c_str());
    for(auto& shape : shapes)
    {
        for(auto& i : shape.mesh.indices)
        {
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
            mesh->mVertices.push_back(position);
            mesh->mTexCoords.push_back(texCoord);
            mesh->mNormals.push_back(normal);
        }
    }
    mesh->mMode = GL_TRIANGLES;
    mesh->mCount = mesh->mVertices.size();
    return mesh;
}

pse::input::input_manager* g_InputManager = nullptr;

pse::font* g_DefaultFont = nullptr;
pse::texture* g_KEKW = nullptr;

pse::scene* g_Scene = nullptr;

int main(int argc, char const *argv[]) 
{
    pse::initialize_memory_system();
    pse::memory::set_tracking(true, 0, "initialiation");

    SifLoadModule("host0:/irx/sio2man.irx", 0, NULL);
    SifLoadModule("host0:/irx/padman.irx", 0, NULL);
    padInit(0);
    Input::Pad::PadDevice pad;
    pad.Open(0, 0);

    bool bCloseRequested = false;

    g_RenderManager = std::make_unique<pse::render_manager>();
    g_InputManager = new pse::input::input_manager();

    /* load assets */

    g_DefaultFont = g_RenderManager->mTextRenderer->load_font("emotion", "emotion.fnt", "emotion.gs", 32.0F, 39.0F);
    g_KEKW = g_RenderManager->mTextureManager->load_texture("kekw", "kekw.gs");

    pse::memory::set_tracking(false, 0);
    pse::mesh* hub = load_obj("Hub_normals.obj", "");
    hub->mTexture = g_RenderManager->mTextureManager->load_texture("high", "High.gs");
    pse::memory::set_tracking(true, 0, "initialiation");

    pse::material* material = new pse::material(
        pse::math::color(1.0F, 1.0F, 1.0F, 1.0F),
        pse::math::color(1.0F, 1.0F, 1.0F, 1.0F),
        pse::math::color(0.0F, 0.0F, 0.0F, 1.0F),
        pse::math::color(0.0F, 0.0F, 0.0F, 1.0F),
        0.0F
    );
    
    pse::math::color ambient = pse::math::color(0.2F, 0.2F, 0.2F, 1.0F);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient.vector);

    /* create scene */

    g_Scene = new(pse::GME_OBJECT) pse::scene(g_RenderManager.get());

    pse::game_object goCamera = g_Scene->create_entity("Camera");
    pse::camera_component& gcCamera = goCamera.add_component<pse::camera_component>(true);
    pse::transform_component& trCamera = goCamera.get_component<pse::transform_component>();
    gcCamera.mCamera.set_perspective(40.0F, 1.0F, 4095.0F);
    trCamera.mTranslation.set(0, 0, 5);

    pse::game_object goLight = g_Scene->create_entity("Sun");
    pse::transform_component& trLight = goLight.get_component<pse::transform_component>();
    pse::light_component lcLight = goLight.add_component<pse::light_component>(g_RenderManager->mLightingManager->allocate_light(), pse::light_component::kDirectional);
    trLight.mRotation.set(3.1415f / 4.0f, 3.1415f / 4.0f, 0);
    lcLight.mLight->mAmbient.set(0.0F, 0.0F, 0.0F, 1.0F);
    lcLight.mLight->mDiffuse.set(1.0F, 1.0F, 1.0F, 1.0F);
    lcLight.mLight->mSpecular.set(0.0F, 0.0F, 0.0F, 1.0F);

    pse::game_object goPlayer = g_Scene->create_entity("Player");
    goPlayer.add_component<pse::mesh_renderer_component>(hub);

    /* metrics stuff */

    std::string metrics = "";
    int frameCounter = 0;

    pse::memory::set_tracking(false);
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
        material->apply();

        /* render scene */
        g_Scene->render();
       
        /* draw debug text */
        {
            orthographic_camera hudCamera = orthographic_camera(640.0F, 480.0F, g_RenderManager->mAspectRatio);
            hudCamera.apply();
            glPushMatrix();
            glTranslatef(-hudCamera.mScreenWidth / 2.0F, hudCamera.mScreenHeight / 2.0F, 0.0F);
            glScalef(24.0F, 24.0F, 1.0F);
            glTranslatef(0.0F, -1.0F, 0.0F);
            g_RenderManager->mTextRenderer->draw_string(g_DefaultFont, metrics);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(-hudCamera.mScreenWidth / 2.0F, -hudCamera.mScreenHeight / 2.0F, 0.0F);
            glScalef(24.0F, 24.0F, 1.0F);
            glTranslatef(0.0F, 1.0F, 0.0F);
            g_RenderManager->mTextRenderer->draw_string(g_DefaultFont, debug_info.str());
            glPopMatrix();
        }
 
        g_RenderManager->end_frame();
    } while (!bCloseRequested);

    pse::memory::set_tracking(true, 0, "terminate");
    
    delete g_Scene;

    g_RenderManager->mTextureManager->delete_texture(g_KEKW->mName.mUuid);
    g_RenderManager->mTextRenderer->delete_font(g_DefaultFont->mName.mUuid);

    delete g_InputManager;
    g_RenderManager.reset();

    pse::memory::print_statistics();

    return 0;
}