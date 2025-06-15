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

/** 
 * @brief Formatted output stream
 * 
 * This function formats the output stream with the given format string and arguments.
 * It uses vsnprintf to format the string and outputs it to the provided ostream.
 * 
 * @param os The output stream to write to
 * @param fmt The format string
 * @param ... The variable arguments for formatting
 * @return std::ostream& The formatted output stream
 */
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

/**
 * @brief Log Device
 * log_device is used to specify the output device for logging
 * kIOP is the IOP's stdlib printf (default)
 * kEE is the EE SIO output (cannot be used with perfmon)
 */
enum log_device
{
    kIOP, kEE
};

/**
 * @brief Log Level
 * log_level is used to specify the severity of the log message
 * kInfo is for informational messages
 * kWarn is for warning messages
 * kError is for error messages
 * kDebug is for debug messages, do not use in production code
 */
enum log_level
{
    kInfo, kWarn, kError, kDebug
};

/** 
 * @brief Initialize the log system
 * 
 * @param device The log device to use, default is kIOP
 * 
 * This function initializes the log system and sets the default log device.
 * It also enables the default log levels (kInfo, kWarn, kError, kDebug).
 */
void initialize(log_device device = kIOP);

/**
 * @brief Terminate the log system
 * 
 * This function terminates the log system and performs any necessary cleanup.
 */
void terminate();

/**
 * @brief Enable a specific log level
 * 
 * @param level The log level to enable
 * 
 * This function enables the specified log level, allowing messages of that level to be logged.
 */
void enable_log_level(log_level level);

/**
 * @brief Disable a specific log level
 * 
 * @param level The log level to disable
 * 
 * This function disables the specified log level, preventing messages of that level from being logged.
 */
void disable_log_level(log_level level);

/**
 * @brief Set the enabled state of a specific log level
 * 
 * @param level The log level to set
 * @param enabled Whether to enable or disable the log level
 * 
 * This function sets the enabled state of the specified log level.
 */
void set_log_level_enabled(log_level level, bool enabled);

/**
 * @brief Check if a specific log level is enabled
 * 
 * @param level The log level to check
 * @return true if the log level is enabled, false otherwise
 * 
 * This function checks if the specified log level is currently enabled.
 */
bool is_log_level_enabled(log_level level);

/**
 * @brief Get the output stream for a specific log level
 * 
 * @param level The log level for which to get the output stream
 * @param file The source file name (default is __builtin_FILE())
 * @param line The source line number (default is __builtin_LINE())
 * @return std::ostream& The output stream for the specified log level
 * 
 * This function returns an output stream that can be used to log messages at the specified log level.
 * If the log level is not enabled, it returns a null output stream.
 */
std::ostream& out(log_level level, const char* file = __builtin_FILE(), const int line = __builtin_LINE());

} // namespace pse::log
