/*****************************************************************************************
 * HEIG-VD
 * Haute Ecole d'Ingenerie et de Gestion du Canton de Vaud
 * School of Business and Engineering in Canton de Vaud
 *****************************************************************************************
 * REDS Institute
 * Reconfigurable Embedded Digital Systems
 *****************************************************************************************
 *
 * File                 : keys_interrupt.c
 * Author               : Kévin BOUGNON-PEIGNE
 * Date                 : 2024.03.20
 *
 * Context              : SOCF tutorial lab
 *
 *****************************************************************************************
 * Brief: light HPS user LED up when HPS user button pressed, for DE1-SoC board
 *
 *****************************************************************************************
 * Modifications :
 * Ver    Date        Student      Comments
 * 1.0    20.03.2024  KBP          SOCF-Lab2: IRQ from FPGA
 * 
 *
 ****************************************************************************************/
#include <stdint.h>
#include "modules/de1soc_io.h"

/* Those functions are only described in a C file,
 * so use "extern" to tell the compiler it is defined elsewhere */
extern void disable_A9_interrupts(void);
extern void set_A9_IRQ_stack(void);
extern void config_GIC(void);
extern void enable_A9_interrupts(void);

#define GPIO1_BASE_ADDR	0xFF709000
#define GPIO1_REG(_x_)	*((volatile uint32_t *) (GPIO1_BASE_ADDR + _x_))

#define GPIO_OUT_REG	0x0
#define GPIO_DIR_REG	0x4
#define GPIO_IN_REG	0x50

#define HPS_KEY_GPIO_NBR	54
#define HPS_KEY_POS		(HPS_KEY_GPIO_NBR % 29)
#define HPS_KEY_MASK		(1 << HPS_KEY_POS)
#define HPS_LED_GPIO_NBR	53
#define HPS_LED_POS		(HPS_LED_GPIO_NBR % 29)
#define HPS_LED_MASK		(1 << HPS_LED_POS)

#define N_KEYS		4
#define KEY0_NBR        0
#define KEY1_NBR        1
#define KEY2_NBR        2
#define KEY3_NBR        3
#define KEY_READ_TIMES	4

int Key_read(uint32_t key_index)
{
	return get_keys() & (1 << key_index);
}

bool key_edge_detection(uint8_t key_index)
{
	static unsigned keys_state[N_KEYS] = {1, 1, 1, 1};

	/* If key is pressed => increment its counter,
	 * until we see enough valid pressure in a row */
	if ( Key_read(key_index) ) {
		if(keys_state[key_index] == KEY_READ_TIMES) {
			/* Increase once to avoid continuous TRUE return */
			++keys_state[key_index];
			return true;

		} else if (keys_state[key_index] < KEY_READ_TIMES) {
			// Count enough times to see a "stable" 1 logic state
			++keys_state[key_index];
			return false;

		} else {
			return false;
		}
	}

	// Key's not pressed, so reset counter
	keys_state[key_index] = 1;
	return false;
}

int main(void)
{
	uint32_t keyUsr, oldKeyUsr = 0;
	uint16_t switches;

	/* ************************* CONFIGURATION ************************* */
	/* Disable ints in the A9 proco. during configuration */
	disable_A9_interrupts();

	/* Initialize the stack pointer for IRQ mode */
	set_A9_IRQ_stack();

	/* GIC, IO & timer configuration */
	config_GIC();

	/* Set direction: LED as Out & KEY as In */
	GPIO1_REG(GPIO_DIR_REG) |=  HPS_LED_MASK;
	GPIO1_REG(GPIO_DIR_REG) &= ~HPS_KEY_MASK;

	/* Enable interrupt coming from KEY3 & KEY2 */
	enable_keys_int(BIT3 | BIT2);

	/* Enable them in the A9 proco. */
	enable_A9_interrupts();

	/* ************************* MAIN PROGRAM ************************** */
	while (1) {
		keyUsr = (GPIO1_REG(GPIO_IN_REG) & HPS_KEY_MASK) >> HPS_KEY_POS;
		switches = get_switches();

		if (keyUsr != oldKeyUsr) {
			// Key active low
			if ( ! keyUsr )
				GPIO1_REG(GPIO_OUT_REG) |= HPS_LED_MASK;
			else
				GPIO1_REG(GPIO_OUT_REG) &= ~HPS_LED_MASK;
		}

		if (key_edge_detection(KEY0_NBR)) {
			write_leds(switches);

			/* On HEX0=LED3..0 and on HEX1=LED7..4 as digits
			 * in base 16 */
			write_digit_on_7seg(0, switches & 0x0F, 16);
			write_digit_on_7seg(1, (switches & 0xF0) >> 4, 16);

			/* On HEX2=LED8 status (displayed as digits 0/1)
			 * in base 10 */
			write_digit_on_7seg(2, (switches & BIT8) ? 1 : 0, 10);

			/* On HEX3=LED9 status (displayed as digits 0/1)
			 * in base 10 */
			write_digit_on_7seg(3, (switches & BIT9) ? 1 : 0, 10);
		}

		if (key_edge_detection(KEY1_NBR)) {
			write_leds(~switches);

			/* On HEX0=LED3..0 and on HEX1=LED7..4 as digits
			 * in base 16 */
			write_digit_on_7seg(0, switches & 0x0F, 16);
			write_digit_on_7seg(1, (switches & 0xF0) >> 4, 16);

			/* On HEX2=LED8 status (displayed as digits 0/1)
			 * in base 10 */
			write_digit_on_7seg(2, (switches & BIT8) ? 1 : 0, 10);

			/* On HEX3=LED9 status (displayed as digits 0/1)
			 * in base 10 */
			write_digit_on_7seg(3, (switches & BIT9) ? 1 : 0, 10);
		}

		oldKeyUsr = keyUsr;
	}

	return 0;
}
