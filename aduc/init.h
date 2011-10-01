/** \file aduc/init.h
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
 * \ingroup aduc_init
 *
 * @{
 */

#ifndef INIT_H
#define INIT_H

#if !defined(STR) && !defined(_STR1)
/* macros used for stringification */
#define STR1(x)  #x
#define STR(x)  STR1(x)
#endif 


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
#define __used       __attribute__((used))

/** Tagged function pointers will reside in an appropriate init section and
 *  called by do_initcalls() during startup
 *
 *  Macro expands to:
 *  static initcall_t __initcall_io_init6 __attribute__((used)) 
 *  __attribute__((__section__(".initcall" "6" ".init"))) = io_init;
 */
#define module_init(fn, level)    __define_initcall(fn, level)

#define __define_initcall(fn, level)  \
        static initcall_t __initcall_##fn##level __used \
        __attribute__((__section__(".initcall" STR(level) ".init"))) = fn


/** @} */

#endif  /* !INIT_H */
