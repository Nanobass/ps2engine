//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        real3d.hpp
//
// Description: Real3D Header File
//
// Sub System:  Real3D Rendering Engine
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
#include <vector>
#include <list>

#include <libcdvd-common.h>

//========================================
// Project Includes
//========================================

/* pdi */
#include <pdi/pdi.hpp>

namespace r3d {

struct r3dChunk
{
    uint32_t ckID;
    uint32_t ckSize;
    uint8_t* ckData;

    r3dChunk() : ckID(0), ckSize(0), ckData(nullptr) {}
    ~r3dChunk() { if(ckData) delete ckData; }

    r3dChunk(const r3dChunk& other) = delete;
    r3dChunk& operator=(const r3dChunk& other) = delete;

    r3dChunk(r3dChunk&& other) noexcept
        : ckID(std::exchange(other.ckID, 0)), ckSize(std::exchange(other.ckSize, 0)), ckData(std::exchange(other.ckData, nullptr))
    {}

    r3dChunk& operator=(r3dChunk&& other) noexcept
    {
        if (this == &other)
            return *this;
        
        delete ckData;

        ckID = other.ckID;
        ckSize = other.ckSize;
        ckData = other.ckData;

        other.ckData = nullptr;
        other.ckSize = 0;
        return *this;
    }

    friend std::istream& operator>>(std::istream& is, r3dChunk& chunk)
    {
        is.read(reinterpret_cast<char*>(&chunk.ckID), sizeof(chunk.ckID));
        is.read(reinterpret_cast<char*>(&chunk.ckSize), sizeof(chunk.ckSize));
        delete chunk.ckData;
        chunk.ckData = new(GMA_FILE_IO) uint8_t[chunk.ckSize];
        is.read(reinterpret_cast<char*>(chunk.ckData), chunk.ckSize);
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, const r3dChunk& chunk)
    {
        os.write(reinterpret_cast<const char*>(&chunk.ckID), sizeof(chunk.ckID));
        os.write(reinterpret_cast<const char*>(&chunk.ckSize), sizeof(chunk.ckSize));
        os.write(reinterpret_cast<const char*>(chunk.ckData), chunk.ckSize);
        return os;
    }
};

struct r3dImageHeader
{
    static const uint32_t ID = 0x10;
    uint16_t mWidth, mHeight;
    uint32_t mFormat;
} __attribute__ ((packed));

struct r3dImageChunk
{
    static const uint32_t ID = 0x11;
    uint32_t mOffset, mSize;
    uint8_t mData[16384];
    uint8_t mEndChunk;
} __attribute__ ((packed));

struct r3dEndChunk
{
    static const uint32_t ID = 0xFFFFFFFF;
};

} // namespace r3d