/** \file firmware/main.c
 * \brief The firmware for ATmega devices
 *
 * \author Copyright (C) 2009 samplemaker
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
 * \defgroup firmware_generic Generic Firmware Parts
 * \ingroup firmware
 *
 * \defgroup firmware_personality_groups Firmware Personality Groups
 * \ingroup firmware
 *
 * \defgroup firmware_memories Memory types and layout
 * \ingroup firmware_generic
 *
 * There can be a number of kinds of variables.
 *
 *   a) Uninitialized non-register variables.  Those are placed in the
 *      .bss section in the ELF file, and in the SRAM on the device.
 *      The whole SRAM portion corresponding to the .bss section is
 *      initialized to zero bytes in the startup code, so we do not
 *      need to initialized those variables anywhere.
 *
 *   b) Initialized non-register variables.  Those are placed in the
 *      .data section in the ELF file, and in the SRAM on the device.
 *      The whole SRAM portion corresponding to the .data section is
 *      initialized to their respective byte values by autogenerated
 *      code executed before main() is run.
 *
 *   c) Initialized constants in the .text section.  Those are placed
 *      into the program flash on the device, and due to the AVR's
 *      Harvard architecture, need special instructions to
 *      read. Unused so far.
 *
 *   d) Register variables.  We only use them in the uninitialized
 *      variety so far for the assembly language version
 *      ISR(ADC_vect), if you choose to compile and link that.
 *
 *   e) EEPROM variables.  We are not using those anywhere yet.
 *
 * All in all, this means that for normal memory variables,
 * initialized or uninitialized, we do not need to initialize anything
 * at the start of main().
 *
 * Also note that the ATmega644 has 4K of SRAM. With an ADC resolution
 * of 10 bit, we need to store 2^10 = 1024 = 1K values in our
 * histogram table. This results in the following memory sizes for the
 * histogram table:
 *
 *    uint16_t: 2K
 *    uint24_t: 3K
 *    uint32_t: 4K
 *
 * We could fit the global variables into otherwise unused registers
 * to free some more SRAM, but we cannot move the stack into register
 * space. This means we cannot use uint32_t counters in the table -
 * the absolute maximum sized integer we can use is our self-defined
 * "uint24_t" type.
 *
 * \addtogroup firmware_generic
 * @{
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "aduc.h"
#include "flash.h"
#include "int.h"
#include "init.h"

#include "compiler.h"
#include "uart-comm.h"
#include "uart-printf.h"
#include "frame-comm.h"
#include "packet-comm.h"
#include "frame-defs.h"
#include "packet-defs.h"
#include "wdt-softreset.h"
#include "timer1-measurement.h"
#include "timer1-get-duration.h"
#include "main.h"
#include "data-table.h"
#include "switch.h"

/* Only try compiling for supported MCU types */
/*#if defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__)
#else
# error Unsupported MCU!
#endif
*/

/** Define static string in a single place */
const char PSTR_INVALID_EEPROM_DATA[] = "Invalid EEPROM data";

/** Define static string in a single place */
const char PSTR_DONE[]                = "DONE";

/** Define static string in a single place */
const char PSTR_MEASURING[]           = "MEASURING";

/** Define static string in a single place */
const char PSTR_READY[]               = "READY";

/** Define static string in a single place */
const char PSTR_RESET[]               = "RESET";


/** Global flag controlling the measurement in progress flow */
volatile uint8_t measurement_finished;



/** Configure unused pins */
void __init main_io_init_unused_pins(void)
{
  /** \todo configure unused pins */
}
/** Put function into init section, register function pointer and
 *  execute function at start up
 */
register_init5(main_io_init_unused_pins);


/**
 * The one trailing byte is the length of the actually transmitted
 * param set.
 */
personality_param_t pparam_sram;
 /* personality_param_t pparam_eeprom;
BARE_COMPILE_TIME_ASSERT(sizeof(pparam_sram) == sizeof(pparam_eeprom)); */


