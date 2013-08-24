/** \file firmware/perso-geiger-time-series.c
 * \brief Personality: Geiger Counter Recording Time Series
 *
 * \author Copyright (C) 2010 samplemaker
 * \author Copyright (C) 2010 Hans Ulrich Niedermann <hun@n-dimensional.de>
 * \author Copyright (C) 1998, 1999, 2000, 2007, 2008, 2009 Free Software Foundation, Inc.
 *         (for the assembly code in ts_init() to clear data_table)
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
 * \defgroup geiger_time_series Personality: Geiger Counter Recording Time Series
 * \ingroup firmware_personality_groups
 *
 * Geiger Counter recording time series
 *
 * @{
 */

#include <stddef.h>

#include "aduc.h"
#include "init.h"

/** Histogram element size */
#define BITS_PER_VALUE 24

#include "packet-comm.h"
#include "timer1-measurement.h"
#include "uart-printf.h"
#include "main.h"
#include "table-element.h"
#include "data-table.h"
#include "beep.h"

#define TOG_LED_TIME_BASE (GP4DAT ^= _BV(GP_DATA_OUTPUT_Px1))
#define TOG_LED_EVENT (GP4DAT ^= _BV(GP_DATA_OUTPUT_Px0))

#define RST_EOI_ENA (PLADIN |= _BV(1))
#define RST_EOI_DIS (PLADIN &=~ _BV(1))


#define DEBUG_TRIGGER 1

/*todo: need to initialize a bool with FALSE -> otherwise use bss */
static int gf_measurement_finished;


/** The table
 *
 * Note that we have the table location and size determined by the
 * linker script time-series-table.x.
 */
extern volatile table_element_t table[] asm("data_table");


/** End of the table: Still needs rounding */
extern volatile table_element_t data_table_end[];


/** Pseudo symbol - just use its address */
extern volatile char data_table_size[];


/** Data table info
 *
 * \see data_table
 */
data_table_info_t data_table_info = {
  /** Actual size of #data_table in bytes
   * We update this value whenever new time series data has been
   * recorded. The initial value is "one element".
   */
  sizeof(table[0]),
  /** Type of value table we send */
  VALUE_TABLE_TYPE_TIME_SERIES,
  /** Table element size */
  BITS_PER_VALUE
};


/** See * \see data_table */
PERSONALITY("geiger-time-series",
            2,0,
            1,
            0,/* should be  ((size_t)(&data_table_size)). see workaround */
            BITS_PER_VALUE);


/** End of the table: Never write to *table_cur when (table_cur>=table_end)! */
volatile table_element_t *volatile table_end =
  (table_element_t volatile *)((char *)data_table_end -
                                   (sizeof(table_element_t)-1));

/** Pointer to the current place to store the next value at */
volatile table_element_t *volatile table_cur = table;


/** Workaround
 *
 */
void __init personality_info_init(void)
{
  personality_info.sizeof_table = (size_t)(&data_table_size);
}
module_init(personality_info_init, 8);


/** Print some status messages for debugging */
void __init data_table_print_status(void)
{
#ifdef VERBOSE_STARTUP_MESSAGES
  uprintf("<data_table_print_status>");
  uprintf("%-25s %p", "table",      table);
  uprintf("%-25s %p", "table_cur",  table_cur);
  uprintf("%-25s %p", "table_end",  table_end);
  const size_t UV(sizeof_table) = ((char*)table_end) - ((char*)table_cur);
  uprintf("%-25s 0x%x = %d >= %d * %d",
          "table_end - table_cur",
          _UV(sizeof_table), _UV(sizeof_table),
          _UV(sizeof_table)/sizeof(*table_cur), sizeof(*table_cur));
  uprintf("</data_table_print_status>");
#endif
}
module_init(data_table_print_status, 8);


/** Initialize peripherals
 *
 * Set up output pins speaker and LEDs
 */
