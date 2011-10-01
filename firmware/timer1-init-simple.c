/** \file firmware/timer1-init-simple.c
 * \brief Timer init to simply periodically trigger timer ISR
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
 * \defgroup timer1_init_simple Timer init to simply periodically trigger timer ISR
 * \ingroup firmware_personality_groups
 *
 * Timer init to simply periodically trigger timer ISR.
 *
 * @{
 */

#include "aduc.h"
#include "init.h"
#include "reset.h"

#include "data-table.h"
#include "timer1-constants.h"
#include "timer1-measurement.h"
#include "packet-comm.h"






/** Set up our IO pins */
static
void __init timer1_simple_io_init(void)
{
 /* measurement in progress LED */
  /* configure P4.1 as GPIO: */
  GP4CON |= _FS(GP_SELECT_FUNCTION_Px1, MASK_00);
  /* configure P4.1 as output */
  GP4DAT |= _BV(GP_DATA_DIRECTION_Px1);
}
/** Put function into init section, register function pointer and
 *  execute function at start up
 */
module_init(timer1_simple_io_init, 5);



/** Configure 16 bit timer to trigger an ISR every second
 *
 * Configure "measurement in progress toggle LED-signal"
 */
void timer1_init(const uint16_t timer1_value)
{
  orig_timer1_count = timer1_value;
  timer1_count = timer1_value;

  /** Safeguard: We cannot handle 0 or 1 count measurements. */
  if (orig_timer1_count <= 1) {
    send_text("Unsupported timer value <= 1");
    soft_reset();
  }

 /** Configure 32 bit Timer2 (used for timer base) */

  /* - Select appropriate clock source
   * - Run timer in periodic mode (automatic reload from T2LD)
   */
  T2CON |= (_FS(TIMER2_PRESCALER, TIMER2_PRESCALER_VALUE) |
            _FS(TIMER2_CLKSOURCE, TIMER2_CLK)              |
            _BV(TIMER2_MODE) );
  /* Force downcount */
  T2CON &= ~_BV(TIMER2_COUNT_DIR);

  /* Timer compare match value */
  T2LD = TIMER2_LOAD_VALUE_DOWNCNT;
  T2CON |= _BV(TIMER2_ENABLE);
  /* Enable interrupt flag for Timer2 */
  IRQEN |= _BV(INT_WAKEUP_TIMER2);
}


void timer1_init_quick(void)
{
 // \todo
}


/** @} */

/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */

