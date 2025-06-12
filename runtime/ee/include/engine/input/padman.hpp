//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        padman.hpp
//
// Description: padman library interface
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

#include <engine/input/input.hpp>

namespace pse::input
{
    
class padman_exception : public input_exception
{
public:
    padman_exception(input_error error, int port, int slot) 
        : input_exception(error), mPort(port), mSlot(slot)
    {}

    int mPort, mSlot;
};

class padman_device : public input_device
{
public:
    padman_device(int port, int slot = 0)
        : mPort(port), mSlot(slot)
    {
        mName = std::string("padman_device: ") + (char)('0' + mPort) + (char)('A' + mSlot);

        if(!padPortOpen(mPort, mSlot, mPadBuffer)) throw padman_exception(kNoDevice, mPort, mSlot);
        sync();

        // check if controller has a dualshock mode
        int i = 0, modes = padInfoMode(mPort, mSlot, PAD_MODETABLE, -1);
        for(;i < modes; i++) {
            if (padInfoMode(mPort, mSlot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK) break;
        }
        if(i >= modes) throw padman_exception(kInvalidDevice, mPort, mSlot);

        // do some more checking
        if(!padInfoMode(mPort, mSlot, PAD_MODECUREXID, 0)) throw padman_exception(kInvalidDevice, mPort, mSlot);
        
        // lock the mode button
        padSetMainMode(mPort, mSlot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);
        sync();

        mActuatorCount = padInfoAct(mPort, mSlot, -1, 0);
        if(mActuatorCount != 0) {
            mActAlign[0] = 0;  // Enable small engine
            mActAlign[1] = 1;  // Enable big engine
            mActAlign[2] = 0xff;
            mActAlign[3] = 0xff;
            mActAlign[4] = 0xff;
            mActAlign[5] = 0xff;
            padSetActAlign(mPort, mSlot, mActAlign);
            sync();
        }

    }

    ~padman_device()
    {
        sync();
        padPortClose(mPort, mSlot);
    }

    const std::string& get_name()
    {
        return mName;
    }

    void poll()
    {
        int ret = padGetState(mPort, mSlot);
        if (ret == PAD_STATE_DISCONN) throw padman_exception(kDisconnected, mPort, mSlot);
        if((ret != PAD_STATE_STABLE) && (ret != PAD_STATE_FINDCTP1)) return;
        ret = padRead(mPort, mSlot, &mPadButtons);
    }

    float get_axis(int id)
    {
        return 0.0F;
    }

    bool get_button(int id)
    {
        return false;
    }

    void vibrate(int id, int value)
    {

    }

    void sync()
    {
        int state = -1;
        int lastState = -1;
        do {
            state = padGetState(mPort, mSlot);
            if (state != lastState) {
                char stateString[16];
                padStateInt2String(state, stateString);
                pse::log::out(pse::log::kDebug) << mName << " state='" << stateString << "'" << std::endl;
            }
            if (state == PAD_STATE_DISCONN) throw padman_exception(kDisconnected, mPort, mSlot);
            if (state == PAD_STATE_ERROR) throw padman_exception(kDriverError, mPort, mSlot);
            lastState = state;
        } while((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1));
        if (lastState == -1) throw padman_exception(kSyncError, mPort, mSlot);
    }

    static bool g_PadmanInitialized;
    static int g_PadmanUsageCount;

    int mPort, mSlot;
    std::string mName;
    padButtonStatus mPadButtons;
    char mPadBuffer[256] __attribute__((aligned(64)));
    int mActuatorCount;
    char mActAlign[6];
};
    
} // namespace pse::input