void __init personality_io_init(void)
{
  /* LED_EVENT (will be toggled when a GM event is detected) */
  /* configure P4.0 as GPIO */
  GP4CON |= _FS(GP_SELECT_FUNCTION_Px0, MASK_00);
  /* configure P4.0 as output */
  GP4DAT |= _BV(GP_DATA_DIRECTION_Px0);
}
module_init(personality_io_init, 5);


void __runRam ISR_PLA_INT0(void)
{
  _beep();
  TOG_LED_EVENT;

  if (table_cur < table_end) {
    table_element_inc(table_cur);
  }

  /* if a certain threshold is reached do s.th. nice */
  /*if (  table_element_cmp_eq(table_cur, 0xab)) {
    TOG_LED_TIME_BASE;
  }*/

  /* reset the PLA trigger latch */
  RST_EOI_ENA;
  RST_EOI_DIS;
}


volatile uint16_t timer1_count;
volatile uint16_t orig_timer1_count;


void ISR_WAKEUP_TIMER2(void)
{
  TOG_LED_TIME_BASE;

  if (!gf_measurement_finished) {
    /** We do not touch the measurement_finished flag ever again after
     * setting it. */
    timer1_count--;
    if (timer1_count == 0) {
      /* Timer has elapsed. Advance to next counter element in time
       * series, and restart the timer countdown. */
      table_cur++;
      if (table_cur < table_end) {
        data_table_info.size += sizeof(*table_cur);
        timer1_count = orig_timer1_count;
      } else {
        gf_measurement_finished = 1;
      }
    }
  }
  /* clear timer2 interrupt flag at eoi */
  T2CLRI = 0x00;
}



#if DEBUG_TRIGGER
#define TIMER3_INTERVAL 20000ULL // [us]
#include "set_timer.h"

void ISR_WATCHDOG_TIMER3(void){
  GP1DAT ^= _BV(GP_DATA_OUTPUT_Px5);
  /* clear timer3 interrupt flag at eoi */
  T3CLRI = 0x00;
}
inline static void
trig_test_init(void){
  /* clear TIMER3_WDT_ENABLE, TIMER3_COUNT_DIR
   * (force down counting) and TIMER3_SECURE
   * set prescaler and run timer in periodic mode
   * (automatic reload from T3LD)                  */
  T3CON = (_FS(TIMER3_PRESCALER, TIMER3_PRESCALER_VALUE) |
           _BV(TIMER3_MODE) );
  /* timer compare match value */
  T3LD = TIMER3_LOAD_VALUE_DOWNCNT;
  T3CON |= _BV(TIMER3_ENABLE);
  /* enable interrupt flag for Timer3 */
  IRQEN |= _BV(INT_WATCHDOG_TIMER3);
}
#endif




