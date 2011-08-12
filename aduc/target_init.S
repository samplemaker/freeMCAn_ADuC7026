/** \file /aduc/target_init.S
 * \brief ADUC-port specific target initialization and IRQ functions
 *
 * \author Copyright (C) 2011 samplemaker
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301 USA
 *
 *
 * For target initialization see:
 *
 * "ARM7TDMI Revision: r4p1 Technical Reference Manual"
 * http://infocenter.arm.com/help/topic/com.arm.doc.ddi0210c/DDI0210B.pdf
 *
 * "ARM Compiler toolchain V4.1 Developing Software for ARM Processors" 
 * http://infocenter.arm.com/help/topic/com.arm.doc.dui0471c/DUI0471C_developing_for_arm_processors.pdf
 *
 * "Precision Analog Microcontroller, 12-Bit Analog I/O, ARM7TDMI MCU
 *  ADuC7019/20/21/22/24/25/26/27/28/29 (REV. D)"
 * http://www.analog.com/static/imported-files/data_sheets/ADuC7019_20_21_22_24_25_26_27_28_29.pdf
 *
 * \defgroup ADUC
 * \ingroup ADUC
 *
 * @{
 */

#include "aduc7026.h"
#include "int.h"

.extern main
.extern _irq_handler


/** Current program status register (cpsr) bit definitions and mode identifiers
 *
 * CPSR_c = [I F T M4 M3 M2 M1 M0] 
 *
 * See: ARM7TDMI Technical Reference Manual p. 61
 *
 */
	.equ	I_FLAG, 0x80	/* IRQ */
	.equ	F_FLAG, 0x40	/* FIQ */
	.equ	T_FLAG, 0x20	/* Arm<>Thumb state */
	.equ	M_USR, 0x10	/* User Mode */
	.equ	M_FIQ, 0x11	/* Fast Interrupt Mode */
	.equ	M_IRQ, 0x12	/* Interrupt Mode */
	.equ	M_SVC, 0x13	/* Supervisor Mode */
	.equ	M_ABT, 0x17	/* Abort Mode */
	.equ	M_UND, 0x1B	/* Undefined Mode */
	.equ	M_SYS, 0x1F	/* System Mode */

/** Stack sizes in operating modes 
 *
 */
	.equ	STACKSIZE_UND, 4
	.equ	STACKSIZE_SVC, 256
	.equ	STACKSIZE_ABT, 4
	.equ	STACKSIZE_FIQ, 4
	.equ	STACKSIZE_IRQ, 256
	.equ	STACKSIZE_USR, 256


.section .vectors,"x"
.code 32
.align 0

/** Jump island exception vector table for operating modes
 *
 * See: ARM7TDMI Technical Reference Manual p. 67
 *
 */
	b	_init0		/* Reset -> Supervisor  */
	b	.	        /* Undefined Instruction -> Undefinded */
	b	_swi_handler	/* Software Interrupt -> Supervisor */
	b	.		/* Prefetch Abort -> Abort */
	b	.		/* Data Abort -> Abort */
	nop			/* Reserved -> Reserved */
	b	_irq_handler	/* IRQ -> IRQ */
	b	.		/* FIQ -> FIQ */


.section .init0,"x"
.code 32
.align 4

_init0:

/** Initialize / reset phase lock loop and power control
 *
 * See: ADUC7026 datasheet p. 55
 *
 */
