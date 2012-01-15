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
 * Note: eepflash_write() is nonreentrant!
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

/** uint8_t pattern indicating a block is written in the flash
 *
 */
#define BLOCKHEADER_BLOCK_IS_VALID 0xaa

/** uint16_t address field pointer signalizing that a block is
 *  not yet written (NULL pointer)
 *
 */
#define BLOCKMNGR_BLOCK_IS_VOID ((void*)0)


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
 * The data is written in a linear row into the flash. In front
 * of each datafield a header is written. header and data is referred
 * to as block.
 *
 * Note: Size of header must be even and structure must be packed
 */
typedef struct {
  /* a number identifying the origin of the datafield after the header */
  uint8_t  block_id;
  /* tag to indicate the end of the flash queue */
  uint8_t  validmarker;
  /* size of the user data in the data field. the FEE tools can
     only write an even data size. whenever we read from flash
     we will restore the data field size by rounding up this value */
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
flash_erase(const uint8_t sector_start, const uint8_t sector_end)
{
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
  return true;
}


/** Write character buffer to flash
 *
 * Note: Can only write 16 bit wise - len must be even
 */
static int8_t
flash_write(const char *src,  const char *dst, const uint16_t len)
{
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
  return true;
}


inline static void
blockheader_set_valid (blockheader_t *header)
{
  header->validmarker = BLOCKHEADER_BLOCK_IS_VALID;
}


inline static uint8_t
blockheader_check_valid (const blockheader_t *header)
{
  return(header->validmarker == BLOCKHEADER_BLOCK_IS_VALID);
}


/** Initialize ram pointers which point to newest valid blocks
 *
 * Blocks which are not present in the flash queue will tagged
 * as "unitialized" (NA) in the ram pointer.
 */
inline static
void blockmngr_init(void)
{
  /* mark all blocks as invalid  */
  for (uint8_t block_id = 0;
       block_id < EEPFLASH_NUM_USED_BLOCKS;
       block_id ++){
     blockmngr.block_start[block_id] = BLOCKMNGR_BLOCK_IS_VOID;
  }
  /* from the beginning of the active section */
  char *p_block = flashsections.active_start;
  blockheader_t *header = (blockheader_t *)(p_block);
  /* for all valid blocks (blocks recorded in the flash queue) */
  while (blockheader_check_valid(header)){
    /* the last block in the queue is the most recent block */
    blockmngr.block_start[header->block_id] = p_block;
    /* jump to next header */
    p_block += _ALIGN(header->data_size, 2) +
               sizeof(blockheader_t);
    header = (blockheader_t *)(p_block);
  }
  /* record end position of the flash queue */
  blockmngr.block_queue_end = p_block;
}


/** Initialize pointer to flash sections (working and copy page)
 *
 */
inline static
void flashsections_init(void)
{
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
    /* either flash is virgin or currently section one is in use,
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
void flashsections_swap(void)
{
  /* erase current active section (working section) */
  const uint8_t sector_start =
    MEMSIZE_TO_NUMSECTOR(flashsections.active_start, __flash_start__ );
  const uint8_t sector_end =
    MEMSIZE_TO_NUMSECTOR(flashsections.active_end, __flash_start__) - 1;
  flash_erase(sector_start, sector_end);

  /* swap section pointers active <-> alternate */
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
blockmngr_get_block(char **p_block, const uint8_t block_id)
{
  if (blockmngr.block_start[block_id] == BLOCKMNGR_BLOCK_IS_VOID){
    return false;
  }
  else {
    *p_block = blockmngr.block_start[block_id];
    return true;
  }
}


/** Return pointer to the end of the record queue
 *
 */
inline static void
blockmngr_end_of_queue(char **p_queue_end)
{
  *p_queue_end = blockmngr.block_queue_end;
}


/** Perform a cleanup from active into alternate flash section
 *
 * Expunge block with block_id == expunge_id
 */
inline static void
flashsections_cleanup(const uint8_t expunge_id)
{
  char *p_src;
  char *p_dst = flashsections.alternate_start;
  for (uint8_t block_id = 0;
       block_id < EEPFLASH_NUM_USED_BLOCKS;
       block_id++){
    /* get pointer to the newest block referenced by block_id */
    if (blockmngr_get_block((char **)&p_src, block_id) &&
       (block_id != expunge_id)){
      /* extract header */
      blockheader_t *header = (blockheader_t *)(p_src);
      /* length of data field written (must be even; round up) */
      const uint16_t len = _ALIGN(header->data_size, 2) +
                           sizeof(blockheader_t);
      /* copy block to alternate section */
      flash_write(p_src, p_dst, len);
      /* adjust to the end of the record queue */
      p_dst += len;
    }
    /* else: no block found -> nothing */
  }
}

/*-----------------------------------------------------------------------------
 * Memory service functions (service layer)
 *-----------------------------------------------------------------------------
 */

/** Copy data from newest valid block with BLOCKID
 *
 * Returns false if no valid block was found, true elsewise
 */
int8_t
eepflash_copy_block(char *p_ram, const uint8_t block_id)
{
  char *p_flash;
  /* if block is registered in the ram pointer */
  if (blockmngr_get_block((char **)&p_flash, block_id)){
    blockheader_t *header = (blockheader_t *)(p_flash);
    /* point to start and end of the data field (flash) */
    char *p_data = p_flash + sizeof(blockheader_t);
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
 * If there is not enough memory in the active flash section a cleanup
 * into alternate section is performed and after that active and
 * alternate sections are swapped
 */
void
eepflash_write(const char *p_ram,
               const uint16_t user_len,
               const uint8_t block_id)
{
  char *p_block;
  /* seek to the end of the record queue */
  blockmngr_end_of_queue((char **)&p_block);
  /* create a new blockheader */
  blockheader_t blockheader;
  blockheader.block_id = block_id;
  blockheader.data_size = user_len;
  blockheader_set_valid(&blockheader);
  /* FEE: the data field length recorded must be even (round up) */
  const uint16_t data_len = _ALIGN(user_len, 2);
  /* if running out of space */
  if ((p_block + sizeof(blockheader_t) + data_len) >
       flashsections.active_end) {
    /* perform a cleanup */
    flashsections_cleanup(block_id);
    /* swap working <-> alternate section */
    flashsections_swap();
    /* update block pointers */
    blockmngr_init();
    /* seek to the new end of the record queue */
    blockmngr_end_of_queue((char **)&p_block);
  }
  /* if user wants to write still more data than possible */
  if ((p_block + sizeof(blockheader_t) + data_len) >
       flashsections.active_end) {
   /* \todo: Cause ARM exception (e.g. abort) and halt target
             in exception trap */
  }
  else {
    /* write block (header and data field) */
    flash_write((char *)&blockheader, p_block, sizeof(blockheader_t));
    flash_write(p_ram, p_block + sizeof(blockheader_t), user_len);
    /* register new block in ram pointer */
    blockmngr.block_start[block_id] = p_block;
    blockmngr.block_queue_end = p_block +
                                sizeof(blockheader_t) +
                                data_len;
  }
}


static
void __init eepflash_init(void)
{
  /* initialize pointers to active and alternate flash sections */
  flashsections_init();
  /* initialize ram pointers pointing to newest valid blocks */
  blockmngr_init();
}

module_init(eepflash_init, 0);



/** @} */
