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
#include "modules/de1soc.h"
int __auto_semihosting;



int main(void){
    
	uint16_t switches;

    
    while(1)
    {
       	switches = get_switches();
        write_leds(switches);
    
    }

}