/** Send value table packet to controller via serial port (layer 3).
 *
 * \param reason The reason why we are sending the value table
 *               (#packet_value_table_reason_t).
 *
 * Note that send_table() might take a significant amount of time.
 * For example, at 9600bps, transmitting a good 3KByte will take a
 * good 3 seconds.  If you disable interrupts for that time and want
 * to continue the measurement later, you will want to properly pause
 * the timer.  We are currently keeping interrupts enabled if we
 * continue measuring, which avoids this issue.
 *
 * Note that for 'I' value tables it is possible that we send fluked
 * values due to overflows.
 */
void send_table(const packet_value_table_reason_t reason)
{
  const uint16_t duration = get_duration();

  packet_value_table_header_t header = {
    data_table_info.element_size,
    reason,
    data_table_info.type,
    duration,
    pparam_sram.length
  };
  frame_start(FRAME_TYPE_VALUE_TABLE,
              sizeof(header) + pparam_sram.length + data_table_info.size);
  uart_putb((const void *)&header, sizeof(header));
  uart_putb((const void *)pparam_sram.params, pparam_sram.length);
  uart_putb((const void *)data_table, data_table_info.size);
  frame_end();
}


void send_personality_info(void)
{
  frame_start(FRAME_TYPE_PERSONALITY_INFO,
              sizeof(personality_info) + personality_name_length);
  uart_putb((const void *)&personality_info, sizeof(personality_info));
  uart_putb((const void *)personality_name, personality_name_length);
  frame_end();
}


/** Send parameters from EEPROM
 *
 * Caution: The caller is responsible for copying the parameters from
 * EEPROM to SRAM before calling this function.
 */
void send_eeprom_params_in_sram(void);
void send_eeprom_params_in_sram(void)
{
  const uint8_t length = pparam_sram.length;
  if (length == 0xff || length > sizeof(pparam_sram.params)) {
    send_text(PSTR_INVALID_EEPROM_DATA);
  } else {
    frame_start(FRAME_TYPE_PARAMS_FROM_EEPROM, length);
    uart_putb((const void *)pparam_sram.params, length);
    frame_end();
  }
}


void params_copy_from_eeprom_to_sram(void)
{
  /* fetch data from flash */
  char *param_eeprom;
  uint16_t num_chars;
  eepflash_read((char **)&param_eeprom,
                &num_chars, BLOCKID_0);
  /* copy data from flash to ram */
  char *p_dst = (char *)&pparam_sram;
  const char *p_end = (char *) (param_eeprom + num_chars);
  while (param_eeprom != p_end)
    *(p_dst++) = *(param_eeprom++);

/* \todo
  dirty hack - implement "eeprom_read_block" in flash.c !
  eeprom_read_block(&pparam_sram, &pparam_eeprom,
                    sizeof(pparam_sram));
*/
}


void general_personality_start_measurement_sram(void)
{
  switch_lock();
  personality_start_measurement_sram();
}


/**
 * \defgroup firmware_fsm Firmware FSM
 * \ingroup firmware_generic
 *
 * Implements the finite state machine (FSM) as described in \ref
 * embedded_fsm.  The "ST_foo" and "ST_FOO" definitions from
 * #firmware_state_t refer to the states from that FSM definition.
 *
 * \image html firmware-fsm.png "Device as State Machine"
 *
 * See \ref embedded_fsm for more information on this FSM state
 * transition diagram.
 *
 * @{
 */


/** List of states for firmware state machine
 *
 * \see communication_protocol
 */
typedef enum {
  STP_READY,
  STP_MEASURING,
  STP_DONE
  /* STP_ERROR not actually modelled as a state */
  /* STP_RESET not actually modelled as a state */
} firmware_state_t;


/** Firmware FSM event handler for finished measurement */
inline static
firmware_state_t firmware_handle_measurement_finished(const firmware_state_t pstate)
{
  switch (pstate) {
  case STP_MEASURING:
    /* end measurement */
/*    cli();*/
    disable_IRQs_usermode();

    on_measurement_finished();
    send_table(PACKET_VALUE_TABLE_DONE);
    return STP_DONE;
    break;
  default:
    send_text("invalid state transition");
    wdt_soft_reset();
    break;
  }
}


