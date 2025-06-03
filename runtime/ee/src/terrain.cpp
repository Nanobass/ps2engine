//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        terrain.cpp
//
// Description: 
//
//=============================================================================

#include <terrain.hpp>

//========================================
// System Includes
//========================================

//========================================
// Project Includes
//========================================


void CTerrainChunk::InterpolateVertex(pse::math::vec3* p, uint8_t isoLevel, CTerrainVoxel* v0, pse::math::vec3 p0, CTerrainVoxel* v1, pse::math::vec3 p1)
{
    if(v0->mIntensity == isoLevel) { *p = p0; return; }
    if(v1->mIntensity == isoLevel) { *p = p1; return; }
    if(v0->mIntensity == v1->mIntensity) { *p = p0; return; }
    float mu = (float) (isoLevel - v0->mIntensity) / (float) (v1->mIntensity - v0->mIntensity);
    *p = p0 + mu * (p1 - p0);
}

void CTerrainChunk::GenerateTriangles(int index, pse::math::vec3* vertlist) {
    for (int t = 0; triTable[index][t] != -1; t += 3) {
        pse::math::vec3 p0 = vertlist[triTable[index][t + 0]];
        pse::math::vec3 p1 = vertlist[triTable[index][t + 1]];
        pse::math::vec3 p2 = vertlist[triTable[index][t + 2]];
        
        pse::math::vec3 u = p1 - p0;
        pse::math::vec3 v = p2 - p0;
        pse::math::vec3 cross = u.cross(v);
        pse::math::vec3 normal = cross.normalized();

        pse::math::color color = pse::math::color(normal, 1.0F);

        glColor3f(color.r, color.g, color.b);
        glNormal3f(normal.x, normal.y, normal.z);
        glVertex3f(p0.x, p0.y, p0.z);
        
        glColor3f(color.r, color.g, color.b);
        glNormal3f(normal.x, normal.y, normal.z);
        glVertex3f(p1.x, p1.y, p1.z);
        
        glColor3f(color.r, color.g, color.b);
        glNormal3f(normal.x, normal.y, normal.z);
        glVertex3f(p2.x, p2.y, p2.z);
    }
}

