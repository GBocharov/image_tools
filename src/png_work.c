//
// Created by gleb on 5/20/20.
//

#include "png_work.h"

bool read_png_file(char *file_name, PNG *image)
{

    char header[8];

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp)
    {
        puts("Some error handling: file could not be opened");
        return false;
    }

    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
    {
        puts("Some error handling: file is not recognized as a PNG");
        fclose(fp);
        return false;
    }

    /* initialize stuff */
    image->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!image->png_ptr)
    {
        puts("Some error handling: png_create_read_struct failed");
        fclose(fp);
        png_destroy_read_struct(&image->png_ptr, NULL, NULL);
        return false;
    }

    image->info_ptr = png_create_info_struct(image->png_ptr);
    if (!image->info_ptr)
    {
        puts("Some error handling : png_create_info_struct failed");
        fclose(fp);
        png_destroy_read_struct(&image->png_ptr, &image->info_ptr, NULL);
        return false;
    }

    if (setjmp(png_jmpbuf(image->png_ptr)))
    {
        puts("Some error handling : error during init_io");
        fclose(fp);
        png_destroy_read_struct(&image->png_ptr, &image->info_ptr, NULL);
        return false;
    }

    png_init_io(image->png_ptr, fp);
    png_set_sig_bytes(image->png_ptr, 8);

    png_read_info(image->png_ptr, image->info_ptr);

    image->width = png_get_image_width(image->png_ptr, image->info_ptr);
    image->height = png_get_image_height(image->png_ptr, image->info_ptr);
    image->color_type = png_get_color_type(image->png_ptr, image->info_ptr);
    image->bit_depth = png_get_bit_depth(image->png_ptr, image->info_ptr);

    image->channels = png_get_channels(image->png_ptr, image->info_ptr);

    image->number_of_passes = png_set_interlace_handling(image->png_ptr);
    png_read_update_info(image->png_ptr, image->info_ptr);

    /* read file */
    if (setjmp(png_jmpbuf(image->png_ptr)))
    {
        puts("Some error handling : error during read image");
        fclose(fp);
        png_destroy_read_struct(&image->png_ptr, &image->info_ptr, NULL);
        return false;
    }

    image->row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * image->height);
    int row_byte_size = png_get_rowbytes(image->png_ptr, image->info_ptr);
    image->row_data = (png_byte *) malloc(image->height * row_byte_size);

    for (int i = 0; i < image->height; i++)
    {
        image->row_pointers[i] = (png_byte *) (image->row_data + i * row_byte_size);
    }

    png_read_image(image->png_ptr, image->row_pointers);

    fclose(fp);
    return true;

}


bool write_png_file(char *file_name, PNG *image)
{
    FILE *fp = fopen(file_name, "wb");
    if (!fp)
    {
        puts("Some error handling: file could not be opened");
        return false;
    }

    /* initialize stuff */
    image->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!image->png_ptr)
    {
        puts("Some error handling: png_create_write_struct failed");
        fclose(fp);
        return false;
    }

    image->info_ptr = png_create_info_struct(image->png_ptr);
    if (!image->info_ptr)
    {
        puts("Some error handling: png_create_info_struct failed");
        fclose(fp);
        return false;
    }

    if (setjmp(png_jmpbuf(image->png_ptr)))
    {
        puts("Some error handling: error during init_io");
        fclose(fp);
        return false;
    }

    png_init_io(image->png_ptr, fp);

    /* write header */
    if (setjmp(png_jmpbuf(image->png_ptr)))
    {

        puts("Some error handling: error during writing header");
        fclose(fp);
        return false;
    }

    png_set_IHDR(image->png_ptr, image->info_ptr, image->width, image->height,
                 image->bit_depth, image->color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(image->png_ptr, image->info_ptr);

    /* write bytes */
    if (setjmp(png_jmpbuf(image->png_ptr)))
    {
        puts("Some error handling: error during writing bytes");
        fclose(fp);
        return false;
    }

    png_write_image(image->png_ptr, image->row_pointers);


    /* end write */
    if (setjmp(png_jmpbuf(image->png_ptr)))
    {
        puts("Some error handling: error during end of write");
        fclose(fp);
        return false;
    }

    png_write_end(image->png_ptr, NULL);
    fclose(fp);
    return true;
}


void free_png(PNG *png)
{
    free(png->row_pointers);
    free(png->row_data);
}
