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
#include <stdlib.h>
#include <string.h>

#include "axi_lw.h"
#include "image.h"

int __auto_semihosting;

#define DRIVER_AVAILABLE 1

#define DEBUG_PRINT 1

#if DRIVER_AVAILABLE
#include <sys/ioctl.h>

#define DRIVER_NAME "/dev/de1_io"
#define WR_VALUE _IOW('a', 'a', int32_t *)
#else
static void *base;
#endif
static int fd;

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

#define ITF_REG(_x_) *(volatile uint32_t *)((AXI_LW_HPS_FPGA_BASE_ADD) + _x_)

#define CNST 0x0

#define KERNEL_0_2_OFFSET 0x4
#define KERNEL_3_5_OFFSET 0x8
#define KERNEL_6_8_OFFSET 0xC

#define IMG_OFFSET 0x10

#define RETURN_OFFSET 0x1C
#define SIZE_OFFSET 0x18

#define READ_WRITE_OFFSET 0x20

#define READ_BIT 0x2
#define WRITE_BIT 0x1

#define CST 0xBADB100D

#define KERNEL_WIDTH 3
#define KERNEL_HEIGHT 3
#define KERNEL_SIZE KERNEL_HEIGHT *KERNEL_WIDTH

#define PNG_STRIDE_IN_BYTES 0
#define COMPONENT_RGBA 4
#define COMPONENT_RGB 3
#define COMPONENT_GRAYSCALE 1

#define KERNEL_COUNT 5
#define KERNEL_HEIGHT 3
#define KERNEL_WIDTH 3

static uint8_t kernel[KERNEL_HEIGHT][KERNEL_WIDTH];

const uint8_t kernels[KERNEL_COUNT][KERNEL_HEIGHT][KERNEL_WIDTH] = {
    /* Identity 0*/
    {
        {0, 0, 0},
        {0, 1, 0},
        {0, 0, 0}},
    /* Edge detection 1*/
    {
        {0, 1, 0},
        {1, -4, 1},
        {0, 1, 0}},
    /* Sharpen 2*/
    {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}},
    /* Box blur 3*/
    {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1}},
    /* Gaussian blur 4*/
    {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}}};

void write_register(uint32_t index, uint32_t value)
{

#if DRIVER_AVAILABLE

    // set index
    int err = ioctl(fd, WR_VALUE, index);
    if (err < 0)
    {
        printf("Error while writing register\n");
        err = 1;
    }

    // set value
    err = write(fd, &value, sizeof(uint32_t));
    if (err < 0)
    {
        printf("Error while writing register\n");
        err = 1;
    }
#else

    volatile uint32_t *reg = base + index;
    *reg = value;
#endif
}

uint32_t read_register(uint32_t index)
{

#if DRIVER_AVAILABLE

    uint32_t value;

    // set index
    int err = ioctl(fd, WR_VALUE, index);
    if (err < 0)
    {
        printf("Error while reading register\n");
        err = 1;
    }

    // get value
    err = read(fd, &value, sizeof(uint32_t));
    if (err < 0)
    {
        printf("Error while reading register\n");
        err = 1;
    }

    return value;

#else
    volatile uint32_t *reg = base + index;
    uint32_t value = *reg;
    return value;
#endif
}

// 1 2 1
// 2 4 2
// 1 2 1

// test IMAGE 5x5
#define IMG_SIZE 5
int image[IMG_SIZE][IMG_SIZE] = {
    {0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0},
    {0, 1, 0, 1, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0}};

int can_read()
{
    uint8_t size_out = read_register(SIZE_OFFSET) >> 24 & 0xFF;
#if DEBUG_PRINT
    printf("size_out %d\n", size_out);
#endif
    return size_out;
}

int can_write()
{
    return read_register(READ_WRITE_OFFSET) & WRITE_BIT;
}

void select_kernel(uint8_t kernel_index)
{
    if (kernel_index >= KERNEL_COUNT)
    {
        printf("Invalid kernel index\n");
        return;
    }

    memcpy(kernel, kernels[kernel_index], sizeof(kernel));
}

