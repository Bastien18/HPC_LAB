#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "image.h"
#include "sobel.h"

#define GAUSSIAN_KERNEL_SIZE    3
#define SOBEL_KERNEL_SIZE       3

#define SOBEL_BINARY_THRESHOLD  150  // in the range 0 to uint8_max (255)

const int16_t sobel_v_kernel[SOBEL_KERNEL_SIZE*SOBEL_KERNEL_SIZE] = {
    -1, -2, -1,
     0,  0,  0,
     1,  2,  1,
};

const int16_t sobel_h_kernel[SOBEL_KERNEL_SIZE*SOBEL_KERNEL_SIZE] = {
    -1,  0,  1,
    -2,  0,  2,
    -1,  0,  1,
};

const uint16_t gauss_kernel[GAUSSIAN_KERNEL_SIZE*GAUSSIAN_KERNEL_SIZE] = {
    1, 2, 1,
    2, 4, 2,
    1, 2, 1,
};

struct img_1D_t *edge_detection_1D(const struct img_1D_t *input_img){
    struct img_1D_t *res_img;

    return res_img;
}

void rgb_to_grayscale_1D(const struct img_1D_t *img, struct img_1D_t *result){
    size_t index = 0;

}

void gaussian_filter_1D(const struct img_1D_t *img, struct img_1D_t *res_img, const uint16_t *kernel){
    const uint16_t gauss_ponderation = 16;

}

void sobel_filter_1D(const struct img_1D_t *img, struct img_1D_t *res_img, const int16_t *v_kernel, const int16_t *h_kernel){

}


struct img_chained_t *edge_detection_chained(const struct img_chained_t *input_img)
{
    struct img_chained_t *res_img;

    return res_img;
}

void rgb_to_grayscale_chained(const struct img_chained_t *img, struct img_chained_t *result)
{

}

void gaussian_filter_chained(const struct img_chained_t *img, struct img_chained_t *res_img, const uint16_t *kernel)
{
    const uint16_t gauss_ponderation = 16;

}

void sobel_filter_chained(const struct img_chained_t *img, struct img_chained_t *res_img,
                          const int16_t *v_kernel, const int16_t *h_kernel)
{
    // TODO
}