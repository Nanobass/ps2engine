//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        scene.hpp
//
// Description: Scene
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

#include <unordered_map>

//========================================
// Project Includes
//========================================

#include <real3d/real3d.hpp>

namespace r3d
{

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

class Scene;

struct Texture
{
    uint32_t mID;
    Scene* mScene;
    std::unique_ptr<pdi::Texture> mTexture;

    Texture(uint32_t id, Scene* scene, std::unique_ptr<pdi::Texture> texture)
        : mID(id)
        , mScene(scene)
        , mTexture(std::move(texture))
    {}
};

struct VertexBuffer
{
    uint32_t mID;
    Scene* mScene;
    std::unique_ptr<pdi::VertexBuffer> mBuffer;

    VertexBuffer(uint32_t id, Scene* scene, std::unique_ptr<pdi::VertexBuffer> buffer)
        : mID(id)
        , mScene(scene)
        , mBuffer(std::move(buffer))
    {}
};

struct Mesh
{
    uint32_t mID;
    Scene* mScene;
    std::shared_ptr<Texture> mTexture;
    std::vector<std::shared_ptr<VertexBuffer>> mReferences;
    std::vector<pdi::PrimBuffer> mBuffers;

    Mesh(uint32_t id, Scene* scene)
        : mID(id)
        , mScene(scene)
    {}

    void SetTexture(const std::string& name);
    void AddPrimBufferVT(pdi::PrimType type, size_t count, const std::string& vertex, const std::string texcoords);

};

struct Model
{
    uint32_t mID;
    Scene* mScene;
    std::shared_ptr<Mesh> mMesh;
    Math::Transform mTransform;
    bool mVisible;

    Model(uint32_t id, Scene* scene, const std::string& mesh);
};
        
class Scene
{
public:
    pdi::Device* mDevice;
    pdi::Display* mDisplay;
    pdi::RenderContext* mContext;
    std::unordered_map<uint32_t, std::shared_ptr<Texture>> mTextures;
    std::unordered_map<uint32_t, std::shared_ptr<VertexBuffer>> mBuffers;
    std::unordered_map<uint32_t, std::shared_ptr<Mesh>> mMeshes;
    std::unordered_map<uint32_t, std::shared_ptr<Model>> mModels;

    Scene(pdi::Device* device, pdi::Display* display, pdi::RenderContext* context)
        : mDevice(device)
        , mDisplay(display)
        , mContext(context)
    {}

    void PrintAllocations()
    {
        std::cout << "Scene Allocations:" << std::endl;
        for(auto& texture : mTextures)
        {
            std::cout << "Texture: "
                << "ID=" << texture.first << ", "
                << "Refs=" << texture.second.use_count() << ", "
                << "Width=" << texture.second->mTexture->GetWidth() << ", "
                << "Height=" << texture.second->mTexture->GetHeight() << ", "
                << "Format=" << texture.second->mTexture->GetFormat() 
            << std::endl;
        }
        for(auto& buffer : mBuffers)
        {
            std::cout << "VertexBuffer: "
                << "ID=" << buffer.first << ", "
                << "Refs=" << buffer.second.use_count() << ", "
                << "Size=" << buffer.second->mBuffer->mSize << ", "
                << "ElementSize=" << buffer.second->mBuffer->mElementSize
            << std::endl;
        }
        for(auto& mesh : mMeshes)
        {
            std::cout << "Mesh: "
                << "ID=" << mesh.first << ", "
                << "Refs=" << mesh.second.use_count()
            << std::endl;
        }
        for(auto& model : mModels)
        {
            std::cout << "Model: "
                << "ID=" << model.first << ", "
                << "Refs=" << model.second.use_count()
            << std::endl;
        }
        std::cout << std::endl;
    }

