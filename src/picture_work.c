//
// Created by gleb on 5/20/20.
//

#include "picture_work.h"


bool point_in_image(PNG *image, int x, int y)
{
    return !((x < 0) || (x >= image->height) || (y < 0) || (y >= image->width));
}


void set_pixel(PNG *image, const int x, const int y, const png_byte *color)
{
    if (!point_in_image(image, x, y))
    {
        puts("Some error handling: x or y out of range");
        return;
    }
    int pixel_channels = image->channels;
    png_byte *pixel = (void *) (image->row_pointers[x] + y * pixel_channels);
    for (int i = 0; i < pixel_channels; i++)
        pixel[i] = color[i];

}


png_byte *get_pixel(PNG *image, const int x, const int y)
{
    if (!point_in_image(image, x, y))
    {
        puts("x or y out of range");
        return NULL;
    }
    int pixel_channels = image->channels;
    return (image->row_pointers[x] + y * pixel_channels);
}


void resize_image(PNG *image, int new_h, int new_w)
{
    if ((new_h <= 0) || (new_w <= 0))
    {
        puts("Some error handling: new_h <= 0 or new_w <= 0");
        return;
    }
    PNG img;
    img.height = new_h;
    img.width = new_w;
    img.bit_depth = image->bit_depth;
    img.channels = image->channels;
    img.color_type = image->color_type;
    img.info_ptr = image->info_ptr;

    img.row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * img.height);
    int row_byte_size = img.width * img.channels * image->bit_depth / 8;
    img.row_data = (png_byte *) malloc(img.height * row_byte_size);

    for (int i = 0; i < img.height; i++)
        img.row_pointers[i] = (png_byte *) (img.row_data + i * row_byte_size);

    for (int i = 0; (i < image->height) && (i < img.height); i++)
        for (int j = 0; (j < image->width) && (j < img.width); j++)
        {
            set_pixel(&img, i, j, get_pixel(image, i, j));
        }
    free_png(image);

    png_set_rows(img.png_ptr, img.info_ptr, img.row_pointers);
    *image = img;
}


void resize_image_for_draw_border(PNG *image, int border_size)
{
    PNG img;
    img.height = image->height + 2 * border_size;
    img.width = image->width + 2 * border_size;
    img.bit_depth = image->bit_depth;
    img.channels = image->channels;
    img.color_type = image->color_type;
    img.info_ptr = image->info_ptr;


    img.row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * img.height);
    int row_byte_size = img.width * img.channels * image->bit_depth / 8;
    img.row_data = (png_byte *) malloc(img.height * row_byte_size);


    for (int i = 0; i < img.height; i++)
        img.row_pointers[i] = (png_byte *) (img.row_data + i * row_byte_size);




    for (int i = border_size; i < img.height - border_size; i++)
        for (int j = border_size; j < img.width - border_size; j++)
        {
            set_pixel(&img, i, j, get_pixel(image, i - border_size, j - border_size));
        }

    free_png(image);

    png_set_rows(img.png_ptr, img.info_ptr, img.row_pointers);
    *image = img;
}

bool is_eq_color(const png_byte *source, const png_byte *dest, int channels)
{
    for (int i = 0; i < channels; i++)
        if (source[i] != dest[i])
            return false;
    return true;
}
