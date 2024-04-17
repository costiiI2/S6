#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "image.h"
#include "sobel.h"

#ifdef __SSE__
#include <xmmintrin.h>
#else
#error "SSE is not supported on this platform"
#endif




#define GAUSSIAN_KERNEL_SIZE 3
#define SOBEL_KERNEL_SIZE 3
#define SOBEL_BINARY_THRESHOLD 150 // in the range 0 to uint8_max (255)

const int16_t sobel_v_kernel[SOBEL_KERNEL_SIZE * SOBEL_KERNEL_SIZE] = {
    -1,
    -2,
    -1,
    0,
    0,
    0,
    1,
    2,
    1,
};

const int16_t sobel_h_kernel[SOBEL_KERNEL_SIZE * SOBEL_KERNEL_SIZE] = {
    -1,
    0,
    1,
    -2,
    0,
    2,
    -1,
    0,
    1,
};

const uint16_t gauss_kernel[GAUSSIAN_KERNEL_SIZE * GAUSSIAN_KERNEL_SIZE] = {
    1,
    2,
    1,
    2,
    4,
    2,
    1,
    2,
    1,
};

struct img_1D_t *edge_detection_1D(const struct img_1D_t *input_img)
{
    struct img_1D_t *res_img, *tmp_img;

    printf("width: %d\n", input_img->width);
    printf("height: %d\n", input_img->height);

    // allocate memory for the result image
    res_img = allocate_image_1D(input_img->width, input_img->height, COMPONENT_GRAYSCALE);
    tmp_img = allocate_image_1D(input_img->width, input_img->height, COMPONENT_GRAYSCALE);

    rgb_to_grayscale_1D(input_img, res_img);
    gaussian_filter_1D(res_img, tmp_img, gauss_kernel);
    sobel_filter_1D(tmp_img, res_img, sobel_v_kernel, sobel_h_kernel);
    free_image(tmp_img);

    return res_img;
}


void rgb_to_grayscale_1D(const struct img_1D_t *img, struct img_1D_t *result)
{
    int components = img->components;

    if (components != COMPONENT_RGB && components != COMPONENT_RGBA)
    {
        fprintf(stderr, "Error: image is not in RGB format\n");
        return;
    }

    int width = img->width;
    int height = img->height;
    int size = width * height;

    uint8_t *src = img->data;
    uint8_t *dst = result->data;

    __m128 factor = _mm_set_ps(0, FACTOR_B, FACTOR_G, FACTOR_R);

    for (int i = 0; i < size; i++)
    {
        int index = i * components;
        __m128 rgb = _mm_set_ps(0, src[index + B_OFFSET], src[index + G_OFFSET], src[index + R_OFFSET]);
        __m128 product = _mm_mul_ps(rgb, factor);
        __m128 sum = _mm_add_ps(product, _mm_movehl_ps(product, product));
        sum = _mm_add_ss(sum, _mm_shuffle_ps(sum, sum, 1));
        dst[i] = (uint8_t)_mm_cvtss_f32(sum);
    }

    result->width = width;
    result->height = height;
    result->components = COMPONENT_GRAYSCALE;
}


void gaussian_filter_1D(const struct img_1D_t *img, struct img_1D_t *res_img, const uint16_t *kernel)
{
    const uint16_t gauss_ponderation = 16;
    int width = img->width;
    int height = img->height;

    if (img->components != COMPONENT_GRAYSCALE)
    {
        fprintf(stderr, "Error: image is not in grayscale format\n");
        return;
    }

    for (int i = 0; i < width * height; i++)
{
    int x = i % width;
    int y = i / width;
    __m128 sum = _mm_setzero_ps();
    for (int j = 0; j < GAUSSIAN_KERNEL_SIZE; j++)
    {
        for (int k = 0; k < GAUSSIAN_KERNEL_SIZE; k++)
        {
            int x_k = x + k - 1;
            int y_j = y + j - 1;
            if (x_k >= 0 && x_k < width && y_j >= 0 && y_j < height)
            {
                // if the pixel is out of the image, we take the pixel current pixel
                if (x_k < 0 || x_k >= width)
                    x_k = x;
                if (y_j < 0 || y_j >= height)
                    y_j = y;

                __m128 pixel = _mm_set1_ps(img->data[y_j * width + x_k]);
                __m128 kernel_val = _mm_set1_ps(kernel[j * GAUSSIAN_KERNEL_SIZE + k]);
                sum = _mm_add_ps(sum, _mm_mul_ps(pixel, kernel_val));
            }
        }
    }
    res_img->data[i] = _mm_cvtss_f32(sum) / gauss_ponderation;
    }

    res_img->width = width;
    res_img->height = height;
    res_img->components = COMPONENT_GRAYSCALE;
}

