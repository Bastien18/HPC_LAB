#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "sobel.h"

#define EXPECTED_NB_ARGS 4

void print_usage(){
    printf("Please provide an input image and an output image and finally an indication of the mode you want to use (1 for 1D tab or 2 for 2D tab)\n");
}

int main(int argc, char** argv) {
    struct img_1D_t *img_1d;
    struct img_chained_t *img_chained;
    struct img_1D_t *result_img;
    struct img_chained_t *result_img_chained;

    int mode;

    if (argc != EXPECTED_NB_ARGS) {
        fprintf(stderr, "Invalid number of arguments\n");
        print_usage();
        return EXIT_FAILURE;
    }

    mode = atoi(argv[3]);

    if(mode == 1){
        img_1d = load_image_1D(argv[1]);
        result_img = edge_detection_1D(img_1d);
        save_image(argv[2], img_1d);
    } else if (mode == 2){
        img_chained = load_image_chained(argv[1]);
        result_img_chained = edge_detection_chained(img_chained);
        save_image_chained(argv[2], img_chained);
    }

    return 0;
}