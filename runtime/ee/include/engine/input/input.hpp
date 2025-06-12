//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
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

#include <stdexcept>
#include <vector>
#include <memory>

//========================================
// PS2SDK Includes
//========================================

#include <libpad.h>

//========================================
// Project Includes
//========================================

#include <core/log.hpp>

namespace pse::input
{

enum input_error
{
    kDisconnected, kNoDevice, kInvalidDevice, kSyncError, kDriverError
};

class input_exception : public std::exception
{
public:
    input_exception(input_error error) : mInputError(error) {}
    input_error mInputError;
    const char* what() { return "input error"; }
};

class input_device
{
public:
    virtual void poll() = 0;
    virtual const std::string& get_name() = 0;
    virtual float get_axis(int id) = 0;
    virtual bool get_button(int id) = 0;
    virtual void vibrate(int id, int value) = 0;
};

class input_manager
{
public:
    input_manager();
    ~input_manager();

    void detect_controllers();
    void poll_controllers();
    int get_number_of_controllers();
    input_device& get_controller(int i);
    
private:
    std::vector<std::unique_ptr<input_device>> mDevices;
};
    
} // namespace pse::input
