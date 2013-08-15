/** \file firmware/beep.h
 * \brief Providing a "beep" loudspeaker function
 *
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
 * 
 * @{
 */

#ifndef BEEP_H
#define BEEP_H

/**
 *  Gating signal
 */

#define BEEP_LENGTH 21000ULL /*us*/


/**
* Square wave generator waveform generation for beep signal
*/

/** Frequency (resonant frequency of piezo sounder) */
#define BEEP_FREQUENCY 4000ULL

void beep_kill_all(void);

inline static
void _beep(void)
{
  /* dcyc = 50% */
  PWMCH0 = 0x0;

  /* trigger gating signal (start timer) */
  T0CON |= _BV(TIMER0_ENABLE);
}

#endif /* !BEEP_H */

/** @} */