void sobel_filter_1D(const struct img_1D_t *img, struct img_1D_t *res_img, const int16_t *v_kernel, const int16_t *h_kernel)
{
    int width = img->width;
    int height = img->height;

    if (img->components != COMPONENT_GRAYSCALE)
    {
        fprintf(stderr, "Error: image is not in grayscale format\n");
        return;
    }

    for (int i = 0; i < width * height; i++)
    {
        int x = i % width;
        int y = i / width;
        __m128 sum_v = _mm_setzero_ps();
        __m128 sum_h = _mm_setzero_ps();
        for (int j = 0; j < SOBEL_KERNEL_SIZE; j++)
        {
            for (int k = 0; k < SOBEL_KERNEL_SIZE; k++)
            {
                int x_k = x + k - 1;
                int y_j = y + j - 1;
                if (x_k >= 0 && x_k < width && y_j >= 0 && y_j < height)
                {
                    // if the pixel is out of the image, we take the pixel current pixel
                    if (x_k < 0 || x_k >= width)
                        x_k = x;
                    if (y_j < 0 || y_j >= height)
                        y_j = y;

                    __m128 pixel = _mm_set1_ps(img->data[y_j * width + x_k]);
                    __m128 v_kernel_val = _mm_set1_ps(v_kernel[j * SOBEL_KERNEL_SIZE + k]);
                    __m128 h_kernel_val = _mm_set1_ps(h_kernel[j * SOBEL_KERNEL_SIZE + k]);
                    sum_v = _mm_add_ps(sum_v, _mm_mul_ps(pixel, v_kernel_val));
                    sum_h = _mm_add_ps(sum_h, _mm_mul_ps(pixel, h_kernel_val));
                }
            }
        }
        __m128 sum = _mm_sqrt_ps(_mm_add_ps(_mm_mul_ps(sum_v, sum_v), _mm_mul_ps(sum_h, sum_h)));
        res_img->data[i] = _mm_cvtss_f32(sum);

        if (res_img->data[i] > SOBEL_BINARY_THRESHOLD)
            res_img->data[i] = 255;
        else
            res_img->data[i] = 0;
    }

    res_img->width = width;
    res_img->height = height;
    res_img->components = COMPONENT_GRAYSCALE;
}

struct img_chained_t *edge_detection_chained(const struct img_chained_t *input_img)
{
    struct img_chained_t *res_img, *tmp_img;

    // allocate memory for the result image
    res_img = allocate_image_chained(input_img->width, input_img->height, COMPONENT_GRAYSCALE);
    tmp_img = allocate_image_chained(input_img->width, input_img->height, COMPONENT_GRAYSCALE);

    rgb_to_grayscale_chained(input_img, res_img);
    gaussian_filter_chained(res_img, tmp_img, gauss_kernel);
    sobel_filter_chained(tmp_img, res_img, sobel_v_kernel, sobel_h_kernel);
    return res_img;
}

void rgb_to_grayscale_chained(const struct img_chained_t *img, struct img_chained_t *result)
{
    int components = img->components;

    if (components != COMPONENT_RGB && components != COMPONENT_RGBA)
    {
        fprintf(stderr, "Error: image is not in RGB format\n");
        return;
    }
    else
    {
        printf("start conversion\n");
    }

    int width = img->width;
    int height = img->height;
    int size = width * height;
    // chainedlist that start at bottom right corner
    struct pixel_t *current_pixel = img->first_pixel;
    struct pixel_t *result_pixel = result->first_pixel;

    for (int i = 0; i < size; i++)
    {
        uint8_t r = current_pixel->pixel_val[R_OFFSET];
        uint8_t g = current_pixel->pixel_val[G_OFFSET];
        uint8_t b = current_pixel->pixel_val[B_OFFSET];

        result_pixel->pixel_val[0] = (uint8_t)(FACTOR_R * r + FACTOR_G * g + FACTOR_B * b);

        current_pixel = current_pixel->next_pixel;
        result_pixel = result_pixel->next_pixel;
    }

    result->width = width;
    result->height = height;
    result->components = COMPONENT_GRAYSCALE;
}

