/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America

       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */

#include "ps2gl/lighting.h"
#include "ps2gl/dlist.h"
#include "ps2gl/glcontext.h"
#include "ps2gl/material.h"
#include "ps2gl/matrix.h"

/********************************************
 * CImmLight
 */

// static data

int CImmLight::NumLights[3] = { 0, 0, 0 };

CImmLight::CImmLight(CGLContext& context, int lightNum)
    : CLight(context, lightNum)
    , Ambient(0.0f, 0.0f, 0.0f, 1.0f)
    , Diffuse(0.0f, 0.0f, 0.0f, 0.0f)
    , Specular(0.0f, 0.0f, 0.0f, 0.0f)
    , Position(0.0f, 0.0f, 1.0f, 0.0f)
    , SpotDirection(0.0f, 0.0f, -1.0f, 0.0f)
    , SpotCutoff(180.0f)
    , SpotExponent(0.0f)
    , ConstantAtten(1.0f)
    , LinearAtten(0.0f)
    , QuadAtten(0.0f)
    , bIsEnabled(false)
    , Type(kDirectional)
{
    if (lightNum == 0) {
        // Light0 has different initial values
        Diffuse  = pse::math::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        Specular = pse::math::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

void CImmLight::CheckTypeChange(tLightType oldType)
{
    if (oldType != Type && bIsEnabled) {
        CRendererManager& rm = GLContext.GetImmGeomManager().GetRendererManager();
        NumLights[oldType]--;
        NumLights[Type]++;
        rm.NumLightsChanged(oldType, NumLights[oldType]);
        rm.NumLightsChanged(Type, NumLights[Type]);
    }
}

void CImmLight::SetEnabled(bool enabled)
{
    if (bIsEnabled != enabled) {
        bIsEnabled = enabled;
        if (enabled)
            NumLights[Type]++;
        else
            NumLights[Type]--;

        GLContext.GetImmLighting().SpecularChanged();

        CRendererManager& rm = GLContext.GetImmGeomManager().GetRendererManager();
        rm.NumLightsChanged(Type, NumLights[Type]);
    }
}

void CImmLight::SetSpecular(pse::math::vec4 specular)
{
    pse::math::vec4 zero(0, 0, 0, 0);
    if ((specular == zero) ^ (Specular == zero)) {
        Specular = specular;
        GLContext.GetImmLighting().SpecularChanged();
    } else
        Specular = specular;

    TellRendererLightPropChanged();
}

void CImmLight::SetPosition(pse::math::vec4 position)
{
    CImmMatrixStack& modelView = GLContext.GetModelViewStack();

    Position = modelView.GetTop() * position;
    TellRendererLightPropChanged();

    tLightType oldType = Type;
    Type               = (SpotCutoff == 180.0f) ? kPoint : kSpot;
    CheckTypeChange(oldType);
}

void CImmLight::SetDirection(pse::math::vec4 direction)
{
    CImmMatrixStack& modelView = GLContext.GetModelViewStack();

    Position = modelView.GetTop() * direction.normalize();
    TellRendererLightPropChanged();

    tLightType oldType = Type;
    Type               = kDirectional;
    CheckTypeChange(oldType);
}

/********************************************
 * CDListLight
 */

class CLightPropCmd : public CDListCmd {
    GLenum LightNum, Property;
    pse::math::vec4 Value;

public:
    CLightPropCmd(GLenum lightNum, GLenum prop, pse::math::vec4 value)
        : LightNum(lightNum)
        , Property(prop)
        , Value(value)
    {
    }
    CDListCmd* Play()
    {
        glLightfv(LightNum, Property, reinterpret_cast<float*>(&Value));
        return CDListCmd::GetNextCmd(this);
    }
};

void CDListLight::SetAmbient(pse::math::vec4 ambient)
{
    GLContext.GetDListManager().GetOpenDList()
        += CLightPropCmd(GL_LIGHT0 | LightNum, GL_AMBIENT, ambient);
    TellRendererLightPropChanged();
}

void CDListLight::SetDiffuse(pse::math::vec4 diffuse)
{
    GLContext.GetDListManager().GetOpenDList()
        += CLightPropCmd(GL_LIGHT0 | LightNum, GL_DIFFUSE, diffuse);
    TellRendererLightPropChanged();
}

void CDListLight::SetSpecular(pse::math::vec4 specular)
{
    GLContext.GetDListManager().GetOpenDList()
        += CLightPropCmd(GL_LIGHT0 | LightNum, GL_SPECULAR, specular);
    TellRendererLightPropChanged();
    GLContext.SpecularEnabledChanged(); // maybe
}

void CDListLight::SetPosition(pse::math::vec4 position)
{
    GLContext.GetDListManager().GetOpenDList()
        += CLightPropCmd(GL_LIGHT0 | LightNum, GL_POSITION, position);
    TellRendererLightPropChanged();
}

void CDListLight::SetDirection(pse::math::vec4 direction)
{
    GLContext.GetDListManager().GetOpenDList()
        += CLightPropCmd(GL_LIGHT0 | LightNum, GL_POSITION, direction);
    TellRendererLightPropChanged();
}

void CDListLight::SetSpotDirection(pse::math::vec4 dir)
{
    GLContext.GetDListManager().GetOpenDList()
        += CLightPropCmd(GL_LIGHT0 | LightNum, GL_SPOT_DIRECTION, dir);
    TellRendererLightPropChanged();
}

void CDListLight::SetSpotCutoff(float cutoff)
{
    GLContext.GetDListManager().GetOpenDList()
        += CLightPropCmd(GL_LIGHT0 | LightNum, GL_SPOT_CUTOFF, pse::math::vec4(cutoff, 0, 0, 0));
    TellRendererLightPropChanged();
}

void CDListLight::SetSpotExponent(float exp)
{
    GLContext.GetDListManager().GetOpenDList()
        += CLightPropCmd(GL_LIGHT0 | LightNum, GL_AMBIENT, pse::math::vec4(exp, 0, 0, 0));
    TellRendererLightPropChanged();
}

void CDListLight::SetConstantAtten(float atten)
{
    GLContext.GetDListManager().GetOpenDList()
        += CLightPropCmd(GL_LIGHT0 | LightNum, GL_CONSTANT_ATTENUATION, pse::math::vec4(atten, 0, 0, 0));
    TellRendererLightPropChanged();
}

void CDListLight::SetLinearAtten(float atten)
{
    GLContext.GetDListManager().GetOpenDList()
        += CLightPropCmd(GL_LIGHT0 | LightNum, GL_LINEAR_ATTENUATION, pse::math::vec4(atten, 0, 0, 0));
    TellRendererLightPropChanged();
}

void CDListLight::SetQuadAtten(float atten)
{
    GLContext.GetDListManager().GetOpenDList()
        += CLightPropCmd(GL_LIGHT0 | LightNum, GL_QUADRATIC_ATTENUATION, pse::math::vec4(atten, 0, 0, 0));
    TellRendererLightPropChanged();
}

void CDListLight::SetEnabled(bool yesNo)
{
    GLContext.GetDListManager().GetOpenDList()
        += CEnableCmd(GL_LIGHT0 | LightNum);
    TellRendererLightsEnabledChanged();
}

/********************************************
 * CLighting
 */

CImmLighting::CImmLighting(CGLContext& context)
    : CLighting(context)
    , CurrentColor(0.0f, 0.0f, 0.0f, 0.0f)
    , GlobalAmbient(0.0f, 0.0f, 0.0f, 0.0f)
    , Light0(context, 0)
    , Light1(context, 1)
    , Light2(context, 2)
    , Light3(context, 3)
    , Light4(context, 4)
    , Light5(context, 5)
    , Light6(context, 6)
    , Light7(context, 7)
    , IsEnabled(false)
    , NumLightsWithNonzeroSpecular(0)
{
    Lights[0] = &Light0;
    Lights[1] = &Light1;
    Lights[2] = &Light2;
    Lights[3] = &Light3;
    Lights[4] = &Light4;
    Lights[5] = &Light5;
    Lights[6] = &Light6;
    Lights[7] = &Light7;
}

void CImmLighting::SpecularChanged()
{
    int count = 0;
    pse::math::vec4 zero(0, 0, 0, 0);
    for (int i = 0; i < 8; i++)
        if (Lights[i]->IsEnabled() && Lights[i]->GetSpecular() != zero)
            count++;

    NumLightsWithNonzeroSpecular = count;
    if (NumLightsWithNonzeroSpecular == 0) {
        GLContext.SpecularEnabledChanged();
        GLContext.GetImmGeomManager().GetRendererManager().SpecularEnabledChanged(false);
    } else
        GLContext.GetMaterialManager().GetImmMaterial().LightsHaveSpecular();
}

void CImmLighting::MaterialHasSpecular()
{
    if (NumLightsWithNonzeroSpecular > 0) {
        GLContext.SpecularEnabledChanged();
        GLContext.GetImmGeomManager().GetRendererManager().SpecularEnabledChanged(true);
    }
}

CDListLighting::CDListLighting(CGLContext& context)
    : CLighting(context)
    , Light0(context, 0)
    , Light1(context, 1)
    , Light2(context, 2)
    , Light3(context, 3)
    , Light4(context, 4)
    , Light5(context, 5)
    , Light6(context, 6)
    , Light7(context, 7)
{
    Lights[0] = &Light0;
    Lights[1] = &Light1;
    Lights[2] = &Light2;
    Lights[3] = &Light3;
    Lights[4] = &Light4;
    Lights[5] = &Light5;
    Lights[6] = &Light6;
    Lights[7] = &Light7;
}

class CSetLightingEnabledCmd : public CDListCmd {
    bool IsEnabled;

public:
    CSetLightingEnabledCmd(bool enabled)
        : IsEnabled(enabled)
    {
    }
    CDListCmd* Play()
    {
        pGLContext->GetImmLighting().SetLightingEnabled(IsEnabled);
        return CDListCmd::GetNextCmd(this);
    }
};

void CDListLighting::SetLightingEnabled(bool enabled)
{
    GLContext.GetDListManager().GetOpenDList() += CSetLightingEnabledCmd(enabled);
    GLContext.LightingEnabledChanged();
}

class CSetGlobalAmbientCmd : public CDListCmd {
    pse::math::vec4 Ambient;

public:
    CSetGlobalAmbientCmd(pse::math::vec4 newAmb)
        : Ambient(newAmb)
    {
    }
    CDListCmd* Play()
    {
        pGLContext->GetImmLighting().SetGlobalAmbient(Ambient);
        return CDListCmd::GetNextCmd(this);
    }
};

void CDListLighting::SetGlobalAmbient(pse::math::vec4 newAmb)
{
    GLContext.GetDListManager().GetOpenDList() += CSetGlobalAmbientCmd(newAmb);
    TellRendererLightPropChanged();
}

/********************************************
 * gl interface
 */

void setPosition(CLight& light, float x, float y, float z, float w)
{
    pse::math::vec4 pos(x, y, z, w);
    if (w == 0.0f)
        light.SetDirection(pos);
    else
        light.SetPosition(pos);
}

void glLightfv(GLenum lightNum,
    GLenum pname,
    const GLfloat* params)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    CLighting& lighting = pGLContext->GetLighting();
    CLight& light       = lighting.GetLight(0x7 & lightNum);

    switch (pname) {
    case GL_AMBIENT:
        light.SetAmbient(pse::math::vec4(params[0], params[1], params[2], params[3]));
        break;
    case GL_DIFFUSE:
        light.SetDiffuse(pse::math::vec4(params[0], params[1], params[2], params[3]));
        break;
    case GL_SPECULAR:
        light.SetSpecular(pse::math::vec4(params[0], params[1], params[2], params[3]));
        break;
    case GL_POSITION:
        setPosition(light, params[0], params[1], params[2], params[3]);
        break;
    case GL_SPOT_DIRECTION:
        light.SetPosition(pse::math::vec4(params[0], params[1], params[2], 0.0f).normalize());
        break;
    case GL_SPOT_EXPONENT:
        light.SetSpotExponent(*params);
        break;
    case GL_SPOT_CUTOFF:
        light.SetSpotCutoff(*params);
        break;
    case GL_CONSTANT_ATTENUATION:
        light.SetConstantAtten(*params);
        break;
    case GL_LINEAR_ATTENUATION:
        light.SetLinearAtten(*params);
        break;
    case GL_QUADRATIC_ATTENUATION:
        light.SetQuadAtten(*params);
        break;
    default:
        mError("Shouldn't get here.");
    }
}

void glLightf(GLenum lightNum, GLenum pname, GLfloat param)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    CLighting& lighting = pGLContext->GetLighting();
    CLight& light       = lighting.GetLight(0x7 & lightNum);

    switch (pname) {
    case GL_SPOT_EXPONENT:
        light.SetSpotExponent(param);
        break;
    case GL_SPOT_CUTOFF:
        light.SetSpotCutoff(param);
        break;
    case GL_CONSTANT_ATTENUATION:
        light.SetConstantAtten(param);
        break;
    case GL_LINEAR_ATTENUATION:
        light.SetLinearAtten(param);
        break;
    case GL_QUADRATIC_ATTENUATION:
        light.SetQuadAtten(param);
        break;
    default:
        mError("Shouldn't get here.");
    }
}

