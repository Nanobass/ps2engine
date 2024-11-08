//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        fragment.hpp
//
// Description: Fragment Renderer
//              Useful with GL_TRIANGLE_STRIP and GL_QUAD_STRIP
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================

#include <engine/texturemanager.hpp>

#include <ps2memory.hpp>

namespace Engine
{

struct Material
{
    name mName;
    color mAmbient, mDiffuse, mSpecular, mEmission;
    float mShininess;
    GLenum mColorMaterial;

    Material(const name& name, const color& ambient, const color& diffuse, const color& specular = color(1,1,1), const color& emission = color(0,0,0), float shininess = 0.0F)
        : mName(name), mAmbient(ambient), mDiffuse(diffuse), mSpecular(specular), mEmission(emission), mShininess(0.0F), mColorMaterial(0)
    {}

    Material(const name& name, const color& ambient, const color& specular = color(1,1,1), const color& emission = color(0,0,0), float shininess = 0.0F)
        : mName(name), mAmbient(ambient), mDiffuse(0, 0, 0), mSpecular(specular), mEmission(emission), mShininess(shininess), mColorMaterial(GL_DIFFUSE)
    {}

    void Bind()
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT, mAmbient.vector);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mDiffuse.vector);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mSpecular.vector);
        glMaterialfv(GL_FRONT, GL_EMISSION, mEmission.vector);
        glMaterialfv(GL_FRONT, GL_SHININESS, &mShininess);
        if(mColorMaterial) 
        {
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT, mColorMaterial);
        } else glDisable(GL_COLOR_MATERIAL);
    }

};


struct GeometryFragment
{

    inline static const size_t FloatsPerVertex = 3 + 2 + 3 + 4; // vec3 + vec2 + vec3 + vec4

    GLsizei mCount;
    vec3* mVertices;
    vec2* mTextureCoordinates;
    vec3* mNormals;
    vec4* mColors;

    GeometryFragment() {}

    GeometryFragment(GLsizei count, vec3* vertices, vec2* textureCoords, vec3* normals, vec4* colors)
        : mCount(count), mVertices(vertices), mTextureCoordinates(textureCoords), mNormals(normals), mColors(colors)
    {}

};

struct Geometry
{
    name mName;
    GLenum mPrimitive;
    bool mColorArray;
    buffer<GeometryFragment> mFragments;

    Geometry(const name& name, GLenum primitive, bool colorArray, size_t fragmentCount)
        : mName(name), mPrimitive(primitive), mColorArray(colorArray), mFragments(fragmentCount)
    {}

    void Draw()
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        if(mColorArray) glEnableClientState(GL_COLOR_ARRAY);
        else glDisableClientState(GL_COLOR_ARRAY);
        for(auto& fragment : mFragments)
        {
            glVertexPointer(3, GL_FLOAT, 0, fragment.mVertices);
            glTexCoordPointer(2, GL_FLOAT, 0, fragment.mTextureCoordinates);
            glNormalPointer(GL_FLOAT, 0, fragment.mNormals);
            if(mColorArray) glColorPointer(4, GL_FLOAT, 0, fragment.mColors);
            glDrawArrays(mPrimitive, 0, fragment.mCount);
        }
    }

};

struct FragmentRenderer
{
    GeometryAllocator* mGeometryAllocator;
    TextureManager* mTextureManager;

    std::map<uint32_t, std::unique_ptr<Geometry>> mGeometries;
    std::map<uint32_t, std::unique_ptr<Material>> mMaterials;

    GeometryFragment CreateFragment(GLsizei count)
    {
        size_t bufferSize = count * GeometryFragment::FloatsPerVertex;
        float* buffer = mGeometryAllocator->AllocateFloats(bufferSize);
        vec3* vertices = (vec3*) (buffer + count * 0);
        vec2* textureCoords = (vec2*) (buffer + count * 3);
        vec3* normals = (vec3*) (buffer + count * 5);
        vec4* colors = (vec4*) (buffer + count * 8);
        return GeometryFragment(count, vertices, textureCoords, normals, colors);
    }

    GeometryFragment& AddFragment(Geometry* geom, uint32_t index, GLsizei count)
    {
        geom->mFragments[index] = CreateFragment(count);
        return geom->mFragments[index];
    }

    void DeleteFragment(const GeometryFragment& fragment)
    {
        float* buffer = (float*) fragment.mVertices;
        mGeometryAllocator->FreeFloats(buffer);
    }

    Geometry* CreateGeometry(const name& name, GLenum primitive, bool colorArray, size_t fragmentCount)
    {
        auto geometry = std::make_unique<Geometry>(name, primitive, colorArray, fragmentCount);
        Geometry* pointer = geometry.get();
        mGeometries[name.mID] = std::move(geometry);
        return pointer;
    }

    Geometry* FindGeometry(const uint32_t& name)
    {
        auto it = mGeometries.find(name);
        return it->second.get();
    }

    void DeleteGeometry(const uint32_t& name)
    {
        auto it = mGeometries.find(name);
        Geometry* geom = it->second.get();
        for(auto& fragment : geom->mFragments) DeleteFragment(fragment);
        mGeometries.erase(it);
    }

    Material* CreateMaterial(const name& name, const color& ambient, const color& diffuse, const color& specular = color(1,1,1), const color& emission = color(0,0,0), float shininess = 0.0F)
    {
        auto material = std::make_unique<Material>(name, ambient, diffuse, specular, emission, shininess);
        Material* pointer = material.get();
        mMaterials[name.mID] = std::move(material);
        return pointer;
    }

    Material* CreateMaterial(const name& name, const color& ambient, const color& specular = color(1,1,1), const color& emission = color(0,0,0), float shininess = 0.0F)
    {
        auto material = std::make_unique<Material>(name, ambient, specular, emission, shininess);
        Material* pointer = material.get();
        mMaterials[name.mID] = std::move(material);
        return pointer;
    }

    Material* FindMaterial(const uint32_t& name)
    {
        auto it = mMaterials.find(name);
        return it->second.get();
    }

    void DeleteMaterial(const uint32_t& name)
    {
        auto it = mMaterials.find(name);
        mMaterials.erase(it);
    }

};
    
} // namespace Engine
