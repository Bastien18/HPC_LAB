#include <stdlib.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "image.h"

struct img_1D_t *load_image_1D(const char *path){
    struct img_1D_t *img;

    /* Allocate struct */
    img = (struct img_1D_t*)malloc(sizeof(struct img_1D_t));
    if (!img) {
        fprintf(stderr, "[%s] struct allocation error\n", __func__);
        perror(__func__);
        exit(EXIT_FAILURE);
    }

    /* Load the image */ 
    img->data = stbi_load(path, &(img->width), &(img->height), &(img->components), 0);
    if (!(img->data)) {
        fprintf(stderr, "[%s] stbi load image failed\n", __func__);
        perror(__func__);
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG
    fprintf(stdout, "[%s] image %s loaded (%d components, %dx%d)\n", __func__,
            path, img->components, img->width, img->height);
#endif

    return img;
}

void print_linked_list(struct img_chained_t *img_chained) {
    printf("Linked list contents:\n");
    struct pixel_t *current_pixel = img_chained->first_pixel;
    while (current_pixel != NULL) {
        printf("Pixel values: ");
        for (int i = 0; i < img_chained->components; ++i) {
            printf("%u ", current_pixel->pixel_val[i]);
        }
        printf("\n");
        current_pixel = current_pixel->next_pixel;
    }
}

struct img_chained_t *load_image_chained(const char *path) {
    struct img_chained_t *img_chained = malloc(sizeof(struct img_chained_t));
    if (!img_chained) {
        fprintf(stderr, "[%s] struct allocation error\n", __func__);
        perror(__func__);
        exit(EXIT_FAILURE);
    }

    img_chained->first_pixel = NULL; // Initialize the list to be empty

    /* Load the image */
    int width, height, components;
    uint8_t *data = stbi_load(path, &width, &height, &components, 0);
    if (!data) {
        fprintf(stderr, "[%s] stbi load image failed\n", __func__);
        perror(__func__);
        exit(EXIT_FAILURE);
    }

    img_chained->width = width;
    img_chained->height = height;
    img_chained->components = components;

    for (int i = 0; i < width * height * components; i += components) {
        struct pixel_t *new_pixel = malloc(sizeof(struct pixel_t));
        if (!new_pixel) {
            fprintf(stderr, "[%s] pixel allocation error\n", __func__);
            perror(__func__);
            exit(EXIT_FAILURE);
        }

        // Allocate memory for pixel_val
        new_pixel->pixel_val = malloc(sizeof(uint8_t) * components);
        if (!new_pixel->pixel_val) {
            fprintf(stderr, "[%s] pixel_val allocation error\n", __func__);
            perror(__func__);
            exit(EXIT_FAILURE);
        }

        // Copy pixel data from data[i] to new_pixel->pixel_val
        memcpy(new_pixel->pixel_val, &data[i], sizeof(uint8_t) * components);

        // Insert the new pixel at the beginning of the list
        new_pixel->next_pixel = img_chained->first_pixel;
        img_chained->first_pixel = new_pixel;
    }

#ifdef DEBUG
    fprintf(stdout, "[%s] image %s loaded (%d components, %dx%d)\n", __func__,
            path, img_chained->components, img_chained->width, img_chained->height);
#endif

    // Free the image data since we've already copied the necessary pixels
    free(data);

    return img_chained;
}

int save_image(const char *dest_path, const struct img_1D_t *img) {
    int ret;

    ret = stbi_write_png(dest_path, img->width, img->height, img->components, img->data, PNG_STRIDE_IN_BYTES);
    if(ret) {
        #ifdef DEBUG
        fprintf(stdout, "[%s] PNG file %s saved (%dx%d)\n", __func__, dest_path, img->width, img->height);
        #endif
    }
    else {
        fprintf(stderr, "[%s] save image failed\n", __func__);
    }

    return ret;
}

int save_image_chained(const char *dest_path, const struct img_chained_t *img) {
    int ret = 0;

    // Allocate memory for image data
    uint8_t *data = malloc(img->width * img->height * img->components);
    if (!data) {
        fprintf(stderr, "[%s] image data allocation error\n", __func__);
        return -1;
    }

    // Copy pixel data from linked list to the data array
    struct pixel_t *current_pixel = img->first_pixel;
    int index = img->width * img->height * img->components - 1; // Start from the last pixel
    while (current_pixel != NULL) {
        for (int i = img->components - 1; i >= 0; --i) {
            data[index--] = current_pixel->pixel_val[i];
        }
        current_pixel = current_pixel->next_pixel;
    }

    // Save the image using stbi_write_png
    ret = stbi_write_png(dest_path, img->width, img->height, img->components, data, PNG_STRIDE_IN_BYTES);
    if (ret) {
#ifdef DEBUG
        fprintf(stdout, "[%s] PNG file %s saved (%dx%d)\n", __func__, dest_path, img->width, img->height);
#endif
    } else {
        fprintf(stderr, "[%s] save image failed\n", __func__);
    }

    // Free the allocated memory for image data
    free(data);

    return ret;
}



struct img_1D_t *allocate_image_1D(int width, int height, int components){
    struct img_1D_t *img;

    if (components == 0 || components > COMPONENT_RGBA)
        return NULL;

    /* Allocate struct */
    img = (struct img_1D_t*)malloc(sizeof(struct img_1D_t));
    if (!img) {
        fprintf(stderr, "[%s] allocation error\n", __func__);
        perror(__func__);
        exit(EXIT_FAILURE);
    }

    img->width = width;
    img->height = height;
    img->components = components;

    /* Allocate space for image data */
    img->data = (uint8_t*)calloc(img->width * img->height *img->components, sizeof(uint8_t));
    if (!(img->data)) {
        fprintf(stderr, "[%s] image allocation error\n", __func__);
        perror(__func__);
        exit(EXIT_FAILURE);
    }

    return img;
}

struct img_chained_t *allocate_image_chained(int width, int height, int components) {
    struct img_chained_t *img_chained;
    if (components == 0 || components > COMPONENT_RGBA)
        return NULL;

    img_chained = (struct img_chained_t*)malloc(sizeof(struct img_chained_t));
    
    if (!img_chained) {
        fprintf(stderr, "[%s] struct allocation error\n", __func__);
        perror(__func__);
        exit(EXIT_FAILURE);
    }

    img_chained->width = width;
    img_chained->height = height;
    img_chained->components = components;

    img_chained->first_pixel = NULL; // Initialize the list to be empty

    // Allocate memory for the linked list of pixels
    for (int i = 0; i < width * height; ++i) {
        struct pixel_t *new_pixel = (struct pixel_t*)malloc(sizeof(struct pixel_t));
        if (!new_pixel) {
            fprintf(stderr, "[%s] pixel allocation error\n", __func__);
            perror(__func__);
            exit(EXIT_FAILURE);
        }

        // Allocate memory for pixel_val
        new_pixel->pixel_val = malloc(sizeof(uint8_t) * components);
        if (!new_pixel->pixel_val) {
            fprintf(stderr, "[%s] pixel_val allocation error\n", __func__);
            perror(__func__);
            exit(EXIT_FAILURE);
        }

        // Initialize pixel_val to zero
        memset(new_pixel->pixel_val, 0, sizeof(uint8_t) * components);

        // Insert the new pixel at the beginning of the list
        new_pixel->next_pixel = img_chained->first_pixel;
        img_chained->first_pixel = new_pixel;
    }

    return img_chained;
}

void free_image(struct img_1D_t *img) {
    stbi_image_free(img->data);
    free(img);
}