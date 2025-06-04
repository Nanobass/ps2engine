//=============================================================================
// _____     ___ ____                ____
//  ____|   |    ____| |     |    | |____|
// |     ___|   |____  |____ |____| |    |
// ----------------------------------------
// Open Source Lua Game Engine
//
// File:        input.hpp
//
// Description: Input Manager
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

#include <cinttypes>
#include <core/math.hpp>
#include <libpad.h>

//========================================
// Project Includes
//========================================

namespace Input {

struct Input
{
    float mInputValue;
    bool mPressed;

    Input(const int& value = 127, const int& center = 127, const int& range = 128, const int& deadzone = 16, const int& threshold = 32) 
    {
        int centered = value - center;
        if(abs(centered) < deadzone) centered = 0;
        mPressed = abs(centered) > threshold;
        mInputValue = (float) centered / (float) range;
    }
};

enum ConnectionState
{
    Connected,
    Waiting,
    Disconnected,
};

struct InputDevice
{
    virtual void Poll() = 0;

    virtual ConnectionState State() = 0;

    virtual Input GetInput(uint32_t input) = 0;
    virtual bool InputChanged(uint32_t input) = 0;

    virtual void SetRumble(uint32_t rumble, float value) = 0;
};

} // end namespace GameEngine