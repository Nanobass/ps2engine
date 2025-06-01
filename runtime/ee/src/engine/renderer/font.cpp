//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        font.cpp
//
// Description: 
//
//=============================================================================

#include <engine/renderer/font.hpp>

//========================================
// System Includes
//========================================

#include <fstream>
#include <utility>
#include <memory>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <exception>
#include <sstream>
#include <regex>

//========================================
// Project Includes
//========================================

namespace pse
{

Font* TextRenderer::LoadFont(const memory::name& name, const std::string& fnt, const std::string& img, float size, float lineHeight)
{
    std::unique_ptr<Font> font = std::make_unique<Font>(name);
    font->mFontTexture = mTextureManager->LoadTexture(img, img);
    font->mFontTexture->SetFilter(GL_NEAREST);
    font->mSize = size;
    font->mLineHeight = lineHeight / size;

    glBindTexture(GL_TEXTURE_2D, font->mFontTexture->mGLName);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    float texWidth = font->mFontTexture->GetWidth();
    float texHeight = font->mFontTexture->GetHeight();

    std::ifstream file(fnt);

    std::string line;
    std::getline(file, line); // info...
    std::getline(file, line); // common...
    std::getline(file, line); // page...
    std::getline(file, line); // chars...
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string literal_char; ss >> literal_char;
        if(literal_char.length() != 4 || literal_char != "char") break;

        std::string junk, value;

        int id, x, y, width, height, xoffset, yoffset, xadvance;

        std::getline(ss, junk, '=');
        std::getline(ss, value, ' ');
        id = std::stoi(value);

        std::getline(ss, junk, '=');
        std::getline(ss, value, ' ');
        x = std::stoi(value);

        std::getline(ss, junk, '=');
        std::getline(ss, value, ' ');
        y = std::stoi(value);

        std::getline(ss, junk, '=');
        std::getline(ss, value, ' ');
        width = std::stoi(value);

        std::getline(ss, junk, '=');
        std::getline(ss, value, ' ');
        height = std::stoi(value);

        std::getline(ss, junk, '=');
        std::getline(ss, value, ' ');
        xoffset = std::stoi(value);

        std::getline(ss, junk, '=');
        std::getline(ss, value, ' ');
        yoffset = std::stoi(value);

        std::getline(ss, junk, '=');
        std::getline(ss, value, ' ');
        xadvance = std::stoi(value);

        Glyph& glyph = font->mGlyphs[id];
        glyph.mU1 = (float) x / texWidth;
        glyph.mV1 = (float) y / texHeight;
        glyph.mU2 = glyph.mU1 + (float) width / texWidth;
        glyph.mV2 = glyph.mV1 + (float) height / texHeight;
        glyph.mWidth = (float) width / size;
        glyph.mHeight = (float) height / size;
        glyph.mXOffset = (float) xoffset / size;
        glyph.mYOffset = -(float) yoffset / size - (float) height / size + 1;
        glyph.mXAdvance = (float) xadvance / size;
    }
    
    Font* ret = font.get();
    mFonts[name.mID] = std::move(font);
    return ret;
}

void TextRenderer::DrawString(Font* font, std::string string)
{
    float x = 0.0F, y = 0.0F;

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    font->mFontTexture->Bind();
    
    glBegin(GL_QUADS);
    for(auto chr : string)
    {
        Glyph glyph = font->mGlyphs[chr];

        if(chr == '\n')
        {
            y -= 1.0F;
            x = 0.0F;
        }
        
        float x1 = x + glyph.mXOffset;
        float y1 = y + glyph.mYOffset + glyph.mHeight;
        float x2 = x + glyph.mXOffset + glyph.mWidth;
        float y2 = y + glyph.mYOffset;

        glTexCoord2f(glyph.mU1, glyph.mV1);
        glVertex2f(x1, y1); // TL
        glTexCoord2f(glyph.mU2, glyph.mV1);
        glVertex2f(x2, y1); // TR
        glTexCoord2f(glyph.mU2, glyph.mV2);
        glVertex2f(x2, y2); // BR
        glTexCoord2f(glyph.mU1, glyph.mV2);
        glVertex2f(x1, y2); // BL

        x += glyph.mXAdvance;
    }
    glEnd();
}
    
} // namespace pse
