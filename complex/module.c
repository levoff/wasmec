/*
 This is a demo code to show the C and JS interaction trough shared memory buffer
 Custom math functions implemented in wasmath.c : round, floor, sqrt, abs, fabs
 No standard libraries used to reduce the generated wasm code size
 Levon Hovhannisyan 2022, levyan64@gmail.com
 */

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #define EXPORT EMSCRIPTEN_KEEPALIVE
#else
    #define EXPORT 
#endif

#define OLIVEC_IMPLEMENTATION
#include "olive.c"
#include "wasmath.c"
#include "wastring.c"

#define BLADES 3
#define CYCLE_WIDTH 100
#define BLADES_T_CYCLE_WIDTH 300

int height;
int width;
int pixelCount;
int ch;
int cw;
int x_old, y_old;
double maxDistance;
unsigned int data[2000000]; // Use static memory to avoid malloc
Olivec_Canvas oc;


unsigned int* EXPORT init(int cWidth, int cHeight) {
  width = cWidth;
  height = cHeight;
  pixelCount = width * height;
  x_old = -1;
  y_old = -1;
  ch = height >> 1;
  cw = width >> 1;
  maxDistance = wasmath_sqrt32(ch * ch + cw * cw);

  oc = olivec_canvas(data, width, height, width);
  olivec_fill(oc, 0xFF131313);
  olivec_text(oc, "hello, this is a wasm demo.", 20, height/2-50, olivec_default_font, 2, 0xFFfff1f1);
  olivec_text(oc, "please select an example to play with.", 20, height/2-20, olivec_default_font, 2, 0xFFfff1f1);

  return &data[0];
}

void example_draw_curve(int ts, int x_new, int y_new) {
  
  if (x_new == x_old && y_new == y_old)
    return;

  if (x_new == -1 && y_new == -1) {
    x_old = -1;
    y_old = -1;
    return;
  }

  if (x_old == -1 && y_old == -1) {
    x_old = x_new;
    y_old = y_new;
  }

  // Line
  olivec_circle(oc, x_new, y_new, 2, 0xFF20a0ff);
  olivec_line(oc, x_new, y_new, x_old, y_old, 0xFF1fff00);
  x_old = x_new;
  y_old = y_new;

  // Text-coordinates
  olivec_rect(oc, 8, 8, 100, 16, 0xFFfff1f1);
  char x_center_str[32] = {0};
  char y_center_str[32] = {0};
  intostr(x_new, x_center_str);
  intostr(y_new, y_center_str);
  strcat3(x_center_str, ',', y_center_str);
  olivec_text(oc, x_center_str, 10, 10, olivec_default_font, 2, 0xFF000000);
}

void example_draggable_ball(int x_new, int y_new) {
  if (x_new == -1 && y_new == -1) {
    x_old = -1;
    y_old = -1;
    return;
  }
  // Circle
  uint16_t r = 50;
  olivec_fill(oc, 0xFFfff1f1);
  olivec_circle(oc, x_new, y_new, r, 0xFF20a0ff);
  olivec_circle(oc, x_new+10, y_new+10, r, 0x0F20a0ff);

  // Text-coordinates
  char x_center_str[32] = {0};
  char y_center_str[32] = {0};
  intostr(x_new, x_center_str);
  intostr(y_new, y_center_str);
  strcat3(x_center_str, ',', y_center_str);
  olivec_text(oc, x_center_str, 10, 10, olivec_default_font, 2, 0xFF000000);
}

void example_spiral(int timestamp) {
    for (int y = 0; y < height; y++) {
    int dy = ch - y;
    int dysq = dy * dy;
    int yw = y * width;
    for (int x = 0; x < width; x++) {
      int dx = cw - x;
      int dxsq = dx * dx;
      double angle = wasmath_atan2(dx, dy) / RAD;
      // Arbitrary mangle of the distance, just something that looks pleasant
      int asbs = dxsq + dysq;
      double distanceFromCenter = wasmath_isqrt16(asbs);//sqrt_wasm(asbs);//(double)isqrt32_sy(asbs)/65536;
      double scaledDistance = asbs / 400.0 + distanceFromCenter;
      double lerp = 1.0 - (wasmath_fmod(wasmath_fabs(scaledDistance - timestamp + angle * BLADES_T_CYCLE_WIDTH), CYCLE_WIDTH)) / CYCLE_WIDTH;
      // Fade R more slowly
      double absoluteDistanceRatioGB = 1.0 - distanceFromCenter / maxDistance;
      double absoluteDistanceRatioR = absoluteDistanceRatioGB * 0.8 + 0.2;
      int fadeB = wasmath_round(50.0 * lerp * absoluteDistanceRatioGB);
      int fadeR = wasmath_round(240.0 * lerp * absoluteDistanceRatioR * (1.0 + lerp) / 2.0);
      int fadeG = wasmath_round(120.0 * lerp * lerp * lerp * absoluteDistanceRatioGB);
      data[yw + x] =
        (255 << 24) |   // A
        (fadeB << 16) | // B
        (fadeG << 8) |  // G
        fadeR;          // R
    }
  }

  // Text-coordinates
  olivec_rect(oc, 8, 8, 100, 16, 0xFFfff1f1);
  char str[32] = {0};
  intostr(timestamp, str);
  olivec_text(oc, str, 10, 10, olivec_default_font, 2, 0xFF000000);
}


void EXPORT render(double timestamp, int x_new, int y_new, int opcode) {
  switch (opcode) {
    case 1:
      olivec_fill(oc, 0xFF131313);
      break;
    case 2:
      olivec_fill(oc, 0xFF831746);
      break;
    case 3:
      example_draw_curve(timestamp/1000, x_new, y_new); 
      break;
    case 4:
      example_draggable_ball(x_new, y_new);
      break;
    case 5:
      example_spiral(wasmath_floor(timestamp/10.0 + 2000.0));
      break;
    default: 
      break;
  }
}

