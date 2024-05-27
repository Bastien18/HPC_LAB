#include "k-means.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>

//#define VECTORIZE 1

// This function will calculate the euclidean distance between two pixels.
// Instead of using coordinates, we use the RGB value for evaluating distance.
float distance(uint8_t *p1, uint8_t *p2)
{
    float r_diff = p1[0] - p2[0];
    float g_diff = p1[1] - p2[1];
    float b_diff = p1[2] - p2[2];
    return sqrt(r_diff * r_diff + g_diff * g_diff + b_diff * b_diff);
}

#ifdef VECTORIZE
float distance_avx2(__m128i p1, __m128i p2, size_t components)
{
    __m128i p1_32 = _mm_cvtepu8_epi32(p1);
    __m128i p2_32 = _mm_cvtepu8_epi32(p2);

    // Soustraire p1 de p2
    __m128i diff_32 = _mm_sub_epi32(p1_32, p2_32);

    // Convertir le résultat en float si nécessaire
    __m128 diff_float = _mm_cvtepi32_ps(diff_32);

    // Élever chaque flottant au carré
    __m128 squared_diff = _mm_mul_ps(diff_float, diff_float);

    float sum = 0;

    for (size_t i = 0; i < components; ++i)
    {
        sum += *(((float *)(&squared_diff)) + i);
    }

    return sum;
}
#endif

// Function to initialize cluster centers using the k-means++ algorithm
void kmeans_pp(struct img_1D_t *image, int num_clusters, uint8_t *centers)
{
    // Select a random pixel as the first cluster center
    int first_center = (rand() % (image->width * image->height)) * image->components;

    // Set the RGB values of the first center
    centers[0 + R_OFFSET] = image->data[first_center + R_OFFSET];
    centers[0 + G_OFFSET] = image->data[first_center + G_OFFSET];
    centers[0 + B_OFFSET] = image->data[first_center + B_OFFSET];

    float *distances = (float *)malloc(image->width * image->height * sizeof(float));

#ifdef VECTORIZE
    // Chargement des composants du centre
    __m128i dest = _mm_loadu_si128((__m128i *)centers);

    for (int i = 0; i < image->width * image->height; i++)
    {
        // Chargement des composant du pixel
        __m128i src = _mm_loadu_si128((__m128i *)(image->data + i * image->components));

        distances[i] = distance_avx2(src, dest, image->components);
    }
#endif

#ifndef VECTORIZE
    // Calculate distances from each pixel to the first center
    uint8_t *dest = malloc(image->components * sizeof(uint8_t));
    memcpy(dest, centers, image->components * sizeof(uint8_t));
    uint8_t *src = malloc(image->components * sizeof(uint8_t));
    for (int i = 0; i < image->width * image->height; ++i)
    {
        memcpy(src, image->data + i * image->components, image->components * sizeof(uint8_t));

        distances[i] = distance(src, dest) * distance(src, dest);
    }
    free(src);
    free(dest);
#endif

    uint8_t *new_center = malloc(image->components * sizeof(uint8_t));
    uint8_t *src = malloc(image->components * sizeof(uint8_t));

    // Loop to find remaining cluster centers
    for (int i = 1; i < num_clusters; ++i)
    {
        float total_weight = 0.0;

        // Calculate total weight (sum of distances)
        for (int j = 0; j < image->width * image->height; j += 4)
        {
            total_weight += distances[i];
        }

        float r = ((float)rand() / (float)RAND_MAX) * total_weight;
        int index = 0;

        // Choose the next center based on weighted probability
        while (index < image->width * image->height && r > distances[index])
        {
            r -= distances[index];
            index++;
        }

        // Set the RGB values of the selected center
        centers[i * image->components + R_OFFSET] = image->data[index * image->components + R_OFFSET];
        centers[i * image->components + G_OFFSET] = image->data[index * image->components + G_OFFSET];
        centers[i * image->components + B_OFFSET] = image->data[index * image->components + B_OFFSET];

#ifdef VECTORIZE
        // Chargement des composants du centre
        dest = _mm_loadu_si128((__m128i *)new_center);

        for (int j = 0; j < image->width * image->height; j++)
        {
            // Chargement des composant du pixel
            __m128i src = _mm_loadu_si128((__m128i *)(image->data + i * image->components));

            float dist = distance_avx2(src, dest, image->components);

            if (dist < distances[j])
            {
                distances[j] = dist;
            }
        }
#endif

#ifndef VECTORIZE
        // Update distances based on the new center
        memcpy(dest, centers + i * image->components, image->components * sizeof(uint8_t));

        for (int j = 0; j < image->width * image->height; j++)
        {

            memcpy(src, image->data + j * image->components, image->components * sizeof(uint8_t));

            float dist = distance(src, new_center) * distance(src, new_center);

            if (dist < distances[j])
            {
                distances[j] = dist;
            }
        }
#endif
    }
    free(src);
    free(new_center);
    free(distances);
}

