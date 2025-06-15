//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        basic.cpp
//
// Description: 
//
//=============================================================================

#include <engine/renderer/basic.hpp>

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================

#include <GL/ps2gl.h>
#include <GL/gl.h>

namespace pse
{

void basic_renderer::clear(const math::color& color)
{
    pglClearColor(color);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void basic_renderer::draw_point(float x, float y, const math::color& color)
{
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_POINTS);
    glColor4fv(color.vector);
    glVertex2f(x, y);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void basic_renderer::draw_line(float x1, float y1, float x2, float y2, const math::color& color)
{
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINES);
    glColor4fv(color.vector);
    glVertex2f(x1, y1);
    glColor4fv(color.vector);
    glVertex2f(x2, y2);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void basic_renderer::draw_rectangle(float x, float y, float width, float height, const math::color& color)
{
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINES);
    glColor4fv(color.vector);
    glVertex2f(x, y);
    glColor4fv(color.vector);
    glVertex2f(x + width, y);
    glColor4fv(color.vector);
    glVertex2f(x + width, y);
    glColor4fv(color.vector);
    glVertex2f(x + width, y - height);
    glColor4fv(color.vector);
    glVertex2f(x + width, y - height);
    glColor4fv(color.vector);
    glVertex2f(x, y - height);
    glColor4fv(color.vector);
    glVertex2f(x, y - height);
    glColor4fv(color.vector);
    glVertex2f(x, y);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void basic_renderer::fill_rectangle(float x, float y, float width, float height, const math::color& color)
{
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glColor4fv(color.vector);
    glVertex2f(x, y);
    glColor4fv(color.vector);
    glVertex2f(x + width, y);
    glColor4fv(color.vector);
    glVertex2f(x + width, y - height);
    glColor4fv(color.vector);
    glVertex2f(x, y - height);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void basic_renderer::draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, const math::color& color1, const math::color& color2, const math::color& color3)
{
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINES);
    glColor4fv(color1.vector);
    glVertex2f(x1, y1);
    glColor4fv(color2.vector);
    glVertex2f(x2, y2);
    glColor4fv(color2.vector);
    glVertex2f(x2, y2);
    glColor4fv(color3.vector);
    glVertex2f(x3, y3);
    glColor4fv(color3.vector);
    glVertex2f(x3, y3);
    glColor4fv(color1.vector);
    glVertex2f(x1, y1);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void basic_renderer::fill_triangle(float x1, float y1, float x2, float y2, float x3, float y3, const math::color& color1, const math::color& color2, const math::color& color3)
{
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_TRIANGLES);
    glColor4fv(color1.vector);
    glVertex2f(x1, y1);
    glColor4fv(color2.vector);
    glVertex2f(x2, y2);
    glColor4fv(color3.vector);
    glVertex2f(x3, y3);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void basic_renderer::draw_circle(float x, float y, float width, float height, const math::color& color)
{
    draw_arc(x, y, width, height, 0.0F, 360.0F, color);
}

void basic_renderer::fill_circle(float x, float y, float width, float height, const math::color& color)
{
    fill_arc(x, y, width, height, 0.0F, 360.0F, color);
}

void basic_renderer::draw_arc(float x, float y, float width, float height, float angle1, float angle2, const math::color& color)
{
    float radius = (width + height) / 2;
    int n = radius < 48 ? 12 : (radius / 4);
    float m = 2.0f * math::PI / n;

    float cx = x + width / 2.0F;
    float cy = y + height / 2.0F;

    int start = angle1 / 360.0F * n;
    int end = angle2 / 360.0F * n;

    glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINE_STRIP);
    for(int i = start; i <= end; i++) { 
        glColor4fv(color.vector);
        glVertex2f(
            cx + width * math::cos(i * m) / 2.0F, 
            cy - height * math::sin(i * m) / 2.0F
        );
    }
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void basic_renderer::fill_arc(float x, float y, float width, float height, float angle1, float angle2, const math::color& color)
{
    float radius = (width + height) / 2;
    int n = radius < 48 ? 12 : (radius / 4);
    float m = 2.0f * math::PI / n;

    float cx = x + width / 2.0F;
    float cy = y + height / 2.0F;

    int start = angle1 / 360.0F * n;
    int end = angle2 / 360.0F * n;

    glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINE_STRIP);
    glColor4fv(color.vector);
    glVertex2f(cx, cy);
    for(int i = start; i <= end; i++) { 
        glColor4fv(color.vector);
        glVertex2f(
            cx + width * math::cos(i * m) / 2.0F, 
            cy - height * math::sin(i * m) / 2.0F
        );
    }
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void basic_renderer::draw_image(float x, float y, float width, float height, texture_ptr image, const math::color& color)
{
    image->bind();
    glBegin(GL_QUADS);
    glColor4fv(color.vector);
    glTexCoord2f(0.0F, 0.0F);
    glVertex2f(x, y);
    glColor4fv(color.vector);
    glTexCoord2f(1.0F, 0.0F);
    glVertex2f(x + width, y);
    glColor4fv(color.vector);
    glTexCoord2f(1.0F, 1.0F);
    glVertex2f(x + width, y - height);
    glColor4fv(color.vector);
    glTexCoord2f(0.0F, 1.0F);
    glVertex2f(x, y - height);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void basic_renderer::draw_image_rotated(float x, float y, float width, float height, float angle, texture_ptr image, const math::color& color)
{
    glTranslatef(width / 2.0F, -height / 2.0F, 0.0F);
    glRotatef(angle, 0.0F, 0.0F, 1.0F);
    glTranslatef(-width / 2.0F, height / 2.0F, 0.0F);
    draw_image(x, y, width, height, image, color);
}

} // namespace pse