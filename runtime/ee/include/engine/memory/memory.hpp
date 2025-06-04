//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        memory.hpp
//
// Description: Memory System
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================

#include <core/memory.hpp>
#include <core/memory.hpp>

namespace pse
{

enum game_memory_allocator : memory::allocator_id
{
    GME_FAST = memory::PSE_ALLOCATOR_END,
    GME_END
};

inline bool g_MemorySystemInitialized = false;

void initialize_memory_system();
    
} // namespace pse

void* operator new(size_t size);
void* operator new(size_t size, std::align_val_t align);

void* operator new[](size_t size);
void* operator new[](size_t size, std::align_val_t align);

void* operator new(size_t size, const pse::memory::allocator_id& alloc);
void* operator new[](size_t size, const pse::memory::allocator_id& alloc);

void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;