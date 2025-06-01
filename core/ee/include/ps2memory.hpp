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
#include <cstddef>
#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <new>
#include <stack>
#include <memory>
#include <iostream>
#include <utility>

//========================================
// Project Includes
//========================================

#include <ps2heap.hpp>

namespace pse::memory
{
    
uint32_t joaat(const std::string& str);
uint32_t md5sum(const std::string& str);

struct name
{
    std::string mStringName;
    uint32_t mID;

    name(const std::string& name) 
        : mStringName(name), mID(joaat(name))
    {}

    name(const char* name)
        : mStringName(name), mID(joaat(mStringName))
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

}