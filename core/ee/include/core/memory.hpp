//=============================================================================
// _____     ___ ____          ____   ____   ____
//  ____|   |    ____| |____| |____  |____| |____|
// |     ___|   |____  |    | |____  |    | |
// -----------------------------------------------
// Open Source Memory Allocator for Playstation 2
//
// File:        core/memory.hpp
//
// Description: Memory System Foundation
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

#include <cstddef>
#include <cstdint>
#include <string>
#include <string.h>
#include <malloc.h>
#include <stdexcept>
#include <iostream>
#include <utility>

//========================================
// PS2SDK Includes
//========================================

//========================================
// Project Includes
//========================================

#include <core/log.hpp>

//========================================
// Externs
//========================================

extern "C" {
    extern unsigned int sz_MallocState;
    extern void* pAv_;
    extern unsigned int* g_pSBrkPointer;
    extern unsigned int g_StartOfMemory;
    extern unsigned int g_EndOfMemory;

    extern void* dlmalloc(size_t size);
    extern void dlfree(void* ptr);
    extern size_t dlmalloc_usable_size(void* ptr);
}

namespace pse::memory
{

/** 
 * @brief Allocator ID type
 * 
 * This type is used to identify different allocators in the memory system.
 * It is an integer type that can be used to reference specific allocators.
 */
typedef int allocator_id;

/** 
 * @brief Internal Allocators
 * 
 * This enumeration defines the IDs for the default allocators used in the memory system.
 * The IDs are used to identify and manage different allocators.
 */
enum pse_allocators : allocator_id
{
    PSE_ALLOCATOR_DEFAULT = 0, /* system malloc wrapper */
    PSE_ALLOCATOR_END
};

struct allocator;
struct dlheap;

/**
 * @brief Initialize the memory system
 * 
 * This function initializes the memory system, setting up the default allocator and preparing for memory management.
 * It should be called before any memory allocation or deallocation operations.
 * 
 * add a function like this to the application code to initialize the memory system:
 * bool g_MemorySystemSafety = false;
 * 
 * bool check_memory_system()
 * {
 *     if(g_MemorySystemSafety)
 *     {
 *        // this is a safety check to prevent recursive initialization
 *         printf("memory system fallback\n");
 *         return false;
 *     }
 *     if(!pse::g_MemorySystemInitialized)
 *     {
 *         g_MemorySystemSafety = true;
 *         pse::initialize_memory_system();
 *         g_MemorySystemSafety = false;
 *     }
 *     return true;
 * }
 * and in your new operator overloads, you can check if the memory system is initialized:
 * void* operator new(size_t size) 
 * {
 *    if(!check_memory_system()) return std::malloc(size);
 *    void* ptr = pse::memory::allocate(pse::memory::get_current_allocator(), size);
 *    if (!ptr) throw std::bad_alloc();
 *    return ptr;
 * }
 * 
 * and in your delete operator overloads, you can check if the memory system is initialized:
 * void operator delete(void* ptr) noexcept 
 * {
 *     if(!check_memory_system())
 *     {
 *         std::free(ptr);
 *         return;
 *     }
 *     pse::memory::deallocate(ptr);
 * }
 * 
 * @note This function should be called only once, and it is not thread-safe.
 * @throws std::runtime_error if the memory system is already initialized.
 * @see terminate() for cleaning up the memory system.
 * 
 */
void initialize();

/**
 * @brief Terminate the memory system
 * 
 * This function terminates the memory system, cleaning up resources and unregistering allocators.
 * It should be called when the application is done with memory management.
 * 
 * @note This function should be called only once, and it is not thread-safe.
 * @see initialize() for setting up the memory system.
 * 
 */
void terminate();

/**
 * @brief Allocate memory using a specific allocator
 * @param alloc The allocator ID to use for allocation
 * @param size The size of memory to allocate in bytes
 */
void* allocate(allocator_id alloc, size_t size);

/**
 * @brief Allocate memory using the current allocator
 * @param size The size of memory to allocate in bytes
 */
void* allocate(size_t size);

/**
 * @brief Deallocate memory using a specific allocator
 * @param alloc The allocator ID to use for deallocation
 * @param ptr The pointer to the memory to deallocate
 */
void deallocate(allocator_id alloc, void* ptr);

/**
 * @brief Deallocate memory using the default allocator
 * @param ptr The pointer to the memory to deallocate
 * @note this function has to search all allocators to find the one that allocated the memory
 */
void deallocate(void* ptr);

/**
 * @brief Get the default allocator ID
 * 
 * This function returns the ID of the default allocator used in the memory system.
 * 
 * @return The allocator ID of the default allocator.
 */
allocator_id get_default_allocator();

/**
 * @brief Set the default allocator ID
 * 
 * This function sets the default allocator to be used in the memory system.
 * 
 * @param alloc The allocator ID to set as the default allocator.
 * @throws std::runtime_error if the allocator is not registered.
 */
void set_default_allocator(allocator_id alloc);

/**
 * @brief Find the allocator ID for a given pointer
 * 
 * This function searches through all registered allocators to find the one that allocated the given pointer.
 * 
 * @param ptr The pointer to search for.
 * @return The allocator ID that allocated the pointer, or PSE_ALLOCATOR_DEFAULT if not found.
 */
allocator_id find_allocator(void* ptr);

/**
 * @brief Get the allocator ID for a given allocator instance
 * 
 * This function retrieves the allocator ID associated with a specific allocator instance.
 * 
 * @param alloc The allocator instance to get the ID for.
 * @return The allocator ID of the given allocator instance.
 */
allocator_id get_allocator_id(allocator* alloc);

/**
 * @brief Get the allocator instance for a given allocator ID
 * 
 * This function retrieves the allocator instance associated with a specific allocator ID.
 * 
 * @param alloc The allocator ID to get the instance for.
 * @return The allocator instance associated with the given ID.
 * @throws std::runtime_error if the allocator is not registered.
 */
allocator* get_allocator(allocator_id alloc);

/**
 * @brief Reset the allocator
 * 
 * This function resets allocators like bump_alllocator to their initial state.
 * 
 * @param alloc The allocator ID to reset.
 * @throws std::runtime_error if the allocator is not registered.
 */
void reset_allocator(allocator_id alloc);

/**
 * @brief Push an allocator onto the stack
 * 
 * This function pushes an allocator ID onto the allocator stack, allowing for nested allocations.
 * 
 * @param alloc The allocator ID to push onto the stack.
 * @throws std::overflow_error if the stack is full.
 */
void push_allocator(allocator_id alloc);

/**
 * @brief Pop an allocator from the stack
 * 
 * This function pops the top allocator ID from the allocator stack, returning it for use.
 * 
 * @return The allocator ID that was popped from the stack.
 * @throws std::underflow_error if the stack is empty.
 */
allocator_id pop_allocator();

/**
 * @brief Get the current allocator ID
 * 
 * This function retrieves the current allocator ID from the allocator stack.
 * 
 * @return The allocator ID of the current allocator.
 */
allocator_id get_current_allocator();

/**
 * @brief Set the current allocator ID
 * 
 * This function sets the current allocator ID on the allocator stack.
 * 
 * @param alloc The allocator ID to set as the current allocator.
 * @throws std::runtime_error if the allocator is not registered.
 */
void set_current_allocator(allocator_id alloc);


void set_tracking(bool enabled, uint32_t trackingId);

/**
 * @brief Print memory statistics
 * 
 * This function prints the memory statistics for all registered allocators, including total free memory,
 * largest block, number of allocations, and peak usage.
 */
void print_statistics();

/**
 * @brief Get the total free memory across all allocators
 * 
 * This function calculates and returns the total free memory available across all registered allocators.
 * 
 * @return The total free memory in bytes.
 */
size_t get_total_free_memory();

/**
 * @brief Callback function type for out of memory conditions
 * This type defines the signature for a callback function that will be called when an out of memory condition occurs.
 * @param alloc The allocator ID that encountered the out of memory condition.
 * @param size The size of memory that was requested but could not be allocated.
 * @param args Additional arguments that can be passed to the callback function.
 */
typedef void (out_of_memory_callback_t)(allocator_id alloc, size_t size, void* args);

/**
 * @brief Set the out of memory callback
 * 
 * This function sets a callback that will be called when an out of memory condition occurs.
 * 
 * @param cb The callback function to set.
 * @param args Additional arguments to pass to the callback.
 */
void set_out_of_memory_callback(out_of_memory_callback_t* cb, void* args);

/**
 * @brief Register an allocator
 * 
 * This function registers an allocator with the memory system, allowing it to be used for memory allocation.
 * 
 * @param alloc The allocator ID to register.
 * @param alloc_ptr The allocator instance to register.
 * @throws std::runtime_error if the allocator is already registered.
 */
void register_allocator(allocator_id alloc, allocator* alloc_ptr);

/**
 * @brief Unregister an allocator
 * 
 * This function unregisters an allocator from the memory system, allowing it to be removed and cleaned up.
 * 
 * @param alloc The allocator ID to unregister.
 * @return The allocator instance that was unregistered.
 * @throws std::runtime_error if the allocator is not registered.
 */
allocator* unregister_allocator(allocator_id alloc);

/**
 * @brief Register a pool allocator
 * 
 * This function registers a pool allocator with the memory system, allowing it to be used for memory allocation.
 * 
 * @param id The allocator ID to register.
 * @param parent The parent allocator ID for this pool allocator.
 * @param name The name of the pool allocator.
 * @param blockSize The size of each block in the pool.
 * @param blockCount The number of blocks in the pool.
 */
void register_pool_allocator(allocator_id id, allocator_id parent, const char* name, size_t blockSize, size_t blockCount);

/**
 * @brief Register a bump allocator
 * 
 * This function registers a bump allocator with the memory system, allowing it to be used for memory allocation.
 * 
 * @param id The allocator ID to register.
 * @param parent The parent allocator ID for this bump allocator.
 * @param name The name of the bump allocator.
 * @param size The size of the memory pool for the bump allocator.
 */
void register_bump_allocator(allocator_id id, allocator_id parent, const char* name, size_t size);

/**
 * @brief Register a douglea allocator
 * 
 * This function registers a douglea allocator with the memory system, allowing it to be used for memory allocation.
 * 
 * @param id The allocator ID to register.
 * @param parent The parent allocator ID for this douglea allocator.
 * @param name The name of the douglea allocator.
 * @param size The size of the memory pool for the douglea allocator.
 */
void register_douglea_allocator(allocator_id id, allocator_id parent, const char* name, size_t size);

/** 
 * @brief Allocator Interface
 * This interface defines the methods that all allocators must implement.
 * It provides a common interface for memory allocation, deallocation, and statistics retrieval.
 */
struct allocator
{
    virtual ~allocator() = default;
    virtual void* allocate(size_t size) = 0;
    virtual void deallocate(void* ptr) = 0;
    virtual bool allocated(void* ptr) = 0;
    virtual void reset() { }
    virtual size_t get_allocation_size(void* ptr) = 0;
    virtual void get_statistics(size_t& total_free_memory, size_t& largest_block, size_t& number_of_allocations, size_t& peak_usage) = 0;
    virtual size_t get_size() = 0;
    virtual allocator_id get_id() const = 0;
    virtual const char* get_name() const = 0;
};

/**
 * @brief Default Allocator
 * 
 * This allocator uses the standard malloc and free functions for memory management.
 */
struct default_allocator : public allocator
{
    void* allocate(size_t size) override 
    { 
        void* ptr = std::malloc(size);
        size_t actual_size = malloc_usable_size(ptr);
        mLargestBlock = std::max(mLargestBlock, actual_size);
        mNumberOfAllocations++;
        mUsed += actual_size;
        mPeakUsage = std::max(mPeakUsage, mUsed);
        return ptr;
    }
    void deallocate(void* ptr) override 
    {
        size_t actual_size = malloc_usable_size(ptr);
        mUsed -= actual_size;
        mNumberOfAllocations--;
        std::free(ptr);
    }
    bool allocated(void* ptr) override 
    { 
        // all non-definable allocations are managed by malloc
        return true; 
    } 
    size_t get_allocation_size(void* ptr) override 
    { 
        return malloc_usable_size(ptr); 
    }
    void get_statistics(size_t& total_free_memory, size_t& largest_block, size_t& number_of_allocations, size_t& peak_usage) override
    {
        total_free_memory = GetFreeSize();
        largest_block = mLargestBlock;
        number_of_allocations = mNumberOfAllocations;
        peak_usage = mPeakUsage;
    }
    size_t get_size() override { return -1; };
    allocator_id get_id() const override { return PSE_ALLOCATOR_DEFAULT; }
    const char* get_name() const override { return "malloc"; }
private:

