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

#define KERNEL_0_2_OFFSET 0x4
#define KERNEL_3_5_OFFSET 0x8
#define KERNEL_6_8_OFFSET 0xC

#define IMG_OFFSET 0x10

#define RETURN_OFFSET 0x1C

#define READ_WRITE_OFFSET 0x20

#define READ_BIT 0x2
#define WRITE_BIT 0x1

#define CST 0xBADB100D

#define KERNEL_SIZE 9

#define PNG_STRIDE_IN_BYTES 0

#define COMPONENT_RGBA 4
#define COMPONENT_RGB 3
#define COMPONENT_GRAYSCALE 1

static int fd;

void write_register(uint32_t off, uint32_t value)
{

    size_t length = _SC_PAGE_SIZE,
           mapped_length;
    off_t offset = AXI_LW_HPS_FPGA_BASE_ADD,
          pge_offset;

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
}

uint32_t read_register(uint32_t off)
{

    size_t length = _SC_PAGE_SIZE,
           mapped_length;
    off_t offset = AXI_LW_HPS_FPGA_BASE_ADD,
          pge_offset;

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

    return value;
}

// 1 2 1
// 2 4 2
// 1 2 1
int image[5][5] = {
    {0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0},
    {0, 1, 0, 1, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0}};

int convolved_image[5][5] = {
    {0, 0, 0, 0, 0},
    {0, 8, 9, 8, 0},
    {0, 9, 7, 9, 0},
    {0, 8, 9, 8, 0},
    {0, 0, 0, 0, 0}};

const uint8_t kernel[9] = {1, 2, 1, 2, 4, 2, 1, 2, 1};

int can_read()
{
    uint32_t reg = read_register(READ_WRITE_OFFSET);
    printf("img_full: %d, out_empty: %d, process_full: %d, process_empty: %d, out_full: %d, img_empty: %d\n",
           !(reg >> 0 & 0x1), !(reg >> 1 & 0x1), reg >> 2 & 0x1, reg >> 3 & 0x1, reg >> 4 & 0x1, reg >> 5 & 0x1);
    return read_register(READ_WRITE_OFFSET) & READ_BIT;
}

int can_write()
{
    return read_register(READ_WRITE_OFFSET) & WRITE_BIT;
}

