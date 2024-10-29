//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        ps2memory.hpp
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
#include <list>
#include <string>
#include <ostream>
#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <new>

//========================================
// Project Includes
//========================================

// define memory 'areas' to group allocations

enum GameMemoryAllocator
{
    GMA_GENERIC,                // for generic allocations (default allocator)
    GMA_PERSISTANT,             // for persistant allocations, such as singletons or global objects
    GMA_GRAPHICS,               // for graphical content such as textures, meshes and animations
    GMA_PS2GL,                  // for ps2gl
    NUM_GAME_MEMORY_ALLOCATORS  // number of memory allocators
};

inline std::string g_AllocatorNames[] = {
    "GMA_GENERIC",
    "GMA_PERSISTANT",
    "GMA_GRAPHICS",
    "GMA_PS2GL",
    "nullptr"
};

// route allocations through our memory system

void* gmaAllocateMemory(GameMemoryAllocator allocator, size_t size, size_t align = 1) 
{
    return memalign(align, size);
}

void gmaFreeMemory(void* p)
{
    free(p);
}

void* operator new (std::size_t size, GameMemoryAllocator allocator)
{
    return gmaAllocateMemory(allocator, size);
}

void* operator new[] (std::size_t size, GameMemoryAllocator allocator)
{
    return gmaAllocateMemory(allocator, size);
}

void* operator new (std::size_t size)
{
    return gmaAllocateMemory(GMA_GENERIC, size);
}

void* operator new[] (std::size_t size)
{
    return gmaAllocateMemory(GMA_GENERIC, size);
}

void* operator new (std::size_t size, std::align_val_t align)
{
    return gmaAllocateMemory(GMA_GENERIC, size, (size_t) align);
}

void* operator new[] (std::size_t size, std::align_val_t align)
{
    return gmaAllocateMemory(GMA_GENERIC, size, (size_t) align);
}

void operator delete (void* ptr)
{
    gmaFreeMemory(ptr);
}

void operator delete[] (void* ptr)
{
    gmaFreeMemory(ptr);
}

namespace gma
{

template <typename T, GameMemoryAllocator Alloc>
class allocator: public std::allocator<T>
{
public:
        typedef size_t size_type;
        typedef T* pointer;
        typedef const T* const_pointer;

        template<typename _Tp1>
        struct rebind
        {
            typedef allocator<_Tp1, Alloc> other;
        };

        pointer allocate(size_type n, const void *hint=0)
        {
                return (pointer) gmaAllocateMemory(Alloc, n*sizeof(T));
        }

        void deallocate(pointer p, size_type n)
        {
                return gmaFreeMemory(p);
        }

        allocator() throw(): std::allocator<T>() {}
        allocator(const allocator &a) throw(): std::allocator<T>(a) {}
        template <class U>                    
        allocator(const allocator<U, Alloc> &a) throw(): std::allocator<T>(a) {}
        ~allocator() throw() {}
};

namespace allocators
{

template <typename T>
using generic = gma::allocator<T, GMA_GENERIC>;

template <typename T>
using persistant = gma::allocator<T, GMA_PERSISTANT>;

template <typename T>
using graphics = gma::allocator<T, GMA_GRAPHICS>;
    
} // namespace allocator

} // namespace gma

template <typename T, GameMemoryAllocator Alloc>
using vector = std::vector<T, gma::allocator<T, Alloc>>;

template <typename T, GameMemoryAllocator Alloc>
using list = std::list<T, gma::allocator<T, Alloc>>;

template <typename K, typename V, GameMemoryAllocator Alloc>
using map = std::map<K, V, std::less<K>, gma::allocator<std::pair<K, V>, Alloc>>;

template <typename T, GameMemoryAllocator Alloc = GMA_GENERIC>
class buffer
{
    using type = T;

public:
    
    explicit buffer(std::size_t size, GameMemoryAllocator alloc = Alloc)
        :   mSize(size), 
            mData(new(alloc) T[size]), 
            mAllocator(alloc) 
        {}

    
    ~buffer()
    {
        if(mData) delete[] mData;
    }

    explicit buffer(const buffer& other) 
        :   mSize(other.mSize), 
            mData(new(mAllocator) T[other.mSize]), 
            mAllocator(other.mAllocator)
    {
        std::uninitialized_copy(other.begin(), other.end(), begin());
    }

    buffer& operator=(const buffer& other)
    {
        if (this != &other)
        {
            ~buffer();
            mSize = other.mSize;
            mData = new(mAllocator) T[other.mSize];
            mAllocator = other.mAllocator;
            std::uninitialized_copy(other.begin(), other.end(), begin());
        }
        return *this;
    }

    buffer(buffer&& other) noexcept 
        :   mSize(std::exchange(other.mSize, 0)), 
            mData(std::exchange(other.mData, nullptr)), 
            mAllocator(std::move(other.mAllocator))
    {}

    buffer& operator=(buffer&& other) noexcept
    {
        if (this != &other)
        {
            mSize = std::exchange(other.mSize, 0);
            mData = std::exchange(other.mData, nullptr);
            mAllocator = std::move(other.mAllocator);
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
    GameMemoryAllocator mAllocator;
};

namespace stream
{
    
template <typename T>
void read_buf(std::istream& in, T* data, size_t n)
{
    in.read((char*)data, n*sizeof(T));
}

template <typename T>
void read_buf(std::istream& in, buffer<T>& buffer)
{
    in.read((char*)buffer.data(), buffer.size()*sizeof(T));
}

void read_string(std::istream& in, std::string& string)
{
    uint16_t size; in.read((char*)&size,sizeof(size));
    string.resize(size);
    read_buf(in, string.data(), string.size());
}

template <typename T>
T read(std::istream& in)
{
    T t;
    read_buf(in, &t, 1);
    return t;
}

} // namespace Stream

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