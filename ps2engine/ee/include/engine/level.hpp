//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        level.hpp
//
// Description: Level halt ne
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================

#include <ps2math.hpp>

#include <memory/memory.hpp>
#include <memory/buffer.hpp>

namespace Engine
{

struct LevelCamera
{

};

struct LevelObject
{
    Math::Vec3 mPosition, mRotation, mScale;
    Math::Mat4 mModelMatrix;

    void InvalidatePosition()
    {
        mModelMatrix = Math::Transformation(mPosition, mRotation, mScale);
    }
};

struct Level
{

};
    
} // namespace Engine
