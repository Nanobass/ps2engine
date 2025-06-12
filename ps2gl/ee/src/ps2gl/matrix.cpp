/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America

       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */

#include "ps2s/math.h"

#include "ps2gl/dlgmanager.h"
#include "ps2gl/dlist.h"
#include "ps2gl/glcontext.h"
#include "ps2gl/immgmanager.h"
#include "ps2gl/matrix.h"

/********************************************
 * CDListMatrixStack
 */

class CMatrixPopCmd : public CDListCmd {
public:
    CMatrixPopCmd() {}
    CDListCmd* Play()
    {
        glPopMatrix();
        return CDListCmd::GetNextCmd(this);
    }
};

void CDListMatrixStack::Pop()
{
    GLContext.GetDListGeomManager().Flush();
    GLContext.GetDListManager().GetOpenDList() += CMatrixPopCmd();
    GLContext.XformChanged();
}

class CMatrixPushCmd : public CDListCmd {
public:
    CMatrixPushCmd() {}
    CDListCmd* Play()
    {
        glPushMatrix();
        return CDListCmd::GetNextCmd(this);
    }
};

void CDListMatrixStack::Push()
{
    GLContext.GetDListManager().GetOpenDList() += CMatrixPushCmd();
}

class CMatrixConcatCmd : public CDListCmd {
    pse::math::mat4 Matrix, Inverse;

public:
    CMatrixConcatCmd(const pse::math::mat4& mat, const pse::math::mat4& inv)
        : Matrix(mat)
        , Inverse(inv)
    {
    }
    CDListCmd* Play()
    {
        pGLContext->GetCurMatrixStack().Concat(Matrix, Inverse);
        return CDListCmd::GetNextCmd(this);
    }
};

void CDListMatrixStack::Concat(const pse::math::mat4& xform, const pse::math::mat4& inverse)
{
    GLContext.GetDListGeomManager().Flush();
    GLContext.GetDListManager().GetOpenDList() += CMatrixConcatCmd(xform, inverse);
    GLContext.XformChanged();
}

class CMatrixSetTopCmd : public CDListCmd {
    pse::math::mat4 Matrix, Inverse;

public:
    CMatrixSetTopCmd(const pse::math::mat4& mat, const pse::math::mat4& inv)
        : Matrix(mat)
        , Inverse(inv)
    {
    }
    CDListCmd* Play()
    {
        pGLContext->GetCurMatrixStack().SetTop(Matrix, Inverse);
        return CDListCmd::GetNextCmd(this);
    }
};

void CDListMatrixStack::SetTop(const pse::math::mat4& newMat, const pse::math::mat4& newInv)
{
    GLContext.GetDListGeomManager().Flush();
    GLContext.GetDListManager().GetOpenDList() += CMatrixSetTopCmd(newMat, newInv);
    GLContext.XformChanged();
}

/********************************************
 * gl api
 */

void glMatrixMode(GLenum mode)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    pGLContext->SetMatrixMode(mode);
}

void glLoadIdentity(void)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    CMatrixStack& matStack = pGLContext->GetCurMatrixStack();

    pse::math::mat4 ident;
    ident.set_identity();
    matStack.SetTop(ident, ident);
}

void glPushMatrix(void)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    CMatrixStack& matStack = pGLContext->GetCurMatrixStack();
    matStack.Push();
}

void glPopMatrix(void)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    CMatrixStack& matStack = pGLContext->GetCurMatrixStack();
    matStack.Pop();
}

void glLoadMatrixf(const GLfloat* m)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    CMatrixStack& matStack = pGLContext->GetCurMatrixStack();
    pse::math::mat4 newMat;

    // unfortunately we can't assume the matrix is qword-aligned
    float* dest = reinterpret_cast<float*>(&newMat);
    for (int i = 0; i < 16; i++)
        *dest++ = *m++;

    matStack.SetTop(newMat, newMat.inverted());
}

void glFrustum(GLdouble left, GLdouble right,
    GLdouble bottom, GLdouble top,
    GLdouble zNear, GLdouble zFar)
{
    /*
     * NOTE:
     *   The PS2 does not support GL_LESS/GL_LEQUAL
     *   but this is what 99% of OpenGL programs use.
     *
     *   As a result depth is inverted.
     *   See glDepthFunc/glFrustum/glOrtho
     */
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    pse::math::mat4 xform(
            (2.0f * zNear) / (right - left),
            0.0f,
            0.0f,
            0.0f,
            0.0f,
            (2.0f * zNear) / (top - bottom),
            0.0f,
            0.0f,
            (right + left) / (right - left),
            (top + bottom) / (top - bottom),
            -(zFar + zNear) / (zFar - zNear),
            -1.0f,
            0.0f,
            0.0f,
            (-2.0f * zFar * zNear) / (zFar - zNear),
            0.0f
    );

    pse::math::mat4 inv(
            (right - left) / (2 * zNear),
            0,
            0,
            0,
            0,
            (top - bottom) / (2 * zNear),
            0,
            0,
            0,
            0,
            0,
            -(zFar - zNear) / (2 * zFar * zNear),
            (right + left) / (2 * zNear),
            (top + bottom) / (2 * zNear),
            -1,
            (zFar + zNear) / (2 * zFar * zNear)
    );

    CMatrixStack& matStack = pGLContext->GetCurMatrixStack();
    matStack.Concat(xform, inv);
}

