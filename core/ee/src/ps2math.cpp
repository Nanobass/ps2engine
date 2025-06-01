#include <ps2math.hpp>
#include <math3d.h>

extern volatile const u32 MATH3DX_ATAN_TABLE[9] alignas(sizeof(float)) = {
    0x3f7ffff5, 0xbeaaa61c, 0x3e4c40a6, 0xbe0e6c63, 0x3dc577df,
    0xbd6501c4, 0x3cb31652, 0xbb84d7e7, 0x3f490fdb,
};

extern volatile const float MATH3DX_ATAN_TABLE2[8] = {
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

float isqrt(const float& x) { return 1.0F / sqrt(x); }

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
        "la			$8, MATH3DX_ATAN_TABLE    \n\t"
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
        "la			$7, MATH3DX_ATAN_TABLE2    \n\t"
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

float randomf(const float& min, const float& max) 
{
  float random = ((float)rand()) / (float)RAND_MAX;
  float diff = max - min;
  float r = random * diff;
  return min + r;
}

int randomi(const int& min, const int& max) 
{
  return rand() % (max - min + 1) + min;
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

bool equalf(const float& a, const float& b, const float& epsilon) 
{
  return fabs(a - b) < epsilon;
}

float mod(const float& x, const float& y) 
{
  /*
   * Portable fmod(x,y) implementation for systems
   * that don't have it. Adapted from code found here:
   * http://www.opensource.apple.com/source/mPlanes[pY].thon/mPlanes[pY].thon-3/mPlanes[pY].thon/Python/fmod.c
   */
  float i, f;

  if (fabs(y) < 0.00001F) {
    return 0.0F;
  }

  i = floorf(x / y);
  f = x - i * y;

  if ((x < 0.0f) != (y < 0.0f)) {
    f = f - y;
  }

  return f;
}

float acos(const float& x) 
{
  float y = sqrt(1.0f - x * x);
  float t = atan2(y, x);
  return t;
}

float sin(const float& x) { return pse::math::cos(x - HALF_PI); }

float tan(const float& x) { return pse::math::sin(x) / pse::math::cos(x); }

//
//  Matrix Math
//

mat4::mat4()
{
    identity(*this);
}

mat4::mat4(const mat4& matrix)
{
    matrix_copy(this->matrix, (float*)matrix.matrix);
}

void mat4::operator=(const mat4& matrix)
{
    matrix_copy(this->matrix, (float*)matrix.matrix);
}

mat4 operator*(const mat4& lhs, const mat4& rhs)
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
        : "r"(output.matrix), "r"(rhs.matrix), "r"(lhs.matrix)
        : "memory"
    );
    return output;
}

vec4 operator*(const mat4& matrix, const vec4& vector)
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
        : "r"(output.vector), "r"(vector.vector), "r"(matrix.matrix)
    );
    return output;
}

vec4 operator*(const vec4& vector, const mat4& matrix)
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
        : "r"(output.vector), "r"(vector.vector), "r"(matrix.matrix)
    );
    return output;
}

std::ostream& operator<<(std::ostream& os, const mat4& matrix)
{
    for (int i = 0; i < 4; ++i) 
        for (int j = 0; j < 4; ++j) 
        {
            os << "m" << i << j << "=" << matrix.m[i][j];
            if(j < 4 || i < 3) os << " ";
        }
    return os;
}

void identity(const mat4& matrix)
{
    asm volatile(
        "vsub.xyzw  $vf4, $vf0, $vf0 \n\t"
        "vadd.w     $vf4, $vf4, $vf0 \n\t"
        "vmr32.xyzw $vf5, $vf4       \n\t"
        "vmr32.xyzw $vf6, $vf5       \n\t"
        "vmr32.xyzw $vf7, $vf6       \n\t"
        "sqc2       $vf4, 0x30(%0)   \n\t"
        "sqc2       $vf5, 0x20(%0)   \n\t"
        "sqc2       $vf6, 0x10(%0)   \n\t"
        "sqc2       $vf7, 0x0(%0)    \n\t"
        :
        : "r"(matrix.matrix)
    );
}

