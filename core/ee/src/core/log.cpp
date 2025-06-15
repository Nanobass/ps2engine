//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        log.cpp
//
// Description: Log System Foundation
//
//=============================================================================

#include <core/log.hpp>

//========================================
// System Includes
//========================================

//========================================
// PS2SDK Includes
//========================================

//========================================
// Project Includes
//========================================

//========================================
// Definitions
//========================================

namespace pse::log
{

int g_LogLevelMask = 0;
null_ostream g_NullOutput;
log_device g_DefaultDevice;

void initialize(log_device device)
{
    g_DefaultDevice = device;
    out(kInfo) << "log initialized" << std::endl;
    enable_log_level(kInfo);
    enable_log_level(kWarn);
    enable_log_level(kError);
    enable_log_level(kDebug);
}

void terminate()
{
    out(kInfo) << "log terminate" << std::endl;
}

void enable_log_level(log_level level)
{
    g_LogLevelMask |= (1 << level);
}

void disable_log_level(log_level level)
{
    g_LogLevelMask &= ~(1 << level);
}

void set_log_level_enabled(log_level level, bool enabled)
{
    if(enabled) enable_log_level(level);
    else disable_log_level(level);
}

bool is_log_level_enabled(log_level level)
{
    return (g_LogLevelMask >> level) & 1;
}

std::ostream& out(log_device device, log_level level, const char* file, const int line)
{
    if(!is_log_level_enabled(level))
    {
        return g_NullOutput;
    }

    // i hate compilers
    std::ostream* osp = &g_NullOutput;
    switch(device)
    {
        case kIOP: osp = &std::cout; break;
        case kEE: /* add ee sio for memory leak server */ ; break;
    }
    std::ostream& os = *osp;
    
    switch(level)
    {
        case kInfo : os << "[INFO ]"; break;
        case kWarn : os << "[WARN ]"; break;
        case kError: os << "[ERROR]"; break;
        case kDebug: os << "[DEBUG]"; break;
    }
    os << " " << file << ":" << line << ":";
    os << " ";
    return os;
}

std::ostream& out(log_level level, const char* file, const int line)
{
    return out(g_DefaultDevice, level, file, line);
}
    
} // namespace pse::log
