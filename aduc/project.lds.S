/** \file /aduc/project.lds.S
 * \brief ADUC-port specific linker command file
 *
 * \author Copyright (C) 2011 samplemaker
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * \defgroup aduc_linker_cmd Linker command definitions for ADUC bare silicon
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
 * flash sector size is 512 byte
 *
 */

#define IN_LINKERCOMMAND_FILE

#include "arm7tdmi.h"


OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

MEMORY
{
  flash   (rx) : ORIGIN = 0x00080000, LENGTH = 62k
  ram     (rw) : ORIGIN = 0x00010000, LENGTH = 8k
}

/** Linker symbols used for eeprom/flash and stack
 *
 */
__flash_start__ = ORIGIN(flash);
__flash_end__ = ORIGIN(flash) + LENGTH(flash);
__ram_end__ = ORIGIN(ram) + LENGTH(ram);
__stack_start__ = __ram_end__ - STACK_SIZE_TOTAL;

/**  Output sections
 *
 */
SECTIONS
{

  .init : {                  /* flash @ 0x80000 */
    *(.vector)               /* jump island for various ARM modes */
    KEEP(*(.vector))         /* to be kept although --gc-sections is passed to ld */
    . = ALIGN(4);
    *(.init_target)          /* startup code; referenced by vector  */
    *(.init.text)            /* do_initcalls(); referenced by initcode */
    __initcall_start = . ;
    *(.initcall0.init)       /* function pointers to init functions */
    KEEP(*(.initcall0.init)) /* these are not referenced and must be kept */
    *(.initcall1.init)
    KEEP(*(.initcall1.init))
    *(.initcall2.init)
    KEEP(*(.initcall2.init))
    *(.initcall3.init)
    KEEP(*(.initcall3.init))
    *(.initcall4.init)
    KEEP(*(.initcall4.init))
    *(.initcall5.init)
    KEEP(*(.initcall5.init))
    *(.initcall6.init)
    KEEP(*(.initcall6.init))
    *(.initcall7.init)
    KEEP(*(.initcall7.init))
    *(.initcall8.init)
    KEEP(*(.initcall8.init))
    __initcall_end = . ;
    . = ALIGN(4);
  } >flash

/** Code and const variables remaining in the flash
 *
 */
  .text :
  {
    *(.text)     /* executable code */
    *(.text.*)
    *(.prog_memory)
    *(.rodata)   /* read-only constants */
    *(.rodata*)  /* read-only data like .rodata.str1.1 */
    *(.glue_7t)  /* input sections arm, thumb interworking code */
    *(.glue_7)
    . = ALIGN(4);
    _etext = . ;
    PROVIDE (etext = .);
  } >flash

/** Initialized global variables which are copied from flash to ram at startup.
 *  Load memory adress points to the end of .text. Virtual memory adress
 *  points to the beginning of ram origin.
 *
 */
  .data : AT (ADDR (.text) + SIZEOF (.text))
  {
    _data = . ;
    *(.data)
    *(.data*)    /* occurs if --data-sections is specified */
    *(.ramrun)   /* code execution in ram */
    . = ALIGN(4);
    _edata = . ;
    PROVIDE (edata = .);
  } >ram

  _data_load_start = LOADADDR(.data);
  _data_load_end = _data_load_start + SIZEOF(.data);

/** Initialized to zero and read-write global variables
 *
 */
 .bss : AT (ADDR (.bss))
  {
    __bss_start = . ;
    __bss_start__ = . ;
    *(.bss)
    *(COMMON)        /* common symbols like 'global int' */
    . = ALIGN(4);
    __bss_end__ = . ;
    __bss_end = . ;
  } >ram

/** Ram data that should not be deleted during a system reset
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

/** Check whether sufficient space is left for the stack
 *
 */
 .reserved_stack :
  {
    . = . + STACK_SIZE_TOTAL ;
  }  >ram

/** eepflash configuration
 *
 */
  /* locate to 30 sectors above the end of flash code & data 
  _eepflash_section1_start = _data_load_end + 30*FLASH_SECTOR_SIZE_BYTES; */
  _eepflash_section1_start = 0x85000;

  /* rectify this address on flash sector boundarys */
  _eepflash_section1_start = ALIGN(_eepflash_section1_start, FLASH_SECTOR_SIZE_BYTES);

  /* flash section size: 
   * store 2 x 11 bit data table, 3byte each element plus some extra space 
   * for additional data and overhead ~ 24 + 2 sectors before sections1 & 2
   * are swapped into alternate section
   */
  _eepflash_section_size = 26*FLASH_SECTOR_SIZE_BYTES;

  /* automatic gather for all flash space available
     _eepflash_section_size = (__flash_end__ - FLASH_SECTOR_SIZE_BYTES 
                               - _eepflash_section1_start) / 2;  
   */

  _eepflash_section1_end = _eepflash_section1_start + _eepflash_section_size;
  _eepflash_section2_start = _eepflash_section1_end;
  _eepflash_section2_end = _eepflash_section2_start + _eepflash_section_size;

/** to avoid orphaned sections are placed somewhere in this area and 
 * to check wheather we are running out of flash space we put one
 * empty output section to force a linker error if something goes wrong
 */
  .reserved_eepflash _eepflash_section1_start :
  {
    /* eepflash section1 and section2 */    
    . = . + 2*_eepflash_section_size;
    /* ADuC7019/ADuC702x Silicon Anomaly List:
     * Access to Flash Address 0x0008F7FF Causes a Data Abort [er022] 
     * --> avoid last sector to be allocated for eepflash 
     */
    . = . + FLASH_SECTOR_SIZE_BYTES;
  }  >flash


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
