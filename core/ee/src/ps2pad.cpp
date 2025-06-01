//=============================================================================
// _____     ___ ____                ____
//  ____|   |    ____| |     |    | |____|
// |     ___|   |____  |____ |____| |    |
// ----------------------------------------
// Open Source Lua Game Engine
//
// File:        pad.cpp
//
// Description: Pad Manager
//
//=============================================================================

#include <ps2pad.hpp>

//========================================
// System Includes
//========================================

#include <libpad.h>

//========================================
// Project Includes
//========================================

namespace Input::Pad {

void PadDevice::Open(int port, int slot)
{
    if(mState == Connected)
    {
        Close();
    }
    mPort = port;
    mSlot = slot;
    mState = Waiting;
    printf("Pad[%d, %d]\n", mPort, mSlot);
    int ret;
    if(!(ret = padPortOpen(mPort, mSlot, mPadBuf)))
    {
        mState = Disconnected;
        printf("Can't Open Pad[%d, %d] ret=%d\n", mPort, mSlot, ret);
    }
    WaitReady();

    int modes = padInfoMode(mPort, mSlot, PAD_MODETABLE, -1);
    if(!modes)
    {
        mState = Disconnected;
        printf("Pad[%d, %d] is not a 'DualShock 2' Controller\n", mPort, mSlot);
    }

    // Verify that the controller has a DUAL SHOCK mode
    int i = 0;
    do {
        if (padInfoMode(mPort, mSlot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK) break;
        i++;
    } while (i < modes);

    if(i >= modes)
    {
        mState = Disconnected;
        printf("Pad[%d, %d] is not a 'DualShock 2' Controller\n", mPort, mSlot);
    }

    // If ExId != 0x0 => This controller has actuator engines
    // This check should always pass if the Dual Shock test above passed
    ret = padInfoMode(mPort, mSlot, PAD_MODECUREXID, 0);
    if(!ret)
    {
        mState = Disconnected;
        printf("Pad[%d, %d] is not a 'DualShock 2' Controller\n", mPort, mSlot);
    }

    printf("Enabling 'DualShock 2' functions\n");

    // When using MMODE_LOCK, user cant change mode with Select button
    padSetMainMode(mPort, mSlot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

    WaitReady();
    if(padInfoPressMode(mPort, mSlot)) printf("'DualShock 2' has Pressure Sensitive Buttons\n");

    WaitReady();
    padEnterPressMode(mPort, mSlot);  // Set pressure sensitive mode

    WaitReady();
    mActuators = padInfoAct(mPort, mSlot, -1, 0);
    printf("Pad[%d, %d] has %d Actuator/s\n", mPort, mSlot, mActuators);
    if (mActuators != 0) {
        mActAlign[0] = 0;  // Enable small engine
        mActAlign[1] = 1;  // Enable big engine
        mActAlign[2] = 0xff;
        mActAlign[3] = 0xff;
        mActAlign[4] = 0xff;
        mActAlign[5] = 0xff;
        WaitReady();
        printf("Pad[%d, %d] align=%d\n", mPort, mSlot, padSetActAlign(mPort, mSlot, mActAlign));
    }
     
    WaitReady();
    printf("Pad[%d, %d] Initialized\n", mPort, mSlot);

    mState = Connected;
}

void PadDevice::Close()
{
    WaitReady();
    padPortClose(mPort, mSlot);
    mState = Disconnected;
}

void PadDevice::Poll()
{
    int ret = padGetState(mPort, mSlot);
    if (ret == PAD_STATE_DISCONN)
    {
        printf("Pad[%d, %d] disconnected\n", mPort, mSlot);
        mState = Disconnected;
    }

    if((ret != PAD_STATE_STABLE) && (ret != PAD_STATE_FINDCTP1)) return;

    ret = padRead(mPort, mSlot, &mPadButtons);
    if(ret) 
    {
        mPadData = 0xFFFF ^ mPadButtons.btns;
        mOldPad = mNewPad;
        mNewPad = mPadData;

        mInputs[Cross] = Input(mPadButtons.cross_p, 0, 255, 0, 16);
        mInputs[Circle] = Input(mPadButtons.circle_p, 0, 255, 0, 16);
        mInputs[Square] = Input(mPadButtons.square_p, 0, 255, 0, 16);
        mInputs[Triangle] = Input(mPadButtons.triangle_p, 0, 255, 0, 16);
        
        mInputs[Start] = Input(mPadData & PAD_START ? 255 : 0, 0, 255, 0, 16);
        mInputs[Select] = Input(mPadData & PAD_SELECT ? 255 : 0, 0, 255, 0, 16);

        mInputs[Left] = Input(mPadButtons.left_p, 0, 255, 0, 16);
        mInputs[Right] = Input(mPadButtons.right_p, 0, 255, 0, 16);
        mInputs[Up] = Input(mPadButtons.up_p, 0, 255, 0, 16);
        mInputs[Down] = Input(mPadButtons.down_p, 0, 255, 0, 16);

        mInputs[L1] = Input(mPadButtons.l1_p, 0, 255, 0, 16);
        mInputs[L2] = Input(mPadButtons.l2_p, 0, 255, 0, 16);
        mInputs[R1] = Input(mPadButtons.r1_p, 0, 255, 0, 16);
        mInputs[R2] = Input(mPadButtons.r2_p, 0, 255, 0, 16);

        mInputs[L3] = Input(mPadData & PAD_L3 ? 255 : 0, 0, 255, 0, 16);
        mInputs[R3] = Input(mPadData & PAD_R3 ? 255 : 0, 0, 255, 0, 16);

        mInputs[DpadX] = Input(-mPadButtons.left_p + mPadButtons.right_p, 0, 255, 0, 16);
        mInputs[DpadY] = Input(-mPadButtons.down_p + mPadButtons.up_p, 0, 255, 0, 16);

        mInputs[LeftX] = Input(mPadButtons.ljoy_h, 127, 128, 16, 32);
        mInputs[LeftY] = Input(mPadButtons.ljoy_v, 127, 128, 16, 32);
        mInputs[RightX] = Input(mPadButtons.rjoy_h, 127, 128, 16, 32);
        mInputs[RightY] = Input(mPadButtons.rjoy_v, 127, 128, 16, 32);

    }
}

ConnectionState PadDevice::State()
{
    return mState;
}

Input PadDevice::GetInput(uint32_t input)
{
    if(input > 0 && input < PadInputCount)
        return mInputs[input];
    return Input();
}

bool PadDevice::InputChanged(uint32_t input)
{
    //if(input > 0 && input < PadInputCount)
      //  return mInputsChanged[input];
    return mNewPad != mOldPad;
}

void PadDevice::SetRumble(uint32_t rumble, float value)
{
    switch(rumble)
    {
        case Small: mActAlign[Small] = (int) (value * 255); break;
        case Big: mActAlign[Big] = (int) (value * 255); break;
        default: break;
    }
    padSetActDirect(mPort, mSlot, mActAlign);
}

void PadDevice::WaitReady()
{
    ConnectionState old = mState;
    mState = Waiting;
    int state, lastState;
    char stateString[16];
    state = padGetState(mPort, mSlot);
    lastState = -1;
    while ((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1)) {
        if (state != lastState) {
            padStateInt2String(state, stateString);
            printf("Pad[%d, %d] state changed: %s\n", mPort, mSlot, stateString);
        }
        lastState = state;
        state = padGetState(mPort, mSlot);
    }
    if (lastState != -1) printf("Pad[%d, %d] is Ready\n", mPort, mSlot);
    mState = old;
}

} // end namespace GameEngine