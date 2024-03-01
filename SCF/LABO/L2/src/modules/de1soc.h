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
 *****************************************************************************************
 * Modifications :
 * Ver    Date        Student      Comments
 * 0.0    23.01.2024  CCT & KBP    Initial version.
 *
*****************************************************************************************/
#ifndef __DE1SOC_IO_H__
#define __DE1SOC_IO_H__

#include <stdint.h>
#include <stdbool.h>
#include "../axi_lw.h"

#ifndef BIT0
#define BIT0	(1 << 0)
#define BIT1	(1 << 1)
#define BIT2	(1 << 2)
#define BIT3	(1 << 3)
#define BIT4	(1 << 4)
#define BIT5	(1 << 5)
#define BIT6	(1 << 6)
#define BIT7	(1 << 7)
#endif

/* *** OFFSETS ************************************************************* */
#define INTERFACE_OFST		0x10000

// Based offset defined by lad sheet
#define OFST_CONSTANT_ID	0x0
#define OFST_KEYS			0x10
#define OFST_SWITCHES		0x20
#define OFST_LEDS			0x00
#define OFST_HEX3_0			0x30
#define OFST_HEX5_4			0x40

/* *** ACCESS MACRO ******************************************************** */
#define ITF_REG(_x_)	*(volatile uint32_t *)							\
						((AXI_LW_HPS_FPGA_BASE_ADD + INTERFACE_OFST) + _x_)

/* *** BITS DEFINITIONS **************************************************** */
#define KEYS_MASK		0xF
#define SWITCHES_MASK	0x3FF
#define LEDS_MASK		0xFF

#define HEX_MASK		0x7F
#define HEX_BIT_SIZE	7
#define HEX0_NBR		0
#define HEX1_NBR		1
#define HEX2_NBR		2
#define HEX3_NBR		3
#define HEX4_NBR		4
#define HEX5_NBR		5

/* ************************************************************************* */
/** get_constant(): Read interface constant defined by students */
uint32_t get_constant(void);

/** get_keys(): Read DE1SoC's keys
 * Note: Result is already inverted in functions, because they're active LOW */
uint32_t get_keys(void);

/** get_keys(): Read DE1SoC switches */
uint32_t get_switches(void);

/** Accessors on DE1SoC's LEDs */
uint32_t get_leds(void);
void write_leds(uint32_t maskled);

/** Simplified function to toggle LEDs */
void toggle_leds(uint32_t maskled);

/** Accessors on a specific DE1SoC's 7Seg */
uint8_t get_7seg_hex(uint8_t nbr7seg);
void write_7seg_hex(uint8_t nbr7seg, uint8_t mask7seg);

/** Accessors on DE1SoC's 7Seg: HEX3 to HEX0 */
uint32_t get_7segs_hex3_0(void);
void write_7segs_hex3_0(uint32_t mask7seg);

/** Accessors on DE1SoC's 7Seg: HEX5 to HEX4 */
uint32_t get_7segs_hex5_4(void);
void write_7segs_hex5_4(uint32_t mask7seg);

/** write_nbr_on_7segs()
 * "Write" nbr on 7Segs HEX4 to HEX0
 * NOTE: As for this application, HEX5 is used to display
 *       the ouf-of-bound side exceeded, it is omitted in this function */
void write_nbr_on_7segs(uint32_t n);

#endif /* __DE1SOC_IO_H__ */
