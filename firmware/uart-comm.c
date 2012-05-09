/** \file firmware/uart-comm.c
 * \brief ADuC7026 UART communication implementation (layer 1)
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
 * \defgroup uart_comm Firmware UART Communications
 * \ingroup firmware_generic
 *
 * Implements the byte stream part of the communication protocol
 * (Layer 1).
 *
 * @{
 */


#include <stdint.h>

#include "aduc.h"
#include "init.h"

#include "uart-defs.h"
#include "uart-comm.h"
#include "checksum.h"


/* Normal 450 UART Baud Rate Generation */
#define UART_DL ((F_HCLK) / (2ULL * 16ULL * (UART_BAUDRATE)))
#define BAUD_REAL_DL ((F_HCLK) / (2ULL * 16ULL * (UART_DL)))
#define BAUD_ERROR ((BAUD_REAL_DL * 1000ULL) / (UART_BAUDRATE))

/* If baud error is too high revert to FD */
#if ((BAUD_ERROR<(1000-UART_RELTOL)) || (BAUD_ERROR>(1000+UART_RELTOL)))
  #define USE_FRACTIONAL_DIVIDER 1
  #warning baud error exceeds UART_RELTOL => reverting to fractional divider
#else
  #define USE_FRACTIONAL_DIVIDER 0
#endif

/* Check fractional divider if requested */
#if USE_FRACTIONAL_DIVIDER
#undef BAUD_ERROR
#define UART_FBM_VALUE (1ULL)
#define UART_FBN_VALUE (((2048ULL * ((BAUD_REAL_DL) - (UART_FBM_VALUE) * \
                         (UART_BAUDRATE))) / (UART_BAUDRATE)) & 0x7FF)

#define BAUD_REAL_FD ((2048ULL * (BAUD_REAL_DL)) /                       \
                      (2048ULL * (UART_FBM_VALUE) + (UART_FBN_VALUE)))
#define BAUD_ERROR ((BAUD_REAL_FD * 1000UL) / (UART_BAUDRATE))

#if ((BAUD_ERROR<(1000-UART_RELTOL)) || (BAUD_ERROR>(1000+UART_RELTOL)))
  #error baud error is too high although using fractional divider
#endif


#endif

#define UARTL_DL (UART_DL & 0xff)
#define UARTH_DL (UART_DL >> 8)


static checksum_accu_t cs_accu_send;
static checksum_accu_t cs_accu_recv;


/** UART initialisation to 8 databits no parity
 *
 */
static
void __init uart_init(void)
{
  /* set up port pin P1.1 (TX) as SOUT
   * set up port pin P1.0 (RX) as SIN
   */
  GP1CON |= (_FS(GP_SELECT_FUNCTION_Px1, MASK_01) |
             _FS(GP_SELECT_FUNCTION_Px0, MASK_01) );
  /* configure SOUT as output */
  GP1DAT |= _BV(GP_DATA_DIRECTION_Px1);
  /* configure SIN as input and disable pull up */
  GP1PAR &= ~_BV(GP_PAR_PULL_UP_Px0);
  GP1DAT &= ~_BV(GP_DATA_DIRECTION_Px0);

  /* clear UART_BRK (operate in normal mode), UART_PEN (no parity), 
   * UART_STOP (1 stop bit) and set word length: 8 bits */
  COMCON0 = (_BV(UART_WLS0) | _BV(UART_WLS1));
  /* no modem (reset modem register) */
  COMCON1 = 0x0;

  #if USE_FRACTIONAL_DIVIDER
    /* set M = 1 (FBM), set FBN according to macro and enable FD */
    COMDIV2 = (_FS(UART_FBM, MASK_01)        |
               _FS(UART_FBN, UART_FBN_VALUE) |
               _BV(UART_FBEN) );
  #else
    /* no fractional divider (clear UART_FBEN) */
    COMDIV2 = 0x0;
  #endif

  /* 1.) set baud rate:
   *     register for access to divisor latch
   *     DIV0 & DIV1 registers and write divider */
  COMCON0 |= _BV(UART_DLAB);
  COMDIV0 = UARTL_DL;
  COMDIV1 = UARTH_DL;
  /* 2.) reset access to COMRX/COMTX receive and transmit
   *     registers by default (memory share with COMDIVn) */
  COMCON0 &= ~_BV(UART_DLAB);

  cs_accu_send = checksum_reset();
  cs_accu_recv = checksum_reset();
}

/** Put function into init section, register function pointer and
 *  execute function at start up
 */
module_init(uart_init, 5);



/** Send checksum */
void uart_send_checksum(void)
{
  const uint8_t v = cs_accu_send & 0xff;
  uart_putc((const char)v);
}


void uart_send_checksum_reset(void)
{
  cs_accu_send = checksum_reset();
}


/** Write character to UART */
void uart_putc(const char c)
{
    /* poll until bit is set and output buffer is empty */
    loop_until_bit_is_set(COMSTA0, UART_TEMT);

    /* put the char */
    COMTX = c;

    /* update the checksum state with c */
    cs_accu_send = checksum_update(cs_accu_send, c);
}


/** Write data buffer of arbitrary size and content to UART */
void uart_putb(const void *buf, size_t len)
{
  for (const char *s = (const char *)buf; len > 0; s++, len--) {
    uart_putc(*s);
  }
}


/** Read a character from the UART */
char uart_getc()
{
    /* Poll til bit is set and char is in input buffer */
    loop_until_bit_is_set( COMSTA0, UART_DR );

    /* Get the character and clear the bit */
    const char ch = COMRX;

    return ch;
}


/** Check whether received byte c and matches the checksum
 *
 * \return boolean value in char
 */
char uart_recv_checksum_matches(const uint8_t data)
{
  return checksum_matches(cs_accu_recv, data);
}


void uart_recv_checksum_reset(void)
{
  cs_accu_recv = checksum_reset();
}


/* update the checksum state with ch */
void uart_recv_checksum_update(const char ch)
{
  cs_accu_recv = checksum_update(cs_accu_recv, ch);
}


/** @} */


/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
