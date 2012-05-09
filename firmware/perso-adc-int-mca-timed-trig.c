/** \file firmware/perso-adc-int-mca-timed-trig.c
 * \brief Personality: MCA with internal ADC and timed trigger
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
 * \defgroup perso_adc_int_mca_timed_trig Personality: MCA with internal ADC and timed trigger
 * \ingroup firmware_personality_groups
 *
 * Internal ADC code.
 *
 * @{
 */


#include <stdlib.h>

/** Histogram element size */
#define ELEMENT_SIZE_IN_BYTES 3


#include "aduc.h"
#include "perso-adc-int-global.h"
#include "packet-comm.h"
#include "table-element.h"
#include "data-table.h"
#include "timer1-adc-trigger.h"
#include "main.h"


/** \bug adc-int-mca-timed does not work. Measurements lead to a reboot. */


/** Number of elements in the histogram table */
#define MAX_COUNTER (1<<ADC_RESOLUTION)


/** Histogram table
 *
 * For the definition of sizeof_table, see adc-int-histogram.c.
 *
 * \see data_table
 */
volatile table_element_t table[MAX_COUNTER] asm("data_table");


/** See * \see data_table */
data_table_info_t data_table_info = {
  /** Actual size of #data_table in bytes */
  sizeof(table),
  /** Type of value table we send */
  VALUE_TABLE_TYPE_HISTOGRAM,
  /** Table element size */
  ELEMENT_SIZE_IN_BYTES
};


/** See * \see data_table */
PERSONALITY("adc-int-mca-timed",
            2,2,
            10,
            sizeof(table),
            ELEMENT_SIZE_IN_BYTES);


/** AD conversion complete interrupt entry point
  *
  * This function is called when an A/D conversion has completed.
  * Downsampling of base analog samples and update of histogram table.
  * Actually one could implement a low pass filter here before
  * downsampling to fullfill shannons sample theoreme
  */
void ISR_ADC(void){
  /* downsampling of analog data via skip_samples */
  if (skip_samples == 0) {
    /* starting from bit 16 the result is stored in ADCDAT.
       reading the ADCDATA also clears flag in ADCSTA */
    const uint32_t result =  ADCDAT;
    /* adjust to correct size */
    const uint16_t index = (result >> (16 + 12 - ADC_RESOLUTION));
    volatile table_element_t *element = &(table[index]);
    table_element_inc(element);
    skip_samples = orig_skip_samples;
  } else {
    skip_samples--;
  }

  /* measurement duration */
  if (!measurement_finished) {
    timer1_count--;
    if (timer1_count == 0) {
      measurement_finished = 1;
    }
  }
}


/** Do nothing */
void on_measurement_finished(void)
{
}


/** @} */

/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
