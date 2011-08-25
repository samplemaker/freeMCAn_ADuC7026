/** \file /aduc/project.ld
 * \brief ADUC-port specific linker command file
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

/** memory regions for device ADUC7026
 *
 * sram
 * total: 8 kbyte
 * range: 0x10000 .. 0x11FFF
 *
 * flash - userspace
 * total: 62 kbyte = 31k x 16 bit 
 * range: 0x80000 .. 0x8F7FF
 * flash - reserved for factory-configured boot page (onchip kernel)
 * total: 2 kbyte = 1k x 16 bit 
 * range: 0x8F800 .. 0x8FFFF
 *
 * flash page (sector size) is 512 byte
 *
 */

MEMORY
{
  flash   (rx) : ORIGIN = 0x00080000, LENGTH = 62k
  ram     (rw) : ORIGIN = 0x00010000, LENGTH = 8k
}

__stack_end__ = ORIGIN(ram) + LENGTH(ram) - 4;

/**  Output sections
 *
 */
SECTIONS
{

/** Code and const variables which remain in flash
 *
 */
  .text :
  {
    *(.vectors)  /* jump island for ARM modes */
    *(.init0)    /* target initialization startup code */
    *(.text)     /* code */
    *(.text.*)
    *(.rodata)   /* read only constants for flash */
    *(.rodata*)  /* wildcart matched residual read only data i.e. .rodata.str1.1 */
    *(.glue_7t)  /* input sections arm, thumb interworking code */
    *(.glue_7)
    . = ALIGN(4);
    _etext = . ;
    PROVIDE (etext = .);
  } >flash

/** read & write and initialized global variables copied from flash
 *  to ram at startup. Load memory adress at end of .text. Virtual memory adress
 *  at ram
 *
 */
  .data : AT (ADDR (.text) + SIZEOF (.text))
  {
    _data = . ;
    *(.data)
    . = ALIGN(4);
    _edata = . ;
    PROVIDE (edata = .);
  } >ram

  _data_load_start = LOADADDR(.data);
  _data_load_end = _data_load_start + SIZEOF(.data);

/** zero inititialized and read write global variables
 *
 */
 .bss : AT (ADDR (.bss))
  {
    __bss_start = . ;
    __bss_start__ = . ;
    *(.bss)
    *(COMMON)        /* common symbols like i.e 'int' */
    . = ALIGN(4);
    __bss_end__ = . ;
  } >ram

/** global data which remains in ram only and which is not cleared after reset
 *
 */
  .noinit :
  {
    PROVIDE (__noinit_start = .) ;
    *(.noinit)
    . = ALIGN(4);
    PROVIDE (__noinit_end = .) ;
    _end = . ;
    PROVIDE (end = .);
    PROVIDE (__heap_start = .) ;
  }  >ram

  /* Stabs debugging sections.  */
  .stab            0 : { *(.stab) }
  .stabstr         0 : { *(.stabstr) }
  .stab.excl       0 : { *(.stab.excl) }
  .stab.exclstr    0 : { *(.stab.exclstr) }
  .stab.index      0 : { *(.stab.index) }
  .stab.indexstr   0 : { *(.stab.indexstr) }
  .comment         0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug           0 : { *(.debug) }
  .line            0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo   0 : { *(.debug_srcinfo) }
  .debug_sfnames   0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges   0 : { *(.debug_aranges) }
  .debug_pubnames  0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info      0 : { *(.debug_info) }
  .debug_abbrev    0 : { *(.debug_abbrev) }
  .debug_line      0 : { *(.debug_line) }
  .debug_frame     0 : { *(.debug_frame) }
  .debug_str       0 : { *(.debug_str) }
  .debug_loc       0 : { *(.debug_loc) }
  .debug_macinfo   0 : { *(.debug_macinfo) }
  .debug_ranges    0 : { *(.debug_ranges) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }

}


/** @} */