mat4 transpose(const mat4& matrix)
{
    mat4 output;
    matrix_transpose(output.matrix, (float*) matrix.matrix);
    return output;
}

mat4 translation(const vec4& positon)
{
    mat4 output = mat4();
    output.matrix[12] = positon.x;
    output.matrix[13] = positon.y;
    output.matrix[14] = positon.z;
    output.matrix[15] = positon.w;
    return output;
}

mat4 rotationX(const float& r)
{
    mat4 output = mat4();
    float c = cos(r);
    float s = sin(r);
    output.matrix[5] = c;   
    output.matrix[6] = s;   
    output.matrix[9] = -s;  
    output.matrix[10] = c;  
    return output;
}

mat4 rotationY(const float& r)
{
    mat4 output = mat4();
    float c = cos(r);
    float s = sin(r);
    output.matrix[0] = c;  
    output.matrix[2] = -s; 
    output.matrix[8] = s;  
    output.matrix[10] = c; 
    return output;
}

mat4 rotationZ(const float& r)
{
    mat4 output = mat4();
    float c = cos(r);
    float s = sin(r);
    output.matrix[0] = c;  
    output.matrix[1] = s;  
    output.matrix[4] = -s; 
    output.matrix[5] = c;  
    return output;
}

mat4 rotation(const vec4& rotation)
{
    return rotationZ(rotation.z) * rotationY(rotation.y) * rotationX(rotation.x);
}

mat4 scale(const vec4& scale)
{
    mat4 output = mat4();
    output.matrix[0] = scale.x;
    output.matrix[5] = scale.y;
    output.matrix[10] = scale.z;
    output.matrix[15] = scale.w;
    return output;
}

mat4 transformation(const vec4& position, const vec4& rotation, const vec4& scale)
{
    return pse::math::scale(scale) * pse::math::rotation(rotation) * pse::math::translation(position);
}

