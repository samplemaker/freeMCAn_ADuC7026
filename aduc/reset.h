/** \file aduc/reset.h
 * \brief Implements functions to reset the target
 *
 * \author Copyright (C) 2011 samplemaker
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
 * \defgroup reset_target Reset target 
 * \ingroup ADUC
 *
 * @{
 */

#ifndef RESET_H
#define RESET_H

#include "stdint.h"

#include "aduc.h"
#include "defs.h"


/** Forces a softreset
 *
 * Note: Could be implemented as hard reset using 
 *       a watchdog timeout at TIMER3
 */
static inline
void soft_reset(void) __attribute__((noreturn));
static inline
void soft_reset(void)
{
  RSTSTA |= _BV(RST_SOFTRST);
  /* supress warnings */
  while (1){}
}


/** @} */

#endif  /* !RESET_H */
