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

//========================================
// Project Includes
//========================================

/* engine */
#include <engine/texturemanager.hpp>

/* core */
#include <core/log.hpp>
#include <core/math.hpp>
#include <core/memory.hpp>

/* ps2gl */
#include <GL/gl.h>
#include <GL/ps2gl.h>
#include <GL/ps2glu.hpp>

namespace pse
{

struct glyph {
    float mU1, mV1, mU2, mV2;
    float mWidth, mHeight, mXOffset, mYOffset, mXAdvance;
};

struct font {
    memory::name mName;
    texture* mFontTexture;
    std::array<glyph, 128> mGlyphs;
    float mSize, mLineHeight;

    font(memory::name name) 
        : mName(name)
    {}

};

struct text_renderer {
    
    std::map<uuid, std::unique_ptr<font>> mFonts;
    texture_manager* mTextureManager;

    text_renderer(texture_manager* textureManager) : mTextureManager(textureManager)
    {
        for(auto& e : mFonts)
        {
            font* font = e.second.get();
            mTextureManager->delete_texture(font->mFontTexture->mName.mUuid);
        }
        mFonts.clear();
    }

    ~text_renderer()
    {

    }

    void draw_string(font* font, std::string string);

    font* load_font(const memory::name& name, const std::string& fnt, const std::string& img, float size, float lineHeight);

    void delete_font(uint32_t name)
    {
        auto it = mFonts.find(name);
        font* font = it->second.get();
        mTextureManager->delete_texture(font->mFontTexture->mName.mUuid);
        mFonts.erase(it);
    }
};
    
} // namespace pse
