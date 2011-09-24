/** \file aduc/flash.c
 * \brief Driver support for eeprom in flash emulation
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
 *
 * \defgroup aduc_flash Flash driver for eeprom in flash emulation
 * \ingroup ADUC
 *
 * Implements driver functions for storing data into flash
 *
 * @{
 */

/*-----------------------------------------------------------------------------
 * Includes
 *-----------------------------------------------------------------------------
 */

#include <stdint.h>
#include "aduc7026.h"
#include "flash.h"

/*-----------------------------------------------------------------------------
 * Defines
 *-----------------------------------------------------------------------------
 */

/** ADUC flash sector size definition
 *
 *  2^9 = 512 bytes per sector
 */
#define FLASH_SECTOR_DEFS 9

/** Macros for sector size calculation
 *
 */
#define MEM_TO_NUMSECTOR(value) ((value) >> (FLASH_SECTOR_DEFS))
#define FLASH_SECTOR_SIZE ((1UL) << (FLASH_SECTOR_DEFS))

/** uint8_t pattern for marking a block as valid
 *
 */
#define BLOCK_VALID_MARKER 0xaa

/*-----------------------------------------------------------------------------
 * Variables
 *-----------------------------------------------------------------------------
 */

extern char __flash_start__[];
extern char _eepflash_start[], _eepflash_end[];
extern char _eepflash_cpy_start[], _eepflash_cpy_end[];

/** \brief eepflash block header
 *
 * Size of header must be even and structure must be packed
 */
struct block_header_struct {
  uint8_t  block_id;
  uint8_t  valid_marker;
  uint16_t data_size;
} __attribute__((__packed__));

typedef struct block_header_struct block_header_t;

typedef enum {
  false = 0,
  true  = 1
} BOOL;


/*-----------------------------------------------------------------------------
 * Local prototypes (not visible in other modules)
 *-----------------------------------------------------------------------------
 */


/** Low level function to erase flash sectors
 *
 * Erase all pages indexed by argument
 */
static int8_t
hw_erase(const uint8_t sector_start, const uint8_t sector_end){
  FEEMOD |= _BV(FEE_EW_PROTECTION);
  for (uint8_t cnt = sector_start; cnt <= sector_end; cnt++){
    FEEADR = FLASH_SECTOR_SIZE * cnt;
    FEECON = FEE_CMD_SINGLE_ERASE;
    /* while FEE_CONTROLLER_BUSY */
    uint8_t state = true;
    while (state){
      volatile uint8_t flash_status = FEESTA;
      switch (flash_status & (_BV(FEE_FAIL) | _BV(FEE_PASS)) ) {
         case (_BV(FEE_FAIL)) :
             /* error: exit */
           return false;
         break;
         case (_BV(FEE_PASS)) :
             /* write successfull: continue with next character */
             state = false;
         break;
         default:
             /* poll until sector erased */
         break;
      }
    }
  }
  /* data string written successfully: exit */
  return true;
}


/** Low level function to write a character buffer to flash
 *
 * Can only write 16 bit wise
 *
 * len: length [bytes] to write. must be even and integer
 * dst: logic adress to write
 * src: pointer to character buffer
 */
static int8_t
hw_write(const char *src,  const char *dst, const uint16_t len){
  uint16_t dst_vma = (uint16_t)(dst - __flash_start__);
  FEEMOD |= _BV(FEE_EW_PROTECTION);
  for (uint16_t cnt = 0; cnt < len; cnt += 2, dst_vma += 2){
    if ((cnt + 1) == len){
      /* avoid accessing last even element if len is odd */
      FEEDAT = (0xff00) | (uint8_t)(src[cnt]);
    }
    else {
      FEEDAT = ((uint16_t)(src[cnt + 1]) << 8) | (uint8_t)(src[cnt]);
    }
    FEEADR = dst_vma;
    FEECON = FEE_CMD_SINGLE_WRITE;
    /* FEESTA is reset if it is read */
    /* (while FEE_CONTROLLER_BUSY)   */
    uint8_t state = true;
    while (state){
      volatile uint8_t flash_status = FEESTA;
      switch (flash_status & (_BV(FEE_FAIL) | _BV(FEE_PASS)) ) {
         case (_BV(FEE_FAIL)) :
             /* error: exit */
             return false;
         break;
         case (_BV(FEE_PASS)) :
             /* write successfull: continue with next character */
             state = false;
         break;
         default:
             /* including case (_BV(FEE_FAIL) | _BV(FEE_PASS)) which
              * should never occur:
              * poll until data is written                 */
         break;
      }
    }
  }
  /* data string written successfully: exit */
  return true;
}


/** Set valid_marker in eepflash block header descriptor
 *
 */
inline static void
header_mark_valid (block_header_t *header)
{
  header->valid_marker = BLOCK_VALID_MARKER;
}


/** Checks if a block header field is marked as valid
 *
 */
inline static uint8_t
header_check_valid (const block_header_t *header)
{
  return(header->valid_marker == BLOCK_VALID_MARKER);
}


/** Returns pointer to next empty space inside eepflash working page
 *
 * Implemented as linear search
 */
inline static void
blocks_seek_end(char **end){
  char *cur_pos = _eepflash_start;
  /* get the first header */
  block_header_t *header_next = (block_header_t *)(cur_pos);
  while (header_check_valid(header_next)){
     /* jump to next header as long as blocks were found.
      * take into account that size and stored data_size may be different */
     cur_pos += _ALIGN(header_next->data_size, 2) + sizeof(block_header_t);
     header_next = (block_header_t *)(cur_pos);
  }
  *end = cur_pos;
}


