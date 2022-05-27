//
// Created by gleb on 5/20/20.
//

#ifndef CW_2_2_PNG_WORK_H
#define CW_2_2_PNG_WORK_H

#include <stdlib.h>
#include <png.h>
#include <stdbool.h>

typedef struct Png
{
    int width, height;
    png_byte color_type;
    png_byte bit_depth;

    int channels;

    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep *row_pointers;
    png_byte *row_data;
} PNG;

bool read_png_file(char *, PNG *);
bool write_png_file(char *, PNG * );

void free_png(PNG * );


#endif //CW_2_2_PNG_WORK_H