void glOrtho(GLdouble left, GLdouble right,
    GLdouble bottom, GLdouble top,
    GLdouble zNear, GLdouble zFar)
{
    /*
     * NOTE:
     *   The PS2 does not support GL_LESS/GL_LEQUAL
     *   but this is what 99% of OpenGL programs use.
     *
     *   As a result depth is inverted.
     *   See glDepthFunc/glFrustum/glOrtho
     */
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    pse::math::mat4 xform(
            (2.0f) / (right - left),
            0.0f,
            0.0f,
            0.0f,
            0.0f,
            (2.0f) / (top - bottom),
            0.0f,
            0.0f,
            0.0f,
            0.0f,
            -2 / (zFar - zNear),
            0.0f,
            -(right + left) / (right - left),
            -(top + bottom) / (top - bottom),
            -(zFar + zNear) / (zFar - zNear),
            1.0f
    );

    pse::math::mat4 inv(
            (right - left) / 2,
            0,
            0,
            0,
            0,
            (top - bottom) / 2,
            0,
            0,
            0,
            0,
            (zFar - zNear) / -2,
            0,
            (right + left) / 2,
            (top + bottom) / 2,
            (zFar + zNear) / 2,
            1
    );

    CMatrixStack& matStack = pGLContext->GetCurMatrixStack();
    matStack.Concat(xform, inv);
}

void glMultMatrixf(const GLfloat* m)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    // unfortunately we can't assume the matrix is qword-aligned
    // the casts to float below fix an apparent parse error... something's up here..
    pse::math::mat4 newMatrix(
        m[0], m[1], m[2], m[3],
        m[4], m[5], m[6], m[7],
        m[8], m[9], m[10], m[11],
        m[12], m[13], m[14], m[15]);

    // close your eyes.. this is a temporary hack

    /*
   // assume that newMatrix consists of rotations, uniform scales, and translations
   pse::math::vec4 scaledvec( 1, 0, 0, 0 );
   scaledvec = newMatrix * scaledvec;
   float scale = scaledvec.length();

   pse::math::mat4 invMatrix = newMatrix;
   invMatrix.set_col3( pse::math::vec4(0,0,0,0) );
   invMatrix.transpose_in_place();
   invMatrix.set_col3( -newMatrix.get_col3() );
   pse::math::mat4 scaleMat;
   scaleMat.set_scale( pse::math::vec3(scale, scale, scale) );
   invMatrix = scaleMat * invMatrix;
   */

    CMatrixStack& matStack = pGLContext->GetCurMatrixStack();
    matStack.Concat(newMatrix, newMatrix.inverted());

    //     mWarn( "glMultMatrix is not correct" );
}

void glRotatef(GLfloat angle,
    GLfloat x, GLfloat y, GLfloat z)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    pse::math::mat4 xform, inverse;
    pse::math::vec3 axis(x, y, z);
    axis.normalize();
    xform.set_rotate(angle, axis);
    inverse.set_rotate(-angle, axis);

    CMatrixStack& matStack = pGLContext->GetCurMatrixStack();
    matStack.Concat(xform, inverse);
}

void glScalef(GLfloat x, GLfloat y, GLfloat z)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    pse::math::mat4 xform, inverse;
    xform.set_scale(pse::math::vec3(x, y, z));
    inverse.set_scale(pse::math::vec3(1.0f / x, 1.0f / y, 1.0f / z));

    CMatrixStack& matStack = pGLContext->GetCurMatrixStack();
    matStack.Concat(xform, inverse);
}

void glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    pse::math::mat4 xform, inverse;
    pse::math::vec3 direction(x, y, z);
    xform.set_translate(direction);
    inverse.set_translate(-direction);

    CMatrixStack& matStack = pGLContext->GetCurMatrixStack();
    matStack.Concat(xform, inverse);
}

void pglLoadMatrix(const pse::math::mat4& matrix)
{
    CMatrixStack& matStack = pGLContext->GetCurMatrixStack();
    matStack.SetTop(matrix, matrix.inverted());
}

void pglMultMatrix(const pse::math::mat4& matrix)
{
    CMatrixStack& matStack = pGLContext->GetCurMatrixStack();
    matStack.Concat(matrix, matrix.inverted());
}

void pglLoadMatrixCombined(const pse::math::mat4& matrix, const pse::math::mat4& inverted)
{
    CMatrixStack& matStack = pGLContext->GetCurMatrixStack();
    matStack.SetTop(matrix, inverted);
}

void pglMultMatrixCombined(const pse::math::mat4& matrix, const pse::math::mat4& inverted)
{
    CMatrixStack& matStack = pGLContext->GetCurMatrixStack();
    matStack.Concat(matrix, inverted);
}