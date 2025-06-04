/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America

       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */

#ifndef ps2gl_lighting_h
#define ps2gl_lighting_h

#include "GL/gl.h"

#include <core/math.hpp>

#include <core/log.hpp>
#include "ps2gl/dlgmanager.h"
#include "ps2gl/glcontext.h"
#include "ps2gl/immgmanager.h"

class CGLContext;

/********************************************
 * CLight
 */

class CLight {
protected:
    CGLContext& GLContext;
    int LightNum;

public:
    CLight(CGLContext& context, int lightNum)
        : GLContext(context)
        , LightNum(lightNum)
    {
    }

    virtual void SetAmbient(pse::math::vec4 ambient)     = 0;
    virtual void SetDiffuse(pse::math::vec4 diffuse)     = 0;
    virtual void SetSpecular(pse::math::vec4 specular)   = 0;
    virtual void SetPosition(pse::math::vec4 position)   = 0;
    virtual void SetDirection(pse::math::vec4 direction) = 0;
    virtual void SetSpotDirection(pse::math::vec4 dir)   = 0;
    virtual void SetSpotCutoff(float cutoff)          = 0;
    virtual void SetSpotExponent(float exp)           = 0;
    virtual void SetConstantAtten(float atten)        = 0;
    virtual void SetLinearAtten(float atten)          = 0;
    virtual void SetQuadAtten(float atten)            = 0;

    virtual void SetEnabled(bool yesNo) = 0;
};

/********************************************
 * CImmLight
 */

class CImmLight : public CLight {
    pse::math::vec4 Ambient, Diffuse, Specular;
    pse::math::vec4 Position, SpotDirection;
    float SpotCutoff, SpotExponent;
    float ConstantAtten, LinearAtten, QuadAtten;
    bool bIsEnabled;

    // tLightType is defined in rendervsm.h
    tLightType Type;

    static int NumLights[3];

    inline void TellRendererLightPropChanged()
    {
        GLContext.LightPropChanged();
    }

    void CheckTypeChange(tLightType oldType);

public:
    CImmLight(CGLContext& context, int lightNum);

    void SetAmbient(pse::math::vec4 ambient)
    {
        Ambient = ambient;
        TellRendererLightPropChanged();
    }
    void SetDiffuse(pse::math::vec4 diffuse)
    {
        Diffuse = diffuse;
        TellRendererLightPropChanged();
    }
    void SetSpecular(pse::math::vec4 specular);
    void SetPosition(pse::math::vec4 position);
    void SetDirection(pse::math::vec4 direction);

    void SetSpotDirection(pse::math::vec4 dir)
    {
        SpotDirection = dir;
        TellRendererLightPropChanged();
    }
    void SetSpotCutoff(float cutoff)
    {
        tLightType oldType = Type;
        if (Type != kDirectional)
            Type = (cutoff == 180.0f) ? kPoint : kSpot;
        CheckTypeChange(oldType);
        SpotCutoff = cutoff;
        TellRendererLightPropChanged();
    }
    void SetSpotExponent(float exp)
    {
        SpotExponent = exp;
        TellRendererLightPropChanged();
    }

    void SetConstantAtten(float atten)
    {
        ConstantAtten = atten;
        TellRendererLightPropChanged();
    }
    void SetLinearAtten(float atten)
    {
        LinearAtten = atten;
        TellRendererLightPropChanged();
    }
    void SetQuadAtten(float atten)
    {
        QuadAtten = atten;
        TellRendererLightPropChanged();
    }

    void SetEnabled(bool enabled);

    inline pse::math::vec4 GetAmbient() const { return Ambient; }
    inline pse::math::vec4 GetDiffuse() const { return Diffuse; }
    inline pse::math::vec4 GetSpecular() const { return Specular; }
    inline pse::math::vec4 GetPosition() const { return Position; }

    inline pse::math::vec4 GetSpotDir() const { return SpotDirection; }
    inline float GetSpotCutoff() const { return SpotCutoff; }
    inline float GetSpotExponent() const { return SpotExponent; }

