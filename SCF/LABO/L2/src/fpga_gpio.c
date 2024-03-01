/*****************************************************************************************
 * HEIG-VD
 * Haute Ecole d'Ingenerie et de Gestion du Canton de Vaud
 * School of Business and Engineering in Canton de Vaud
 *****************************************************************************************
 * REDS Institute
 * Reconfigurable Embedded Digital Systems
 *****************************************************************************************
 *
 * File                 : hps_gpio.c
 * Author               :
 * Date                 :
 *
 * Context              : SOCF tutorial lab
 *
 *****************************************************************************************
 * Brief: light HPS user LED up when HPS user button pressed, for DE1-SoC board
 *
 *****************************************************************************************
 * Modifications :
 * Ver    Date        Student      Comments
 * 1.0	  21.02.24    CCO	   allumage de la led sur appui d'un boutton
 *
 *****************************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include "axi_lw.h"
#include "modules/alfanum.h"
#include "modules/de1soc.h"
int __auto_semihosting;

int main(void)
{
    write_7seg_hex(4, 0);   
    write_7seg_hex(5, 0);

    uint16_t switches;
    uint8_t keys;


    while (1)
    {

        keys = get_keys();
    

        switches = get_switches();

        // if key 0 is pressed, light up the first led
        if (!(keys & 0x1))
        {
            write_leds(switches);


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
        else if (!(keys & 0x2))
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
        else
        {
            write_leds(0);
            write_7seg_hex(0, 0);
            write_7seg_hex(1, 0);
            write_7seg_hex(2, 0);
            write_7seg_hex(3, 0);
        }

      
    }
}
