#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "image.h"
#include "sobel.h"

#define DEBUG 1
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

const int16_t gauss_kernel[GAUSSIAN_KERNEL_SIZE * GAUSSIAN_KERNEL_SIZE] = {
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
    // Declaration and allocation of result image
    struct img_1D_t *res_img, *res_img_gs, *res_img_gauss;
    res_img_gs = allocate_image_1D(input_img->width, input_img->height, COMPONENT_GRAYSCALE);
    res_img_gauss = allocate_image_1D(input_img->width, input_img->height, COMPONENT_GRAYSCALE);
    res_img = allocate_image_1D(input_img->width, input_img->height, COMPONENT_GRAYSCALE);

    // Convert to grayscale
    rgb_to_grayscale_1D(input_img, res_img_gs);

    // Apply Gaussian Filter
    gaussian_filter_1D(res_img_gs, res_img_gauss, gauss_kernel);

    // Apply sobel
    sobel_filter_1D(res_img_gauss, res_img, sobel_v_kernel, sobel_h_kernel);

    // Free intermediate ressource
    free_image(res_img_gs);
    free_image(res_img_gauss);

    return res_img;
}

void rgb_to_grayscale_1D(const struct img_1D_t *img, struct img_1D_t *result)
{
    // Loop for every pixel in the matrix
    for (size_t i = 0; i < img->height; ++i)
    {
        for (size_t j = 0; j < img->width; ++j)
        {
            // Only copy pixel if grayscale
            if (img->components == COMPONENT_GRAYSCALE)
            {
                result->data[i * img->width + j] = img->data[i * img->width + j];
            }
            // Knowing alpha is not defined, for rgb and rgba input we'll use the standard rgb to grayscale conversion (skiping alpha component)
            else
            {
                result->data[i * img->width + j] = img->data[(i * img->width + j) * img->components + R_OFFSET] * FACTOR_R +
                                                   img->data[(i * img->width + j) * img->components + G_OFFSET] * FACTOR_G +
                                                   img->data[(i * img->width + j) * img->components + B_OFFSET] * FACTOR_B;
            }
        }
    }
}

uint16_t filter_pixel_1D(const struct img_1D_t *img, const int16_t *kernel, size_t kernel_size, size_t x, size_t y)
{
    uint16_t val = 0;

    for (size_t i = 0; i < kernel_size; ++i)
    {
        for (size_t j = 0; j < kernel_size; ++j)
        {
            size_t pixel_x = x - (kernel_size / 2) + j;
            size_t pixel_y = y - (kernel_size / 2) + i;

            val += img->data[pixel_y * img->width + pixel_x] * kernel[i * kernel_size + j];
        }
    }

    return val;
}

void gaussian_filter_1D(const struct img_1D_t *img, struct img_1D_t *res_img, const int16_t *kernel)
{
    const int16_t gauss_ponderation = 16;

    // Loop for every pixel in the matrix
    for (size_t i = 0; i < img->height; ++i)
    {
        for (size_t j = 0; j < img->width; ++j)
        {
            // Store the value if we are on a border pixel
            if (i == 0 || i == img->height - 1 || j == 0 || j == img->width - 1)
            {
                res_img->data[i * img->width + j] = img->data[i * img->width + j];
            }
            // Apply gaussian filter on the current pixel
            else
            {
                res_img->data[i * img->width + j] = filter_pixel_1D(img, kernel, GAUSSIAN_KERNEL_SIZE, j, i) / gauss_ponderation;
            }
        }
    }
}