    void* AllocateLargestFreeBlock(size_t* Size)
    {
    size_t s0, s1;
    void* p;

    s0 = ~(size_t)0 ^ (~(size_t)0 >> 1);

    while (s0 && (p = malloc(s0)) == NULL)
        s0 >>= 1;

    if (p)
        free(p);

    s1 = s0 >> 1;

    while (s1)
    {
        if ((p = malloc(s0 + s1)) != NULL)
        {
        s0 += s1;
        free(p);
        }
        s1 >>= 1;
    }

    while (s0 && (p = malloc(s0)) == NULL)
        s0 ^= s0 & -s0;

    *Size = s0;
    return p;
    }

    size_t GetFreeSize(void)
    {
    size_t total = 0;
    void* pFirst = NULL;
    void* pLast = NULL;

    for (;;)
    {
        size_t largest;
        void* p = AllocateLargestFreeBlock(&largest);

        if (largest < sizeof(void*))
        {
        if (p != NULL)
            free(p);
        break;
        }

        *(void**)p = NULL;

        total += largest;

        if (pFirst == NULL)
        pFirst = p;

        if (pLast != NULL)
        *(void**)pLast = p;

        pLast = p;
    }

    while (pFirst != NULL)
    {
        void* p = *(void**)pFirst;
        free(pFirst);
        pFirst = p;
    }

    return total;
    }