/** Firmware FSM event handler for pressed switch */
inline static
firmware_state_t firmware_handle_switch_pressed(const firmware_state_t pstate)
{
  switch (pstate) {
  case STP_READY:
    params_copy_from_eeprom_to_sram();
    const uint8_t length = pparam_sram.length;
    if ((length == 0xff) || (length > sizeof(pparam_sram.params))) {
      send_text(PSTR_INVALID_EEPROM_DATA);
      send_state(PSTR_READY);
      return STP_READY;
    } else {
      general_personality_start_measurement_sram();
      send_state(PSTR_MEASURING);
      return STP_MEASURING;
    }
    break;
  default:
    /* silently ignore the switch press in all other states */
    /* send_text("ignoring pressed switch"); */
    return pstate;
    break;
  }
}


/** Firmware FSM event handler for receiving a command packet from the host
 *
 * \param pstate current FSM state
 * \param cmd the command we are to handle
 * \return new state
 *
 * Implicit parameters via global variables:
 *   personality_param_sram[0..sizeof(personality_param_sram)-2] param+token data
 *   personality_param_sram[sizeof(personality_param_sram)-1] size of param+token data
 */
inline static
firmware_state_t firmware_handle_command(const firmware_state_t pstate,
                                         const uint8_t cmd)
{
  /* temp vars */
  const frame_cmd_t c = (frame_cmd_t)cmd;

  uprintf("EAT PACKET: %c", cmd);

  switch (pstate) {
  case STP_READY:
    switch (c) {
    case FRAME_CMD_PERSONALITY_INFO:
      send_personality_info();
      /* fall through */
    case FRAME_CMD_ABORT:
    case FRAME_CMD_INTERMEDIATE:
    case FRAME_CMD_STATE:
      send_state(PSTR_READY);
      return STP_READY;
      break;
    case FRAME_CMD_PARAMS_TO_EEPROM:
      /* The param length has already been checked by the frame parser */
      send_state("PARAMS_TO_EEPROM");

/* \todo
      eeprom_update_block(&pparam_sram, &pparam_eeprom,
                          sizeof(pparam_eeprom));
*/
      eepflash_write((char *)&pparam_sram,
                     sizeof(pparam_sram), BLOCKID_0);

      send_state(PSTR_READY);
      return STP_READY;
      break;
    case FRAME_CMD_PARAMS_FROM_EEPROM:
      params_copy_from_eeprom_to_sram();
      send_eeprom_params_in_sram();
      send_state(PSTR_READY);
      return STP_READY;
      break;
    case FRAME_CMD_MEASURE:
      /* The param length has already been checked by the frame parser */
      general_personality_start_measurement_sram();
      send_state(PSTR_MEASURING);
      return STP_MEASURING;
      break;
    case FRAME_CMD_RESET:
      send_state(PSTR_RESET);
      wdt_soft_reset();
      break;
    }
    break;
  case STP_MEASURING:
    switch (c) {
    case FRAME_CMD_INTERMEDIATE:
      /** The value table will be updated asynchronously from ISRs
       * like ISR(ADC_vect) or ISR(TIMER1_foo), i.e. independent from
       * this main loop.  This will cause glitches in the intermediate
       * values as the values in the table often consist of more than
       * a single 8bit machine word.  However, we have decided that
       * for *intermediate* results, those glitches are acceptable.
       *
       * Keeping interrupts enabled has the additional advantage that
       * the measurement continues during send_table(), so we need not
       * concern ourselves with pausing the measurement timer, or with
       * making sure we properly reset the hardware which triggered
       * our ISR within the appropriate time range or anything
       * similar.
       *
       * If you decide to bracket the send_table() call with a
       * cli()/sei() pair, be aware that you need to solve the issue
       * of resetting the hardware properly. For example, with the
       * adc-int-mca personality, resetting the peak hold capacitor on
       * resume if an event has been detected by the analog circuit
       * while we had interrupts disabled and thus ISR(ADC_vect) could
       * not reset the peak hold capacitor.
       */
      send_table(PACKET_VALUE_TABLE_INTERMEDIATE);
      send_state(PSTR_MEASURING);
      return STP_MEASURING;
      break;
    case FRAME_CMD_PERSONALITY_INFO:
      send_personality_info();
      /* fall through */
    case FRAME_CMD_PARAMS_TO_EEPROM:
    case FRAME_CMD_PARAMS_FROM_EEPROM:
    case FRAME_CMD_MEASURE:
    case FRAME_CMD_RESET:
    case FRAME_CMD_STATE:
      send_state(PSTR_MEASURING);
      return STP_MEASURING;
      break;
    case FRAME_CMD_ABORT:
      send_state(PSTR_DONE);
  /*    cli();*/
      disable_IRQs_usermode();

      on_measurement_finished();
      send_table(PACKET_VALUE_TABLE_ABORTED);
      send_state(PSTR_DONE);
      return STP_DONE;
      break;
    }
    break;
  case STP_DONE:
    switch (c) {
    case FRAME_CMD_PERSONALITY_INFO:
      send_personality_info();
      /* fall through */
    case FRAME_CMD_STATE:
      send_state(PSTR_DONE);
      return STP_DONE;
      break;
    case FRAME_CMD_RESET:
      send_state(PSTR_RESET);
      wdt_soft_reset();
      break;
    default:
      send_table(PACKET_VALUE_TABLE_RESEND);
      send_state(PSTR_DONE);
      return STP_DONE;
      break;
    }
    break;
  }
  send_text("STP_ERROR");
  wdt_soft_reset();
}