void convolute_test()
{

    int img[IMG_SIZE][IMG_SIZE] = {0};
    int i_read = 1;
    int j_read = 1;
    for (int i = 1; i < (IMG_SIZE - 1); i++)
    {
        for (int j = 1; j < (IMG_SIZE - 1); j++)
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
#if DEBUG_PRINT
                    printf("%d %d %d\n", (pixel_data >> 24) & 0xFF, (pixel_data >> 16) & 0xFF, (pixel_data >> 8) & 0xFF);
                    if (k == 9)
                    {
                        printf("-----\n");
                    }
#endif

                    pixel_count = 0; // Reset the count for the next batch
                    write_register(IMG_OFFSET, pixel_data);
                    if (k == 9)
                    {
                        break;
                    }
                }

                while (can_read())
                {

                    int result = read_register(RETURN_OFFSET);
#if DEBUG_PRINT
                    printf("Read %d %d = %d  \n", j_read, i_read, result);
#endif
                    img[j_read][i_read++] = result;
                    if (i_read == IMG_SIZE - 1)
                    {
                        i_read = 1;
                        j_read++;
                    }
                }
            }
        }
    }
#if DEBUG_PRINT
    printf("input fifo size %d\n", read_register(SIZE_OFFSET) >> 8 & 0xFF);
    printf("output fifo size %d\n", read_register(SIZE_OFFSET) >> 24 & 0xFF);

    printf("starting last read\n");
#endif

    while (can_read())
    {

        int result = read_register(RETURN_OFFSET);
#if DEBUG_PRINT
        printf("Read %d %d = %d after \n", j_read, i_read, result);
#endif
        img[j_read][i_read++] = result;
        if (i_read == IMG_SIZE - 1)
        {
            i_read = 1;
            j_read++;
        }
    }

#if DEBUG_PRINT
    printf(" J read %d - I read %d\n", j_read, i_read);
    printf("can_read %d\n", can_read());
#endif

    for (int i = 0; i < IMG_SIZE; i++)
    {
        for (int j = 0; j < IMG_SIZE; j++)
        {
            printf("%03d ", img[i][j]);
        }
        printf("\n");
    }
}

void test_fifo()
{
    for (int i = 0; i < IMG_SIZE; i++)
    {
        printf("Writing %x\n", i);
        write_register(0x14, i);
        printf("size %x\n",
               read_register(SIZE_OFFSET));
    }
    printf("----------------\n");
    printf("size %x\n",
           read_register(SIZE_OFFSET));
    printf("----------------\n");
    for (int i = 0; i < IMG_SIZE; i++)
    {
        printf("Reading %x\n",
               read_register(0x14));
        printf("size %x\n",
               read_register(SIZE_OFFSET));
    }
}

struct img_1D_t *convolution_1D(struct img_1D_t *img)
{
    struct img_1D_t *result_img;
    int i, j;
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
        for (j = 1; j < img->height - 1; j++)
        {
            for (i = 1; i < img->width - 1; i++)
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
                        pixels[k] = (*channels[c])[tmp_j * img->width + tmp_i];
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
#if DEBUG_PRINT
                        printf("%d %d %d\n", (pixel_data >> 24) & 0xFF, (pixel_data >> 16) & 0xFF, (pixel_data >> 8) & 0xFF);
                        if (k == 9)
                        {
                            printf("-----\n");
                        }
#endif

                        pixel_count = 0; // Reset the count for the next batch
                        write_register(IMG_OFFSET, pixel_data);
                        if (k == 9)
                        {
                            break;
                        }
                    }
                }

                if (can_read())
                {

                    int result = read_register(RETURN_OFFSET);
#if DEBUG_PRINT
                    printf("Read %d %d = %d  \n", j_read, i_read, result);
#endif
                    (*result_channels[c])[j_read * img->width + i_read++] = result;
                    if (i_read == img->width - 1)
                    {
                        i_read = 1;
                        j_read++;
                    }
                }
            }
        }

        // Wait for all data to be read
        while (can_read())
        {

            int result = read_register(RETURN_OFFSET);
#if DEBUG_PRINT
            printf("Reading data at %d %d = %d after \n", i_read, j_read, result);
#endif
            (*result_channels[c])[j_read * img->width + i_read++] = result;
            if (i_read == img->width - 1)
            {
                i_read = 1;
                j_read++;
            }
        }
    }

    return result_img;
}

