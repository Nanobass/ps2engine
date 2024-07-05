#include <ps2math.hpp>
#include <math3d.h>

extern volatile const u32 MATH3DX_ATAN_TABLE[9] alignas(sizeof(float)) = {
    0x3f7ffff5, 0xbeaaa61c, 0x3e4c40a6, 0xbe0e6c63, 0x3dc577df,
    0xbd6501c4, 0x3cb31652, 0xbb84d7e7, 0x3f490fdb,
};

extern volatile const float MATH3DX_ATAN_TABLE2[8] = {
    0.0f,
    (Math::PI / 2.0f),
    (Math::PI / 2.0f),
    (Math::PI),
    (-Math::PI),
    (-Math::PI / 2.0f),
    (-Math::PI / 2.0f),
    0.0f,
};

namespace Math {

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

float sin(const float& x) { return Math::cos(x - HALF_PI); }

float tan(const float& x) { return Math::sin(x) / Math::cos(x); }

//
//  Matrix Math
//

Mat4::Mat4()
{
    Identity(*this);
}

Mat4::Mat4(const Mat4& matrix)
{
    matrix_copy(this->matrix, (float*)matrix.matrix);
}

/*
Mat4::Mat4(Quaternion q)
{
    float x = q.x, y = q.y, z = q.z, w = q.w;
    float x2 = x * x, y2 = y * y, z2 = z * z;
    float xy = x * y, xz = x * z, yz = y * z;
    float wx = w * x, wy = w * y, wz = w * z;

    m[0][0] = 1.0f - 2.0f * (y2 + z2);
    m[0][1] = 2.0f * (xy - wz);
    m[0][2] = 2.0f * (xz + wy);

    m[1][0] = 2.0f * (xy + wz);
    m[1][1] = 1.0f - 2.0f * (x2 + z2);
    m[1][2] = 2.0f * (yz - wx);

    m[2][0] = 2.0f * (xz - wy);
    m[2][1] = 2.0f * (yz + wx);
    m[2][2] = 1.0f - 2.0f * (x2 + y2);

    m[3][3] = 1.0f;
}
*/

void Mat4::operator=(const Mat4& matrix)
{
    matrix_copy(this->matrix, (float*)matrix.matrix);
}

Mat4 operator*(const Mat4& lhs, const Mat4& rhs)
{
    Mat4 output;
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

Vec4 operator*(const Mat4& matrix, const Vec4& vector)
{
    Vec4 output;
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

Vec4 operator*(const Vec4& vector, const Mat4& matrix)
{
    Vec4 output;
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

std::ostream& operator<<(std::ostream& os, const Mat4& matrix)
{
    for (int i = 0; i < 4; ++i) 
        for (int j = 0; j < 4; ++j) 
        {
            os << "m" << i << j << "=" << matrix.m[i][j];
            if(j < 4 || i < 3) os << " ";
        }
    return os;
}

void Identity(const Mat4& matrix)
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

Mat4 Transpose(const Mat4& matrix)
{
    Mat4 output;
    matrix_transpose(output.matrix, (float*) matrix.matrix);
    return output;
}

Mat4 Translation(const Vec4& positon)
{
    Mat4 output = Mat4();
    output.matrix[12] = positon.x;
    output.matrix[13] = positon.y;
    output.matrix[14] = positon.z;
    output.matrix[15] = positon.w;
    return output;
}

Mat4 RotationX(const float& r)
{
    Mat4 output = Mat4();
    float c = cos(r);
    float s = sin(r);
    output.matrix[5] = c;   
    output.matrix[6] = s;   
    output.matrix[9] = -s;  
    output.matrix[10] = c;  
    return output;
}

Mat4 RotationY(const float& r)
{
    Mat4 output = Mat4();
    float c = cos(r);
    float s = sin(r);
    output.matrix[0] = c;  
    output.matrix[2] = -s; 
    output.matrix[8] = s;  
    output.matrix[10] = c; 
    return output;
}

Mat4 RotationZ(const float& r)
{
    Mat4 output = Mat4();
    float c = cos(r);
    float s = sin(r);
    output.matrix[0] = c;  
    output.matrix[1] = s;  
    output.matrix[4] = -s; 
    output.matrix[5] = c;  
    return output;
}

Mat4 Rotation(const Vec4& rotation)
{
    return RotationZ(rotation.z) * RotationY(rotation.y) * RotationX(rotation.x);
}

Mat4 Scale(const Vec4& scale)
{
    Mat4 output = Mat4();
    output.matrix[0] = scale.x;
    output.matrix[5] = scale.y;
    output.matrix[10] = scale.z;
    output.matrix[15] = scale.w;
    return output;
}

Mat4 Transformation(const Vec4& position, const Vec4& rotation, const Vec4& scale)
{
    return Scale(scale) * Rotation(rotation) * Translation(position);
}

Mat4 LookAt(const Vec4& position, const Vec4& lookAt, const Vec4& up)
{
    Mat4 output;
    Vec4 viewVec, upVec;
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
        : "r"(viewVec.vector), "r"(upVec.vector), "r"(position.vector), "r"(lookAt.vector)
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
        : "r"(output.matrix), "r"(position.vector), "r"(viewVec.vector), "r"(upVec.vector)
    );
    return output;
}

Mat4 Projection(const float& fov, const float& width, const float& height, const float& scale, const float& ratio, const float& near, const float& far)
{
    float fovYdiv2 = HALF_ANG2RAD * fov;
    float cotFOV = 1.0F / (Math::sin(fovYdiv2) / Math::cos(fovYdiv2));
    float w = cotFOV * (width / scale) / ratio;
    float h = cotFOV * (height / scale);

    Mat4 output;
    output.matrix[0] = w;
    output.matrix[1] = 0.0F;
    output.matrix[2] = 0.0F;
    output.matrix[3] = 0.0F;

    output.matrix[4] = 0.0F;
    output.matrix[5] = -h;
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
//  Vec4 Math
//

Vec4::Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

Vec4::Vec4(const Vec4& vector)
{
    asm volatile(
        "lqc2		$vf1, 0x00(%1)	\n"
        "sqc2		$vf1, 0x00(%0)	\n"
        :
        : "r"(this->vector), "r"(vector.vector)
    );
}

Vec4::Vec4(const Vec3& vector, float w_)
{
    x = vector.x;
    y = vector.y;
    z = vector.z;
    w = w_;
}

Vec4::Vec4(const Vec2& vector, float z_, float w_)
{
    x = vector.x;
    y = vector.y;
    z = z_;
    w = w_;
}

void Vec4::operator=(const Vec4& vector)
{
    asm volatile(
        "lqc2		$vf1, 0x00(%1)	\n"
        "sqc2		$vf1, 0x00(%0)	\n"
        :
        : "r"(this->vector), "r"(vector.vector)
    );
}

Vec4 operator+(const Vec4& lhs, const Vec4& rhs) 
{
    Vec4 output;
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

void operator+=(Vec4& lhs, const Vec4& rhs)
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

Vec4 operator-(const Vec4& lhs, const Vec4& rhs)
{
    Vec4 output;
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

void operator-=(Vec4& lhs, const Vec4& rhs)
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

Vec4 operator*(const Vec4& lhs, const Vec4& rhs)
{
    Vec4 output;
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

void operator*=(Vec4& lhs, const Vec4& rhs)
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

Vec4 operator*(const Vec4& vector, float scalar)
{
    Vec4 output;
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

Vec4 operator*(float scalar, const Vec4& vector)
{
    Vec4 output;
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

void operator*=(Vec4& vector, float scalar)
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

Vec4 operator/(const Vec4& lhs, const Vec4& rhs)
{
    return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w};
}

void operator/=(Vec4& lhs, const Vec4& rhs)
{
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    lhs.z /= rhs.z;
    lhs.w /= rhs.w;
}

Vec4 operator/(const Vec4& vector, float scalar)
{
    Vec4 output;
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

Vec4 operator/(float scalar, const Vec4& vector)
{
    Vec4 output;
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

void operator/=(Vec4& vector, float scalar)
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

Vec4 operator-(const Vec4& vector)
{
    return {-vector.x, -vector.y, -vector.z, vector.w};
}

float Dot(const Vec4& lhs, const Vec4& rhs)
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

float Dot3(const Vec4& lhs, const Vec4& rhs)
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

Vec4 Cross(const Vec4& lhs, const Vec4& rhs)
{
    Vec4 output;
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

Vec4 Normalize(const Vec4& vector)
{
    Vec4 output;
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

Vec4 Normalize3(const Vec4& vector)
{
    Vec4 output;
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

Vec4 Lerp(const Vec4& lhs, const Vec4& rhs, float f)
{
    Vec4 output;
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

std::ostream& operator<<(std::ostream& os, const Vec4& vector)
{
    os << "x=" << vector.x << " y=" << vector.y << " z=" << vector.z << " w=" << vector.w;
    return os;
}

//
//  Vec3 Math
//

Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

Vec3::Vec3(const Vec4& vector)
{
    x = vector.x;
    y = vector.y;
    z = vector.z;
}

Vec3::Vec3(const Vec3& vector)
{
    x = vector.x;
    y = vector.y;
    z = vector.z;
}

Vec3::Vec3(const Vec2& vector)
{
    x = vector.x;
    y = vector.y;
    z = 0.0F;
}

void Vec3::operator=(const Vec3& vector) { x = vector.x; y = vector.y; z = vector.z; }

Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

void operator+=(Vec3& lhs, const Vec3& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
}

Vec3 operator-(const Vec3& lhs, const Vec3& rhs) 
{    
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

void operator-=(Vec3& lhs, const Vec3& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
}

Vec3 operator*(const Vec3& lhs, const Vec3& rhs)
{
    return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z};
}

void operator*=(Vec3& lhs, const Vec3& rhs)
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
    lhs.z *= rhs.z;
}

Vec3 operator*(const Vec3& vector, float scalar)
{
    return {vector.x * scalar, vector.y * scalar, vector.z * scalar};
}

Vec3 operator*(float scalar, const Vec3& vector)
{
    return {vector.x * scalar, vector.y * scalar, vector.z * scalar};
}

void operator*=(Vec3& vector, float scalar)
{
    vector.x *= scalar;
    vector.y *= scalar;
    vector.z *= scalar;
}

Vec3 operator/(const Vec3& lhs, const Vec3& rhs)
{
    return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z};
}

void operator/=(Vec3& lhs, const Vec3& rhs)
{
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
    lhs.z /= rhs.z;
}

Vec3 operator/(const Vec3& vector, float scalar)
{
    return {vector.x / scalar, vector.y / scalar, vector.z / scalar};
}

Vec3 operator/(float scalar, const Vec3& vector)
{
    return {vector.x / scalar, vector.y / scalar, vector.z / scalar};
}

void operator/=(Vec3& vector, float scalar)
{
    vector.x /= scalar;
    vector.y /= scalar;
    vector.z /= scalar;
}

Vec3 operator-(const Vec3& vector) 
{
    return { -vector.x, -vector.y, -vector.z };
}

float Dot(const Vec3& lhs, const Vec3& rhs)
{
    return Dot(Vec4(lhs), Vec4(rhs));
}

Vec3 Cross(const Vec3& lhs, const Vec3& rhs)
{
    return Vec3(Cross(Vec4(lhs), Vec4(rhs)));
}

Vec3 Normalize(const Vec3& vector)
{
    return Vec3(Normalize(Vec4(vector, 0.0F)));
}

std::ostream& operator<<(std::ostream& os, const Vec3& vector)
{
    os << "x=" << vector.x << " y=" << vector.y << " z=" << vector.z;
    return os;
}

//
//  Vec2 Math
//

Vec2::Vec2(float x, float y) : x(x), y(y) {}

Vec2::Vec2(const Vec4& vector)
{
    x = vector.x;
    y = vector.y;
}

Vec2::Vec2(const Vec3& vector)
{
    x = vector.x;
    y = vector.y;
}

Vec2::Vec2(const Vec2& vector)
{
    x = vector.x;
    y = vector.y;
}

void Vec2::operator=(const Vec2& vector) { x = vector.x; y = vector.y; }

Vec2 operator+(const Vec2& lhs, const Vec2& rhs) 
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

void operator+=(Vec2& lhs, const Vec2& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
}

Vec2 operator-(const Vec2& lhs, const Vec2& rhs) 
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

void operator-=(Vec2& lhs, const Vec2& rhs) 
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
}

Vec2 operator*(const Vec2& lhs, const Vec2& rhs) 
{
    return {lhs.x * rhs.x, lhs.y * rhs.y};
}

void operator*=(Vec2& lhs, const Vec2& rhs) 
{
    lhs.x *= rhs.x;
    lhs.y *= rhs.y;
}

Vec2 operator*(const Vec2& vector, float scalar)
{
    return {vector.x * scalar, vector.y * scalar};
}

Vec2 operator*(float scalar, const Vec2& vector) 
{
    return {vector.x * scalar, vector.y * scalar};
}

void operator*=(Vec2& vector, float scalar) 
{
    vector.x *= scalar;
    vector.y *= scalar;
}

Vec2 operator/(const Vec2& lhs, const Vec2& rhs) 
{
    return {lhs.x / rhs.x, lhs.y / rhs.y};
}

void operator/=(Vec2& lhs, const Vec2& rhs) 
{
    lhs.x /= rhs.x;
    lhs.y /= rhs.y;
}

Vec2 operator/(const Vec2& vector, float scalar) 
{
    return {vector.x / scalar, vector.y / scalar};
}

Vec2 operator/(float scalar, const Vec2& vector) 
{
    return {scalar / vector.x, scalar / vector.y};
}

void operator/=(Vec2& vector, float scalar) 
{
    vector.x /= scalar;
    vector.y /= scalar;
}

Vec2 operator-(const Vec2& vector) 
{
    return {-vector.x, -vector.y};
}

float Dot(const Vec2& lhs, const Vec2& rhs)
{
    return Dot(Vec4(lhs), Vec4(rhs));
}

Vec2 Normalize(const Vec2& vector)
{
    return Vec2(Normalize(Vec4(vector, 0.0F, 0.0F)));
}

std::ostream& operator<<(std::ostream& os, const Vec2& vector)
{
    os << "x=" << vector.x << " y=" << vector.y;
    return os;
}

//
// Quaternion Functions
//

Quaternion::Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

void Quaternion::operator=(const Quaternion& quaternion)
{
    asm volatile(
        "lqc2		$vf1, 0x00(%1)	\n"
        "sqc2		$vf1, 0x00(%0)	\n"
        :
        : "r"(this->vector), "r"(quaternion.vector)
    );
}

Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs)
{
    Quaternion output;
    asm __volatile__ (
      "lqc2       $vf4,   0x0(%1)         \n\t"
      "lqc2       $vf5,   0x0(%2)         \n\t"
      "vadd.xyzw   $vf6,   $vf4,   $vf5    \n\t"
      "sqc2       $vf6,   0x0(%0)         \n\t"
      :
      : "r"(output.vector), "r"(lhs.vector), "r"(rhs.vector)
    );
    return output;
}

void operator+=(Quaternion& lhs, const Quaternion& rhs)
{
    asm volatile(
        "lqc2      $vf4, 0x0(%0)    \n\t"
        "lqc2      $vf5, 0x0(%1)    \n\t"
        "vadd.xyzw  $vf4, $vf4, $vf5 \n\t"
        "sqc2      $vf4, 0x0(%0)    \n\t"
        :
        : "r"(lhs.vector), "r"(rhs.vector)
    );
}

Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs)
{
    Quaternion output;
    asm __volatile__ (
      "lqc2       $vf4,   0x0(%1)         \n\t"
      "lqc2       $vf5,   0x0(%2)         \n\t"
      "vsub.xyzw   $vf6,   $vf4,   $vf5    \n\t"
      "sqc2       $vf6,   0x0(%0)         \n\t"
      :
      : "r"(output.vector), "r"(lhs.vector), "r"(rhs.vector)
    );
    return output;
}

void operator-=(Quaternion& lhs, const Quaternion& rhs)
{
    asm volatile(
        "lqc2      $vf4, 0x0(%0)    \n\t"
        "lqc2      $vf5, 0x0(%1)    \n\t"
        "vsub.xyzw  $vf4, $vf4, $vf5 \n\t"
        "sqc2      $vf4, 0x0(%0)    \n\t"
        :
        : "r"(lhs.vector), "r"(rhs.vector)
    );
}

Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
{
    return {
        lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
        lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w,
        lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z
    };
}

void operator*=(Quaternion& lhs, const Quaternion& rhs)
{
    lhs = lhs * rhs;
}

Quaternion operator*(const Quaternion& lhs, const float scalar)
{
    return {lhs.x * scalar, lhs.y * scalar, lhs.z * scalar, lhs.w * scalar};
}

void operator*=(Quaternion& lhs, const float scalar)
{
    lhs.x *= scalar;
    lhs.y *= scalar;
    lhs.z *= scalar;
    lhs.w *= scalar;
}

Quaternion operator*(const Quaternion& lhs, const Vec4& rhs)
{
    return lhs * Quaternion(rhs.x, rhs.y, rhs.z, 0) * Conjugate(lhs);
}

Quaternion Normalize(const Quaternion& quaternion)
{
    Quaternion output;
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
        : "r"(quaternion.vector), "r"(output.vector)
    );
    return output;
}

Quaternion Conjugate(const Quaternion& quaternion)
{
    return {-quaternion.x, -quaternion.y, -quaternion.z, quaternion.w};
}

std::ostream& operator<<(std::ostream& os, const Quaternion& quaternion)
{
    os << "x=" << quaternion.x << " y=" << quaternion.y << " z=" << quaternion.z << " w=" << quaternion.w;
    return os;
}

//
//  Color Functions
//

Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

void Color::operator=(const Color& vector)
{
    asm volatile(
        "lqc2		$vf1, 0x00(%1)	\n"
        "sqc2		$vf1, 0x00(%0)	\n"
        :
        : "r"(this->vector), "r"(vector.vector)
    );
}

Color operator+(const Color& lhs, const Color& rhs) 
{
    Color output;
    asm __volatile__ (
      "lqc2       $vf4,   0x0(%1)         \n\t"
      "lqc2       $vf5,   0x0(%2)         \n\t"
      "vadd.xyzw   $vf6,   $vf4,   $vf5    \n\t"
      "sqc2       $vf6,   0x0(%0)         \n\t"
      :
      : "r"(output.vector), "r"(lhs.vector), "r"(rhs.vector)
    );
    return output;
}

void operator+=(Color& lhs, const Color& rhs)
{
    asm volatile(
        "lqc2      $vf4, 0x0(%0)    \n\t"
        "lqc2      $vf5, 0x0(%1)    \n\t"
        "vadd.xyzw  $vf4, $vf4, $vf5 \n\t"
        "sqc2      $vf4, 0x0(%0)    \n\t"
        :
        : "r"(lhs.vector), "r"(rhs.vector)
    );
}

Color operator-(const Color& lhs, const Color& rhs)
{
    Color output;
    asm __volatile__ (
      "lqc2       $vf4,   0x0(%1)         \n\t"
      "lqc2       $vf5,   0x0(%2)         \n\t"
      "vsub.xyzw   $vf6,   $vf4,   $vf5    \n\t"
      "sqc2       $vf6,   0x0(%0)         \n\t"
      :
      : "r"(output.vector), "r"(lhs.vector), "r"(rhs.vector)
    );
    return output;
}

void operator-=(Color& lhs, const Color& rhs)
{
    asm volatile(
        "lqc2      $vf4, 0x0(%0)    \n\t"
        "lqc2      $vf5, 0x0(%1)    \n\t"
        "vsub.xyzw  $vf4, $vf4, $vf5 \n\t"
        "sqc2      $vf4, 0x0(%0)    \n\t"
        :
        : "r"(lhs.vector), "r"(rhs.vector)
    );
}

Color operator*(const Color& lhs, const Color& rhs)
{
    Color output;
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

void operator*=(Color& lhs, const Color& rhs)
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

Color operator*(const Color& vector, float scalar)
{
    Color output;
    asm volatile(
        "lqc2       $vf4,   0x0(%1)         \n\t"
        "mfc1       $8,     %2              \n\t"
        "qmtc2      $8,     $vf5            \n\t"
        "vmulx.xyzw  $vf6,   $vf4,   $vf5    \n\t"
        "vmove.w    $vf6,   $vf4            \n\t"
        "sqc2       $vf6,   0x0(%0)         \n\t"
        :
        : "r"(output.vector), "r"(vector.vector), "f"(scalar)
    );
    return output;
}

Color operator*(float scalar, const Color& vector)
{
    Color output;
    asm volatile(
        "lqc2       $vf4,   0x0(%1)         \n\t"
        "mfc1       $8,     %2              \n\t"
        "qmtc2      $8,     $vf5            \n\t"
        "vmulx.xyzw  $vf6,   $vf4,   $vf5    \n\t"
        "vmove.w    $vf6,   $vf4            \n\t"
        "sqc2       $vf6,   0x0(%0)         \n\t"
        :
        : "r"(output.vector), "r"(vector.vector), "f"(scalar)
    );
    return output;
}

void operator*=(Color& vector, float scalar)
{
    asm volatile(
        "lqc2       $vf4, 0x0(%0)     \n\t"
        "mfc1       $8,   %1          \n\t"
        "qmtc2      $8,   $vf5        \n\t"
        "vmulx.xyzw  $vf4, $vf4, $vf5  \n\t"
        "sqc2       $vf4, 0x0(%0)     \n\t"
        :
        : "r"(vector.vector), "f"(scalar)
    );
}

Color operator/(const Color& lhs, const Color& rhs)
{
    return {lhs.r / rhs.r, lhs.g / rhs.g, lhs.b / rhs.b, lhs.a / rhs.a};
}

void operator/=(Color& lhs, const Color& rhs)
{
    lhs.r /= rhs.r;
    lhs.g /= rhs.g;
    lhs.b /= rhs.b;
    lhs.a /= rhs.a;
}

Color operator/(const Color& vector, float scalar)
{
    return {vector.r / scalar, vector.g / scalar, vector.b / scalar, vector.a / scalar};
}

Color operator/(float scalar, const Color& vector)
{
    return {vector.r / scalar, vector.g / scalar, vector.b / scalar, vector.a / scalar};
}

void operator/=(Color& vector, float scalar)
{
    vector.r /= scalar;
    vector.g /= scalar;
    vector.b /= scalar;
    vector.a /= scalar;
}

Color operator-(const Color& vector)
{
    return {-vector.r, -vector.g, -vector.b, -vector.a};
}

std::ostream& operator<<(std::ostream& os, const Color& vector)
{
    os << "r=" << vector.r << " g=" << vector.g << " b=" << vector.b << " a=" << vector.a;
    return os;
}

//
//  High Level Functions
//

Plane::Plane() : mDistance(0) {}

Plane::Plane(const Vec4& a, const Vec4& b, const Vec4& c)
{
    Update(a, b, c);
}

void Plane::Update(const Vec4& a, const Vec4& b, const Vec4& c)
{
    this->mNormal = Normalize3(Cross(c - b, a - b));
    this->mDistance = -Dot3(mNormal, b);
}

float Plane::DistanceTo(const Vec4& t_vec) const
{
    return this->mDistance + Dot3(mNormal, t_vec);
}

Frustum::Frustum() {}

Frustum::Frustum(const Mat4& mat) { Set(mat); }

void Frustum::Set(const Mat4& mat)
{
    mPlanes[nX].x = mat.m[0][3] + mat.m[0][0];
    mPlanes[nX].y = mat.m[1][3] + mat.m[1][0];
    mPlanes[nX].z = mat.m[2][3] + mat.m[2][0];
    mPlanes[nX].w = mat.m[3][3] + mat.m[3][0];
    mPlanes[nX] = Normalize(mPlanes[nX]);

    mPlanes[pX].x = mat.m[0][3] - mat.m[0][0];
    mPlanes[pX].y = mat.m[1][3] - mat.m[1][0];
    mPlanes[pX].z = mat.m[2][3] - mat.m[2][0];
    mPlanes[pX].w = mat.m[3][3] - mat.m[3][0];
    mPlanes[pX] = Normalize(mPlanes[pX]);

    mPlanes[nY] = mat.m[0][3] + mat.m[0][1];
    mPlanes[nY] = mat.m[1][3] + mat.m[1][1];
    mPlanes[nY] = mat.m[2][3] + mat.m[2][1];
    mPlanes[nY] = mat.m[3][3] + mat.m[3][1];
    mPlanes[nY] = Normalize(mPlanes[nY]);

    mPlanes[pY] = mat.m[0][3] - mat.m[0][1];
    mPlanes[pY] = mat.m[1][3] - mat.m[1][1];
    mPlanes[pY] = mat.m[2][3] - mat.m[2][1];
    mPlanes[pY] = mat.m[3][3] - mat.m[3][1];
    mPlanes[pY] = Normalize(mPlanes[pY]);

    mPlanes[nZ] = mat.m[0][3] + mat.m[0][2];
    mPlanes[nZ] = mat.m[1][3] + mat.m[1][2];
    mPlanes[nZ] = mat.m[2][3] + mat.m[2][2];
    mPlanes[nZ] = mat.m[3][3] + mat.m[3][2];
    mPlanes[nZ] = Normalize(mPlanes[nZ]);

    mPlanes[pZ] = mat.m[0][3] - mat.m[0][2];
    mPlanes[pZ] = mat.m[1][3] - mat.m[1][2];
    mPlanes[pZ] = mat.m[2][3] - mat.m[2][2];
    mPlanes[pZ] = mat.m[3][3] - mat.m[3][2];
    mPlanes[pZ] = Normalize(mPlanes[pZ]);
}

float Frustum::DistanceTo(const Vec4& min, const Vec4& max, int plane)
{
    return mPlanes[plane].x * (mPlanes[plane].x < 0 ? max.x : min.x) + mPlanes[plane].y * (mPlanes[plane].y < 0 ? max.y : min.y) + mPlanes[plane].z * (mPlanes[plane].z < 0 ? max.z : min.z) + mPlanes[plane].w;
}

bool Frustum::Test(const Vec4& pt)
{
    return  mPlanes[nX].x * pt.x + mPlanes[nX].y * pt.y + mPlanes[nX].z * pt.z + mPlanes[nX].w >= 0 &&
            mPlanes[pX].x * pt.x + mPlanes[pX].y * pt.y + mPlanes[pX].z * pt.z + mPlanes[pX].w >= 0 &&
            mPlanes[nY].x * pt.x + mPlanes[nY].y * pt.y + mPlanes[nY].z * pt.z + mPlanes[nY].w >= 0 &&
            mPlanes[pY].x * pt.x + mPlanes[pY].y * pt.y + mPlanes[pY].z * pt.z + mPlanes[pY].w >= 0 &&
            mPlanes[nZ].x * pt.x + mPlanes[nZ].y * pt.y + mPlanes[nZ].z * pt.z + mPlanes[nZ].w >= 0 &&
            mPlanes[pZ].x * pt.x + mPlanes[pZ].y * pt.y + mPlanes[pZ].z * pt.z + mPlanes[pZ].w >= 0;
}

bool Frustum::Test(const BoundingBox& bb)
{
    return  mPlanes[nX].x * (mPlanes[nX].x < 0 ? bb.mMin.x : bb.mMax.x) + mPlanes[nX].y * (mPlanes[nX].y < 0 ? bb.mMin.y : bb.mMax.y) + mPlanes[nX].z * (mPlanes[nX].z < 0 ? bb.mMin.z : bb.mMax.z) >= -mPlanes[nX].w &&
            mPlanes[pX].x * (mPlanes[pX].x < 0 ? bb.mMin.x : bb.mMax.x) + mPlanes[pX].y * (mPlanes[pX].y < 0 ? bb.mMin.y : bb.mMax.y) + mPlanes[pX].z * (mPlanes[pX].z < 0 ? bb.mMin.z : bb.mMax.z) >= -mPlanes[pX].w &&
            mPlanes[nY].x * (mPlanes[nY].x < 0 ? bb.mMin.x : bb.mMax.x) + mPlanes[nY].y * (mPlanes[nY].y < 0 ? bb.mMin.y : bb.mMax.y) + mPlanes[nY].z * (mPlanes[nY].z < 0 ? bb.mMin.z : bb.mMax.z) >= -mPlanes[nY].w &&
            mPlanes[pY].x * (mPlanes[pY].x < 0 ? bb.mMin.x : bb.mMax.x) + mPlanes[pY].y * (mPlanes[pY].y < 0 ? bb.mMin.y : bb.mMax.y) + mPlanes[pY].z * (mPlanes[pY].z < 0 ? bb.mMin.z : bb.mMax.z) >= -mPlanes[pY].w &&
            mPlanes[nZ].x * (mPlanes[nZ].x < 0 ? bb.mMin.x : bb.mMax.x) + mPlanes[nZ].y * (mPlanes[nZ].y < 0 ? bb.mMin.y : bb.mMax.y) + mPlanes[nZ].z * (mPlanes[nZ].z < 0 ? bb.mMin.z : bb.mMax.z) >= -mPlanes[nZ].w &&
            mPlanes[pZ].x * (mPlanes[pZ].x < 0 ? bb.mMin.x : bb.mMax.x) + mPlanes[pZ].y * (mPlanes[pZ].y < 0 ? bb.mMin.y : bb.mMax.y) + mPlanes[pZ].z * (mPlanes[pZ].z < 0 ? bb.mMin.z : bb.mMax.z) >= -mPlanes[pZ].w;
}

bool Frustum::Test(const Sphere& sp)
{
    return  mPlanes[nX].x * sp.mPosition.x + mPlanes[nX].y * sp.mPosition.y + mPlanes[nX].z * sp.mPosition.z + mPlanes[nX].w >= -sp.mRadius &&
            mPlanes[pX].x * sp.mPosition.x + mPlanes[pX].y * sp.mPosition.y + mPlanes[pX].z * sp.mPosition.z + mPlanes[pX].w >= -sp.mRadius &&
            mPlanes[nY].x * sp.mPosition.x + mPlanes[nY].y * sp.mPosition.y + mPlanes[nY].z * sp.mPosition.z + mPlanes[nY].w >= -sp.mRadius &&
            mPlanes[pY].x * sp.mPosition.x + mPlanes[pY].y * sp.mPosition.y + mPlanes[pY].z * sp.mPosition.z + mPlanes[pY].w >= -sp.mRadius &&
            mPlanes[nZ].x * sp.mPosition.x + mPlanes[nZ].y * sp.mPosition.y + mPlanes[nZ].z * sp.mPosition.z + mPlanes[nZ].w >= -sp.mRadius &&
            mPlanes[pZ].x * sp.mPosition.x + mPlanes[pZ].y * sp.mPosition.y + mPlanes[pZ].z * sp.mPosition.z + mPlanes[pZ].w >= -sp.mRadius;
}

Mat4 Transform::GetTransformationMatrix()
{
    return Transformation(mPosition, mRotation, mScale);
}

Mat4 Camera::GetViewMatrix()
{
    return LookAt(mPosition, mLookAt, mUp);
}

Vec4 Camera::GetTarget()
{
    return Normalize3(mLookAt - mPosition);
}

}