// This function performs k-means clustering on an image.
// It takes as input the image, its dimensions (width and height), and the number of clusters to find.
void kmeans(struct img_1D_t *image, int num_clusters)
{
    uint8_t *centers = calloc(num_clusters * image->components, sizeof(uint8_t));

    // Initialize the cluster centers using the k-means++ algorithm.
    kmeans_pp(image, num_clusters, centers);

    int *assignments = (int *)malloc(image->width * image->height * sizeof(int));

#ifdef VECTORIZE
    for (int i = 0; i < image->width * image->height; i++)
    {
        float min_dist = INFINITY;
        int best_cluster = -1;

        // Chargement des composant du pixel
        __m128i src = _mm_loadu_si128((__m128i *)(image->data + i * image->components));

        for (size_t c = 0; c < num_clusters; c++)
        {
            // Chargement des composants du centre
            __m128i dest = _mm_loadu_si128((__m128i *)(centers + c * image->components));

            float dist = sqrt(distance_avx2(src, dest, image->components));

            if (dist < min_dist)
            {
                min_dist = dist;
                best_cluster = c;
            }
        }

        assignments[i] = best_cluster;
    }
#endif

#ifndef VECTORIZE
    uint8_t *dest = malloc(image->components * sizeof(uint8_t));
    uint8_t *src = malloc(image->components * sizeof(uint8_t));

    // Assign each pixel in the image to its nearest cluster.
    for (int i = 0; i < image->width * image->height; ++i)
    {
        float min_dist = INFINITY;
        int best_cluster = -1;

        memcpy(src, image->data + i * image->components, image->components * sizeof(uint8_t));

        for (int c = 0; c < num_clusters; c++)
        {
            memcpy(dest, centers + c * image->components, image->components * sizeof(uint8_t));

            float dist = distance(src, dest);

            if (dist < min_dist)
            {
                min_dist = dist;
                best_cluster = c;
            }

            assignments[i] = best_cluster;
        }
    }

    free(src);
    free(dest);
#endif

    ClusterData *cluster_data = (ClusterData *)calloc(num_clusters, sizeof(ClusterData));

    // Compute the sum of the pixel values for each cluster.
    for (int i = 0; i < image->width * image->height; ++i)
    {
        int cluster = assignments[i];
        cluster_data[cluster].count++;
        cluster_data[cluster].sum_r += (int)image->data[i * image->components + R_OFFSET];
        cluster_data[cluster].sum_g += (int)image->data[i * image->components + G_OFFSET];
        cluster_data[cluster].sum_b += (int)image->data[i * image->components + B_OFFSET];
    }

    // Update cluster centers based on the computed sums
    for (int c = 0; c < num_clusters; ++c)
    {
        if (cluster_data[c].count > 0)
        {
            centers[c * image->components + R_OFFSET] = cluster_data[c].sum_r / cluster_data[c].count;
            centers[c * image->components + G_OFFSET] = cluster_data[c].sum_g / cluster_data[c].count;
            centers[c * image->components + B_OFFSET] = cluster_data[c].sum_b / cluster_data[c].count;
        }
    }

    free(cluster_data);

    // Update image data with the cluster centers
    for (int i = 0; i < image->width * image->height; ++i)
    {
        int cluster = assignments[i];
        image->data[i * image->components + R_OFFSET] = centers[cluster * image->components + R_OFFSET];
        image->data[i * image->components + G_OFFSET] = centers[cluster * image->components + G_OFFSET];
        image->data[i * image->components + B_OFFSET] = centers[cluster * image->components + B_OFFSET];
    }

    free(assignments);
    free(centers);
}