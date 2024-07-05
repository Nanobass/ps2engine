//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        memory.hpp
//
// Description: Heap Foundation Header
//
// Sub System:  Heap Foundation
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

/* standard library */
#include <math.h>
#include <cstddef>
#include <cstdint>
#include <malloc.h>

//========================================
// Project Includes
//========================================

// define memory 'areas' to group allocations

enum GameMemoryAllocator
{
    GMA_DEFAULT = 0,          // for regular allocations
    GMA_STATIC,               // for static allocations, persistant for duration of program
    GMA_GRAPHICS,             // for real3d
    GMA_FILE_IO,              // for real3d file io
    NUM_GAME_MEMORY_ALLOCATORS
};

// route allocations through our memory system

void* operator new (size_t size, GameMemoryAllocator allocator)
{
    return memalign(16, size);
}

void* operator new[] (size_t size, GameMemoryAllocator allocator)
{
    return memalign(16, size);
}

void* operator new (size_t size)
{
    return memalign(16, size);
}

void* operator new[] (size_t size)
{
    return memalign(16, size);
}

void operator delete (void* ptr)
{
    free(ptr);
}

void operator delete[] (void* ptr)
{
    free(ptr);
}