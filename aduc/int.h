/** \file ./int.h
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
 * \addtogroup LIB_IRQ
 *
 * @{
 */

#ifndef INT_H
#define INT_H

/* used by software interrupt handler */
#define ENABLE_GLOBALIRQ_BY_SWI   0x01
#define DISABLE_GLOBALIRQ_BY_SWI  0x02

#ifndef __ASSEMBLER__

/* macros used for synthesizing asm inline macros */
#define STR1(x)  #x
#define STR(x)  STR1(x)


/** \brief Enable interrupts within nonpriviledged usermode
 *
 *  Trigger a software interrupt and switch to supervisor mode
 *  to manipulate the I-Flag in cpsr.
 *  This call is atomic.
 */
inline static
void enable_IRQs_usermode(void){
  /* makes nothing but
   * asm volatile ("SVC 0xab" ::);
   */
  asm volatile ( STR(SVC ENABLE_GLOBALIRQ_BY_SWI) ::);
}

/** \brief Disable interrupts within nonpriviledged usermode
 *
 *  Trigger a software interrupt and switch to supervisor mode
 *  to manipulate the I-Flag in cpsr.
 *  This call is atomic.
 */
inline static
void disable_IRQs_usermode(void){
  /* makes nothing but
   * asm volatile ("SVC 0x0" ::);
   */
  asm volatile ( STR(SVC DISABLE_GLOBALIRQ_BY_SWI) ::);

}


/*
int atomic_inc(volatile int *ptr)
{
  int result, modified;
 
  asm volatile ("1:"                "\t@ loop label"    "\n\t"
                "ldrex %0,[%1]"     "\t@ load value"    "\n\t"
                "add   %0,%0,#1"    "\t@ increment it"  "\n\t"
                "strex %2,%0,[%1]"  "\t@ attempt store" "\n\t"
                "cmp   %2,#0"       "\t@ if not atomic" "\n\t"
                "bne   1b"          "\t@ then repeat"   "\n\t"
                : "=&r" (result)
                : "r" (ptr), "r" (modified)
                : "cc", "memory"
               );
  return result;
}

inline static
uint8_t atomic_swap (uint8_t value, volatile uint8_t *flag)
{
  register uint8_t tmp_reg;
  asm volatile( "\n\t"
                "swpb %[tmp_reg], %[value], [%[flag]] \n\t"
                : [flag] "=&r" (flag),
                  [tmp_reg] "=&r" (tmp_reg)
                : [value] "r" (value)
                : "memory"
              );
 return (tmp_reg);
}
*/

#endif

/** @} */

#endif  /* !INT_H */
