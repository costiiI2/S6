

#include <stdio.h>
#include <stdlib.h>
#include <likwid-marker.h>


#define SIZE 1000000

float sum_non_vectorized(float *array, size_t size) {
    float sum = 0.0f;
    for (size_t i = 0; i < size; i++) {
        sum += array[i];
    }
    return sum;
}

int main() {
    float *array = (float *)malloc(SIZE * sizeof(float));
    
    // Remplir le tableau avec des 1.0f
    for (size_t i = 0; i < SIZE; i++) {
        array[i] = 1.0f;
    }



    LIKWID_MARKER_INIT;
    // Calculer la somme des valeurs du tableau
    LIKWID_MARKER_START("sum");
    float sum = sum_non_vectorized(array, SIZE);
    LIKWID_MARKER_STOP("sum");
    LIKWID_MARKER_CLOSE;
    printf("Somme non vectorisée : %f\n", sum);

    // Libérer la mémoire allouée
    free(array);
    return 0;
}
