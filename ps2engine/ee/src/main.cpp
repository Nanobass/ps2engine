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

/* ps2sdk */
#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <stdio.h>

#include <ps2pad.hpp>
using namespace Input;

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

std::unique_ptr<pdi::Texture> LoadGsTexture(const std::string& path, pdi::Device* device)
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

std::unique_ptr<pdi::VertexBuffer> CopyVertexBuffer(float* data, size_t size, size_t elementSize, pdi::Device* device)
{
    std::unique_ptr<pdi::VertexBuffer> buffer = device->CreateVertexBuffer(size, elementSize);
    std::copy_n(data, size, buffer->mData.data());
    return buffer;
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
    auto device = pdi::CreatePS2();
    auto display = device->CreateDisplay();
    display->InitDisplay(640, 512, 32);
    auto context = device->CreateRenderContext(display.get());

    SifLoadModule("host0:/irx/sio2man.irx", 0, NULL);
    SifLoadModule("host0:/irx/padman.irx", 0, NULL);
    padInit(0);
    Pad::PadDevice pad;
    pad.Open(0, 0);

    context->SetClearColor(Math::Color(0.1F, 0.1F, 0.1F));
    context->SetClearDepth(1.0F);
    context->SetAmbientLight(Math::Color(1.0F, 1.0F, 1.0F));
    context->EnableZBuffer(true);
    context->SetCullMode(pdi::PDI_CULL_OFF);
    context->SetViewport(0, 0, display->GetWidth(), display->GetHeight());
    context->SetPerspective(1.0F, 4000.0F, 90.0F, display->GetAspectRatio());
    context->EnableLighting(true);
    context->SetLight(0, pdi::PDI_LIGHT_DIRECTIONAL, Math::Color(1.0F, 1.0F, 1.0F), Math::Vec3(0, 0, 0), Math::Vec3(0, 0, 1));

    r3d::Scene scene(device.get(), display.get(), context.get());
    scene.AddTexture("grass", LoadGsTexture("host:grass128_32bit.gs", device.get()));
    scene.AddTexture("fabian", LoadGsTexture("host:fabian256_32bit.gs", device.get()));
    scene.AddVertexBuffer("quad_v", CopyVertexBuffer(verts, 12, 3, device.get()));
    scene.AddVertexBuffer("quad_t", CopyVertexBuffer(texels, 8, 2, device.get()));
    
    std::shared_ptr<r3d::Model> grass_model, fabian_model;
    {
        std::shared_ptr<r3d::Mesh> grass_m = scene.CreateMesh("grass_m");
        grass_m->SetTexture("grass");
        grass_m->AddPrimBufferVT(pdi::PRIM_TRIANGLE_STRIP, 4, "quad_v", "quad_t");
        grass_model = scene.CreateModel("grass_model", "grass_m");

        std::shared_ptr<r3d::Mesh> fabian_m = scene.CreateMesh("fabian_m");
        fabian_m->SetTexture("fabian");
        fabian_m->AddPrimBufferVT(pdi::PRIM_TRIANGLE_STRIP, 4, "quad_v", "quad_t");
        fabian_model = scene.CreateModel("fabian_model", "fabian_m");
    }

    grass_model->mTransform.mPosition.x = -1.25F;
    fabian_model->mTransform.mPosition.x = 1.25F;

    Math::Camera camera;
    camera.mPosition = Math::Vec3(0, 0, 1.5F);
    camera.mLookAt = Math::Vec3(0, 0, 0);
    camera.mUp = Math::Vec3(0, 1, 0);

    do {
        pad.Poll();
        if(pad.InputChanged(Pad::Square) && pad.GetInput(Pad::Square).mPressed) grass_model = nullptr;
        if(pad.InputChanged(Pad::Circle) && pad.GetInput(Pad::Circle).mPressed) fabian_model = nullptr;
        if(pad.InputChanged(Pad::Triangle) && pad.GetInput(Pad::Triangle).mPressed) scene.PrintAllocations();
        if(pad.InputChanged(Pad::Select) && pad.GetInput(Pad::Select).mPressed) scene.ClearUnreferencedAssets();

        context->BeginFrame();
        context->Clear(pdi::PDI_COLOR_BUFFER | pdi::PDI_DEPTH_BUFFER);

        context->LoadMatrix(camera.GetViewMatrix());
        if(grass_model != nullptr) scene.DrawModel(grass_model.get());
        if(fabian_model != nullptr) scene.DrawModel(fabian_model.get());

        context->EndFrame();
        display->SwapBuffers();
    } while (1);

    return 0;
}
