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
int __auto_semihosting;


/* we define macros for setting and clearing bits in a register without changing the other bits */
#define SET_BIT(reg, bit) ((reg) |= (1U << (bit)))
#define CLEAR_BIT(reg, bit) ((reg) &= ~(1U << (bit)))

/* we define the base address of GPIO 1 where our led and button are assigned */
#define BASE_ADDR 0xFF709000
#define EXTERNAL_PORT_OFFSET 0x50

/* we define the base address of the led and button */
#define LED_OFFSET 24
#define BUTTON_OFFSET 25

/* we define the offset of the data registry and base registry for GPIO 1 */
#define DIR_ADDR_OFFSET 0x4 // 0 = input 1 = output
#define DATA_ADDR_OFFSET 0x0 // 0 = low 1 = high


int main(void){
    
    /* we declare direction and data addreses for our leds and button*/
    volatile uint32_t *data_led = (uint32_t *) (BASE_ADDR + DATA_ADDR_OFFSET);
    volatile uint32_t *dir_led = (uint32_t *)(BASE_ADDR + DIR_ADDR_OFFSET);

    /* for the button we read the External Port A Register */
    volatile uint32_t *data_button = (uint32_t *) (BASE_ADDR + EXTERNAL_PORT_OFFSET );
    volatile uint32_t *dir_button = (uint32_t *)(BASE_ADDR + DIR_ADDR_OFFSET);

    /* we set the direction of the led to output and the button to input */
    SET_BIT(*dir_led, LED_OFFSET);
    CLEAR_BIT(*dir_button, BUTTON_OFFSET);

    while(1)
    {
        /* we check if the button is pressed or not and we light the led accordingly */
        if(*data_button & (1U << BUTTON_OFFSET))
            CLEAR_BIT(*data_led, LED_OFFSET);        	       
        else
            SET_BIT(*data_led, LED_OFFSET);
    
    }

}
