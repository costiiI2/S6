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
 * Author               :
 * Date                 :
 *
 * Context              : SOCF tutorial lab
 *
 *****************************************************************************************
 * Brief: laboratoire 2
 *
 *****************************************************************************************
 * Modifications :
 * Ver    Date        Student      Comments
 * 1.0	  01.03.24    CCO	       First version
 *
 *****************************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include "axi_lw.h"
#include "modules/alfanum.h"
#include "modules/de1soc.h"
int __auto_semihosting;

uint8_t key_pressed = -1;

/*
    * @brief: This function is called when the key interrupt is triggered
 */
void hps_key_ISR()
{
   
    /* read wich key has been pressed */
    int keys = get_keys();
    
    CLEAR_INTERRUPT();
}


void handle_key0_pressed(int keys, int switches, int* hexa) {
        write_leds(switches);
        write_7seg_hex(0, hexa[switches & 0xF]);
        write_7seg_hex(1, hexa[(switches >> 4) & 0xF]);

        if((switches >> 8) & 0x1 ) {
            write_7seg_hex(2, ONE);
        } else {
            write_7seg_hex(2, 0);
        }

        if ((switches >> 9) & 0x1) {
            write_7seg_hex(3, ONE);
        } else {
            write_7seg_hex(3, 0);
        }

    
}

void handle_key1_pressed(int keys, int switches, int* hexa) {
        write_leds(!switches);
        write_7seg_hex(0, hexa[switches & 0xF]);
        write_7seg_hex(1, hexa[(switches >> 4) & 0xF]);

        if((switches >> 8) & 0x1 ) {
            write_7seg_hex(2, ONE);
        } else {
            write_7seg_hex(2, 0);
        }

        if ((switches >> 9) & 0x1) {
            write_7seg_hex(3, ONE);
        } else {
            write_7seg_hex(3, 0);
        }

}

int main(void)
{

    /* Disable interrupts to configure them */
    disable_A9_interrupts();

    /* Set the IRQ stack pointer */
    set_A9_IRQ_stack();

    /* Init GIC */
    config_GIC();

    /* Enable interrupts */
    enable_A9_interrupts();

    write_7seg_hex(4, 0);   
    write_7seg_hex(5, 0);

    uint16_t switches;
    uint8_t keys;
    uint16_t leds_status=0;


    while (1)
    {

        keys = get_keys();
        switches = get_switches();

        // if key 0 is pressed, light up the first led
        if(key_pressed == 0) {
            handle_key0_pressed(keys, switches, hexa);
            leds_status = switches;
            key_pressed = -1;
        }
        // if key 1 is pressed, light up the second led
        else if(key_pressed == 1) {
            handle_key1_pressed(keys, switches, hexa);
            leds_status = ~switches;
            key_pressed = -1;
        }
        

      
    }
}
