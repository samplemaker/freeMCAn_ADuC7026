/** \file set_timer.h
 * \brief Timer macros
 *
 * \author Copyright (C) 2012 samplemaker
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
 * \defgroup settimer Timer macros
 * \ingroup Library
 *
 *
 *  \code
 *  #include "clock.h"
 *  #define TIMER0_CLOCK_DIVISION_FACTOR 256000000ULL
 *  #define TIMER0_INTERVAL 50000ULL
 *  #define TIMER1_CLOCK_DIVISION_FACTOR 256000000ULL
 *  #define TIMER1_CLK TIMER1_CORE_CLK
 *  #define TIMER1_INTERVAL 1000000ULL
 *  #define TIMER2_CLOCK_DIVISION_FACTOR 256000000ULL
 *  #define TIMER2_CLK TIMER2_CORE_CLK
 *  #define TIMER2_INTERVAL 1000000ULL
 *  #define TIMER3_CLOCK_DIVISION_FACTOR 16000000ULL
 *  #define TIMER3_INTERVAL 1000000ULL
 *  #include "set_timer.h"
 *  \endcode
 *
 *  Set_timer.h calculates all necessary register settings to setup
 *  timer0 .. 3. All calculations are done using the C preprocessor
 *
 *  To use this macro timer division factor, timer clock source,
 *  the timer interval and the cpu core frequency must be
 *  defined in advance. The cpu frequency can be included with "clock.h"
 *
 *  If nothing is defined the macro will setup default values for all
 *  four timers.
 *
 *  Example usage:
 *
 *  \code
 *
 * \endcode
 *
 */

#ifndef SET_TIMER_H
#define SET_TIMER_H


/** \addtogroup settimer
 * @{ */


/** @} */



/**  Defines for TIMER0 (RTOS Timer)
 *
 * HCLK only
 */

/** Select a valid clock divider depending on interval and clock source */
#ifndef TIMER0_CLOCK_DIVISION_FACTOR
  #define TIMER0_CLOCK_DIVISION_FACTOR 256000000ULL
#endif

/** Timeout time */
#ifndef TIMER0_INTERVAL
  #define TIMER0_INTERVAL 50000ULL // [us]
#endif

/** Timer0 prescaler selection (16Bit timer)
 *
 * 0: No prescaling
 * 1: Divider=16
 * 2: Divider=256
 *
 */
#if   (TIMER0_CLOCK_DIVISION_FACTOR == 256000000ULL)
  #define TIMER0_PRESCALER_VALUE 2
#elif (TIMER0_CLOCK_DIVISION_FACTOR == 16000000ULL)
  #define TIMER0_PRESCALER_VALUE 1
#elif (TIMER0_CLOCK_DIVISION_FACTOR == 1000000ULL)
  #define TIMER0_PRESCALER_VALUE 0
#else
  #error Invalid TIMER0_CLOCK_DIVISION_FACTOR value!
#endif

/** Timer0 load value calculation for timer initialization
 *
 * Depends on timer Interval and divider choosen
 */
#define TIMER0_LOAD_VALUE                         \
   ( ((TIMER0_INTERVAL) * (F_HCLK)) /              \
     (TIMER0_CLOCK_DIVISION_FACTOR) )

#if (TIMER0_LOAD_VALUE < 0x00FF)
  #error TIMER0_LOAD_VALUE: too low => try to decrease timer0 divider
#endif
#if (TIMER0_LOAD_VALUE > 0xFF00)
  #error TIMER0_LOAD_VALUE: too high => try to increase timer0 divider
#endif


/**  TIMER1 (General-Purpose Timer)
 *
 */

/** Select a valid clock divider depending on interval and clock source */
#ifndef TIMER1_CLOCK_DIVISION_FACTOR
  #define TIMER1_CLOCK_DIVISION_FACTOR 256000000ULL
#endif

/** Select source to be connected to TIMER1
 *  32 kHz external crystal, core clock frequency, or GPIO (P1.0 or P0.6)
 */
