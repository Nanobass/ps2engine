//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        reader.hpp
//
// Description: Real3D File Format Reader
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

#include <fstream>

//========================================
// Project Includes
//========================================

#include <engine/renderer.hpp>

#include <ps2memory.hpp>
#include <ps2math.hpp>

namespace Real3D
{

struct Chunk
{
    uint32_t mType;
    name mName;
    size_t mLength;

    static uint32_t read_type(std::istream& stream)
    {
        uint32_t type;
        stream.read((char*) &type, sizeof(type));
        return type;
    }

    static name read_name(std::istream& stream)
    {
        uint16_t nameLength;
        stream.read((char*) &nameLength, sizeof(nameLength));
        std::string nameString;
        nameString.resize(nameLength);
        stream.read(nameString.data(), nameString.length());
        uint32_t nameHash;
        stream.read((char*) &nameHash, sizeof(nameHash));
        assert(joaat(nameString) == nameHash);
        return name(nameString);
    }

    static size_t read_length(std::istream& stream)
    {
        size_t length;
        stream.read((char*) &length, sizeof(length));
        return length;
    }

    Chunk(const uint32_t& type, std::istream& stream) 
        : mType(type), mName(read_name(stream)), mLength(read_length(stream))
    {
        std::cout << "Chunk: Type=" << mType << " Name=" << mName.mStringName << std::endl;
    }

    virtual ~Chunk() {}

    virtual void Install(Engine::MasterRenderer* renderer) = 0;
};

namespace Chunks
{

struct TextureChunk : public Chunk
{
    static constexpr uint32_t ID = 0x30;

    GLenum mFormat, mType;
    Engine::TextureBuffer mCore;

    static GLenum read_format(std::istream& stream)
    {
        GLenum format;
        stream.read((char*) &format, sizeof(format));
        return format;
    }

    static GLenum read_type(std::istream& stream)
    {
        GLenum type;
        stream.read((char*) &type, sizeof(type));
        return type;
    }
    
    static Engine::TextureBuffer read_core(std::istream& stream, GLenum format, GLenum type)
    {
        uint16_t width, height;
        uint8_t compression, bpp;
        stream.read((char*) &compression, sizeof(compression));
        stream.read((char*) &width, sizeof(width));
        stream.read((char*) &height, sizeof(height));
        stream.read((char*) &bpp, sizeof(bpp));
        Engine::TextureBuffer core(width, height, bpp, format, type);
        stream.read((char*)core.mData.data(), core.mData.size());
        return core;
    }

    TextureChunk(std::istream& stream) 
        : Chunk(ID, stream), mFormat(read_format(stream)), mType(read_type(stream)), mCore(read_core(stream, mFormat, mType))
    {
    }

    void Install(Engine::MasterRenderer* renderer)
    {
        renderer->mTextureManager->CreateTexture(mName, std::move(mCore));
    }
};
    
} // namespace Chunks


struct Reader
{

    std::map<uint32_t, std::function<std::unique_ptr<Chunk>(std::istream&)>> mChunkFactories;
    std::ifstream mInput;

    Reader(const std::string& file) : mInput(file)
    {
        #define ChunkFactoryEntry(type) mChunkFactories[Chunks::type::ID] = [] (std::istream& input) { return std::make_unique<Chunks::type>(input); }
        ChunkFactoryEntry(TextureChunk);
        #undef ChunkFactoryEntry
    }

    std::unique_ptr<Chunk> ReadChunk()
    {
        if(mInput.peek(), mInput.eof()) return nullptr;
        uint32_t type = Chunk::read_type(mInput);
        return mChunkFactories[type](mInput);
    }


};
    
} // namespace Real3D
