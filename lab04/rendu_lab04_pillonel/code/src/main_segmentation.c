#include "k-means.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    struct img_1D_t *img;
    int nb_cluster = 0;
    clock_t start, end;

    if (argc != 4) {
        fprintf(stderr, "Usage : %s <img_src.png> <nb_clusters> <img_dest.png>\n", argv[0]);
        return EXIT_FAILURE;
    }

    nb_cluster = atoi(argv[2]);
    if (nb_cluster <= 0){
        printf("The number of clusters should be greater than 0\n");
        return 1;
    }

    img = load_image_1D(argv[1]);

    printf("Image loaded!\n");

    start = clock();
    kmeans(img, nb_cluster);
    end = clock();

    save_image(argv[3], img);

    free_image(img);

    printf("%f\n", (float)(end - start) / CLOCKS_PER_SEC);
    printf("Programm ended successfully\n\n");

    return 0;
}
