//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        hal.hpp
//
// Description: Platform Driver Interface
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

/* standard library */
#include <math.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <list>

//========================================
// Project Includes
//========================================

#include <memory/buffer.hpp>
#include <pdi/pditypes.hpp>

namespace pdi {

class Display
{
public:
    virtual bool InitDisplay(uint32_t width, uint32_t height, uint32_t bpp) = 0;
    virtual bool InitDisplay(DisplayMode* initData) = 0;

    virtual uint32_t GetHeight() = 0;
    virtual uint32_t GetWidth() = 0;
    virtual uint32_t GetDepth() = 0;
    virtual DisplayMode GetDisplayMode() = 0;
    
    virtual float GetAspectRatio() = 0;

    virtual void SwapBuffers() = 0;

public:
    virtual ~Display() {};
};

struct TextureBuffer
{
    static uint32_t GetSize(uint16_t width, uint16_t height, TextureFormat psm)
    {
        switch(psm)
        {
        case TEXTURE_RGBA_8_8_8_8: 
            return width * height * 4;
        case TEXTURE_RGB_8_8_8: 
            return width * height * 3;
        case TEXTURE_RGBA_5_5_5_1: 
        case TEXTURE_RGBA_4_4_4_4:
            return width * height * 2;
        case TEXTURE_INDEX_8:
            return width * height;
        case TEXTURE_INDEX_4:
            return width * height * 4 / 8;
        default: return 0;
        }
    }

    uint16_t mWidth = 0;
    uint16_t mHeight = 0;
    TextureFormat mFormat = TEXTURE_INVALID;
    buffer<uint8_t> mData;

    TextureBuffer(uint16_t width, uint16_t height, TextureFormat format)
        :   mWidth(width),
            mHeight(height),
            mFormat(format),
            mData(GetSize(), GMA_GRAPHICS)
    {}

    TextureBuffer(const TextureBuffer&) = delete;
    TextureBuffer& operator=(const TextureBuffer&) = delete;

    TextureBuffer(TextureBuffer&& other) noexcept
        : mWidth(std::exchange(other.mWidth, 0)),
          mHeight(std::exchange(other.mHeight, 0)),
          mFormat(std::exchange(other.mFormat, TEXTURE_INVALID)),
          mData(std::move(other.mData))
    {}

    uint32_t GetSize() 
    {
        return GetSize(mWidth, mHeight, mFormat);
    }
};

class Texture
{
    virtual TextureFormat GetFormat() = 0;
    virtual uint32_t GetWidth() = 0;
    virtual uint32_t GetHeight() = 0;
    virtual uint8_t GetNumberOfMipMaps() = 0;

public:
    virtual ~Texture() {};
};

struct VertexBuffer
{
    uint32_t mSize = 0;
    uint16_t mElementSize = 0;
    buffer<float> mData;

    VertexBuffer(uint32_t size, uint32_t elementSize)
        :   mSize(size),
            mElementSize(elementSize),
            mData(size, GMA_GRAPHICS)
    {}

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    VertexBuffer(VertexBuffer&& other) noexcept
        : mSize(std::exchange(other.mSize, 0)),
          mElementSize(std::exchange(other.mElementSize, 0)),
          mData(std::move(other.mData))
    {}
};

class PrimBuffer
{
public:
    PrimType type;
    size_t count;
    VertexBuffer* mVertexBuffer = nullptr;
    VertexBuffer* mTextureCoordBuffer = nullptr;
    VertexBuffer* mNormalBuffer = nullptr;
    VertexBuffer* mColorBuffer = nullptr;
};

class RenderContext
{
public:
    // frame synchronisation
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;

    // buffer clearing
    virtual void SetClearColor(const Math::Color& colour) = 0;
    virtual Math::Color GetClearColor() = 0;
    virtual void SetClearDepth(float depth) = 0;
    virtual float GetClearDepth() = 0;
    virtual void Clear(uint32_t mask) = 0;

    // matrix stack
    virtual void SetMatrixMode(MatrixMode mode) = 0;
    virtual void IdentityMatrix() = 0;
    virtual void LoadMatrix(const Math::Mat4& matrix) = 0;
    virtual void PushMatrix() = 0;
    virtual void PopMatrix() = 0;
    virtual void MultMatrix(const Math::Mat4& matrix) = 0;
    virtual Math::Mat4 GetMatrix() = 0;

    virtual void PushIdentityMatrix() = 0;
    virtual void PushLoadMatrix(const Math::Mat4& matrix) = 0;
    virtual void PushMultMatrix(const Math::Mat4& matrix) = 0;

    virtual void SetViewport(float left, float top, float right, float bottom) = 0;
    virtual void GetViewport(float* left, float* top, float* right, float* bottom) = 0;

    virtual void SetPerspective(float near, float far, float fov, float aspect) = 0;
    virtual void SetOrthographic(float aspect) = 0;

    // lighting
    virtual uint32_t GetMaxLights() = 0;

    virtual void SetAmbientLight(Math::Color col) = 0;
    virtual Math::Color GetAmbientLight() = 0;
    
    virtual void EnableLighting(bool enabled) = 0;
    virtual void EnableLight(int handle, bool active) = 0;
    virtual void SetLight(int handle, LightType type, Math::Color color, Math::Vec3 position, Math::Vec3 direction) = 0;

    // drawing
    virtual void DrawPrimBuffer(Texture* t, PrimBuffer* b) = 0;

    // culling 
    virtual void SetCullMode(CullMode mode) = 0;
    virtual CullMode GetCullMode() = 0;

    // z-buffer control
    virtual void EnableZBuffer(bool enable) = 0;
    virtual bool IsZBufferEnabled() = 0;

    virtual void SetZWrite(bool) = 0;
    virtual bool GetZWrite() = 0;

    // polygon fill
    virtual void SetFillMode(FillMode mode) = 0;
    virtual FillMode GetFillMode() = 0;

    virtual void DrawSync() = 0;

public:
    virtual ~RenderContext() {};
};

class Device
{
public:
    virtual Display* CreateDisplay() = 0;
    virtual RenderContext* CreateRenderContext(Display* d) = 0;
    virtual Texture* CreateTexture(TextureBuffer core) = 0;
    virtual Texture* CreateTexture(TextureBuffer core, TextureBuffer palette) = 0;

public:
    virtual ~Device() {};
};

} // namespace hal