
#include "CW_tasks.h"

void print_info(PNG *image)
{
    printf("\n%d\n", image->height);
    printf("%d\n", image->width);
    printf("%d\n", image->channels);
    printf("%d\n", image->bit_depth);
    printf("%d\n", image->color_type);
    printf("%d\n", image->number_of_passes);
}

void replace_color(PNG *image, const png_byte *source, const png_byte *dest)
{
    for (int i = 0; i < image->height; i++)
    {
        for (int j = 0; j < image->width; j++)
        {
            if (is_eq_color(get_pixel(image, i, j), source, image->channels))
            {
                set_pixel(image, i, j, dest);
            }
        }
    }
}

void draw_rectangle_inner_border(PNG *image, RECTANGLE *rect, const png_byte *color, int size)
{
    if ((rect->x_start < 0) || (rect->y_start < 0) || (rect->x_end >= image->height) || (rect->y_end >= image->width))
    {
        puts("Some error handling : rectangle out of range");
        return;
    }
    for (int k = 0; k < size; k++)
    {
        for (int i = rect->x_start + k; i <= rect->x_end - k; i++)
        {
            set_pixel(image, i, rect->y_start + k, color);
            set_pixel(image, i, rect->y_end - k, color);
        }
        for (int i = rect->y_start + k; i <= rect->y_end - k; i++)
        {
            set_pixel(image, rect->x_start + k, i, color);
            set_pixel(image, rect->x_end - k, i, color);
        }
    }
}

MASK get_mask(PNG *image, const png_byte *color)
{
    MASK mask;
    mask.data = (bool **) malloc(sizeof(bool *) * image->height);
    for (int i = 0; i < image->height; i++)
        mask.data[i] = calloc(image->width, sizeof(bool));
    mask.height = image->height;
    mask.width = image->width;

    for (int i = 0; i < image->height; i++)
    {
        for (int j = 0; j < image->width; j++)
            if (is_eq_color(get_pixel(image, i, j), color, image->channels))
                mask.data[i][j] = 1;
    }
    return mask;
}

void free_mask(MASK *mask)
{
    for (int i = 0; i < mask->height; i++)
        free(mask->data[i]);
    free(mask->data);
}

RECTANGLE find_rectangle(MASK *mask, int x, int y)
{
    int x_start = x;
    int y_start = y;
    int x_end = x;
    int y_end = y;
    int count = 0;

    while (count < 3)
    {
        if ((count != 1) && (count != 3))
        {
            for (int i = y_start; i <= y_end; i++)
            {
                if ((x_end + 1 >= mask->height) || (mask->data[x_end + 1][i] != 1))
                {
                    count += 1;
                    break;
                }
            }
            if ((count != 1) && (count != 3))
                x_end = x_end + 1;
        }

        if ((count != 2) && (count != 3))
        {
            for (int i = x_start; i <= x_end; i++)
            {
                if ((y_end + 1 >= mask->width) || (mask->data[i][y_end + 1] != 1))
                {
                    count += 2;
                    break;
                }
            }
            if ((count != 2) && (count != 3))
                y_end = y_end + 1;
        }
    }
    RECTANGLE r = {x_start, y_start, x_end, y_end};
    return r;
}

void find_rectangles(PNG *image, const png_byte *color, const png_byte *color1, int border_size)
{
    RECTANGLE r;
    MASK mask = get_mask(image, color);

    for (int i = 0; i < image->height; i++)
        for (int j = 0; j < image->width; j++)
            if (mask.data[i][j] == 1)
            {
                r = find_rectangle(&mask, i, j);
                if ((r.x_end - r.x_start > border_size * 2) && (r.y_end - r.y_start > border_size * 2))
                {
                    for (int k = r.x_start; k <= r.x_end; k++)
                        for (int n = r.y_start; n <= r.y_end; n++)
                            mask.data[k][n] = 0;
                    draw_rectangle_inner_border(image, &r, color1, border_size);
                }
            }
    free_mask(&mask);
}


void draw_pattern(PNG *image, const int *pattern, int pattern_h, int pattern_w)
{
    png_byte color[4] = {0, 255, 0, 255};
    for (int i = 0; i < image->height; i++)
        for (int j = 0; j < image->width; j++)
            if (pattern[(i % pattern_h) * pattern_h + j % pattern_w] == 1)
            {
                set_pixel(image, i, j, color);
            }
}

bool is_border(PNG *image, int x, int y, int border_size)
{
    if ((x <= border_size) || (x >= image->height - border_size) || (y <= border_size) ||
        (y >= image->width - border_size))
        return true;
    return false;
}

