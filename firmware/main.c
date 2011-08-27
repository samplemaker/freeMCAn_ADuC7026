/** \file firmware/main.c
 * \brief The firmware for ADUC devices
 *
 * \author Copyright (C) 2009 samplemaker
 * \author Copyright (C) 2010 samplemaker
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
 * \defgroup firmware Firmware
 *
 * \defgroup firmware_memories Memory types and layout
 * \ingroup firmware
 *
 *
 * \addtogroup firmware
 * @{
 */

/*------------------------------------------------------------------------------
 * Includes
 *------------------------------------------------------------------------------
 */

/*
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
*/

#include <inttypes.h>
#include "aduc7026.h"
#include "timer.h"
#include "int.h"

#include "global.h"
#include "uart-comm.h"
#include "frame-comm.h"
#include "packet-comm.h"
#include "frame-defs.h"
#include "packet-defs.h"
#include "firmware-version.h"


/*------------------------------------------------------------------------------
 * Defines
 *------------------------------------------------------------------------------
 */

#define TOG_LED1 (GP4DAT ^= _BV(GP_DATA_OUTPUT_Px1))

/** Number of elements in the histogram table */
#define MAX_COUNTER (1<<ADC_RESOLUTION)


/*------------------------------------------------------------------------------
 * Variables  (static, not visible in other modules)
 *------------------------------------------------------------------------------
 */


/** \var table
 * \brief histogram table
 *
 **/
volatile histogram_element_t table[MAX_COUNTER];


/** timer counter
 *
 * Initialized once by main() with value received from host
 * controller. Never touched by main() again after starting the timer
 * interrupt.
 *
 * Timer interrupt handler has exclusive access to read/writes
 * timer_count to decrement, once the timer ISR has been enabled.
 */
volatile uint16_t timer_count;


/** Original timer count received in the command.
 *
 * Used later for determining how much time has elapsed yet. Written
 * once only, when the command has been received.
 */
volatile uint16_t orig_timer_count;


/** Timer counter has reached zero.
 *
 * Used to signal from the timer ISR to the main program that the
 * timer has elapsed.
 *
 * Will be set to 1 when max_timer_count is exceeded, is 0 otherwise.
 * Written only once by timer interrupt handler. Read by main
 * loop. Only accessible with atomic read/write operations.
 */
volatile uint8_t timer_flag;


/*------------------------------------------------------------------------------
 * Local prototypes (not visible in other modules)
 *------------------------------------------------------------------------------
 */


/** AD conversion complete interrupt entry point
 *
 * This function is called when an A/D conversion has completed.
 * Update histogram
 * Discharge peak hold capacitor
 */
inline static
void ISR_ADC(void){
  /* starting from bit 16 the result is stored in ADCDAT.
     reading the ADCDATA also clears flag in ADCSTA */
  const uint32_t result =  ADCDAT;

  /* adjust to correct size */
  const uint16_t index = (result >> (16 + 12 - ADC_RESOLUTION));

  volatile histogram_element_t *element = &(table[index]);
  histogram_element_inc(element);

}


/** Timer ISR
 *
 * When timer has elapsed, the global #timer_flag is set.
 * Note that we are using non nested interrupts and that
 * interrupts are disabled globally if this code is executed.
 * Therefore read and writes are atomic here.
 *
 * Note that we are counting down the timer_count, so it will start
 * with its maximum value and count down to zero.
 *
 */
inline static
void ISR_WAKEUP_TIMER2(void){
  /* toggle a sign */
  TOG_LED1;

  if (!timer_flag) {
    /* We do not touch the timer_flag ever again after setting it */
    timer_count--;
    if (timer_count == 0) {
      /* timer has elapsed, set the flag to signal the main program */
      timer_flag = 1;
    }
  }

  T2CLRI = 0x00;
}

