//=============================================================================
// _____     ___ ____   ____  ___   ___     ___   ____
//  ____|   |    ____| |____ |   | |  _  | |   | |____
// |     ___|   |____  |____ |   | |___| | |   | |____
// ---------------------------------------------------
// Open Source Game Engine for Playstation 2
//
// File:        math.cpp
//
// Description: 
//
//=============================================================================

#include <core/math.hpp>

//========================================
// System Includes
//========================================

  #include <iomanip>

//========================================
// PS2SDK Includes
//========================================

//========================================
// Project Includes
//========================================

//========================================
// Definitions
//========================================

extern volatile const u32 PS2MATH_ATAN_TABLE[9] alignas(sizeof(float)) = {
    0x3f7ffff5, 0xbeaaa61c, 0x3e4c40a6, 0xbe0e6c63, 0x3dc577df,
    0xbd6501c4, 0x3cb31652, 0xbb84d7e7, 0x3f490fdb,
};

extern volatile const float PS2MATH_ATAN_TABLE2[8] = {
    0.0f,
    (pse::math::PI / 2.0f),
    (pse::math::PI / 2.0f),
    (pse::math::PI),
    (-pse::math::PI),
    (-pse::math::PI / 2.0f),
    (-pse::math::PI / 2.0f),
    0.0f,
};

