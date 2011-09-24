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

/* attribute used as ISR  replacement functions   */

#define __stub __attribute__((weak))

/*-----------------------------------------------------------------------------
 * Prototypes
 *-----------------------------------------------------------------------------
 */


/* Empty stubs. Real code may be implemented in another module.
 *
 */
void __stub ISR_ADC(void);
void __stub ISR_TIMER0(void);
void __stub ISR_TIMER1(void);
void __stub ISR_WAKEUP_TIMER2(void);
void __stub ISR_EXTINT0(void);
void __stub ISR_WATCHDOG_TIMER3(void);


/** \brief IRQ - ADC (empty stub)
 *
 *  Processing code for ADC IRQ
 */
void ISR_ADC(void){
}

/** \brief IRQ - TIMER0 (empty stub)
 *
 *  Processing code for TIMER0 IRQ
 */
void ISR_TIMER0(void){
}

/** \brief IRQ - TIMER1 (empty stub)
 *
 *  Processing code for TIMER1 IRQ
 */
void ISR_TIMER1(void){
}

/** \brief IRQ - TIMER2 (empty stub)
 *
 *  Processing code for TIMER2 IRQ
 */
void ISR_WAKEUP_TIMER2(void){
}

/** \brief IRQ - INT0 (empty stub)
 *
 *  Processing code for INT0 IRQ
 */
void ISR_EXTINT0(void){
}

/** \brief IRQ - INT_WATCHDOG_TIMER3 (empty stub)
 *
 *  Processing code for TIMER3 IRQ
 */
void ISR_WATCHDOG_TIMER3(void){
}


/** \brief IRQ - handler (coordinator) function
 *
 *  Redirects IRQ processing according to the IRQ-source (except SWI)
 */
void __attribute__ ((interrupt("IRQ"))) _irq_handler(void);
void _irq_handler(void)
{
  /* which interrupt is enabled and pending?
   */
  const uint32_t irq_status = IRQSTA;
  if (bit_is_set(irq_status, INT_ADC_CHANNEL)){
    ISR_ADC();
  };
  if (bit_is_set(irq_status, INT_TIMER0)){
    ISR_TIMER0();
    /* clear timer0 interrupt flag at eoi */
    T0CLRI = 0x00;
  };
  if (bit_is_set(irq_status, INT_TIMER1)){
    ISR_TIMER1();
    /* clear timer1 interrupt flag at eoi */
    T1CLRI = 0x00;
  };
  if (bit_is_set(irq_status, INT_WAKEUP_TIMER2)){
    ISR_WAKEUP_TIMER2();
    /* clear timer2 interrupt flag at eoi */
    T2CLRI = 0x00;
  };
  if (bit_is_set(irq_status, INT_WATCHDOG_TIMER3)){
    ISR_WATCHDOG_TIMER3();
    /* clear timer3 interrupt flag at eoi */
    T3CLRI = 0x00;
  };
  if (bit_is_set(irq_status, INT_EXTERNAL_IRQ0)){
    ISR_EXTINT0();
  };
}


/** Software interrupt handler. Enabling and disabling the global I-Flag
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
               "cmp    r0, #" STR(ENABLE_GLOBALIRQ_BY_SWI)  " \n\t"
               "beq swi_enable_irq                            \n\t"
               "cmp    r0, #" STR(DISABLE_GLOBALIRQ_BY_SWI) " \n\t"
               "beq swi_disable_irq                           \n\t"

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
               :: /* Nothing to do: 
                     No used registers, no operands!             */
  );
}

/** @} */
