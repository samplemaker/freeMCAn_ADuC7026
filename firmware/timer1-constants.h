/** \file firmware/timer1-constants.h
 * \brief Constants for setting up timer1
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
 * \defgroup timer1_constants Constants for setting up timer1
 * \ingroup firmware_generic
 * @{
 */

#ifndef TIMER1_CONSTANTS_H
#define TIMER1_CONSTANTS_H


#include "reset.h"

/**  Defines for TIMER0 (RTOS Timer)
 *
 * HCLK only
 */

/** Select a valid clock divider depending on interval and clock source */
#define TIMER0_CLOCK_DIVISION_FACTOR 256ULL

/** Timeout time                            */
#define TIMER0_INTERVAL 50ULL       // [ms]

/** Timer0 prescaler selection (16Bit timer)
 *
 * 0: No prescaling
 * 1: Divider=16
 * 2: Divider=256
 *
 */
#if   (TIMER0_CLOCK_DIVISION_FACTOR == 256ULL)
# define TIMER0_PRESCALER_VALUE 2
#elif (TIMER0_CLOCK_DIVISION_FACTOR == 16ULL)
# define TIMER0_PRESCALER_VALUE 1
#elif (TIMER0_CLOCK_DIVISION_FACTOR == 1ULL)
# define TIMER0_PRESCALER_VALUE 0
#else
# error Invalid TIMER0_CLOCK_DIVISION_FACTOR value!
#endif

/** Timer0 load value calculation for timer initialization
 *
 * Depends on timer Interval and divider choosen
 */
#define TIMER0_LOAD_VALUE                          \
  ( (TIMER0_INTERVAL)  *  ((F_HCLK) /              \
  ( (TIMER0_CLOCK_DIVISION_FACTOR) * 1000ULL) ) )

#if (TIMER0_LOAD_VALUE < 0x00FF)
# error TIMER0_LOAD_VALUE: too low => try to decrease timer0 divider
#endif
#if (TIMER0_LOAD_VALUE > 0xFF00)
# error TIMER0_LOAD_VALUE: too high => try to increase timer0 divider
#endif


/**  Defines for TIMER1 (General-Purpose Timer)
 *
 * 32 kHz external crystal, core clock frequency, or GPIO (P1.0 or P0.6)
 */

/** Select a valid clock divider depending on interval and clock source */
#define TIMER1_CLOCK_DIVISION_FACTOR_x1000 256000ULL

/** Select source to be connected to TIMER1 */
#define TIMER1_CLK TIMER1_CORE_CLK

/** Timeout time                            */
#define TIMER1_INTERVAL 1000ULL             // [ms]


/** TIMER1 prescaler selection (32Bit timer)
 *
 *   0: No prescaling
 *   4: Divider=16
 *   8: Divider=256
 *  15: Divider=32.768
 *
 */
#if   (TIMER1_CLOCK_DIVISION_FACTOR_x1000 == 256000ULL)
   #define TIMER1_PRESCALER_VALUE 8
#elif (TIMER1_CLOCK_DIVISION_FACTOR_x1000 == 16000ULL)
   #define TIMER1_PRESCALER_VALUE 4
#elif (TIMER1_CLOCK_DIVISION_FACTOR_x1000 == 1000ULL)
   #define TIMER1_PRESCALER_VALUE 0
#elif (TIMER1_CLOCK_DIVISION_FACTOR_x1000 == 32768ULL)
   #define TIMER1_PRESCALER_VALUE 15
#else
   #error Invalid TIMER1_CLOCK_DIVISION_FACTOR_x1000 value!
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
    ( (TIMER1_INTERVAL)  *  ((F_XTAL) /                   \
    ( (TIMER1_CLOCK_DIVISION_FACTOR_x1000) ) ) )
#elif (TIMER1_CLK == TIMER1_CORE_CLK)
  #define TIMER1_LOAD_VALUE_DOWNCNT                       \
    ( (TIMER1_INTERVAL)  *  ((F_UCLK) /                   \
    ( (TIMER1_CLOCK_DIVISION_FACTOR_x1000) ) ) )
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
#define TIMER2_CLOCK_DIVISION_FACTOR_x1000 256000ULL

