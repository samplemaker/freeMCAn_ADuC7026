/** \file firmware/timer1-get-duration.c
 * \brief  Get duration measurement timer has been running
 *
 * \author Copyright (C) 2010 samplemaker
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
 * \defgroup timer1_get_duration Measurement Timer ISR: Get Duration
 * \ingroup firmware_personality_groups
 *
 * @{
 */

#include "timer1-get-duration.h"
#include "timer1-measurement.h"


/** Get measurement time elapsed
 *
 * When an interrupt is recognised, the core allows the current instruction
 * to  complete. This might be even a LDM (load-multiple) of a long list
 * of registers. Therefore loading timer_count with LDRH Rd, [Rb, #6bit_offset]
 * is an atomic instruction and not interrupted by ISR_WAKEUP_TIMER2.
 */
uint16_t get_duration(void)
{
  return (orig_timer1_count - timer1_count);
}


/** @} */

/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
