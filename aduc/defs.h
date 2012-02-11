/** \file aduc/defs.h
 * \brief Macro defines accessing and processing MMRs
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
 * \defgroup aduc_generic_defines Generic macros
 * \ingroup ADUC
 *
 * @{
 */

#ifndef DEFS_H
#define DEFS_H


/** Field macro: Creates a bitmask as specified by argument
 *
 */
#define _FS(num_shifts, value) ((value) << (num_shifts))

/** One bit macro: Creates a one bit bitmask at position bit_no
 *
 */
#define _BV(bit_no) (1 << (bit_no))


#ifdef __ASSEMBLER__

#define _MMR_SIZE_32(rel_address) rel_address
#define _MMR_SIZE_16(rel_address) rel_address
#define _MMR_SIZE_08(rel_address) rel_address

#else /* !__ASSEMBLER__ */

/** Access to 8,16,32-bit numbers by absolute adress
 *
 */
#define _MMR_RW_32(abs_address) (*(volatile uint32_t *)(abs_address))
#define _MMR_RW_16(abs_address) (*(volatile uint16_t *)(abs_address))
#define _MMR_RW_08(abs_address) (*(volatile uint8_t *)(abs_address))

/** Memory mapped register access relative to absolute adress conversion
 *
 */
#define _MMR_SIZE_32(rel_address) _MMR_RW_32((rel_address) + __MMR_BASE)
#define _MMR_SIZE_16(rel_address) _MMR_RW_16((rel_address) + __MMR_BASE)
#define _MMR_SIZE_08(rel_address) _MMR_RW_08((rel_address) + __MMR_BASE)

/** Set bit position bit_no to one
 *
 */
#define bit_is_set(mmr, bit_no) ((mmr) & _BV(bit_no))

/** Set bit position bit_no to zero
 *
 */
#define bit_is_clear(mmr, bit_no) (!((mmr) & _BV(bit_no)))

/** Loop until bitnumber bit_no is set to one im MMR
 *
 */
#define loop_until_bit_is_set(mmr, bit_no) do { }   \
        while (bit_is_clear(mmr, bit_no))

/** Loop until bitnumber bit_no is set to zero im MMR
 *
 */
#define loop_until_bit_is_clear(mmr, bit_no) do { } \
        while (bit_is_set(mmr, bit_no))

/** Alignment to next higher value
 *
 */
#define _ALIGN(value, size) (((value)+((size)-1))&(~((size)-1)))


#endif /* !__ASSEMBLER__ */


/** Binary bitmasks used in user code and assembler
 *
 */
#define MASK_00000            0
#define MASK_00001            1
#define MASK_00010            2
#define MASK_00011            3
#define MASK_00100            4
#define MASK_00101            5
#define MASK_00110            6
#define MASK_00111            7
#define MASK_01000            8
#define MASK_01001            9
#define MASK_01010            10
#define MASK_01011            11
#define MASK_01100            12
#define MASK_01101            13
#define MASK_01110            14
#define MASK_01111            15
#define MASK_10000            16
#define MASK_10001            17
#define MASK_10010            18
#define MASK_10011            19
#define MASK_10100            20
#define MASK_10101            21
#define MASK_10110            22
#define MASK_10111            23
#define MASK_11000            24
#define MASK_11001            25
#define MASK_11010            26
#define MASK_11011            27
#define MASK_11100            28
#define MASK_11101            29
#define MASK_11110            30
#define MASK_11111            31

#define MASK_0000             0
#define MASK_0001             1
#define MASK_0010             2
#define MASK_0011             3
#define MASK_0100             4
#define MASK_0101             5
#define MASK_0110             6
#define MASK_0111             7
#define MASK_1000             8
#define MASK_1001             9
#define MASK_1010             10
#define MASK_1011             11
#define MASK_1100             12
#define MASK_1101             13
#define MASK_1110             14
#define MASK_1111             15

#define MASK_000              0
#define MASK_001              1
#define MASK_010              2
#define MASK_011              3
#define MASK_100              4
#define MASK_101              5
#define MASK_110              6
#define MASK_111              7

#define MASK_00               0
#define MASK_01               1
#define MASK_10               2
#define MASK_11               3


/** @} */

#endif  /* !DEFS_H */