void draw_border_pattern(PNG *image, const int *pattern, int pattern_h, int pattern_w, int border_size)
{

    resize_image_for_draw_border(image, border_size);

    png_byte color[4] = {0, 255, 0, 255};
    for (int i = 0; i < image->height; i++)
        for (int j = 0; j < image->width; j++)
            if ((pattern[(i % pattern_h) * pattern_h + j % pattern_w] == 1) && (is_border(image, i, j, border_size)))
            {
                set_pixel(image, i, j, color);
            }
}


void draw_border_gradient(PNG *image, int border_size)
{
    resize_image_for_draw_border(image, border_size);

    png_byte color1[4] = {108, 73, 3, 255};
    png_byte color2[4] = {238, 160, 8, 255};

    for (int i = 0; i < image->height; i++)
        for (int j = 0; j <= border_size; j++)
        {
            double delta = fabs((double) border_size / 2 - j) / ((double) border_size / 2);

            png_byte color[4] = {color1[0] + (color2[0] - color1[0]) * delta,
                                 color1[1] + (color2[1] - color1[1]) * delta,
                                 color1[2] + (color2[2] - color1[2]) * delta,
                                 255};
            set_pixel(image, i, j, color);
        }

    for (int i = 0; i < image->height; i++)
        for (int j = image->width - border_size; j < image->width; j++)
        {
            double delta = fabs((double) (image->width - (double) border_size / 2) - j) / ((double) border_size / 2);

            png_byte color[4] = {color1[0] + (color2[0] - color1[0]) * delta,
                                 color1[1] + (color2[1] - color1[1]) * delta,
                                 color1[2] + (color2[2] - color1[2]) * delta,
                                 255};
            set_pixel(image, i, j, color);
        }

    for (int i = 0; i < border_size; i++)
        for (int j = i; j < image->width - i; j++)
        {
            double delta = fabs((double) ((double) border_size / 2) - i) / ((double) border_size / 2);

            png_byte color[4] = {color1[0] + (color2[0] - color1[0]) * delta,
                                 color1[1] + (color2[1] - color1[1]) * delta,
                                 color1[2] + (color2[2] - color1[2]) * delta,
                                 255};
            set_pixel(image, i, j, color);
        }

    for (int i = image->height - border_size; i < image->height; i++)
        for (int j = image->height - i; j < image->width - (image->height - i); j++)
        {
            double delta = fabs((double) (image->height - (double) border_size / 2) - i) / ((double) border_size / 2);


            png_byte color[4] = {color1[0] + (color2[0] - color1[0]) * delta,
                                 color1[1] + (color2[1] - color1[1]) * delta,
                                 color1[2] + (color2[2] - color1[2]) * delta,
                                 255};
            set_pixel(image, i, j, color);
        }
}

int max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

void draw_border_fractal(PNG *image, int border_size)
{
    resize_image_for_draw_border(image, border_size);

    png_byte color[4] = {0, 255, 0, 255};
    png_byte black[4] = {0, 0, 0, 228};
    int height = image->height - 1;
    int width = image->width - 1;
    if (height % 2 == width % 2)
        height--;

    for (int i = 0; i < image->height; i++)
        for (int j = 0; j < image->width; j++)
            if (is_border(image, i, j, border_size))
                set_pixel(image, i, j, black);
    int x = 1;
    int y = 1;
    int len = 10;
    int count = 0;
    int k = 1;
    int i = 1;
    int j = 1;

    while (1)
    {
        color[0] = (abs(height / 2 - x) - abs(width / 2 - y)) % 256;
        color[1] = (abs(height / 2 - x) + abs(width / 2 - y)) % 256;
        color[2] = max(abs(height / 2 - x) * 1.5, abs(width / 2 - y) * 1.5) % 256;

        x += i;
        y += j;
        if (x == height)
        {
            i = -i;
            count++;
        }
        if (x == 0)
        {
            i = -i;
            count++;
        }
        if (y == width)
        {
            j = -j;
            count++;

        }
        if (y == 0)
        {
            j = -j;
            count++;
        }
        if (((x == 0) && (y == 0)) || ((x == 0) && (y == width)) || ((x == height) && (y == 0)) ||
            ((x == height) && (y == width)))
            break;
        if (count > 5000)
            break;
        len += k;
        if (abs(len) == 20)
            k = -k;
        if ((len > 7) && (is_border(image, x, y, border_size)))
            set_pixel(image, x, y, color);
    }
}