mat4 lookAt(const vec4& position, const vec4& lookAt, const vec4& up)
{
    mat4 output;
    vec4 viewvec, upvec;
    asm volatile(
        // eye
        "lqc2          $vf4, 0x00(%2)      \n\t"
        // obj
        "lqc2		    $vf5, 0x00(%3)      \n\t"
        // view_vec = $vf7
        "vsub.xyz	    $vf7, $vf4, $vf5    \n\t"
        "vmove.xyzw   $vf6, $vf0          \n\t"
        // $vf6 = { 0.0f, 1.0f, 0.0f, 1.0f }
        "vaddw.y		$vf6, $vf0, $vf0    \n\t"
        "vopmula.xyz	$ACC, $vf6, $vf7    \n\t"
        // vec = $vf9
        "vopmsub.xyz	$vf9, $vf7, $vf6    \n\t"
        "vopmula.xyz	$ACC, $vf7, $vf9    \n\t"
        // up_vec = $vf8
        "vopmsub.xyz	$vf8, $vf9, $vf7    \n\t"
        // view_vec
        "sqc2		    $vf7, 0x00(%0)      \n\t"
        // up_vec
        "sqc2		    $vf6, 0x00(%1)      \n\t"
        :
        : "r"(viewvec.vector), "r"(upvec.vector), "r"(position.vector), "r"(lookAt.vector)
    );

    asm volatile(
        "lqc2         $vf9, 0x00(%2)          \n\t"
        // mtmp.unit()
        "lqc2		    $vf10, 0x00(%3)         \n\t"
        // mtmp[1][PW] = 0.0F
        "vsub.w       $vf5, $vf0, $vf0        \n\t"
        // vtmp.outerProduct(vy, vz);
        "vopmula.xyz  $ACC, $vf10, $vf9       \n\t"
        "vopmsub.xyz	$vf11, $vf9, $vf10      \n\t"
        // mtmp[0] = vtmp.normalize();
        "vmul.xyz	    $vf12, $vf11, $vf11     \n\t"
        "vaddy.x		$vf12, $vf12, $vf12     \n\t"
        "vaddz.x		$vf12, $vf12, $vf12     \n\t"
        "vrsqrt       $Q, $vf0w, $vf12x       \n\t"
        "vsub.xyzw    $vf4, $vf0, $vf0        \n\t"
        "vwaitq                               \n\t"
        "vmulq.xyz    $vf4, $vf11, $Q         \n\t"
        // mtmp[2] = vz.normalize();
        "vmul.xyz     $vf12, $vf9, $vf9       \n\t"
        "vaddy.x		$vf12, $vf12, $vf12     \n\t"
        "vaddz.x		$vf12, $vf12, $vf12     \n\t"
        "vrsqrt       $Q, $vf0w, $vf12x       \n\t"
        "vsub.xyzw    $vf6, $vf0, $vf0        \n\t"
        "vwaitq                               \n\t"
        "vmulq.xyz    $vf6, $vf9, $Q          \n\t"
        // mtmp[1].outerProduct(mtmp[2], mtmp[0]);
        "vopmula.xyz	$ACC, $vf6, $vf4        \n\t"
        "vopmsub.xyz	$vf5, $vf4, $vf6        \n\t"
        // mtmp.transpose(pos);
        "lqc2		    $vf7, 0x00(%1)          \n\t"
        // m = mtmp.inverse();
        "qmfc2.ni		$11, $vf0               \n\t"
        "qmfc2.ni		$8, $vf4                \n\t"
        "qmfc2.ni		$9, $vf5                \n\t"
        "qmfc2.ni		$10, $vf6               \n\t"

        "pextlw		$12, $9, $8             \n\t"
        "pextuw		$13, $9, $8             \n\t"
        "pextlw		$14, $11, $10           \n\t"
        "pextuw		$15, $11, $10           \n\t"
        "pcpyld		$8, $14, $12            \n\t"
        "pcpyud		$9, $12, $14            \n\t"
        "pcpyld		$10, $15, $13           \n\t"

        "qmtc2.ni		$8, $vf16               \n\t"
        "qmtc2.ni		$9, $vf17               \n\t"
        "qmtc2.ni		$10, $vf18              \n\t"
        "vmulax.xyz	$ACC, $vf16, $vf7       \n\t"
        "vmadday.xyz	$ACC, $vf17, $vf7       \n\t"
        "vmaddz.xyz	$vf5, $vf18, $vf7       \n\t"
        "vsub.xyzw	$vf5, $vf0, $vf5        \n\t"

        "sq			$8, 0x00(%0)            \n\t"
        "sq			$9, 0x10(%0)            \n\t"
        "sq			$10, 0x20(%0)           \n\t"
        "sqc2			$vf5, 0x30(%0)          \n\t"
        :
        : "r"(output.matrix), "r"(position.vector), "r"(viewvec.vector), "r"(upvec.vector)
    );
    return output;
}

mat4 projection(const float& fov, const float& width, const float& height, const float& scale, const float& ratio, const float& near, const float& far)
{
    float fovYdiv2 = HALF_ANG2RAD * fov;
    float cotFOV = 1.0F / (pse::math::sin(fovYdiv2) / pse::math::cos(fovYdiv2));
    float w = cotFOV * (width / scale) / ratio;
    float h = cotFOV * (height / scale);

    mat4 output;
    output.matrix[0] = w;
    output.matrix[1] = 0.0F;
    output.matrix[2] = 0.0F;
    output.matrix[3] = 0.0F;

    output.matrix[4] = 0.0F;
    output.matrix[5] = h;
    output.matrix[6] = 0.0F;
    output.matrix[7] = 0.0F;

    output.matrix[8] = 0.0F;
    output.matrix[9] = 0.0F;
    output.matrix[10] = (far + near) / (far - near);
    output.matrix[11] = -1.0F;

    output.matrix[12] = 0.0F;
    output.matrix[13] = 0.0F;
    output.matrix[14] = (2.0F * far * near) / (far - near);
    output.matrix[15] = 0.0F;
    return output;
}

//
//  vec4 Math
//

vec4::vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

vec4::vec4(const vec4& vector)
{
    asm volatile(
        "lqc2		$vf1, 0x00(%1)	\n"
        "sqc2		$vf1, 0x00(%0)	\n"
        :
        : "r"(this->vector), "r"(vector.vector)
    );
}

