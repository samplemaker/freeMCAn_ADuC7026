/** \file firmware/perso-adc-int-mca-ext-trig.c
 * \brief Personality: MCA with internal ADC and external trigger
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
 * \defgroup perso_adc_int_mca_ext_trig Personality: MCA with internal ADC and external trigger
 * \ingroup firmware_personality_groups
 *
 * Internal ADC code.
 *
 * @{
 */


#include <stdlib.h>

/** Histogram element size */
#define ELEMENT_SIZE_IN_BYTES 3

#include "aduc7026.h"
#include "main.h"
#include "perso-adc-int-global.h"
#include "packet-comm.h"
#include "table-element.h"
#include "data-table.h"

#include "timer1-measurement.h"


/* \todo : comment following lines to have the "real working code" */
#include "timer1-constants.h"
#define DEBUG_ADC_TRIGGER 1



/** Number of elements in the histogram table */
#define MAX_COUNTER (1<<ADC_RESOLUTION)

/** Histogram table
 *
 * ATmega644P has 4Kbyte RAM.  When using 10bit ADC resolution,
 * MAX_COUNTER==1024 and 24bit values will still fit (3K table).
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
  ELEMENT_SIZE_IN_BYTES,
};


/** See * \see data_table */
PERSONALITY("adc-int-mca",
            2,0,
            1,
            sizeof(table),
            ELEMENT_SIZE_IN_BYTES);


#if DEBUG_ADC_TRIGGER
void ISR_WATCHDOG_TIMER3(void){
  GP1DAT ^= _BV(GP_DATA_OUTPUT_Px5);
  T3CLRI = 0x00;
}
inline static void 
adctest_init(void){
  /* clear watch dog and force down counting */
  T3CON &=~ ( _BV(TIMER3_COUNT_DIR) |
              _BV(TIMER3_SECURE) |
              _BV(TIMER3_WDT_ENABLE) );
  /* set prescaler and run timer in periodic mode
* (automatic reload from T3LD) */
  T3CON |= (_FS(TIMER3_PRESCALER, TIMER3_PRESCALER_VALUE) |
            _BV(TIMER3_MODE) );
  /* timer compare match value */
  T3LD = TIMER3_LOAD_VALUE_DOWNCNT;
  T3CON |= _BV(TIMER3_ENABLE);
  /* enable interrupt flag for Timer3 */
  IRQEN |= _BV(INT_WATCHDOG_TIMER3);
}
#endif


/** Initialize peripherals
 *
 * Configure peak hold capacitor reset pin.
 */
static
void __init personality_io_init(void)
{
  // \todo reset pin
}
/** Put function into init section, register function pointer and
 *  execute function at start up
 */
register_init5(personality_io_init);


/** AD conversion complete interrupt entry point
 *
 * This function is called when an A/D conversion has completed.
 * Update histogram
 * Discharge peak hold capacitor
 */
void ISR_ADC(void){
  /* pull pin to discharge peak hold capacitor                    */
  /** \todo worst case calculation: runtime & R7010 */
  // \todo
 
  /* starting from bit 16 the result is stored in ADCDAT.
     reading the ADCDATA also clears flag in ADCSTA */
  const uint32_t result =  ADCDAT;

  /* adjust to correct size */
  const uint16_t index = (result >> (16 + 12 - ADC_RESOLUTION));

 volatile table_element_t *element = &(table[index]);
  table_element_inc(element);

  /* set pin to GND and release peak hold capacitor   */
  // \todo

}
  

/** Programmable logic array used for edged triggering the ADC
 *
 * HCLK synchronous implementation for triggering the ADC
 * Element0: Logic function for generating the output pulse
 * Element4: Flip flop shifter for generating a one clock pulse
 * Element5: Input flip flop to suppress glitches (debouncer)
 *
 * \todo write driver for external D-Flipflop because ADUC7026 ADC
 * is level triggered (not edge) at #CONVstart
 */
