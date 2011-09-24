/** \file aduc/target_init.S
 * \brief ADUC-port specific target initialization and IRQ functions
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
 * \defgroup aduc_target_init Startup code for ADUC bare silicon
 * \ingroup ADUC
 *
 * @{
 */

#include "aduc7026.h"
#include "reset.h"

.extern main
.extern _irq_handler
.extern _swi_handler
.extern do_initcalls

/** Stack sizes in operating modes
 *
 */
	.equ	STACKSIZE_UND, 4
	.equ	STACKSIZE_SVC, 256
	.equ	STACKSIZE_ABT, 4
	.equ	STACKSIZE_FIQ, 4
	.equ	STACKSIZE_IRQ, 256
	.equ	STACKSIZE_USR, 256

.code 32
.align 0
.section .vector, "ax"

/** Jump island exception vector table for operating modes
 *
 * See: ARM7TDMI Technical Reference Manual p. 67
 *
 * Note: In the ADUC7026 asic the FLASH is mirrored at ADDR
 *       0x00000 by default after reset (MMR-REMAP bit 0 unset).
 *       To not keep on executing in this address range lets jump
 *       to the flash adress range 0x80000 onwards. Therefore we
 *       have to reload the handler address from a "literal pool"
 *       rather than using a branch (b $targetaddr) instruction.
 *
 */
_vector:
	ldr	pc, init_target		@ Reset -> Supervisor
	ldr	pc, und_handler		@ Undefined Instruction -> Undefinded
	ldr	pc, swi_handler		@ Software Interrupt -> Supervisor
	ldr	pc, pabt_handler	@ Prefetch Abort -> Abort
	ldr	pc, dabt_handler	@ Data Abort -> Abort
	nop				@ Reserved -> Reserved
	ldr	pc, irq_handler		@ IRQ -> IRQ
	ldr	pc, fiq_handler		@ FIQ -> FIQ

/* Handler functions may be implemented in foreign modules
 * without doing any changes here
 */

init_target:	.word	_init_target
und_handler:	.word	_und_handler
swi_handler:	.word	_swi_handler
pabt_handler:	.word	_pabt_handler
dabt_handler:	.word	_dabt_handler
irq_handler:	.word	_irq_handler
fiq_handler:	.word	_fiq_handler

/* Power on reset init handled inside this file (non weak)     */

	.weak	_und_handler
	.set	_und_handler, _xcpt_endless
	.weak	_swi_handler
	.equ	_swi_handler, _xcpt_endless
	.weak	_pabt_handler
	.set	_pabt_handler, _xcpt_endless
	.weak	_dabt_handler
	.set	_dabt_handler, _xcpt_endless
	.weak	_irq_handler
	.equ	_irq_handler, _xcpt_endless
	.weak	_fiq_handler
	.set	_fiq_handler, _xcpt_endless

_xcpt_endless:
	b	_xcpt_endless

.ascii "freeMCAn ADUC / (C) 2011 samplemaker and Hans Ulrich Niedermann"

.size   _vector, . - _vector


.code 32
.align 4
.section .init_target, "ax"

.global _init_target
.func _init_target

_init_target:

/** Initialize / reset phase lock loop and power control
 *
 * See: ADUC7026 datasheet p. 55
 *
 */
#if RESET_POWER_AND_PLL_AT_BOOT
	ldr	r0, =__MMR_BASE
	mov	r1, #0xaa	@ Access to PLLCON needs special sequence
	str	r1, [r0, #PLLKEY1]
	mov	r1, #PLLCON_RESET_CFG
	str	r1, [r0, #PLLCON]
	mov	r1, #0x55
	str	r1, [r0, #PLLKEY2]
	mov	r1, #0x01	@ Access to POWCON needs special sequence
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

/** Clear .bss section
 *
 * Note: Requires ALIGN(4).
 */
	mov	r0, #0			@ r0 = 0
	ldr	r1, =__bss_start__	@ r1 = addr(start of .bss section)
	ldr	r2, =__bss_end__	@ r2 = addr(end of .bss section)
zero_bss:
        cmp	r1, r2			@ (r1 ==<> r2) ?
	strlo	r0, [r1], #4		@ if r1<r2: *(r1)=r0 then r1+=4
	blo	zero_bss		@ branch if condition lower

/** Initialize variables in RAM
 *
 * Copy block .data from flash (LMA) starting after .text with size of .data
 * into RAM (VMA) starting at _data.
 *
 * Note: Requires ALIGN(4).
 *
 */
	ldr	r0, =_data_load_start	@ r0 = addr(end of .text => fash)
	ldr	r1, =_data		@ r1 = addr(start of .data => ram)
	ldr	r2, =_data_load_end	@ r2 = addr(end of .data => ram)
copy_data:
	cmp	r0, r2			@ (r0 ==<> r2) ?
	ldrlo	r3, [r0], #4		@ if r0<r2: r3=*(r0) then r0+=4
	strlo	r3, [r1], #4		@ if r0<r2: *(r1)=r3 then r1+=4
	blo	copy_data		@ branch if condition lower

/** Execute initialization code and jump to main
 *
 */
	ldr	r0,=do_initcalls
	mov	lr,pc			@ set return address
	bx	r0			@ execute _init section code

	ldr	r0,=main
	mov	lr,pc			@ set return address
	bx	r0			@ could be thumb / never return

	b	.			@ should never come here

.size    _init_target, . - _init_target
.endfunc


/** @} */

.end
