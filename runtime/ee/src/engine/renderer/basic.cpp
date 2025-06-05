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

namespace pse
{

void BasicRenderer::Clear(const pse::math::color& color)
{
    gluClearColor(color);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BasicRenderer::DrawPoint(float x, float y, const pse::math::color& color)
{
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_POINTS);
    glColor4fv(color.vector);
    glVertex2f(x, y);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void BasicRenderer::DrawLine(float x1, float y1, float x2, float y2, const pse::math::color& color)
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

void BasicRenderer::DrawRectangle(float x, float y, float width, float height, const pse::math::color& color)
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

void BasicRenderer::FillRectangle(float x, float y, float width, float height, const pse::math::color& color)
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

void BasicRenderer::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, const pse::math::color& color1, const pse::math::color& color2, const pse::math::color& color3)
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

void BasicRenderer::FillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, const pse::math::color& color1, const pse::math::color& color2, const pse::math::color& color3)
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

void BasicRenderer::DrawCircle(float x, float y, float width, float height, const pse::math::color& color)
{
    DrawArc(x, y, width, height, 0.0F, 360.0F, color);
}

void BasicRenderer::FillCircle(float x, float y, float width, float height, const pse::math::color& color)
{
    FillArc(x, y, width, height, 0.0F, 360.0F, color);
}

void BasicRenderer::DrawArc(float x, float y, float width, float height, float angle1, float angle2, const pse::math::color& color)
{
    float radius = (width + height) / 2;
    int n = radius < 48 ? 12 : (radius / 4);
    float m = 2.0f * pse::math::PI / n;

    float cx = x + width / 2.0F;
    float cy = y + height / 2.0F;

    int start = angle1 / 360.0F * n;
    int end = angle2 / 360.0F * n;

    glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINE_STRIP);
    for(int i = start; i <= end; i++) { 
        glColor4fv(color.vector);
        glVertex2f(
            cx + width * pse::math::cos(i * m) / 2.0F, 
            cy - height * pse::math::sin(i * m) / 2.0F
        );
    }
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void BasicRenderer::FillArc(float x, float y, float width, float height, float angle1, float angle2, const pse::math::color& color)
{
    float radius = (width + height) / 2;
    int n = radius < 48 ? 12 : (radius / 4);
    float m = 2.0f * pse::math::PI / n;

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
            cx + width * pse::math::cos(i * m) / 2.0F, 
            cy - height * pse::math::sin(i * m) / 2.0F
        );
    }
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void BasicRenderer::DrawImage(float x, float y, float width, float height, pse::texture* image, const pse::math::color& color)
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

void BasicRenderer::DrawImageRotated(float x, float y, float width, float height, float angle, pse::texture* image, const pse::math::color& color)
{
    glTranslatef(width / 2.0F, -height / 2.0F, 0.0F);
    glRotatef(angle, 0.0F, 0.0F, 1.0F);
    glTranslatef(-width / 2.0F, height / 2.0F, 0.0F);
    DrawImage(x, y, width, height, image, color);
}

} // namespace pse
