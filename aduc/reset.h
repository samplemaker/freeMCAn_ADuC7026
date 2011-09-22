/** \file ./reset.h
 * \brief Reset configuration for startup code and core clock defintions
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
 * \defgroup ADUC
 * \ingroup ADUC
 *
 * @{
 */

#ifndef RESET_H
#define RESET_H


/** Boot set up for PLL and power control (if required)
 *
 */
#define RESET_POWER_AND_PLL_AT_BOOT  1

/* CD = 0 (No clock divider), PC = 0 (Active Mode) */
#define POWCON_RESET_CFG             0x0    
/* MDCLK = 1 (PLL, Default configuration )         */
#define PLLCON_RESET_CFG             0x1    


#ifndef __ASSEMBLER__

/** Frequency definition external crystal
 *
 */
#define F_XTAL  32768ULL

/** Frequency definition system clock
 *
 * The PLL locks onto a multiple (1275) of the internal oscillator or an
 * external crystal to provide a stabel 41.78 MHz clock (UCLK)
 * for the system. 
 *
 */
#define F_UCLK  41780000ULL

/** Frequency definition internal resonator
 *
 * 32.768 kHz +-3% oscillator
 * ADuC7019/20/21/22/24/25/26/27/28/29
 *
 */
#define F_OSC 32768ULL

/** Frequency definition of CPU core frequency
 *
 *   To allow power saving, the core can operate at F_UCLK
 *   frequency, or at binary submultiples of it. The actual core 
 *   operating frequency (UCLK/2^CD) is refered to as HCLK. 
 *   F_HCLK = F_UCLK / 2^CD
 *   0: run core with 41.78 Mhz
 *   1: run core with 41.78/2 Mhz
 *   2: run core with 41.78/4 Mhz
 *   3: run core with 41.78/8 Mhz -> default without boot
 *   ...
 *
 */
#if RESET_POWER_AND_PLL_AT_BOOT
  #define F_HCLK (F_UCLK / (1ULL << ((POWCON_RESET_CFG) & 0x7)) )
#else
  #define F_HCLK 5222500ULL
#endif


#endif

/** @} */

#endif  /* !RESET_H */
