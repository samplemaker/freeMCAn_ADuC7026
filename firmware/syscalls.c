/** \file syscalls.c
 * \brief Newlib stub implementation if freeMCAn is linked against newlib
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
 * \defgroup syscall_stubs Syscall stubs implementation for newlib
 * \ingroup ADUC
 *
 * Non operating system interface stub implementation.
 * If we want to link against C librarys like the newlib we must spent
 * some minimalistic error handling and exit code to get libc to link, 
 * and exit gracefully in case of a fail.
 * Checkout the newlib documentation how to implement the stubs if you 
 * intend to link freemcan against newlib.
 *
 * @{
 */

#include <sys/types.h>

/* Implementation for malloc() gathering empty RAM space */

extern char _end; 

void *_sbrk ( int incr )
{ 
  static char *heap_end = NULL; 
  char *prev_heap_end; 

  if (heap_end == NULL)
    heap_end = &_end; 
  prev_heap_end = heap_end; 

  /* Error handling: Heap runs into stack 
   * Could call an ARM abort handler
   * if (heap_end + incr > stack_ptr) {
   *   printk ("Heap/Stack collision! \n");
   *   abort (); }
   */

  heap_end += incr; 
  return (void *) prev_heap_end; 
} 