void CTerrainChunk::GenerateMesh() {

    int isoLevel = mTerrain->isoLevel();

    mMesh.mList = glGenLists(1);
    glNewList(mMesh.mList, GL_COMPILE);
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(voxel().x, voxel().y, voxel().z);
    glBegin(GL_TRIANGLES);
    
    for (int x = 0; x < cChunkSize - 1; x++)
    {
        for (int y = 0; y < cChunkSize - 1; y++)
        {
            for (int z = 0; z < cChunkSize - 1; z++)
            {
                uint8_t index = 0;

                if (this->at(x + 0, y + 0, z + 1 /* 0 */ )->mIntensity < isoLevel) index |= 1;
                if (this->at(x + 1, y + 0, z + 1 /* 1 */ )->mIntensity < isoLevel) index |= 2;
                if (this->at(x + 1, y + 0, z + 0 /* 2 */ )->mIntensity < isoLevel) index |= 4;
                if (this->at(x + 0, y + 0, z + 0 /* 3 */ )->mIntensity < isoLevel) index |= 8;
                if (this->at(x + 0, y + 1, z + 1 /* 4 */ )->mIntensity < isoLevel) index |= 16;
                if (this->at(x + 1, y + 1, z + 1 /* 5 */ )->mIntensity < isoLevel) index |= 32;
                if (this->at(x + 1, y + 1, z + 0 /* 6 */ )->mIntensity < isoLevel) index |= 64;
                if (this->at(x + 0, y + 1, z + 0 /* 7 */ )->mIntensity < isoLevel) index |= 128;

                pse::math::vec3 vertlist[12];
                
                uint16_t edge = edgeTable[index];
                if(edge == 0) continue;
                if (edge & 1)    InterpolateVertex(&vertlist[0 ], isoLevel, this->at(x + 0, y + 0, z + 1), pse::math::vec3(x + 0, y + 0, z + 1 /* 0 */) , this->at(x + 1, y + 0, z + 1), pse::math::vec3(x + 1, y + 0, z + 1 /* 1 */));
                if (edge & 2)    InterpolateVertex(&vertlist[1 ], isoLevel, this->at(x + 1, y + 0, z + 1), pse::math::vec3(x + 1, y + 0, z + 1 /* 1 */) , this->at(x + 1, y + 0, z + 0), pse::math::vec3(x + 1, y + 0, z + 0 /* 2 */));
                if (edge & 4)    InterpolateVertex(&vertlist[2 ], isoLevel, this->at(x + 1, y + 0, z + 0), pse::math::vec3(x + 1, y + 0, z + 0 /* 2 */) , this->at(x + 0, y + 0, z + 0), pse::math::vec3(x + 0, y + 0, z + 0 /* 3 */));
                if (edge & 8)    InterpolateVertex(&vertlist[3 ], isoLevel, this->at(x + 0, y + 0, z + 0), pse::math::vec3(x + 0, y + 0, z + 0 /* 3 */) , this->at(x + 0, y + 0, z + 1), pse::math::vec3(x + 0, y + 0, z + 1 /* 0 */));
                if (edge & 16)   InterpolateVertex(&vertlist[4 ], isoLevel, this->at(x + 0, y + 1, z + 1), pse::math::vec3(x + 0, y + 1, z + 1 /* 4 */) , this->at(x + 1, y + 1, z + 1), pse::math::vec3(x + 1, y + 1, z + 1 /* 5 */));
                if (edge & 32)   InterpolateVertex(&vertlist[5 ], isoLevel, this->at(x + 1, y + 1, z + 1), pse::math::vec3(x + 1, y + 1, z + 1 /* 5 */) , this->at(x + 1, y + 1, z + 0), pse::math::vec3(x + 1, y + 1, z + 0 /* 6 */));
                if (edge & 64)   InterpolateVertex(&vertlist[6 ], isoLevel, this->at(x + 1, y + 1, z + 0), pse::math::vec3(x + 1, y + 1, z + 0 /* 6 */) , this->at(x + 0, y + 1, z + 0), pse::math::vec3(x + 0, y + 1, z + 0 /* 7 */));
                if (edge & 128)  InterpolateVertex(&vertlist[7 ], isoLevel, this->at(x + 0, y + 1, z + 0), pse::math::vec3(x + 0, y + 1, z + 0 /* 7 */) , this->at(x + 0, y + 1, z + 1), pse::math::vec3(x + 0, y + 1, z + 1 /* 4 */));
                if (edge & 256)  InterpolateVertex(&vertlist[8 ], isoLevel, this->at(x + 0, y + 0, z + 1), pse::math::vec3(x + 0, y + 0, z + 1 /* 0 */) , this->at(x + 0, y + 1, z + 1), pse::math::vec3(x + 0, y + 1, z + 1 /* 4 */));
                if (edge & 512)  InterpolateVertex(&vertlist[9 ], isoLevel, this->at(x + 1, y + 0, z + 1), pse::math::vec3(x + 1, y + 0, z + 1 /* 1 */) , this->at(x + 1, y + 1, z + 1), pse::math::vec3(x + 1, y + 1, z + 1 /* 5 */));
                if (edge & 1024) InterpolateVertex(&vertlist[10], isoLevel, this->at(x + 1, y + 0, z + 0), pse::math::vec3(x + 1, y + 0, z + 0 /* 2 */) , this->at(x + 1, y + 1, z + 0), pse::math::vec3(x + 1, y + 1, z + 0 /* 6 */));
                if (edge & 2048) InterpolateVertex(&vertlist[11], isoLevel, this->at(x + 0, y + 0, z + 0), pse::math::vec3(x + 0, y + 0, z + 0 /* 3 */) , this->at(x + 0, y + 1, z + 0), pse::math::vec3(x + 0, y + 1, z + 0 /* 7 */));

                GenerateTriangles(index, vertlist);
            }
        }
    }
   

    glEnd();
    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
    glEndList();
}
