#include <string.h>
#include <getopt.h>
#include "png_work.h"
#include "picture_work.h"
#include "CW_tasks.h"

int pixel_pattern[64] = {
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0
};
enum METHOD
{
    NONE = -1, DRAW_BORDER, REPLACE_COLOR, FIND_RECTANGLES, HELP
};

typedef struct arg_p
{
    char *input_file;
    char *output_file;
    char *method_optarg;
    enum METHOD method;
} ARG_P;

bool is_RGBA(PNG *image)
{
    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGB)
    {
        puts("Some error handling: input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA");
        return false;
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) != PNG_COLOR_TYPE_RGBA)
    {
        puts("Some error handling: color_type of input file must be PNG_COLOR_TYPE_RGBA");
        return false;
    }
    return true;
}

void print_help()
{
    puts("-h                                                                       Display this information\n");
    puts("--help                                                                   Display this information\n");
    puts("-i <source_file_name>                                                    Enter source file\n");
    puts("-o <dest_file_name>                                                      Enter destination file\n");
    puts("-B \"<fill_type> <size>\"                                                  Draw border\n");
    puts("--draw_border \"<fill_type> <size>\"                                       Draw border\n");
    puts("-S \"<source_color:[r g b a]> <final_color[r g b a]>\"                     Replace color1 with color2\n");
    puts("--substitute_color \"<source_color:[r g b a]> <final_color[r g b a]>\"     Replace color1 with color2\n");
    puts("-R \"<fill_color:[r g b a]> border_size\"                                  Find and outline rectangles filled with a given color\n");
    puts("--find_rectangles \"<fill_color:[r g b a]> border_size\"                   Find and outline rectangles filled with a given color\n");
}

void draw_border_p(PNG *image, char *arg)
{
    char fill_type[20] = {0};
    int border_size = 0;
    if (sscanf(arg, "%19s %d", fill_type, &border_size) != 2)
    {
        printf("<%s>", fill_type);
        puts("Some error handling: incorrect fill_type or bordr_size");
        return;
    }


    if (((strcmp(fill_type, "fractal") != 0) && (strcmp(fill_type, "gradient") != 0) &&
         (strcmp(fill_type, "pattern") != 0)))
    {
        puts("Some error handling: no such fill_type");
        return;
    }

    if ((border_size > 1000))
    {
        puts("Some error handling: border_size too big");
        return;
    }
    if (border_size <= 0)
    {
        puts("Some error handling: border_size <= 0");
        return;
    }

    if (strcmp(fill_type, "fractal") == 0)
    {
        draw_border_fractal(image, border_size);
    }
    if (strcmp(fill_type, "gradient") == 0)
    {
        draw_border_gradient(image, border_size);
    }
    if (strcmp(fill_type, "pattern") == 0)
    {
        draw_border_pattern(image, pixel_pattern, 8, 8, border_size);
    }
}

void replace_color_p(PNG *image, char *arg)
{
    int color1[4];
    int color2[4];

    if (sscanf(arg, "[%d %d %d %d] [%d %d %d %d]", &color1[0], &color1[1], &color1[2], &color1[3],
               &color2[0], &color2[1], &color2[2], &color2[3]) != 8)
    {
        puts("Some error handling: incorrect RGBA color");
        return;
    }

    png_byte clr1[4];
    png_byte clr2[4];

    for (int i = 0; i < 4; i++)
    {
        if ((color1[i] >= 256) || (color1[i] < 0) || (color2[i] < 0) || (color2[i] >= 256))
        {
            puts("Some error handling: RGBA parameters should be [0, 255]");
            return;
        }
        clr1[i] = (png_byte) color1[i];
        clr2[i] = (png_byte) color2[i];
    }
    replace_color(image, clr1, clr2);
}

