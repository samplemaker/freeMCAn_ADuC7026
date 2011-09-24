/** \file firmware/timer1-adc-trigger.c
 * \brief Timer hardware directly triggering ADC
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
 * \defgroup timer1_adc_trigger Timer hardware directly triggering ADC
 * \ingroup firmware_personality_groups
 *
 * Timer hardware directly triggering ADC
 *
 * @{
 */


/* #define TIMEBASE */

#include "aduc.h"
#include "init.h"

#include "main.h"
#include "data-table.h"
#include "perso-adc-int-global.h"
#include "timer1-adc-trigger.h"
#include "timer1-constants.h"
#include "packet-comm.h"


/** timer counter
 *
 * Initialized once by main() with value received from host
 * controller. Never touched by main() again after starting the timer
 * interrupt.
 *
 * Timer interrupt handler has exclusive access to read/writes
 * timer1_count to decrement, once the timer ISR has been enabled.
 */
volatile uint16_t timer1_count;


/** timer counter */
volatile uint16_t timer1_count;


/** Original timer count received in the command.
 *
 * Used later for determining how much time has elapsed yet. Written
 * once only, when the command has been received.
 */
volatile uint16_t orig_timer1_count;


/** FIXME
 *
 * Is sent by hostware. Number of dropped analog samples (downsampling of
 * analog signal sampled with timer1 time base)
 */
volatile uint16_t orig_skip_samples;
volatile uint16_t skip_samples;


/** Set up our IO pins */
void timer1_adc_trigger_io_init(void)
{
   /* \todo */
}
/** Put function into init section, register function pointer and
 *  execute function at start up
 */
register_init5(timer1_adc_trigger_io_init);


/** Configure 16 bit timer to trigger an ISR every 0.1 second
 *
 * Configure "measurement in progress toggle LED-signal"
 */
void timer1_init(void)
{
  /** configure 32 bit Timer1  */

  /* - Select appropriate clock source
   * - Run timer in periodic mode (automatic reload from T1LD)
   */
  T1CON |= (_FS(TIMER1_PRESCALER, TIMER1_PRESCALER_VALUE) |
            _FS(TIMER1_CLKSOURCE, TIMER1_CLK)             |
            _BV(TIMER1_MODE) );
  /* Force downcount, no capture */
  T1CON &= ~ (_BV(TIMER1_COUNT_DIR) | _BV(TIMER1_CAPTURE_ENABLE));

  /* Timer compare match value */
  T1LD = TIMER1_LOAD_VALUE_DOWNCNT;
  T1CON |= _BV(TIMER1_ENABLE);
  /* Enable interrupt flag for Timer1 */
  //IRQEN |= _BV(INT_TIMER1);
  IRQCLR |= _BV(INT_TIMER1);
}


/** \todo Should give out a reasonable value */
uint16_t get_duration(void)
{
  return 0;
}


/** ADC initialisation and configuration
 *
 * ADC configured as auto trigger
 * Trigger source compare register B
 * Use external analog reference AREF at PIN 32
 * AD input channel on Pin 40 ADC0
 */
inline static
void adc_init(void)
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
            _FS(ADC_TRIGGER_SOURCE, MASK_001)    );
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


/** \bug Handle two uint16_t values from parameters: measurement
 *       duration and skip_samples.
 */
void personality_start_measurement_sram(void)
{
  size_t ofs = 0;

  if (personality_info.param_data_size_timer_count == 2) {
    const void *timer1_count_vp = &pparam_sram.params[ofs];
    const uint16_t *timer1_count_p = timer1_count_vp;
    orig_timer1_count = *timer1_count_p;
    timer1_count = *timer1_count_p;

    /** Safeguard: We cannot handle 0 or 1 count measurements.
     *
     * Enable this if you do something with get_duration() above.
     *
    if (orig_timer1_count <= 1) {
      send_text_P(PSTR("Unsupported timer value <= 1"));
      soft_reset();
    }
    */

    ofs += 2;
  }

  if (personality_info.param_data_size_skip_samples == 2) {
    const void *skip_samples_vp = &pparam_sram.params[ofs];
    const uint16_t *skip_samples_p = skip_samples_vp;
    orig_skip_samples = *skip_samples_p;
    skip_samples = *skip_samples_p;
  }

  adc_init();
  timer1_init();
}


/** @} */

/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
