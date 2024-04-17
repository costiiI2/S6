#include "k-means.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __SSE__
#include <xmmintrin.h>
#else
#error "SSE is not supported on this platform"
#endif

// This function will calculate the euclidean distance between two pixels.
// Instead of using coordinates, we use the RGB value for evaluating distance.
float distance(uint8_t *p1, uint8_t *p2)
{
    // Load RGB values into SSE registers
    __m128 v1 = _mm_set_ps(0, p1[2], p1[1], p1[0]);
    __m128 v2 = _mm_set_ps(0, p2[2], p2[1], p2[0]);

    // Subtract the two vectors
    __m128 diff = _mm_sub_ps(v1, v2);

    // Square the differences
    __m128 square_diff = _mm_mul_ps(diff, diff);

    // Sum the squares
    __m128 sum = _mm_add_ps(square_diff, _mm_movehl_ps(square_diff, square_diff));
    sum = _mm_add_ss(sum, _mm_shuffle_ps(sum, sum, 1));

    // Take the square root of the sum
    __m128 result = _mm_sqrt_ss(sum);

    // Store the result back into a regular float variable
    float distance;
    _mm_store_ss(&distance, result);

    return distance;
}

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

    // Calculate distances from each pixel to the first center
    __m128 center = _mm_set_ps(0, centers[R_OFFSET], centers[G_OFFSET], centers[B_OFFSET]);

    for (int i = 0; i < image->width * image->height; ++i)
    {
        __m128 pixel = _mm_set_ps(0, image->data[i * image->components + R_OFFSET],
                                  image->data[i * image->components + G_OFFSET],
                                  image->data[i * image->components + B_OFFSET]);

        // Calculate the distance between the pixel and the center
        __m128 diff = _mm_sub_ps(pixel, center);
        __m128 square_diff = _mm_mul_ps(diff, diff);
        __m128 sum = _mm_add_ps(square_diff, _mm_movehl_ps(square_diff, square_diff));
        sum = _mm_add_ss(sum, _mm_shuffle_ps(sum, sum, 1));
        __m128 result = _mm_sqrt_ss(sum);

        _mm_store_ss(&distances[i], result);
    }

    // Loop to find remaining cluster centers
    for (int i = 1; i < num_clusters; ++i)
    {
        float total_weight = 0.0;

        // Calculate total weight (sum of distances)
        for (int j = 0; j < image->width * image->height; ++j)
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
        // Update distances based on the new center
        center = _mm_set_ps(0, centers[i * image->components + R_OFFSET],
                            centers[i * image->components + G_OFFSET],
                            centers[i * image->components + B_OFFSET]);

        for (int j = 0; j < image->width * image->height; j++)
        {
            __m128 pixel = _mm_set_ps(0, image->data[j * image->components + R_OFFSET],
                                      image->data[j * image->components + G_OFFSET],
                                      image->data[j * image->components + B_OFFSET]);

            __m128 diff = _mm_sub_ps(pixel, center);
            __m128 square_diff = _mm_mul_ps(diff, diff);
            __m128 sum = _mm_add_ps(square_diff, _mm_movehl_ps(square_diff, square_diff));
            sum = _mm_add_ss(sum, _mm_shuffle_ps(sum, sum, 1));
            __m128 result = _mm_sqrt_ss(sum);

            float dist;
            _mm_store_ss(&dist, result);

            if (dist < distances[j])
            {
                distances[j] = dist;
            }
        }
    }
    free(distances);
}

void kmeans(struct img_1D_t *image, int num_clusters)
{
    uint8_t *centers = calloc(num_clusters * image->components, sizeof(uint8_t));

    // Initialize the cluster centers using the k-means++ algorithm.
    kmeans_pp(image, num_clusters, centers);

    int *assignments = (int *)calloc(image->width * image->height, sizeof(int));

    // Assign each pixel in the image to its nearest cluster.
    for (int i = 0; i < image->width * image->height; ++i)
    {
        float min_dist = INFINITY;
        int best_cluster = -1;

        __m128 src = _mm_set_ps(0, image->data[i * image->components + R_OFFSET],
                                image->data[i * image->components + G_OFFSET],
                                image->data[i * image->components + B_OFFSET]);

        for (int c = 0; c < num_clusters; c++)
        {
            __m128 dest = _mm_set_ps(0, centers[c * image->components + R_OFFSET],
                                     centers[c * image->components + G_OFFSET],
                                     centers[c * image->components + B_OFFSET]);

            __m128 diff = _mm_sub_ps(src, dest);
            __m128 square_diff = _mm_mul_ps(diff, diff);
            __m128 sum = _mm_add_ps(square_diff, _mm_movehl_ps(square_diff, square_diff));
            sum = _mm_add_ss(sum, _mm_shuffle_ps(sum, sum, 1));
            __m128 result = _mm_sqrt_ss(sum);

            float dist;
            _mm_store_ss(&dist, result);

            if (dist < min_dist)
            {
                min_dist = dist;
                best_cluster = c;
            }

            assignments[i] = best_cluster;
        }
    }

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
            __m128 sum = _mm_set_ps(0, cluster_data[c].sum_r, cluster_data[c].sum_g, cluster_data[c].sum_b);
            __m128 count = _mm_set1_ps(cluster_data[c].count);
            __m128 center = _mm_div_ps(sum, count);

            centers[c * image->components + R_OFFSET] = (uint8_t)_mm_cvtss_f32(_mm_shuffle_ps(center, center, _MM_SHUFFLE(0, 0, 0, 0)));

            centers[c * image->components + G_OFFSET] = (uint8_t)_mm_cvtss_f32(_mm_shuffle_ps(center, center, _MM_SHUFFLE(0, 0, 0, 1)));

            centers[c * image->components + B_OFFSET] = (uint8_t)_mm_cvtss_f32(_mm_shuffle_ps(center, center, _MM_SHUFFLE(0, 0, 0, 2)));
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