    size_t mLargestBlock = 0;
    size_t mNumberOfAllocations = 0;
    size_t mUsed = 0;
    size_t mPeakUsage = 0;
};

/**
 * @brief Pool Allocator
 * This allocator manages a fixed-size memory pool, allowing for efficient allocation and deallocation of memory blocks of a specific size.
 * It is useful for scenarios where memory usage patterns are predictable and can be optimized for speed.
 */
struct pool_allocator : public allocator
{
    pool_allocator(allocator_id id, allocator_id parent, const char* name, size_t blockSize, size_t mBlockCount)
        : mId(id), mParent(parent), mName(name), mBlockSize(blockSize), mBlockCount(mBlockCount), mUsedBlocks(0), mPeakUsedBlocks(0)
    {
        mMemoryPoolSize = blockSize * mBlockCount;
        mMemoryPool = pse::memory::allocate(mParent, mMemoryPoolSize);
    }

    ~pool_allocator() override
    {
        pse::memory::deallocate(mParent, mMemoryPool);
    }

    void* allocate(size_t size) override 
    {
        if (size > mBlockSize || mUsedBlocks >= mBlockCount) return nullptr;

        
        mUsedBlocks++;
        mPeakUsedBlocks = std::max(mPeakUsedBlocks, mUsedBlocks);

        return nullptr;
    }

