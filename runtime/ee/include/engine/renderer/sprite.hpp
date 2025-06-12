//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        sprite.hpp
//
// Description: Sprite Renderer
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

//========================================
// PS2SDK Includes
//========================================

//========================================
// Project Includes
//========================================

#include <engine/texturemanager.hpp>

#include <core/memory.hpp>

namespace pse
{

struct sprite_renderer
{
    texture_manager* mTextureManager;
    std::unique_ptr<material> mMaterial;

    sprite_renderer(texture_manager* textureManager) 
        :   mTextureManager(textureManager)
    {
        mMaterial = std::make_unique<material>(math::color(1.0F, 1.0F, 1.0F, 1.0F), math::color(0.0F, 0.0F, 0.0F, 1.0F), math::color(0.0F, 0.0F, 0.0F, 1.0F));
    }

    void render_sprite(texture* texture, const math::texel& offset, const math::texel& scale, const math::color& tint = math::color(1.0F, 1.0F, 1.0F, 1.0F))
    {
        texture->bind();
        mMaterial->mAmbient = tint;
        mMaterial->apply();
        
        math::texel tTL = offset;
        math::texel tBR = offset + scale;
        math::texel tTR = math::texel(tBR.x, tTL.y);
        math::texel tBL = math::texel(tTL.x, tBR.y);
        
        glBegin(GL_QUADS);
        
        glColor4fv(tint.vector);
        glTexCoord2f(tBL.u, tBL.v);
        glVertex2f(0.0F, 0.0F);
        
        glColor4fv(tint.vector);
        glTexCoord2f(tTL.u, tTL.v);
        glVertex2f(0.0F, 1.0F);
        
        glColor4fv(tint.vector);
        glTexCoord2f(tTR.u, tTR.v);
        glVertex2f(1.0F, 1.0F);
        
        glColor4fv(tint.vector);
        glTexCoord2f(tBR.u, tBR.v);
        glVertex2f(1.0F, 0.0F);

        glEnd();
    }

};
    
} // namespace pse
