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

#include <stdint.h>

#include "aduc.h"
#include "init.h"

#include "beep.h"

#define TIMER0_CLOCK_DIVISION_FACTOR 256000000ULL
#define TIMER0_INTERVAL BEEP_LENGTH
#include "set_timer.h"


#define PWM_DAT_0 (F_HCLK/(2 * BEEP_FREQUENCY))


/** Set up PWM for wave generator and timer0 for gating signal
 */
void __init beep_init_and_start(void)
{
  /** configure square wave generator
   */

  /* switching frequency (PWMDAT0 = F_HCLK / (2 × fPWM)) */
  PWMDAT0 = PWM_DAT_0;
  /* duty cycle in single update:                               *
   *                                                            *
   * High side: t0HH = (PWMDAT0 + 2(PWMCH0 − PWMDAT1)) / F_HCLK *
   * High side: t0HL = (PWMDAT0 − 2(PWMCH0 − PWMDAT1)) / F_HCLK *
   * Low side:  t0LH = (PWMDAT0 − 2(PWMCH0 + PWMDAT1)) / F_HCLK *
   * Low side:  t0LL = (PWMDAT0 + 2(PWMCH0 + PWMDAT1)) / F_HCLK *
   *                                                            *
   * tS = 2 × PWMDAT0 / F_HCLK                                  *
   *                                                            *
   * i.e. t0LH/tS = 1/2 − (PWMCH0 + PWMDAT1) / PWMDAT0          *
   *                                                            *
   * PWMCH0 = (PWM_DAT_0 >> 1) - PWM_DAT_0 * (dcyc[%]/100);     *
   *                                                            */
  PWMCH0 = (PWM_DAT_0 >> 1);
  /* clear sync interrupt */
  PWMSTA = _BV(PWM_PWMSYNCINT);
  /* single update mode, enable synchronization (PWMSYNC), enable PWM */
  PWMCON = (_BV(PWM_SYNC_EN) |
            _BV(PWM_EN)       );
  /* enable PWM0L output (clear bit PWM_0L_EN). disable all other */
  PWMEN = (_BV(PWM_0H_EN) |
           _BV(PWM_1L_EN) |
           _BV(PWM_1H_EN) |
           _BV(PWM_2L_EN) |
           _BV(PWM_2H_EN));

  /* configure P3.1 (PWM0L) as PWM */
  GP3CON |= _FS(GP_SELECT_FUNCTION_Px1, MASK_01);

  /** configure gating signal
   */

  /* set prescaler and run timer in periodic mode (automatic
     reload from T0LD) */
  T0CON = (_FS(TIMER0_PRESCALER, TIMER0_PRESCALER_VALUE) |
           _BV(TIMER0_MODE) );
  T0LD = TIMER0_LOAD_VALUE;
  /* enable interrupt flag for Timer0 */
  IRQEN |= _BV(INT_TIMER0);
}
module_init(beep_init_and_start, 7);


inline static
void beep_stop_and_reset(void)
{
  /* dcyc = 0% */
  PWMCH0 = (PWM_DAT_0 >> 1);
  /* ??? */
  T0CLRI = 0x00;
  /* stop gating signal (timer0) */
  T0CON &= ~_BV(TIMER0_ENABLE);
}


/** Gating signal elapsed
 *
 *  Stop and reset timer
 */
void __runRam ISR_TIMER0(void)
{
  beep_stop_and_reset();
}


/** Kill a running beep
 *
 * If CLI is called during a running beep the beep needs to be killed
 * explicitely otherwise the beep would keep on running for a infinite time
 */
void beep_kill_all(void)
{
  beep_stop_and_reset();
}


/** @} */