    void deallocate(void* ptr) override 
    {
        mUsedBlocks--;
    }

    bool allocated(void* ptr) override 
    { 
        return (uintptr_t) ptr >= (uintptr_t) mMemoryPool && (uintptr_t) ptr < ((uintptr_t) mMemoryPool + mMemoryPoolSize); 
    } 

    size_t get_allocation_size(void* ptr) override
    {
        return mBlockSize;
    }

    void get_statistics(size_t& total_free_memory, size_t& largest_block, size_t& number_of_allocations, size_t& peak_usage) override
    {
        total_free_memory = (mBlockCount - mUsedBlocks) * mBlockSize;
        largest_block = mBlockSize;
        number_of_allocations = mUsedBlocks;
        peak_usage = mPeakUsedBlocks * mBlockSize;
    }

    size_t get_size() override { return mMemoryPoolSize; };
    allocator_id get_id() const override { return mId; }
    const char* get_name() const override { return mName; }

private:

    allocator_id mId;
    allocator_id mParent;
    const char* mName;
    
    void* mMemoryPool;
    size_t mMemoryPoolSize;

    size_t mBlockSize;
    size_t mBlockCount;
    size_t mUsedBlocks;
    size_t mPeakUsedBlocks;
    
};

/**
 * @brief Bump Allocator
 * This allocator provides a simple bump allocation strategy, where memory is allocated in a linear fashion from a pre-allocated memory pool.
 * It is efficient for scenarios where memory allocations are short-lived and can be reset easily.
 * It does not support deallocation of individual blocks, only a reset of the entire pool.
 */
struct bump_allocator : public allocator
{
    bump_allocator(allocator_id id, allocator_id parent, const char* name, size_t size)
        : mId(id), mParent(parent), mName(name), mSize(size), mUsed(0), mLargestBlock(0), mNumberOfAllocations(0), mPeakUsage(0)
    {
        mMemoryPool = pse::memory::allocate(mParent, size);
    }

