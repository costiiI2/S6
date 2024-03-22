/*****************************************************************************************
 * HEIG-VD
 * Haute Ecole d'Ingenerie et de Gestion du Canton de Vaud
 * School of Business and Engineering in Canton de Vaud
 *****************************************************************************************
 * REDS Institute
 * Reconfigurable Embedded Digital Systems
 *****************************************************************************************
 *
 * File                 : execptions.c
 * Author               : Sébastien Masle
 * Date                 : 16.02.2018
 *
 * Context              : SOCF class
 *
 *****************************************************************************************
 * Brief: defines exception vectors for the A9 processor
 *        provides code that sets the IRQ mode stack, and that dis/enables interrupts
 *        provides code that initializes the generic interrupt controller
 *
 *****************************************************************************************
 * Modifications :
 * Ver    Date        Engineer      Comments
 * 0.0    16.02.2018  SMS           Initial version.
 * 1.0    20.03.2024  KBP           SOCF-Lab2: IRQ from FPGA
 *
 *****************************************************************************************/
#include <stdio.h>
#include <stdint.h>

#include "address_map_arm.h"
#include "defines.h"

#define FPGA_IRQ0_NBR	72

void hps_keys_ISR(void);

// Define the IRQ exception handler
void __attribute__ ((interrupt)) __cs3_isr_irq(void)
{
	/***********
	 * TO DO
	 **********/

	// Read CPU Interface registers to determine which peripheral has caused an interrupt
	int intId = GIC_CPU_ITF_REG(OFSET_ICCIAR);

	// Handle the interrupt if it comes from the KEYs
	if (intId == FPGA_IRQ0_NBR) {
		hps_keys_ISR();
	} else {
		printf("Interrupt ID(%d) other than FPGA_IRQ0(%d)...\n",
		       intId, FPGA_IRQ0_NBR);
	}

	// Clear interrupt from the CPU Interface
	GIC_CPU_ITF_REG(OFSET_ICCEOIR) = intId;

	return;
} 

// Define the remaining exception handlers
void __attribute__ ((interrupt)) __cs3_reset (void)
{
	while(1);
}

void __attribute__ ((interrupt)) __cs3_isr_undef (void)
{
	while(1);
}

void __attribute__ ((interrupt)) __cs3_isr_swi (void)
{
	while(1);
}

void __attribute__ ((interrupt)) __cs3_isr_pabort (void)
{
	while(1);
}

void __attribute__ ((interrupt)) __cs3_isr_dabort (void)
{
	while(1);
}

void __attribute__ ((interrupt)) __cs3_isr_fiq (void)
{
	while(1);
}

/* 
 * Initialize the banked stack pointer register for IRQ mode
 */
void set_A9_IRQ_stack(void)
{
	uint32_t stack, mode;
	// top of A9 onchip memory, aligned to 8 bytes
	stack = A9_ONCHIP_END - 7;

	/* change processor to IRQ mode with interrupts disabled */
	mode = INT_DISABLE | IRQ_MODE;
	asm("msr cpsr, %[ps]" : : [ps] "r" (mode));
	/* set banked stack pointer */
	asm("mov sp, %[ps]" : : [ps] "r" (stack));

	/* go back to SVC mode before executing subroutine return! */
	mode = INT_DISABLE | SVC_MODE;
	asm("msr cpsr, %[ps]" : : [ps] "r" (mode));
}

/*
 * Turn off interrupts in the ARM processor
 */
void disable_A9_interrupts(void)
{
	int status = SVC_MODE | INT_DISABLE;
	asm("msr cpsr, %[ps]" : : [ps] "r"(status));
}

/* 
 * Turn on interrupts in the ARM processor
 */
void enable_A9_interrupts(void)
{
	uint32_t status = SVC_MODE | INT_ENABLE;
	asm("msr cpsr, %[ps]" : : [ps]"r"(status));
}

void config_interrupt(int N, int CPU_target) {
	int reg_offset, bitN, address;

	/* Configure the Interrupt Set-Enable Registers (ICDISERn).
	 * reg_offset = integer_div(N / 32) * 4 = int(N / 8) = int(N >> 3)
	 * value = 1 << (N mod 32) */
	reg_offset = (N >> 3) & 0xFFFFFFFC;
	bitN    = N & 0x1F;
	address = (GIC_DISTRIBUTOR_BASE_ADD + OFSET_ICDISER0) + reg_offset;

	/* Now that we know the register address and value,
	 * set the appropriate bit */
	*(volatile int *)address |= (0x1 << bitN);

	/* Configure the Interrupt Processor Targets Register (ICDIPTRn)
	 * reg_offset = integer_div(N / 4) * 4 = int(N / 1) = int(N)
	 * index = N mod 4 */
	reg_offset = (N & 0xFFFFFFFC);
	bitN    = N & 0x3;
	address = (GIC_DISTRIBUTOR_BASE_ADD + OFSET_ICDIPTR0) +
		  reg_offset + bitN;

	/* Now that we know the register address and value,
	 * write to (only) the appropriate byte */
	*(volatile char *)address = (char) CPU_target;
}

/* 
 * Configure the Generic Interrupt Controller (GIC)
 */
void config_GIC(void)
{
	/***********
	 * TO DO
	 **********/
	// Configure interrupt of FPGA_IRQ0 on CPU0
	config_interrupt(FPGA_IRQ0_NBR, CPU0);

	/* Set Interrupt Priority Mask Register (ICCPMR)
	 * to enable ints of all prio. */
	GIC_CPU_ITF_REG(OFSET_ICCPMR) = 0xFFFF;

	/* Set CPU Interface Control Register (ICCICR)
	 * to enable signaling of ints */
	GIC_CPU_ITF_REG(OFSET_ICCICR) = 1;

	// Enable Distributor (ICDDCR) to send pending interrupts to CPUs
	GIC_DISTRIB_REG(OFSET_ICDDCR) = 1;
}

