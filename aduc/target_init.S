/** \file aduc/target_init.S
 * \brief ADuC specific target initialization
 *
 * \author Copyright (C) 2011 samplemaker
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
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
 * ADuC7019/20/21/22/24/25/26/27/28/29 (REV. D)"
 * http://www.analog.com/static/imported-files/data_sheets/ADuC7019_20_21_22_24_25_26_27_28_29.pdf
 *
 * \defgroup aduc_target_init Startup code for ADuC bare silicon
 * \ingroup ADUC
 *
 * @{
 */

/*-----------------------------------------------------------------------------
 * Includes
 *-----------------------------------------------------------------------------
 */

#include "aduc.h"

/*-----------------------------------------------------------------------------
 * Defines
 *-----------------------------------------------------------------------------
 */

#define CLOCKSEL_EXTCRYST_TIMER2_INIT ((250*32768)/1000000) /* 250us */
#define CLOCKSEL_EXTCRYST_TIMER2_POLL ((150*32768)/1000000) /* 150us */

/*-----------------------------------------------------------------------------
 * Variables
 *-----------------------------------------------------------------------------
 */

.extern main
.extern do_initcalls
.extern _irq_handler
.extern _swi_handler

.code 32
.align 0
.section .vector, "ax"

/** Jump island exception vector table
 *
 * See:  ARM7TDMI Technical Reference Manual p. 67
 *
 * Note: After power on reset the flash (0x80000 onwards) is mirrored
 *       at address 0x00000 (MMR-REMAP bit 0 unset). To not keep on
 *       executing code from 0x00000 onwards we have to jump into the
 *       0x80000 address range. Therefore we load the handler address
 *       from a "literal pool" instead of using a branch (b $targetaddr)
 *       instruction. This is necessary if we want to use breakpoints.
 *
 */
_vector:				/* Exception type; Mode on entry */
	ldr	pc, init_target		/* Reset; Supervisor */
	ldr	pc, und_handler		/* Undefined Instruction; Undefinded */
	ldr	pc, swi_handler		/* Software Interrupt; Supervisor */
	ldr	pc, pabt_handler	/* Prefetch Abort; Abort */
	ldr	pc, dabt_handler	/* Data Abort; Abort */
	nop				/* Reserved ; Reserved */
	ldr	pc, irq_handler		/* IRQ; IRQ */
	ldr	pc, fiq_handler		/* FIQ; FIQ */

/* Literal pool holding handler addresses */

init_target:	.word	_init_target
und_handler:	.word	_und_handler
swi_handler:	.word	_swi_handler
pabt_handler:	.word	_pabt_handler
dabt_handler:	.word	_dabt_handler
irq_handler:	.word	_irq_handler
fiq_handler:	.word	_fiq_handler

/* .ltorg */

/* Empty handler stubs (handlers may be implemented in foreign modules).
 * Set to default handler (infinite loop).
 * Note: Power on reset is handled inside this file and must be non weak.
 */
	.weak	_und_handler
	.set	_und_handler, _xcptn_trap
	.weak	_swi_handler
	.set	_swi_handler, _xcptn_trap
	.weak	_pabt_handler
	.set	_pabt_handler, _xcptn_trap
	.weak	_dabt_handler
	.set	_dabt_handler, _xcptn_trap
	.weak	_irq_handler
	.set	_irq_handler, _xcptn_trap
	.weak	_fiq_handler
	.set	_fiq_handler, _xcptn_trap

_xcptn_trap:
	b	_xcptn_trap

.ascii "freeMCAn ADUC / (C) 2011 samplemaker and Hans Ulrich Niedermann"

.size   _vector, . - _vector


.code 32
.align 4
.section .init_target, "ax"

.global _init_target
.func _init_target

_init_target:

/** Setup Clock selection, PLL and core clock divider
 *
 * See:  ADuC7026 datasheet p. 55
 *
 * Note: The selection of the clock source is in the PLLCON register. By
 *       default, the part uses the internal oscillator feeding the PLL.
 *       PLLCON controls the operating mode of the clock system, whereas
 *       POWCON controls the core clock frequency and the power-down mode.
 *
 *       ACTIVE: all parts powered
 *       PAUSE:  core halted
 *       NAP:    peripherals also halted
 *       SLEEP:  PLL also halted
 *       STOP:   only GPIO IRQ powered
 */
	ldr	r0, =__MMR_BASE