void sobel_filter_1D(const struct img_1D_t *img, struct img_1D_t *res_img, const int16_t *v_kernel, const int16_t *h_kernel)
{
    int16_t v_value, h_value;

    // Loop for every pixel in the matrix
    for (size_t i = 0; i < img->height; i++)
    {
        for (size_t j = 0; j < img->width; j++)
        {
            // Store the value if we are on a border pixel
            if (i == 0 || i == img->height - 1 || j == 0 || j == img->width - 1)
            {
                res_img->data[i * img->width + j] = img->data[i * img->width + j];
            }
            // Apply sobel filters on the current pixel to determine if it's an edge or not
            else
            {
                v_value = filter_pixel_1D(img, v_kernel, SOBEL_KERNEL_SIZE, j, i);
                h_value = filter_pixel_1D(img, h_kernel, SOBEL_KERNEL_SIZE, j, i);

                if (sqrt(v_value * v_value + h_value * h_value) >= SOBEL_BINARY_THRESHOLD)
                {
                    res_img->data[i * img->width + j] = 0;
                }
                else
                {
                    res_img->data[i * img->width + j] = 255;
                }
            }
        }
    }
}

struct img_chained_t *edge_detection_chained(const struct img_chained_t *input_img)
{
    struct img_chained_t *res_img, *res_img_gs, *res_img_gauss;
    res_img = allocate_image_chained(input_img->width, input_img->height, COMPONENT_GRAYSCALE);
    res_img_gs = allocate_image_chained(input_img->width, input_img->height, COMPONENT_GRAYSCALE);
    res_img_gauss = allocate_image_chained(input_img->width, input_img->height, COMPONENT_GRAYSCALE);

    // Convert into grayscale
    rgb_to_grayscale_chained(input_img, res_img_gs);

    // Apply gauss filter
    gaussian_filter_chained(res_img_gs, res_img_gauss, gauss_kernel);

    // Apply edge detection
    sobel_filter_chained(res_img_gauss, res_img, sobel_v_kernel, sobel_h_kernel);

    // Free intermediate ressource
    free_image_chained(res_img_gs);
    free_image_chained(res_img_gauss);

    return res_img;
}

void rgb_to_grayscale_chained(const struct img_chained_t *img, struct img_chained_t *result)
{
    struct pixel_t *img_px = img->first_pixel;
    struct pixel_t *res_px = result->first_pixel;

    // Loop for every pixel in the matrix
    for (size_t i = 0; i < img->height; ++i)
    {
        for (size_t j = 0; j < img->width; ++j)
        {
            // Only copy pixel if grayscale
            if (img->components == COMPONENT_GRAYSCALE)
            {
                *res_px->pixel_val = *img_px->pixel_val;
            }
            // Knowing alpha is not defined, for rgb and rgba input we'll use the standard rgb to grayscale conversion (skiping alpha component)
            else
            {
                *res_px->pixel_val = img_px->pixel_val[R_OFFSET] * FACTOR_R +
                                     img_px->pixel_val[G_OFFSET] * FACTOR_G +
                                     img_px->pixel_val[B_OFFSET] * FACTOR_B;
            }

            img_px = img_px->next_pixel;
            res_px = res_px->next_pixel;
        }
    }
}

uint16_t filter_pixel_chained(struct pixel_t **right_border_px, const int16_t *kernel, size_t kernel_size)
{
    uint16_t val = 0;
    struct pixel_t *actual_px;

    // Now iterate over each neighbor pixels
    for (size_t i = 0; i < kernel_size; ++i)
    {
        actual_px = right_border_px[i];

        for (size_t j = 0; j < kernel_size; ++j)
        {
            val += *actual_px->pixel_val * kernel[(kernel_size - i - 1) * kernel_size + (kernel_size - j - 1)];
            actual_px = actual_px->next_pixel;
        }
    }

    return val;
}