#ifndef TIMER1_CLK
  #define TIMER1_CLK TIMER1_CORE_CLK
#endif

/** Timeout time */
#ifndef TIMER1_INTERVAL
  #define TIMER1_INTERVAL 100000ULL // [us]
#endif


/** TIMER1 prescaler selection (32Bit timer)
 *
 *   0: No prescaling
 *   4: Divider=16
 *   8: Divider=256
 *  15: Divider=32.768
 *
 */
#if   (TIMER1_CLOCK_DIVISION_FACTOR == 256000000ULL)
   #define TIMER1_PRESCALER_VALUE 8
#elif (TIMER1_CLOCK_DIVISION_FACTOR == 16000000ULL)
   #define TIMER1_PRESCALER_VALUE 4
#elif (TIMER1_CLOCK_DIVISION_FACTOR == 1000000ULL)
   #define TIMER1_PRESCALER_VALUE 0
#elif (TIMER1_CLOCK_DIVISION_FACTOR == 32768000ULL)
   #define TIMER1_PRESCALER_VALUE 15
#else
   #error Invalid TIMER1_CLOCK_DIVISION_FACTOR value!
#endif


/** TIMER1 load value calculation for timer initialization
 *
 * Depends on various clock sources connected to the timer
 * Depends on timer Interval and divider choosen
 *
 * Note: If the clock source is a GPIO the user must provide 
 *       these macros himself
 */
#if   (TIMER1_CLK == TIMER1_EXT_XTAL)
  #define TIMER1_LOAD_VALUE_DOWNCNT                       \
    ( ((TIMER1_INTERVAL) * (F_XTAL)) /                    \
      (TIMER1_CLOCK_DIVISION_FACTOR) )
#elif (TIMER1_CLK == TIMER1_CORE_CLK)
  #define TIMER1_LOAD_VALUE_DOWNCNT                       \
    ( ((TIMER1_INTERVAL) * (F_HCLK)) /                    \
      (TIMER1_CLOCK_DIVISION_FACTOR) )
#endif

#if (TIMER1_LOAD_VALUE_DOWNCNT < 0x000000FFULL)
  #error TIMER1_LOAD_VALUE_DOWNCNT: too low
#endif
#if (TIMER1_LOAD_VALUE_DOWNCNT > 0xFF000000ULL)
  #error TIMER1_LOAD_VALUE_DOWNCNT: too high
#endif

#define TIMER1_LOAD_VALUE_UPCNT (0xFFFFFFFFULL - TIMER1_LOAD_VALUE_DOWNCNT)


/**  Defines for TIMER2 (Wake-Up Timer)
 *
 * Internal Oscillator, External Crystal or HCLK
 */

/** Select a valid clock divider depending on interval and clock source */
#ifndef TIMER2_CLOCK_DIVISION_FACTOR
  #define TIMER2_CLOCK_DIVISION_FACTOR 256000000ULL
#endif

/** Select source to be connected to TIMER2 */
#ifndef TIMER2_CLK
  #define TIMER2_CLK TIMER2_CORE_CLK
#endif

/** Timeout time */
#ifndef TIMER2_INTERVAL
  #define TIMER2_INTERVAL 1000000ULL // [us]
#endif

/** Timer2 prescaler selection (32Bit timer)
 *
 *   0: No prescaling
 *   4: Divider=16
 *   8: Divider=256
 *  15: Divider=32.768
 *
 */
#if   (TIMER2_CLOCK_DIVISION_FACTOR == 256000000ULL)
   #define TIMER2_PRESCALER_VALUE 8
#elif (TIMER2_CLOCK_DIVISION_FACTOR == 16000000ULL)
   #define TIMER2_PRESCALER_VALUE 4
#elif (TIMER2_CLOCK_DIVISION_FACTOR == 1000000ULL)
   #define TIMER2_PRESCALER_VALUE 0
