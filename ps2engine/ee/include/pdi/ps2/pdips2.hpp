//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        pdips2.hpp
//
// Description: PS2 Platform Driver
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

/* ps2sdk */
#include <kernel.h>
#include <graph.h>
#include <graph_vram.h>
#include <gs_psm.h>
#include <osd_config.h>

/* standard library */
#include <math.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <list>

/* ps2gl */
#include <GL/ps2gl.h>
#include <GL/gl.h>

/* ps2stuff */
#include <ps2s/gs.h>

//========================================
// Project Includes
//========================================

#include <pdi/pdi.hpp>

namespace pdi
{

namespace opengl
{
void GetOpenGLTextureFormatAndType(TextureFormat texformat, GLenum& format, GLenum& type)
{
    switch (texformat)
        {
        case TEXTURE_RGBA_8_8_8_8: 
            format = GL_RGBA;
            type = GL_UNSIGNED_INT_8_8_8_8;
        break;
        case TEXTURE_RGB_8_8_8: 
            format = GL_RGB;
            type = GL_UNSIGNED_BYTE;
        break;
        case TEXTURE_RGBA_5_5_5_1: 
            format = GL_RGBA;
            type = GL_UNSIGNED_SHORT_5_5_5_1;
        break;
        case TEXTURE_RGBA_4_4_4_4:
            format = GL_RGBA;
            type = GL_UNSIGNED_SHORT_4_4_4_4;
        break;
        case TEXTURE_INDEX_8:  
            format = GL_COLOR_INDEX;
            type = GL_UNSIGNED_BYTE;
        break;
        case TEXTURE_INDEX_4:
            format = GL_COLOR_INDEX;
            type = GL_UNSIGNED_SHORT_4_4_4_4; // is this right????
        break;
        case TEXTURE_INVALID: break;
    }
}
} // namespace opengl

namespace ps2
{
float GetSystemAspectRatio()
{
    switch (configGetTvScreenType())
    {
    case TV_SCREEN_43: return 4.0F / 3.0F;
    case TV_SCREEN_169: return 16.0F / 9.0F;
    case TV_SCREEN_FULL: return 1.0F; // should i ingore this one???
    default: return 4.0F / 3.0F; // we should never get here!!!
    }
}

pdi::TextureFormat GetTextureFormat(GS::tPSM psm)
{
    switch(psm)
    {
        case GS::kPsm32: 
            return pdi::TEXTURE_RGBA_8_8_8_8;
        case GS::kPsm24: 
            return pdi::TEXTURE_RGB_8_8_8;
        case GS::kPsm16: 
        case GS::kPsm16s: 
            return pdi::TEXTURE_RGBA_5_5_5_1;
        case GS::kPsm8: 
        case GS::kPsm8h: 
            return pdi::TEXTURE_INDEX_8;
        case GS::kPsm4: 
        case GS::kPsm4hh: 
        case GS::kPsm4hl: 
            return pdi::TEXTURE_INDEX_4;
        default: 
            return pdi::TEXTURE_INVALID;
    }
}
} // namespace ps2

class PS2Texture : public Texture
{
public:
    TextureBuffer* mCore = nullptr;
    TextureBuffer* mClutBuffer = nullptr;
    uint8_t mFunction = 0;
    GLuint mGLName;

    PS2Texture(TextureBuffer core, uint8_t function) : mFunction(function), mGLName(0)
    {
        mCore = new(GMA_GRAPHICS) TextureBuffer(std::move(core));
        glGenTextures(1, &mGLName);
        Upload();
    }

    PS2Texture(TextureBuffer core, TextureBuffer clut, uint8_t function) : mFunction(function)
    {
        mCore = new(GMA_GRAPHICS) TextureBuffer(std::move(core));
        mClutBuffer = new(GMA_GRAPHICS) TextureBuffer(std::move(clut));
        glGenTextures(1, &mGLName);
        Upload();
    }

    ~PS2Texture()
    {
        if(mCore) delete mCore;
        if(mClutBuffer) delete mClutBuffer;
        if(mGLName != 0) glDeleteTextures(1, &mGLName);
    }