vec4::vec4(const vec3& vector, float w_)
{
    x = vector.x;
    y = vector.y;
    z = vector.z;
    w = w_;
}

vec4::vec4(const vec2& vector, float z_, float w_)
{
    x = vector.x;
    y = vector.y;
    z = z_;
    w = w_;
}

void vec4::operator=(const vec4& vector)
{
    asm volatile(
        "lqc2		$vf1, 0x00(%1)	\n"
        "sqc2		$vf1, 0x00(%0)	\n"
        :
        : "r"(this->vector), "r"(vector.vector)
    );
}

vec4 operator+(const vec4& lhs, const vec4& rhs) 
{
    vec4 output;
    asm __volatile__ (
      "lqc2       $vf4,   0x0(%1)         \n\t"
      "lqc2       $vf5,   0x0(%2)         \n\t"
      "vadd.xyz   $vf6,   $vf4,   $vf5    \n\t"
      "sqc2       $vf6,   0x0(%0)         \n\t"
      :
      : "r"(output.vector), "r"(lhs.vector), "r"(rhs.vector)
    );
    return output;
}

void operator+=(vec4& lhs, const vec4& rhs)
{
    asm volatile(
        "lqc2      $vf4, 0x0(%0)    \n\t"
        "lqc2      $vf5, 0x0(%1)    \n\t"
        "vadd.xyz  $vf4, $vf4, $vf5 \n\t"
        "sqc2      $vf4, 0x0(%0)    \n\t"
        :
        : "r"(lhs.vector), "r"(rhs.vector)
    );
}

vec4 operator-(const vec4& lhs, const vec4& rhs)
{
    vec4 output;
    asm __volatile__ (
      "lqc2       $vf4,   0x0(%1)         \n\t"
      "lqc2       $vf5,   0x0(%2)         \n\t"
      "vsub.xyz   $vf6,   $vf4,   $vf5    \n\t"
      "sqc2       $vf6,   0x0(%0)         \n\t"
      :
      : "r"(output.vector), "r"(lhs.vector), "r"(rhs.vector)
    );
    return output;
}

void operator-=(vec4& lhs, const vec4& rhs)
{
    asm volatile(
        "lqc2      $vf4, 0x0(%0)    \n\t"
        "lqc2      $vf5, 0x0(%1)    \n\t"
        "vsub.xyz  $vf4, $vf4, $vf5 \n\t"
        "sqc2      $vf4, 0x0(%0)    \n\t"
        :
        : "r"(lhs.vector), "r"(rhs.vector)
    );
}

vec4 operator*(const vec4& lhs, const vec4& rhs)
{
    vec4 output;
    asm __volatile__ (
      "lqc2       $vf4,   0x0(%1)         \n\t"
      "lqc2       $vf5,   0x0(%2)         \n\t"
      "vmul.xyzw   $vf6,   $vf4,   $vf5    \n\t"
      "sqc2       $vf6,   0x0(%0)         \n\t"
      :
      : "r"(output.vector), "r"(lhs.vector), "r"(rhs.vector)
    );
    return output;
}

void operator*=(vec4& lhs, const vec4& rhs)
{
    asm volatile(
        "lqc2      $vf4, 0x0(%0)    \n\t"
        "lqc2      $vf5, 0x0(%1)    \n\t"
        "vmul.xyzw  $vf4, $vf4, $vf5 \n\t"
        "sqc2      $vf4, 0x0(%0)    \n\t"
        :
        : "r"(lhs.vector), "r"(rhs.vector)
    );
}

vec4 operator*(const vec4& vector, float scalar)
{
    vec4 output;
    asm volatile(
        "lqc2       $vf4,   0x0(%1)         \n\t"
        "mfc1       $8,     %2              \n\t"
        "qmtc2      $8,     $vf5            \n\t"
        "vmulx.xyz  $vf6,   $vf4,   $vf5    \n\t"
        "vmove.w    $vf6,   $vf4            \n\t"
        "sqc2       $vf6,   0x0(%0)         \n\t"
        :
        : "r"(output.vector), "r"(vector.vector), "f"(scalar)
    );
    return output;
}

