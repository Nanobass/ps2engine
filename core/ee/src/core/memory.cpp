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

#include <core/memory.hpp>

//========================================
// System Includes
//========================================

#include <kernel.h>

//========================================
// Project Includes
//========================================

#include <core/perfmon.hpp>

// extern size_t GetFreeSize(void);

namespace pse::perfmon::messages
{

struct allocation : public message
{
    pse::memory::allocator_id mAllocatorId;
    uint32_t mTrackingId = 0;
    size_t mSize;
    void* mPtr;

    allocation(pse::memory::allocator_id alloc, size_t size, void* ptr, uint32_t trackingId = 0)
        : message(kMemoryAllocation), mAllocatorId(alloc), mSize(size), mPtr(ptr), mTrackingId(trackingId) {}
};

struct deallocation : public message
{
    pse::memory::allocator_id mAllocatorId;
    uint32_t mTrackingId = 0;
    size_t mSize;
    void* mPtr;

    deallocation(pse::memory::allocator_id alloc, size_t size, void* ptr, uint32_t trackingId = 0)
        : message(kMemoryDeallocation), mAllocatorId(alloc), mSize(size), mPtr(ptr), mTrackingId(trackingId) {}
};
    
} // namespace pse::perfmon::messages


namespace pse::memory
{

bool g_MemoryFoundationInitialized = false;

const int g_MaxAllocators = 16;
allocator* g_Allocators[g_MaxAllocators] = { nullptr };
const int g_MaxAllocatorStack = 16;
allocator_id g_AllocatorStack[g_MaxAllocatorStack] = { -1 };
int g_AllocatorStackIndex = 0;
allocator_id g_DefaultAllocator = -1;
out_of_memory_callback_t * g_OutOfMemoryCallback = nullptr;
void* g_OutOfMemoryCallbackArgs = nullptr;

bool g_TrackingEnabled = false;
uint32_t g_TrackingId = 0;

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

void initialize()
{
    if(g_MemoryFoundationInitialized) throw std::runtime_error("memory system already initialized");
    register_allocator(PSE_ALLOCATOR_DEFAULT, &g_DefaultAllocatorInstance);
    set_default_allocator(PSE_ALLOCATOR_DEFAULT);
    push_allocator(PSE_ALLOCATOR_DEFAULT);
    pse::log::initialize();
    g_MemoryFoundationInitialized = true;
}

void terminate()
{
    pse::log::terminate();
    g_MemoryFoundationInitialized = false;
    unregister_allocator(PSE_ALLOCATOR_DEFAULT);
}

void out_of_memory_callback(allocator_id alloc, size_t size);

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
    if(g_TrackingEnabled)
    {
        pse::perfmon::messages::allocation msg(alloc, actual_size, ptr, g_TrackingId);
        pse::perfmon::post_message(msg, sizeof(msg));
    }
    return ptr;
}

void* allocate(size_t size)
{
    return allocate(get_current_allocator(), size);
}

void deallocate(allocator_id alloc, void* ptr)
{
    check_allocator(alloc);
    allocator* allocator_ptr = g_Allocators[alloc];
    size_t actual_size = allocator_ptr->get_allocation_size(ptr);
    allocator_ptr->deallocate(ptr);
    if(g_TrackingEnabled)
    {
        pse::perfmon::messages::deallocation msg(alloc, actual_size, ptr, g_TrackingId);
        pse::perfmon::post_message(msg, sizeof(msg));
    }
}

void deallocate(void* ptr)
{
    allocator_id alloc = find_allocator(ptr);
    check_allocator(alloc);
    allocator* allocator_ptr = g_Allocators[alloc];
    size_t actual_size = allocator_ptr->get_allocation_size(ptr);
    allocator_ptr->deallocate(ptr);
    if(g_TrackingEnabled)
    {
        pse::perfmon::messages::deallocation msg(alloc, actual_size, ptr, g_TrackingId);
        pse::perfmon::post_message(msg, sizeof(msg));
    }
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
    // unknown memory must have been malloc't by the system
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

void set_tracking(bool enabled, uint32_t trackingId)
{
    g_TrackingEnabled = enabled;
    g_TrackingId = trackingId;
}

void print_statistics()
{
    size_t total_free_memory = 0;
    size_t largest_block = 0;
    size_t number_of_allocations = 0;
    size_t peak_usage = 0;
    std::ostream& os = log::out(log::kDebug) << "statistics" << std::endl;
    for (int i = 0; i < g_MaxAllocators; ++i)
    {
        if (g_Allocators[i])
        {
            g_Allocators[i]->get_statistics(total_free_memory, largest_block, number_of_allocations, peak_usage);
            os << "  allocator " << g_Allocators[i]->get_name() << " (" << i << "): "  << std::endl;
            os << "    total free memory: " << total_free_memory / 1024 << " kbytes"  << std::endl;
            os << "    largest block: " << largest_block / 1024 << " kbytes"  << std::endl;
            os << "    number of allocations: " << number_of_allocations  << std::endl;
            os << "    peak usage: " << peak_usage / 1024 << " kbytes" << std::endl;
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
        log::out(log::kError) << "out of memory in allocator " << alloc << " requested size " << size << std::endl;
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

uint32_t joaat(const std::string& str)
{
	uint32_t hash = 0;
	for (const char c : str)
	{
		hash += c;
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

} // namespace pse::memory