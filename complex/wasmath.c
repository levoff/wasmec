
#ifndef WASMATH_C_
#define WASMATH_C_

#define LLONG_MIN  -9223372036854775807ll
#define LLONG_MAX   9223372036854775807ll

#define PI 3.14159265358979323846
#define RAD 6.283185307179586
#define COEFF_1 0.7853981633974483
#define COEFF_2 2.356194490192345

#include <stdint.h>

int16_t fast_sqrt_chatGPT_fixed(int32_t number) {
    int32_t i;
    int32_t x2, y;
    const int32_t threehalfs = (int32_t)(1.5 * (1 << 15));

    x2 = number >> 1;
    y = number;
    i = *(int32_t*) &y;
    i = 0x5f3759df - (i >> 1);
    y = *(int32_t*) &i;
    y = ((threehalfs - (x2 * y)) >> 15) * y;
    return (int16_t)(1.0 / y * (1 << 15));
}


float fast_sqrt_chatGPT_float(float number) {
    long i;
    float x2, y;
    const float threehalfs = 1.5f;

    x2 = number * 0.5f;
    y = number;
    i = *(long*) &y;
    i = 0x5f3759df - (i >> 1);
    y = *(float*) &i;
    y = y * (threehalfs - (x2 * y * y));
    return 1 / y;
}


unsigned long wasmath_isqrt16f16(unsigned long h) {
  // From SY
  unsigned long x;
  unsigned long y;
  unsigned char i;

  x = y = 0;
  for (i = 0; i < 32; i++) {
    x = (x << 1) | 1;
    if (y < x)
      x -= 2;
    else
      y -= x;
    x++;
    y <<= 1;
    if ((h & 0x80000000))
      y |= 1;
    h <<= 1;
    y <<= 1;
    if ((h & 0x80000000))
      y |= 1;
    h <<= 1;
  }
  // 16 MSB are integer part and 16 LSB are fractional part
  // float sqrt = (float)x(asbs)/65536
  return x;
}

unsigned wasmath_isqrt16(unsigned Value) {
  unsigned Root = 0;
  unsigned Bit;
  for (Bit = 0x4000; Bit > 0; Bit >>= 2) {
    unsigned Trial = Root + Bit;
    Root >>= 1;
    if (Trial <= Value) {
      Root += Bit;
      Value -= Trial;
    }
  }
  return Root;
}

unsigned char wasmath_isqrt8(unsigned char Value) {
  unsigned char Root = 0;
  unsigned char Bit;
  for (Bit = 0x40; Bit > 0; Bit >>= 2) {
    unsigned char Trial = Root + Bit;
    Root >>= 1;
    if (Trial <= Value) {
      Root += Bit;
      Value -= Trial;
    }
  }
  return Root;
}

unsigned int wasmath_sqrt32(unsigned int a_nInput) {
    unsigned int op  = a_nInput;
    unsigned int res = 0;
    unsigned int one = 1uL << 30; // The second-to-top bit is set: use 1u << 14 for uint16_t type; use 1uL<<30 for uint32_t type

    // "one" starts at the highest power of four <= than the argument.
    while (one > op){
     one >>= 2;
    }

    while (one != 0){
     if (op >= res + one){
       op = op - (res + one);
       res = res +  2 * one;
     }
     res >>= 1;
     one >>= 2;
    }
    return res;
}

int wasmath_abs(int x) {
    return x < 0 ? -x : x;
}

double wasmath_fabs(double x) {
    return x < 0 ? -x : x;
}

double wasmath_floor(double num) {
    if (num >= (double)(LLONG_MAX) || num <= (double)LLONG_MIN || num != num) {
        /* handle large values, infinities and nan */
        return num;
    }
    long long n = (long long)num;
    double d = (double)n;
    if (d == num || num >= 0)
        return d;
    else
        return d - 1;
}

int wasmath_round(double x) {
    if (x < 0.0)
        return (int)(x - 0.5);
    else
        return (int)(x + 0.5);
}

double wasmath_atan2(int y, int x) {
  double abs_y = wasmath_abs(y) + 1e-10;
  double angle;
  if (x >= 0) {
    double r = (x - abs_y) / (x + abs_y);
    angle = 0.1963 * r * r * r - 0.9817 * r + COEFF_1;
  } else {
    double r = (x + abs_y) / (abs_y - x);
    angle = 0.1963 * r * r * r - 0.9817 * r + COEFF_2;
  }
  return y < 0 ? -angle : angle;
}


float wasmath_fast_atan_float(float x) {
    // Constants
    const float a0 = 1.6867629106f;
    const float a1 = -0.4378497304f;
    const float a2 = 0.0524089524f;
    const float a3 = -0.0025315845f;

    float x2 = x*x;
    float result = (a3*x2 + a2)*x2 + a1*x + a0;
    result *= x;
    return result;
}


double wasmath_fmod(double a, double b)
{
  // Using the 'native' fmod would require us to provide the module with asm2wasm...
  return (a - b * wasmath_floor(a / b));
}

#endif //WASMATH_C_
