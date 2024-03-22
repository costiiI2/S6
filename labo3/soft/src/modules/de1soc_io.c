/*****************************************************************************************
 * HEIG-VD
 * Haute Ecole d'Ingenerie et de Gestion du Canton de Vaud
 * School of Business and Engineering in Canton de Vaud
 *****************************************************************************************
 * REDS Institute
 * Reconfigurable Embedded Digital Systems
 *****************************************************************************************
 *
 * File                 : de1soc.h
 * Author               : Constantino CECCHET & Kévin BOUGNON-PEIGNE
 * Date                 : 23.01.2024
 *
 * Context              : ARE-Lab6
 *
 *****************************************************************************************
 * Brief: Header file for bus AXI lightweight HPS to FPGA defines definition
 *        Based on ARE-L5 between Bastien Pillonel (BPL) & KBP
 *
 * Note: Keys & 7Segments are active LOW /!\
 *****************************************************************************************
 * Modifications :
 * Ver    Date        Student      Comments
 * 0.0    23.01.2024  CCT & KBP    Initial version.
 * 1.0    20.03.2024  KBP          SOCF-Lab2: IRQ from FPGA
 *
 ****************************************************************************************/
#include "de1soc_io.h"
#include <stdio.h>
#include <stdint.h>

static uint32_t addr_offset_7seg[] = { OFST_HEX3_0, OFST_HEX5_4 };
/* Digits defined ACTIVE HIGH */
static uint8_t digits[] = {	0x3F, 0x06, 0x5B, 0x4F, 0x66,
				0x6D, 0x7D, 0x07, 0x7F, 0x6F,
				0x77, 0x7C, 0x58, 0x5E, 0x79,
				0x71	};

void hps_keys_ISR(void) {
	uint32_t reg = get_leds();

	// Detect button
	if (ITF_REG(OFST_KEYS + OFST_EDGECAPTURE) & BIT2)	// LEDs ROR
		reg = (reg >> 1) | (reg << 9);
	else if (ITF_REG(OFST_KEYS + OFST_EDGECAPTURE) & BIT3)	// LEDs ROL
		reg = (reg << 1) | (reg >> 9);

	write_leds(reg & LEDS_MASK);

	// Ack. int.
	ITF_REG(OFST_KEYS + OFST_EDGECAPTURE) = 0;
}

/* ========================= INTERFACE I/O ACCESS ========================= */
/*uint32_t get_constant(void) {
  return ITF_REG(OFST_CONSTANT_ID);
  }*/

void enable_keys_int(uint8_t mask)
{
	ITF_REG(OFST_KEYS + OFST_INTMASK) |= (mask & KEYS_MASK);
}

/* ============================ DE1 I/O ACCESS ============================ */
uint32_t get_keys(void)
{
	return (~ITF_REG(OFST_KEYS)) & KEYS_MASK;
}

uint32_t get_switches(void)
{
	return ITF_REG(OFST_SWITCHES) & SWITCHES_MASK;
}

uint32_t get_leds(void)
{
	return ITF_REG(OFST_LEDS) & LEDS_MASK;
}

void write_leds(uint32_t maskled)
{
	ITF_REG(OFST_LEDS) = maskled & LEDS_MASK;
}

void toggle_leds(uint32_t maskled)
{
	ITF_REG(OFST_LEDS) ^= (maskled & LEDS_MASK);
}

uint8_t get_7seg_hex(uint8_t nbr7seg)
{
	uint8_t shift_pos = HEX_BIT_SIZE * (nbr7seg % 4);

	return (ITF_REG(addr_offset_7seg[nbr7seg / 4]) >> shift_pos) & HEX_MASK;
}

void write_7seg_hex(uint8_t nbr7seg, uint8_t mask7seg)
{
	uint32_t reg = 0;
	uint8_t address_idx = nbr7seg / 4;
	uint8_t offset_7seg = (nbr7seg % 4) * HEX_BIT_SIZE;

	reg = ITF_REG(addr_offset_7seg[address_idx]);

	/* Reset concerned 7seg */
	reg |= (HEX_MASK << offset_7seg);

	/* Set   concerned 7seg */
	reg &= ~((mask7seg & HEX_MASK) << offset_7seg);

	ITF_REG(addr_offset_7seg[address_idx]) = reg;
}

uint32_t get_7segs_hex3_0(void)
{
	return ITF_REG(OFST_HEX3_0);
}

void write_7segs_hex3_0(uint32_t mask7segs)
{
	ITF_REG(OFST_HEX3_0) = ~mask7segs;
}

uint32_t get_7segs_hex5_4(void)
{
	return ITF_REG(OFST_HEX5_4);
}

void write_7segs_hex5_4(uint32_t mask7segs)
{
	ITF_REG(OFST_HEX5_4) = ~mask7segs;
}

void write_digit_on_7seg(uint8_t nbr7seg, uint8_t n, uint8_t base)
{
	uint8_t offset = (nbr7seg < 4) ? OFST_HEX3_0 : OFST_HEX5_4;
	uint8_t shift  = (nbr7seg % 4) * HEX_BIT_SIZE;
	uint32_t reg;

	if ((base < 2) || (base > 16))	return;

	reg = ITF_REG(offset);

	// Reset display (active LOW)
	reg |= (HEX_MASK << shift);
	// As active LOW, XOR op. toggle active bits and make it work
	reg ^= (digits[n % base] << shift);

	ITF_REG(offset) = reg;
}

void write_nbr_on_7segs(uint32_t n)
{
	uint32_t reg, display = 0;
	static uint8_t i;

	/* Display on HEX3 to 0 */
	for (i = 0; i < 28; i += HEX_BIT_SIZE) {
		display |= ((digits[n % 10] & HEX_MASK) << i);
		if ( ! (n /= 10) )	break;
	}
	ITF_REG(OFST_HEX3_0) = ~display;

	/* If n = 0, simply clear HEX4 through iowrite() */
	display = 0;
	if (n) {
		for (i = 0; i < 7; i += HEX_BIT_SIZE) {
			display |= ((digits[n % 10] & HEX_MASK) << i);
			if ( ! (n /= 10) )	break;
		}
	}
	/* Only HEX4, as HEX5 is used to display limit IRQ (spec. to 1 ARE lab) */
	reg = ITF_REG(OFST_HEX5_4);
	reg = (reg | HEX_MASK) ^ display;
	ITF_REG(OFST_HEX5_4) = reg;
}
