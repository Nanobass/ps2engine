//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        terrain.hpp
//
// Description: Voxel Terrain System
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

/* standard library */
#include <stdint.h>
#include <list>

/* ps2gl */
#include <GL/ps2gl.h>
#include <GL/gl.h>

//========================================
// Project Includes
//========================================

/* ps2math */
#include <ps2math.hpp>

extern int edgeTable[256];
extern int triTable[256][16];

struct CTerrainVoxel;
class CTerrainChunk;
class CTerrain;

struct CIPosition {
    int x, y, z;
    
    CIPosition operator+(const CIPosition& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    CIPosition operator-(const CIPosition& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    CIPosition operator*(const CIPosition& other) const {
        return {x * other.x, y * other.y, z * other.z};
    }

    CIPosition operator/(const CIPosition& other) const {
        return {x / other.x, y / other.y, z / other.z};
    }

    CIPosition operator*(const int& scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }

    CIPosition operator/(const int& scalar) const {
        return {x / scalar, y / scalar, z / scalar};
    }

    CIPosition operator%(const int& scalar) const {
        return {x % scalar, y % scalar, z % scalar};
    }

    bool operator==(const CIPosition& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const CIPosition& other) const {
        return x != other.x || y != other.y || z != other.z;
    }

    friend std::ostream& operator<<(std::ostream& os, const CIPosition& pos) {
        os << "(" << pos.x << ", " << pos.y << ", " << pos.z << ")";
        return os;
    }
};

struct CTerrainVoxel {
    uint8_t mMaterial;
    uint8_t mIntensity;
};

struct CTerrainMesh {
    bool mMeshDirty;
    GLuint mList;
};

class CTerrainChunk {
public:
    static const int cChunkSize = 16;
    static const int cVoxelCount = cChunkSize * cChunkSize * cChunkSize;
    static const int cLayerCount = cChunkSize * cChunkSize;

    CTerrainChunk(CTerrain* terrain, CIPosition position)
      : mVoxels{0}
      , mTerrain(terrain)    
      , mPosition(position)
      , mMesh({0, false})
    {}
    ~CTerrainChunk() {}

    CTerrainChunk(const CTerrainChunk&) = delete;
    CTerrainChunk& operator=(const CTerrainChunk&) = delete;
    CTerrainChunk(CTerrainChunk&&) = default;
    CTerrainChunk& operator=(CTerrainChunk&&) = default;

    CIPosition position() const { return mPosition; }
    CIPosition voxel() const { return mPosition * cChunkSize; }
    CTerrainMesh& mesh() { return mMesh; }

    CTerrainVoxel* get(CIPosition voxel) {
        return at(voxel.x, voxel.y, voxel.z);
    }

    void InterpolateVertex(pse::math::vec3* p, uint8_t isoLevel, CTerrainVoxel* v0, pse::math::vec3 p0, CTerrainVoxel* v1, pse::math::vec3 p1);
    void GenerateTriangles(int index, pse::math::vec3* vertlist);
    void GenerateMesh();

private:
    CTerrainVoxel* at(int localX, int localY, int localZ) {
        if(localX < 0 || localX >= cChunkSize || localY < 0 || localY >= cChunkSize || localZ < 0 || localZ >= cChunkSize)
            return nullptr;
        return &mVoxels[localY * cLayerCount + localZ * cChunkSize + localX];
    }

    CTerrainVoxel* atOffset(int quadraticOffset, int linearOffset, int constantOffset) {
        if(quadraticOffset < 0 || quadraticOffset >= cVoxelCount || linearOffset < 0 || linearOffset >= cLayerCount || constantOffset < 0 || constantOffset >= cChunkSize)
            return nullptr;
        return &mVoxels[quadraticOffset + linearOffset + constantOffset];
    }
    
private:
    CTerrainVoxel mVoxels[cVoxelCount];
    CTerrain* mTerrain;
    CIPosition mPosition;
public:
    CTerrainMesh mMesh;

};

class CTerrain {
public:
    
    CTerrain(uint8_t isoLevel = 32) 
      : mIsoLevel(isoLevel)
    {}

    ~CTerrain() {
        mChunks.clear();
    }

    CTerrain(const CTerrain&) = delete;
    CTerrain& operator=(const CTerrain&) = delete;
    CTerrain(CTerrain&&) = default;
    CTerrain& operator=(CTerrain&&) = default;

    uint8_t isoLevel() const { return mIsoLevel; }
    void isoLevel(uint8_t isoLevel) { mIsoLevel = isoLevel; }

    CTerrainChunk* addChunk(CIPosition position) {
        return &mChunks.emplace_back(this, position);
    }

    void removeChunk(CIPosition position) {

    }

    std::list<CTerrainChunk>& chunks() {
        return mChunks;
    }
    
    CTerrainChunk* getChunk(CIPosition position) {
        for(auto& chunk : mChunks) 
            if(chunk.position() == position) 
                return &chunk;
        return nullptr;
    }

    CTerrainChunk* getChunkAtVoxel(CIPosition voxel) {
        return getChunk(voxel % CTerrainChunk::cChunkSize);
    }

    CTerrainVoxel* getVoxel(CIPosition voxel) {
        CTerrainChunk* chunk = getChunkAtVoxel(voxel);
        if(!chunk) return nullptr;
        return chunk->get(voxel - chunk->voxel());
    }

private:
    uint8_t mIsoLevel;
    std::list<CTerrainChunk> mChunks;
};

#include <noise.h>

inline void makeChunk(CTerrain* terrain, int x, int y, int z)
{
    CTerrainChunk* chunk = terrain->addChunk({x, y, z});
    if(!chunk) return;
    for(int i = 0; i < CTerrainChunk::cChunkSize; i++)
    {
        for(int j = 0; j < CTerrainChunk::cChunkSize; j++)
        {
            for(int k = 0; k < CTerrainChunk::cChunkSize; k++)
            {
                chunk->get({i, j, k})->mIntensity = PerlinNoise::GetValue(
                    (x * CTerrainChunk::cChunkSize + i) * 0.0201f,
                    (y * CTerrainChunk::cChunkSize + j) * 0.0201f,
                    (z * CTerrainChunk::cChunkSize + k) * 0.0201f
                ) * 255;
            }
        }
    }
}