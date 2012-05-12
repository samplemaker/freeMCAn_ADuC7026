/** \file firmware/perso-adc-int-log-timed-trig.c
 * \brief Personality: Data logger with internal ADC and timed trigger
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
 * \defgroup perso_adc_int_log_timed_trig Personality: Data logger with internal ADC and timed trigger
 * \ingroup firmware_personality_groups
 *
 * Internal ADC based timed ADC sampling.
 *
 * @{
 */


#include <stdlib.h>

#include "aduc.h"
#include "init.h"

/** Histogram element size */
#define ELEMENT_SIZE_IN_BYTES 2

#include "perso-adc-int-global.h"
#include "packet-comm.h"
#include "table-element.h"
#include "data-table.h"

#include "timer1-adc-trigger.h"
#include "timer1-constants.h"
#include "main.h"


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
   * recorded. The initial value is "zero bytes" (elements).
   */
  0,
  /** Type of value table we send */
  VALUE_TABLE_TYPE_SAMPLES,
  /** Table element size */
  ELEMENT_SIZE_IN_BYTES
};


/** See * \see data_table */
PERSONALITY("adc-int-timed-sampling",
            0,2,
            10,
            0,
            ELEMENT_SIZE_IN_BYTES);


/** End of the table: Never write to *table_cur when (table_cur>=table_end)! */
volatile table_element_t *volatile table_end =
  (table_element_t volatile *)((char *)data_table_end -
                                   (sizeof(table_element_t)-1));

/** Pointer to the current place to store the next value at */
volatile table_element_t *volatile table_cur = table;


/* forward declaration */
inline static
void timer1_halt(void);


/** Workaround
 *
 */
void __init personality_info_init(void)
{
  personality_info.sizeof_table = (size_t)(&data_table_size);
}
module_init(personality_info_init, 8);


/** Print some status messages for debugging
 *
 * \bug (copied from geiger-time-series.c)
 */
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


/** AD conversion complete interrupt entry point
 *
 * This function is called when an A/D conversion has completed.
 * Downsampling of base analog samples and update of sample table.
 * Actually one could implement a low pass filter here before
 * downsampling to fullfill shannons sample theoreme
 */
void ISR_ADC(void){
  /* toggle a time base signal */
  TOG_LED_ISR;

  /* starting from bit 16 the result is stored in ADCDAT.
     reading the ADCDATA also clears flag in ADCSTA */
  const uint32_t result =  ADCDAT;

  /* downsampling of analog data */
  if (skip_samples == 0) {
    if (!measurement_finished) {
      const uint16_t value = (result >> (16 + 12 - ADC_RESOLUTION));
      *table_cur = value;
      table_cur++;
      data_table_info.size += sizeof(*table_cur);
      skip_samples = orig_skip_samples;
      if (table_cur >= table_end) {
        timer1_halt();
        /* tell main() that measurement is over                     */
        measurement_finished = 1;
      }
    }
  } else {
    skip_samples--;
  }
}


/** Switch off trigger to stop any sampling of the analog signal
 *
 *
 */
inline static
void timer1_halt(void)
{
  T1CON &= ~_BV(TIMER1_ENABLE);
}


/** Show the user that the measurement has been finished
 *
 *
 */
inline static
void timer1_init_quick(void)
{
/* \todo */
}


/** Callback */
void on_measurement_finished(void)
{
  /* alert user */
  timer1_init_quick();
}


/** @} */

/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
