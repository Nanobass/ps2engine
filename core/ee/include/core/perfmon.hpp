//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        perfmon.hpp
//
// Description: Performance Analyzer over EE SIO
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

#include <cstdint>
#include <cstddef>

//========================================
// PS2SDK Includes
//========================================

//========================================
// Project Includes
//========================================

namespace pse::perfmon
{

/** 
 * @brief Message types for performance monitoring
 * 
 * This enum defines the different types of messages that can be sent for performance monitoring.
 * Each message type corresponds to a specific event or action in the system, keep them here, so the
 * PC Software can include this header and know what to expect.
 */
enum message_type : uint32_t
{
    kInitialization = 0x00000001,
    kTermination = 0x00000002,
    kHeartbeat = 0x00000003,

    kMemoryAllocation = 0x00001001,
    kMemoryDeallocation = 0x00001002
};

/** 
 * @brief Base class for performance monitoring messages
 * 
 * This class serves as the base class for all messages used in performance monitoring.
 */
struct message
{
    message_type mType;
    message(message_type type) : mType(type) {}
    virtual ~message() = default;
};

namespace messages
{

/** 
 * @brief Initialization message
 * This message is sent when the performance monitoring system is initialized.
 */
struct initialization : public message
{
    initialization() : message(kInitialization) {}
};

/** 
 * @brief Termination message
 * This message is sent when the performance monitoring system is terminated.
 */
struct termination : public message
{
    termination() : message(kTermination) {}
};

/** 
 * @brief Heartbeat message
 * This message is sent periodically to indicate that the system is still running.
 */
struct heartbeat : public message
{
    int mFrameCount;
    heartbeat(int frameCount) : message(kHeartbeat), mFrameCount(frameCount) {}
};
    
} // namespace messages

/**
 * @brief Initialize the performance monitoring system
 * @param block If true, the initialization will block until complete
 * This function initializes the performance monitoring system, setting up the necessary communication
 * channels and preparing for message handling. If `block` is true, it will wait until the PC Software is ready
 * to receive messages.
 */
void initialize(bool block = false);

/**
 * @brief Terminate the performance monitoring system
 * This function terminates the performance monitoring system, cleaning up resources and stopping message handling.
 * It sends a termination message to indicate that the system is shutting down.
 */
void terminate();

/**
 * @brief Post a message to the performance monitoring system
 * @param msg The message to post
 * @param size The size of the message in bytes
 * This function sends a message to the performance monitoring system. It checks if the system is initialized
 * before attempting to send the message.
 */
void post_message(message& msg, size_t size);
    
} // namespace pse::perfmon
