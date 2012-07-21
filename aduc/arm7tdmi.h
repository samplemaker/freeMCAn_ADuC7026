/** \file aduc/arm7tdmi.h
 * \brief Defines for ADUC - ARM7TDMI digital core
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
 * \defgroup aduc_ARM7TDMI Defines for ADUC-ARM7TDMI digital core
 * \ingroup ADUC
 *
 * ARM7TDMI specific defines: 
 * Current program status register (cpsr) bit definitions and mode identifiers.
 * Stack.
 *
 * @{
 */

#ifndef ARM7TDMI_H
#define ARM7TDMI_H

/* Project stack sizes for all processor modes
 */
#define STACKSIZE_UND  4
#define STACKSIZE_SVC  256
#define STACKSIZE_ABT  4
#define STACKSIZE_FIQ  4
#define STACKSIZE_IRQ  256
#define STACKSIZE_USR  256

/** Total stack size used
 */
#define STACK_SIZE_TOTAL ((STACKSIZE_UND) + (STACKSIZE_SVC) + (STACKSIZE_ABT) \
                        + (STACKSIZE_FIQ) + (STACKSIZE_IRQ) + (STACKSIZE_USR))

/** 2^9 = 512 bytes per flash sector
 */
#define FLASH_SECTOR_SIZE 9

/** Flash sector size definition
 */
#define FLASH_SECTOR_SIZE_BYTES (1 << (FLASH_SECTOR_SIZE))


#ifndef IN_LINKERCOMMAND_FILE

/* Current program status register (cpsr) bit definitions and mode identifiers
 *
 * CPSR_c = [I F T M4 M3 M2 M1 M0]
 *
 * See: ARM7TDMI Technical Reference Manual p. 61
 *
 */

/** IRQ Mask
 */
#define I_FLAG  0x80

/** FIQ Mask
 */
#define F_FLAG  0x40

/** Arm <> Thumb state Mask
 */
#define T_FLAG  0x20

/** User Mode Mask
 */
#define M_USR   0x10

/** Fast Interrupt Mode Mask
 */
#define M_FIQ   0x11

/** Interrupt Mode Mask
 */
#define M_IRQ   0x12

/** Supervisor Mode Mask
 */
#define M_SVC   0x13

/** Abort Mode Mask
 */
#define M_ABT   0x17

/** Undefined Mode Mask
 */
#define M_UND   0x1B

/** System Mode Mask
 */
#define M_SYS   0x1F


#endif /* !IN_LINKERCOMMAND_FILE */


/** @} */

#endif  /* !ARM7TDMI_H */