void gaussian_filter_chained(const struct img_chained_t *img, struct img_chained_t *res_img, const int16_t *kernel)
{
    const uint16_t gauss_ponderation = 16;
    struct pixel_t *img_px = img->first_pixel;
    struct pixel_t *res_px = res_img->first_pixel;

    struct pixel_t *right_border_px[GAUSSIAN_KERNEL_SIZE];

    // Find the first right borders pixel
    right_border_px[0] = img->first_pixel;
    for (size_t i = 1; i < GAUSSIAN_KERNEL_SIZE; ++i)
    {

        right_border_px[i] = right_border_px[i - 1];

        for (size_t j = 0; j < img->width; ++j)
        {
            right_border_px[i] = right_border_px[i]->next_pixel;
        }
    }

    // Loop for every pixel in the matrix
    for (size_t i = 0; i < img->height; ++i)
    {
        for (size_t j = 0; j < img->width; ++j)
        {
            // Store the value if we are on a border pixel
            if (i == 0 || i == img->height - 1 || j == 0 || j == img->width - 1)
            {
                *res_px->pixel_val = *img_px->pixel_val;
            }
            // Apply gaussian filter on the current pixel
            else
            {
                *res_px->pixel_val = filter_pixel_chained(right_border_px, gauss_kernel, GAUSSIAN_KERNEL_SIZE) / gauss_ponderation;

                // Move right border of the matrix
                for (size_t i_border = 0; i_border < GAUSSIAN_KERNEL_SIZE; i_border++)
                {
                    // Move the right border of 3 pixel if we reach end of line
                    if (j == img->width - 2)
                    {
                        for (size_t skip = 0; skip < GAUSSIAN_KERNEL_SIZE; ++skip)
                            right_border_px[i_border] = right_border_px[i_border]->next_pixel;
                    }
                    // Move only once otherwise
                    else
                    {
                        right_border_px[i_border] = right_border_px[i_border]->next_pixel;
                    }
                }
            }

            img_px = img_px->next_pixel;
            res_px = res_px->next_pixel;
        }
    }
}

void sobel_filter_chained(const struct img_chained_t *img, struct img_chained_t *res_img,
                          const int16_t *v_kernel, const int16_t *h_kernel)
{
    struct pixel_t *img_px = img->first_pixel;
    struct pixel_t *res_px = res_img->first_pixel;
    int16_t v_value, h_value;

    struct pixel_t *right_border_px[SOBEL_KERNEL_SIZE];

    // Find the first right borders pixel
    right_border_px[0] = img->first_pixel;
    for (size_t i = 1; i < SOBEL_KERNEL_SIZE; ++i)
    {
        right_border_px[i] = right_border_px[i - 1];

        for (size_t j = 0; j < img->width; ++j)
        {
            right_border_px[i] = right_border_px[i]->next_pixel;
        }
    }

    // Loop for every pixel in the matrix
    for (size_t i = 0; i < img->height; ++i)
    {
        for (size_t j = 0; j < img->width; ++j)
        {
            // Store the value if we are on a border pixel
            if (i == 0 || i == img->height - 1 || j == 0 || j == img->width - 1)
            {
                *res_px->pixel_val = *img_px->pixel_val;
            }
            // Apply sobel filter on the current pixel
            else
            {
                v_value = filter_pixel_chained(right_border_px, v_kernel, SOBEL_KERNEL_SIZE);
                h_value = filter_pixel_chained(right_border_px, h_kernel, SOBEL_KERNEL_SIZE);

                if(sqrt(v_value*v_value + h_value*h_value) >= SOBEL_BINARY_THRESHOLD){
                    *res_px->pixel_val = 0;
                }else{
                    *res_px->pixel_val = UINT8_MAX;
                }

                // Move right border of the matrix
                for (size_t i_border = 0; i_border < GAUSSIAN_KERNEL_SIZE; i_border++)
                {
                    // Move the right border of 3 pixel if we reach end of line
                    if (j == img->width - 2)
                    {
                        for (size_t skip = 0; skip < GAUSSIAN_KERNEL_SIZE; ++skip)
                            right_border_px[i_border] = right_border_px[i_border]->next_pixel;
                    }
                    // Move only once otherwise
                    else
                    {
                        right_border_px[i_border] = right_border_px[i_border]->next_pixel;
                    }
                }
            }

            img_px = img_px->next_pixel;
            res_px = res_px->next_pixel;
        }
    }
}