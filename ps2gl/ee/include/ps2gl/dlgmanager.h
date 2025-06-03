/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America

       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */

#ifndef ps2gl_dlgmanager_h
#define ps2gl_dlgmanager_h

#include "ps2gl/gmanager.h"

/********************************************
 * CDListGeomManager - the display list renderer
 */

class CDList;

class CDListGeomManager : public CGeomManager {

    CDmaPacket *CurVertexBuf, *CurNormalBuf, *CurTexCoordBuf, *CurColorBuf;

    CDList* CurDList;
    CGeometryBlock Geometry;

    bool RendererMayHaveChanged;

    void DrawBlock(CGeometryBlock& block);
    void CommitNewGeom();

    void DrawingIndexedArray();
    void DrawingLinearArray();

public:
    CDListGeomManager(CGLContext& context);
    virtual ~CDListGeomManager() {}

    void PrimChanged(GLenum prim);

    // user state

    void EnableCustom(uint64_t flag);
    void DisableCustom(uint64_t flag);

    void SetUserRenderContextChanged() { mError("not implemented yet"); }

    // geometry

    void BeginDListDef();
    void EndDListDef();

    void BeginGeom(GLenum mode);
    void Vertex(pse::math::vec4 newVert);
    void Normal(pse::math::vec3 normal);
    void TexCoord(float u, float v);
    void Color(pse::math::vec4 color);
    void EndGeom();
    void DrawArrays(GLenum mode, int first, int count);
    void DrawIndexedArrays(GLenum primType,
        int numIndices, const unsigned char* indices,
        int numVertices);
    void Flush();
};

#endif // ps2gl_dlgmanager_h
