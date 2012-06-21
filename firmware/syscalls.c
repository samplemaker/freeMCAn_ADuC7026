/** \file syscalls.c
 * \brief Syscall stub implementations
 *
 * \author Copyright (C) 2012 samplemaker
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
 * \defgroup syscall_stubs Syscall implementations for newlib
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

/* Implementation gathering empty RAM for the heap (malloc()) */

extern char __heap_start;
extern char __stack_start__;

void *_sbrk (int nbytes)
{
  static char *heap_end = NULL;
  char *prev_heap_end;

  /* if heap_end is not yet initialized
     (first alloc): set heap_end to _end/__heap_start */
  if (heap_end == NULL){
    heap_end = &__heap_start;
  }
  /* save old heap_end */
  prev_heap_end = heap_end;

  if ((heap_end + nbytes) < &__stack_start__) {
    /* if there is enough free space adjust to new heap_end
       otherwise hook system */
    heap_end += nbytes;
    return (void *) prev_heap_end;
  }
  else{
    /* Heap - Stack collision! */
    while (1){}
  }

}
