/*****************************************************************************************
 *
 * File                 : fpga_gpio.c
 * Author               : Cecchet Costantino
 * Date                 : 01.03.2024
 *
 * Context              : Lab09
 *
 *****************************************************************************************
 * Brief: laboratoire 3
 *
 *****************************************************************************************
 * Modifications :
 * Ver    Date        Student      Comments
 * 1.0	  01.06.24    CCO	       First version Labo 9
 *
 *****************************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "axi_lw.h"
#include "image.h"

int __auto_semihosting;

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

#define ITF_REG(_x_) *(volatile uint32_t *)((AXI_LW_HPS_FPGA_BASE_ADD) + _x_)

#define CNST 0x0

#define KERNEL_0_3_OFFSET 0x4
#define KERNEL_4_7_OFFSET 0x8
#define KERNEL_8_OFFSET 0xC

#define IMG_OFFSET 0x10

#define RETURN_OFFSET 0x1C

#define READ_WRITE_OFFSET 0x20

#define READ_BIT 0x1
#define WRITE_BIT 0x2

#define CST 0xBADB100D

#define KERNEL_SIZE 9

#define PNG_STRIDE_IN_BYTES 0

#define COMPONENT_RGBA 4
#define COMPONENT_RGB 3
#define COMPONENT_GRAYSCALE 1

void write_register(uint32_t off, uint32_t value)
{

    int fd;

    size_t length = _SC_PAGE_SIZE,
           mapped_length;
    off_t offset = AXI_LW_HPS_FPGA_BASE_ADD,
          pge_offset;

    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
    {
        printf("Couldn't open /dev/mem... Program abort!\n");
        return;
    }

    /* mmap()'s offset must be page aligned */
    pge_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);

    /* Real length considers the offset and lower page difference */
    mapped_length = length + offset - pge_offset;

    void *base = mmap(NULL, mapped_length,
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED, fd, pge_offset);

    volatile uint32_t *reg = base + off;
    *reg = value;

    munmap(base, MAP_SIZE);
    close(fd);
}

uint32_t read_register(uint32_t off)
{
    int fd;

    size_t length = _SC_PAGE_SIZE,
           mapped_length;
    off_t offset = AXI_LW_HPS_FPGA_BASE_ADD,
          pge_offset;

    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
    {
        printf("Couldn't open /dev/mem... Program abort!\n");
        return 0;
    }
 

    /* mmap()'s offset must be page aligned */
    pge_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);

    /* Real length considers the offset and lower page difference */
    mapped_length = length + offset - pge_offset;
    
    void *base = mmap(NULL, mapped_length,
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED, fd, pge_offset);

    volatile uint32_t *reg = base + off;
    uint32_t value = *reg;


    munmap(base, MAP_SIZE);
    close(fd);

    return value;
}

const uint8_t kernel[KERNEL_SIZE] = {
    1,
    2,
    1,
    2,
    4,
    2,
    1,
    2,
    1,
};


struct img_1D_t *convolution_1D(struct img_1D_t *img)
{
    struct img_1D_t *result_img;
    int i, j, k;
    int width = img->width;
    int height = img->height;
    int components = img->components;

    result_img = allocate_image_1D(width, height, components);

    // Define a pointer to the color channels
    uint8_t **channels[3] = {&img->r, &img->g, &img->b};
    uint8_t **result_channels[3] = {&result_img->r, &result_img->g, &result_img->b};

