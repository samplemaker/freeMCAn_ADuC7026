/** \file aduc/interrupt.c
 * \brief Interrupt handling
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
 * \defgroup aduc_interrupt Interrupt support (IRQ and SWI)
 * \ingroup ADUC
 *
 * Interrupt handler implementations for asynchronous hardware interrupt
 * requests (IRQ) and software interrupt (SWI)
 *
 * @{
 */


/*-----------------------------------------------------------------------------
 * Includes
 *-----------------------------------------------------------------------------
 */

#include <stdint.h>

#include "aduc.h"
#include "defs.h"


/*-----------------------------------------------------------------------------
 * Defines
 *-----------------------------------------------------------------------------
 */

/* weak attribute for implementing the ISR stubs */
#define __stub(default_handler) \
        __attribute__ ((weak, alias (STR(default_handler))))


/*-----------------------------------------------------------------------------
 * Prototypes
 *-----------------------------------------------------------------------------
 */

static void _isr_trap(void);


/* ISR handler interface pointing to _isr_trap
 *
 * Replacement code to serve the IRQ must be implemented in 
 * foreign modules.
 */
void ISR_ADC(void)                      __stub(_isr_trap);
void ISR_TIMER0(void)                   __stub(_isr_trap);
void ISR_TIMER1(void)                   __stub(_isr_trap);
void ISR_WAKEUP_TIMER2(void)            __stub(_isr_trap);
void ISR_WATCHDOG_TIMER3(void)          __stub(_isr_trap);
void ISR_PLL_LOCK(void)                 __stub(_isr_trap);
void ISR_PLA_INT0(void)                  __stub(_isr_trap);


/** Default interrupt service handler
 *
 * Endless loop if an ISR is called but no external code is linked
 * Note: You may check for the caller in the link register.
 *       Is ARM32 -> $ADDR = LR - 0x4
 *
 */
static void _isr_trap(void){ while (1){} }


/* IRQEN:  Ones indicate that the interrupt request from
 *         the source is unmasked (use this to enable IRQs)
 *
 * IRQCLR: Write ones to clear the corresponding bit in IRQEN
 *         (use this to mask an source - do not use IRQEN!)
 *
 * IRQSTA: Ones indicate that the sources have an interrupt
 *         enabled and pending. Use this in ISR for distribution.
 *
 * IRQSIG: Ones indicate that the IRQ source has an interrupt
 *         pending (regardless wheather it is masked or not)
 */

/** IRQ - handler coordinator
 *
 *  Redirects IRQ processing according to the IRQ-source
 */
void __attribute__ ((interrupt("IRQ"))) __runRam _irq_handler(void);

void _irq_handler(void)
{
  /* which interrupt is enabled and pending?
   */
  if (bit_is_set(IRQSTA, INT_ADC_CHANNEL)){
    ISR_ADC();
  }
  if (bit_is_set(IRQSTA, INT_TIMER0)){
    ISR_TIMER0();
    /* clear timer0 interrupt flag at eoi */
    T0CLRI = 0x00;
  }
  /* Note:
   * When using an asynchronous clock-to-clock timer, the
   * interrupt in the timer block may take more time to clear
   * than the time it takes for the code in the interrupt routine to
   * execute. Ensure that the interrupt signal is cleared before
   * leaving the interrupt service routine. This can be done by
   * checking the IRQSTA MMR.
   */
  if (bit_is_set(IRQSTA, INT_TIMER1)){
    ISR_TIMER1();
    /* clear timer1 interrupt flag at eoi */
    T1CLRI = 0x00;
  }
  if (bit_is_set(IRQSTA, INT_WAKEUP_TIMER2)){
    ISR_WAKEUP_TIMER2();
    /* clear timer2 interrupt flag at eoi */
    T2CLRI = 0x00;
  }
  if (bit_is_set(IRQSTA, INT_WATCHDOG_TIMER3)){
    ISR_WATCHDOG_TIMER3();
    /* clear timer3 interrupt flag at eoi */
    T3CLRI = 0x00;
  }
  if (bit_is_set(IRQSTA, INT_PLL_LOCK)){
    ISR_PLL_LOCK();
  }
  if (bit_is_set(IRQSTA, INT_PLA_IRQ0)){
    ISR_PLA_INT0();
  }
}


/** Software interrupt handler
 *
 *  Enable-/disable the global IRQ-Flag: \n
 *  1.) The I-Flag in cpsr_c cannot be written in user mode but only in
 *      a priviledged mode. \n
 *  2.) Switch to supervisor mode by software interrupt. \n
 *  3.) Entering the SWI-exception the processor does: \n
 *      3a.)  Save the address of the next instruction (return adr.)
 *            in the appropriate Link Register: (lr_svc = pc + 4) \n
 *      3b.)  spsr_svc = current cpsr \n
 *      3c.)  Overwrite M-field in cpsr with the svc mode bits \n
 *      3d.)  Force ARM state \n
 *      3e.)  Disable I & F - Flag in cpsr \n
 *      3f.)  Jump to exception vector address \n
 *  4.) The code gets the SWI argument and disables/enables the I-Flag \n
 *
 *  Note: (13 sp, 14 lr, 15 pc).
 *
 *  Nothing to clobber since not in user context
 *
 *  See: ARM Compiler toolchain p. 116
 *
 */
void __attribute__ ((naked)) _swi_handler(void);
void _swi_handler(void)
{
  asm volatile("\n\t"
               /* Save workspace and current return address */
               "stmfd	sp!,{r0-r12,lr}\n\t"
               /* Get old user mode cpsr from spsr_svc */
               "mrs	r1, spsr\n\t"
               /* SWI occurred in Thumb state? */
               "tst	r1, #" STR(T_FLAG) "\n\t"
               /* Yes: Load SVC instruction halfword */
               "ldrneh	r0, [lr,#-2]\n\t"
               /* Extract comment field */
               "bicne	r0, r0, #0xFF00\n\t"
               /* No: Occured in ARM state */
               "ldreq	r0, [lr,#-4]\n\t"
               /* Clear top 8 bits of SVC instruction */
               "biceq	r0, r0, #0xFF000000\n\t"
               "cmp	r0, #" STR(SWI_ENABLE_IRQ) "\n\t"
               "beq	swi_enable_irq\n\t"
               "cmp	r0, #" STR(SWI_DISABLE_IRQ) "\n\t"
               "beq	swi_disable_irq\n\t"

               /* exit including default case if SWI is unknown */
               "swi_end:\n\t"
               /* Store condition field with updated  I-Flag */
               "msr	spsr_c, r1\n\t"
               /* Return to instruction following the SVC instr.
                  ^ in this context means restore CPSR from SPSR */
               "ldmfd	sp!, {r0-r12,pc}^ \n\t"

               "swi_enable_irq:\n\t"
               "bic	r1, r1, #" STR(I_FLAG) "\n\t"
               "b	swi_end\n\t"

               "swi_disable_irq:\n\t"
               "orr	r1, r1, #" STR(I_FLAG) "\n\t"
               "b	swi_end\n\t"
               :: /* context is handled by code prologue and epilogue
                     therefore no clobbers and no need to write and
                     reload cached memory values  */
  );
}


/** @} */
