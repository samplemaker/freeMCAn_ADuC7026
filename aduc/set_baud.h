/** \file set_baud.h
 * \brief UART macross
 *
 * \author Copyright (C) 2012 Samplemaker
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
 * \defgroup set_baud UART macros
 * \ingroup ADUC
 *
 *
 *  \code
 *  #define UART_BAUDRATE 9600
 *  #include "clock.h"
 *  #include "set_baud.h"
 *  \endcode
 *
 *  set_baud.h calculates all necessary register settings to setup the
 *  UART. All calculations are done using the C preprocessor.
 *
 *  To use this macro baudrate and the cpu core frequency must be
 *  defined in advance. The cpu frequency can be included with "clock.h".
 *
 *  Additionally a limit for the baudrate tolerance can be given upon
 *  either fractional divider mode or normal baudrate calculation is
 *  invoked. If not used a default value of 1% accuracy is assumed.
 *
 *  Example usage:
 *
 *  \code
 *
 *  #include "aduc.h"
 *  #include "clock.h"
 *
 *  #define UART_BAUDRATE 115200UL
 *  #define UART_RELTOL 10
 *
 *  #include "set_baud.h"
 *
 *  static void
 *  uart_init(void)
 *  {
 *  //setup peripherals
 *  GP1CON |= (_FS(GP_SELECT_FUNCTION_Px1, MASK_01) |
 *             _FS(GP_SELECT_FUNCTION_Px0, MASK_01) );
 *  // clear UART_BRK (operate in normal mode), UART_PEN (no parity), 
 *  // UART_STOP (1 stop bit)
 *  // set word length: 8 bits
 *  COMCON0 = _FS(UART_WLS, MASK_11);
 *  // no modem (reset modem register)
 *  COMCON1 = 0x0;
 *  #if USE_FRACTIONAL_DIVIDER
 *    COMDIV2 = (_FS(UART_FBM, UART_FBM_VALUE) |
 *               _FS(UART_FBN, UART_FBN_VALUE) |
 *               _BV(UART_FBEN) );
 *  #else
 *    COMDIV2 = 0x0;
 *  #endif
 *  //register for access to divisor latch DIV0 & DIV1 registers
 *  //and write divider
 *  COMCON0 |= _BV(UART_DLAB);
 *  COMDIV0 = UARTL_DL;
 *  COMDIV1 = UARTH_DL;
 *  // reset access to COMRX/COMTX receive and transmit
 *  COMCON0 &= ~_BV(UART_DLAB);
 *  }
 * \endcode
 *
 */

#ifndef SET_BAUD_H
#define SET_BAUD_H


/** \addtogroup set_baud
 * @{ */



/** @} */


/**
 * #define UART_RELTOL 10
 *  Input and output macro for "set_baud.h"
 *  Maximum admissible UART baud rate error. Error between requested baud rate and
 *  real baudrate [per mill]. Default value is 10 [=1.0%].
 *
 * #define UARTL_DL
 *  Output macro from "set_baud.h"
 *  Contains baudrate divider for divisor latch register DIV0.
 *
 * #define UARTH_DL
 *  Output macro from "set_baud.h"
 *  Contains baudrate divider for divisor latch register DIV1.
 *
 * #define USE_FRACTIONAL_DIVIDER
 *  Output macro from "set_baud.h"
 *  Set to 1 if the desired baud rate tolerance could only
 *  be achieved and if fractional divider is invoked.
 *
 * #define UART_FBM_VALUE
 *  Output macro from "set_baud.h"
 *  FBM value if the fractional divider is invoked.
 *
 * #define UART_FBN_VALUE
 *  Output macro from "set_baud.h"
 *  FBN value if the fractional divider is invoked.
 */


#ifndef F_HCLK
  #error "set_baud.h needs F_HCLK"
#endif

#ifndef UART_BAUDRATE
  #error "set_baud.h needs UART_BAUDRATE"
#endif

/* Maximum admissible UART baud rate error. Error between requested baud
   rate and real baudrate [per mill]. Default value is 10 [=1.0%]. */
#ifndef UART_RELTOL
  #define UART_RELTOL 10
#endif

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

#endif /* !SET_BAUD_H */