    ~bump_allocator() override
    {
        pse::memory::deallocate(mParent, mMemoryPool);
    }

    void reset() 
    {
        if(mNumberOfAllocations > 0) 
        {
            throw std::runtime_error("bump heap reset while allocations are still present");
        }
        mUsed = 0;
    }

    void* allocate(size_t size) override 
    {
        if (mUsed + size > mSize) return nullptr;
        void* ptr = (void*) ((uintptr_t) mMemoryPool + mUsed);
        mUsed += size;
        mLargestBlock = std::max(mLargestBlock, size);
        mNumberOfAllocations++;
        mPeakUsage = std::max(mPeakUsage, mUsed);
        return ptr;
    }

    void deallocate(void* ptr) override 
    {
        mNumberOfAllocations--;
    }

    bool allocated(void* ptr) override 
    { 
        return (uintptr_t) ptr >= (uintptr_t) mMemoryPool && (uintptr_t) ptr < ((uintptr_t) mMemoryPool + mSize); 
    } 

    size_t get_allocation_size(void* ptr) override
    {
        return 0;
    }

    void get_statistics(size_t& total_free_memory, size_t& largest_block, size_t& number_of_allocations, size_t& peak_usage) override
    {
        total_free_memory = mSize - mUsed;
        largest_block = mLargestBlock;
        number_of_allocations = mNumberOfAllocations;
        peak_usage = mPeakUsage;
    }

    size_t get_size() override { return mSize; };
    allocator_id get_id() const override { return mId; }
    const char* get_name() const override { return mName; }
private:
    allocator_id mId;
    allocator_id mParent;
    const char* mName;

    void* mMemoryPool;
    size_t mSize;
    size_t mUsed;
    size_t mLargestBlock;
    size_t mNumberOfAllocations;
    size_t mPeakUsage;
};

/**
 * @brief Douglea Allocator
 * This allocator is a wrapper around the dlmalloc library, providing a memory allocation interface that uses the dlmalloc implementation.
 * It is designed to be used in scenarios where a more advanced memory management strategy is required, such as fragmentation handling and efficient memory usage.
 */
struct douglea_allocator : public allocator
{
    douglea_allocator(allocator_id id, allocator_id parent, const char* name, size_t size)
        : mId(id), mParent(parent), mName(name), mSize(size), mUsed(0), mLargestBlock(0), mNumberOfAllocations(0), mPeakUsage(0)
    {
        mMallocState = pse::memory::allocate(mParent, sz_MallocState);
        mStartOfMemory = pse::memory::allocate(mParent, size);
        mEndOfMemory = (void*)((uintptr_t)mStartOfMemory + size);
        mSBrkPointer = mStartOfMemory;
        memset(mMallocState, 0, sz_MallocState);
    }

