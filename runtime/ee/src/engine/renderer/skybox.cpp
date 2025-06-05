//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        skybox.cpp
//
// Description: 
//
//=============================================================================

#include <engine/renderer/skybox.hpp>

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================

namespace pse
{

void skybox_renderer::load_skybox(const std::string& img, float size, int divisions)
{
    if(mList || mSkyTexture) unload_skybox();

    float subSize = size / (float) divisions;

    mSkyTexture = mTextureManager->load_texture(img, img);

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

void skybox_renderer::unload_skybox()
{
    glDeleteLists(mList, 1);
    mTextureManager->delete_texture(mSkyTexture->mName.mUuid);
}

void skybox_renderer::render_sky(perspective_camera& camera, bool render)
{
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    camera.apply();

    if(mList && mSkyTexture && render) 
    {
        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glDepthMask(GL_FALSE);

        mSkyTexture->bind();
        glPushMatrix();
        glTranslatef(camera.mPosition.x, camera.mPosition.y, camera.mPosition.z);
        glCallList(mList);
        glPopMatrix();
        pglFreeTexture(mSkyTexture->mGLName);
    
        glEnable(GL_CULL_FACE);
        glEnable(GL_LIGHTING);
        glDepthMask(GL_TRUE);
    }
}
    
} // namespace pse