inline static
void adc_int_trigger_src_conf(void)
{

  /* PLAs triggering the adc:
   * 0b0000: Element0  (BLOCK0)
   * 0b0001: Element1  (BLOCK0)
   * 0b1111: Element15 (BLOCK1)
   */
  PLAADC |= (_BV(PLA_ADC_CONV_START) |
             _FS(PLA_ADC_CONV_SRC, MASK_0000) );
  /* Configure PLA ELEMENT0 (BLOCK0)
   * - MUX3: Select MUX1, not GPIO (nothing to do)
   * - MUX1: Connect element 5 at MUX1
   * - MUX2: Select MUX0, not PLAIN
   * - MUX0: Connect element 4 at MUX0
   * - Select logical function of the block (B and not A)
   * - Bypass flip-flop (MUX4)
   */
  PLAELM0 |= (/*_BV(PLA_MUX3_CONTROL)             |*/
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
  PLAELM4 |= (/*_BV(PLA_MUX3_CONTROL)             |*/
              _FS(PLA_MUX1_CONTROL, MASK_10)      |
              _FS(PLA_LOOKUP_TABLE, MASK_1010) );
  /* Configure PLA ELEMENT5 (BLOCK0)
   * - MUX3: Select GPIO, not MUX1
   * - Select logical function of the block:
   *   B -> route MUX3 -> trigger on rising edge
   *   Not B -> trigger on falling edge
   * - Use flip-flop (MUX4) (nothing to do)
   */
  PLAELM5 |= (_BV(PLA_MUX3_CONTROL)               |
  #if ADC_TRIGGER_ON_RISING_EDGE
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
  #if DEBUG_ADC_TRIGGER
    GP1DAT |= _BV(GP_DATA_DIRECTION_Px5);
  #else
    GP1DAT &=~ _BV(GP_DATA_DIRECTION_Px5);
  #endif
  /* PLA-BLOCK0 clock source selection
   * Clock source:
   * HCLK: MASK_011
   * P0.5: MASK_000
   */
  PLACLK |= _FS(PLA_BLOCK0_CLK_SRC, MASK_011);

}


/** ADC initialisation and configuration
 *
 */
inline static
void adc_int_init(void)
{
  /* - Set clock speed (default value is 0b001 = fADC/2)
   * - ADC acquisition time (default value is 0b10 = eight clocks)
   * - Power control: set ADC to normal mode
   * - Set conversion mode:
   *     Single ended (default): 0b00
   *     (Vin- can be left floating)
   *     Pseudo differential   : 0b10
   *     (Ground must be connected to ADCNEG (Pin 9))
   * - Choose trigger source :
   *     #CONVstart:       MASK_000
   *     TIMER1:           MASK_001
   *     TIMER0:           MASK_010
   *     SOFTWARE SINGLE:  MASK_011
   *     SOFTWARE CONT:    MASK_100
   *     PLA:              MASK_101
   */
  ADCCON = (_FS(ADC_CLOCK_SPEED, MASK_001)     |
            _FS(ADC_ACQUISITION_TIME, MASK_10) |
            _BV(ADC_POWER_CONTROL)             |
            _FS(ADC_CONVERSION_MODE, MASK_00)  |
            _FS(ADC_TRIGGER_SOURCE, MASK_101)    );
  /* Use ADCbusy pin (NOTE: Conflicts with olimex board switch!)
   * ADCCON |= _BV(ADC_ENABLE_ADCBUSY);
   */

  /* Channel selection: ADC0 = 00000 */
  ADCCP = _FS(ADC_PCHANNEL_SELECTION, MASK_00000);
  /* Set bit for internal bandgap reference.
   * external reference  otherwise but must connect a voltage reference
   * to pin 68 (Vref) */
  REFCON |= _BV(REF_BANDGAP_ENABLE);
  /* Engage adc */
  ADCCON |=  _BV(ADC_ENABLE_CONVERION);
  /* Enable ADC IRQ */
  IRQEN |= _BV(INT_ADC_CHANNEL);
  /* Poll for 'result is ready':  while (!ADCSTA){};*/
}


/** ADC subsystem and trigger setup */
inline static
void adc_init(void)
{
  adc_int_trigger_src_conf();
  adc_int_init();
}


void personality_start_measurement_sram(void)
{
  const void *voidp = &pparam_sram.params[0];
  const uint16_t *timer1_value = voidp;
  adc_init();
  #if DEBUG_ADC_TRIGGER
    adctest_init();
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
