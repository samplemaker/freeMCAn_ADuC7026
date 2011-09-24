/** \file aduc/defs.h
 * \brief Various macro defines
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

/** Functional pointer to init functions employing 1xlink register size
 *
 */
typedef void (*initcall_t)(void);


/** Used for gathering init functions and putting their code into an
 *  extra init section
 *
 *  Macro expands to:
 *  __attribute__ ((__section__(".init.text"))) __attribute__((__cold__))
 */
#define __init       __section(.init.text) __cold
#define __section(S) __attribute__ ((__section__(#S)))
#define __cold       __attribute__((__cold__))

/** Tagged function pointers will reside in an appropriate init section and
 *  called by do_initcalls() during startup
 *
 *  Macro expands to:
 *  static initcall_t __initcall_io_init6 __attribute__((used)) 
 *  __attribute__((__section__(".initcall" "6" ".init"))) = io_init;
 */
#define register_init0(fn)    __define_initcall("0",fn,0)
#define register_init1(fn)    __define_initcall("1",fn,1)
#define register_init2(fn)    __define_initcall("2",fn,2)
#define register_init3(fn)    __define_initcall("3",fn,3)
#define register_init4(fn)    __define_initcall("4",fn,4)
#define register_init5(fn)    __define_initcall("5",fn,5)
#define register_init6(fn)    __define_initcall("6",fn,6)
#define register_init7(fn)    __define_initcall("7",fn,7)
#define register_init8(fn)    __define_initcall("8",fn,8)

#define __used __attribute__((used))

#define __define_initcall(level,fn,id)  \
        static initcall_t __initcall_##fn##id __used \
        __attribute__((__section__(".initcall" level ".init"))) = fn

/* handle data in program space */
#define PROGMEM __section(.prog_memory)

/* for implementing stubs       */
#define __weak __attribute__((weak))

#endif

/** @} */

#endif  /* !DEFS_H */
