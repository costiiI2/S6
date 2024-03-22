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


/* Interrupts related functions */
extern void config_GIC();
extern void disable_A9_interrupts();
extern void enable_A9_interrupts();
extern void set_A9_IRQ_stack();

/* offset to clear the interrupt */
#define TIMER_EOI_OFFSET 0xC
/* offset to read the interrupt status */
#define TIMER_INSTAT_OFFSET 0x10

#define TIMER_BASE_ADD 0x1000


/* Timer related defines and marcos */
#define TIMER_REG(_offset_) *(volatile uint32_t *)(TIMER_BASE_ADD + _offset_)

/* Timer interrupt macros */
#define CLEAR_INTERRUPT() TIMER_REG(TIMER_EOI_OFFSET)

uint8_t key_pressed = -1;

/*
    * @brief: This function is called when the key interrupt is triggered
 */
void hps_key_ISR()
{
   
    /* read wich key has been pressed */
    int keys = get_keys();
	printf("irq\n");
write_leds(0xf0);
    
    CLEAR_INTERRUPT();
}


void handle_key0_pressed(int keys, int switches, const char* hexa) {
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

void handle_key1_pressed(int keys, int switches, const char* hexa) {
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
	write_leds(0xff);
    uint16_t switches;
    uint8_t keys;
    uint16_t leds_status=0;


    while (1)
    {

        keys = get_keys();
        switches = get_switches();
	printf("swtiches %d \n");

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
  else if (!(keys & 0x4))
        {
            write_leds(!switches);
            
             write_7seg_hex(0, hexa[switches & 0xF]);
            write_7seg_hex(1, hexa[(switches >> 4) & 0xF]);
            // if swirch 8 and 9 are on, light up the 2nd and 3rd 7seg display
        
            if((switches >> 8) & 0x1 )
            {
                write_7seg_hex(2, ONE);
            }
            else
            {
                write_7seg_hex(2, 0);
            }
            if ((switches >> 9) & 0x1)
            {
                write_7seg_hex(3, ONE);
            }
            else
            {
                write_7seg_hex(3, 0);
            }
        }
          else if (!(keys & 0x8))
        {
            write_leds(!switches);
       
            write_7seg_hex(0, hexa[switches & 0xF]);
            write_7seg_hex(1, hexa[(switches >> 4) & 0xF]);
            // if swirch 8 and 9 are on, light up the 2nd and 3rd 7seg display
        
            if((switches >> 8) & 0x1 )
            {
                write_7seg_hex(2, TWO);
            }
            else
            {
                write_7seg_hex(2, 0);
            }
            if ((switches >> 9) & 0x1)
            {
                write_7seg_hex(3, TWO);
            }
            else
            {
                write_7seg_hex(3, 0);
            }
        }



      
    }
}
