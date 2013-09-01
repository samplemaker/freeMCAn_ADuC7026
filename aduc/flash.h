/** \file aduc/flash.h
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
 * \ingroup aduc_flash
 *
 * @{
 */

#ifndef FLASH_H
#define FLASH_H

#include <stdbool.h>
#include <stddef.h>

/* Eepflash memory service identifier. Define here various BLOCKIDs */
enum {
  EEPFLASH_FRAME_CMD_PARAMS,
  EEPFLASH_DATA_TABLE,
  /* Termination is need by the EEPFLASH tools */
  EEPFLASH_NUM_USED_BLOCKS
};


void eepflash_write(const char *src, const uint16_t user_len, const uint8_t block_id);
size_t eepflash_copy_block(char *p_ram, const uint8_t block_id);


/** @} */

#endif /* !FLASH_H */


/*
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