#elif (TIMER2_CLOCK_DIVISION_FACTOR == 32768000ULL)
   #define TIMER2_PRESCALER_VALUE 15
#else
   #error Invalid TIMER2_CLOCK_DIVISION_FACTOR value!
#endif


/** Timer2 load value calculation for timer initialization
 *
 * Depends on various clock sources connected to the timer
 * Depends on timer Interval and divider choosen
 */
#if   (TIMER2_CLK == TIMER2_EXT_XTAL)
  #define TIMER2_LOAD_VALUE_DOWNCNT                       \
    ( ((TIMER2_INTERVAL) * (F_XTAL)) /                    \
      (TIMER2_CLOCK_DIVISION_FACTOR) )
#elif (TIMER2_CLK == TIMER2_INT_OSC)
  #define TIMER2_LOAD_VALUE_DOWNCNT                       \
    ( ((TIMER2_INTERVAL) * (F_OSC)) /                     \
      (TIMER2_CLOCK_DIVISION_FACTOR) )
#elif (TIMER2_CLK == TIMER2_CORE_CLK)
  #define TIMER2_LOAD_VALUE_DOWNCNT                       \
    ( ((TIMER2_INTERVAL) * (F_HCLK)) /                    \
      (TIMER2_CLOCK_DIVISION_FACTOR) )
#endif

#if (TIMER2_LOAD_VALUE_DOWNCNT < 0x000000FFULL)
  #error TIMER2_LOAD_VALUE_DOWNCNT: too low
#endif
#if (TIMER2_LOAD_VALUE_DOWNCNT > 0xFF000000ULL)
  #error TIMER2_LOAD_VALUE_DOWNCNT: too high
#endif

#define TIMER2_LOAD_VALUE_UPCNT (0xFFFFFFFFULL - TIMER2_LOAD_VALUE_DOWNCNT)


/**  Defines for TIMER3 (WatchDog-Timer)
 *
 *
 */

/** Select a valid clock divider depending on interval and clock source */
#ifndef TIMER3_CLOCK_DIVISION_FACTOR
  #define TIMER3_CLOCK_DIVISION_FACTOR 1000000ULL
#endif

/** Timeout time */
#ifndef TIMER3_INTERVAL
  #define TIMER3_INTERVAL 50000ULL // [us]
#endif

/** Timer3 prescaler selection (16Bit timer)
 *
 *   0: No prescaling
 *   1: Divider=16
 *   2: Divider=256
 *
 */
#if   (TIMER3_CLOCK_DIVISION_FACTOR == 256000000ULL)
  #define TIMER3_PRESCALER_VALUE 2
#elif (TIMER3_CLOCK_DIVISION_FACTOR == 16000000ULL)
  #define TIMER3_PRESCALER_VALUE 1
#elif (TIMER3_CLOCK_DIVISION_FACTOR == 1000000ULL)
  #define TIMER3_PRESCALER_VALUE 0
#else
  #error Invalid TIMER3_CLOCK_DIVISION_FACTOR value!
#endif

/** Timer3 load value calculation for timer initialization
 *
 * Depends on timer Interval and divider choosen
 */
#define TIMER3_LOAD_VALUE_DOWNCNT                  \
  ( ((TIMER3_INTERVAL) *  (F_OSC)) /               \
    (TIMER3_CLOCK_DIVISION_FACTOR) )

/** Timer3 load value calculation for timer initialization
 *
 * Depends on the timer Interval and divider
 *
 */
#if (TIMER3_LOAD_VALUE_DOWNCNT < 0x00FFULL)
  #error TIMER3_LOAD_VALUE_DOWNCNT: too low
#endif
#if (TIMER3_LOAD_VALUE_DOWNCNT > 0xFF00ULL)
  #error TIMER3_LOAD_VALUE_DOWNCNT: too high
#endif

#define TIMER3_LOAD_VALUE_UPCNT (0xFFFFULL - TIMER3_LOAD_VALUE_DOWNCNT)


#endif /* !SET_TIMER_H */