inline static
void trigger_src_conf(void)
{
  PLAIRQ = (_BV(PLA_IRQ0_ENA) |
            _FS(PLA_IRQ0_SRC, MASK_0001) );

  /* Configure PLA ELEMENT0 (BLOCK0)
   * - MUX3: Select MUX1, not GPIO (nothing to do)
   * - MUX1: Connect element 5 at MUX1
   * - MUX2: Select MUX0, not PLAIN
   * - MUX0: Connect element 4 at MUX0
   * - Select logical function of the block (B and not A)
   * - Bypass flip-flop (MUX4)
   */
  PLAELM0 = (/*_BV(PLA_MUX3_CONTROL)             |*/
             _FS(PLA_MUX1_CONTROL, MASK_10)      |
             _BV(PLA_MUX2_CONTROL)               |
             _FS(PLA_MUX0_CONTROL, MASK_10)      |
             _FS(PLA_LOOKUP_TABLE, MASK_0010)    |
             _BV(PLA_MUX4_CONTROL));
  /* Configure PLA ELEMENT4 (BLOCK0)
   * - MUX3: Select MUX1, not GPIO (nothing to do)
   * - MUX1: Connect element 5 at MUX1
   * - Select logical function of the block (B -> route MUX3)
   * - Use flip-flop (MUX4) (nothing to do)
   */
  PLAELM4 = (/*_BV(PLA_MUX3_CONTROL)             |*/
             _FS(PLA_MUX1_CONTROL, MASK_10)      |
             _FS(PLA_LOOKUP_TABLE, MASK_1010) );
  /* Configure PLA ELEMENT5 (BLOCK0)
   * - MUX3: Select GPIO, not MUX1
   * - Select logical function of the block:
   *   B -> route MUX3 -> trigger on rising edge
   *   Not B -> trigger on falling edge
   * - Use flip-flop (MUX4) (nothing to do)
   */
  PLAELM5 = (_BV(PLA_MUX3_CONTROL)               |
  #if TRIGGER_ON_RISING_EDGE
             _FS(PLA_LOOKUP_TABLE, MASK_1010)
  #else
             _FS(PLA_LOOKUP_TABLE, MASK_0101)
  #endif
             );
  /* configure P1.5 as GPIO and input for PLA5
   * may be configured as output and switched by software, timer
   * for testing.
   * \todo : configure as input
   */
  GP1CON |= _FS(GP_SELECT_FUNCTION_Px5, MASK_00);
  #if DEBUG_TRIGGER
    GP1DAT |= _BV(GP_DATA_DIRECTION_Px5);
  #else
    GP1DAT &=~ _BV(GP_DATA_DIRECTION_Px5);
  #endif

  /* Configure PLA ELEMENT1 (BLOCK0)
   * - MUX3: Select MUX1 not GPIO (nothing to do)
   * - MUX1: Connect element 3 at MUX1
   * - MUX2: Select PLAIN (RST at EOI), not MUX0 (nothing to do)
   * - MUX0: nothing to do
   * - Select logical function of the block (B and not A)
   * - Bypass flip-flop (MUX4)
   */
  PLAELM1 = (/*_BV(PLA_MUX3_CONTROL)               |*/
             _FS(PLA_MUX1_CONTROL, MASK_01)      |
             /*_BV(PLA_MUX2_CONTROL)               |*/
             /*_FS(PLA_MUX0_CONTROL, MASK_01)      |*/
             _FS(PLA_LOOKUP_TABLE, MASK_0010)    |
             _BV(PLA_MUX4_CONTROL));
  /* Configure PLA ELEMENT3 (BLOCK0)
   * - MUX3: Select MUX1, not GPIO (nothing to do)
   * - MUX1: Connect element 1 at MUX1
   * - MUX2: Select MUX0, not PLAIN
   * - MUX0: Connect element 0 at MUX0
   * - Select logical function of the block (logic OR)
   * - Bypass flip-flop (MUX4)
   */
  PLAELM3 = (/*_BV(PLA_MUX3_CONTROL)               |*/
             _FS(PLA_MUX1_CONTROL, MASK_00)      |
             _BV(PLA_MUX2_CONTROL)               |
             _FS(PLA_MUX0_CONTROL, MASK_00)      |
             _FS(PLA_LOOKUP_TABLE, MASK_1110)    |
             _BV(PLA_MUX4_CONTROL));

  /* PLA-BLOCK0 clock source selection
   * Clock source:
   * HCLK: MASK_011
   * P0.5: MASK_000
   */
  PLACLK |= _FS(PLA_BLOCK0_CLK_SRC, MASK_011);

  /* reset the PLA trigger latch */
  RST_EOI_ENA;
  RST_EOI_DIS;

  IRQEN |= _BV(INT_PLA_IRQ0);
}


void on_measurement_finished(void)
{
  beep_kill_all();
  timer1_init_quick();
}


void personality_start_measurement_sram(void)
{
  const void *voidp = &pparam_sram.params[0];
  const uint16_t *timer1_value = voidp;
  trigger_src_conf();

  #if DEBUG_TRIGGER
    trig_test_init();
  #endif

  timer1_init(*timer1_value);
}

/** @} */

/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */

