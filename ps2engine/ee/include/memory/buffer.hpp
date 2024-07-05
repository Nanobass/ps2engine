//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        buffer.hpp
//
// Description: Buffer Type
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================

#include <memory/memory.hpp>

template <typename T, GameMemoryAllocator Alloc = GMA_DEFAULT>
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

    friend std::ostream& operator<<(std::ostream& os, const buffer<T, Alloc>& buffer)
    {
        os.write((const char*)buffer.mData, buffer.mSize * sizeof(T));
        return os;
    }

    friend std::istream& operator>>(std::istream& is, buffer<T, Alloc>& buffer)
    {
        is.read((char*)buffer.mData, buffer.mSize * sizeof(T));
        return is;
    }

private:
    std::size_t mSize;
    T* mData;
    GameMemoryAllocator mAllocator;
};