//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        perfom.cpp
//
// Description: Performance Analyzer over EE SIO
//
//=============================================================================

#include <core/perfmon.hpp>

//========================================
// System Includes
//========================================

//========================================
// PS2SDK Includes
//========================================

#include <sio.h>

//========================================
// Project Includes
//========================================

//========================================
// Definitions
//========================================

namespace pse::perfmon
{

bool gInitialized = false;

void initialize(bool block)
{
    sio_init(38400, 0, 0, 0, 0);
    gInitialized = true;
    messages::initialization msg = messages::initialization();
    post_message(msg, sizeof(msg));
    
}

void terminate()
{
    messages::termination msg = messages::termination();
    post_message(msg, sizeof(msg));
    gInitialized = false;
}

void post_message(message& msg, size_t size)
{
    if (!gInitialized) return;
    sio_write(&msg, size);
}

} // namespace pse::perfmon