    void Upload()
    {
        GLenum format, type;
        opengl::GetOpenGLTextureFormatAndType(mCore->mFormat, format, type);
        glBindTexture(GL_TEXTURE_2D, mGLName);
        glTexImage2D(GL_TEXTURE_2D, 0 /* mipmap */, GL_RGBA /* ignored */, mCore->mWidth, mCore->mHeight, 0 /* border */, format, type, mCore->mData.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        if(mClutBuffer)
        {
            uint32_t width = 0;
            if(mCore->mFormat == TEXTURE_INDEX_8) width = 256;
            else if(mCore->mFormat == TEXTURE_INDEX_4) width = 16;
            GLenum format, type;
            opengl::GetOpenGLTextureFormatAndType(mClutBuffer->mFormat, format, type);
            glColorTable(GL_COLOR_TABLE, GL_RGBA, width, format, type, mClutBuffer->mData.data());
        }
    }

    TextureFormat GetFormat() { return mCore->mFormat; }
    uint32_t GetWidth() { return mCore->mWidth; }
    virtual uint32_t GetHeight() { return mCore->mHeight; }
    virtual uint8_t GetNumberOfMipMaps() { return 1; }

    std::ostream& operator<<(std::ostream& os)
    {
        os << "Texture " << mCore->mWidth << "x" << mCore->mHeight << " "; 
        switch (mCore->mFormat)
        {
        case TEXTURE_RGBA_8_8_8_8:
            os << "32-Bit";
        break;
        case TEXTURE_RGB_8_8_8:
            os << "24-Bit";
        break;
        case TEXTURE_RGBA_5_5_5_1:
            os << "16-Bit";
        break;
        case TEXTURE_INDEX_8:
            os << "8-Bit";
        break;
        case TEXTURE_INDEX_4:
            os << "4-Bit";
        break;
        default: break;
        }
        if(mClutBuffer && (mCore->mFormat == TEXTURE_INDEX_8 || mCore->mFormat == TEXTURE_INDEX_4))
        {
            switch (mClutBuffer->mFormat)
            {
                case TEXTURE_RGBA_8_8_8_8:
                    os << " (32-Bit)";
                break;
                case TEXTURE_RGB_8_8_8:
                    os << " (24-Bit)";
                break;
                case TEXTURE_RGBA_5_5_5_1:
                    os << " (16-Bit)";
                break;
                default: break;
            }
        }
        return os;
    }
};

enum TVMode
{
    PAL, NTSC, DTV480P
};

class PS2Display : public Display
{
    pgl_slot_handle_t frame_slot_0 , frame_slot_1 , depth_slot ;
    pgl_area_handle_t frame_area_0 , frame_area_1 , depth_area ;

    uint32_t mWidth, mHeight, mDepth;
    float mAspectRatio;

public:

    bool InitDisplay(uint32_t width, uint32_t height, uint32_t bpp)
    {
        DisplayMode mode;
        mode.width = width;
        mode.height = height;
        mode.bpp = bpp;
        return InitDisplay(&mode);
    }

