/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America

       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */

#ifndef ps2gl_material_h
#define ps2gl_material_h

#include "GL/gl.h"

#include <ps2math.hpp>

#include "ps2gl/debug.h"
#include "ps2gl/dlgmanager.h"
#include "ps2gl/glcontext.h"
#include "ps2gl/immgmanager.h"

class CGLContext;

class CMaterial {
protected:
    CGLContext& GLContext;

public:
    CMaterial(CGLContext& context)
        : GLContext(context)
    {
    }

    virtual void SetAmbient(pse::math::vec4 ambient)   = 0;
    virtual void SetDiffuse(pse::math::vec4 diffuse)   = 0;
    virtual void SetSpecular(pse::math::vec4 specular) = 0;
    virtual void SetEmission(pse::math::vec4 emission) = 0;
    virtual void SetShininess(float shine)          = 0;
};

class CImmMaterial : public CMaterial {
    pse::math::vec4 Ambient, Diffuse, Specular, Emission;
    float Shininess;

    inline void TellRendererMaterialChanged()
    {
        GLContext.CurMaterialChanged();
    }

public:
    CImmMaterial(CGLContext& context);

    void SetAmbient(pse::math::vec4 ambient)
    {
        Ambient = ambient;
        TellRendererMaterialChanged();
    }
    void SetDiffuse(pse::math::vec4 diffuse)
    {
        Diffuse = diffuse;
        TellRendererMaterialChanged();
    }
    void SetSpecular(pse::math::vec4 specular);
    void SetEmission(pse::math::vec4 emission)
    {
        Emission = emission;
        TellRendererMaterialChanged();
    }
    void SetShininess(float shine)
    {
        Shininess = shine;
        TellRendererMaterialChanged();
    }

    inline pse::math::vec4 GetAmbient() const { return Ambient; }
    inline pse::math::vec4 GetDiffuse() const { return Diffuse; }
    inline pse::math::vec4 GetSpecular() const { return Specular; }
    inline pse::math::vec4 GetEmission() const { return Emission; }
    inline float GetShininess() const { return Shininess; }

    void LightsHaveSpecular();
};

class CDListMaterial : public CMaterial {
    inline void TellRendererMaterialChanged()
    {
        GLContext.CurMaterialChanged();
    }

public:
    CDListMaterial(CGLContext& context)
        : CMaterial(context)
    {
    }

    void SetAmbient(pse::math::vec4 ambient);
    void SetDiffuse(pse::math::vec4 diffuse);
    void SetSpecular(pse::math::vec4 specular);
    void SetEmission(pse::math::vec4 emission);
    void SetShininess(float shine);
};

class CMaterialManager {
    CGLContext& GLContext;

    CImmMaterial ImmMaterial;
    CDListMaterial DListMaterial;
    CMaterial* CurMaterial;

    pse::math::vec4 CurColor;
    GLenum ColorMaterialMode;
    bool UseColorMaterial;
    bool InDListDef;

public:
    CMaterialManager(CGLContext& context)
        : GLContext(context)
        , ImmMaterial(context)
        , DListMaterial(context)
        , CurMaterial(&ImmMaterial)
        , CurColor(1, 1, 1, 1)
        , ColorMaterialMode(GL_AMBIENT_AND_DIFFUSE)
        , UseColorMaterial(false)
        , InDListDef(false)
    {
        ImmMaterial.SetDiffuse(pse::math::vec4(0.8f, 0.8f, 0.8f, 1.0f));
    }

    CMaterial& GetCurMaterial() { return *CurMaterial; }
    CImmMaterial& GetImmMaterial() { return ImmMaterial; }
    CDListMaterial& GetDListMaterial() { return DListMaterial; }
    pse::math::vec4 GetCurColor() const { return CurColor; }
    GLenum GetColorMaterialMode() const { return ColorMaterialMode; }
    bool GetColorMaterialEnabled() const { return UseColorMaterial; }

    void Color(pse::math::vec4 color);
    void SetUseColorMaterial(bool yesNo);
    void SetColorMaterialMode(GLenum mode);

    void BeginDListDef()
    {
        CurMaterial = &DListMaterial;
        InDListDef  = true;
    }
    void EndDListDef()
    {
        CurMaterial = &ImmMaterial;
        InDListDef  = false;
    }
};

#endif // ps2gl_material_h