void gaussian_filter_chained(const struct img_chained_t *img, struct img_chained_t *res_img, const uint16_t *kernel)
{
    const uint16_t gauss_ponderation = 16;
    int width = img->width;
    int height = img->height;

    if (img->components != COMPONENT_GRAYSCALE)
    {
        fprintf(stderr, "Error: image is not in grayscale format\n");
        return;
    }

 
    struct pixel_t *current_pixel = img->first_pixel;
    struct pixel_t *res_pixel = res_img->first_pixel;
    struct pixel_t *bottom_pixel = current_pixel;

    for (int i = (width * height )- 1; i > 0; i--)
    {
        int x = i % width;
        int y = i / width;
        uint32_t sum = 0;

        // if its a border pixel, we take the current pixel as result
        if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
        {
            res_pixel->pixel_val[0] = current_pixel->pixel_val[0] ;
            res_pixel = res_pixel->next_pixel;
            current_pixel = current_pixel->next_pixel;
            continue;
        }
        
        struct pixel_t *tmp_pixel = bottom_pixel;
        for (int j = 0; j < SOBEL_KERNEL_SIZE * SOBEL_KERNEL_SIZE; j++)
        {
            sum += tmp_pixel->pixel_val[0] * kernel[ j];
            tmp_pixel = tmp_pixel->next_pixel;
            if (j == 2 || j == 5)
            {
                for (int k = 0; k < width-SOBEL_KERNEL_SIZE-1; k++)
                {
                    tmp_pixel = tmp_pixel->next_pixel;
                }
            }

        }

        res_pixel->pixel_val[0] = sum / gauss_ponderation;
        res_pixel = res_pixel->next_pixel;
        current_pixel = current_pixel->next_pixel;
        bottom_pixel = bottom_pixel->next_pixel;

        if (x == 1 && y != 0)
        {

            bottom_pixel = bottom_pixel->next_pixel;
            bottom_pixel = bottom_pixel->next_pixel;
        }
    }

    res_img->width = width;
    res_img->height = height;
    res_img->components = COMPONENT_GRAYSCALE;
}

void sobel_filter_chained(const struct img_chained_t *img, struct img_chained_t *res_img,
                          const int16_t *v_kernel, const int16_t *h_kernel)
{
    int width = img->width;
    int height = img->height;

    if (img->components != COMPONENT_GRAYSCALE)
    {
        fprintf(stderr, "Error: image is not in grayscale format\n");
        return;
    }

    struct pixel_t *current_pixel = img->first_pixel;
    struct pixel_t *res_pixel = res_img->first_pixel;
    struct pixel_t *bottom_pixel = current_pixel;

    for (int i = (width * height )- 1; i > 0; i--)
    {
        int x = i % width;
        int y = i / width;
        uint32_t sum_v = 0;
        uint32_t sum_h = 0;

        // if its a border pixel, we take the current pixel as result
        if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
        {
            res_pixel->pixel_val[0] = current_pixel->pixel_val[0] ;
            res_pixel = res_pixel->next_pixel;
            current_pixel = current_pixel->next_pixel;
            continue;
        }
        
        struct pixel_t *tmp_pixel = bottom_pixel;
        for (int j = 0; j < SOBEL_KERNEL_SIZE * SOBEL_KERNEL_SIZE; j++)
        {
            sum_h += tmp_pixel->pixel_val[0] * h_kernel[ j];
            sum_v += tmp_pixel->pixel_val[0] * v_kernel[ j];
            tmp_pixel = tmp_pixel->next_pixel;
            if (j == 2 || j == 5)
            {
                for (int k = 0; k < width-SOBEL_KERNEL_SIZE-1; k++)
                {
                    tmp_pixel = tmp_pixel->next_pixel;
                }
            }

        }


        res_pixel->pixel_val[0] = sqrt(sum_v * sum_v + sum_h * sum_h);
        if (res_pixel->pixel_val[0] > SOBEL_BINARY_THRESHOLD)
            res_pixel->pixel_val[0] = 255;
        else
            res_pixel->pixel_val[0] = 0;

        res_pixel = res_pixel->next_pixel;
        current_pixel = current_pixel->next_pixel;
        bottom_pixel = bottom_pixel->next_pixel;

        if (x == 1 && y != 0)
        {

            bottom_pixel = bottom_pixel->next_pixel;
            bottom_pixel = bottom_pixel->next_pixel;
        }
    }

    res_img->width = width;
    res_img->height = height;
    res_img->components = COMPONENT_GRAYSCALE;
}