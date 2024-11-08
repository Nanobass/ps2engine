//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        buffer.hpp
//
// Description: Float VertexBuffer
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
#include <assert.h>

//========================================
// Project Includes
//========================================

#include <ps2math.hpp>

namespace Engine
{

template <typename T = Math::Vec4>
struct VertexBuffer
{
    using type = T;

public:
    
    explicit VertexBuffer(std::size_t size)
        :   mSize(size), 
            mData(new type[size])
        {}
    
    ~VertexBuffer()
    {
        if(mData) delete[] mData;
    }

    // no copying, no, ONLY EXPLICIT COPYING, WE DO NOT HAVE GIGABYTES OF SYSTEM MEMORY
    explicit VertexBuffer(const VertexBuffer& other) = delete;
    VertexBuffer& operator=(const VertexBuffer& other) = delete;

    // moving is fine and kinda required without copying
    VertexBuffer(VertexBuffer&& other) noexcept 
        :   mSize(std::exchange(other.mSize, 0)), 
            mData(std::exchange(other.mData, nullptr))
    {}

    VertexBuffer& operator=(VertexBuffer&& other) noexcept
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

    type* data() const
    {
        return mData;
    }

    type& operator[](std::size_t index)
    {
        return mData[index];
    }

    const type& operator[](std::size_t index) const
    {
        return mData[index];
    }

private:
    std::size_t mSize;
    type* mData;
};
    
} // namespace Engine
