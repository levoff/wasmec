
#ifndef WASMATH_C_
#define WASMATH_C_

#define LLONG_MIN  -9223372036854775807ll
#define LLONG_MAX   9223372036854775807ll

#define PI 3.14159265358979323846
#define RAD 6.283185307179586
#define COEFF_1 0.7853981633974483
#define COEFF_2 2.356194490192345
#define BLADES 3
#define CYCLE_WIDTH 100
#define BLADES_T_CYCLE_WIDTH 300

unsigned long isqrt32_sy(unsigned long h) {
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

unsigned isqrt16(unsigned Value) {
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

unsigned char isqrt8(unsigned char Value) {
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

unsigned int sqrt_wasm(unsigned int a_nInput) {
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

int abs_wasm(int x) {
    return x < 0 ? -x : x;
}

double fabs_wasm(double x) {
    return x < 0 ? -x : x;
}

double floor_wasm(double num) {
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

int round_wasm(double x) {
    if (x < 0.0)
        return (int)(x - 0.5);
    else
        return (int)(x + 0.5);
}

double atan2_wasm(int y, int x) {
  double abs_y = abs_wasm(y) + 1e-10;
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

double fmod_wasm(double a, double b)
{
  // Using the 'native' fmod would require us to provide the module with asm2wasm...
  return (a - b * floor_wasm(a / b));
}

#endif //WASMATH_C_