    bool InitDisplay(DisplayMode* initData)
    {
        TVMode mode = PAL;
        pglRemoveAllGsMemSlots();
        switch (mode)
        {
        case PAL:
            mWidth = 640;
            mHeight = 512;
            graph_set_mode(GRAPH_MODE_INTERLACED, GRAPH_MODE_PAL, GRAPH_MODE_FIELD, GRAPH_ENABLE);
            frame_slot_0 = pglAddGsMemSlot(  0, 80, GS_PSM_24);
            frame_slot_1 = pglAddGsMemSlot( 80, 80, GS_PSM_24);
            depth_slot   = pglAddGsMemSlot(160, 80, GS_PSMZ_24);
            pglLockGsMemSlot(frame_slot_0);
            pglLockGsMemSlot(frame_slot_1);
            pglLockGsMemSlot(depth_slot);
            frame_area_0 = pglCreateGsMemArea(mWidth, mHeight, GS_PSM_24);
            frame_area_1 = pglCreateGsMemArea(mWidth, mHeight, GS_PSM_24);
            depth_area   = pglCreateGsMemArea(mWidth, mHeight, GS_PSMZ_24);
            pglBindGsMemAreaToSlot(frame_area_0, frame_slot_0);
            pglBindGsMemAreaToSlot(frame_area_1, frame_slot_1);
            pglBindGsMemAreaToSlot(depth_area, depth_slot);
            pglSetDrawBuffers(PGL_INTERLACED, frame_area_0, frame_area_1, depth_area);
            pglSetDisplayBuffers(PGL_INTERLACED, frame_area_0, frame_area_1);
        break;
        case NTSC:
            mWidth = 640;
            mHeight = 448;
            graph_set_mode(GRAPH_MODE_INTERLACED, GRAPH_MODE_NTSC, GRAPH_MODE_FIELD, GRAPH_ENABLE);
            frame_slot_0 = pglAddGsMemSlot(  0, 70, GS_PSM_24);
            frame_slot_1 = pglAddGsMemSlot( 70, 70, GS_PSM_24);
            depth_slot   = pglAddGsMemSlot(140, 70, GS_PSMZ_24);
            pglLockGsMemSlot(frame_slot_0);
            pglLockGsMemSlot(frame_slot_1);
            pglLockGsMemSlot(depth_slot);
            frame_area_0 = pglCreateGsMemArea(mWidth, mHeight, GS_PSM_24);
            frame_area_1 = pglCreateGsMemArea(mWidth, mHeight, GS_PSM_24);
            depth_area   = pglCreateGsMemArea(mWidth, mHeight, GS_PSMZ_24);
            pglBindGsMemAreaToSlot(frame_area_0, frame_slot_0);
            pglBindGsMemAreaToSlot(frame_area_1, frame_slot_1);
            pglBindGsMemAreaToSlot(depth_area, depth_slot);
            pglSetDrawBuffers(PGL_INTERLACED, frame_area_0, frame_area_1, depth_area);
            pglSetDisplayBuffers(PGL_INTERLACED, frame_area_0, frame_area_1);
        break;
        case DTV480P:
            mWidth = 640;
            mHeight = 480;
            graph_set_mode(GRAPH_MODE_NONINTERLACED, GRAPH_MODE_HDTV_480P, GRAPH_MODE_FRAME, GRAPH_DISABLE);
            frame_slot_0 = pglAddGsMemSlot(  0, 75, GS_PSM_24);
            frame_slot_1 = pglAddGsMemSlot( 75, 75, GS_PSM_24);
            depth_slot   = pglAddGsMemSlot(150, 75, GS_PSMZ_24);
            pglLockGsMemSlot(frame_slot_0);
            pglLockGsMemSlot(frame_slot_1);
            pglLockGsMemSlot(depth_slot);
            frame_area_0 = pglCreateGsMemArea(mWidth, mHeight, GS_PSM_24);
            frame_area_1 = pglCreateGsMemArea(mWidth, mHeight, GS_PSM_24);
            depth_area   = pglCreateGsMemArea(mWidth, mHeight, GS_PSMZ_24);
            pglBindGsMemAreaToSlot(frame_area_0, frame_slot_0);
            pglBindGsMemAreaToSlot(frame_area_1, frame_slot_1);
            pglBindGsMemAreaToSlot(depth_area, depth_slot);
            pglSetDrawBuffers(PGL_NONINTERLACED, frame_area_0, frame_area_1, depth_area);
            pglSetDisplayBuffers(PGL_NONINTERLACED, frame_area_0, frame_area_1);
        break;
        }
        // TODO implement texture pages
        // 8 bit textures mangled into the frame and depth buffers
        // 128x128
        pglAddGsMemSlot(0,    8,   GS::kPsm8h);
        pglAddGsMemSlot(8,    8,   GS::kPsm8h);
        pglAddGsMemSlot(16,   8,   GS::kPsm8h);
        pglAddGsMemSlot(24,   8,   GS::kPsm8h);
        // 256x128
        pglAddGsMemSlot(32,   16,  GS::kPsm8h);
        // 256x256
        pglAddGsMemSlot(48,   32,  GS::kPsm8h);
        pglAddGsMemSlot(80,   32,  GS::kPsm8h);
        // 512x256
        pglAddGsMemSlot(112,  64,  GS::kPsm8h);
        pglAddGsMemSlot(176,  64,  GS::kPsm8h);

        // fill the rest with 32 bit textures
        // 64x32
        pglAddGsMemSlot(240,  1,   GS::kPsm32);
        pglAddGsMemSlot(241,  1,   GS::kPsm32);
        pglAddGsMemSlot(242,  1,   GS::kPsm32);
        pglAddGsMemSlot(243,  1,   GS::kPsm32);
        // 64x64
        pglAddGsMemSlot(244,  2,   GS::kPsm32);
        pglAddGsMemSlot(246,  2,   GS::kPsm32);
        pglAddGsMemSlot(248,  2,   GS::kPsm32);
        pglAddGsMemSlot(250,  2,   GS::kPsm32);
        pglAddGsMemSlot(252,  2,   GS::kPsm32);
        pglAddGsMemSlot(254,  2,   GS::kPsm32);
        // 128x128
        pglAddGsMemSlot(256,  8,   GS::kPsm32);
        pglAddGsMemSlot(264,  8,   GS::kPsm32);
        pglAddGsMemSlot(272,  8,   GS::kPsm32);
        pglAddGsMemSlot(280,  8,   GS::kPsm32);
        pglAddGsMemSlot(288,  8,   GS::kPsm32);
        pglAddGsMemSlot(296,  8,   GS::kPsm32);
        pglAddGsMemSlot(304,  8,   GS::kPsm32);
        pglAddGsMemSlot(312,  8,   GS::kPsm32);
        // 256x256
        pglAddGsMemSlot(320,  32,  GS::kPsm32);
        pglAddGsMemSlot(352,  32,  GS::kPsm32);
        // 512x256
        pglAddGsMemSlot(384,  64,  GS::kPsm32);
        pglAddGsMemSlot(448,  64,  GS::kPsm32);
        
        pglPrintGsMemAllocation();
        mAspectRatio = ps2::GetSystemAspectRatio();
        return true;
    }