/* DEBUG ADC */
inline static
void ISR_TIMER0(void){
  GP1DAT ^= _BV(GP_DATA_OUTPUT_Px5);
  T0CLRI = 0x00;
}
/* DEBUG ADC */
inline static
void DEBUG_(void){
  T0CON |= (_FS(TIMER0_PRESCALER, TIMER0_PRESCALER_VALUE) |
            _BV(TIMER0_MODE) );
  T0LD = TIMER0_LOAD_VALUE;
  T0CON |= _BV(TIMER0_ENABLE);
  IRQEN |= _BV(INT_TIMER0);
}

/** IRQ - handler coordinator
 *
 * Distributes the IRQ processing according to the IRQ-source
 *
 */
void _irq_handler(void) __attribute__ ((interrupt ("IRQ")));
void _irq_handler(void)
{
  /* which interrupt is enabled and pending? */
  if (IRQSTA & _BV(INT_ADC_CHANNEL)){
    ISR_ADC();
  };
  if (IRQSTA & _BV(INT_WAKEUP_TIMER2)){
    ISR_WAKEUP_TIMER2();
  };
  if (IRQSTA & _BV(INT_TIMER0)){
    ISR_TIMER0();
  };

}

/** Get measurement time elapsed
 *
 * Poll until value in destiny is correct (may be accessed by an interrupt)
 *
 */
inline static
uint16_t get_duration(void)
{
  uint16_t a, b;
  a = timer_count;
  do {
    b = a;
    a = timer_count;
  } while (a != b);
  /* Now 'a' and 'b' both contain the same valid #timer1_count value. */
  const uint16_t duration = orig_timer_count - a;
  return duration;
}

/** ADC initialisation and configuration
 *
 */
