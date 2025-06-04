/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America

       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */

#ifndef ps2gl_matrix_h
#define ps2gl_matrix_h

#include <core/log.hpp>
#include <core/math.hpp>

#include "ps2gl/drawcontext.h"
#include "ps2gl/glcontext.h"

/********************************************
 * CMatrixStack
 */

class CGLContext;

class CMatrixStack {
protected:
    CGLContext& GLContext;
    static const int MaxStackDepth = 16;
    pse::math::mat4 Matrices[MaxStackDepth], InverseMatrices[MaxStackDepth];
    int CurStackDepth;

public:
    CMatrixStack(CGLContext& context)
        : GLContext(context)
        , CurStackDepth(0)
    {
        Matrices[0].set_identity();
        InverseMatrices[0].set_identity();
    }
    virtual ~CMatrixStack()
    {
    }

    virtual void Pop()  = 0;
    virtual void Push() = 0;
    virtual void Concat(const pse::math::mat4& xform, const pse::math::mat4& inverse) = 0;
    virtual void SetTop(const pse::math::mat4& newMat, const pse::math::mat4& newInv) = 0;
};

/********************************************
 * CImmMatrixStack
 */

class CImmMatrixStack : public CMatrixStack {
public:
    CImmMatrixStack(CGLContext& context)
        : CMatrixStack(context)
    {
    }

    void Pop()
    {
        mErrorIf(CurStackDepth == 0, "No matrices to pop!");
        --CurStackDepth;
        GLContext.GetImmDrawContext().SetVertexXformValid(false);
    }

    void Push()
    {
        mErrorIf(CurStackDepth == MaxStackDepth - 1,
            "No room on stack!");
        Matrices[CurStackDepth + 1]        = Matrices[CurStackDepth];
        InverseMatrices[CurStackDepth + 1] = InverseMatrices[CurStackDepth];
        ++CurStackDepth;
    }

    void Concat(const pse::math::mat4& xform, const pse::math::mat4& inverse)
    {
        pse::math::mat4& curMat = Matrices[CurStackDepth];
        pse::math::mat4& curInv = InverseMatrices[CurStackDepth];
        curMat             = curMat * xform;
        curInv             = inverse * curInv;
        GLContext.GetImmDrawContext().SetVertexXformValid(false);
    }

    void SetTop(const pse::math::mat4& newMat, const pse::math::mat4& newInv)
    {
        Matrices[CurStackDepth]        = newMat;
        InverseMatrices[CurStackDepth] = newInv;
        GLContext.GetImmDrawContext().SetVertexXformValid(false);
    }

    const pse::math::mat4& GetTop() const { return Matrices[CurStackDepth]; }
    const pse::math::mat4& GetInvTop() const { return InverseMatrices[CurStackDepth]; }
};

/********************************************
 * CDListMatrixStack
 */

class CDListMatrixStack : public CMatrixStack {
public:
    CDListMatrixStack(CGLContext& context)
        : CMatrixStack(context)
    {
    }

    void Pop();
    void Push();
    void Concat(const pse::math::mat4& xform, const pse::math::mat4& inverse);
    void SetTop(const pse::math::mat4& newMat, const pse::math::mat4& newInv);
};

#endif // ps2gl_matrix_h
