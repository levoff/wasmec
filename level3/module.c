/*
 This is a demo code to show the C and JS interaction trough shared memory buffer
 Custom math functions implemented in wasmath.c : round, floor, sqrt, abs, fabs
 No standard libraries used to reduce the generated wasm code size
 Levon Hovhannisyan 2022, levyan64@gmail.com
 */
#define OLIVEC_IMPLEMENTATION
#include "olive.c"
#include "wasmath.c"
#include "wastring.c"

int height;
int width;
int pixelCount;
int ch;
int cw;
double maxDistance;
unsigned int data[2000000]; // Use static memory to avoid malloc

int x_old, y_old;

Olivec_Canvas oc;

unsigned int* init(int cWidth, int cHeight) {
  width = cWidth;
  height = cHeight;
  x_old = -1;
  y_old = -1;
  oc = olivec_canvas(data, width, height, width);
  olivec_fill(oc, 0xFF131313);
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
  char str[32] = {0};
  intostr(ts, str);
  olivec_text(oc, str, 10, 10, olivec_default_font, 2, 0xFF000000);
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
  olivec_text(oc, x_center_str, 20, 20, olivec_default_font, 2, 0xFF000000);
}


void render(double timestamp, int x_new, int y_new, int opcode) {

  int scaledtimestamp = timestamp;//floor_wasm(timestamp / 10.0 + 2000.0);


  switch (opcode) {
    case 1:
      olivec_fill(oc, 0xFF131313);
      break;
    case 2:
      olivec_fill(oc, 0xFF831746);
      break;
    case 3:
      //f3();
      break;
    default: 
      break;
  }

  example_draw_curve(scaledtimestamp, x_new, y_new);  
  // example_draggable_ball(x_new, y_new);  

}