void set_kernel()
{

    write_register(KERNEL_0_2_OFFSET, kernel[0][0] << 24 | kernel[0][1] << 16 | kernel[0][2] << 8);
    write_register(KERNEL_3_5_OFFSET, kernel[1][0] << 24 | kernel[1][1] << 16 | kernel[1][2] << 8);
    write_register(KERNEL_6_8_OFFSET, kernel[2][0] << 24 | kernel[2][1] << 16 | kernel[2][2] << 8);
}

void read_kernel()
{
    int k_0_3 = read_register(KERNEL_0_2_OFFSET);
    int k_4_7 = read_register(KERNEL_3_5_OFFSET);
    int k_8 = read_register(KERNEL_6_8_OFFSET);
    printf("%03d %03d %03d\n", (k_0_3 >> 24) & 0xFF, (k_0_3 >> 16) & 0xFF, (k_0_3 >> 8) & 0xFF);
    printf("%03d %03d %03d\n", (k_4_7 >> 24) & 0xFF, (k_4_7 >> 16) & 0xFF, (k_4_7 >> 8) & 0xFF);
    printf("%03d %03d %03d\n", (k_8 >> 24) & 0xFF, (k_8 >> 16) & 0xFF, (k_8 >> 8) & 0xFF);
}

void print_img(struct img_1D_t *img)
{
    int i, j;
    for (j = 0; j < img->height; j++)
    {
        for (i = 0; i < img->width; i++)
        {
            printf("%03d ", img->r[j * img->width + i]);
        }
        printf("\n");
    }
}

int setup()
{
    printf("Setting up FPGA\n");
    uint32_t cst = read_register(CNST);
    /* read CST */
    if (cst != CST)
    {
        printf("Error: CST not found\n");
        printf("Expected: %x\n", CST);
        printf("Received: %x\n", cst);

        return -1;
    }
    else
    {
        printf("CST found\n");
    }

    //prepare fifo 3 elements in and 1 out
    write_register(IMG_OFFSET, 0);
    write_register(IMG_OFFSET, 0);
    write_register(IMG_OFFSET, 0);
    read_register(RETURN_OFFSET);

    return 0;
}

void convolution_image(char *img_path, char *output_path)
{
    struct img_1D_t *img_1d;
    struct img_1D_t *result_img;
    printf("Loading image\n");
    img_1d = load_image_1D(img_path);
    printf("Convoluting image\n");
    result_img = convolution_1D(img_1d);
    printf("Saving image\n");
    save_image(output_path, result_img);

#if DEBUG_PRINT
    printf("Printing image\n");
    print_img(img_1d);
    printf("Printing result\n");
    print_img(result_img);
#endif
}

int main(int argc, char **argv)
{

#if DRIVER_AVAILABLE
    if ((fd = open(DRIVER_NAME, O_RDWR | O_SYNC)) == -1)
#else
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
#endif
    {
        printf("Couldn't open /dev/mem... Program abort!\n");
        return EXIT_FAILURE;
    }

#if DRIVER_AVAILABLE == 0
    size_t length = _SC_PAGE_SIZE,
           mapped_length;
    off_t offset = AXI_LW_HPS_FPGA_BASE_ADD,
          pge_offset;

    /* mmap()'s offset must be page aligned */
    pge_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);

    /* Real length considers the offset and lower page difference */
    mapped_length = length + offset - pge_offset;

    base = mmap(NULL, mapped_length,
                PROT_READ | PROT_WRITE,
                MAP_SHARED, fd, pge_offset);
#endif

    if (setup() < 0)
    {
        return EXIT_FAILURE;
    }
    switch (argc)
    {
    case 1:
        printf("\nFifo test\n");
        test_fifo();
        break;
    case 2:
        select_kernel(atoi(argv[1]));
        set_kernel();
        printf("Test mode\n");
        convolute_test();

        break;
    case 4:
        select_kernel(atoi(argv[1]));
        set_kernel();
        printf("Image mode\n");
        convolution_image(argv[2], argv[3]);
        break;
    default:
        fprintf(stderr, "Invalid number of arguments\n");
        printf("Please provide an input image and an output image \n");
        close(fd);
        return EXIT_FAILURE;
    }

#if DRIVER_AVAILABLE == 0
    munmap(base, MAP_SIZE);
#endif
    close(fd);
    return EXIT_SUCCESS;
}