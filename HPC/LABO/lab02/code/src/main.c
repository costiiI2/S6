#include <stdio.h>
#include <stdlib.h>
#include <likwid-marker.h>

#include "image.h"
#include "sobel.h"

#define EXPECTED_NB_ARGS 4

void print_usage()
{
    printf("Please provide an input image and an output image and finally an indication of the mode you want to use (1 for 1D tab or 2 for 2D tab)\n");
}
/*
LIKWID_MARKER_INIT;
[...]
LIKWID_MARKER_START("name");
[...] <- code à analyser
LIKWID_MARKER_STOP("name");
[...]
LIKWID_MARKER_CLOSE
*/
int main(int argc, char **argv)
{
    struct img_1D_t *img_1d;
    struct img_chained_t *img_chained;
    struct img_1D_t *result_img;
    struct img_chained_t *result_img_chained;

    int mode;

    if (argc != EXPECTED_NB_ARGS)
    {
        fprintf(stderr, "Invalid number of arguments\n");
        print_usage();
        return EXIT_FAILURE;
    }

    mode = atoi(argv[3]);
    LIKWID_MARKER_INIT;
    
    if (mode == 1)
    {
        img_1d = load_image_1D(argv[1]);
        LIKWID_MARKER_START("name");
        result_img = edge_detection_1D(img_1d);
        LIKWID_MARKER_STOP("name");
        save_image(argv[2], result_img);
    }
    else if (mode == 2)
    {
        img_chained = load_image_chained(argv[1]);
        LIKWID_MARKER_START("name");
        result_img_chained = edge_detection_chained(img_chained);
        LIKWID_MARKER_STOP("name");
        save_image_chained(argv[2], result_img_chained);
    }
    
    LIKWID_MARKER_CLOSE;
    return 0;
}