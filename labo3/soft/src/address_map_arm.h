/*****************************************************************************************
 * HEIG-VD
 * Haute Ecole d'Ingenerie et de Gestion du Canton de Vaud
 * School of Business and Engineering in Canton de Vaud
 *****************************************************************************************
 * REDS Institute
 * Reconfigurable Embedded Digital Systems
 *****************************************************************************************
 *
 * File                 : address_map_arm.h
 * Author               : Sébastien Masle
 * Date                 : 16.02.2018
 *
 * Context              : SOCF class
 *
 *****************************************************************************************
 * Brief: provides address values that exist in the system
 *
 *****************************************************************************************
 * Modifications :
 * Ver    Date        Engineer      Comments
 * 0.0    16.02.2018  SMS           Initial version.
 * 1.0    20.03.2024  KBP           SOCF-Lab2: IRQ from FPGA
 *
*****************************************************************************************/

#define BOARD                 		"DE1-SoC"

/* Memory */
#define DDR_BASE              		0x00000000
#define DDR_END               		0x3FFFFFFF
#define A9_ONCHIP_BASE        		0xFFFF0000
#define A9_ONCHIP_END         		0xFFFFFFFF
#define SDRAM_BASE            		0xC0000000
#define SDRAM_END             		0xC3FFFFFF
#define FPGA_ONCHIP_BASE      		0xC8000000
#define FPGA_ONCHIP_END       		0xC803FFFF
#define FPGA_CHAR_BASE        		0xC9000000
#define FPGA_CHAR_END         		0xC9001FFF

/* GIC: CPU Interface register */
#define GIC_CPU_ITF_BASE_ADD		0xFFFEC100
#define OFSET_ICCICR			0x0		/*  1bit  */
#define OFSET_ICCPMR			0x4		/*  8bits */
#define OFSET_ICCIAR			0xC		/* 10bits */
#define OFSET_ICCEOIR			0x10		/* 10bits */
#define GIC_CPU_ITF_REG(_x_)		*(volatile uint32_t *)	\
						(GIC_CPU_ITF_BASE_ADD + _x_)
					// _x_ respecting offset to base addr.

/* GIC: Distributor register */
#define GIC_DISTRIBUTOR_BASE_ADD	0xFFFED000
#define OFSET_ICDDCR			0x0		/*  1bit  */
#define OFSET_ICDISER0			0x100		/* 32bits */
#define OFSET_ICDIPR0			0x400		/* 32bits */
#define OFSET_ICDIPTR0			0x800		/* 32bits */
#define OFSET_ICDICFR0			0xC00		/* 32bits */
#define GIC_DISTRIB_REG(_x_)		*(volatile uint32_t *)	\
						(GIC_DISTRIBUTOR_BASE_ADD + _x_)
					// _x_ respecting offset to base addr.
