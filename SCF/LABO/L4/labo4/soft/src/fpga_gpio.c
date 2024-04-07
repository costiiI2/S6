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

#define CST 0xBADB100D

static uint8_t key_pressed = 0;
static uint8_t error = 0;
static uint8_t hex = 0;



void setup()
{
    /* read CST */
    if (ITF_REG(0x0) != CST)
    {
      // break;
      error = 1;
    }
    /* test read write */
    ITF_REG(0x4) = CST;
    if (ITF_REG(0x4) != CST)
    {
      // break;
      error = 1;
    }

    if (error == 1)
    {
       return;
    }
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
    ITF_REG(EDGE_CAP) = key_pressed;
}

void decimal_write_7segs_hex3_0(int val)
{
    int val_save = val;
    int hex = 0;
    int i = 0;
    while (val_save > 0)
    {
        hex = val_save % 16;
        write_7seg_hex(i, hex);
        val_save = val_save / 16;
        i++;
    }
}
    

int main(void)
{

    setup();
    if (error == 1)
    {
        
        return 1;
    }
          

    key_pressed = 0;
    while (1)
    {
        // read the key pressed
        key_pressed = ITF_REG(EDGE_CAP);

        // write val in decimal

        if (key_pressed & KEY_0)
        {
            hex = get_switches();
            write_leds(0x1) ;
            clear_key_pressed(KEY_0);
        }
        if (key_pressed & KEY_1)
        {
            clear_key_pressed(KEY_1);
            /*if (hex == 1023){
                write_leds(0x3FF);
                error = 1;
                continue;
            }*/
            write_leds(0x2) ;
            hex++;
        }
        if (key_pressed & KEY_2)
        {
            clear_key_pressed(KEY_2);
           /* if (hex == 0){
                write_leds(0x3FF);
                error = 1;
                continue;
            }*/
            write_leds(0x4) ;
                        hex--;
        }
        if (key_pressed & KEY_3)
        {
            clear_key_pressed(KEY_3);
            hex = 0;
            write_leds(0x8) ;
        }

        //decimal_write_7segs_hex3_0(hex);

    }
}
