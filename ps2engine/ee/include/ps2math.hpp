#pragma once

#if defined(_EE)
#define PS2MATH_PS2
#elif defined(_WIN32) | defined(LINUX)
#define PS2MATH_PC
#endif

#include <math3d.h>
#include <cmath>
#include <ostream>

#ifdef PS2MATH_PC
typedef float VECTOR[4] __attribute__((__aligned__(16)));
typedef float MATRIX[16] __attribute__((__aligned__(16)));
#endif

namespace Math {

union Vec4;
struct Vec3;
struct Vec2;

union Mat4 
{
    MATRIX matrix alignas(sizeof(float) * 16);
    float m[4][4];

    Mat4();
    Mat4(const Mat4& matrix);

    void operator=(const Mat4& vector);
};

union Vec4
{
    VECTOR vector alignas(sizeof(float) * 4);
    struct { float x,y,z,w; };

    Vec4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);
    Vec4(const Vec4& vector);
    Vec4(const Vec3& vector, float w = 1.0f);
    Vec4(const Vec2& vector, float z = 0.0f, float w = 1.0f);

    void operator=(const Vec4& vector);
};
 
struct Vec3 
{
    float x,y,z;

    Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f);
    Vec3(const Vec4& vector);
    Vec3(const Vec3& vector);
    Vec3(const Vec2& vector);

    void operator=(const Vec3& vector);
};

struct Vec2
{
    float x,y;

    Vec2(float x = 0.0f, float y = 0.0f);
    Vec2(const Vec4& vector);
    Vec2(const Vec3& vector);
    Vec2(const Vec2& vector);

    void operator=(const Vec2& vector);
};

union Quaternion 
{
    VECTOR vector alignas(sizeof(float) * 4);
    struct { float x,y,z,w; };

    Quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f);

    void operator=(const Quaternion& vector);
};

union Color
{
    VECTOR vector alignas(sizeof(float) * 4);
    struct { float r,g,b,a; };

    Color(float r = 1.0F, float g = 1.0F, float b = 1.0F, float a = 1.0F);

    void operator=(const Color& vector);
};

struct Plane
{
    Vec4 mNormal;
    float mDistance;

    Plane();
    Plane(const Vec4& a, const Vec4& b, const Vec4& c);

    void Update(const Vec4& a, const Vec4& b, const Vec4& c);

    float DistanceTo(const Vec4& t_vec) const;
};

struct BoundingBox 
{
    Vec4 mMin, mMax;
};

struct Sphere
{
    Vec4 mPosition;
    float mRadius;
};

struct Frustum 
{
    static inline int nX = 0; 
    static inline int pX = 1; 
    static inline int nY = 2; 
    static inline int pY = 3; 
    static inline int nZ = 4;
    static inline int pZ = 5;

    Vec4 mPlanes[6];

    Frustum();
    Frustum(const Mat4& mat);

    void Set(const Mat4& mat);

    float DistanceTo(const Vec4& min, const Vec4& max, int plane);

    bool Test(const Vec4& pt);
    bool Test(const BoundingBox& bb);
    bool Test(const Sphere& sp);
};

struct Transform
{
    Vec4 mPosition = Vec4(0, 0, 0);
    Vec4 mRotation = Vec4(0, 0, 0);
    Vec4 mScale = Vec4(1, 1, 1);

    Mat4 GetTransformationMatrix();
};

struct Camera
{
    Vec4 mPosition = Vec4(0, 0, 0);
    Vec4 mLookAt = Vec4(0, 0, -1);
    Vec4 mUp = Vec4(0, 1, 0);

    Mat4 GetViewMatrix();

    Vec4 GetTarget();

};