    inline float GetConstantAtten() const { return ConstantAtten; }
    inline float GetLinearAtten() const { return LinearAtten; }
    inline float GetQuadAtten() const { return QuadAtten; }

    inline bool IsEnabled() const { return bIsEnabled; }
    inline bool IsDirectional() const { return (Type == kDirectional); }
    inline bool IsPoint() const { return (Type == kPoint); }
    inline bool IsSpot() const { return (Type == kSpot); }
};

/********************************************
 * CDListLight
 */

class CDListLight : public CLight {
    inline void TellRendererLightPropChanged()
    {
        GLContext.LightPropChanged();
    }
    inline void TellRendererLightsEnabledChanged()
    {
        GLContext.NumLightsChanged();
    }

public:
    CDListLight(CGLContext& context, int lightNum)
        : CLight(context, lightNum)
    {
    }

    void SetAmbient(pse::math::vec4 ambient);
    void SetDiffuse(pse::math::vec4 diffuse);
    void SetSpecular(pse::math::vec4 specular);
    void SetPosition(pse::math::vec4 position);
    void SetDirection(pse::math::vec4 direction);

    void SetSpotDirection(pse::math::vec4 dir);
    void SetSpotCutoff(float cutoff);
    void SetSpotExponent(float exp);

    void SetConstantAtten(float atten);
    void SetLinearAtten(float atten);
    void SetQuadAtten(float atten);

    void SetEnabled(bool yesNo);
};

/********************************************
 * CLighting
 */

class CLighting {
protected:
    CGLContext& GLContext;
    static const int NumLights = 8;

public:
    CLighting(CGLContext& context)
        : GLContext(context)
    {
    }
    virtual ~CLighting()
    {
    }

    virtual CLight& GetLight(int num) = 0;

    virtual void SetLightingEnabled(bool enabled)      = 0;
    virtual void SetGlobalAmbient(pse::math::vec4 newAmb) = 0;
};

/********************************************
 * CImmLighting
 */

class CImmLighting : public CLighting {
    pse::math::vec4 CurrentColor;
    pse::math::vec4 GlobalAmbient;
    CImmLight Light0, Light1, Light2, Light3, Light4, Light5, Light6, Light7;
    CImmLight* Lights[NumLights];
    bool IsEnabled;
    int NumLightsWithNonzeroSpecular;

    inline void TellRendererLightPropChanged()
    {
        GLContext.LightPropChanged();
    }

public:
    CImmLighting(CGLContext& context);

    CImmLight& GetImmLight(int num)
    {
        mAssert(num < NumLights);
        return *Lights[num];
    }
    CLight& GetLight(int num) { return GetImmLight(num); }

    void SetLightingEnabled(bool enabled)
    {
        GLContext.LightingEnabledChanged();
        GLContext.GetImmGeomManager().GetRendererManager().LightingEnabledChanged(enabled);
        IsEnabled = enabled;
    }
    bool GetLightingEnabled() const { return IsEnabled; }

    void SetGlobalAmbient(pse::math::vec4 newAmb)
    {
        GlobalAmbient = newAmb;
        TellRendererLightPropChanged();
    }
    pse::math::vec4 GetGlobalAmbient() { return GlobalAmbient; }

    void SpecularChanged();
    void MaterialHasSpecular();
};

/********************************************
 * CDListLighting
 */

class CDListLighting : public CLighting {
    CDListLight Light0, Light1, Light2, Light3, Light4, Light5, Light6, Light7;
    CDListLight* Lights[NumLights];

    inline void TellRendererLightPropChanged()
    {
        GLContext.LightPropChanged();
    }

public:
    CDListLighting(CGLContext& context);

    CDListLight& GetDListLight(int num)
    {
        mAssert(num < NumLights);
        return *Lights[num];
    }
    CLight& GetLight(int num) { return GetDListLight(num); }

    void SetLightingEnabled(bool enabled);
    void SetGlobalAmbient(pse::math::vec4 newAmb);
};

#endif // ps2gl_lighting_h
