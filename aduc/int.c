/** \file aduc/int.c
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
#include "int.h"

/*-----------------------------------------------------------------------------
 * Defines
 *-----------------------------------------------------------------------------
 */

/* attribute used for ISR replacement functions */
#define __isr_stub(replacement) __attribute__ ((weak, alias (STR(replacement))));

/*-----------------------------------------------------------------------------
 * Prototypes
 *-----------------------------------------------------------------------------
 */

void ISR_TRAP(void);


/* Empty stubs for ISR code pointing to ISR_TRAP
 *
 * The real code must be implemented elsewhere
 */
void ISR_ADC()             __isr_stub(ISR_TRAP)
void ISR_TIMER0()          __isr_stub(ISR_TRAP)
void ISR_TIMER1()          __isr_stub(ISR_TRAP)
void ISR_WAKEUP_TIMER2()   __isr_stub(ISR_TRAP)
void ISR_EXTINT0()         __isr_stub(ISR_TRAP)
void ISR_WATCHDOG_TIMER3() __isr_stub(ISR_TRAP)


/* ISR trap if no external modul is specified but IRQ is
 * unmasked */
void ISR_TRAP(void){
  while (1){}
}


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

/** \brief IRQ - handler (coordinator) function
 *
 *  Redirects IRQ processing according to the IRQ-source (except SWI)
 */
void __attribute__ ((interrupt("IRQ"))) _irq_handler(void);
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
  if (bit_is_set(IRQSTA, INT_EXTERNAL_IRQ0)){
    ISR_EXTINT0();
  }
}


/** Software interrupt handler. Enabling and disabling the global I-Flag
 *
 *  1.) The I-Flag in cpsr_c cannot be written in user mode but only in
 *	a priviledged mode. \n
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
               /* Save workspace and current return address      */
               "stmfd  sp!,{r0-r12,lr}                        \n\t"
               /* Get old user mode cpsr from spsr_svc           */
               "mrs    r1, spsr                               \n\t"
               /* SWI occurred in Thumb state?                   */
               "tst    r1, #" STR(T_FLAG)                   " \n\t"
               /* Yes: Load SVC instruction halfword             */
               "ldrneh r0, [lr,#-2]                           \n\t"
               /* Extract comment field                          */
               "bicne  r0, r0, #0xFF00                        \n\t"
               /* No: Occured in ARM state                       */
               "ldreq  r0, [lr,#-4]                           \n\t"
               /* Clear top 8 bits of SVC instruction            */
               "biceq  r0, r0, #0xFF000000                    \n\t"
               "cmp    r0, #" STR(SWI_ENABLE_IRQ)           " \n\t"
               "beq swi_enable_irq                            \n\t"
               "cmp    r0, #" STR(SWI_DISABLE_IRQ)          " \n\t"
               "beq swi_disable_irq                           \n\t"

               /* exit including default case if SWI is unknown  */
               "swi_end:                                      \n\t"
               /* Store condition field with updated  I-Flag     */
               "msr    spsr_c, r1                             \n\t"
               /* Return to instruction following the SVC instr.
                  ^ in this context means restore CPSR from SPSR */
               "ldmfd  sp!, {r0-r12,pc}^                      \n\t"

               "swi_enable_irq:                               \n\t"
               "bic    r1, r1, #" STR(I_FLAG)               " \n\t"
               "b      swi_end                                \n\t"

               "swi_disable_irq:                              \n\t"
               "orr    r1, r1, #" STR(I_FLAG)               " \n\t"
               "b      swi_end                                \n\t"
               :: /* Nothing to do since complete context is
                     handled by code entry and exit        */
  );
}

/** @} */