/** @} */


/** AVR firmware's main event loop function
 *
 * The main event loop detects all incoming events and dispatches them
 * to the appropriate handler functions (firmware_handle_*).
 *
 * Incoming events are e.g. "button pressed", "incoming byte on
 * USART", or "measurement finished".
 *
 * Note that eventually we are always forcing the system through a
 * reset after a measurement has finished (by having the watchdog
 * timer reset the AVR device). This ensures the device is in a
 * well-defined default state when the next measurement is being set
 * up.
 *
 * The main event loop function directly integrates the following FSM
 * which parses the incoming bytes from the USART into command frames
 * in order to save code size.
 *
 * \dot
 * digraph firmware_frame_fsm {
 *   node [shape=ellipse, fontname=Helvetica, fontsize=10];
 *   edge [fontname=Helvetica, fontsize=10];
 *   magic [ label="STF_MAGIC" ];
 *   command [ label="STF_COMMAND" ];
 *   length [ label="STF_LENGTH" ];
 *   param [ label="STF_PARAM" ];
 *   checksum [ label="STF_CHECKSUM" ];
 *   magic:nw -> magic:nw [ label="mismatch\ni:=0" ];
 *   magic -> magic [ label="match magic[i++] && i<magic_size\n-/-" ];
 *   magic -> command [ label="match magic[i++] && i>=magic_size\n-/-" ];
 *   command -> length;
 *   length -> param [ label="length>0\ni:=0" ];
 *   length -> checksum [ label="length==0\n-/-" ];
 *   param -> param [ label="i<length\ni++" ];
 *   param -> checksum [ label="i>=length\n-/-" ];
 *   checksum -> magic [ label="chksum match\nhandle_frame, i:=0" ];
 *   checksum -> magic [ label="chksum fail\ni:=0" ];
 * }
 * \enddot
 */
inline static
void main_event_loop(void)
  __attribute__ ((noreturn));
