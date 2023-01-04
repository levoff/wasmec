#include <_types/_uint16_t.h>
#define OLIVEC_IMPLEMENTATION
#include "olive.c"
#include <errno.h>
#include <stdio.h>
#include <math.h>


#define WIDTH 500
#define HEIGHT 500
#define return_defer(value) do { result = (value); goto defer; } while (0)

uint32_t pixels[WIDTH*HEIGHT];


int olivec_save_to_ppm_file (uint32_t *pixels, size_t width, size_t height, const char *file_path)
{  
    int result = 0;
    FILE *f = NULL;
    
    {
        f = fopen (file_path, "wb");
        if (f == NULL) return_defer(errno) ;

        fprintf(f, "P6\n%zu %zu 255\n", width, height);
        if (ferror(f)) return_defer (errno);

        for (size_t i = 0; i < width*height; ++i) {
            uint32_t pixel = pixels[i];
            uint8_t bytes[3] = {
                (pixel>> (8*0)) &0xFF,
                (pixel>> (8*1)) &0xFF,
                (pixel>> (8*2)) &0xFF,
            };
            fwrite(bytes, sizeof (bytes), 1, f);
            if (ferror (f)) return_defer (errno);
        }
    }

defer:
    if(f) fclose(f);
    return result;
}

typedef struct {
    int x;
    int y;
} Point_t;


void rotate_point(Point_t *pt, Point_t rot_center, float rot_angle_rad)
{
    // [1] Translate
    int x1 = pt->x - rot_center.x;
    int y1 = pt->y - rot_center.y;
    // [2] Rotate and Back-translate
    pt->x = round(x1*cos(rot_angle_rad) - y1*sin(rot_angle_rad) + rot_center.x);
    pt->y = round(x1*sin(rot_angle_rad) + y1*cos(rot_angle_rad) + rot_center.y);
}

float distance(Point_t pt1, Point_t pt2) {
    return sqrtf(pow(pt2.x - pt1.x,2) + pow(pt2.y - pt1.y, 2));
}

int main(void)
{
    Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT, WIDTH);
    olivec_fill(oc, 0xFFFFFFFF);

    uint16_t r = 200;

    olivec_circle(oc, WIDTH/2, HEIGHT/2, r, 0xFF444444);
    olivec_line(oc, WIDTH/2, HEIGHT/2, WIDTH/2.0 + (r-20)*cos(1), HEIGHT/2.0 + (r-20)*sin(1), 0xFFffff00);
    olivec_line(oc, WIDTH/2, HEIGHT/2, WIDTH/2.0 + (r-50)*cos(3.6), HEIGHT/2 + (r-50)*sin(3.6), 0xFFffff00);

    float angle = -1.5708;
    uint8_t radii = 12;

    for (int i = 0; i < radii; ++i)
        {
            olivec_line(oc, 
                        WIDTH/2.0 + (r-30)*cos(angle), HEIGHT/2.0 + (r-30)*sin(angle), 
                        WIDTH/2.0 + r*cos(angle), HEIGHT/2.0 + r*sin(angle),
                        0xFFfff1f1);
            angle += 6.283/radii;
        }

    radii = 60;
    for (int i = 0; i < radii; ++i)
        {
            olivec_line(oc, 
                        WIDTH/2.0 + (r-10)*cos(angle), HEIGHT/2.0 + (r-10)*sin(angle), 
                        WIDTH/2.0 + r*cos(angle), HEIGHT/2.0 + r*sin(angle),
                        0xFFfff1f1);
            angle += 6.283/radii;
        }

    //olivec_text(oc, "21:25", WIDTH/2-70, HEIGHT/2-15, olivec_default_font, 5, 0xFFFFFFFF);
    olivec_circle(oc, WIDTH/2, HEIGHT/2, 3, 0xFF000000);

    // Rotation experiments
    Point_t center = {WIDTH/2, HEIGHT/2};
    Point_t start = { .x = 250, .y = 250 };
    Point_t point = start;
    Point_t rot_center = { WIDTH/2, HEIGHT/4 + (HEIGHT/2-r)/2 };
    
    for (float theta = 0; theta < M_PI/2; theta+=0.1) {

        rotate_point(&rot_center, center, 0.1);
        olivec_circle(oc, rot_center.x, rot_center.y, 1, 0xFFff0000);
    }


    olivec_circle(oc, rot_center.x, rot_center.y, 3, 0xFFff0000);
    olivec_circle(oc, point.x, point.y , 3, 0xFF0000ff);
    olivec_circle(oc, rot_center.x, rot_center.y, 70, 0x1Fff0000);

    printf("Radius = %f\n", distance(point, rot_center));

    for (float theta = 0; theta < 2*M_PI; theta+=0.1) {

        rotate_point(&point, rot_center, -theta);
        olivec_circle(oc, point.x, point.y, 1, 0xFF00f0f0);
        point = start;
    }

    olivec_save_to_ppm_file(pixels, WIDTH, HEIGHT, "test.ppm");
    return 0;
}