struct Body {
    Transform mTransform;
    Vec4 mVelocity;
    Vec4 mForce;
    float mMass;
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

Mat4 operator*(const Mat4& lhs, const Mat4& rhs);

Vec4 operator*(const Mat4& matrix, const Vec4& vector);
Vec4 operator*(const Vec4& vector, const Mat4& matrix);

std::ostream& operator<<(std::ostream& os, const Mat4& matrix);

void Identity(const Mat4& matrix);

Mat4 Transpose(const Mat4& matrix);

Mat4 Translation(const Vec4& positon);

Mat4 RotationX(const float& r);
Mat4 RotationY(const float& r);
Mat4 RotationZ(const float& r);
Mat4 Rotation(const Vec4& rotation);

Mat4 Scale(const Vec4& scale);

Mat4 Transformation(const Vec4& position, const Vec4& rotation, const Vec4& scale);

Mat4 LookAt(const Vec4& position, const Vec4& lookAt, const Vec4& up);

Mat4 Projection(const float& fov, const float& width, const float& height, const float& scale, const float& ratio, const float& near, const float& far);

//
//  Vec4 Math
//

Vec4 operator+(const Vec4& lhs, const Vec4& rhs);
void operator+=(Vec4& lhs, const Vec4& rhs);

Vec4 operator-(const Vec4& lhs, const Vec4& rhs);
void operator-=(Vec4& lhs, const Vec4& rhs);

Vec4 operator*(const Vec4& lhs, const Vec4& rhs);
void operator*=(Vec4& lhs, const Vec4& rhs);

Vec4 operator*(const Vec4& vector, float scalar);
Vec4 operator*(float scalar, const Vec4& vector);
void operator*=(Vec4& vector, float scalar);

Vec4 operator/(const Vec4& lhs, const Vec4& rhs);
void operator/=(Vec4& lhs, const Vec4& rhs);

Vec4 operator/(const Vec4& vector, float scalar);
Vec4 operator/(float scalar, const Vec4& vector);
void operator/=(Vec4& vector, float scalar);

Vec4 operator-(const Vec4& vector);

float Dot(const Vec4& lhs, const Vec4& rhs);
float Dot3(const Vec4& lhs, const Vec4& rhs);
Vec4 Cross(const Vec4& lhs, const Vec4& rhs);
Vec4 Normalize(const Vec4& vector);
Vec4 Normalize3(const Vec4& vector);
Vec4 Lerp(const Vec4& lhs, const Vec4& rhs, float f);

std::ostream& operator<<(std::ostream& os, const Vec4& vector);

//
//  Vec3 Math
//

Vec3 operator+(const Vec3& lhs, const Vec3& rhs);
void operator+=(Vec3& lhs, const Vec3& rhs);

Vec3 operator-(const Vec3& lhs, const Vec3& rhs);
void operator-=(Vec3& lhs, const Vec3& rhs);

Vec3 operator*(const Vec3& lhs, const Vec3& rhs);
void operator*=(Vec3& lhs, const Vec3& rhs);

Vec3 operator*(const Vec3& vector, float scalar);
Vec3 operator*(float scalar, const Vec3& vector);
void operator*=(Vec3& vector, float scalar);

Vec3 operator/(const Vec3& lhs, const Vec3& rhs);
void operator/=(Vec3& lhs, const Vec3& rhs);

Vec3 operator/(const Vec3& vector, float scalar);
Vec3 operator/(float scalar, const Vec3& vector);
void operator/=(Vec3& vector, float scalar);

Vec3 operator-(const Vec3& vector);

float Dot(const Vec3& lhs, const Vec3& rhs);
Vec3 Cross(const Vec3& lhs, const Vec3& rhs);
Vec3 Normalize(const Vec3& vector);

std::ostream& operator<<(std::ostream& os, const Vec3& vector);

//
//  Vec2 Math
//

Vec2 operator+(const Vec2& lhs, const Vec2& rhs);
void operator+=(Vec2& lhs, const Vec2& rhs);

Vec2 operator-(const Vec2& lhs, const Vec2& rhs);
void operator-=(Vec2& lhs, const Vec2& rhs);

Vec2 operator*(const Vec2& lhs, const Vec2& rhs);
void operator*=(Vec2& lhs, const Vec2& rhs);

Vec2 operator*(const Vec2& vector, float scalar);
Vec2 operator*(float scalar, const Vec2& vector);
void operator*=(Vec2& vector, float scalar);

Vec2 operator/(const Vec2& lhs, const Vec2& rhs);
void operator/=(Vec2& lhs, const Vec2& rhs);

Vec2 operator/(const Vec2& vector, float scalar);
Vec2 operator/(float scalar, const Vec2& vector);
void operator/=(Vec2& vector, float scalar);

Vec2 operator-(const Vec2& vector);

float Dot(const Vec2& lhs, const Vec2& rhs);
Vec2 Normalize(const Vec2& vector);

std::ostream& operator<<(std::ostream& os, const Vec2& vector);

//
// Quaternion Functions
//

Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs);
void operator+=(Quaternion& lhs, const Quaternion& rhs);

Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs);
void operator-=(Quaternion& lhs, const Quaternion& rhs);

Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);
void operator*=(Quaternion& lhs, const Quaternion& rhs);

Quaternion operator*(const Quaternion& lhs, const float scalar);
void operator*=(Quaternion& lhs, const float scalar);

Quaternion operator*(const Quaternion& lhs, const Vec4& rhs);

Quaternion Normalize(const Quaternion& quaternion);
Quaternion Conjugate(const Quaternion& quaternion);

std::ostream& operator<<(std::ostream& os, const Quaternion& quaternion);

//
// Color Functions
//

Color operator+(const Color& lhs, const Color& rhs);
void operator+=(Color& lhs, const Color& rhs);

Color operator-(const Color& lhs, const Color& rhs);
void operator-=(Color& lhs, const Color& rhs);

Color operator*(const Color& lhs, const Color& rhs);
void operator*=(Color& lhs, const Color& rhs);

Color operator*(const Color& vector, float scalar);
Color operator*(float scalar, const Color& vector);
void operator*=(Color& vector, float scalar);

Color operator/(const Color& lhs, const Color& rhs);
void operator/=(Color& lhs, const Color& rhs);

Color operator/(const Color& vector, float scalar);
Color operator/(float scalar, const Color& vector);
void operator/=(Color& vector, float scalar);

Color operator-(const Color& vector);

std::ostream& operator<<(std::ostream& os, const Color& vector);

}
