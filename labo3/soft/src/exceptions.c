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
 *
*****************************************************************************************/
#include <stdint.h>

#include "address_map_arm.h"
#include "defines.h"

#define IRQ_KEY 72

#define ENABLE 0x1
#define DISABLE 0x0

/* Configurations defines for GIC */
#define INTERFACE_BASE_ADD 0xFFFEC100
#define INTERFACE_LOCAL_PRIO 0xFFFF
#define OFFSET_INTERFACE_LOCAL_PRIO 0x4
#define ICCEOIR 0x10
#define ICCIAR 0x0C
#define CPU_LOCAL_OFFSET(off) *(volatile uint32_t *)(INTERFACE_BASE_ADD + off)

#define DISTRIB_BASE_ADD 0xFFFED000
#define DISTRIB_OFFSET(off) *(volatile uint32_t *)(DISTRIB_BASE_ADD + off)


void config_interrupt(int n, int CPU_target);

void hps_key_ISR(void);

// Define the IRQ exception handler
void __attribute__ ((interrupt)) __cs3_isr_irq(void)
{
	
	/* Read CPU Interface  (ICCIAR) registers to determine which peripheral has caused an interrupt */
	int id = CPU_LOCAL_OFFSET(ICCIAR);

	/* Handle the interrupt if it comes from the timer */
	if (id == IRQ_KEY)
		hps_key_ISR();
	else {
		/*other irq*/
	}
	/* Clear interrupt from the CPU Interface (ICCEOIR) */
	CPU_LOCAL_OFFSET(ICCEOIR) = id;

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
	stack = A9_ONCHIP_END - 7;		// top of A9 onchip memory, aligned to 8 bytes
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
 * Turn on interrupts in the ARM processor
*/
void enable_A9_interrupts(void)
{
	uint32_t status = SVC_MODE | INT_ENABLE;
	asm("msr cpsr, %[ps]" : : [ps]"r"(status));
}


/*
 * Turn off interrupts in the ARM processor
 */
void disable_A9_interrupts(void)
{
	uint32_t status = SVC_MODE | INT_DISABLE;
	asm("msr cpsr, %[ps]" : : [ps] "r"(status));
}


/*
 * Configure the Generic Interrupt Controller (GIC)
 */
void config_GIC(void)
{
	config_interrupt(IRQ_KEY, 0x1);

	/* CPU interface enable and priority bits */
	CPU_LOCAL_OFFSET(OFFSET_INTERFACE_LOCAL_PRIO) |= INTERFACE_LOCAL_PRIO;
	CPU_LOCAL_OFFSET(0x0) = ENABLE;

	/* DISTRIBUTOR enable */
	DISTRIB_OFFSET(0x0) = ENABLE;
}

void config_interrupt(int n, int CPU_target)
{
	int reg_offset, index, value, address;
	/* Configure the Interrupt Set-Enable Registers (ICDISERn).
	 * reg_offset = (integer_div(N / 32) * 4
	 * value = 1 << (N mod 32) */
	reg_offset = (n >> 3) & 0xFFFFFFFC;
	index = n & 0x1F;
	value = 0x1 << index;
	address = 0xFFFED100 + reg_offset;
	/* Now that we know the register address and value, set the appropriate bit */
	*(int *)address |= value;

	/* Configure the Interrupt Processor Targets Register (ICDIPTRn)
	 * reg_offset = integer_div(N / 4) * 4
	 * index = N mod 4 */
	reg_offset = (n & 0xFFFFFFFC);
	index = n & 0x3;
	address = 0xFFFED800 + reg_offset + index;
	/* Now that we know the register address and value, write to (only) the
	 * appropriate byte */
	*(char *)address = (char)CPU_target;
}