    uint32_t GetHeight() { return mWidth; }
    uint32_t GetWidth() { return mHeight; }
    uint32_t GetDepth() { return mDepth; }

    DisplayMode GetDisplayMode()
    {
        DisplayMode mode;
        mode.width = mWidth;
        mode.height = mHeight;
        mode.bpp = 32;
        return mode;
    }
    
    float GetAspectRatio() { return mAspectRatio; }

    void SwapBuffers() {}

};

class PS2RenderContext : public RenderContext
{
private:
    bool mFirstFrame;
    PS2Display* mDisplay;

    Math::Color mClearColor;
    float mClearDepth;
    Math::Color mAmbientColor;
    MatrixMode mMatrixMode = PDI_MATRIX_MODELVIEW;
    float mViewportTop, mViewportBottom, mViewportLeft, mViewportRight;
    CullMode mCullMode;
    bool mZTest, mZWrite;
    FillMode mFillMode;

public:

    PS2RenderContext(PS2Display* display) : mFirstFrame(true), mDisplay(display) 
    {
        glEnable(GL_RESCALE_NORMAL);
    }

    void BeginFrame() { pglBeginGeometry(); }

    void EndFrame()
    {
        pglEndGeometry();
        if(!mFirstFrame) pglFinishRenderingGeometry(PGL_DONT_FORCE_IMMEDIATE_STOP);
        else mFirstFrame = false;
        pglWaitForVSync();
        pglSwapBuffers();
        pglRenderGeometry();
    }

    // buffer clearing
    void SetClearColor(const Math::Color& color) { mClearColor = color; glClearColor(color.r, color.g, color.b, color.a); }
    Math::Color GetClearColor() { return mClearColor; }
    void SetClearDepth(float depth) { mClearDepth = depth; glClearDepth(depth); }
    float GetClearDepth() { return mClearDepth; }

    void Clear(uint32_t mask)
    {
        GLbitfield bits = 0;
        if(mask & PDI_COLOR_BUFFER) bits |= GL_COLOR_BUFFER_BIT;
        if(mask & PDI_DEPTH_BUFFER) bits |= GL_DEPTH_BUFFER_BIT;
        glClear(bits);
    }

    // matrix stack
    void SetMatrixMode(MatrixMode mode)
    {
        mMatrixMode = mode;
        switch (mode)
        {
        case PDI_MATRIX_MODELVIEW: glMatrixMode(GL_MODELVIEW); break;
        case PDI_MATRIX_PROJECTION: glMatrixMode(GL_PROJECTION); break;
        case PDI_MATRIX_TEXTURE: glMatrixMode(GL_TEXTURE); break;
        }
    }