/** Select source to be connected to timer2 */
#define TIMER2_CLK TIMER2_CORE_CLK

/** Timeout time                            */
#define TIMER2_INTERVAL 1000ULL             // [ms]


/** Timer2 prescaler selection (32Bit timer)
 *
 *   0: No prescaling
 *   4: Divider=16
 *   8: Divider=256
 *  15: Divider=32.768
 *
 */
#if   (TIMER2_CLOCK_DIVISION_FACTOR_x1000 == 256000ULL)
   #define TIMER2_PRESCALER_VALUE 8
#elif (TIMER2_CLOCK_DIVISION_FACTOR_x1000 == 16000ULL)
   #define TIMER2_PRESCALER_VALUE 4
#elif (TIMER2_CLOCK_DIVISION_FACTOR_x1000 == 1000ULL)
   #define TIMER2_PRESCALER_VALUE 0
#elif (TIMER2_CLOCK_DIVISION_FACTOR_x1000 == 32768ULL)
   #define TIMER2_PRESCALER_VALUE 15
#else
   #error Invalid TIMER2_CLOCK_DIVISION_FACTOR_x1000 value!
#endif


/** Timer2 load value calculation for timer initialization
 *
 * Depends on various clock sources connected to the timer
 * Depends on timer Interval and divider choosen
 */
#if   (TIMER2_CLK == TIMER2_EXT_XTAL)
  #define TIMER2_LOAD_VALUE_DOWNCNT                       \
    ( (TIMER2_INTERVAL)  *  ((F_XTAL) /                   \
    ( (TIMER2_CLOCK_DIVISION_FACTOR_x1000) ) ) )
#elif (TIMER2_CLK == TIMER2_INT_OSC)
  #define TIMER2_LOAD_VALUE_DOWNCNT                       \
    ( (TIMER2_INTERVAL)  *  ((F_OSC) /                    \
    ( (TIMER2_CLOCK_DIVISION_FACTOR_x1000) ) ) )
#elif (TIMER2_CLK == TIMER2_CORE_CLK)
  #define TIMER2_LOAD_VALUE_DOWNCNT                       \
    ( (TIMER2_INTERVAL)  *  ((F_UCLK) /                   \
    ( (TIMER2_CLOCK_DIVISION_FACTOR_x1000) ) ) )
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
#define TIMER3_CLOCK_DIVISION_FACTOR 1ULL

/** Timeout time                            */
#define TIMER3_INTERVAL 50ULL              // [ms]

/** Timer3 prescaler selection (16Bit timer)
 *
 *   0: No prescaling
 *   1: Divider=16
 *   2: Divider=256
 *
 */
#if   (TIMER3_CLOCK_DIVISION_FACTOR == 256ULL)
# define TIMER3_PRESCALER_VALUE 2
#elif (TIMER3_CLOCK_DIVISION_FACTOR == 16ULL)
# define TIMER3_PRESCALER_VALUE 1
#elif (TIMER3_CLOCK_DIVISION_FACTOR == 1ULL)
# define TIMER3_PRESCALER_VALUE 0
#else
# error Invalid TIMER3_CLOCK_DIVISION_FACTOR value!
#endif

/** Timer3 load value calculation for timer initialization
 *
 * Depends on timer Interval and divider choosen
 */
#define TIMER3_LOAD_VALUE_DOWNCNT                  \
  ( (TIMER3_INTERVAL)  *  ((F_OSC) /               \
  ( (TIMER3_CLOCK_DIVISION_FACTOR) * 1000ULL ) ) )

/** Timer3 load value calculation for timer initialization
 *
 * Depends on the timer Interval and divider
 *
 */
#if (TIMER3_LOAD_VALUE_DOWNCNT < 0x00FFULL)
# error TIMER3_LOAD_VALUE_DOWNCNT: too low
#endif
#if (TIMER3_LOAD_VALUE_DOWNCNT > 0xFF00ULL)
# error TIMER3_LOAD_VALUE_DOWNCNT: too high
#endif

#define TIMER3_LOAD_VALUE_UPCNT (0xFFFFULL - TIMER3_LOAD_VALUE_DOWNCNT)


#endif /* !TIMER1_CONSTANTS_H */


/** @} */


/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