vec4 operator*(float scalar, const vec4& vector)
{
    vec4 output;
    asm volatile(
        "lqc2       $vf4,   0x0(%1)         \n\t"
        "mfc1       $8,     %2              \n\t"
        "qmtc2      $8,     $vf5            \n\t"
        "vmulx.xyz  $vf6,   $vf4,   $vf5    \n\t"
        "vmove.w    $vf6,   $vf4            \n\t"
        "sqc2       $vf6,   0x0(%0)         \n\t"
        :
        : "r"(output.vector), "r"(vector.vector), "f"(scalar)
    );
    return output;
}

void operator*=(vec4& vector, float scalar)
{
    asm volatile(
        "lqc2       $vf4, 0x0(%0)     \n\t"
        "mfc1       $8,   %1          \n\t"
        "qmtc2      $8,   $vf5        \n\t"
        "vmulx.xyz  $vf4, $vf4, $vf5  \n\t"
        "sqc2       $vf4, 0x0(%0)     \n\t"
        :
        : "r"(vector.vector), "f"(scalar)
    );
}

vec4 operator/(const vec4& lhs, const vec4& rhs)
{
    return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w};
}

void operator/=(vec4& lhs, const vec4& rhs)
{
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    lhs.z /= rhs.z;
    lhs.w /= rhs.w;
}

vec4 operator/(const vec4& vector, float scalar)
{
    vec4 output;
    asm volatile(
        "mfc1		$8, %2            \n\t"
        "qmtc2  $8, $vf5          \n\t"
        "vdiv		$Q, $vf0w, $vf5x  \n\t"
        "lqc2		$vf4, 0x00(%1)    \n\t"
        "vwaitq                   \n\t"
        "vmulq.xyz	$vf4, $vf4, $Q\n\t"
        "sqc2		$vf4, 0x00(%0)    \n\t"
        :
        : "r"(output.vector), "r"(vector.vector), "f"(scalar)
        : "$8"
    );
    return output;
}

vec4 operator/(float scalar, const vec4& vector)
{
    vec4 output;
    asm volatile(
        "mfc1		$8, %2            \n\t"
        "qmtc2  $8, $vf5          \n\t"
        "vdiv		$Q, $vf0w, $vf5x  \n\t"
        "lqc2		$vf4, 0x00(%1)    \n\t"
        "vwaitq                   \n\t"
        "vmulq.xyz	$vf4, $vf4, $Q\n\t"
        "sqc2		$vf4, 0x00(%0)    \n\t"
        :
        : "r"(output.vector), "r"(vector.vector), "f"(scalar)
        : "$8"
    );
    return output;
}

void operator/=(vec4& vector, float scalar)
{
    asm volatile(
        "mfc1		$8, %1            \n\t"
        "qmtc2  $8, $vf5          \n\t"
        "vdiv		$Q, $vf0w, $vf5x  \n\t"
        "lqc2		$vf4, 0x00(%0)    \n\t"
        "vwaitq                   \n\t"
        "vmulq.xyz	$vf4, $vf4, $Q\n\t"
        "sqc2		$vf4, 0x00(%0)    \n\t"
        :
        : "r"(vector.vector), "f"(scalar)
        : "$8"
    );
}

vec4 operator-(const vec4& vector)
{
    return {-vector.x, -vector.y, -vector.z, vector.w};
}

float dot(const vec4& lhs, const vec4& rhs)
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
        : "r"(lhs.vector), "r"(rhs.vector)
    );
    return output;
}

float dot3(const vec4& lhs, const vec4& rhs)
{
    float output;
    asm volatile(
        "lqc2     $vf4, 0x0(%1)  \n\t"
        "lqc2     $vf5, 0x0(%2)  \n\t"
        "vmul.xyz $vf6, $vf4, $vf5 \n\t"
        "vaddy.x  $vf6, $vf6, $vf6 \n\t"
        "vaddz.x  $vf6, $vf6, $vf6 \n\t"
        "qmfc2    $2,  $vf6      \n\t"
        "mtc1     $2,  %0       \n\t"
        : "=f"(output)
        : "r"(lhs.vector), "r"(rhs.vector)
    );
    return output;
}