namespace pse::math {

//
// Math Functions
//

float sin(const float& x) { return pse::math::cos(x - HALF_PI); }

float cos(float x) 
{
    float r;
    asm volatile(
        "lui     $9,  0x3f00        \n\t"
        ".set noreorder             \n\t"
        ".align 3                   \n\t"
        "abs.s   %0,  %1            \n\t"
        "lui     $8,  0xbe22        \n\t"
        "mtc1    $9,  $f1           \n\t"
        "ori     $8,  $8,    0xf983 \n\t"
        "mtc1    $8,  $f8           \n\t"
        "lui     $9,  0x4b00        \n\t"
        "mtc1    $9,  $f3           \n\t"
        "lui     $8,  0x3f80        \n\t"
        "mtc1    $8,  $f2           \n\t"
        "mula.s  %0,  $f8           \n\t"
        "msuba.s $f3, $f2           \n\t"
        "madda.s $f3, $f2           \n\t"
        "lui     $8,  0x40c9        \n\t"
        "msuba.s %0,  $f8           \n\t"
        "ori     $8,  0x0fdb        \n\t"
        "msub.s  %0,  $f1,   $f2    \n\t"
        "lui     $9,  0xc225        \n\t"
        "abs.s   %0,  %0            \n\t"
        "lui     $10, 0x3e80        \n\t"
        "mtc1    $10, $f7           \n\t"
        "ori     $9,  0x5de1        \n\t"
        "sub.s   %0,  %0,    $f7    \n\t"
        "lui     $10, 0x42a3        \n\t"
        "mtc1    $8,  $f3           \n\t"
        "ori     $10, 0x3458        \n\t"
        "mtc1    $9,  $f4           \n\t"
        "lui     $8,  0xc299        \n\t"
        "mtc1    $10, $f5           \n\t"
        "ori     $8,  0x2663        \n\t"
        "mul.s   $f8, %0,    %0     \n\t"
        "lui     $9,  0x421e        \n\t"
        "mtc1    $8,  $f6           \n\t"
        "ori     $9,  0xd7bb        \n\t"
        "mtc1    $9,  $f7           \n\t"
        "nop                        \n\t"
        "mul.s   $f1, %0,    $f8    \n\t"
        "mul.s   $f9, $f8,   $f8    \n\t"
        "mula.s  $f3, %0            \n\t"
        "mul.s   $f2, $f1,   $f8    \n\t"
        "madda.s $f4, $f1           \n\t"
        "mul.s   $f1, $f1,   $f9    \n\t"
        "mul.s   %0,  $f2,   $f9    \n\t"
        "madda.s $f5, $f2           \n\t"
        "madda.s $f6, $f1           \n\t"
        "madd.s  %0,  $f7,   %0     \n\t"
        ".set reorder               \n\t"
        : "=&f"(r)
        : "f"(x)
        : "$f1", "$f2", "$f3", "$f4", "$f5", "$f6", "$f7", "$f8", "$f9", "$8", "$9", "$10"
    );
    return r;
}

float tan(const float& x) { return pse::math::sin(x) / pse::math::cos(x); }

float atan2(float y, float x) 
{
    float r;
    asm volatile(
        "mtc1		$0, %0      \n\t"
        "abs.s		$f1, %1   \n\t"
        "abs.s		$f2, %2   \n\t"
        "c.lt.s		%2, %0    \n\t"
        "move		$9, $0      \n\t"
        "bc1f		_atan_00    \n\t"
        "addiu		$9, $9, 4   \n\t"
        "_atan_00:            \n\t"
        "c.eq.s		%2, %0      \n\t"
        "bc1f		_atan_00_1    \n\t"
        "c.eq.s		%1, %2      \n\t"
        "bc1t		_atan_06      \n\t"
        "c.lt.s		%1, %0      \n\t"
        "bc1f		_atan_00_1    \n\t"
        "addiu		$9, $0, 3	    \n\t"
        "_atan_00_1:		    \n\t"
        "mul.s		%1, %1, %2    \n\t"
        "c.lt.s		%1, %0	    \n\t"
        "bc1f		_atan_01    \n\t"
        "addiu		$9, $9, 2			    \n\t"
        "c.lt.s		$f2, $f1	    \n\t"
        "bc1f		_atan_02    \n\t"
        "addiu		$9, $9, 1	    \n\t"
        "b			_atan_02	    \n\t"
        "_atan_01:			    \n\t"
        "c.lt.s		$f1, $f2    \n\t"
        "bc1f		_atan_02    \n\t"
        "addiu		$9, $9, 1	    \n\t"
        "_atan_02:			    \n\t"
        "c.lt.s		$f1, $f2    \n\t"
        "bc1f		_atan_03    \n\t"
        "mov.s		%1, $f2    \n\t"
        "mov.s		%2, $f1    \n\t"
        "b			_atan_04    \n\t"
        "_atan_03:    \n\t"
        "mov.s		%1, $f1    \n\t"
        "mov.s		%2, $f2    \n\t"
        "_atan_04:    \n\t"
        "mfc1		$6, %1    \n\t"
        "mfc1		$7, %2    \n\t"
        "la			$8, PS2MATH_ATAN_TABLE    \n\t"
        "lqc2		$vf4, 0x0($8)    \n\t"
        "lqc2		$vf5, 0x10($8)    \n\t"
        "lqc2		$vf6, 0x20($8)    \n\t"
        "qmtc2		$6, $vf21    \n\t"
        "qmtc2		$7, $vf22    \n\t"
        "vadd.x		$vf23, $vf21, $vf22    \n\t"
        "vsub.x		$vf22, $vf22, $vf21    \n\t"
        "vdiv     $Q, $vf22x, $vf23x    \n\t"
        "vwaitq    \n\t"
        "vaddq.x  $vf21, $vf0, $Q		    \n\t"
        "vmul.x		$vf22, $vf21, $vf21    \n\t"
        "vmulax.x	$ACC, $vf21, $vf4	    \n\t"
        "vmul.x		$vf21, $vf21, $vf22    \n\t"
        "vmadday.x	$ACC, $vf21, $vf4	    \n\t"
        "vmul.x		$vf21, $vf21, $vf22    \n\t"
        "vmaddaz.x	$ACC, $vf21, $vf4	    \n\t"
        "vmul.x		$vf21, $vf21, $vf22    \n\t"
        "vmaddaw.x	$ACC, $vf21, $vf4	    \n\t"
        "vmul.x		$vf21, $vf21, $vf22    \n\t"
        "vmaddax.x	$ACC, $vf21, $vf5	    \n\t"
        "vmul.x		$vf21, $vf21, $vf22    \n\t"
        "vmadday.x	$ACC, $vf21, $vf5	    \n\t"
        "vmul.x		$vf21, $vf21, $vf22    \n\t"
        "vmaddaz.x	$ACC, $vf21, $vf5	    \n\t"
        "vmul.x		$vf21, $vf21, $vf22    \n\t"
        "vmaddaw.x	$ACC, $vf21, $vf5	    \n\t"
        "vmaddw.x	$vf21, $vf6, $vf0    \n\t"
        "qmfc2		$6, $vf21    \n\t"
        "mtc1		$6, %0    \n\t"
        "andi		$8, $9, 1    \n\t"
        "sll			$9, $9, 2    \n\t"
        "la			$7, PS2MATH_ATAN_TABLE2    \n\t"
        "add			$9, $9, $7    \n\t"
        "lw			$7, 0x0($9)    \n\t"
        "mtc1		$7, %1    \n\t"
        "beq			$8, $0, _atan_05    \n\t"
        "sub.s		%0, %1, %0    \n\t"
        "b			_atan_06    \n\t"
        "_atan_05:    \n\t"
        "add.s		%0, %1, %0    \n\t"
        "_atan_06:    \n\t"
        : "=&f"(r)
        : "f"(x), "f"(y)
        : "$6", "$7", "$8", "$9", "$f0", "$f1", "$f2"
    );
    return r;
}

float asin(float x) 
{
    float r;
    asm volatile(
        "lui     $9,  0x3f00        \n\t"
        ".set noreorder             \n\t"
        ".align 3                   \n\t"
        "abs.s   %0,  %1            \n\t"
        "lui     $8,  0xbe22        \n\t"
        "mtc1    $9,  $f1           \n\t"
        "ori     $8,  $8,    0xf983 \n\t"
        "mtc1    $8,  $f8           \n\t"
        "lui     $9,  0x4b00        \n\t"
        "mtc1    $9,  $f3           \n\t"
        "lui     $8,  0x3f80        \n\t"
        "mtc1    $8,  $f2           \n\t"
        "mula.s  %0,  $f8           \n\t"
        "msuba.s $f3, $f2           \n\t"
        "madda.s $f3, $f2           \n\t"
        "lui     $8,  0x40c9        \n\t"
        "msuba.s %0,  $f8           \n\t"
        "ori     $8,  0x0fdb        \n\t"
        "msub.s  %0,  $f1,   $f2    \n\t"
        "lui     $9,  0xc225        \n\t"
        "abs.s   %0,  %0            \n\t"
        "lui     $10, 0x3e80        \n\t"
        "mtc1    $10, $f7           \n\t"
        "ori     $9,  0x5de1        \n\t"
        "sub.s   %0,  %0,    $f7    \n\t"
        "lui     $10, 0x42a3        \n\t"
        "mtc1    $8,  $f3           \n\t"
        "ori     $10, 0x3458        \n\t"
        "mtc1    $9,  $f4           \n\t"
        "lui     $8,  0xc299        \n\t"
        "mtc1    $10, $f5           \n\t"
        "ori     $8,  0x2663        \n\t"
        "mul.s   $f8, %0,    %0     \n\t"
        "lui     $9,  0x421e        \n\t"
        "mtc1    $8,  $f6           \n\t"
        "ori     $9,  0xd7bb        \n\t"
        "mtc1    $9,  $f7           \n\t"
        "nop                        \n\t"
        "mul.s   $f1, %0,    $f8    \n\t"
        "mul.s   $f9, $f8,   $f8    \n\t"
        "mula.s  $f3, %0            \n\t"
        "mul.s   $f2, $f1,   $f8    \n\t"
        "madda.s $f4, $f1           \n\t"
        "mul.s   $f1, $f1,   $f9    \n\t"
        "mul.s   %0,  $f2,   $f9    \n\t"
        "madda.s $f5, $f2           \n\t"
        "madda.s $f6, $f1           \n\t"
        "madd.s  %0,  $f7,   %0     \n\t"
        ".set reorder               \n\t"
        : "=&f"(r)
        : "f"(x)
        : "$f1", "$f2", "$f3", "$f4", "$f5", "$f6", "$f7", "$f8", "$f9", "$8", "$9", "$10"
    );
    return r;
}

float acos(const float& x) 
{
  float y = sqrt(1.0f - x * x);
  float t = atan2(y, x);
  return t;
}

//
//  vec4 Math
//

vec4::vec4() : x(0.0F), y(0.0F), z(0.0F), w(0.0f) {}
vec4::vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
vec4::vec4(const vec4& vector) { x = vector.x; y = vector.y; z = vector.z; w = vector.w; }
vec4::vec4(const vec3& vector, float w_) : x(vector.x), y(vector.y), z(vector.z), w(w_) {}

vec4& vec4::set(float x_, float y_, float z_, float w_) { x = x_; y = y_; z = z_; w = w_; return *this; }
vec4& vec4::set(const vec4& vector) { x = vector.x; y = vector.y; z = vector.z; w = vector.w; return *this; }
vec4& vec4::set(const vec3& vector, float w_) { x = vector.x; y = vector.y; z = vector.z; w = w_; return *this; }
vec4& vec4::set(const vec2& vector, float z_, float w_) { x = vector.x; y = vector.y; z = z_; w = w_; return *this; }

vec4& vec4::operator=(const vec4& vector) { x = vector.x; y = vector.y; z = vector.z; w = vector.w; return *this; }
vec4& vec4::operator=(const vec3& vector) { x = vector.x; y = vector.y; z = vector.z; return *this; }
vec4& vec4::operator=(const vec2& vector) { x = vector.x; y = vector.y; return *this; }

float vec4::dot(const vec4& other) const
{
    float output;
    asm volatile(
        "lqc2     $vf4, 0x0(%1)  \n\t"
        "lqc2     $vf5, 0x0(%2)  \n\t"
        "vmul.xyzw $vf6, $vf4, $vf5 \n\t"
        "vaddy.x  $vf6, $vf6, $vf6 \n\t"
        "vaddz.x  $vf6, $vf6, $vf6 \n\t"
        "vaddw.x  $vf6, $vf6, $vf6 \n\t"
        "qmfc2    $2,  $vf6      \n\t"
        "mtc1     $2,  %0       \n\t"
        : "=f"(output)
        : "r"(vector), "r"(other.vector)
    );
    return output;
}

float vec4::length() const { return sqrtf(dot(*this)); }

vec4 vec4::normalized() const
{
    vec4 output;
    asm volatile(
        "lqc2       $vf4, 0x0(%0)    \n\t"
        "vmul.xyzw   $vf5, $vf4,  $vf4  \n\t"
        "vaddy.x    $vf5, $vf5,  $vf5  \n\t"
        "vaddz.x    $vf5, $vf5,  $vf5  \n\t"
        "vaddw.x    $vf5, $vf5,  $vf5  \n\t"
        "vrsqrt     $Q,   $vf0w, $vf5x \n\t"
        "vwaitq                     \n\t"
        "vsub.xyzw   $vf6, $vf0,  $vf0  \n\t"
        "vwaitq                     \n\t"
        "vmulq.xyzw  $vf6, $vf4,  $Q    \n\t"
        "sqc2       $vf6, 0x0(%1)    \n\t"
        :
        : "r"(vector), "r"(output.vector)
    );
    return output;
}

vec4& vec4::normalize() {
    vec4 work = normalized();
    return set(work);
}

// Vector addition
vec4 vec4::operator+(const vec4& other) const
{
    vec4 output;
    asm __volatile__ (
      "lqc2       $vf4,   0x0(%1)         \n\t"
      "lqc2       $vf5,   0x0(%2)         \n\t"
      "vadd.xyzw  $vf6,   $vf4,   $vf5    \n\t"
      "sqc2       $vf6,   0x0(%0)         \n\t"
      :
      : "r"(output.vector), "r"(vector), "r"(other.vector)
    );
    return output;
}

vec4& vec4::operator+=(const vec4& other)
{
    asm volatile(
        "lqc2      $vf4, 0x0(%0)    \n\t"
        "lqc2      $vf5, 0x0(%1)    \n\t"
        "vadd.xyzw $vf4, $vf4, $vf5 \n\t"
        "sqc2      $vf4, 0x0(%0)    \n\t"
        :
        : "r"(vector), "r"(other.vector)
    );
    return *this;
}

// Vector subtraction
vec4 vec4::operator-(const vec4& other) const
{
    vec4 output;
    asm __volatile__ (
      "lqc2       $vf4,   0x0(%1)         \n\t"
      "lqc2       $vf5,   0x0(%2)         \n\t"
      "vsub.xyzw  $vf6,   $vf4,   $vf5    \n\t"
      "sqc2       $vf6,   0x0(%0)         \n\t"
      :
      : "r"(output.vector), "r"(vector), "r"(other.vector)
    );
    return output;
}

vec4& vec4::operator-=(const vec4& other)
{
    asm volatile(
        "lqc2      $vf4, 0x0(%0)    \n\t"
        "lqc2      $vf5, 0x0(%1)    \n\t"
        "vsub.xyzw $vf4, $vf4, $vf5 \n\t"
        "sqc2      $vf4, 0x0(%0)    \n\t"
        :
        : "r"(vector), "r"(other.vector)
    );
    return *this;
}

vec4 vec4::operator*(const vec4& other) const
{
    vec4 output;
    asm __volatile__ (
      "lqc2       $vf4,   0x0(%1)         \n\t"
      "lqc2       $vf5,   0x0(%2)         \n\t"
      "vmul.xyzw  $vf6,   $vf4,   $vf5    \n\t"
      "sqc2       $vf6,   0x0(%0)         \n\t"
      :
      : "r"(output.vector), "r"(vector), "r"(other.vector)
    );
    return output;
}

vec4& vec4::operator*=(const vec4& other)
{
    asm volatile(
        "lqc2      $vf4, 0x0(%0)    \n\t"
        "lqc2      $vf5, 0x0(%1)    \n\t"
        "vmul.xyzw $vf4, $vf4, $vf5 \n\t"
        "sqc2      $vf4, 0x0(%0)    \n\t"
        :
        : "r"(vector), "r"(other.vector)
    );
    return *this;
}

vec4 vec4::operator*(float scalar) const
{
    vec4 output;
    asm volatile(
        "lqc2       $vf4,   0x0(%1)         \n\t"
        "mfc1       $8,     %2              \n\t"
        "qmtc2      $8,     $vf5            \n\t"
        "vmulx.xyzw $vf6,   $vf4,   $vf5    \n\t"
        "sqc2       $vf6,   0x0(%0)         \n\t"
        :
        : "r"(output.vector), "r"(vector), "f"(scalar)
        : "$8"
    );
    return output;
}

vec4& vec4::operator*=(float scalar)
{
    asm volatile(
        "lqc2       $vf4, 0x0(%0)     \n\t"
        "mfc1       $8,   %1          \n\t"
        "qmtc2      $8,   $vf5        \n\t"
        "vmulx.xyzw $vf4, $vf4, $vf5  \n\t"
        "sqc2       $vf4, 0x0(%0)     \n\t"
        :
        : "r"(vector), "f"(scalar)
        : "$8"
    );
    return *this;
}

vec4 vec4::operator/(const vec4& other) const
{
    vec4 output;
    output.x = x / other.x;
    output.y = y / other.y;
    output.z = z / other.z;
    output.w = w / other.w;
    return output;
}

vec4& vec4::operator/=(const vec4& other)
{
    x /= other.x;
    y /= other.y;
    z /= other.z;
    w /= other.w;
    return *this;
}

vec4 vec4::operator/(float scalar) const
{
    vec4 output;
    asm volatile(
        "mfc1       $8, %2            \n\t"
        "qmtc2      $8, $vf5          \n\t"
        "vdiv       $Q, $vf0w, $vf5x  \n\t"
        "lqc2       $vf4, 0x00(%1)    \n\t"
        "vwaitq                       \n\t"
        "vmulq.xyzw $vf4, $vf4, $Q    \n\t"
        "sqc2       $vf4, 0x00(%0)    \n\t"
        :
        : "r"(output.vector), "r"(vector), "f"(scalar)
        : "$8"
    );
    return output;
}

vec4& vec4::operator/=(float scalar)
{
    asm volatile(
        "mfc1       $8, %1            \n\t"
        "qmtc2      $8, $vf5          \n\t"
        "vdiv       $Q, $vf0w, $vf5x  \n\t"
        "lqc2       $vf4, 0x00(%0)    \n\t"
        "vwaitq                       \n\t"
        "vmulq.xyzw $vf4, $vf4, $Q    \n\t"
        "sqc2       $vf4, 0x00(%0)    \n\t"
        :
        : "r"(vector), "f"(scalar)
        : "$8"
    );
    return *this;
}

vec4 vec4::operator-() const
{
    vec4 output;
    output.x = -x;
    output.y = -y;
    output.z = -z;
    output.w = -w;
    return output;
}

bool vec4::operator==(const vec4& rhs) const
{
    return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
}

bool vec4::operator!=(const vec4& rhs) const
{
    return !(*this == rhs);
}

std::ostream& operator<<(std::ostream& os, const vec4& vector)
{
    os << "x=" << vector.x << " y=" << vector.y << " z=" << vector.z << " w=" << vector.w;
    return os;
}

//
//  vec3 Math
//

vec3::vec3() : x(0.0F), y(0.0F), z(0.0F) {}
vec3::vec3(float x, float y, float z) : x(x), y(y), z(z) {}
vec3::vec3(const vec4& vector) : x(vector.x), y(vector.y), z(vector.z) {}
vec3::vec3(const vec3& vector) : x(vector.x), y(vector.y), z(vector.z) {}

void vec3::set(float x_, float y_, float z_) { x = x_; y = y_; z = z_; }
void vec3::set(const vec4& vector) { x = vector.x; y = vector.y; z = vector.z; }
void vec3::set(const vec3& vector) { x = vector.x; y = vector.y; z = vector.z; }

vec3& vec3::operator=(const vec4& vector) { x = vector.x; y = vector.y; z = vector.z; return *this; }
vec3& vec3::operator=(const vec3& vector) { x = vector.x; y = vector.y; z = vector.z; return *this; }

float vec3::dot(const vec3& other) const { return x * other.x + y * other.y + z * other.z; }

float vec3::length() const { return sqrtf(dot(*this)); }

vec3 vec3::cross(const vec3& other) const
{
    vec3 result;
    result.x = y * other.z - z * other.y;
    result.y = z * other.x - x * other.z;
    result.z = x * other.y - y * other.x;
    return result;
}

vec3 vec3::normalized() const { return *this / length(); }

vec3& vec3::normalize() { *this = normalized(); return *this; }

vec3 vec3::operator+(const vec3& other) const
{
    return vec3(x + other.x, y + other.y, z + other.z);
}

vec3& vec3::operator+=(const vec3& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

vec3 vec3::operator-(const vec3& other) const
{
    return vec3(x - other.x, y - other.y, z - other.z);
}

vec3& vec3::operator-=(const vec3& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

vec3 vec3::operator*(const vec3& other) const
{
    return vec3(x * other.x, y * other.y, z * other.z);
}

vec3& vec3::operator*=(const vec3& other)
{
    x *= other.x;
    y *= other.y;
    z *= other.z;
    return *this;
}

vec3 vec3::operator*(float scalar) const
{
    return vec3(x * scalar, y * scalar, z * scalar);
}

vec3& vec3::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

vec3 vec3::operator/(const vec3& other) const
{
    return vec3(x / other.x, y / other.y, z / other.z);
}

vec3& vec3::operator/=(const vec3& other)
{
    x /= other.x;
    y /= other.y;
    z /= other.z;
    return *this;
}

vec3 vec3::operator/(float scalar) const
{
    float invScalar = 1.0f / scalar;
    return vec3(x * invScalar, y * invScalar, z * invScalar);
}

vec3& vec3::operator/=(float scalar)
{
    float invScalar = 1.0f / scalar;
    x *= invScalar;
    y *= invScalar;
    z *= invScalar;
    return *this;
}

vec3 vec3::operator-() const
{
    return vec3(-x, -y, -z);
}

bool vec3::operator==(const vec3& rhs) const
{
    return (x == rhs.x && y == rhs.y && z == rhs.z);
}

bool vec3::operator!=(const vec3& rhs) const
{
    return !(*this == rhs);
}

std::ostream& operator<<(std::ostream& os, const vec3& vector)
{
    os << "x=" << vector.x << " y=" << vector.y << " z=" << vector.z;
    return os;
}

//
//  vec2 Math
//

vec2::vec2() : x(0.0F), y(0.0F) {}
vec2::vec2(float x, float y) : x(x), y(y) {}
vec2::vec2(const vec2& vector) : x(vector.x), y(vector.y) {}

void vec2::set(float x_, float y_) { x = x_; y = y_; }
void vec2::set(const vec2& vector) { x = vector.x; y = vector.y; }

vec2& vec2::operator=(const vec2& vector) { x = vector.x; y = vector.y; return *this; }

float vec2::dot(const vec2& other) const { return x * other.x + y * other.y; }

float vec2::length() const { return sqrtf(dot(*this)); }

vec2 vec2::normalized() const { return *this / length(); }
const vec2& vec2::normalize() { *this = normalized(); return *this; }

vec2 vec2::operator+(const vec2& other) const
{
    return vec2(x + other.x, y + other.y);
}

vec2& vec2::operator+=(const vec2& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

vec2 vec2::operator-(const vec2& other) const
{
    return vec2(x - other.x, y - other.y);
}

vec2& vec2::operator-=(const vec2& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

vec2 vec2::operator*(const vec2& other) const
{
    return vec2(x * other.x, y * other.y);
}

vec2& vec2::operator*=(const vec2& other)
{
    x *= other.x;
    y *= other.y;
    return *this;
}

vec2 vec2::operator*(float scalar) const
{
    return vec2(x * scalar, y * scalar);
}

vec2& vec2::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}

vec2 vec2::operator/(const vec2& other) const
{
    return vec2(x / other.x, y / other.y);
}

vec2& vec2::operator/=(const vec2& other)
{
    x /= other.x;
    y /= other.y;
    return *this;
}

vec2 vec2::operator/(float scalar) const
{
    float invScalar = 1.0f / scalar;
    return vec2(x * invScalar, y * invScalar);
}

vec2& vec2::operator/=(float scalar)
{
    float invScalar = 1.0f / scalar;
    x *= invScalar;
    y *= invScalar;
    return *this;
}

vec2 vec2::operator-() const
{
    return vec2(-x, -y);
}

bool vec2::operator==(const vec2& rhs) const
{
    return (x == rhs.x && y == rhs.y);
}

bool vec2::operator!=(const vec2& rhs) const
{
    return !(*this == rhs);
}

std::ostream& operator<<(std::ostream& os, const vec2& vector)
{
    os << "x=" << vector.x << " y=" << vector.y;
    return os;
}

//
//  Matrix Math
//

mat4& mat4::set_identity() 
{
    asm volatile(
        "vsub.xyzw  $vf4, $vf0, $vf0 \n\t"
        "vadd.w     $vf4, $vf4, $vf0 \n\t"
        "vmr32.xyzw $vf5, $vf4      \n\t"
        "vmr32.xyzw $vf6, $vf5      \n\t"
        "vmr32.xyzw $vf7, $vf6      \n\t"
        "sqc2       $vf4, 0x30(%0) \n\t"
        "sqc2       $vf5, 0x20(%0) \n\t"
        "sqc2       $vf6, 0x10(%0) \n\t"
        "sqc2       $vf7, 0x0(%0)  \n\t"
        :
        : "r"(matrix)
    );
    return *this;
}

mat4& mat4::set_translate(const vec3& position)
{
    set_identity();
    matrix[12] = position.x;
    matrix[13] = position.y;
    matrix[14] = position.z;
    matrix[15] = 1.0F;
    return *this;
}

mat4& mat4::set_rotate(const float& rotation, const vec3& axis)
{
    float c = cos(rotation);
    float s = sin(rotation);

    matrix[0] = axis.x * axis.x * (1 - c) + c;
    matrix[1] = axis.y * axis.x * (1 - c) + axis.z * s;
    matrix[2] = axis.x * axis.z * (1 - c) - axis.y * s;
    matrix[3] = 0.0F;

    matrix[4] = axis.x * axis.y * (1 - c) - axis.z * s;
    matrix[5] = axis.y * axis.y * (1 - c) + c;
    matrix[6] = axis.y * axis.z * (1 - c) + axis.x * s;
    matrix[7] = 0.0F;

    matrix[8] = axis.x * axis.z * (1 - c) + axis.y * s;
    matrix[9] = axis.y * axis.z * (1 - c) - axis.x * s;
    matrix[10] = axis.z * axis.z * (1 - c) + c;
    matrix[11] = 0.0F;

    matrix[12] = 0.0F;
    matrix[13] = 0.0F;
    matrix[14] = 0.0F;
    matrix[15] = 1.0F;
    return *this;
}

mat4& mat4::set_rotate_x(const float& rotation)
{
    float c = cos(rotation);
    float s = sin(rotation);
    matrix[5] = c;
    matrix[6] = s;
    matrix[9] = -s;
    matrix[10] = c;
    return *this;
}

mat4& mat4::set_rotate_y(const float& rotation)
{
    float c = cos(rotation);
    float s = sin(rotation);
    matrix[0] = c;
    matrix[2] = -s;
    matrix[8] = s;
    matrix[10] = c;
    return *this;
}

mat4& mat4::set_rotate_z(const float& rotation)
{
    float c = cos(rotation);
    float s = sin(rotation);
    matrix[0] = c;
    matrix[1] = s;
    matrix[4] = -s;
    matrix[5] = c;
    return *this;
}

mat4& mat4::set_rotate(const vec3& rotation)
{
    set_identity();
    mat4 work;
    work.set_rotate_x(rotation.x);
    *this *= work;
    work.set_rotate_y(rotation.y);
    *this *= work;
    work.set_rotate_z(rotation.z);
    return *this *= work;
}

mat4& mat4::set_scale(const vec3& scale)
{
    set_identity();
    m[0][0] = scale.x;
    m[1][1] = scale.y;
    m[2][2] = scale.z;
    return *this;
}

mat4& mat4::set(const mat4& other)
{
    asm volatile(
        "lqc2		$vf1, 0x00(%1)  \n"
        "lqc2		$vf2, 0x10(%1)  \n"
        "lqc2		$vf3, 0x20(%1)  \n"
        "lqc2		$vf4, 0x30(%1)  \n"
        "sqc2		$vf1, 0x00(%0)  \n"
        "sqc2		$vf2, 0x10(%0)  \n"
        "sqc2		$vf3, 0x20(%0)  \n"
        "sqc2		$vf4, 0x30(%0)  \n"
        :
        : "r"(matrix), "r"(other.matrix)
    );
    return *this;
}

mat4& mat4::set(float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33)
{
    m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
    m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
    m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
    m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    return *this;    
}

mat4& mat4::operator=(const mat4& matrix) 
{ 
    return set(matrix);
}

mat4& mat4::transpose()
{
    mat4 result = transposed();
    return set(result);
}

mat4 mat4::transposed() const
{
    mat4 result;
    result.matrix[0x00] = matrix[0x00];
    result.matrix[0x01] = matrix[0x04];
    result.matrix[0x02] = matrix[0x08];
    result.matrix[0x03] = matrix[0x0C];
    result.matrix[0x04] = matrix[0x01];
    result.matrix[0x05] = matrix[0x05];
    result.matrix[0x06] = matrix[0x09];
    result.matrix[0x07] = matrix[0x0D];
    result.matrix[0x08] = matrix[0x02];
    result.matrix[0x09] = matrix[0x06];
    result.matrix[0x0A] = matrix[0x0A];
    result.matrix[0x0B] = matrix[0x0E];
    result.matrix[0x0C] = matrix[0x03];
    result.matrix[0x0D] = matrix[0x07];
    result.matrix[0x0E] = matrix[0x0B];
    result.matrix[0x0F] = matrix[0x0F];
    return result;
}

mat4& mat4::invert()
{
    mat4 result = inverted();
    return set(result);
}

mat4 mat4::inverted() const
{

    /* transpose matrix */
    mat4 src = transposed();

    /* calculate pairs for first 8 elements (cofactors) */
    float tmp[12]; /* temp array for pairs */
    tmp[0]  = src.matrix[10] * src.matrix[15];
    tmp[1]  = src.matrix[11] * src.matrix[14];
    tmp[2]  = src.matrix[9] * src.matrix[15];
    tmp[3]  = src.matrix[11] * src.matrix[13];
    tmp[4]  = src.matrix[9] * src.matrix[14];
    tmp[5]  = src.matrix[10] * src.matrix[13];
    tmp[6]  = src.matrix[8] * src.matrix[15];
    tmp[7]  = src.matrix[11] * src.matrix[12];
    tmp[8]  = src.matrix[8] * src.matrix[14];
    tmp[9]  = src.matrix[10] * src.matrix[12];
    tmp[10] = src.matrix[8] * src.matrix[13];
    tmp[11] = src.matrix[9] * src.matrix[12];

    /* calculate first 8 elements (cofactors) */
    mat4 dst;
    dst.matrix[0] = tmp[0] * src.matrix[5] + tmp[3] * src.matrix[6] + tmp[4] * src.matrix[7];
    dst.matrix[0] -= tmp[1] * src.matrix[5] + tmp[2] * src.matrix[6] + tmp[5] * src.matrix[7];
    dst.matrix[1] = tmp[1] * src.matrix[4] + tmp[6] * src.matrix[6] + tmp[9] * src.matrix[7];
    dst.matrix[1] -= tmp[0] * src.matrix[4] + tmp[7] * src.matrix[6] + tmp[8] * src.matrix[7];
    dst.matrix[2] = tmp[2] * src.matrix[4] + tmp[7] * src.matrix[5] + tmp[10] * src.matrix[7];
    dst.matrix[2] -= tmp[3] * src.matrix[4] + tmp[6] * src.matrix[5] + tmp[11] * src.matrix[7];
    dst.matrix[3] = tmp[5] * src.matrix[4] + tmp[8] * src.matrix[5] + tmp[11] * src.matrix[6];
    dst.matrix[3] -= tmp[4] * src.matrix[4] + tmp[9] * src.matrix[5] + tmp[10] * src.matrix[6];
    dst.matrix[4] = tmp[1] * src.matrix[1] + tmp[2] * src.matrix[2] + tmp[5] * src.matrix[3];
    dst.matrix[4] -= tmp[0] * src.matrix[1] + tmp[3] * src.matrix[2] + tmp[4] * src.matrix[3];
    dst.matrix[5] = tmp[0] * src.matrix[0] + tmp[7] * src.matrix[2] + tmp[8] * src.matrix[3];
    dst.matrix[5] -= tmp[1] * src.matrix[0] + tmp[6] * src.matrix[2] + tmp[9] * src.matrix[3];
    dst.matrix[6] = tmp[3] * src.matrix[0] + tmp[6] * src.matrix[1] + tmp[11] * src.matrix[3];
    dst.matrix[6] -= tmp[2] * src.matrix[0] + tmp[7] * src.matrix[1] + tmp[10] * src.matrix[3];
    dst.matrix[7] = tmp[4] * src.matrix[0] + tmp[9] * src.matrix[1] + tmp[10] * src.matrix[2];
    dst.matrix[7] -= tmp[5] * src.matrix[0] + tmp[8] * src.matrix[1] + tmp[11] * src.matrix[2];

    /* calculate pairs for second 8 elements (cofactors) */
    tmp[0]  = src.matrix[2] * src.matrix[7];
    tmp[1]  = src.matrix[3] * src.matrix[6];
    tmp[2]  = src.matrix[1] * src.matrix[7];
    tmp[3]  = src.matrix[3] * src.matrix[5];
    tmp[4]  = src.matrix[1] * src.matrix[6];
    tmp[5]  = src.matrix[2] * src.matrix[5];
    tmp[6]  = src.matrix[0] * src.matrix[7];
    tmp[7]  = src.matrix[3] * src.matrix[4];
    tmp[8]  = src.matrix[0] * src.matrix[6];
    tmp[9]  = src.matrix[2] * src.matrix[4];
    tmp[10] = src.matrix[0] * src.matrix[5];
    tmp[11] = src.matrix[1] * src.matrix[4];

    /* calculate second 8 elements (cofactors) */
    dst.matrix[8] = tmp[0] * src.matrix[13] + tmp[3] * src.matrix[14] + tmp[4] * src.matrix[15];
    dst.matrix[8] -= tmp[1] * src.matrix[13] + tmp[2] * src.matrix[14] + tmp[5] * src.matrix[15];
    dst.matrix[9] = tmp[1] * src.matrix[12] + tmp[6] * src.matrix[14] + tmp[9] * src.matrix[15];
    dst.matrix[9] -= tmp[0] * src.matrix[12] + tmp[7] * src.matrix[14] + tmp[8] * src.matrix[15];
    dst.matrix[10] = tmp[2] * src.matrix[12] + tmp[7] * src.matrix[13] + tmp[10] * src.matrix[15];
    dst.matrix[10] -= tmp[3] * src.matrix[12] + tmp[6] * src.matrix[13] + tmp[11] * src.matrix[15];
    dst.matrix[11] = tmp[5] * src.matrix[12] + tmp[8] * src.matrix[13] + tmp[11] * src.matrix[14];
    dst.matrix[11] -= tmp[4] * src.matrix[12] + tmp[9] * src.matrix[13] + tmp[10] * src.matrix[14];
    dst.matrix[12] = tmp[2] * src.matrix[10] + tmp[5] * src.matrix[11] + tmp[1] * src.matrix[9];
    dst.matrix[12] -= tmp[4] * src.matrix[11] + tmp[0] * src.matrix[9] + tmp[3] * src.matrix[10];
    dst.matrix[13] = tmp[8] * src.matrix[11] + tmp[0] * src.matrix[8] + tmp[7] * src.matrix[10];
    dst.matrix[13] -= tmp[6] * src.matrix[10] + tmp[9] * src.matrix[11] + tmp[1] * src.matrix[8];
    dst.matrix[14] = tmp[6] * src.matrix[9] + tmp[11] * src.matrix[11] + tmp[3] * src.matrix[8];
    dst.matrix[14] -= tmp[10] *src.matrix[11] + tmp[2] * src.matrix[8] + tmp[7] * src.matrix[9];
    dst.matrix[15] = tmp[10] * src.matrix[10] + tmp[4] * src.matrix[8] + tmp[9] * src.matrix[9];
    dst.matrix[15] -= tmp[8] * src.matrix[9] + tmp[11] * src.matrix[10] + tmp[5] * src.matrix[8];

    /* calculate determinant */
    float det = src.matrix[0] * dst.matrix[0] + src.matrix[1] * dst.matrix[1] + src.matrix[2] * dst.matrix[2] + src.matrix[3] * dst.matrix[3];

    /* calculate matrix inverse */
    det = 1 / det;
    for (int j = 0; j < 16; j++)
        dst.matrix[j] *= det;

    return dst;
}

mat4 mat4::operator*(const mat4& other) const
{
    mat4 output;
    asm volatile(
        "lqc2         $vf1, 0x00(%1) \n\t"
        "lqc2         $vf2, 0x10(%1) \n\t"
        "lqc2         $vf3, 0x20(%1) \n\t"
        "lqc2         $vf4, 0x30(%1) \n\t"
        "lqc2         $vf5, 0x00(%2) \n\t"
        "lqc2         $vf6, 0x10(%2) \n\t"
        "lqc2         $vf7, 0x20(%2) \n\t"
        "lqc2         $vf8, 0x30(%2) \n\t"
        "vmulax.xyzw  $ACC, $vf5, $vf1 \n\t"
        "vmadday.xyzw $ACC, $vf6, $vf1 \n\t"
        "vmaddaz.xyzw $ACC, $vf7, $vf1 \n\t"
        "vmaddw.xyzw  $vf1, $vf8, $vf1 \n\t"
        "vmulax.xyzw  $ACC, $vf5, $vf2 \n\t"
        "vmadday.xyzw $ACC, $vf6, $vf2 \n\t"
        "vmaddaz.xyzw $ACC, $vf7, $vf2 \n\t"
        "vmaddw.xyzw  $vf2, $vf8, $vf2 \n\t"
        "vmulax.xyzw  $ACC, $vf5, $vf3 \n\t"
        "vmadday.xyzw $ACC, $vf6, $vf3 \n\t"
        "vmaddaz.xyzw $ACC, $vf7, $vf3 \n\t"
        "vmaddw.xyzw  $vf3, $vf8, $vf3 \n\t"
        "vmulax.xyzw  $ACC, $vf5, $vf4 \n\t"
        "vmadday.xyzw $ACC, $vf6, $vf4 \n\t"
        "vmaddaz.xyzw $ACC, $vf7, $vf4 \n\t"
        "vmaddw.xyzw  $vf4, $vf8, $vf4 \n\t"
        "sqc2         $vf1, 0x00(%0) \n\t"
        "sqc2         $vf2, 0x10(%0) \n\t"
        "sqc2         $vf3, 0x20(%0) \n\t"
        "sqc2         $vf4, 0x30(%0) \n\t"
        :
        : "r"(output.matrix), "r"(other.matrix), "r"(matrix)
        : "memory"
    );
    return output;
}

mat4& mat4::operator*=(const mat4& other)
{
    mat4 temp;
    asm volatile(
        "lqc2         $vf1, 0x00(%1) \n\t"
        "lqc2         $vf2, 0x10(%1) \n\t"
        "lqc2         $vf3, 0x20(%1) \n\t"
        "lqc2         $vf4, 0x30(%1) \n\t"
        "lqc2         $vf5, 0x00(%2) \n\t"
        "lqc2         $vf6, 0x10(%2) \n\t"
        "lqc2         $vf7, 0x20(%2) \n\t"
        "lqc2         $vf8, 0x30(%2) \n\t"
        "vmulax.xyzw  $ACC, $vf5, $vf1 \n\t"
        "vmadday.xyzw $ACC, $vf6, $vf1 \n\t"
        "vmaddaz.xyzw $ACC, $vf7, $vf1 \n\t"
        "vmaddw.xyzw  $vf1, $vf8, $vf1 \n\t"
        "vmulax.xyzw  $ACC, $vf5, $vf2 \n\t"
        "vmadday.xyzw $ACC, $vf6, $vf2 \n\t"
        "vmaddaz.xyzw $ACC, $vf7, $vf2 \n\t"
        "vmaddw.xyzw  $vf2, $vf8, $vf2 \n\t"
        "vmulax.xyzw  $ACC, $vf5, $vf3 \n\t"
        "vmadday.xyzw $ACC, $vf6, $vf3 \n\t"
        "vmaddaz.xyzw $ACC, $vf7, $vf3 \n\t"
        "vmaddw.xyzw  $vf3, $vf8, $vf3 \n\t"
        "vmulax.xyzw  $ACC, $vf5, $vf4 \n\t"
        "vmadday.xyzw $ACC, $vf6, $vf4 \n\t"
        "vmaddaz.xyzw $ACC, $vf7, $vf4 \n\t"
        "vmaddw.xyzw  $vf4, $vf8, $vf4 \n\t"
        "sqc2         $vf1, 0x00(%0) \n\t"
        "sqc2         $vf2, 0x10(%0) \n\t"
        "sqc2         $vf3, 0x20(%0) \n\t"
        "sqc2         $vf4, 0x30(%0) \n\t"
        :
        : "r"(temp.matrix), "r"(other.matrix), "r"(matrix)
        : "memory"
    );
    return set(temp);
}

vec4 mat4::operator*(const vec4& vector) const
{
    vec4 output;
    asm volatile(
        "lqc2		$vf1, 0x00(%2)	\n"
        "lqc2		$vf2, 0x10(%2)	\n"
        "lqc2		$vf3, 0x20(%2)	\n"
        "lqc2		$vf4, 0x30(%2)	\n"
        "lqc2		$vf5, 0x00(%1)	\n"
        "vmulaw	$ACC, $vf4, $vf0\n"
        "vmaddax	$ACC, $vf1, $vf5\n"
        "vmadday	$ACC, $vf2, $vf5\n"
        "vmaddz	$vf6, $vf3, $vf5\n"
        "sqc2		$vf6, 0x00(%0)	\n"
        :
        : "r"(output.vector), "r"(vector.vector), "r"(matrix)
    );
    return output;
}

std::ostream& operator<<(std::ostream& os, const mat4& matrix)
{
    os << std::fixed << std::setprecision(2);
    for (int i = 0; i < 4; ++i) 
        for (int j = 0; j < 4; ++j) 
        {
            os << "m" << i << j << "=" << matrix.m[i][j];
            if(j < 4 || i < 3) os << " ";
        }
    return os;
}

mat4 translation(const vec3& position)
{
    mat4 output = mat4();
    output.set_translate(position);
    return output;
}

mat4 rotationX(const float& r)
{
    mat4 output = mat4();
    output.set_rotate_x(r);
    return output;
}

mat4 rotationY(const float& r)
{
    mat4 output = mat4();
    output.set_rotate_y(r);
    return output;
}

mat4 rotationZ(const float& r)
{
    mat4 output = mat4();
    output.set_rotate_z(r);
    return output;
}

mat4 rotation(const vec3& rotation)
{
    return rotationZ(rotation.z) * rotationY(rotation.y) * rotationX(rotation.x);
}

mat4 scale(const vec3& scale)
{
    mat4 output = mat4();
    output.set_scale(scale);
    return output;
}

mat4 transformation(const vec3& position, const vec3& rotation, const vec3& scale)
{
    return pse::math::scale(scale) * pse::math::rotation(rotation) * pse::math::translation(position);
}

}