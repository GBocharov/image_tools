//
// Created by gleb on 5/20/20.
//

#ifndef CW_2_2_PICTURE_WORK_H
#define CW_2_2_PICTURE_WORK_H

#include <png.h>
#include <stdbool.h>
#include "png_work.h"

bool point_in_image(PNG * image, int x, int y);


void set_pixel(PNG * image, const int , const int , const png_byte *);
png_byte *get_pixel(PNG *image, const int x, const int y);
void resize_image(PNG *image, int new_h, int new_w);
void resize_image_for_draw_border(PNG *image, int border_size);
bool is_eq_color(const png_byte *source, const png_byte *dest, int channels);

#endif //CW_2_2_PICTURE_WORK_H
