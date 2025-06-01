//=============================================================================
// _____     ___ ____          ____   ____   ____
//  ____|   |    ____| |____| |____  |____| |____|
// |     ___|   |____  |    | |____  |    | |
// -----------------------------------------------
// Open Source Memory Allocator for Playstation 2
//
// File:        ps2heap.cpp
//
// Description: Heap Foundation Source
//
//=============================================================================

#include <ps2heap.hpp>

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================

extern size_t GetFreeSize(void);

namespace pse::memory
{

const int g_MaxAllocators = 16;
allocator* g_Allocators[g_MaxAllocators] = { nullptr };
const int g_MaxAllocatorStack = 16;
allocator_id g_AllocatorStack[g_MaxAllocatorStack] = { -1 };
int g_AllocatorStackIndex = 0;
allocator_id g_DefaultAllocator = -1;
out_of_memory_callback_t * g_OutOfMemoryCallback = nullptr;
void* g_OutOfMemoryCallbackArgs = nullptr;

bool g_TrackingEnabled = false;
size_t g_TrackingThreshold = 0;
const char* g_TrackingName = nullptr;

default_allocator g_DefaultAllocatorInstance;

void check_allocator_id(allocator_id alloc)
{
    if (alloc < 0 || alloc >= g_MaxAllocators)
    {
        throw std::out_of_range("invalid allocator id");
    }
}

void check_allocator(allocator_id alloc)
{
    check_allocator_id(alloc);
    if (!g_Allocators[alloc]) throw std::runtime_error("invalid allocator");
}

bool do_tracking(size_t size)
{
    if (!g_TrackingEnabled) return false;
    if (size < g_TrackingThreshold) return false;
    return true;
}

void track_memory(const char* action, size_t size, allocator* alloc)
{
    if (do_tracking(size))
    {
        if(g_TrackingName)
        {
            std::cout << "memory: " << action << " " << size << " bytes on " << alloc->get_name() << " (" << g_TrackingName << ")" << std::endl;
        }
        else
        {
            std::cout << "memory: " << action << " " << size << " bytes on " << alloc->get_name() << std::endl;
        }
    }
}

void initialize()
{
    register_allocator(PSE_ALLOCATOR_DEFAULT, &g_DefaultAllocatorInstance);
    set_default_allocator(PSE_ALLOCATOR_DEFAULT);
    push_allocator(PSE_ALLOCATOR_DEFAULT);
    std::cout << "memory: free memory at startup: " << get_total_free_memory() / 1024 << " KBytes" << std::endl;
}

void terminate()
{
    unregister_allocator(PSE_ALLOCATOR_DEFAULT);
}

void* allocate(allocator_id alloc, size_t size)
{
    check_allocator(alloc);
    allocator* allocator_ptr = g_Allocators[alloc];
    void* ptr =  allocator_ptr->allocate(size);
    if (!ptr)
    {
        out_of_memory_callback(alloc, size);
        throw std::bad_alloc();
    }
    size_t actual_size = allocator_ptr->get_allocation_size(ptr);
    track_memory("allocated", actual_size, allocator_ptr);
    return ptr;
}

void deallocate(allocator_id alloc, void* ptr)
{
    check_allocator(alloc);
    allocator* allocator_ptr = g_Allocators[alloc];
    size_t actual_size = allocator_ptr->get_allocation_size(ptr);
    allocator_ptr->deallocate(ptr);
    track_memory("freed", actual_size, allocator_ptr);
}

void deallocate(void* ptr)
{
    allocator_id alloc = find_allocator(ptr);
    check_allocator(alloc);
    allocator* allocator_ptr = g_Allocators[alloc];
    size_t actual_size = allocator_ptr->get_allocation_size(ptr);
    allocator_ptr->deallocate(ptr);
    track_memory("freed", actual_size, allocator_ptr);
}

allocator_id get_default_allocator()
{
    return g_DefaultAllocator;
}

void set_default_allocator(allocator_id alloc)
{
    check_allocator(alloc);
    g_DefaultAllocator = alloc;
}

allocator_id find_allocator(void* ptr)
{
    for (int i = 1; i < g_MaxAllocators; ++i)
    {
        if (g_Allocators[i] && g_Allocators[i]->allocated(ptr)) return static_cast<allocator_id>(i);
    }
    // unknown memory must have been mallocated by the system
    return PSE_ALLOCATOR_DEFAULT;
}

allocator_id get_allocator_id(allocator* alloc)
{
    return alloc->get_id();
}

allocator* get_allocator(allocator_id alloc)
{
    check_allocator(alloc);
    return g_Allocators[alloc];
}

void reset_allocator(allocator_id alloc)
{
    check_allocator(alloc);
    g_Allocators[alloc]->reset();
}

void push_allocator(allocator_id alloc)
{
    if (g_AllocatorStackIndex >= g_MaxAllocatorStack) throw std::overflow_error("allocator stack overflow");
    g_AllocatorStack[g_AllocatorStackIndex++] = alloc;
}

allocator_id pop_allocator()
{
    if (g_AllocatorStackIndex <= 0) throw std::underflow_error("allocator stack underflow");
    return g_AllocatorStack[--g_AllocatorStackIndex];
}

allocator_id get_current_allocator()
{
    if (g_AllocatorStackIndex <= 0)
    {
        return g_DefaultAllocator;
    }
    return g_AllocatorStack[g_AllocatorStackIndex - 1];
}

void set_current_allocator(allocator_id alloc)
{
    check_allocator(alloc);
    if (g_AllocatorStackIndex <= 0) g_DefaultAllocator = alloc;
    else g_AllocatorStack[g_AllocatorStackIndex - 1] = alloc;
}

void set_tracking(bool enabled, size_t threshold, const char* name)
{
    g_TrackingEnabled = enabled;
    g_TrackingThreshold = threshold;
    g_TrackingName = name;
    if (enabled) std::cout << "memory: tracking enabled with threshold: " << threshold << " bytes" << std::endl;
    else std::cout << "memory: tracking disabled" << std::endl;
}

void print_statistics()
{
    size_t total_free_memory = 0;
    size_t largest_block = 0;
    size_t number_of_allocations = 0;
    size_t peak_usage = 0;
    std::cout << "memory: statistics" << std::endl;
    for (int i = 0; i < g_MaxAllocators; ++i)
    {
        if (g_Allocators[i])
        {
            g_Allocators[i]->get_statistics(total_free_memory, largest_block, number_of_allocations, peak_usage);
            std::cout << "  allocator " << g_Allocators[i]->get_name() << " (" << i << "): "  << std::endl
                      << "    total free memory: " << total_free_memory / 1024 << " kbytes"  << std::endl
                      << "    largest block: " << largest_block / 1024 << " kbytes"  << std::endl
                      << "    number of allocations: " << number_of_allocations  << std::endl
                      << "    peak usage: " << peak_usage / 1024 << " kbytes" << std::endl;
        }
    }
}

size_t get_total_free_memory()
{
    size_t total = 0;
    size_t total_free_memory = 0;
    size_t largest_block = 0;
    size_t number_of_allocations = 0;
    size_t peak_usage = 0;
    for (int i = 0; i < g_MaxAllocators; ++i)
    {
        if (g_Allocators[i])
        {
            g_Allocators[i]->get_statistics(total_free_memory, largest_block, number_of_allocations, peak_usage);
            total += total_free_memory;
        }
    }
    return total;
}

void set_out_of_memory_callback(out_of_memory_callback_t* cb, void* args)
{
    g_OutOfMemoryCallback = cb;
    g_OutOfMemoryCallbackArgs = args;
}

void out_of_memory_callback(allocator_id alloc, size_t size)
{
    if (g_OutOfMemoryCallback) g_OutOfMemoryCallback(alloc, size, g_OutOfMemoryCallbackArgs);
    else
    {
        std::cout << "out of memory in allocator " << alloc << " requested size " << size << std::endl;
        print_statistics();
        throw std::runtime_error("out of memory");
    }
}

void register_allocator(allocator_id alloc, allocator* alloc_ptr)
{
    check_allocator_id(alloc);
    if (g_Allocators[alloc]) throw std::runtime_error("allocator already registered");
    g_Allocators[alloc] = alloc_ptr;
}

allocator* unregister_allocator(allocator_id alloc)
{
    check_allocator_id(alloc);
    if (!g_Allocators[alloc]) throw std::runtime_error("allocator not registered");
    allocator* alloc_ptr = g_Allocators[alloc];
    g_Allocators[alloc] = nullptr;
    return alloc_ptr;
}

void register_pool_allocator(allocator_id id, allocator_id parent, const char* name, size_t blockSize, size_t blockCount)
{
    check_allocator_id(id);
    allocator* allocator = new(allocate(parent, sizeof(pool_allocator))) pool_allocator(id, parent, name, blockSize, blockCount);
    register_allocator(id, allocator);
}

void register_bump_allocator(allocator_id id, allocator_id parent, const char* name, size_t size)
{
    check_allocator_id(id);
    allocator* allocator = new(allocate(parent, sizeof(bump_allocator))) bump_allocator(id, parent, name, size);
    register_allocator(id, allocator);
}

void register_douglea_allocator(allocator_id id, allocator_id parent, const char* name, size_t size)
{
    check_allocator_id(id);
    allocator* allocator = new(allocate(parent, sizeof(douglea_allocator))) douglea_allocator(id, parent, name, size);
    register_allocator(id, allocator);
}

} // namespace pse::memory