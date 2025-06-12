//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        basic.hpp
//
// Description: 
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================

#include <engine/texturemanager.hpp>

#include <core/memory.hpp>

namespace pse
{

struct BasicRenderer
{
    texture_manager* mTextureManager;

    BasicRenderer(texture_manager* textureManager) 
        :   mTextureManager(textureManager) 
    {}

    void Clear(const math::color& color);

    void DrawPoint(float x, float y, const math::color& color);

    void DrawLine(float x1, float y1, float x2, float y2, const math::color& color);

    void DrawRectangle(float x, float y, float width, float height, const math::color& color);
    void FillRectangle(float x, float y, float width, float height, const math::color& color);

    void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, const math::color& color1, const math::color& color2, const math::color& color3);
    void FillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, const math::color& color1, const math::color& color2, const math::color& color3);

    void DrawCircle(float x, float y, float width, float height, const math::color& color);
    void FillCircle(float x, float y, float width, float height, const math::color& color);

    void DrawArc(float x, float y, float width, float height, float angle1, float angle2, const math::color& color);
    void FillArc(float x, float y, float width, float height, float angle1, float angle2, const math::color& color);

    void DrawImage(float x, float y, float width, float height, texture* image, const math::color& color = math::color());
    void DrawImageRotated(float x, float y, float width, float height, float angle, texture* image, const math::color& color = math::color());

};
    
} // namespace pse
