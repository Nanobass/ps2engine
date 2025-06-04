//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        memory.cpp
//
// Description: 
//
//=============================================================================

#include <engine/memory/memory.hpp>

//========================================
// System Includes
//========================================

#include <kernel.h>

//========================================
// Project Includes
//========================================

//========================================
// Definitions
//========================================

namespace pse
{

void exit_handler()
{
    pse::log::out(pse::log::kError) << "system exit" << std::endl;
    memory::print_statistics();
    memory::terminate();
    g_MemorySystemInitialized = false;
}

void crash_handler()
{
    pse::log::out(pse::log::kError) << "system crash" << std::endl;
    memory::print_statistics();
    pse::log::out(pse::log::kError) << "pausing thread" << std::endl;
    SleepThread();
}

void initialize_memory_system() 
{
    if(g_MemorySystemInitialized) return;
    memory::initialize();
    memory::register_douglea_allocator(GME_FAST, memory::PSE_ALLOCATOR_DEFAULT, "Fast", 1024 * 1024 * 2);
    g_MemorySystemInitialized = true;
    std::atexit(exit_handler);
    std::set_terminate(crash_handler);
}

} // namespace pse

bool g_MemorySystemSafety = false;

bool check_memory_system()
{
    if(g_MemorySystemSafety)
    {
        printf("memory system fallback\n");
        return false;
    }
    if(!pse::g_MemorySystemInitialized)
    {
        g_MemorySystemSafety = true;
        pse::initialize_memory_system();
        g_MemorySystemSafety = false;
    }
    return true;
}

void* operator new(size_t size) 
{
    if(!check_memory_system()) return std::malloc(size);
    void* ptr = pse::memory::allocate(pse::memory::get_current_allocator(), size);
    if (!ptr) throw std::bad_alloc();
    return ptr;
}

void* operator new(size_t size, std::align_val_t align) 
{
    if(!check_memory_system()) return std::malloc(size);
    void* ptr = pse::memory::allocate(pse::memory::get_current_allocator(), size);
    if (!ptr) throw std::bad_alloc();
    return ptr;
}

void* operator new[](size_t size) 
{
    if(!check_memory_system()) return std::malloc(size);
    void* ptr = pse::memory::allocate(pse::memory::get_current_allocator(), size);
    if (!ptr) throw std::bad_alloc();
    return ptr;
}

void* operator new[](size_t size, std::align_val_t align) 
{
    if(!check_memory_system()) return std::malloc(size);
    void* ptr = pse::memory::allocate(pse::memory::get_current_allocator(), size);
    if (!ptr) throw std::bad_alloc();
    return ptr;
}


void* operator new(size_t size, const pse::memory::allocator_id& alloc) 
{
    if(!check_memory_system()) return std::malloc(size);
    void* ptr = pse::memory::allocate(alloc, size);
    if (!ptr) throw std::bad_alloc();
    return ptr;
}

void* operator new[](size_t size, const pse::memory::allocator_id& alloc) 
{
    if(!check_memory_system()) return std::malloc(size);
    void* ptr = pse::memory::allocate(alloc, size);
    if (!ptr) throw std::bad_alloc();
    return ptr;
}

void operator delete(void* ptr) noexcept 
{
    if(!check_memory_system())
    {
        std::free(ptr);
        return;
    }
    pse::memory::deallocate(ptr);
}

void operator delete[](void* ptr) noexcept 
{
    if(!check_memory_system())
    {
        std::free(ptr);
        return;
    }
    pse::memory::deallocate(ptr);
}