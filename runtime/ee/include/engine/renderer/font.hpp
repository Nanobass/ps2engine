//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        font.hpp
//
// Description: Text Renderer
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

/* standard library */
#include <cstddef>
#include <cstdint>
#include <map>
#include <array>

/* ps2gl */
#include <GL/gl.h>

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

namespace pse
{

struct Glyph {
    float mU1, mV1, mU2, mV2;
    float mWidth, mHeight, mXOffset, mYOffset, mXAdvance;
};

struct Font {
    memory::name mName;
    Texture* mFontTexture;
    std::array<Glyph, 128> mGlyphs;
    float mSize, mLineHeight;

    Font(memory::name name) 
        : mName(name)
    {}

};

struct TextRenderer {
    
    std::map<uint32_t, std::unique_ptr<Font>> mFonts;
    TextureManager* mTextureManager;

    TextRenderer(TextureManager* textureManager) : mTextureManager(textureManager)
    {
        for(auto& e : mFonts)
        {
            Font* font = e.second.get();
            mTextureManager->DeleteTexture(font->mFontTexture->mName.mID);
        }
        mFonts.clear();
    }

    ~TextRenderer()
    {

    }

    void DrawString(Font* font, std::string string);

    Font* LoadFont(const memory::name& name, const std::string& fnt, const std::string& img, float size, float lineHeight);

    void DeleteFont(uint32_t name)
    {
        auto it = mFonts.find(name);
        Font* font = it->second.get();
        mTextureManager->DeleteTexture(font->mFontTexture->mName.mID);
        mFonts.erase(it);
    }
};
    
} // namespace pse
