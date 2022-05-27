//
// Created by gleb on 5/20/20.
//

#ifndef CW_2_2_CW_TASKS_H
#define CW_2_2_CW_TASKS_H

#include "png_work.h"
#include "picture_work.h"
#include <math.h>

typedef struct Rectangle
{
    int x_start;
    int y_start;
    int x_end;
    int y_end;
} RECTANGLE;

typedef struct Mask
{
    int height;
    int width;
    bool **data;
} MASK;


void print_info(PNG *image);
void replace_color(PNG *image, const png_byte *source, const png_byte *dest);
void draw_rectangle_inner_border(PNG *image, RECTANGLE *rect, const png_byte *color, int size);
MASK get_mask(PNG *image, const png_byte *color);
void free_mask(MASK *mask);
RECTANGLE find_rectangle(MASK *mask, int x, int y);
void find_rectangles(PNG *image, const png_byte *color, const png_byte *color1, int border_size);
void draw_pattern(PNG *image, const int *pattern, int pattern_h, int pattern_w);
bool is_border(PNG *image, int x, int y, int border_size);
void draw_border_pattern(PNG *image, const int *pattern, int pattern_h, int pattern_w, int border_size);
void draw_border_gradient(PNG *image, int border_size);
int max(int a, int b);
void draw_border_fractal(PNG *image, int border_size);
#endif //CW_2_2_CW_TASKS_H
