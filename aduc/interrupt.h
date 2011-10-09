/** \file aduc/int.h
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
 *
 * \ingroup aduc_interrupt
 *
 *
 * @{
 */

#ifndef INT_H
#define INT_H

/* used by software interrupt handler */
#define SWI_ENABLE_IRQ   0x01
#define SWI_DISABLE_IRQ  0x02

#ifndef __ASSEMBLER__

#if !defined(STR) && !defined(STR1)
/* macros used for synthesizing asm inline macros */
#define STR1(x)  #x
#define STR(x)  STR1(x)
#endif

/** \brief Enable interrupts within non-/priviledged usermode
 *
 *  Trigger a software interrupt and switch to supervisor mode
 *  to manipulate the I-Flag in cpsr.
 *  This call is atomic.
 */
inline static
void enable_IRQs_usermode(void){
  asm volatile ( "SVC " STR(SWI_ENABLE_IRQ) ::);
}

/** \brief Disable interrupts within non-/priviledged usermode
 *
 *  Trigger a software interrupt and switch to supervisor mode
 *  to manipulate the I-Flag in cpsr.
 *  This call is atomic.
 */
inline static
void disable_IRQs_usermode(void){
  asm volatile ( "SVC " STR(SWI_DISABLE_IRQ) ::);
}

#endif

/** @} */

#endif  /* !INT_H */
