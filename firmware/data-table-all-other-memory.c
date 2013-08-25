/** \file firmware/data-table-all-other-memory.c
 * \brief Data table occupying all the rest of the SRAM
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
 * \defgroup data_table_all_other_memory Data table occupying all the rest of the SRAM
 * \ingroup firmware_personality_groups
 *
 * Data table occupying all the rest of the SRAM (in conjunction with
 * linker script).
 *
 * @{
 */

#include "init.h"

/* import the symbols in data type inspecific way */
extern char data_table[];
extern char data_table_end[];
extern char data_table[];

/** Zero table
 * 
 * Since table is not located within .bss it is not initialized.
 * We have to do this on our own.
 * Needs to be called once on startup.
 *
 * Note: Code will zero data starting at ADDR $r1
 *       and ending at ADDR $r2. ADDR $r2 is not cleared itself.
 *
 */
void __init data_table_init(void)
{
  asm volatile("\n\t"  
               "mov  r0, #0                              \n\t"
               "ldr  r1, =data_table                     \n\t"
               "ldr  r2, =data_table_end                 \n\t"

               "zero_table%=:                            \n\t"
               "cmp  r1, r2                              \n\t"
               /* if (r1-r2)<0 (N, V flag): *(r1)=r0 then r1+=4 */
               "stmltia r1!, {r0}                        \n\t"
               "blt zero_table%=                         \n\t"

               /* if (r1-r2)<0 (C flag):  *(r1)=r0 then r1+=4
               "strlo r0,[r1], #4                        \n\t"
               "blo  zero_table                          \n\t"  */

               : /* output operands */
               : /* input operands */
                 /* clobbers, change condition code flag, 
                  * store all cached values before and reload them after */
               : "r0", "r1", "r2", "cc", "memory"
               );
}

/** Put function into init section, register function pointer and
 *  execute function at start up
 */
module_init(data_table_init, 5);


/** @} */


/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
