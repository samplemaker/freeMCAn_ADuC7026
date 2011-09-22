/** \file ./flash.h
 * \brief Driver support for eeprom-flash emulation
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
 * \addtogroup LIB_FLASH
 * @{
 */

#ifndef FLASH_H
#define FLASH_H

#define NUM_BLOCKS 3

#define BLOCKID_0 0
#define BLOCKID_1 1
#define BLOCKID_2 2


void eepflash_write(const char *src, const uint16_t user_len, const uint8_t block_id);
int8_t eepflash_read(char **dst, uint16_t *len, const uint8_t block_id);


/** @} */

#endif /* !FLASH_H */


/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
