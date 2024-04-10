#include "k-means.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This function will calculate the euclidean distance between two pixels.
// Instead of using coordinates, we use the RGB value for evaluating distance.
float distance(uint8_t* p1, uint8_t* p2){
    float r_diff = p1[0] - p2[0];
    float g_diff = p1[1] - p2[1];
    float b_diff = p1[2] - p2[2];
    return sqrt(r_diff * r_diff + g_diff * g_diff + b_diff * b_diff);
}

// Function to initialize cluster centers using the k-means++ algorithm
void kmeans_pp(struct img_1D_t *image, int num_clusters, uint8_t *centers){
    // Select a random pixel as the first cluster center
    int first_center = (rand() % (image->width * image->height)) * image->components; 

    // Set the RGB values of the first center
    centers[0 + R_OFFSET] = image->data[first_center + R_OFFSET];
    centers[0 + G_OFFSET] = image->data[first_center + G_OFFSET];
    centers[0 + B_OFFSET] = image->data[first_center + B_OFFSET];

    float *distances = (float *)malloc(image->width * image->height * sizeof(float));

    // Calculate distances from each pixel to the first center
    uint8_t* dest = malloc(image->components * sizeof(uint8_t));
    memcpy(dest, centers, image->components * sizeof(uint8_t));

    for(int i = 0; i < image->width * image->height; ++i){
        uint8_t* src = malloc(image->components * sizeof(uint8_t));
        memcpy(src, image->data + i * image->components, image->components * sizeof(uint8_t));
        
        distances[i] = distance(src, dest) * distance(src, dest);

        free(src); 
    }
    free(dest);

    // Loop to find remaining cluster centers
    for (int i = 1; i < num_clusters; ++i){
        float total_weight = 0.0;

        // Calculate total weight (sum of distances)
        for (int j = 0; j < image->width * image->height; ++j){
            total_weight += distances[i];
        }

        float r = ((float)rand() / (float)RAND_MAX) * total_weight;
        int index = 0;

        // Choose the next center based on weighted probability
        while (r > distances[index]) {
            r -= distances[index];
            index++;
        }

        // Set the RGB values of the selected center
        centers[i * image->components + R_OFFSET] = image->data[index * image->components + R_OFFSET];
        centers[i * image->components + G_OFFSET] = image->data[index * image->components + G_OFFSET];
        centers[i * image->components + B_OFFSET] = image->data[index * image->components + B_OFFSET];

        // Update distances based on the new center
        uint8_t *new_center = malloc(image->components * sizeof(uint8_t));
        memcpy(dest, centers + i * image->components, image->components * sizeof(uint8_t));

        for (int j = 0; j < image->width * image->height; j++) {
            uint8_t* src = malloc(image->components * sizeof(uint8_t));
            memcpy(src, image->data + j * image->components, image->components * sizeof(uint8_t));
            
            float dist = distance(src, new_center) * distance(src, new_center);

            if (dist < distances[j]) {
                distances[j] = dist;
            }

            free(src);
        }

        free(new_center);
    }

    free(distances);
}

// This function performs k-means clustering on an image.
// It takes as input the image, its dimensions (width and height), and the number of clusters to find.
void kmeans(struct img_1D_t *image, int num_clusters){
    uint8_t *centers = calloc(num_clusters * image->components, sizeof(uint8_t));

    // Initialize the cluster centers using the k-means++ algorithm.
    kmeans_pp(image, num_clusters, centers);

    int *assignments = (int *)malloc(image->width * image->height * sizeof(int));

    // Assign each pixel in the image to its nearest cluster.
    for (int i = 0; i < image->width * image->height; ++i){
        float min_dist = INFINITY;
        int best_cluster = -1;

        uint8_t* src = malloc(image->components * sizeof(uint8_t));
        memcpy(src, image->data + i * image->components, image->components * sizeof(uint8_t));

        for (int c = 0; c < num_clusters; c++){
            uint8_t* dest = malloc(image->components * sizeof(uint8_t));
            memcpy(dest, centers + c * image->components, image->components * sizeof(uint8_t));

            float dist = distance(src, dest);

            if (dist < min_dist) {
                min_dist = dist;
                best_cluster = c;
            }

            assignments[i] = best_cluster;
        }
    }

    ClusterData *cluster_data = (ClusterData *)calloc(num_clusters, sizeof(ClusterData));

    // Compute the sum of the pixel values for each cluster.
    for (int i = 0; i < image->width * image->height; ++i){
        int cluster = assignments[i];
        cluster_data[cluster].count++;
        cluster_data[cluster].sum_r += (int)image->data[i * image->components + R_OFFSET];
        cluster_data[cluster].sum_g += (int)image->data[i * image->components + G_OFFSET];
        cluster_data[cluster].sum_b += (int)image->data[i * image->components + B_OFFSET];
    }

    // Update cluster centers based on the computed sums
    for (int c = 0; c < num_clusters; ++c){
        if (cluster_data[c].count > 0){
            centers[c * image->components + R_OFFSET] = cluster_data[c].sum_r / cluster_data[c].count;
            centers[c * image->components + G_OFFSET] = cluster_data[c].sum_g / cluster_data[c].count;
            centers[c * image->components + B_OFFSET] = cluster_data[c].sum_b / cluster_data[c].count;
        }
    }

    free(cluster_data);

    // Update image data with the cluster centers
    for (int i = 0; i < image->width * image->height; ++i){
        int cluster = assignments[i];
        image->data[i * image->components + R_OFFSET] = centers[cluster * image->components + R_OFFSET];
        image->data[i * image->components + G_OFFSET] = centers[cluster * image->components + G_OFFSET];
        image->data[i * image->components + B_OFFSET] = centers[cluster * image->components + B_OFFSET];
    }

    free(assignments);
    free(centers);
}