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

namespace pse
{
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
}

namespace pse::memory
{

typedef int allocator_id;

enum pse_allocators : allocator_id
{
    PSE_ALLOCATOR_DEFAULT = 0, /* system malloc wrapper */
    PSE_ALLOCATOR_END
};

struct allocator;
struct dlheap;

void initialize();
void terminate();
void* allocate(allocator_id alloc, size_t size);
void* allocate(size_t size);
void deallocate(allocator_id alloc, void* ptr);
void deallocate(void* ptr);

allocator_id get_default_allocator();
void set_default_allocator(allocator_id alloc);

allocator_id find_allocator(void* ptr);
allocator_id get_allocator_id(allocator* alloc);
allocator* get_allocator(allocator_id alloc);
void reset_allocator(allocator_id alloc);

void push_allocator(allocator_id alloc);
allocator_id pop_allocator();
allocator_id get_current_allocator();
void set_current_allocator(allocator_id alloc);

void set_tracking(bool enabled, size_t threshold = 0, const char* name = nullptr);
void print_statistics();
size_t get_total_free_memory();

typedef void (out_of_memory_callback_t)(allocator_id alloc, size_t size, void* args);
void set_out_of_memory_callback(out_of_memory_callback_t* cb, void* args);
void out_of_memory_callback(allocator_id alloc, size_t size);

void register_allocator(allocator_id alloc, allocator* alloc_ptr);
allocator* unregister_allocator(allocator_id alloc);

void register_pool_allocator(allocator_id id, allocator_id parent, const char* name, size_t blockSize, size_t blockCount);
void register_bump_allocator(allocator_id id, allocator_id parent, const char* name, size_t size);
void register_douglea_allocator(allocator_id id, allocator_id parent, const char* name, size_t size);

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

uint32_t joaat(const std::string& str);

struct name
{
    std::string mName;
    uuid mUuid;

    name(const std::string& name) 
        : mName(name), mUuid(joaat(name))
    {}

    name(const char* name)
        : mName(name), mUuid(joaat(mName))
    {}

};

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
    explicit buffer(const buffer& other) = delete;
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
