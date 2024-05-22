#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h> 

#define SIZE 1000000

float sum_vectorized(float *array, size_t size) {
    __m256 sum_vec = _mm256_setzero_ps(); 
    size_t i;

    for (i = 0; i <= size - 8; i += 8) {
        __m256 vec = _mm256_loadu_ps(&array[i]); 
        sum_vec = _mm256_add_ps(sum_vec, vec);   
    }

    float sum_array[8];
    _mm256_storeu_ps(sum_array, sum_vec);

    float sum = 0.0f;
    for (int j = 0; j < 8; j++) {
        sum += sum_array[j];
    }

    for (; i < size; i++) {
        sum += array[i];
    }

    return sum;
}

int main() {
    float *array = (float *)malloc(SIZE * sizeof(float));
    
    for (size_t i = 0; i < SIZE; i++) {
        array[i] = 1.0f;
    }

    float sum = sum_vectorized(array, SIZE);
    printf("Somme vectorisée : %f\n", sum);

    free(array);
    return 0;
}
