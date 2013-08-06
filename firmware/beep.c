/** \file firmware/beep.c
 * \brief Providing a "beep" loudspeaker function
 *
 * \author Copyright (C) 2011 samplemaker
 * \author Copyright (C) 2010 Hans Ulrich Niedermann <hun@n-dimensional.de>
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
 * Providing a "beep" loudspeaker function
 *
 * @{
 */

#include "aduc.h"
#include "init.h"

#include "beep.h"

/** Set up timer2 for wave generator and timer0 for gating signal
 */
void __init beep_init_and_start(void)
{
  /** configure square wave generator (timer2)
   */
 
  /** configure gating signal (timer0)
   */
}
module_init(beep_init_and_start, 7);

inline static
void timer0_2_stop_and_reset(void)
{
  /* stop gating signal (timer0) */

  /* reset timer value */

  /* switch off square wave generator (timer2) */

}


/** Gating signal elapsed (timer0)
 *
 *  Stop and reset timer0 & 2
 */
/*ISR_TIMERxx
{
  timer0_2_stop_and_reset();
}*/


/** Kill a running beep
 *
 * If cli() is called during a running beep the beep needs to be killed
 * explicitely otherwise the beep would keep on running for a infinite time
 */
void beep_kill_all(void)
{
  timer0_2_stop_and_reset();

}


/** @} */