void convolute_test()
{
    int timeout = 10;
    int img[5][5] = {0};
    int i_read = 1;
    int j_read = 1;
    can_read();
    for (int i = 1; i < (5- 1); i++)
    {
        for (int j = 1; j < (5 - 1); j++)
        {
            uint8_t pixels[9] = {0};
            int pixel_count = 0;
            int matrix_count = 0;
            int a = 0;
            for (int k = 0; 1;)
            {
                if (pixel_count < 3 || k == 9)
                {
                    int tmp_i = i + (k / 3 - 1);
                    int tmp_j = j + (k % 3 - 1);
                    pixels[k] = image[tmp_j][tmp_i];
                    pixel_count++;
                    matrix_count++;
                    k++;
                }

                if (can_write() && (pixel_count >= 3 || k == 9))
                {
                    // If we have 4 or more pixels, or we have collected all 9 pixels, send them
                    uint32_t pixel_data = 0;
                    for (int p = 0; p < pixel_count; p++)
                    {
                        pixel_data |= pixels[a++] << (24 - (8 * p));
                    }
                   
                    printf("%d %d %d\n", (pixel_data >> 24) & 0xFF, (pixel_data >> 16) & 0xFF, (pixel_data >> 8) & 0xFF);
                    
                    pixel_count = 0; // Reset the count for the next batch
                    write_register(IMG_OFFSET, pixel_data);
                }
            }

            if (can_read())
            {
                int result = read_register(RETURN_OFFSET);
                printf("Read %d %d = %d  \n", j_read, i_read, result);
                img[j_read][i_read++] = result;
                if (i_read == 5 - 1)
                {
                    i_read = 1;
                    j_read++;
                }
            }
        }
    }

    while (j_read < (5 - 1) || timeout > 0)
    {
        if (can_read())
        {
            int result = read_register(RETURN_OFFSET);
            printf("Read %d %d = %d after \n", j_read, i_read, result);
            img[j_read][i_read++] = result;
            if (i_read == 5 - 1)
            {
                i_read = 1;
                j_read++;
            }
            if(j_read == 5 - 1)
            {
                break;
            }
        }
        else
        {
            timeout--;
        }
    }

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            printf("%02d ", img[i][j]);
        }
        printf("\n");
    }
    can_read();
}

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
    printf("Image width: %d\n", img->width);
    printf("Image height: %d\n", img->height);
    // Loop over all color channels
    for (int c = 0; c < 3; c++)
    {
        printf("Convoluting channel %d\n", c);
        // Copy top and bottom edges
        printf("Copying edges\n");
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
        printf("Starting convolution\n");

        // Start convolution
        int i_read = 1;
        int j_read = 1;
        for (j = 1; j < img->height - 1;)
        {
            printf("Convoluting line %d\n", j);
            for (i = 1; i < img->width - 1;)
            {
                // Load all 9 pixels to IP
                for (k = 0; k < 9;)
                {
                    // If a write is ready, store a pixel else loop
                    if (can_write())
                    {
                        printf(".");
                        write_register(IMG_OFFSET, (uint32_t) & (*channels[c])[(j + (k / 3 - 1)) * img->width + (i + (k % 3 - 1))]);
                        k++;
                    }
                }

                // If a result is ready, store it else continue
                if (can_read())
                {
                    int result = read_register(RETURN_OFFSET);
                    printf("Reading data at %d %d = %d before \n", i_read, j_read, result);
                    (*result_channels[c])[j_read * img->width + i_read++] = result;
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
            if (can_read())
            {
                int result = read_register(RETURN_OFFSET);
                printf("Reading data at %d %d = %d after \n", i_read, j_read, result);
                (*result_channels[c])[j_read * img->width + i_read++] = result;
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

void set_kernel()
{
    // kernel in vhdl is
    // 1 2 1 2
    // 4 2 1 2
    // 1 0 0 0
    // const uint8_t kernel[9] = {1, 2, 1, 2, 4, 2, 1, 2, 1};

    write_register(KERNEL_0_2_OFFSET, kernel[0] << 24 | kernel[1] << 16 | kernel[2] << 8);
    write_register(KERNEL_3_5_OFFSET, kernel[3] << 24 | kernel[4] << 16 | kernel[5] << 8 );
    write_register(KERNEL_6_8_OFFSET, kernel[6] << 24 | kernel[7] << 16 | kernel[8] << 8);
}

void read_kernel()
{
    int k_0_3 = read_register(KERNEL_0_2_OFFSET);
    int k_4_7 = read_register(KERNEL_3_5_OFFSET);
    int k_8 = read_register(KERNEL_6_8_OFFSET);
    printf("\n%d %d %d\n%d %d %d\n%d %d %d\n", (k_0_3 >> 24) & 0xFF, (k_0_3 >> 16) & 0xFF, (k_0_3 >> 8) & 0xFF, (k_4_7 >> 24) & 0xFF, (k_4_7 >> 16) & 0xFF, (k_4_7 >> 8) & 0xFF, (k_8 >> 24) & 0xFF, (k_8 >> 16) & 0xFF, (k_8 >> 8) & 0xFF);
}


static int error = 0;

void print_img(struct img_1D_t *img)
{
    int i, j;
    for (j = 0; j < img->height; j++)
    {
        for (i = 0; i < img->width; i++)
        {
            printf("%d ", img->r[j * img->width + i]);
        }
        printf("\n");
    }
}

void setup()
{
    printf("Setting up FPGA\n");
    uint32_t cst = read_register(CNST);
    /* read CST */
    if (cst != CST)
    {
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
        printf("Please provide an input image and an output image \n");

        return EXIT_FAILURE;
    }

    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
    {
        printf("Couldn't open /dev/mem... Program abort!\n");
        return EXIT_FAILURE;
    }
    setup();
    if (error)
    {
        return EXIT_FAILURE;
    }
    struct img_1D_t *img_1d;
    struct img_1D_t *result_img;
    printf("Setting kernel\n");
    set_kernel();

    // read kernel values
    printf("Kernel values: ");
    read_kernel();

    convolute_test();
    close(fd);
    return 0;


    printf("Loading image\n");
    img_1d = load_image_1D(argv[1]);
    printf("Convoluting image\n");
    result_img = convolution_1D(img_1d);
    printf("Saving image\n");
    save_image(argv[2], result_img);

    printf("Printing image\n");
    print_img(img_1d);
    printf("Printing result\n");
    print_img(result_img);

    free_image(img_1d);
    free_image(result_img);

    printf("Convoluting test\n");
    convolute_test();
    close(fd);
}
