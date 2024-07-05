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
#include <fstream>
#include <utility>
#include <memory>
#include <string.h>
#include <iostream>
#include <algorithm>

//========================================
// Project Includes
//========================================

#include <memory/memory.hpp>
#include <pdi/pdi.hpp>

#ifdef PLATFORM_PS2
#include <pdi/ps2/pdips2.hpp>
#endif

#include <real3d/real3d.hpp>
#include <real3d/scene.hpp>

struct GsTextureHeader {
    static const uint32_t MAGIC = 'G' | ('T' << 8) | ('E' << 16) | ('X' << 24);
    uint32_t mMagic;
    uint16_t mWidth;
    uint16_t mHeight;
    uint8_t mPsm;
    uint16_t mClutWidth;
    uint16_t mClutHeight;
    uint8_t mClutPsm;
    uint8_t mComponents;
    uint8_t mFunction;
} __attribute__ ((packed));

pdi::Texture* LoadGsTexture(const std::string& path, pdi::Device* device)
{
    std::ifstream resource(path, std::ios_base::in);
    GsTextureHeader header;
    resource.read((char*) &header, sizeof(GsTextureHeader));

    pdi::TextureBuffer core(header.mWidth, header.mHeight, pdi::ps2::GetTextureFormat((GS::tPSM) header.mPsm));
    resource >> core.mData;

    if( header.mPsm == GS_PSM_8 || header.mPsm == GS_PSM_4 )
    {
        pdi::TextureBuffer clut(header.mClutWidth, header.mClutHeight, pdi::ps2::GetTextureFormat((GS::tPSM) header.mClutPsm));
        resource >> clut.mData;
        return device->CreateTexture(std::move(core), std::move(clut));
    } else {
        return device->CreateTexture(std::move(core));
    }
}

float verts[] = {
    -1.0F, -1.0F, 0.0F,
     1.0F, -1.0F, 0.0F,
    -1.0F,  1.0F, 0.0F,
     1.0F,  1.0F, 0.0F,
};

float texels[] = {
    0.0F,  1.0F,
    1.0F,  1.0F,
    0.0F,  0.0F,
    1.0F,  0.0F,
};

int main(int argc, char const *argv[])
{
    pdi::PS2Device* device = pdi::CreatePS2();

    pdi::Display* display = device->CreateDisplay();
    display->InitDisplay(640, 512, 32);
    pdi::RenderContext* context = device->CreateRenderContext(display);

    context->SetClearColor(Math::Color(0.1F, 0.1F, 0.1F));
    context->SetClearDepth(1.0F);
    context->SetAmbientLight(Math::Color(1.0F, 1.0F, 1.0F));
    context->EnableZBuffer(true);
    context->SetCullMode(pdi::PDI_CULL_OFF);
    context->SetViewport(0, 0, display->GetWidth(), display->GetHeight());
    context->SetPerspective(1.0F, 4000.0F, 90.0F, display->GetAspectRatio());
    context->EnableLighting(true);
    context->SetLight(0, pdi::PDI_LIGHT_DIRECTIONAL, Math::Color(1.0F, 1.0F, 1.0F), Math::Vec3(0, 0, 0), Math::Vec3(0, 0, 1));

    r3d::Scene scene(device, display, context);

    {
        pdi::Texture* grass = LoadGsTexture("host:grass128_32bit.gs", device);
        pdi::VertexBuffer* vertices = new(GMA_GRAPHICS) pdi::VertexBuffer(12, 3);
        pdi::VertexBuffer* texcoords = new(GMA_GRAPHICS) pdi::VertexBuffer(8, 2);
        
        std::copy_n(verts, 12, vertices->mData.data());
        std::copy_n(texels, 8, texcoords->mData.data());

        scene.AddTexture("grass", grass);
        scene.AddVertexBuffer("grass_v", vertices);
        scene.AddVertexBuffer("grass_t", texcoords);
    }
    
    std::shared_ptr<r3d::Model> grass_model;
    {
        
        std::shared_ptr<r3d::Mesh> grass_m = scene.CreateMesh("grass_m");
        grass_m->SetTexture("grass");
        grass_m->AddPrimBufferVT(pdi::PRIM_TRIANGLE_STRIP, 4, "grass_v", "grass_t");
        grass_model = scene.CreateModel("grass_model", "grass_m");
    }

    Math::Camera camera;
    camera.mPosition = Math::Vec3(0, 0, 1.5F);
    camera.mLookAt = Math::Vec3(0, 0, 0);
    camera.mUp = Math::Vec3(0, 1, 0);

    do {
        context->BeginFrame();
        context->Clear(pdi::PDI_COLOR_BUFFER | pdi::PDI_DEPTH_BUFFER);

        context->LoadMatrix(camera.GetViewMatrix());
        scene.DrawModel(grass_model.get());

        context->EndFrame();
        display->SwapBuffers();
    } while (1);

    delete context;
    delete display;
    delete device;
    return 0;
}