    void IdentityMatrix() { glLoadIdentity(); }
    void LoadMatrix(const Math::Mat4& matrix) { glLoadMatrixf(matrix.matrix); }
    void PushMatrix() { glPushMatrix(); }
    void PopMatrix() { glPopMatrix(); }
    void MultMatrix(const Math::Mat4& matrix) { glMultMatrixf(matrix.matrix); }

    Math::Mat4 GetMatrix() 
    { 
        Math::Mat4 matrix; 
        switch(mMatrixMode) {
        case PDI_MATRIX_MODELVIEW: glGetFloatv(GL_MODELVIEW_MATRIX, matrix.matrix); break;
        case PDI_MATRIX_PROJECTION: glGetFloatv(GL_PROJECTION_MATRIX, matrix.matrix); break;
        case PDI_MATRIX_TEXTURE: glGetFloatv(GL_TEXTURE_MATRIX, matrix.matrix); break;
        }
        return matrix; 
    }

    void PushIdentityMatrix() { PushMatrix(); IdentityMatrix(); }
    void PushLoadMatrix(const Math::Mat4& matrix) { PushMatrix(); LoadMatrix(matrix); }
    void PushMultMatrix(const Math::Mat4& matrix) { PushMatrix(); MultMatrix(matrix); }

    void SetViewport(float left, float top, float right, float bottom) { 
        mViewportLeft = left; mViewportTop = top; mViewportRight = right; mViewportBottom = bottom;
        glViewport(left, top, right - left, bottom - top);
    }

    void GetViewport(float* left, float* top, float* right, float* bottom) 
    {
        *left = mViewportLeft;
        *top = mViewportTop;
        *right = mViewportRight;
        *bottom = mViewportBottom;
    }

    void SetPerspective(float near, float far, float fov, float aspect)
    {
        MatrixMode previousMode = mMatrixMode;
        SetMatrixMode(PDI_MATRIX_PROJECTION);
        IdentityMatrix();
        float h = 2.0f * 1.0F * (float)tanf(fov * 3.141592654f / 180.0f / 2.0f);
        float w = h * aspect;
        glFrustum(-w / 2.0f, w / 2.0f, -h / 2.0f, h / 2.0f, 1.0F, 4000.0F);
        SetMatrixMode(previousMode);
    }

    void SetOrthographic(float aspect)
    {
        MatrixMode previousMode = mMatrixMode;
        SetMatrixMode(PDI_MATRIX_PROJECTION);
        IdentityMatrix();
        // TODO
        SetMatrixMode(previousMode);
    }

    // lighting
    uint32_t GetMaxLights() { return 8; }

    void SetAmbientLight(Math::Color col)
    {
        mAmbientColor = col;
        for(uint32_t GL_LIGHTi = 0; GL_LIGHTi < GetMaxLights(); GL_LIGHTi++)
        {
            glLightfv(GL_LIGHTi, GL_AMBIENT, col.vector);
        }
    }

    Math::Color GetAmbientLight() { return mAmbientColor; }
    
    void EnableLighting(bool enabled)
    {
        if(enabled) glEnable(GL_LIGHTING);
        else glDisable(GL_LIGHTING);
    }

    void EnableLight(int handle, bool active) 
    { 
        if(active) glEnable(GL_LIGHT0 + handle); 
        else glDisable(GL_LIGHT0 + handle); 
    }

    void SetLight(int handle, LightType type, Math::Color color, Math::Vec3 position, Math::Vec3 direction)
    {
        int GL_LIGHTi = GL_LIGHT0 + handle;
        glEnable(GL_LIGHTi);
        glLightfv(GL_LIGHTi, GL_AMBIENT, mAmbientColor.vector);
        glLightfv(GL_LIGHTi, GL_DIFFUSE, color.vector);
        switch (type)
        {
        case PDI_LIGHT_DIRECTIONAL:
        {
            Math::Vec4 _direction = Math::Vec4(direction, 0.0F);
            glLightfv(GL_LIGHTi, GL_POSITION, _direction.vector);
        }
        break;
        case PDI_LIGHT_POINT:
        {
            Math::Vec4 _position = Math::Vec4(position, 1.0F);
            glLightfv(GL_LIGHTi, GL_POSITION, _position.vector);
        }
        break;
        case PDI_LIGHT_SPOT:
            // TODO
        break;
        }
    }