vec4 cross(const vec4& lhs, const vec4& rhs)
{
    vec4 output;
    asm volatile(
        "lqc2           $vf4,   0x0(%1)         \n\t"
        "lqc2           $vf5,   0x0(%2)         \n\t"
        "vopmula.xyz    $ACC,   $vf4,   $vf5    \n\t"
        "vopmsub.xyz    $vf8,   $vf5,   $vf4    \n\t"
        "vsub.w         $vf8,   $vf0,   $vf0    \n\t"
        "sqc2           $vf8,   0x0(%0)         \n\t"
        :
        : "r"(output.vector), "r"(lhs.vector), "r"(rhs.vector)
    );
    return output;
}

vec4 normalize(const vec4& vector)
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
        : "r"(vector.vector), "r"(output.vector)
    );
    return output;
}

vec4 normalize3(const vec4& vector)
{
    vec4 output;
    asm volatile(
        "lqc2       $vf4, 0x0(%0)    \n\t"
        "vmul.xyz   $vf5, $vf4,  $vf4  \n\t"
        "vaddy.x    $vf5, $vf5,  $vf5  \n\t"
        "vaddz.x    $vf5, $vf5,  $vf5  \n\t"
        "vrsqrt     $Q,   $vf0w, $vf5x \n\t"
        "vwaitq                     \n\t"
        "vsub.xyz   $vf6, $vf0,  $vf0  \n\t"
        "vaddw.xyz  $vf6, $vf6,  $vf4  \n\t"
        "vwaitq                     \n\t"
        "vmulq.xyz  $vf6, $vf4,  $Q    \n\t"
        "sqc2       $vf6, 0x0(%1)    \n\t"
        :
        : "r"(vector.vector), "r"(output.vector)
    );
    return output;
}

vec4 lerp(const vec4& lhs, const vec4& rhs, float f)
{
    vec4 output;
    asm volatile(
        "lqc2       $vf4, 0x0(%1)       \n\t"  // $vf4 = v1
        "lqc2       $vf5, 0x0(%2)       \n\t"  // $vf5 = v2
        "mfc1       $8,  %3             \n\t"  // $vf6 = t
        "qmtc2      $8,  $vf6           \n\t"  // lerp:
        "vsub.xyzw  $vf7, $vf5, $vf4    \n\t"  // $vf7 = v2 - v1
        "vmulx.xyzw $vf8, $vf7, $vf6    \n\t"  // $vf8 = $vf7 * t
        "vadd.xyzw  $vf9, $vf8, $vf4    \n\t"  // $vf9 = $vf8 + $vf4
        "sqc2       $vf9, 0x0(%0)       \n\t"  // v0  = $vf9
        :
        : "r"(output.vector), "r"(lhs.vector), "r"(rhs.vector), "f"(f)
    );
    return output;
}

std::ostream& operator<<(std::ostream& os, const vec4& vector)
{
    os << "x=" << vector.x << " y=" << vector.y << " z=" << vector.z << " w=" << vector.w;
    return os;
}

//
//  vec3 Math
//

vec3::vec3(float x, float y, float z) : x(x), y(y), z(z) {}

vec3::vec3(const vec4& vector)
{
    x = vector.x;
    y = vector.y;
    z = vector.z;
}

vec3::vec3(const vec3& vector)
{
    x = vector.x;
    y = vector.y;
    z = vector.z;
}

vec3::vec3(const vec2& vector)
{
    x = vector.x;
    y = vector.y;
    z = 0.0F;
}

void vec3::operator=(const vec3& vector) { x = vector.x; y = vector.y; z = vector.z; }

vec3 operator+(const vec3& lhs, const vec3& rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

void operator+=(vec3& lhs, const vec3& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
}

vec3 operator-(const vec3& lhs, const vec3& rhs) 
{    
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

void operator-=(vec3& lhs, const vec3& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
}

vec3 operator*(const vec3& lhs, const vec3& rhs)
{
    return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z};
}

void operator*=(vec3& lhs, const vec3& rhs)
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    lhs.z *= rhs.z;
}

vec3 operator*(const vec3& vector, float scalar)
{
    return {vector.x * scalar, vector.y * scalar, vector.z * scalar};
}

