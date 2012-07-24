/** \file aduc/init.c
 * \brief Module initialization
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
 * \defgroup aduc_init Modul initialization at start up
 * \ingroup ADUC
 *
 * @{
 */


/*-----------------------------------------------------------------------------
 * Includes
 *-----------------------------------------------------------------------------
 */

#include "init.h"


/*-----------------------------------------------------------------------------
 * Variables
 *-----------------------------------------------------------------------------
 */

extern initcall_t __initcall_start[], __initcall_end[];


/*-----------------------------------------------------------------------------
 * Prototypes
 *-----------------------------------------------------------------------------
 */

void __init do_initcalls(void);


/** Execute functions in .init sections at start up
 *
 * Taken and modified from linux-2.6.27.6 kernel-modul initialization
 */
void do_initcalls(void)
{
  initcall_t *call;

  for (call = __initcall_start; call < __initcall_end; call++)
    (*call)();
}


/** @} */
