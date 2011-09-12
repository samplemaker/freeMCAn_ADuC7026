/** \file /aduc/defs.h
 * \brief ADUC-port specific implementation of preprocessor macros 
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

#ifndef DEFS_H
#define DEFS_H


#ifdef __ASSEMBLER__
#define _MMR_SIZE_32(rel_address) rel_address
#define _MMR_SIZE_16(rel_address) rel_address
#define _MMR_SIZE_08(rel_address) rel_address
#endif


#ifndef __ASSEMBLER__

/** Forces a softreset
 *
 */
#define soft_reset() (RSTSTA |= _BV(RST_SOFTRST))

/** Flash sector size definitions
 *
 */
#define MEM_TO_SECTOR(value) ((value) >> (FLASH_SECTOR_DEF))
#define FLASH_SECTOR_SIZE ((1UL) << (FLASH_SECTOR_DEF))

/** Alignment to next higher value 
 *
 */
#define _ALIGN(value, size) (((value)+((size)-1))&(~((size)-1)))

/** Access to 8,16,32-bit numbers by absolute adress
 *
 */
#define _MMR_RW_ABS_32(abs_address) (*(volatile uint32_t *)(abs_address))
#define _MMR_RW_ABS_16(abs_address) (*(volatile uint16_t *)(abs_address))
#define _MMR_RW_ABS_08(abs_address) (*(volatile uint8_t *)(abs_address))

/** Memory mapped register access relative to absolute adress conversion
 *
 */
#define _MMR_SIZE_32(rel_address) _MMR_RW_ABS_32((rel_address) + __MMR_BASE)
#define _MMR_SIZE_16(rel_address) _MMR_RW_ABS_16((rel_address) + __MMR_BASE)
#define _MMR_SIZE_08(rel_address) _MMR_RW_ABS_08((rel_address) + __MMR_BASE)

/** Field macro: Sets a bitmask as specified by argument
 *
 */
#define _FS(num_shifts, value) ((value) << (num_shifts))

/** Some modified defines from avrlibc to keep avr-freemcan code compliant
 *
 */
#define _BV(bit_no) (1 << (bit_no))
#define bit_is_set(mmr, bit_no) ((mmr) & _BV(bit_no))
#define bit_is_clear(mmr, bit_no) (!((mmr) & _BV(bit_no)))
#define loop_until_bit_is_set(mmr, bit_no) do { }   \
        while (bit_is_clear(mmr, bit_no))
#define loop_until_bit_is_clear(mmr, bit_no) do { } \
        while (bit_is_set(mmr, bit_no))

#endif

/** @} */

#endif  /* !DEFS_H */