void glLightModelfv(GLenum pname, const GLfloat* params)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    switch (pname) {
    case GL_LIGHT_MODEL_AMBIENT:
        pGLContext->GetLighting().SetGlobalAmbient(pse::math::vec4(params[0],
            params[1],
            params[2],
            params[3]));
        break;
    case GL_LIGHT_MODEL_COLOR_CONTROL:
        if ((int)*params == GL_SEPARATE_SPECULAR_COLOR) {
            mNotImplemented("separate specular color computation");
        }
        break;
    case GL_LIGHT_MODEL_LOCAL_VIEWER:
        if ((int)*params != 0) {
            mNotImplemented("local viewer");
        }
        break;
    case GL_LIGHT_MODEL_TWO_SIDE:
        if ((int)*params != 0) {
            mNotImplemented("two-sided lighting");
        }
        break;
    default:
        mError("shouldn't get here");
    }
}

void glLightModeli(GLenum pname, int param)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    mNotImplemented();
}

void glGetLightfv(GLenum light, GLenum pname, float* params)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    mNotImplemented();
}

void glFogi(GLenum pname, GLfloat param)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    mNotImplemented();
}

void glFogf(GLenum pname, GLfloat param)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    mNotImplemented();
}

void glFogfv(GLenum pname, const GLfloat* params)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    mNotImplemented();
}

void glFogiv(GLenum pname, const GLint* params)
{
    GL_FUNC_DEBUG("%s\n", __FUNCTION__);

    mNotImplemented();
}
