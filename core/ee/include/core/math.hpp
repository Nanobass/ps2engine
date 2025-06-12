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
        float vector[4] __attribute__ ((aligned(16)));
        struct { float x, y, z, w; };
        struct { float r, g, b, a; };
    };

    vec4();
    vec4(float x, float y, float z, float w = 1.0F);
    vec4(const vec4& vector);
    vec4(const vec3& vector, float w);
    vec4(const vec2& vector, float z, float w);

    vec4& set(float x, float y, float z, float w);
    vec4& set(const vec4& vector);
    vec4& set(const vec3& vector, float w);
    vec4& set(const vec2& vector, float z, float w);

    vec4& operator=(const vec4& vector);
    vec4& operator=(const vec3& vector);
    vec4& operator=(const vec2& vector);

    float dot(const vec4& other) const;
    float length() const;
    vec4 normalized() const;
    vec4& normalize();

    vec4 operator+(const vec4& other) const;
    vec4& operator+=(const vec4& other);

    vec4 operator-(const vec4& other) const;
    vec4& operator-=(const vec4& other);

    vec4 operator*(const vec4& other) const;
    vec4& operator*=(const vec4& other);
    vec4 operator*(float scalar) const;
    vec4& operator*=(float scalar);

    vec4 operator/(const vec4& other) const;
    vec4& operator/=(const vec4& other);
    vec4 operator/(float scalar) const;
    vec4& operator/=(float scalar);

    vec4 operator-() const;
    bool operator==(const vec4& rhs) const;
    bool operator!=(const vec4& rhs) const;
    
    float& operator[] (int i) { return vector[i]; }
    const float& operator[] (int i) const { return vector[i]; }

    friend std::ostream& operator<<(std::ostream& os, const vec4& vector);

};
 
struct vec3 
{
    union
    {
        struct { float x, y, z; };
    };

    vec3();
    vec3(float x, float y, float z);
    vec3(const vec4& vector);
    vec3(const vec3& vector);
    vec3(const vec2& vector, float z);

    void set(float x, float y, float z);
    void set(const vec4& vector);
    void set(const vec3& vector);
    void set(const vec2& vector, float z);

    vec3& operator=(const vec4& vector);
    vec3& operator=(const vec3& vector);
    vec3& operator=(const vec2& vector);

    float dot(const vec3& other) const;
    float length() const;
    vec3 cross(const vec3& other) const;
    vec3 normalized() const;
    vec3& normalize();

    vec3 operator+(const vec3& other) const;
    vec3& operator+=(const vec3& other);

    vec3 operator-(const vec3& other) const;
    vec3& operator-=(const vec3& other);

    vec3 operator*(const vec3& other) const;
    vec3& operator*=(const vec3& other);
    vec3 operator*(float scalar) const;
    vec3& operator*=(float scalar);

    vec3 operator/(const vec3& other) const;
    vec3& operator/=(const vec3& other);
    vec3 operator/(float scalar) const;
    vec3& operator/=(float scalar);

    vec3 operator-() const;
    bool operator==(const vec3& rhs) const;
    bool operator!=(const vec3& rhs) const;

    friend std::ostream& operator<<(std::ostream& os, const vec3& vector);

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

    float dot(const vec2& other) const;
    float length() const;
    vec2 normalized() const;
    const vec2& normalize();
    
    vec2 operator+(const vec2& other) const;
    vec2& operator+=(const vec2& other);

    vec2 operator-(const vec2& other) const;
    vec2& operator-=(const vec2& other);

    vec2 operator*(const vec2& other) const;
    vec2& operator*=(const vec2& other);
    vec2 operator*(float scalar) const;
    vec2& operator*=(float scalar);

    vec2 operator/(const vec2& other) const;
    vec2& operator/=(const vec2& other);
    vec2 operator/(float scalar) const;
    vec2& operator/=(float scalar);

    vec2 operator-() const;
    bool operator==(const vec2& rhs) const;
    bool operator!=(const vec2& rhs) const;

    friend std::ostream& operator<<(std::ostream& os, const vec2& vector);
};

struct mat4 
{
    union
    {
        float matrix[16] __attribute__ ((aligned(16)));
        vec4 m[4];
    };

    mat4() { set_identity(); }
    mat4(const mat4& matrix) { set(matrix); }
    mat4(float m00, float m01, float m02, float m03,
         float m10, float m11, float m12, float m13,
         float m20, float m21, float m22, float m23,
         float m30, float m31, float m32, float m33)
    { set(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33); }
    
    mat4& set_identity();
    mat4& set_translate(const vec3& position);
    mat4& set_rotate(const float& rotation, const vec3& axis);
    mat4& set_rotate_x(const float& rotation);
    mat4& set_rotate_y(const float& rotation);
    mat4& set_rotate_z(const float& rotation);
    mat4& set_rotate(const vec3& rotation);
    mat4& set_scale(const vec3& scale);
    mat4& set(const mat4& other);
    mat4& set(float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33);
    
    mat4& operator=(const mat4& vector);

    mat4& transpose();
    mat4 transposed() const;

    mat4& invert();
    mat4 inverted() const;

    mat4 operator*(const mat4& other) const;
    mat4& operator*=(const mat4& other);

    vec4 operator*(const vec4& vector) const;
    
    vec4& operator[] (int i) { return m[i]; }
    const vec4& operator[] (int i) const { return m[i]; }

    friend std::ostream& operator<<(std::ostream& os, const mat4& matrix);

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
// Matrix Functions
//

mat4 translation(const vec3& position);
mat4 rotationX(const float& r);
mat4 rotationY(const float& r);
mat4 rotationZ(const float& r);
mat4 rotation(const vec3& rotation);
mat4 scale(const vec3& scale);
mat4 transformation(const vec3& position, const vec3& rotation, const vec3& scale);

} // namespace pse::math