#if RESET_POWER_AND_PLL_AT_BOOT
	ldr	r0, =__MMR_BASE
	mov	r1, #0xaa	/* Access to PLLCON needs special sequence */
	str	r1, [r0, #PLLKEY1]
	mov	r1, #PLLCON_RESET_CFG
	str	r1, [r0, #PLLCON]
	mov	r1, #0x55
	str	r1, [r0, #PLLKEY2]
	mov	r1, #0x01	/* Access to POWCON needs special sequence */
	str	r1, [r0, #POWKEY1]
	mov	r1, #POWCON_RESET_CFG
	str	r1, [r0, #POWCON]
	mov	r1, #0xF4
	str	r1, [r0, #POWKEY2]
#endif

/** Setup stack pointers and FIQ/IRQ bits
 *
 * Enter modes, disable FIQ/IRQ-Flags and set up stack pointers. 
 * Priviledged modes first, user mode last because flags cannot be set / 
 * cleared in user mode. 
 * System mode cannot be entered by an exception and must be setup by CPSR 
 * manipulation. We will not provide a stack.
 * Note: (sp -> r14_svc, r14_irq ... ).
 *
 * See: ARM Compiler toolchain p. 55
 *
 */
	ldr	r0, =__stack_end__
	msr	cpsr_c, #M_FIQ|I_FLAG|F_FLAG
	mov	sp, r0
	sub	r0, r0, #STACKSIZE_FIQ
	msr	cpsr_c, #M_IRQ|I_FLAG|F_FLAG
	mov	sp, r0
	sub	r0, r0, #STACKSIZE_IRQ
	msr	cpsr_c, #M_SVC|I_FLAG|F_FLAG
	mov	sp, r0
	sub	r0, r0, #STACKSIZE_SVC
	msr	cpsr_c, #M_ABT|I_FLAG|F_FLAG
	mov	sp, r0
	sub	r0, r0, #STACKSIZE_ABT
	msr	cpsr_c, #M_UND|I_FLAG|F_FLAG
	mov	sp, r0
	sub	r0, r0, #STACKSIZE_UND
	msr	cpsr_c, #M_USR|I_FLAG|F_FLAG
	mov	sp, r0

/** Initialize variables in RAM
 *
 * Copy block .data from flash (LMA) starting after .text with size of .data
 * into RAM (VMA) starting at _data. Note: Requires ALIGN(4).
 *
 */
	ldr	r0, =_data_load_start	/* r3 = addr(end of .text => fash)   */
	ldr	r1, =_data		/* r0 = addr(start of .data => ram)  */
	ldr	r2, =_data_load_end	/* r1 = addr(end of .data => ram)    */
copy_data:
	cmp	r0, r2			/* (r0 ==<> r2) ?                    */
	ldrlo	r3, [r0], #4		/* if r0<r2: r3=*(r0) then r0+=4     */
	strlo	r3, [r1], #4		/* if r0<r2: *(r1)=r3 then r1+=4     */
	blo	copy_data		/* branch if condition lower         */

/** Clear .bss section 
 *
 * Note: Requires ALIGN(4).
 */
	mov	r0, #0			/* r0 = 0                            */
	ldr	r1, =__bss_start__	/* r0 = addr(start of .bss section)  */
	ldr	r2, =__bss_end__	/* r0 = addr(end of .bss section)    */
zero_bss:
        cmp	r1, r2			/* (r1 ==<> r2) ?                    */
	strlo	r0, [r1], #4		/* if r1<r2: *(r1)=r0 then r1+=4     */
	blo	zero_bss		/* branch if condition lower         */

	b	main


.section .text
.code 32
.align 4

/** Software interrupt handler for enabling and disabling the I-Flag
 *
 *  1.) The I-Flag in cpsr_c cannot be written in user mode but only in 
 *	a priviledged mode.
 *  2.) Switch to supervisor mode by software interrupt.
 *  3.) Entering the SWI-exception the processor does:
 *      i.)   Save the address of the next instruction (return adr.) 
 *            in the appropriate Link Register: (lr_svc = pc + 4)
 *      ii.)  spsr_svc = current cpsr
 *      iii.) Overwrite M-field in cpsr with the svc mode bits
 *      iv.)  Force ARM state
 *      v.)   Disable I & F - Flag in cpsr
 *      vi.)  Jump to exception vector address
 *  4.) The code gets the SWI argument and disables/enables the I-Flag
 *
 *  Note: (13 sp, 14 lr, 15 pc)
 *
 *  See: ARM Compiler toolchain p. 116
 *
 */

_swi_handler:
	stmfd	sp!,{r0-r12,lr}	/* Save workspace and current return 
				   Address lr into the stack              */
	mrs	r1, spsr	/* Get old user mode cpsr from spsr_svc   */
	tst	r1, #T_FLAG	/* SWI occurred in Thumb state?           */
	ldrneh	r0, [lr,#-2]	/* Yes: Load SVC instruction halfword via */
	bicne	r0, r0, #0xFF00	/* linkregister and extract comment field */
	ldreq	r0, [lr,#-4]	/* No: Load word SVC instruction and      */
	biceq	r0, r0, #0xFF000000	/* clear top 8 bits of SVC 
					   instruction opcode             */
	cmp	r0, #ENABLE_GLOBALIRQ_BY_SWI
	beq	swi_enable_irq
	cmp	r0, #DISABLE_GLOBALIRQ_BY_SWI
	beq	swi_disable_irq
swi_end:
	msr	spsr_c, r1	/* Store condition field with updated I-Flag */
	ldmfd	sp!, {r0-r12,pc}^	/* Return to instruction following 
					   the SVC instr. ^ in this context 
					   means restore CPSR from SPSR      */

swi_enable_irq:
	bic	r1, r1, #I_FLAG	/* Enable interrupt flag */	
	b	swi_end

swi_disable_irq:
	orr	r1, r1, #I_FLAG	/* Disable interrupt flag */
	b	swi_end			



/** @} */

.end
