//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        skybox.hpp
//
// Description: Skybox Renderer
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
#include <fstream>
#include <utility>
#include <memory>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <exception>
#include <sstream>
#include <regex>

/* ps2sdk */
#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <stdio.h>
#include <graph.h>
#include <gs_psm.h>
#include <osd_config.h>

/* ps2gl */
#include <GL/ps2gl.h>
#include <GL/gl.h>

/* ps2stuff */
#include <ps2s/gs.h>

//========================================
// Project Includes
//========================================

/* engine */
#include <engine/texturemanager.hpp>
#include <engine/camera.hpp>

/* ps2memory */
#include <ps2memory.hpp>

/* ps2glu */
#include <ps2glu.hpp>

/* ps2math */
#include <ps2math.hpp>

namespace Engine
{

struct SkyboxRenderer
{
    TextureManager* mTextureManager;
    Texture* mSkyTexture = nullptr;

    GLuint mList = 0;

    Math::Vec4 mSunPosition = Math::Vec4(0.0F, 0.0F, 1.0F, 0.0F);
    Math::Color mSunAmbient = Math::Color(0.1F, 0.1F, 0.1F, 1.0F);
    Math::Color mSunDiffuse = Math::Color(1.0F, 1.0F, 1.0F, 1.0F);
    Math::Color mSunSpecular = Math::Color(1.0F, 1.0F, 1.0F, 1.0F);

    SkyboxRenderer(TextureManager* textureManager) 
        :   mTextureManager(textureManager) 
    {}

    ~SkyboxRenderer()
    {
        if(mList || mSkyTexture) UnloadSkybox();
    }

    void LoadSkybox(const std::string& img, float size = 16.0F, int divisions = 16)
    {
        if(mList || mSkyTexture) UnloadSkybox();

        float subSize = size / (float) divisions;

        mSkyTexture = mTextureManager->LoadGsTexture(img, img);

        mList = glGenLists(1);
        glNewList(mList, GL_COMPILE);
        glBegin(GL_QUADS);
        // chatgpt is the best, wrap shit in a dlist for moa pafomance
        for (int face = 0; face < 6; ++face) {
            for (int i = 0; i < 16; ++i) {
                for (int j = 0; j < 16; ++j) {
                    float x = -size + j * subSize * 2;
                    float y = -size + i * subSize * 2;

                    // Texture coordinates for each sub-quad
                    float u = j / 16.0f;
                    float v = i / 16.0f;
                    float uNext = (j + 1) / 16.0f;
                    float vNext = (i + 1) / 16.0f;

                    // Calculate the vertices for each face based on the face index
                    float x0, y0, z0, x1, y1, z1, x2, y2, z2, x3, y3, z3;
                    switch (face) {
                        case 0: // Front face
                            x0 = x;       y0 = y;       z0 = -size;
                            x1 = x + 2*subSize; y1 = y;       z1 = -size;
                            x2 = x + 2*subSize; y2 = y + 2*subSize; z2 = -size;
                            x3 = x;       y3 = y + 2*subSize; z3 = -size;
                            break;
                        case 1: // Back face
                            x0 = x;       y0 = y;       z0 = size;
                            x1 = x + 2*subSize; y1 = y;       z1 = size;
                            x2 = x + 2*subSize; y2 = y + 2*subSize; z2 = size;
                            x3 = x;       y3 = y + 2*subSize; z3 = size;
                            break;
                        case 2: // Left face
                            x0 = -size; z0 = x;       y0 = y;
                            x1 = -size; z1 = x + 2*subSize; y1 = y;
                            x2 = -size; z2 = x + 2*subSize; y2 = y + 2*subSize;
                            x3 = -size; z3 = x;       y3 = y + 2*subSize;
                            break;
                        case 3: // Right face
                            x0 = size; z0 = x;       y0 = y;
                            x1 = size; z1 = x + 2*subSize; y1 = y;
                            x2 = size; z2 = x + 2*subSize; y2 = y + 2*subSize;
                            x3 = size; z3 = x;       y3 = y + 2*subSize;
                            break;
                        case 4: // Top face
                            x0 = x;       y0 = size; z0 = y;
                            x1 = x + 2*subSize; y1 = size; z1 = y;
                            x2 = x + 2*subSize; y2 = size; z2 = y + 2*subSize;
                            x3 = x;       y3 = size; z3 = y + 2*subSize;
                            break;
                        case 5: // Bottom face
                            x0 = x;       y0 = -size; z0 = y;
                            x1 = x + 2*subSize; y1 = -size; z1 = y;
                            x2 = x + 2*subSize; y2 = -size; z2 = y + 2*subSize;
                            x3 = x;       y3 = -size; z3 = y + 2*subSize;
                            break;
                    }

                    glTexCoord2f(u, v); glVertex3f(x0, y0, z0);
                    glTexCoord2f(uNext, v); glVertex3f(x1, y1, z1);
                    glTexCoord2f(uNext, vNext); glVertex3f(x2, y2, z2);
                    glTexCoord2f(u, vNext); glVertex3f(x3, y3, z3);
                }
            }
        }
        glEnd();
        glEndList();

    }

    void UnloadSkybox()
    {
        glDeleteLists(mList, 1);
        mTextureManager->DeleteTexture(mSkyTexture->mName.mID);
    }

    void RenderSky(PerspectiveCamera& camera)
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        camera.Apply();

        if(mList && mSkyTexture) 
        {
            glDisable(GL_CULL_FACE);
            glDisable(GL_LIGHTING);
            glDepthMask(GL_FALSE);

            mSkyTexture->Bind();
            glPushMatrix();
            glTranslatef(camera.mPosition.x, camera.mPosition.y, camera.mPosition.z);
            glCallList(mList);
            glPopMatrix();
        
            glEnable(GL_CULL_FACE);
            glEnable(GL_LIGHTING);
            glDepthMask(GL_TRUE);
        }
    }

    void DoLighting()
    {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, mSunPosition.vector);
        glLightfv(GL_LIGHT0, GL_AMBIENT, mSunAmbient.vector);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, mSunDiffuse.vector);
        glLightfv(GL_LIGHT0, GL_SPECULAR, mSunSpecular.vector);
    }

};
    
} // namespace Engine
