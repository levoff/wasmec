#define PI 3.14159265358979323846
#define RAD 6.283185307179586
#define COEFF_1 0.7853981633974483
#define COEFF_2 2.356194490192345
#define BLADES 3
#define CYCLE_WIDTH 100
#define BLADES_T_CYCLE_WIDTH 300


int height;
int width;
int pixelCount;
int ch;
int cw;
double maxDistance;
/*
We'll cheat a bit and just allocate loads of memory
so we don't have to implement malloc
*/
int data[2000000];

// Just declare functions signatures.
// These functions will be provided by JS when wasm is loaded into the browser.
double atan2(double, double);
double round(double);
double sqrt(double);
double floor(double);
double fabs(double);
int abs(int);

int*  init(int cWidth, int cHeight) {
  width = cWidth;
  height = cHeight;
  pixelCount = width * height;
  ch = height >> 1;
  cw = width >> 1;
  maxDistance = sqrt(ch * ch + cw * cw);
  // data = malloc(pixelCount * sizeof(int));
  return &data[0];
}


// Using the 'native' fmod would require us to provide the module with asm2wasm...
double customFmod(double a, double b)
{
  return (a - b * floor(a / b));
}

void  render(double timestamp) {
  int scaledTimestamp = floor(timestamp / 10.0 + 2000.0);
  for (int y = 0; y < height; y++) {
    int dy = ch - y;
    int dysq = dy * dy;
    int yw = y * width;
    for (int x = 0; x < width; x++) {
      int dx = cw - x;
      int dxsq = dx * dx;
      double angle = atan2(dx, dy) / RAD;
      // Arbitrary mangle of the distance, just something that looks pleasant
      int asbs = dxsq + dysq;
      double distanceFromCenter = sqrt(asbs);
      double scaledDistance = asbs / 400.0 + distanceFromCenter;
      double lerp = 1.0 - (customFmod(fabs(scaledDistance - scaledTimestamp + angle * BLADES_T_CYCLE_WIDTH), CYCLE_WIDTH)) / CYCLE_WIDTH;
      // Fade R more slowly
      double absoluteDistanceRatioGB = 1.0 - distanceFromCenter / maxDistance;
      double absoluteDistanceRatioR = absoluteDistanceRatioGB * 0.8 + 0.2;
      int fadeB = round(50.0 * lerp * absoluteDistanceRatioGB);
      int fadeR = round(240.0 * lerp * absoluteDistanceRatioR * (1.0 + lerp) / 2.0);
      int fadeG = round(120.0 * lerp * lerp * lerp * absoluteDistanceRatioGB);
      data[yw + x] =
        (255 << 24) |   // A
        (fadeB << 16) | // B
        (fadeG << 8) |  // G
        fadeR;          // R
    }
  }
}
