/*****************************************************************************************
 * HEIG-VD
 * Haute Ecole d'Ingenerie et de Gestion du Canton de Vaud
 * School of Business and Engineering in Canton de Vaud
 *****************************************************************************************
 * REDS Institute
 * Reconfigurable Embedded Digital Systems
 *****************************************************************************************
 *
 * File                 : fpga_gpio.c
 * Author               : Cecchet Costantino
 * Date                 : 01.03.2024
 *
 * Context              : SOCF tutorial lab
 *
 *****************************************************************************************
 * Brief: laboratoire 3
 *
 *****************************************************************************************
 * Modifications :
 * Ver    Date        Student      Comments
 * 1.0	  01.03.24    CCO	       First version Labo 2
 * 1.1    24.03.24    CCO          modified for SCF L-3
 *
 *****************************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include "axi_lw.h"
#include "modules/alfanum.h"
int __auto_semihosting;

#define CNST 0x0
#define TEST_REG 0x4
#define KEYS 0x8 // not used for this program
#define EDGE_CAP 0xC
#define SWITCHES 0x10
#define LED_OFFSET 0x14
#define LED_SET 0x18 // not used for this program
#define LED_CLR 0x1C // not used for this program
#define HEX3_0_OFFSET 0x20
#define HEX5_4_OFFSET 0x24

#define LED_ON 0x3FF
#define LED_OFF 0x0

#define KEY_0			(1 << 0)
#define KEY_1			(1 << 1)
#define KEY_2			(1 << 2)
#define KEY_3			(1 << 3)

#define MIN_VAL 0
#define MAX_VAL 1023

#define CST 0xbadb100d

#define ITF_REG(_x_)	*(volatile uint32_t *)							\
						((AXI_LW_HPS_FPGA_BASE_ADD) + _x_)


                        
static uint8_t key_pressed = 0;
static uint8_t error = 0;
static uint16_t hex = 0;

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

void write_register(uint32_t off, uint32_t value)
{
 
    int fd;

        size_t length = _SC_PAGE_SIZE,
           mapped_length;
    off_t offset = AXI_LW_HPS_FPGA_BASE_ADD,
          pge_offset;

    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) {
        printf("Couldn't open /dev/mem... Program abort!\n");
        return;
    }

    /* mmap()'s offset must be page aligned */
    pge_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);

    /* Real length considers the offset and lower page difference */
    mapped_length = length + offset - pge_offset;

    void* base = mmap(NULL, mapped_length,    \
                           PROT_READ | PROT_WRITE,    \
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

    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) {
        printf("Couldn't open /dev/mem... Program abort!\n");
        return 0;
    }

    /* mmap()'s offset must be page aligned */
    pge_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);

    /* Real length considers the offset and lower page difference */
    mapped_length = length + offset - pge_offset;

    void* base = mmap(NULL, mapped_length,    \
                           PROT_READ | PROT_WRITE,    \
                           MAP_SHARED, fd, pge_offset);

    volatile uint32_t *reg = base + off;
    uint32_t value = *reg;

    munmap(base, MAP_SIZE);
    close(fd);

    return value;
}

void write_leds(uint32_t maskled)
{
    write_register(LED_OFFSET, maskled);

}

uint32_t read_switches()
{
    uint32_t switches = read_register(SWITCHES);
    
    return switches;
}

void setup()
{
    /* read CST */
    uint32_t cst = read_register(CNST);
    printf("CST: %x,%x\n", cst, CST);
    if (cst != CST)
    {
        printf("Error while reading CST\n");
        error = 1;
        return;
    }
    
    /* test read write */
    write_register(TEST_REG, CST);
    uint32_t test = read_register(TEST_REG);
    printf("TEST_REG: %x\n", test);
    if (test != CST)
    {
        printf("Error while reading TEST_REG\n");
      error = 1;
      return;
    }

    
    /* Base set up*/
    write_leds(LED_OFF);
}

void clear_key_pressed(int key_pressed)
{
    /* clear error */
    if (error == 1)
    {
        write_leds(LED_OFF);
        error = 0;
    }
    /* clear key */
    write_register(EDGE_CAP, key_pressed);
}

void decimal_write_hex(int val)
{
   
    uint32_t hex0_3 = 0;
    uint32_t hex4_5 = 0;
   
    for (int i = 0; i < 6; i++)
{
    if (i < 4)
    {
        hex0_3 = hex0_3 | (hexa[val % 10] << (i * 7));
    }
    else
    {
        hex4_5 = hex4_5 | (hexa[val % 10] << ((i - 4) * 7));
    }
    val = val / 10;
}
 
    write_register(HEX3_0_OFFSET, ~hex0_3);
    write_register(HEX5_4_OFFSET, ~hex4_5);
}
    

int main(void)
{
    printf("Start\n");
    setup();
    /* check if error while initializing */
    if (error == 1){
        printf("Error while initializing\n");
        return 1;
    }
    printf("Press KEY0 to read switches\n");
    
    while (1)
    {
        key_pressed = read_register(EDGE_CAP);

        if (key_pressed & KEY_0)
        {
            clear_key_pressed(KEY_0);
            hex = read_switches();
        }
        if (key_pressed & KEY_1)
        {
            clear_key_pressed(KEY_1);
            if (hex == MIN_VAL){
                write_leds(LED_ON);
                error = 1;
                continue;
            }
            hex--;
        }
        if (key_pressed & KEY_2)
        {
            clear_key_pressed(KEY_2);
            if (hex == MAX_VAL){
                write_leds(LED_ON);
                error = 1;
                continue;
            }
                        hex++;
        }
        if (key_pressed & KEY_3)
        {
            clear_key_pressed(KEY_3);
            hex = 0;
        }

        decimal_write_hex(hex);

    }
}
