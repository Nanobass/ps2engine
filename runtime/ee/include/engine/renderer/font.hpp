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
    memory::resource_id mId;
    texture_ptr mFontTexture;
    std::array<glyph, 128> mGlyphs;
    float mSize, mLineHeight;

    font(memory::resource_id id) 
        : mId(id)
    {}

};

using font_ptr = std::shared_ptr<font>;

class text_renderer;

struct font_deleter
{
    text_renderer* mTextRenderer;
    inline void operator()(font* fnt);
};


struct text_renderer {
    
    std::map<uuid, std::weak_ptr<font>> mFonts;
    texture_manager* mTextureManager;

    text_renderer(texture_manager* textureManager) : mTextureManager(textureManager) {}

    ~text_renderer() {}

    void draw_string(font_ptr font, const std::string& string);

    font_ptr load_font(const memory::resource_id& name, const std::string& fnt, texture_ptr img, float size, float lineHeight);

    font_ptr find_font(const uuid& uuid)
    {
        auto it = mFonts.find(uuid);
        return it->second.lock();
    }

    void delete_font(uint32_t name)
    {
        auto it = mFonts.find(name);
        if(it->second.expired()) mFonts.erase(it);
    }
};

inline void font_deleter::operator()(font* fnt)
{ 
    mTextRenderer->delete_font(fnt->mId.mUuid);
    delete fnt;
}
    
} // namespace pse