inline static
void main_event_loop(void)
{
  /** Frame parser FSM state */
  typedef enum {
    STF_MAGIC,
    STF_COMMAND,
    STF_LENGTH,
    STF_PARAM,
    STF_CHECKSUM,
  } frame_state_t;
  frame_state_t fstate = STF_MAGIC;

  /** Frame parser offset/index into magic/data */
  uint8_t idx = 0;
  /** Frame parser cached data for current frame */
  uint8_t cmd = 0;
  /** Frame parser cached data for current frame */
  uint8_t len = 0;

  /* Firmware FSM State */
  firmware_state_t pstate = STP_READY;

  /* Globally enable interrupts */
/*  sei();*/
  enable_IRQs_usermode();

  /* Firmware main event loop */
  while (1) {

    /* check for "measurement finished" event */
    if (measurement_finished) {
      pstate = firmware_handle_measurement_finished(pstate);
      measurement_finished = 0;
      continue;
    }

    /* check whether a key event occured */
    if (switch_trigger_measurement()) {
      pstate = firmware_handle_switch_pressed(pstate);
      continue;
    }

    /* check whether a byte has arrived via UART */
    if (bit_is_set( COMSTA0, UART_DR )) {
      const char ch = uart_getc();
      const uint8_t byte = (uint8_t)ch;

      frame_state_t next_fstate = fstate;

      switch (fstate) {
      case STF_MAGIC:
        if (byte == FRAME_MAGIC_STR[idx++]) {
          uart_recv_checksum_update(byte);
          if (idx < 4) {
            next_fstate = STF_MAGIC;
          } else {
            next_fstate = STF_COMMAND;
          }
        } else {
          /* syncing, not an error */
          goto restart;
        }
        break;
      case STF_COMMAND:
        uart_recv_checksum_update(byte);
        cmd = byte;
        next_fstate = STF_LENGTH;
        break;
      case STF_LENGTH:
        uart_recv_checksum_update(byte);
        len = byte;
        if (pstate == STP_READY) {
          /* We can only use the personality_param_sram buffer in the
           * STP_READY state. By not writing to the buffer after
           * transitioning from STP_READY, we keep the content of the
           * buffer from the "start measurement" command for sending
           * back later.
           */
          pparam_sram.length = len;
        }
        if (len == 0) {
          next_fstate = STF_CHECKSUM;
        } else if ((len >= personality_param_size) &&
                   (len < MAX_PARAM_LENGTH)) {
          idx = 0;
          next_fstate = STF_PARAM;
        } else {
          /* whoever sent us that wrongly sized data frame made an error */
          /** \todo Find a way to report errors without resorting to
           *        sending free text. */
          send_text("param length mismatch");
          goto error_restart_nomsg;
        }
        break;
      case STF_PARAM:
        uart_recv_checksum_update(byte);
        if (pstate == STP_READY) {
          /* We can only use the personality_param_sram buffer in the
           * STP_READY state. By not writing to the buffer after
           * transitioning from STP_READY, we keep the content of the
           * buffer from the "start measurement" command for sending
           * back later.
           */
          pparam_sram.params[idx] = byte;
        }
        idx++;
        if (idx < len) {
          next_fstate = STF_PARAM;
        } else {
          next_fstate = STF_CHECKSUM;
        }
        break;
      case STF_CHECKSUM:
        if (uart_recv_checksum_matches(byte)) {
          /* checksum successful */
          pstate = firmware_handle_command(pstate, cmd);
          goto restart;
        } else {
          /** \todo Find a way to report checksum failure without
           *        resorting to sending free text. */
          send_text("checksum fail");
          goto error_restart_nomsg;
        }
        break;
      }
      //uprintf("idx=%u", idx);
      goto skip_errors;

    error_restart_nomsg:

    restart:
      next_fstate = STF_MAGIC;
      idx = 0;
      uart_recv_checksum_reset();

    skip_errors:
      fstate = next_fstate;

      continue;
    } /* character received on UART */

  } /* while (1) main event loop */

} /* void main_event_loop(void); */


/** Firmware main() function
 *
 * avr-gcc knows that int main(void) ending with an endless loop and
 * not returning is normal, so we can avoid the
 *
 *    int main(void) __attribute__((noreturn));
 *
 * declaration and compile without warnings (or an unused return instruction
 * at the end of main).
 */
int main(void)
{
  /** No need to initialize global variables here. See \ref
   *  firmware_memories.
   */

  /* ST_booting */

  /** We try not to explicitly call initialization functions at the
   * start of main().  The idea is to implement the initialization
   * functions as ((naked)) and put them in the ".initN" sections so
   * they are called automatically before main() is run.
   *
   * This keeps all foo.c related initialization code inside foo.c,
   * and it also saves us a few bytes in the firmware image which
   * would be used by the call/return instructions.
   */

  send_personality_info();
  send_state(PSTR_READY);

  main_event_loop();
} /* int main(void) */


/** @} */


/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
