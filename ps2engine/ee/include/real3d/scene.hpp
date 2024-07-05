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

    Texture(uint32_t id, Scene* scene, pdi::Texture* texture)
        : mID(id)
        , mScene(scene)
        , mTexture(texture)
    {}
};

struct VertexBuffer
{
    uint32_t mID;
    Scene* mScene;
    std::unique_ptr<pdi::VertexBuffer> mBuffer;

    VertexBuffer(uint32_t id, Scene* scene, pdi::VertexBuffer* buffer)
        : mID(id)
        , mScene(scene)
        , mBuffer(buffer)
    {}
};

struct Mesh
{
    uint32_t mID;
    Scene* mScene;
    std::shared_ptr<Texture> mTexture;
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

    std::shared_ptr<Texture> AddTexture(const std::string& name, pdi::Texture* texture)
    {
        auto tex = std::make_shared<Texture>(joaat(name), this, texture);
        mTextures[tex->mID] = tex;
        return tex;
    }

    std::shared_ptr<VertexBuffer> AddVertexBuffer(const std::string& name, pdi::VertexBuffer* buffer)
    {
        auto buf = std::make_shared<VertexBuffer>(joaat(name), this, buffer);
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
    buffer.mVertexBuffer = mScene->FindVertexBuffer(vertex)->mBuffer.get();
    buffer.mTextureCoordBuffer = mScene->FindVertexBuffer(texcoords)->mBuffer.get();
    mBuffers.push_back(buffer);
}

Model::Model(uint32_t id, Scene* scene, const std::string& mesh)
    : mID(id)
    , mScene(scene)
    , mMesh(scene->FindMesh(mesh))
{}

} // namespace r3d
