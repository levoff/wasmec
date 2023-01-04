/*
 This is a demo code to show the C and JS interaction trough shared memory buffer
 Custom math functions implemented: round, floor, sqrt, abs, fabs
 No standard libraries used to reduce the generated wasm code size
 Levon Hovhannisyan 2022, levyan64@gmail.com
 */

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #define EXPORT EMSCRIPTEN_KEEPALIVE
#else
    #define EXPORT 
#endif

#define LLONG_MIN  -9223372036854775807
#define LLONG_MAX   9223372036854775807

#define PI 3.14159265358979323846
#define RAD 6.283185307179586
#define COEFF_1 0.7853981633974483
#define COEFF_2 2.356194490192345
#define BLADES 3
#define CYCLE_WIDTH 100
#define BLADES_T_CYCLE_WIDTH 300

// #define double float //slows down generated wasm

int height;
int width;
int pixelCount;
int ch;
int cw;
double maxDistance;
int data[2000000]; // Use static memory to avoid malloc



// Custom functions: round, floor, fabs, abs, sqrt,

unsigned int sqrt_wasm(unsigned int a_nInput) {
  unsigned int op = a_nInput;
  unsigned int res = 0;
  unsigned int one = 1uL << 30; // The second-to-top bit is set: use 1u << 14 for uint16_t
                 // type; use 1uL<<30 for uint32_t type

  // "one" starts at the highest power of four <= than the argument.
  while (one > op) {
    one >>= 2;
  }

  while (one != 0) {
    if (op >= res + one) {
      op = op - (res + one);
      res = res + 2 * one;
    }
    res >>= 1;
    one >>= 2;
  }
  return res;
}

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





int abs_wasm(int x) {
    return x < 0 ? -x : x;
}

double fabs_wasm(double x) {
    return x < 0 ? -x : x;
}

double floor_wasm(double num) {
    if (num >= LLONG_MAX || num <= LLONG_MIN || num != num) {
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

double customAtan2(int y, int x) {
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

double customFmod(double a, double b)
{
  // Using the 'native' fmod would require us to provide the module with asm2wasm...
  return (a - b * floor_wasm(a / b));
}



int* EXPORT init(int cWidth, int cHeight) {
  width = cWidth;
  height = cHeight;
  pixelCount = width * height;
  ch = height >> 1;
  cw = width >> 1;
  maxDistance = sqrt_wasm(ch * ch + cw * cw);
  // data = malloc(pixelCount * sizeof(int));
  return &data[0];
}



void EXPORT render(double timestamp) {
  int scaledTimestamp = floor_wasm(timestamp / 10.0 + 2000.0);
  for (int y = 0; y < height; y++) {
    int dy = ch - y;
    int dysq = dy * dy;
    int yw = y * width;
    for (int x = 0; x < width; x++) {
      int dx = cw - x;
      int dxsq = dx * dx;
      double angle = customAtan2(dx, dy) / RAD;
      // Arbitrary mangle of the distance, just something that looks pleasant
      int asbs = dxsq + dysq;
      double distanceFromCenter = isqrt16(asbs);//sqrt_wasm(asbs);//(double)isqrt32_sy(asbs)/65536;
      double scaledDistance = asbs / 400.0 + distanceFromCenter;
      double lerp = 1.0 - (customFmod(fabs_wasm(scaledDistance - scaledTimestamp + angle * BLADES_T_CYCLE_WIDTH), CYCLE_WIDTH)) / CYCLE_WIDTH;
      // Fade R more slowly
      double absoluteDistanceRatioGB = 1.0 - distanceFromCenter / maxDistance;
      double absoluteDistanceRatioR = absoluteDistanceRatioGB * 0.8 + 0.2;
      int fadeB = round_wasm(50.0 * lerp * absoluteDistanceRatioGB);
      int fadeR = round_wasm(240.0 * lerp * absoluteDistanceRatioR * (1.0 + lerp) / 2.0);
      int fadeG = round_wasm(120.0 * lerp * lerp * lerp * absoluteDistanceRatioGB);
      data[yw + x] =
        (255 << 24) |   // A
        (fadeB << 16) | // B
        (fadeG << 8) |  // G
        fadeR;          // R
    }
  }
}

