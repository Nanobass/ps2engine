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

struct basic_renderer
{
    void clear(const math::color& color);

    void draw_point(float x, float y, const math::color& color);

    void draw_line(float x1, float y1, float x2, float y2, const math::color& color);

    void draw_rectangle(float x, float y, float width, float height, const math::color& color);
    void fill_rectangle(float x, float y, float width, float height, const math::color& color);

    void draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, const math::color& color1, const math::color& color2, const math::color& color3);
    void fill_triangle(float x1, float y1, float x2, float y2, float x3, float y3, const math::color& color1, const math::color& color2, const math::color& color3);

    void draw_circle(float x, float y, float width, float height, const math::color& color);
    void fill_circle(float x, float y, float width, float height, const math::color& color);

    void draw_arc(float x, float y, float width, float height, float angle1, float angle2, const math::color& color);
    void fill_arc(float x, float y, float width, float height, float angle1, float angle2, const math::color& color);

    void draw_image(float x, float y, float width, float height, texture_ptr image, const math::color& color = math::color());
    void draw_image_rotated(float x, float y, float width, float height, float angle, texture_ptr image, const math::color& color = math::color());

};
    
} // namespace pse