vec3 operator*(float scalar, const vec3& vector)
{
    return {vector.x * scalar, vector.y * scalar, vector.z * scalar};
}

void operator*=(vec3& vector, float scalar)
{
    vector.x *= scalar;
    vector.y *= scalar;
    vector.z *= scalar;
}

vec3 operator/(const vec3& lhs, const vec3& rhs)
{
    return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z};
}

void operator/=(vec3& lhs, const vec3& rhs)
{
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    lhs.z /= rhs.z;
}

vec3 operator/(const vec3& vector, float scalar)
{
    return {vector.x / scalar, vector.y / scalar, vector.z / scalar};
}

vec3 operator/(float scalar, const vec3& vector)
{
    return {vector.x / scalar, vector.y / scalar, vector.z / scalar};
}

void operator/=(vec3& vector, float scalar)
{
    vector.x /= scalar;
    vector.y /= scalar;
    vector.z /= scalar;
}

vec3 operator-(const vec3& vector) 
{
    return { -vector.x, -vector.y, -vector.z };
}

float dot(const vec3& lhs, const vec3& rhs)
{
    return dot(vec4(lhs), vec4(rhs));
}

vec3 cross(const vec3& lhs, const vec3& rhs)
{
    return vec3(cross(vec4(lhs), vec4(rhs)));
}

vec3 normalize(const vec3& vector)
{
    return vec3(normalize(vec4(vector, 0.0F)));
}

std::ostream& operator<<(std::ostream& os, const vec3& vector)
{
    os << "x=" << vector.x << " y=" << vector.y << " z=" << vector.z;
    return os;
}

//
//  vec2 Math
//

vec2::vec2(float x, float y) : x(x), y(y) {}

vec2::vec2(const vec4& vector)
{
    x = vector.x;
    y = vector.y;
}

vec2::vec2(const vec3& vector)
{
    x = vector.x;
    y = vector.y;
}

vec2::vec2(const vec2& vector)
{
    x = vector.x;
    y = vector.y;
}

void vec2::operator=(const vec2& vector) { x = vector.x; y = vector.y; }

vec2 operator+(const vec2& lhs, const vec2& rhs) 
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

void operator+=(vec2& lhs, const vec2& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
}

vec2 operator-(const vec2& lhs, const vec2& rhs) 
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

void operator-=(vec2& lhs, const vec2& rhs) 
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
}

vec2 operator*(const vec2& lhs, const vec2& rhs) 
{
    return {lhs.x * rhs.x, lhs.y * rhs.y};
}

void operator*=(vec2& lhs, const vec2& rhs) 
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
}

vec2 operator*(const vec2& vector, float scalar)
{
    return {vector.x * scalar, vector.y * scalar};
}

vec2 operator*(float scalar, const vec2& vector) 
{
    return {vector.x * scalar, vector.y * scalar};
}

void operator*=(vec2& vector, float scalar) 
{
    vector.x *= scalar;
    vector.y *= scalar;
}

vec2 operator/(const vec2& lhs, const vec2& rhs) 
{
    return {lhs.x / rhs.x, lhs.y / rhs.y};
}

void operator/=(vec2& lhs, const vec2& rhs) 
{
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
}

vec2 operator/(const vec2& vector, float scalar) 
{
    return {vector.x / scalar, vector.y / scalar};
}

vec2 operator/(float scalar, const vec2& vector) 
{
    return {scalar / vector.x, scalar / vector.y};
}

void operator/=(vec2& vector, float scalar) 
{
    vector.x /= scalar;
    vector.y /= scalar;
}

vec2 operator-(const vec2& vector) 
{
    return {-vector.x, -vector.y};
}

float dot(const vec2& lhs, const vec2& rhs)
{
    return dot(vec4(lhs), vec4(rhs));
}

vec2 normalize(const vec2& vector)
{
    return vec2(normalize(vec4(vector, 0.0F, 0.0F)));
}

std::ostream& operator<<(std::ostream& os, const vec2& vector)
{
    os << "x=" << vector.x << " y=" << vector.y;
    return os;
}

}