/** Returns most recent valid block with respect to BLOCKID
 *
 * Returns false if no valid block was found, true elsewise.
 * Implemented as linear search
 *
 * Note: Returnpointer points to the correct start of block
 * until a new eepflash_write is forced.
 */
inline static int8_t
blocks_getblock(char **block, const uint8_t block_id){
  BOOL valid_block_found = false;
  /* from beginning of the eepflash section */
  char *cur_pos = _eepflash_start;
  /* get first header */
  block_header_t *header = (block_header_t *)(cur_pos);
  /* for all valid header */
  while (header_check_valid(header)){
     /* if a header with correct id is found */
     if (header->block_id == block_id){
       /* update pointer to start of block-header */
       *block = cur_pos;
       valid_block_found = true;
     }
     /* jump to next header. since FEE provides only even block
        length but the user may have specified an odd length (and
        this stored in flash) we must align to get the true length  */
     cur_pos += _ALIGN(header->data_size, 2) +
                sizeof(block_header_t);
     /* get next header */
     header = (block_header_t *)(cur_pos);
  }
  return(valid_block_found);
}


/** Perform a cleanup into copy page and write data back to eepflash
 *
 */
inline static void
blocks_cleanup(char **end){
  /* copy most recent and valid block from working page to copy page */
  char *cur_src;
  char *cur_dst = _eepflash_cpy_start;
  /* for each block ID */
  for (uint8_t block_id = 0; block_id < NUM_BLOCKS; block_id++){
    if (blocks_getblock((char **)&cur_src, block_id)){
      /* get header */
      block_header_t *header = (block_header_t *)(cur_src);
       /* since FEE provides only even block length but the user may
          have specified an odd length (and this is stored in flash)
          we must align to get the true length           */
      const uint16_t block_len = _ALIGN(header->data_size, 2) +
                                 sizeof(block_header_t);
      /* copy most recent block to copy page */
      hw_write(cur_src, cur_dst, block_len);
      cur_dst += block_len;
    }
    /* else: no block found -> therefore nothing to do */
  }
  /* erase working page (eepflash sectors) */
  const uint8_t wp_sector_start =
                MEM_TO_NUMSECTOR(_eepflash_start - __flash_start__ );
  const uint8_t wp_sector_end =
                MEM_TO_NUMSECTOR(_eepflash_end - __flash_start__) - 1;
  hw_erase(wp_sector_start, wp_sector_end);
  /* write copy page to eepflash           */
  const uint16_t cpy_data_length = cur_dst - _eepflash_cpy_start;
  hw_write(_eepflash_cpy_start, _eepflash_start, cpy_data_length);
  /* erase copy page                       */
  const uint8_t cp_sector_start =
                MEM_TO_NUMSECTOR(_eepflash_cpy_start - __flash_start__);
  const uint8_t cp_sector_end =
                MEM_TO_NUMSECTOR(_eepflash_cpy_end - __flash_start__) - 1;
  /* \todo:
   * actually it is not necessary to erase the complete copy page
   * but only sectors where used data resides
   * -> _eepflash_cpy_start + cpy_data_length */
  hw_erase(cp_sector_start, cp_sector_end);
  /* point to next free space in flash */
  *end = (_eepflash_start - _eepflash_cpy_start) + cur_dst;
}


/** Returns most recent valid block-data with respect to BLOCKID
 *
 * Returns false if no valid block was found, true elsewise
 *
 * Note: Returnpointer points to the correct data until
 * a new eepflash_write is forced.
 */
int8_t
eepflash_read(char **data, uint16_t *user_len, const uint8_t block_id){
  if (blocks_getblock(data, block_id)){
    /* get header */
    block_header_t *header = (block_header_t *)(*data);
    /* get virtual datasize (user datasize of data) */
    *user_len = header->data_size;
    /* adjust *data to the start of the data field */
    *data = *data + sizeof(block_header_t);
    return true;
  }
  else{
    return false;
  }
}


/** Write a block with BLOCKID into eepflash
 *
 * If len is odd the true data stored is aligned (round up). Correct
 * length will be recovered since correct len is stored inside the header
 * If eepflash area is full a clean up into copy page and write
 * back into working page is performed.
 */
void
eepflash_write(const char *src, const uint16_t user_len, const uint8_t block_id){
  char *cur_pos;
  /* seek to the end of eepflash */
  blocks_seek_end(&cur_pos);
  /* create a new blockheader with block_id and virtual
     user length of data (length may be odd) */
  block_header_t block_header;
  block_header.block_id = block_id;
  block_header.data_size = user_len;
  header_mark_valid(&block_header);
  /* the fee - flash utilities require an even length as true
     data length */
  const uint16_t data_len = _ALIGN(user_len, 2);
  /* if we are running out of space ... */
  if ((cur_pos + sizeof(block_header_t) + data_len) > _eepflash_end){
    /* ... perform a cleanup and seek to the end */
    blocks_cleanup((char **)&cur_pos);
  }
  /* if user wants to write more data than possible this will crash */
  if ((cur_pos + sizeof(block_header_t) + data_len) > _eepflash_end) {
    /* don't write any data or cause error handling ARM exception */
  }
  else {
    /* write header of new block                           */
    hw_write((char *)&block_header, cur_pos, sizeof(block_header_t));
    /* write data field of new block                       */
    hw_write(src, cur_pos + sizeof(block_header_t), user_len);
  }
}


/** @} */