void find_rectangles_p(PNG *image, char *arg)
{
    int rect_color[4];
    int border_color[4];
    int border_size = 0;
    if (sscanf(arg, "[%d %d %d %d] [%d %d %d %d] %d", &rect_color[0], &rect_color[1], &rect_color[2], &rect_color[3],
               &border_color[0], &border_color[1], &border_color[2], &border_color[3], &border_size) != 9)
    {
        puts("Some error handling: Invalid color or border_size");
        return;
    }
    if (border_size <= 0)
    {
        puts("border_size <= 0");
    }
    if ((border_size * 2 >= image->width) || (border_size * 2 >= image->height))
    {
        puts("border_size too big");
        return;
    }

    png_byte rect_clr[4];
    png_byte border_clr[4];
    for (int i = 0; i < 4; i++)
    {
        if ((rect_color[i] >= 256) || (rect_color[i] < 0) || (border_color[i] < 0) || (border_color[i] >= 256))
        {
            puts("Some error handling: RGBA parameters should be [0, 255]");
            return;
        }
        rect_clr[i] = (png_byte) rect_color[i];
        border_clr[i] = (png_byte) border_color[i];
    }
    find_rectangles(image, rect_clr, border_clr, border_size);
}

void get_method(int argc, char **argv, ARG_P *arg_p)
{
    int opt;

    char *opts = "i:o:hB:R:S:";

    struct option long_options[] = {
            {"help",            no_argument,       NULL, 'h'},
            {"draw_border",     required_argument, NULL, 'B'},
            {"input",           required_argument, NULL, 'i'},
            {"output",          required_argument, NULL, 'o'},
            {"find_rectangles", required_argument, NULL, 'R'},
            {"replace_color",   required_argument, NULL, 'S'},
            {NULL, 0,                              NULL, 0}
    };


    int long_index;

    opt = getopt_long(argc, argv, opts, long_options, &long_index);
    while (opt != -1)
    {
        switch (opt)
        {
            case 'i':
                arg_p->input_file = optarg;
                break;
            case 'o':
                arg_p->output_file = optarg;
                break;
            case 'h':
                if (arg_p->method != NONE)
                {
                    puts("-h INVALID_ARGUMENT");
                    return;
                }
            case 'P':
                if (arg_p->method != NONE)
                {
                    puts("-P INVALID_ARGUMENT");
                    return;
                }
                arg_p->method = HELP;
                break;
            case 'B':
                if (arg_p->method != NONE)
                {
                    puts("-B INVALID_ARGUMENT");
                    return;
                }
                arg_p->method = DRAW_BORDER;
                arg_p->method_optarg = optarg;
                break;
            case 'S':
                if (arg_p->method != NONE)
                {
                    puts("-S INVALID_ARGUMENT");
                    return;
                }
                arg_p->method = REPLACE_COLOR;
                arg_p->method_optarg = optarg;
                break;
            case 'R':
                if (arg_p->method != NONE)
                {
                    perror("-R INVALID_ARGUMENT");
                    return;
                }
                arg_p->method = FIND_RECTANGLES;
                arg_p->method_optarg = optarg;
                break;
            default:
                puts("Some error handling: Invalid flag");
                break;
        }
        opt = getopt_long(argc, argv, opts, long_options, &long_index);
    }
}

int main(int argc, char **argv)
{
    ARG_P arg_p = {NULL, NULL, NULL, NONE};

    PNG image;

    get_method(argc, argv, &arg_p);

    if (arg_p.method == HELP)
    {
        print_help();
        return 0;
    }
    if (arg_p.input_file == NULL)
    {
        puts("Some error handling: input_file not indicated");
        return -1;
    }
    if (arg_p.output_file == NULL)
    {
        arg_p.output_file = "out.png";
    }

    if (!read_png_file(arg_p.input_file, &image))
        return -1;

    if (!is_RGBA(&image))
        return -1;

    switch (arg_p.method)
    {
        case DRAW_BORDER:
            draw_border_p(&image, arg_p.method_optarg);
            break;
        case REPLACE_COLOR:
            replace_color_p(&image, arg_p.method_optarg);
            break;
        case FIND_RECTANGLES:
            find_rectangles_p(&image, arg_p.method_optarg);
            break;
        default:
            break;
    }

    if (!write_png_file(arg_p.output_file, &image))
        return -1;
    free_png(&image);
    return 0;
}

