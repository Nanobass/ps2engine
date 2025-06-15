//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        log.hpp
//
// Description: Log System Foundation
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

#include <ostream>
#include <string>
#include <format>
#include <assert.h>
#include <stdarg.h>

//========================================
// PS2SDK Includes
//========================================

//========================================
// Project Includes
//========================================

#include <core/memory.hpp>

inline std::ostream& formatted(std::ostream& os, const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
    char buffer[1024]; // there are better ways (calling printf to get size, but too lazy right now)
	vsnprintf(buffer, sizeof(buffer), fmt, va);
    os << buffer;
	va_end(va);
    return os;
}

#define mAssert(_a) assert(_a);
#define mError(_msg, _args...) formatted(pse::log::out(pse::log::kError), "" _msg, ##_args) << std::endl
#define mErrorIf(_cond, _msg, _args...) if (_cond) mError(_msg, ##_args);
#define mWarn(_msg, _args...) formatted(pse::log::out(pse::log::kWarn), "" _msg, ##_args) << std::endl
#define mWarnIf(_cond, _msg, _args...) if (_cond) mWarn(_msg, ##_args);
#define mDebugPrint(_msg, _args...) formatted(pse::log::out(pse::log::kDebug), "" _msg, ##_args) << std::endl
#define mNotImplemented(_msg, _args...) formatted(pse::log::out(pse::log::kWarn) << "not implemented ", "" _msg, ##_args) << std::endl

#define GL_FUNC_DEBUG(msg, ...)

namespace pse::log
{

class null_streambuf : public std::streambuf
{
    char buf[64];
protected:
    virtual int overflow( int c ) 
    {
        setp(buf, buf + sizeof(buf));
        return (c == traits_type::eof()) ? '\0' : c;
    }
};

class null_ostream : private null_streambuf, public std::ostream
{
public:
    null_ostream() : std::ostream( this ) {}
    null_streambuf* rdbuf() { return this; }
};

enum log_device
{
    kIOP, kEE
};

enum log_level
{
    kInfo, kWarn, kError, kDebug
};
    
void initialize(log_device device = kIOP);
void terminate();

void enable_log_level(log_level level);
void disable_log_level(log_level level);
void set_log_level_enabled(log_level level, bool enabled);
bool is_log_level_enabled(log_level level);

std::ostream& out(log_device device, log_level level, const char* file = __builtin_FILE(), const int line = __builtin_LINE());
std::ostream& out(log_level level, const char* file = __builtin_FILE(), const int line = __builtin_LINE());

} // namespace pse::log