    // Loop over all color channels
    for (int c = 0; c < 3; c++)
    {
        // Copy top and bottom edges
        for (i = 0; i < img->width; i++)
        {
            (*result_channels[c])[i] = (*channels[c])[i];                                                                   // Top edge
            (*result_channels[c])[(img->height - 1) * img->width + i] = (*channels[c])[(img->height - 1) * img->width + i]; // Bottom edge
        }

        // Copy left and right edges
        for (j = 0; j < img->height; j++)
        {
            (*result_channels[c])[j * img->width] = (*channels[c])[j * img->width];                                   // Left edge
            (*result_channels[c])[j * img->width + img->width - 1] = (*channels[c])[j * img->width + img->width - 1]; // Right edge
        }

        // Start convolution
        int i_read = 1;
        int j_read = 1;
        for (j = 1; j < img->height - 1;)
        {
            for (i = 1; i < img->width - 1;)
            {
                // Load all 9 pixels to IP
                for (k = 0; k < 9;)
                {
                    // If a write is ready, store a pixel else loop
                    if (read_register(READ_WRITE_OFFSET) & WRITE_BIT)
                    {
                        write_register(IMG_OFFSET, (uint32_t) & (*channels[c])[(j + (k / 3 - 1)) * img->width + (i + (k % 3 - 1))]);
                        k++;
                    }
                }

                // If a result is ready, store it else continue
                if (read_register(READ_WRITE_OFFSET) & READ_BIT)
                {
                    (*result_channels[c])[j_read * img->width + i_read++] = read_register(RETURN_OFFSET);
                    if (i_read == img->width - 1)
                    {
                        i_read = 1;
                        j_read++;
                    }
                }
                i++;
            }
            j++;
        }

        // Wait for all data to be read
        while (j_read < img->height - 1)
        {
            if (read_register(READ_WRITE_OFFSET) & READ_BIT)
            {
                (*result_channels[c])[j_read * img->width + i_read++] = read_register(RETURN_OFFSET);
                if (i_read == img->width - 1)
                {
                    i_read = 1;
                    j_read++;
                }
            }
        }
    }

    return result_img;
}

void set_kernel_1D()
{
    write_register(KERNEL_0_3_OFFSET, kernel[0] << 24 | kernel[1] << 16 | kernel[2] << 8 | kernel[3]);
    write_register(KERNEL_4_7_OFFSET, kernel[4] << 24 | kernel[5] << 16 | kernel[6] << 8 | kernel[7]);
    write_register(KERNEL_8_OFFSET, kernel[8]);
}

void print_usage()
{
    printf("Please provide an input image and an output image \n");
}
static int error = 0;

void setup()
{
    printf("Setting up FPGA\n");
    uint32_t cst = read_register(CNST);
    /* read CST */
    if (cst!= CST)  {
        printf("Error: CST not found\n");
        printf("Expected: %x\n", CST);
        printf("Received: %x\n", cst);

        error = 1;
    }
}


int main(int argc, char **argv)
{

    if (argc != 3)
    {
        fprintf(stderr, "Invalid number of arguments\n");
        print_usage();
        return EXIT_FAILURE;
    }

    printf("Welcome to the FPGA Convolution Lab\n");
    setup();
    if (error)
    {
        return EXIT_FAILURE;
    }
    struct img_1D_t *img_1d;
    struct img_1D_t *result_img;
    printf("Setting kernel\n");
    set_kernel_1D();

    // read kernel values
    printf("Kernel values: ");
    int k_0_3 = read_register(KERNEL_0_3_OFFSET);
    int k_4_7 = read_register(KERNEL_4_7_OFFSET);
    int k_8 = read_register(KERNEL_8_OFFSET);
    printf("\n%d %d %d\n%d %d %d\n%d %d %d\n", (k_0_3 >> 24) & 0xFF, (k_0_3 >> 16) & 0xFF, (k_0_3 >> 8) & 0xFF, (k_0_3)&0xFF, (k_4_7 >> 24) & 0xFF, (k_4_7 >> 16) & 0xFF, (k_4_7 >> 8) & 0xFF, (k_4_7)&0xFF, (k_8)&0xFF);

    printf("Loading image\n");
    img_1d = load_image_1D(argv[1]);
    printf("Convoluting image\n");
    result_img = convolution_1D(img_1d);
    printf("Saving image\n");
    save_image(argv[2], result_img);
}