    void ClearUnreferencedAssets()
    {
        for (auto it = mModels.begin(); it != mModels.end(); ) {
            if (it->second.use_count() <= 1) {
                it = mModels.erase(it);
            } else {
                ++it;
            }
        }
        for (auto it = mMeshes.begin(); it != mMeshes.end(); ) {
            if (it->second.use_count() <= 1) {
                it = mMeshes.erase(it);
            } else {
                ++it;
            }
        }
        for (auto it = mBuffers.begin(); it != mBuffers.end(); ) {
            if (it->second.use_count() <= 1) {
                it = mBuffers.erase(it);
            } else {
                ++it;
            }
        }
        for (auto it = mTextures.begin(); it != mTextures.end(); ) {
            if (it->second.use_count() <= 1) {
                it = mTextures.erase(it);
            } else {
                ++it;
            }
        }
    }

    void DrawModel(Model* model)
    {
        mContext->PushMultMatrix(model->mTransform.GetTransformationMatrix());
        Mesh* mesh = model->mMesh.get();
        pdi::Texture* texture = mesh->mTexture->mTexture.get();
        for(auto& primBuffer : mesh->mBuffers)
        {
            mContext->DrawPrimBuffer(texture, &primBuffer);
        }
        mContext->PopMatrix();
    }

    std::shared_ptr<Texture> AddTexture(const std::string& name, std::unique_ptr<pdi::Texture> texture)
    {
        auto tex = std::make_shared<Texture>(joaat(name), this, std::move(texture));
        mTextures[tex->mID] = tex;
        return tex;
    }

    std::shared_ptr<VertexBuffer> AddVertexBuffer(const std::string& name, std::unique_ptr<pdi::VertexBuffer> buffer)
    {
        auto buf = std::make_shared<VertexBuffer>(joaat(name), this, std::move(buffer));
        mBuffers[buf->mID] = buf;
        return buf;
    }

    std::shared_ptr<Mesh> CreateMesh(const std::string& name)
    {
        auto mesh = std::make_shared<Mesh>(joaat(name), this);
        mMeshes[mesh->mID] = mesh;
        return mesh;
    }

    std::shared_ptr<Model> CreateModel(const std::string& name, const std::string& mesh)
    {
        auto model = std::make_shared<Model>(joaat(name), this, mesh);
        mModels[model->mID] = model;
        return model;
    }

    std::shared_ptr<Texture> FindTexture(const std::string& name)
    {
        auto find = mTextures.find(joaat(name));
        assert(find != mTextures.end());
        return find->second;
    }

    std::shared_ptr<VertexBuffer> FindVertexBuffer(const std::string& name)
    {
        auto find = mBuffers.find(joaat(name));
        assert(find != mBuffers.end());
        return find->second;
    }

    std::shared_ptr<Mesh> FindMesh(const std::string& name)
    {
        auto find = mMeshes.find(joaat(name));
        assert(find != mMeshes.end());
        return find->second;
    }

    std::shared_ptr<Model> FindModel(const std::string& name)
    {
        auto find = mModels.find(joaat(name));
        assert(find != mModels.end());
        return find->second;
    }

};

void Mesh::SetTexture(const std::string& name)
{
    mTexture = mScene->FindTexture(name);
}

void Mesh::AddPrimBufferVT(pdi::PrimType type, size_t count, const std::string& vertex, const std::string texcoords)
{
    pdi::PrimBuffer buffer;
    buffer.type = type;
    buffer.count = count;
    {
        auto vertexBuffer = mScene->FindVertexBuffer(vertex);
        mReferences.push_back(vertexBuffer);
        buffer.mVertexBuffer = vertexBuffer->mBuffer.get();        
    }
    {
        auto textureCoordBuffer = mScene->FindVertexBuffer(texcoords);
        mReferences.push_back(textureCoordBuffer);
        buffer.mTextureCoordBuffer = textureCoordBuffer->mBuffer.get();        
    }
    mBuffers.push_back(buffer);
}

Model::Model(uint32_t id, Scene* scene, const std::string& mesh)
    : mID(id)
    , mScene(scene)
    , mMesh(scene->FindMesh(mesh))
{}

} // namespace r3d