#if CLOCKSEL_EXTCRYST
        /* disable IRQs to avoid executing timer2 ISR */
	mrs	r1, cpsr
	orr	r1, r1, #I_FLAG
	msr	cpsr_c, r1
	/* get timer2 act value (tOldVal) */
	ldr	r2, [r0, #T2VAL]
	/* load wakeup timer2 initialization value */
	mov	r1, #CLOCKSEL_EXTCRYST_TIMER2_INIT
	str	r1, [r0, #T2LD]
	/* switch to internal oscillator; divider=1 and enable timer2 */
	mov	r1, #(_FS(TIMER2_CLKSOURCE,TIMER2_INT_OSC) |_BV(TIMER2_ENABLE))
	orr	r3, r0 ,#T2CON
	strh	r1, [r3]
	/* poll until timer has latched */
poll_t2ready:
	ldr	r1, [r0, #T2VAL]
	cmp	r1, r2
	beq	poll_t2ready /* branch if T2VAL == tOldVal */
	cmp	r1, #CLOCKSEL_EXTCRYST_TIMER2_POLL  /* approx 150 us */
	bhi	poll_t2ready /* branch if T2VAL > threshold */
	/* enable timer2 irq flag */
	mov	r1, #_BV(INT_WAKEUP_TIMER2)
	str	r1, [r0, #IRQEN]
	/* PLL, select external crystal (clear OSEL) */
	mov	r1, #0xAA
	str	r1, [r0, #PLLKEY1]
	mov	r1, #(_FS(PLL_MDCLK, MASK_01))
	str	r1, [r0, #PLLCON]
	mov	r1, #0x55
	str	r1, [r0, #PLLKEY2]
	/* force core into NAP mode, 326 kHz */
	mov	r1, #0x01
	str	r1, [r0, #POWKEY1]
	mov	r1, #(_FS(POW_PC, MASK_010) | _FS(POW_CD, MASK_111))
	str	r1, [r0, #POWCON]
	mov	r1, #0xF4
	str	r1, [r0, #POWKEY2]
	/* core is now in nap mode. the core will be powered up 
         * by wakeup timer2
	 * ...
	 * stop timer2 */
	mov	r1, #0
	strh	r1, [r3]
	/* clear timer2 irq flag */
	mov	r1, #_BV(INT_WAKEUP_TIMER2)
	str	r1, [r0, #IRQCLR]
	/* in case of crystal loss we will execute an ISR (halt the target) */
	mov	r1, #_BV(INT_PLL_LOCK)
	str	r1, [r0, #IRQEN]
	/* the core is now running with external crystal on 326 kHz */
#endif
	/* set clock divider as requested by user */
	mov	r1, #0x01	/* Access to POWCON needs special sequence */
	str	r1, [r0, #POWKEY1]
	mov	r1, #POWCON_BOOT_CFG
	str	r1, [r0, #POWCON]
	mov	r1, #0xF4
	str	r1, [r0, #POWKEY2]

/** Setup stack pointers and set FIQ/IRQ control flags
 *
 * 1.) Enter ARM mode
 * 2.) Enable FIQ/IRQ-Flags (switch off FIQ/IRQ's)
 * 3.) Set up stack pointer
 *
 * Since FIQ/IRQ flags can only be set/cleared in priviledged modes we will
 * proceed the user mode (non priviledged) in the end of the stack
 * initialization.
 * The system mode cannot be entered by an exception and must be setup by CPSR
 * manipulation. No stack will be provided.
 *
 * See: ARM Compiler toolchain p. 55
 *
 * Note: (sp -> r14_svc, r14_irq ... ).
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

/** Clear .bss section
 *
 * Note: Requires ALIGN(4) since we write 4 byte wise
 */
	mov	r0, #0			/* r0 = 0 */
	ldr	r1, =__bss_start__	/* r1 = addr(start of .bss section) */
	ldr	r2, =__bss_end__	/* r2 = addr(end of .bss section) */
zero_bss:
        cmp	r1, r2			/* if (r1-r2)<0 (N, V flag): */
	stmltia	r1!, {r0} 		/* if cond then *(r1)=r0 then r1+=4 */
	blt	zero_bss		/* branch if condition */

/** Initialize variables in RAM
 *
 * Copy block .data from flash (LMA) starting after .text with size of .data
 * into RAM (VMA) starting at _data.
 *
 * Note: Requires ALIGN(4) since we copy 4 byte wise
 *
 */
	ldr	r0, =_data_load_start	/* r0 = addr(end of .text => fash) */
	ldr	r1, =_data		/* r1 = addr(start of .data => ram) */
	ldr	r2, =_data_load_end	/* r2 = addr(end of .data => ram) */
copy_data:
	cmp	r0, r2			/* if (r0-r2)<0 (N, V flag): */
	ldmltia	r0!, {r3}		/* if cond then r3=*(r0) then r0+=4 */
	stmltia	r1!, {r3}		/* if cond then *(r1)=r3 then r1+=4 */
	blt	copy_data		/* branch if condition */

/** Execute external initialization code and jump to main
 *
 */
	ldr	r0, =do_initcalls
	mov	lr, pc			/* set return address */
	bx	r0			/* jump to _init code (could be thumb)*/

	ldr	r0, =main
	mov	lr, pc			/* set return address */
	bx	r0			/* could be thumb - never return */

	b	.			/* should never come here */

.size    _init_target, . - _init_target
.endfunc


/** @} */

.end
