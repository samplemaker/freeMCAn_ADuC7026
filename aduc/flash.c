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
 * Implements driver functions for storing data into flash.
 * Note: Eepflash is nonreentrant!
 *
 * @{
 */

/*-----------------------------------------------------------------------------
 * Includes
 *-----------------------------------------------------------------------------
 */

#include <stdint.h>

#include "aduc.h"
#include "init.h"
#include "flash.h"

/*-----------------------------------------------------------------------------
 * Defines
 *-----------------------------------------------------------------------------
 */

/** ADUC flash sector size definition
 *
 *  2^9 = 512 bytes per sector
 */
#define FLASH_SIZE_CONVERSION 9

/** Macros for sector size calculation
 *
 */
#define FLASHADDR_TO_NUMSECTOR(addr, start) (((addr) - (start)) >> (FLASH_SIZE_CONVERSION))
#define FLASH_SECTOR_SIZE ((1UL) << (FLASH_SIZE_CONVERSION))

/** uint8_t pattern for marking a block as valid
 *
 */
#define BLOCK_VALID_MARKER 0xaa

/*-----------------------------------------------------------------------------
 * Variables
 *-----------------------------------------------------------------------------
 */


extern char __flash_start__[];
extern char _eepflash_section1_start[], _eepflash_section1_end[];
extern char _eepflash_section2_start[], _eepflash_section2_end[];


typedef enum {
  false = 0,
  true  = 1
} BOOL;


/** \brief Hold most recent flash section address
 *
 * Hold alternate and active flash section address
 */
struct flash_sections_struct {
  char *alternate_start;
  char *alternate_end;
  char *active_start;
  char *active_end;
};

typedef struct flash_sections_struct flash_sections_t;

static flash_sections_t flash_sections;

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



/*-----------------------------------------------------------------------------
 * Local prototypes (not visible in other modules)
 *-----------------------------------------------------------------------------
 */



/*-----------------------------------------------------------------------------
 * Low level functions (direct hardware access)
 *-----------------------------------------------------------------------------
 */


/** Low level function to erase flash sectors
 *
 * Erase all sectors indexed by argument
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

/*-----------------------------------------------------------------------------
 * Abtraction layer functions
 *-----------------------------------------------------------------------------
 */

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


static
void __init eepflash_init(void){

  /* check if there are valid markers inside section 2 */
  block_header_t *header = (block_header_t *)(_eepflash_section2_start);

  if (header_check_valid(header)){
    /* yes: mark section two as active and section one as alternate */
    flash_sections.active_start = _eepflash_section2_start;
    flash_sections.active_end = _eepflash_section2_end;
    flash_sections.alternate_start = _eepflash_section1_start;
    flash_sections.alternate_end = _eepflash_section1_end;
  } else
  {
    /* no: maybe flash is empty at all or currently section one is in use,
     * then mark section one as active and section two as alternate
     */
    flash_sections.active_start = _eepflash_section1_start;
    flash_sections.active_end = _eepflash_section1_end;
    flash_sections.alternate_start = _eepflash_section2_start;
    flash_sections.alternate_end = _eepflash_section2_end;
  };
}

module_init(eepflash_init, 0);


inline static
void eepflash_swap_and_erase_sections(void){

  /* erase current active section (working section) */
  const uint8_t sector_start =
    FLASHADDR_TO_NUMSECTOR(flash_sections.active_start, __flash_start__ );
  const uint8_t sector_end =
    FLASHADDR_TO_NUMSECTOR(flash_sections.active_end, __flash_start__) - 1;

  hw_erase(sector_start, sector_end);

  /* exchange section addresses */
  char *temp;

  temp = flash_sections.active_start;
  flash_sections.active_start = flash_sections.alternate_start;
  flash_sections.alternate_start = temp;

  temp = flash_sections.active_end;
  flash_sections.active_end = flash_sections.alternate_end;
  flash_sections.alternate_end = temp;
}


/** Returns pointer to next empty space inside eepflash active working area
 *
 * Implemented as linear search
 */
inline static void
blocks_seek_end(char **end){
  char *cur_pos = flash_sections.active_start;
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
  /* from beginning of the active eepflash section */
  char *cur_pos = flash_sections.active_start;
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


/** Perform a cleanup into alternate flash section and swap
 * alternate and active section. Expunge block with block_id.
 *
 */
inline static void
blocks_cleanup(char **end, const uint8_t expunge_id){
  /* copy most recent and valid block from active section to alternate section */
  char *cur_src;
  char *cur_dst = flash_sections.alternate_start;
  /* for each block ID */
  for (uint8_t block_id = 0; block_id < NUM_BLOCKS; block_id++){
    if (blocks_getblock((char **)&cur_src, block_id) && (block_id != expunge_id)){
      /* get header */
      block_header_t *header = (block_header_t *)(cur_src);
       /* since FEE provides only even block length but the user may
          have specified an odd length (and this is stored in flash)
          we must align to get the true length           */
      const uint16_t block_len = _ALIGN(header->data_size, 2) +
                                 sizeof(block_header_t);
      /* copy most recent block to alternate section */
      hw_write(cur_src, cur_dst, block_len);
      cur_dst += block_len;
    }
    /* else: no block found -> therefore nothing to do */
  }

  /* swap working sections:
   * current active section gets alternate (and will be erased) and alternate
   * gets active
   */
  eepflash_swap_and_erase_sections();

  /* return free area in (now) active flash section */
  *end = cur_dst;
}


/** Returns most recent valid block-data with respect to BLOCKID
 *
 *  Returns false if no valid block was found, true elsewise
 *
 */
int8_t
eepflash_copy_block(char *p_ram, const uint8_t block_id){
  char *p_flash;

  if (blocks_getblock((char **)&p_flash, block_id)){

    /* get header */
    block_header_t *header = (block_header_t *)(p_flash);
    /* point to data in flash */
    char *p_data = p_flash + sizeof(block_header_t);
    /* point to the end of user-data (might be smaller than the written data) */
    const char *p_end = (char *) (p_data + header->data_size);

   /* point to destiny in ram */
    char *p_dst = p_ram; 
   /* copy data from flash to ram */

    while (p_data != p_end)
      *(p_dst++) = *(p_data++);

    return true;
  }
  else{
    return false;
  }
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
 * If active eepflash section is full a clean up into alternate section is performed
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
  if ((cur_pos + sizeof(block_header_t) + data_len) > flash_sections.active_end){
    /* ... perform a cleanup and seek to the end */
    blocks_cleanup((char **)&cur_pos, block_id);
  }
  /* if user wants to write still more data than possible this will crash */
  if ((cur_pos + sizeof(block_header_t) + data_len) > flash_sections.active_end) {
    /* abort write or cause error handling ARM exception */
  }
  else {
    /* write header of new block                           */
    hw_write((char *)&block_header, cur_pos, sizeof(block_header_t));
    /* write data field of new block                       */
    hw_write(src, cur_pos + sizeof(block_header_t), user_len);
  }
}


/** @} */
