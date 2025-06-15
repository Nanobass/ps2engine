//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        modelmanager.hpp
//
// Description: Model Manager
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

#include <list>

//========================================
// PS2SDK Includes
//========================================

//========================================
// Project Includes
//========================================

#include <core/memory.hpp>
#include <core/log.hpp>

#include <engine/memory/memory.hpp>

#include <engine/texturemanager.hpp>
#include <engine/material.hpp>

namespace pse
{

struct sub_mesh
{
    material_ptr mMaterial;
    texture_ptr mTexture;
    int mPrimType, mPrimCount;
    memory::buffer<math::vec3> mVertexBuffer;
    memory::buffer<math::vec3> mNormalBuffer;
    memory::buffer<math::vec2> mTexCoordBuffer;
    memory::buffer<math::vec4> mColorBuffer;
    int mDList;

    sub_mesh(material_ptr material, texture_ptr texture, int primType, int primCount, int vertCount, bool hasNormals = true, bool hasTexCoords = true, bool hasColors = false)
        : mMaterial(material)
        , mTexture(texture)
        , mPrimType(primType)
        , mPrimCount(primCount)
        , mVertexBuffer(vertCount)
        , mNormalBuffer(hasNormals ? vertCount : 0)
        , mTexCoordBuffer(hasTexCoords ? vertCount : 0)
        , mColorBuffer(hasColors ? vertCount : 0)
        , mDList(0)
    {}

    ~sub_mesh()
    {
        if(mDList) glDeleteLists(mDList, 1);
    }

    sub_mesh(const sub_mesh& other) = delete;
    sub_mesh& operator=(const sub_mesh& other) = delete;

    void set_vertex_arrays()
    {
        // we will always have these
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, mVertexBuffer.data());
        // not having normals will blow up our dma packets
        glEnableClientState(GL_NORMAL_ARRAY);
        if(mNormalBuffer.emtpy()) pglNormalPointer(3, GL_FLOAT, 0, mVertexBuffer.data());
        else pglNormalPointer(3, GL_FLOAT, 0, mNormalBuffer.data());
        // not having texcoords will also blow up our dma packets
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        if(mTexCoordBuffer.emtpy()) 
        {
            glTexCoordPointer(2, GL_FLOAT, 0, mVertexBuffer.data());
            glDisable(GL_TEXTURE_2D);
        }
        else 
        {
            glTexCoordPointer(2, GL_FLOAT, 0, mTexCoordBuffer.data());
            glEnable(GL_TEXTURE_2D);
        }
        // colors are not duplicated if not present, good job ps2gl
        if(mColorBuffer.emtpy()) glDisableClientState(GL_COLOR_ARRAY);
        else { glEnableClientState(GL_COLOR_ARRAY); glColorPointer(4, GL_FLOAT, 0, mColorBuffer.data()); }
    }
    
    void draw_arrays()
    {
        // apply new material and texture if needed
        if(mMaterial) mMaterial->apply();
        if(mTexture) mTexture->bind();
        // setup buffers
        set_vertex_arrays();
        // do some drawing
        glDrawArrays(mPrimType, 0, mPrimCount);
    }

    void generate_display_list()
    {
        // generate list
        if(mDList) { glDeleteLists(mDList, 1); mDList = 0; }
        if(!mDList) mDList = glGenLists(1);
        glNewList(mDList, GL_COMPILE);
        // apply new material and texture if needed
        if(mMaterial) mMaterial->apply();
        if(mTexture) mTexture->bind();
        // do some drawing
        glDrawArrays(mPrimType, 0, mPrimCount);
        glEndList();
    }

    void draw_display_list()
    {
        if(!mDList) return;
        // setup buffers
        set_vertex_arrays();
        // draw list (does this link dma chains?)
        glCallList(mDList);
    }

};

struct mesh
{
    memory::resource_id mId;
    std::list<sub_mesh> mSubMeshes;

    mesh(memory::resource_id id) 
        : mId(id)
    {
        log::out(log::kInfo) << "created mesh uuid=" << mId << std::endl;
    }

    ~mesh()
    {
        log::out(log::kInfo) << "deleted mesh uuid=" << mId << std::endl;
    }

};

using mesh_ptr = std::shared_ptr<mesh>;

class model_manager;

struct mesh_deleter
{
    model_manager* mModelManager;
    inline void operator()(mesh* m);
};

class model_manager
{
public:
    model_manager() {}

    ~model_manager()
    {
        mMeshes.clear();
    }

    mesh_ptr create_mesh(const memory::resource_id& id)
    {
        mesh_ptr m = mesh_ptr(new mesh(id), mesh_deleter{this});
        mMeshes[id.mUuid] = m;
        return m;
    }

    mesh_ptr find_mesh(const uuid& uuid)
    {
        auto it = mMeshes.find(uuid);
        return it->second.lock();
    }

    void delete_mesh(const uuid& uuid)
    {
        auto it = mMeshes.find(uuid);
        if(it->second.expired()) 
        {
            mMeshes.erase(it);
        }
    }

private:
    // gotta love c++
    std::map<uuid, std::weak_ptr<mesh>> mMeshes;
};

inline void mesh_deleter::operator()(mesh* m) { 
    mModelManager->delete_mesh(m->mId.mUuid); 
    delete m;
}
    
} // namespace pse
