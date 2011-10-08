/** \file data_table_empty_ram.lds
 * \brief Linker command file occupying ram for data table
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
 * \ingroup data_table_all_other_memory
 *
 * @{
 */


#define IN_LINKERCOMMAND_FILE

#include <arm7tdmi.h>

/* \todo file needs various urgent fixes! */ 
SECTIONS {
  data_table = _end ;
  data_table_end = ALIGN(( __stack_end__ - STACK_SIZE_TOTAL - MALLOC_HEAP_SIZE ), 4) ;
  data_table_size = data_table_end - data_table ;
  _end = data_table_end ;
  end = data_table_end ;
  __heap_start = data_table_end ;
  __heap_end = __heap_start + MALLOC_HEAP_SIZE ;
}
INSERT AFTER .noinit ;
ASSERT ( ( __heap_end + STACK_SIZE_TOTAL ) <= __stack_end__, "data table out of space (RAM)") ;
ASSERT ( data_table_size >= 1024, "data table size smaller than 1K" ) ;
