//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        math.hpp
//
// Description: Math System Foundation
//
//=============================================================================

#pragma once

//========================================
// System Includes
//========================================

#include <cmath>
#include <ostream>

//========================================
// PS2SDK Includes
//========================================

#include <math3d.h>

//========================================
// Project Includes
//========================================

namespace pse::math {

struct vec4;
struct vec3;
struct vec2;
using color = vec4;
using texel = vec2;

struct vec4
{
    union
    {
        float vector[4] __attribute__((__aligned__(16)));
        struct { float x,y,z,w; };
        struct { float r,g,b,a; };
    };

    vec4();
    vec4(float x, float y, float z, float w = 1.0F);
    vec4(const vec4& vector);
    vec4(const vec3& vector, float w);

    void set(float x, float y, float z, float w);
    void set(const vec4& vector);
    void set(const vec3& vector, float w);

    vec4& operator=(const vec4& vector);
    vec4& operator=(const vec3& vector);

    float dot(const vec4& other);
    float length();
    vec4 normalized();
    const vec4& normalize();

    inline bool operator==(const vec4& rhs)
    {
        return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
    }

    inline bool operator!=(const vec4& rhs)
    {
        return (x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w);
    }

};
 
struct vec3 
{
    union
    {
        struct { float x,y,z; };
    };

    vec3();
    vec3(float x, float y, float z);
    vec3(const vec4& vector);
    vec3(const vec3& vector);

    void set(float x, float y, float z);
    void set(const vec4& vector);
    void set(const vec3& vector);

    vec3& operator=(const vec4& vector);
    vec3& operator=(const vec3& vector);

    float dot(const vec3& other);
    float length();
    vec3 cross(const vec3& other);
    vec3 normalized();
    const vec3& normalize();

    inline bool operator==(const vec3& rhs)
    {
        return (x == rhs.x && y == rhs.y && z == rhs.z);
    }

    inline bool operator!=(const vec3& rhs)
    {
        return (x != rhs.x || y != rhs.y || z != rhs.z);
    }

};

struct vec2
{
    union
    {
        struct { float x,y; };
        struct { float u,v; };
    };

    vec2();
    vec2(float x, float y);
    vec2(const vec2& vector);

    void set(float x, float y);
    void set(const vec2& vector);

    vec2& operator=(const vec2& vector);

    float dot(const vec2& other);
    float length();
    vec2 normalized();
    const vec2& normalize();
    
    inline bool operator==(const vec2& rhs)
    {
        return (x == rhs.x && y == rhs.y);
    }

    inline bool operator!=(const vec2& rhs)
    {
        return (x != rhs.x || y != rhs.y);
    }
};

struct mat4 
{
    union
    {
        float matrix[16] __attribute__((__aligned__(16)));
        float m[4][4];
        vec4 columns[4];
    };

    mat4();
    mat4(const mat4& matrix);
    mat4(float m00, float m01, float m02, float m03,
         float m10, float m11, float m12, float m13,
         float m20, float m21, float m22, float m23,
         float m30, float m31, float m32, float m33)
    {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
        m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    }

    void set_identity();
    void set_translate(const vec3& position);
    void set_rotate(const float& rotation, const vec3& axis);
    void set_scale(const vec3& scale);

    mat4 transpose();

    void operator=(const mat4& vector);
};

//
// Constants
//

constexpr float HALF_ANG2RAD = 3.14159265358979323846F / 360.0F;
constexpr float ANG2RAD = 3.14159265358979323846F / 180.0F;
constexpr float PI = 3.1415926535897932384626433832795F;
constexpr float HALF_PI = 1.5707963267948966192313216916398F;

//
// Math Functions
//

float sin(const float& x);
float cos(float x);
float tan(const float& x);
float asin(float x);
float acos(const float& x);
float atan2(float y, float x);

//
//  vec4 Math
//

vec4 operator+(const vec4& lhs, const vec4& rhs);
void operator+=(vec4& lhs, const vec4& rhs);

vec4 operator-(const vec4& lhs, const vec4& rhs);
void operator-=(vec4& lhs, const vec4& rhs);

vec4 operator*(const vec4& lhs, const vec4& rhs);
void operator*=(vec4& lhs, const vec4& rhs);

vec4 operator*(const vec4& vector, float scalar);
vec4 operator*(float scalar, const vec4& vector);
void operator*=(vec4& vector, float scalar);

vec4 operator/(const vec4& lhs, const vec4& rhs);
void operator/=(vec4& lhs, const vec4& rhs);

vec4 operator/(const vec4& vector, float scalar);
vec4 operator/(float scalar, const vec4& vector);
void operator/=(vec4& vector, float scalar);

vec4 operator-(const vec4& vector);

std::ostream& operator<<(std::ostream& os, const vec4& vector);

//
//  vec3 Math
//

vec3 operator+(const vec3& lhs, const vec3& rhs);
void operator+=(vec3& lhs, const vec3& rhs);

vec3 operator-(const vec3& lhs, const vec3& rhs);
void operator-=(vec3& lhs, const vec3& rhs);

vec3 operator*(const vec3& lhs, const vec3& rhs);
void operator*=(vec3& lhs, const vec3& rhs);

vec3 operator*(const vec3& vector, float scalar);
vec3 operator*(float scalar, const vec3& vector);
void operator*=(vec3& vector, float scalar);

vec3 operator/(const vec3& lhs, const vec3& rhs);
void operator/=(vec3& lhs, const vec3& rhs);

vec3 operator/(const vec3& vector, float scalar);
vec3 operator/(float scalar, const vec3& vector);
void operator/=(vec3& vector, float scalar);

vec3 operator-(const vec3& vector);

std::ostream& operator<<(std::ostream& os, const vec3& vector);

//
//  vec2 Math
//

vec2 operator+(const vec2& lhs, const vec2& rhs);
void operator+=(vec2& lhs, const vec2& rhs);

vec2 operator-(const vec2& lhs, const vec2& rhs);
void operator-=(vec2& lhs, const vec2& rhs);

vec2 operator*(const vec2& lhs, const vec2& rhs);
void operator*=(vec2& lhs, const vec2& rhs);

vec2 operator*(const vec2& vector, float scalar);
vec2 operator*(float scalar, const vec2& vector);
void operator*=(vec2& vector, float scalar);

vec2 operator/(const vec2& lhs, const vec2& rhs);
void operator/=(vec2& lhs, const vec2& rhs);

vec2 operator/(const vec2& vector, float scalar);
vec2 operator/(float scalar, const vec2& vector);
void operator/=(vec2& vector, float scalar);

vec2 operator-(const vec2& vector);

std::ostream& operator<<(std::ostream& os, const vec2& vector);

//
//  Matrix Math
//

mat4 operator*(const mat4& lhs, const mat4& rhs);

vec4 operator*(const mat4& matrix, const vec4& vector);
vec4 operator*(const vec4& vector, const mat4& matrix);

std::ostream& operator<<(std::ostream& os, const mat4& matrix);

void identity(const mat4& matrix);

mat4 transpose(const mat4& matrix);

mat4 translation(const vec4& positon);

mat4 rotationX(const float& r);
mat4 rotationY(const float& r);
mat4 rotationZ(const float& r);
mat4 rotation(const vec4& rotation);

mat4 scale(const vec4& scale);

mat4 transformation(const vec4& position, const vec4& rotation, const vec4& scale);

mat4 lookAt(const vec4& position, const vec4& lookAt, const vec4& up);

mat4 projection(const float& fov, const float& width, const float& height, const float& scale, const float& ratio, const float& near, const float& far);

} // namespace pse::math