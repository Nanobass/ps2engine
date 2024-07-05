//=============================================================================
// _____     ___ ____                ____
//  ____|   |    ____| |     |    | |____|
// |     ___|   |____  |____ |____| |    |
// ----------------------------------------
// Open Source Lua Game Engine
//
// File:        pad.hpp
//
// Description: Pad Manager
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================

#include <ps2input.hpp>

namespace Input::Pad {

enum PadInput
{
    // Face Buttons
    Cross = 0x00, Circle, Square, Triangle, 
    // Center Buttons
    Start, Select,
    // Dpad
    DpadX, DpadY, Up, Down, Left, Right, 
    // Left Stick
    LeftX, LeftY, L3, 
    // Right Stick
    RightX, RightY, R3, 
    // Shoulder Buttons
    R1, R2, L1, L2,
    // Count
    PadInputCount
};

enum PadRumble
{
    Small = 0x0,
    Big,
    // Count
    PadRumbleCount
};

struct PadDevice : InputDevice
{

    void Open(int port, int slot);
    void Close();

    void Poll();

    ConnectionState State();

    Input GetInput(uint32_t input);
    bool InputChanged(uint32_t input);

    void SetRumble(uint32_t rumble, float value);

private:
    char mPadBuf[256] alignas(sizeof(char) * 256);
    char mActAlign[6];
    int mActuators;
    padButtonStatus mPadButtons;
    uint32_t mPadData, mOldPad, mNewPad;

    Input mInputs[PadInputCount];
    bool mInputsChanged[PadInputCount];

    int mPort, mSlot;
    ConnectionState mState = Disconnected;

    void WaitReady();
};

} // end namespace GameEngine