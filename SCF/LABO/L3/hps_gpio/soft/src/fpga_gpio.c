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

/*
 * @brief: This function is called when the key interrupt is triggered
 */
void hps_key_ISR()
{

    /* read wich key has been pressed */
    key_pressed = ITF_REG(OFST_KEYS + EDGE_CAP);
    /* clear the interrupt */
    ITF_REG(OFST_KEYS + EDGE_CAP) = 0;
}

/*
 * @brief: This function display the value on the leds and the 7-segment display

*/
void write_leds_hex(uint16_t input_value)
{
    write_leds(input_value);
    write_7seg_hex(HEX0_NBR, hexa[input_value & 0xF]);
    write_7seg_hex(HEX1_NBR, hexa[(input_value >> 4) & 0xF]);
}

void write_hex_4_5(uint16_t input_value)
{
    if (input_value & LED_8)
        write_7seg_hex(HEX2_NBR, ONE);
    else
        write_7seg_hex(HEX2_NBR, OFF);

    if (input_value & LED_9)
        write_7seg_hex(HEX3_NBR, ONE);
    else
        write_7seg_hex(HEX3_NBR, OFF);
}

void handle_key_pressed(int input_value, bool invert)
{
    write_hex_4_5(input_value);
    if (invert)
        input_value = ~input_value;
    write_leds_hex(input_value);
}

void setup()
{
    /* Disable interrupts to configure them */
    disable_A9_interrupts();

    /* Set the IRQ stack pointer */
    set_A9_IRQ_stack();

    /* Init GIC */
    config_GIC();

    /* Enable the KEYs interrupts */
    ITF_REG(OFST_KEYS + IRQ_MASK) |= (BIT3 | BIT2 | BIT1 | BIT0);
    
    /* Enable interrupts */
    enable_A9_interrupts();

    /* Base set up*/
    write_leds(OFF);
    for (int i = 0; i < HEX_COUNT; i++)
        write_7seg_hex(i, OFF);
}

int main(void)
{

    setup();
    
    uint16_t leds_status = get_switches();
    key_pressed = 0;

    while (1)
    {

        // if key 0 is pressed, light up with pattern of the switches
        if (key_pressed & KEY_0)
        {
            leds_status = get_switches();
            handle_key_pressed(leds_status, false);
            key_pressed = 0;
        }
        // if key 1 is pressed, light up with the inverted pattern of the switches
        if (key_pressed & KEY_1)
        {
            leds_status = get_switches();
            handle_key_pressed(leds_status, true);
            key_pressed = 0;
        }
        // if key 2 is pressed, shift the leds and hex displays to the left
        if (key_pressed & KEY_2)
        {
            uint8_t last_bit = (leds_status & 0x200) >> 9;
            leds_status = leds_status << 1 | last_bit;
            write_leds_hex(leds_status);
            key_pressed = 0;
        }
        // if key 3 is pressed, shift the leds and hex displays to the right
        if (key_pressed & KEY_3)
        {
            uint8_t first_bit = leds_status & 0x1;
            leds_status = leds_status >> 1 | first_bit << 9;
            write_leds_hex(leds_status);
            key_pressed = 0;
        }
    }
}
