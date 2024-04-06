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
#include "modules/de1soc.h"
int __auto_semihosting;
extern void disable_A9_interrupts(void);
extern void set_A9_IRQ_stack(void);
extern void config_GIC(void);
extern void enable_A9_interrupts(void);

static uint8_t key_pressed = 0;
static uint8_t error = 0;
static uint8_t hex = 0;



void setup()
{
   
    /* Base set up*/
    write_leds(OFF);
    for (int i = 0; i < HEX_COUNT; i++)
        write_7seg_hex(i, OFF);
}

void clear_key_pressed(int key_pressed)
{
    if (error == 1)
    {
        write_leds(0x0);
        error = 0;
    }
    ITF_REG(OFST_KEYS + EDGE_CAP) = key_pressed;
}

void decimal_write_7segs_hex3_0(int val)
{
    int hex = 0;
    int i = 0;
    while (val > 0)
    {
        hex = val % 16;
        write_7seg_hex(i, hex);
        val = val / 16;
        i++;
    }
}
    

int main(void)
{

    setup();
    
    key_pressed = 0;
    while (1)
    {

        // read the key pressed
        key_pressed = ITF_REG(OFST_KEYS + EDGE_CAP);

        // write val in decimal

        if (key_pressed & KEY_0)
        {
            hex = get_switches();
            clear_key_pressed(KEY_0);
        }
        if (key_pressed & KEY_1)
        {
            clear_key_pressed(KEY_1);
            if (hex == 1023){
                write_leds(0x3FF);
                error = 1;
                continue;
            }
            hex++;
        }
        if (key_pressed & KEY_2)
        {
            clear_key_pressed(KEY_2);
            if (hex == 0){
                write_leds(0x3FF);
                error = 1;
                continue;
            }
            hex--;
        }
        if (key_pressed & KEY_3)
        {
            clear_key_pressed(KEY_3);
            hex = 0;
        }

        decimal_write_7segs_hex3_0(hex);

    }
}