    ~douglea_allocator() override
    {
        pse::memory::deallocate(mParent, mStartOfMemory);
        pse::memory::deallocate(mParent, mMallocState);
    }

    void set_dlmalloc_state()
    {
        pAv_ = mMallocState;
        g_StartOfMemory = (unsigned int) mStartOfMemory;
        g_EndOfMemory = (unsigned int) mEndOfMemory;
        g_pSBrkPointer = (unsigned int*) &mSBrkPointer;
    }

    void* allocate(size_t size) override
    {
        set_dlmalloc_state();
        void* ptr = dlmalloc( size );
        size_t actual_size = dlmalloc_usable_size(ptr);
        mLargestBlock = std::max(mLargestBlock, actual_size);
        mNumberOfAllocations++;
        mUsed += actual_size;
        mPeakUsage = std::max(mPeakUsage, mUsed);
        return ptr;
    }

    void deallocate(void* ptr) override
    {
        set_dlmalloc_state();
        size_t actual_size = dlmalloc_usable_size(ptr);
        mUsed -= actual_size;
        mNumberOfAllocations--;
        dlfree( ptr );
    }


    bool allocated(void* ptr) override
    {
        return (ptr >= mStartOfMemory && ptr < mEndOfMemory);
    }

    size_t get_allocation_size(void* ptr) override
    {
        set_dlmalloc_state();
        return dlmalloc_usable_size(ptr);
    }

    void get_statistics(size_t& total_free_memory, size_t& largest_block, size_t& number_of_allocations, size_t& peak_usage) override
    {
        total_free_memory = mSize - mUsed;
        largest_block = mLargestBlock;
        number_of_allocations = mNumberOfAllocations;
        peak_usage = mPeakUsage;
    }

    size_t get_size() override { return mSize; };
    allocator_id get_id() const override { return mId; }
    const char* get_name() const override { return mName; }
private:
    allocator_id mId;
    allocator_id mParent;
    const char* mName;

    void* mMallocState;

    void* mStartOfMemory;
    void* mEndOfMemory;
    void* mSBrkPointer;
    size_t mSize;
    size_t mUsed;
    size_t mLargestBlock;
    size_t mNumberOfAllocations;
    size_t mPeakUsage;
};

/**
 * @brief STL Allocator
 * This allocator is a wrapper around the memory system's allocators, providing an interface compatible with the C++ Standard Library's allocator requirements.
 */
template <typename T>
class stl_allocator {
public:
    using value_type = T;

    stl_allocator(allocator_id alloc = PSE_ALLOCATOR_DEFAULT) noexcept : mAlloc(alloc) {}

    template <typename U>
    stl_allocator(const stl_allocator<U>& other) noexcept : mAlloc(other.mAlloc) {}

    T* allocate(std::size_t n) {
        return static_cast<T*>(pse::memory::allocate(mAlloc, n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t) noexcept {
        pse::memory::deallocate(mAlloc, p);
    }

    template <typename U>
    bool operator==(const stl_allocator<U>& other) const noexcept {
        return mAlloc == other.mAlloc;
    }

    template <typename U>
    bool operator!=(const stl_allocator<U>& other) const noexcept {
        return mAlloc != other.mAlloc;
    }

    allocator_id get_allocator_id() const noexcept { return mAlloc; }

private:
    allocator_id mAlloc;
};

} // namespace pse::memory

//========================================
// Types
//========================================

namespace pse
{
/**
 * @brief UUID class
 * 
 * This class represents a Universally Unique Identifier (UUID) as a 32-bit unsigned integer.
 * It provides a simple interface for creating and using UUIDs, with implicit conversion to uint32_t.
 *
 * The UUID is generated using a random number, which is suitable for many applications where a unique identifier is needed.
 * @note this implementation does not guarantee global uniqueness, but it is sufficient for many use cases.
 */
class uuid
{
public:
    uuid() : mUuid(rand()) {}
    uuid(uint32_t uuid) : mUuid(uuid) {}
    uuid(const uuid&) = default;
    operator uint32_t() const { return mUuid; }
private:
    uint32_t mUuid;
};
} // namespace pse

namespace std {
	template <typename T> struct hash;