void
adc_init(void){
  ADCCON = 0;
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
  ADCCON |= (_FS(ADC_CLOCK_SPEED, MASK_001)     |
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

/** Configure 16 bit timer to trigger an ISR every second
 *
 */
inline static
void timer_init(const uint8_t timer0, const uint8_t timer1){
  /** Set up timer with the combined value we just got the bytes of.
   *
   */
  orig_timer_count = (((uint16_t)timer1)<<8) | timer0;
  timer_count = orig_timer_count;

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

/** Programmable logic array used for edged triggering the ADC
 *
 * HCLK synchronous implementation for edge triggering the ADC
 * Element0: Logic function for generating a one clock pulse
 * Element4: Shifting flip flop for generating a one clock pulse
 * Element5: Input flip flop to suppress glitches
 *
 * \todo write driver for external D-Flipflop because ADUC7026 ADC
 * is level triggered (not edge) at #CONVstart
 */
void pla_init(void){
 
  /* PLAs triggering the adc:
   * 0b0000: Element0  (BLOCK0)
   * 0b0001: Element1  (BLOCK0) 
   * 0b1111: Element15 (BLOCK1)  
   */
  PLAADC |= (_BV(PLA_ADC_CONV_START) |
             _FS(PLA_ADC_CONV_SRC, MASK_0000) ); 
  /* Configure PLA ELEMENT0 (BLOCK0)
   *
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
   *
   * - MUX3: Select MUX1, not GPIO (nothing to do)
   * - MUX1: Connect element 5 at MUX1
   * - Select logical function of the block (B -> route MUX3)
   * - Use flip-flop (MUX4) (nothing to do)
   */
  PLAELM4 |= (/*_BV(PLA_MUX3_CONTROL)             |*/
              _FS(PLA_MUX1_CONTROL, MASK_10)      |
              _FS(PLA_LOOKUP_TABLE, MASK_1010) );
  /* Configure PLA ELEMENT5 (BLOCK0)
   *
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
   *
   * may be configured as output and switched by software, timer
   * for testing.
   * \todo : configure as input 
   */
  GP1CON |= _FS(GP_SELECT_FUNCTION_Px5, MASK_00);
  GP1DAT |= _BV(GP_DATA_DIRECTION_Px5);
  /* PLA-BLOCK0 clock source selection
   * Clock source: 
   * HCLK: MASK_011
   * P0.5: MASK_000
   */
  PLACLK |= _FS(PLA_BLOCK0_CLK_SRC, MASK_011);
 
}


inline static
void timer_init_quick(void)
{
  /* no automatic pin toggling with ADUC avable */
}


/** Initialize peripherals
 *
 * Configure peak hold capacitor reset pin
 * Configure unused pins
 */
inline static
void io_init(void)
{
  /* measurement in progress LED */

  /* configure P4.1 as GPIO: */
  GP4CON |= _FS(GP_SELECT_FUNCTION_Px1, MASK_00);
  /* configure P4.1 as output */
  GP4DAT |= _BV(GP_DATA_DIRECTION_Px1);

  /** \todo configure unused pins */
}


/** \addtogroup firmware_comm
 * @{
 */



/** Send histogram packet to controller via serial port (layer 3).
 *
 * \param type The type of histogram we are sending
 *             (#packet_histogram_type_t).  You may also a dummy value
 *             like '?' or -1 or 0xff or 0 until you make use of that
 *             value on the receiver side.
 *
 * Note that send_histogram() might take a significant amount of time.
 * For example, at 9600bps, transmitting a good 3KByte will take a
 * good 3 seconds.  If you disable interrupts for that time and want
 * to continue the measurement later, you will want to properly pause
 * the timer.  We are currently keeping interrupts enabled if we
 * continue measuring, which avoids this issue.
 *
 * Note that for 'I' histograms it is possible that we send fluked
 * values due to overflows.
 */
static
void send_histogram(const packet_histogram_type_t type);
static
void send_histogram(const packet_histogram_type_t type)
{
  const uint16_t duration = get_duration();

  packet_histogram_header_t header = {
    ELEMENT_SIZE_IN_BYTES,
    type,
    duration,
    orig_timer_count
  };
  frame_start(FRAME_TYPE_HISTOGRAM, sizeof(header)+sizeof(table));
  uart_putb((const void *)&header, sizeof(header));
  uart_putb((const void *)table, sizeof(table));
  frame_end();
}


/** @} */


/** List of states for firmware state machine
 *
 * \see communication_protocol
 */
typedef enum {
  ST_READY,
  ST_timer0,
  ST_timer1,
  ST_checksum,
  ST_MEASURING,
  ST_MEASURING_nomsg,
  ST_DONE,
  ST_RESET
} firmware_state_t;


/** Firmware's main "loop" function
 *
 * Note that we create a "loop" by resetting the core
 * when one loop iteration is finished. This will cause the
 * system to start again with the software in the defined
 * default state.
 *
 * This function implements the finite state machine (FSM) as
 * described in \ref embedded_fsm.  The "ST_foo" and "ST_FOO"
 * definitions from #firmware_state_t refer to the states from that FSM
 * definition.
 *
 * Note that the ST_MEASURING state had to be split into two:
 * ST_MEASURING which prints its name upon entering and immediately
 * continues with ST_MEASURING_nomsg, and ST_MEASURING_nomsg which
 * does not print its name upon entering and is thus feasible for a
 * busy polling loop.
 *
 * int main(void) ending with an endless loop and
 * not returning is normal, so we can avoid the
 *
 *    int main(void) __attribute__((noreturn));
 *
 * declaration and compile without warnings (or an unused return instruction
 * at the end of main).
 */
int main(void)
{
    /** No need to initialize global variables here. See \ref firmware_memories. */

    /* ST_booting */

    uart_init();
    send_text("Booting");
    send_version();

    /* initialize */
    DEBUG_();
    io_init();
    pla_init();
    adc_init();
 
    /** Used while receiving "m" command */
    register uint8_t timer0 = 0;
    /** Used while receiving "m" command */
    register uint8_t timer1 = 0;

    /** Firmware FSM state */
    firmware_state_t state = ST_READY;

    /* Firmware FSM loop */
    while (1) {
      /** Used in several places when reading in characters from UART */
      char ch;
      /** Used in several places when reading in characters from UART */
      frame_cmd_t cmd;
      /** next FSM state */
      firmware_state_t next_state = state;
      switch (state) {
      case ST_READY:
        send_state("READY");
        uart_checksum_reset();
        cmd = ch = uart_getc();
        switch (cmd) {
        case FRAME_CMD_RESET:
          next_state = ST_RESET;
          break;
        case FRAME_CMD_MEASURE:
          next_state = ST_timer0;
          break;
        case FRAME_CMD_STATE:
          next_state = ST_READY;
          break;
        default: /* ignore all other bytes */
          next_state = ST_READY;
          break;
        } /* switch (cmd) */
        break;
      case ST_timer0:
        timer0 = uart_getc();
        next_state = ST_timer1;
        break;
      case ST_timer1:
        timer1 = uart_getc();
        next_state = ST_checksum;
        break;
      case ST_checksum:
        if (uart_checksum_recv()) { /* checksum successful */
          /* begin measurement */
          timer_init(timer0, timer1);
          enable_IRQs_usermode();
          next_state = ST_MEASURING;
        } else { /* checksum fail */
          /** \todo Find a way to report checksum failure without
           *        resorting to sending free text. */
          send_text("checksum fail");
          next_state = ST_RESET;
        }
        break;
      case ST_MEASURING:
        send_state("MEASURING");
        next_state = ST_MEASURING_nomsg;
        break;
      case ST_MEASURING_nomsg:

        /* \todo : atomic ldr? */

        if (timer_flag) { /* done */
          disable_IRQs_usermode();
          send_histogram(PACKET_HISTOGRAM_DONE);
          timer_init_quick();
          next_state = ST_DONE;
        } else if (bit_is_set( COMSTA0, UART_DR )) {
          /* there is a character in the UART input buffer */
          cmd = ch = uart_getc();
          switch (cmd) {
          case FRAME_CMD_ABORT:
            disable_IRQs_usermode();
            send_histogram(PACKET_HISTOGRAM_ABORTED);
            next_state = ST_RESET;
            break;
          case FRAME_CMD_INTERMEDIATE:
            /** The ISR(ADC_vect) will be called when the analog circuit
             * detects an event.  This will cause glitches in the
             * intermediate histogram values However, we have decided that for
             * *intermediate* results, those glitches are acceptable.
             *
             * Keeping interrupts enabled has the additional advantage
             * that the measurement continues during send_histogram(),
             * so we need not concern ourselves with pausing the
             * measurement timer or anything similar.
             *
             * If you decide to bracket the send_histogram() call,
             * be aware that you need to solve the
             * issue of resetting the peak hold capacitor on resume if
             * an event has been detected by the analog circuit while we
             * had interrupts disabled and thus ISR(ADC_vect) could not
             * reset the peak hold capacitor.
             */
            send_histogram(PACKET_HISTOGRAM_INTERMEDIATE);
            next_state = ST_MEASURING;
            break;
          case FRAME_CMD_STATE:
            next_state = ST_MEASURING;
            break;
          default: /* ignore all other bytes */
            next_state = ST_MEASURING;
            break;
          }
        } else { /* neither timer flag set nor incoming UART data */
          next_state = ST_MEASURING_nomsg;
        }
        break;
      case ST_DONE:
        /* STATE: DONE (wait for RESET command while seinding histograms) */
        send_state("DONE");
        cmd = ch = uart_getc();
        switch (cmd) {
        case FRAME_CMD_STATE:
          next_state = ST_DONE;
          break;
        case FRAME_CMD_RESET:
          next_state = ST_RESET;
          break;
        default:
          send_histogram(PACKET_HISTOGRAM_RESEND);
          next_state = ST_DONE;
          break;
        }
        break;
      case ST_RESET:
        send_state("RESET");
        soft_reset();
        break;
      } /* switch (state) */
      state = next_state;
    } /* while (1) */
}

/** @} */


/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