    // drawing
    void DrawPrimBuffer(Texture* t, PrimBuffer* b)
    {
        PS2Texture* texture = static_cast<PS2Texture*>(t);
        if(texture)
        {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texture->mGLName);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        } else glDisable(GL_TEXTURE_2D);
        if(b->mVertexBuffer)
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(b->mVertexBuffer->mElementSize, GL_FLOAT, 0, b->mVertexBuffer->mData.data());
        } else glDisableClientState(GL_VERTEX_ARRAY);
        if(b->mTextureCoordBuffer)
        {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(b->mTextureCoordBuffer->mElementSize, GL_FLOAT, 0, b->mTextureCoordBuffer->mData.data());
        } else glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        if(b->mNormalBuffer)
        {
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, 0, b->mNormalBuffer->mData.data());
        } else glDisableClientState(GL_NORMAL_ARRAY);
        if(b->mColorBuffer)
        {
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(b->mColorBuffer->mElementSize, GL_FLOAT, 0, b->mColorBuffer->mData.data());
        } else glDisableClientState(GL_COLOR_ARRAY);
        GLenum mode;
        switch (b->type)
        {
            case PRIM_TRIANGLES: mode = GL_TRIANGLES; break;
            case PRIM_TRIANGLE_STRIP: mode = GL_TRIANGLE_STRIP; break;
            case PRIM_QUADS: mode = GL_QUADS; break;
            case PRIM_QUAD_STRIP: mode = GL_QUAD_STRIP; break;
            case PRIM_LINES: mode = GL_LINES; break;
            case PRIM_LINE_STRIP: mode = GL_LINE_STRIP; break;
            case PRIM_POINTS: mode = GL_POINTS; break;
        }
        glDrawArrays(mode, 0, b->count);
    }

    // culling 
    void SetCullMode(CullMode mode) 
    { 
        mCullMode = mode;
        switch(mode)
        {
            case PDI_CULL_OFF: glDisable(GL_CULL_FACE); break;
            case PDI_CULL_FRONT: glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); break;
            case PDI_CULL_BACK: glEnable(GL_CULL_FACE); glCullFace(GL_BACK); break;
        }
    }

    CullMode GetCullMode() { return mCullMode; }

    // z-buffer control
    void EnableZBuffer(bool enable)
    {
        mZTest = enable;
        if(enable) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }

    bool IsZBufferEnabled() { return mZTest; }

    void SetZWrite(bool write) 
    {
        mZWrite = write;
        glDepthMask(write);
    }

    bool GetZWrite() { return mZWrite; }

    // polygon fill
    void SetFillMode(FillMode mode)
    {
        mFillMode = mode;
        switch(mode)
        {
            case PDI_FILL: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
            case PDI_LINE: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
            case PDI_POINT: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
        }
    }

    FillMode GetFillMode() { return mFillMode; }

    void DrawSync() { glFlush(); }
};

class PS2Device : public Device
{
public:

    PS2Device()
    {
        *(GIF::Registers::ctrl) = 1; // OSDSYS leaves path 3 busy, so fix that
        pglInit(2 * 1024, 128); // initialize ps2gl, small immediate buffers because we don't use immediate mode
    }

    ~PS2Device()
    {
        pglFinish();
    }

    Display* CreateDisplay()
    {
        
        PS2Display* display = new PS2Display;
        return display;
    }

    RenderContext* CreateRenderContext(Display* d)
    {
        PS2Display* display = static_cast<PS2Display*>(d);
        PS2RenderContext* context = new PS2RenderContext(display);
        return context;
    }

    Texture* CreateTexture(TextureBuffer core) 
    { 
        return new PS2Texture(std::move(core), 0); 
    }

    Texture* CreateTexture(TextureBuffer core, TextureBuffer palette) 
    { 
        return new PS2Texture(std::move(core), std::move(palette), 0);
    }

};

PS2Device* CreatePS2()
{
    return new PS2Device;
}

} // namespace pdi
