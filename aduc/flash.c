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
 * Implements a driver for storing non volatile data into flash
 * Note: Eepflash-write is nonreentrant!
 * Note: CRC checks and error handling is not supported!
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

/** Mem size to number of flash sector(s) conversion
 *
 */
#define MEMSIZE_TO_NUMSECTOR(end, start) \
                           (((end) - (start)) >> (FLASH_SECTOR_CONFIG))

/** uint8_t pattern for marking a block as written in the flash
 *
 */
#define BLOCKHEADER_BLOCK_IS_VALID 0xaa

/** uint16_t address field pointer signalizing that a block is
 *  not yet written (NULL pointer)
 *
 */
#define BLOCKMNGR_BLOCK_IS_VOID (char *)(0xffff)


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


/** \brief Pointers to active and alternate flash sections
 *
 */
typedef struct {
  char *alternate_start;
  char *alternate_end;
  char *active_start;
  char *active_end;
} flashsections_t;

static flashsections_t flashsections;


/** \brief eepflash block header
 *
 * Size of header must be even and structure must be packed
 */
typedef struct {
  uint8_t  block_id;
  uint8_t  validmarker;
  uint16_t data_size;
} __attribute__((__packed__)) blockheader_t;


/** \brief Holds address of most recent blocks in the flash queue
 *
 * block_start == 0xFFFF indicates a block is NA (not yet
 * written) assuming that we will never have a block located
 * above 64k. However this saves EEPFLASH_NUM_USED_BLOCKS
 * bytes RAM (int8 indicator wheather a block is already written)
 */
typedef struct blockmngr_struct {
  /* pointer to start of most recent block with block_id */
  char *block_start[EEPFLASH_NUM_USED_BLOCKS];
  /* points to end of data queue already written into flash */
  char *block_queue_end;
} blockmngr_t;

static blockmngr_t blockmngr;


/*-----------------------------------------------------------------------------
 * Microcontroler abstraction layer functions
 *-----------------------------------------------------------------------------
 */

/** Erases flash sector(s)
 *
 */
static int8_t
flash_erase(const uint8_t sector_start, const uint8_t sector_end){
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
 * len: length [bytes] to write. must be even and integer
 * dst: logic adress to write
 * src: pointer to character source buffer
 *
 * Note: Can only write 16 bit wise
 */
static int8_t
flash_write(const char *src,  const char *dst, const uint16_t len){
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
              * poll until data is written */
         break;
      }
    }
  }
  /* data string written successfully: exit */
  return true;
}


/** Set the block header validmarker to valid
 *
 */
inline static void
blockheader_set_valid (blockheader_t *header)
{
  header->validmarker = BLOCKHEADER_BLOCK_IS_VALID;
}


/** Checks if a block header is tagged valid
 *
 */
inline static uint8_t
blockheader_check_valid (const blockheader_t *header)
{
  return(header->validmarker == BLOCKHEADER_BLOCK_IS_VALID);
}


/** Set all recent blocks to NA in the ram pointer
 *
 * NA means that a block with block_id is not yet written
 * in the flash queue
 */
inline static
void blockmngr_reset_blocks(void){
 /* mark all blocks as invalid (set block address to
    BLOCKMNGR_BLOCK_IS_VOID) */
  for (uint8_t block_id = 0; block_id < EEPFLASH_NUM_USED_BLOCKS; block_id ++){
     blockmngr.block_start[block_id] = BLOCKMNGR_BLOCK_IS_VOID;
  }
}


/** Initialize ram pointer which points to most recent blocks
 *
 * Blocks which are not present in the flash queue are not updated
 * in the ram pointer. That means these will reside as "unitialized"
 * (NA) in the ram pointer.
 *
 * Note: The initialization is implemented as linear search
 */
inline static
void blockmngr_update_blocks(void){
  /* from beginning of the active eepflash section */
  char *p_block = flashsections.active_start;
  /* extract first header */
  blockheader_t *header = (blockheader_t *)(p_block);
  /* for all valid blocks */
  while (blockheader_check_valid(header)){
    /* last found is most recent */
    blockmngr.block_start[header->block_id] = p_block;
    /* jump to next header. since FEE writes only an even block
       length but the user may have specified an odd length
       we must align to get the written data field length */
    p_block += _ALIGN(header->data_size, 2) +
             sizeof(blockheader_t);
    /* get next header */
    header = (blockheader_t *)(p_block);
  }
  blockmngr.block_queue_end = p_block;
}


/** Initialize pointer to flash sections (working and copy page)
 *
 * Note: No check wheather the flash is corrupt
 */
inline static
void flashsections_init(void){
  /* check if there is a valid blockheader at start of
     section two */
  blockheader_t *header = (blockheader_t *)(_eepflash_section2_start);
  if (blockheader_check_valid(header)){
    /* yes: mark section two as active and section one as alternate */
    flashsections.active_start = _eepflash_section2_start;
    flashsections.active_end = _eepflash_section2_end;
    flashsections.alternate_start = _eepflash_section1_start;
    flashsections.alternate_end = _eepflash_section1_end;
  }
  else {
    /* no: maybe flash is virgin or currently section one is in use,
     * then mark section one as active and section two as alternate
     */
    flashsections.active_start = _eepflash_section1_start;
    flashsections.active_end = _eepflash_section1_end;
    flashsections.alternate_start = _eepflash_section2_start;
    flashsections.alternate_end = _eepflash_section2_end;
  };
}


/** Swap alternate and active flash sections
 *
 * The active flash area becomes alternate and the alternate
 * flash area becomes active
 */
