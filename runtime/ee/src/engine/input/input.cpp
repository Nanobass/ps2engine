//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        input.cpp
//
// Description: Input Manager
//
//=============================================================================

#include <engine/input/input.hpp>

//========================================
// System Includes
//========================================

#include <algorithm>

//========================================
// PS2SDK Includes
//========================================

//========================================
// Project Includes
//========================================

#include <engine/input/padman.hpp>

//========================================
// Definitions
//========================================

namespace pse::input
{

input_manager::input_manager()
{

}

input_manager::~input_manager()
{
    mDevices.clear();
}

void input_manager::detect_controllers()
{
    bool padmanActivePads[8];
    for(auto& device : mDevices) {
        if(padman_device* padmanDevice = dynamic_cast<padman_device*>(device.get())) {
            padmanActivePads[padmanDevice->mSlot*4+padmanDevice->mPort] = true;
        }
    }
    for(int i = 0; i < 1; i++) {
        if(!padmanActivePads[i]) {
            try {
                padman_device* padmanDevice = new padman_device(i / 4, i % 4);
                mDevices.emplace_back(padmanDevice);
            } catch(const input_exception& ie) {
                switch (ie.mInputError)
                {
                case kDisconnected:
                case kNoDevice:
                case kInvalidDevice:
                case kSyncError:
                default:
                    break;
                }
            }
        }
    }
}

void input_manager::poll_controllers()
{
    for(auto& device : mDevices)
    {
        try {
            device->poll();
        } catch(const input_exception& ie) {
             switch (ie.mInputError)
             {
             case kDisconnected: log::out(log::kDebug) << device->get_name() << " disconnected" << std::endl; break;
             case kNoDevice: log::out(log::kDebug) << device->get_name() << " is not present" << std::endl; break;
             case kInvalidDevice: log::out(log::kDebug) << device->get_name() << " is of incorrect type" << std::endl; break;
             case kSyncError: log::out(log::kDebug) << device->get_name() << " desynced" << std::endl; break;
             default: log::out(log::kDebug) << device->get_name() << " had an undefined error" << std::endl; break;
             }
         }
    }
    // auto it = std::remove_if(mDevices.begin(), mDevices.end(), [](auto& device) {
    //     try {
    //         device->poll();
    //         return false;
    //     } catch(const input_exception& ie) {
    //         switch (ie.mInputError)
    //         {
    //         case kDisconnected: log::out(log::kDebug) << device->get_name() << " disconnected" << std::endl; break;
    //         case kNoDevice: log::out(log::kDebug) << device->get_name() << " is not present" << std::endl; break;
    //         case kInvalidDevice: log::out(log::kDebug) << device->get_name() << " is of incorrect type" << std::endl; break;
    //         case kSyncError: log::out(log::kDebug) << device->get_name() << " desynced" << std::endl; break;
    //         default: log::out(log::kDebug) << device->get_name() << " had an undefined error" << std::endl; break;
    //         }
    //         return true;
    //     }
    // });
    // mDevices.erase(it, mDevices.end());
}

int input_manager::get_number_of_controllers()
{
    return mDevices.size();
}

input_device& input_manager::get_controller(int i)
{
    return *mDevices[i];
}
    
} // namespace pse::input
