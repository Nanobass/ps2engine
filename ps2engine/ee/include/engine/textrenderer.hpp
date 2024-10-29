//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        textrenderer.hpp
//
// Description: Text Renderer
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
#include <fstream>
#include <utility>
#include <memory>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <exception>
#include <sstream>
#include <regex>

/* ps2sdk */
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

/* ps2stuff */
#include <ps2s/gs.h>

//========================================
// Project Includes
//========================================

/* engine */
#include <engine/texturemanager.hpp>

/* ps2memory */
#include <ps2memory.hpp>

/* ps2glu */
#include <ps2glu.hpp>

/* ps2math */
#include <ps2math.hpp>

namespace Engine
{

struct Glyph {
    float mU1, mV1, mU2, mV2;
    float mWidth, mHeight, mXOffset, mYOffset, mXAdvance;
};

struct Font {
    Texture* mFontTexture;
    std::array<Glyph, 128> mGlyphs;
    float mSize, mLineHeight;
};

struct TextRenderer {
    
    std::map<uint32_t, std::unique_ptr<Font>> mFonts;
    TextureManager* mTextureManager;

    TextRenderer(TextureManager* textureManager) : mTextureManager(textureManager)
    {
        for(auto& e : mFonts)
        {
            Font* font = e.second.get();
            mTextureManager->DeleteTexture(font->mFontTexture->mName);
        }
        mFonts.clear();
    }

    ~TextRenderer()
    {

    }

    Font* LoadFont(uint32_t name, const std::string& fnt, const std::string& img, float size, float lineHeight)
    {
        std::ifstream file(fnt);

        std::unique_ptr<Font> font = std::make_unique<Font>();
        font->mFontTexture = mTextureManager->LoadGsTexture(joaat(img), img);
        font->mSize = size;
        font->mLineHeight = lineHeight / size;

        glBindTexture(GL_TEXTURE_2D, font->mFontTexture->mGLName);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glBindTexture(GL_TEXTURE_2D, 0);

        float texWidth = font->mFontTexture->GetWidth();
        float texHeight = font->mFontTexture->GetHeight();

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
        mFonts[name] = std::move(font);
        return ret;
    }

    void DeleteFont(int name)
    {
        auto it = mFonts.find(name);
        Font* font = it->second.get();
        mTextureManager->DeleteTexture(font->mFontTexture->mName);
        mFonts.erase(it);
    }

    void DrawString(Font* font, std::string string, Math::Color color = Math::Color())
    {
        float x = 0.0F, y = 0.0F;

        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        glBindTexture(GL_TEXTURE_2D, font->mFontTexture->mGLName);
        glBegin(GL_QUADS);
        for(auto chr : string)
        {
            Glyph glyph = font->mGlyphs[chr];
            
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

};
    
} // namespace Engine