inline static
void flashsections_swap(void){
  /* erase current active section (working section) */
  const uint8_t sector_start =
    MEMSIZE_TO_NUMSECTOR(flashsections.active_start, __flash_start__ );
  const uint8_t sector_end =
    MEMSIZE_TO_NUMSECTOR(flashsections.active_end, __flash_start__) - 1;
  flash_erase(sector_start, sector_end);

  /* swap section pointers address for active <-> alternate */
  char *temp;
  temp = flashsections.active_start;
  flashsections.active_start = flashsections.alternate_start;
  flashsections.alternate_start = temp;

  temp = flashsections.active_end;
  flashsections.active_end = flashsections.alternate_end;
  flashsections.alternate_end = temp;
}


/** Return pointer to recent and valid block with BLOCKID
 *
 * Returns false if no valid block was found (block is NA),
 * true elsewise
 */
inline static int8_t
blockmngr_get_block(char **p_block, const uint8_t block_id){
  if (blockmngr.block_start[block_id] == BLOCKMNGR_BLOCK_IS_VOID){
    return false;
  }
  else {
    *p_block = blockmngr.block_start[block_id];
    return true;
  }
}


/** Perform a cleanup into alternate flash section
 *
 * Expunge block with block_id == expunge_id
 */
inline static void
flashsections_cleanup(char **p_queue_end, const uint8_t expunge_id){
  /* copy all recent and valid blocks from active section to
     alternate section */
  char *p_src;
  char *p_dst = flashsections.alternate_start;
  /* for each block with block_id */
  for (uint8_t block_id = 0; block_id < EEPFLASH_NUM_USED_BLOCKS; block_id++){
    /* get most recent and valid block */
    if (blockmngr_get_block((char **)&p_src, block_id) && (block_id != expunge_id)){
      /* extract header */
      blockheader_t *header = (blockheader_t *)(p_src);
       /* since FEE provides only even block length but the user may
          have specified an odd length (and this is stored in flash)
          we must align to get the true length           */
      const uint16_t block_len = _ALIGN(header->data_size, 2) +
                                 sizeof(blockheader_t);
      /* copy block to alternate section */
      flash_write(p_src, p_dst, block_len);
      /* adjust to end of queue in the alternate section */
      p_dst += block_len;
    }
    /* else: no block found -> nothing to do */
  }
  /* return pointer to next vacant area in the active flash section */
  *p_queue_end = p_dst;
}

/*-----------------------------------------------------------------------------
 * Memory service functions (service layer)
 *-----------------------------------------------------------------------------
 */

/** Copy data from most recent and valid block with BLOCKID
 *
 * Returns false if no valid block was found, true elsewise
 */
int8_t
eepflash_copy_block(char *p_ram, const uint8_t block_id){
  char *p_flash;
  /* if the block is registered in the ram pointer */
  if (blockmngr_get_block((char **)&p_flash, block_id)){
    /* extract header */
    blockheader_t *header = (blockheader_t *)(p_flash);
    /* point to start of data stream field (flash) */
    char *p_data = p_flash + sizeof(blockheader_t);
    /* point to the end of user-data (might be smaller than the written data) */
    const char *p_end = (char *) (p_data + header->data_size);
    /* point to data destiny start (ram) */
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


/** Store a block with BLOCKID
 *
 * If there is not enough free memory in the active flash section
 * a cleanup into alternate section is performed and after that
 * active and alternate sections will be swapped
 */
void
eepflash_write(const char *p_ram, const uint16_t user_len, const uint8_t block_id){
  /* seek to the end of the flash queue */
  char *p_block = blockmngr.block_queue_end;
  /* create a new blockheader with block_id and data length as
     specified by user (length may be even or odd) */
  blockheader_t blockheader;
  blockheader.block_id = block_id;
  blockheader.data_size = user_len;
  blockheader_set_valid(&blockheader);
  /* the FEE - flash utilities store only even data length
     (round up) */
  const uint16_t data_len = _ALIGN(user_len, 2);
  /* if we are running out of space ... */
  if ((p_block + sizeof(blockheader_t) + data_len) > flashsections.active_end){
    /* ... perform a cleanup and seek to the end */
    flashsections_cleanup((char **)&p_block, block_id);
    /* swap working <-> alternate section */
    flashsections_swap();
    /* mark all blocks as invalid (reset ram pointer table) */
    blockmngr_reset_blocks();
    /* initialize ram pointer. points now to most recent data */
    blockmngr_update_blocks();
  }
  /* if user wants to write still more data than possible */
  if ((p_block + sizeof(blockheader_t) + data_len) > flashsections.active_end) {

   /* \todo: Cause ARM exception (e.g. abort) and halt target
             in exception trap */

  }
  else {
    /* write header of new block */
    flash_write((char *)&blockheader, p_block, sizeof(blockheader_t));
    /* write data field of new block */
    flash_write(p_ram, p_block + sizeof(blockheader_t), user_len);
    /* update ram table pointing to most recent blocks */
    blockmngr.block_start[block_id] = p_block;
    blockmngr.block_queue_end = p_block + sizeof(blockheader_t) + data_len;
  }
}


static
void __init eepflash_init(void){
  /* initialize pointer to active and alternate flash sections */
  flashsections_init();
  /* mark all blocks as invalid (reset ram pointer table) */
  blockmngr_reset_blocks();
  /* initialize ram pointer. points now to most recent data */
  blockmngr_update_blocks();
}

module_init(eepflash_init, 0);



/** @} */