	template<>
	struct hash<pse::uuid>
	{
		std::size_t operator()(const pse::uuid& uuid) const
		{
			return (uint32_t) uuid;
		}
	};
} // namespace std

namespace pse::memory {

/**
 * @brief Generate a hash value for a string using the joaat algorithm
 * 
 * This function computes a hash value for a given string using the joaat (jenkins one at a time) algorithm.
 * It is used to create unique identifiers for resources based on their names.
 * 
 * @param str The input string to hash.
 * @return The computed hash value as a 32-bit unsigned integer.
 */
uint32_t joaat(const std::string& str);

struct resource_id
{
    std::string mName;
    uuid mUuid;

    resource_id(const std::string& name) 
        : mName(name), mUuid(joaat(name))
    {}

    resource_id(const char* name) 
        : mName(name), mUuid(joaat(name))
    {}

    operator uint32_t() const { return mUuid; }

    friend std::ostream& operator<<(std::ostream& os, const resource_id& id)
    {
        return os << "\"" << id.mName << "\" (" << (uint32_t) id.mUuid << ")";
    }

};

/**
 * @brief Buffer class
 * 
 * This class represents a dynamically allocated buffer for storing elements of type T.
 * It provides an interface for managing the buffer's size, accessing its elements, and iterating over them.
 *
 * similar to std::vector, but without the overhead of dynamic resizing and copying.
 * It is designed for scenarios where a fixed-size buffer is sufficient, and explicit copying is preferred to avoid excessive memory usage.
 * @tparam T The type of elements stored in the buffer.
 */
template <typename T>
class buffer
{
    using type = T;

public:
    
    explicit buffer(std::size_t size, memory::allocator_id alloc = pse::memory::PSE_ALLOCATOR_DEFAULT)
        :   mSize(size), 
            mData((T*)memory::allocate(alloc, size * sizeof(T)))
        {}
    
    ~buffer()
    {
        if(mData) memory::deallocate(mData);
    }

    // no copying, no, ONLY EXPLICIT COPYING, WE DO NOT HAVE GIGABYTES OF SYSTEM MEMORY
    buffer(const buffer& other) = delete;
    buffer& operator=(const buffer& other) = delete;

    // moving is fine and kinda required without copying
    buffer(buffer&& other) noexcept 
        :   mSize(std::exchange(other.mSize, 0)), 
            mData(std::exchange(other.mData, nullptr))
    {}

    buffer& operator=(buffer&& other) noexcept
    {
        if (this != &other)
        {
            mSize = std::exchange(other.mSize, 0);
            mData = std::exchange(other.mData, nullptr);
        }
        return *this;
    }

    using iterator = T*;
    using const_iterator = const T*;

    iterator begin()
    {
        return mData;
    }

    const_iterator begin() const
    {
        return mData;
    }

    iterator end()
    {
        return mData + mSize;
    }

    const_iterator end() const
    {
        return mData + mSize;
    }

    std::reverse_iterator<iterator> rbegin()
    {
        return std::reverse_iterator<iterator>(end());
    }

    std::reverse_iterator<const_iterator> rbegin() const
    {
        return std::reverse_iterator<const_iterator>(end());
    }

    std::reverse_iterator<iterator> rend()
    {
        return std::reverse_iterator<iterator>(begin());
    }

    std::reverse_iterator<const_iterator> rend() const
    {
        return std::reverse_iterator<const_iterator>(begin());
    }

    std::size_t size() const
    {
        return mSize;
    }

    bool emtpy() const
    {
        return mSize == 0;
    }

    T* data() const
    {
        return mData;
    }

    T& operator[](std::size_t index)
    {
        return mData[index];
    }

    const T& operator[](std::size_t index) const
    {
        return mData[index];
    }

private:
    std::size_t mSize;
    T* mData;
};
    
} // namespace pse
