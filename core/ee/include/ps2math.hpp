#pragma once

#include <math3d.h>
#include <cmath>
#include <ostream>

namespace pse::math {

struct vec4;
struct vec3;
struct vec2;
using color = vec4;
using texel = vec2;

struct mat4 
{
    union
    {
        MATRIX matrix alignas(sizeof(float) * 16);
        float m[4][4];
    };

    mat4();
    mat4(const mat4& matrix);

    void operator=(const mat4& vector);
};

struct vec4
{
    union
    {
        VECTOR vector alignas(sizeof(float) * 4);
        struct { float x,y,z,w; };
        struct { float r,g,b,a; };
    };

    vec4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);
    vec4(const vec4& vector);
    vec4(const vec3& vector, float w = 1.0f);
    vec4(const vec2& vector, float z = 0.0f, float w = 1.0f);

    void operator=(const vec4& vector);
};
 
struct vec3 
{
    union
    {
        struct { float x,y,z; };
    };

    vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f);
    vec3(const vec4& vector);
    vec3(const vec3& vector);
    vec3(const vec2& vector);

    void operator=(const vec3& vector);
};

struct vec2
{
    union
    {
        struct { float x,y; };
        struct { float u,v; };
    };

    vec2(float x = 0.0f, float y = 0.0f);
    vec2(const vec4& vector);
    vec2(const vec3& vector);
    vec2(const vec2& vector);

    void operator=(const vec2& vector);
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

float cos(float x);
float asin(float x);
float atan2(float y, float x);
float mod(const float& x, const float& y);
float acos(const float& x);
float sin(const float& x);
float tan(const float& x);
float isqrt(const float& x);
float randomf(const float& min, const float& max);
int randomi(const int& min, const int& max);
bool equalf(const float& a, const float& b, const float& epsilon = 0.00001F);

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

float dot(const vec4& lhs, const vec4& rhs);
float dot3(const vec4& lhs, const vec4& rhs);
vec4 cross(const vec4& lhs, const vec4& rhs);
vec4 normalize(const vec4& vector);
vec4 normalize3(const vec4& vector);
vec4 lerp(const vec4& lhs, const vec4& rhs, float f);

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

float dot(const vec3& lhs, const vec3& rhs);
vec3 cross(const vec3& lhs, const vec3& rhs);
vec3 normalize(const vec3& vector);

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

float dot(const vec2& lhs, const vec2& rhs);
vec2 normalize(const vec2& vector);

std::ostream& operator<<(std::ostream& os, const vec2& vector);

} // namespace pse::math