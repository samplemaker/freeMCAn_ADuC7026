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
 * Note: Eepflash is nonreentrant!
 * Note: CRC checks are not supported!
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

/** Macros for sector calculations
 *
 */
#define FLASHADDR_TO_SECTORNO(addr, start) \
                             (((addr) - (start)) >> (FLASH_SECTOR_CONFIG))

/** uint8_t pattern for marking a block as written (valid)
 *
 */
#define BLOCKHEADER_VALID_MARKER 0xaa

/** uint16_t pointer value signalizing that blocks are not yet written
 *  (NULL pointer)
 *
 */
#define RAMPTR_BLOCK_VOID (char *)(0xffff)


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
typedef struct {
  char *alternate_start;
  char *alternate_end;
  char *active_start;
  char *active_end;
} flssections_t;

static flssections_t flssections;


/** \brief eepflash block header
 *
 * Size of header must be even and structure must be packed
 */
typedef struct {
  uint8_t  block_id;
  uint8_t  validmarker;
  uint16_t data_size;
} __attribute__((__packed__)) blockheader_t;


/** \brief Holds address of most recent valid blocks
 *
 * Curblock_start==0xFFFF indicates a block is NA
 * assuming that we will never have a block located
 * above 64k ;-). However this saves 2*(EEPFLASH_NUM_USED_BLOCKS-1)
 * bytes RAM.
 */
typedef struct ramptr_struct {
  /* address to start of most recent block with block_id */
  char *curblock_start[EEPFLASH_NUM_USED_BLOCKS];
  /* bytes already written or flash space already occupied */
  char *queue_end;
} ramptr_t;

static ramptr_t ramptr;


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
 * Can only write 16 bit wise
 *
 * len: length [bytes] to write. must be even and integer
 * dst: logic adress to write
 * src: pointer to character buffer
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
block_set_valid (blockheader_t *header)
{
  header->validmarker = BLOCKHEADER_VALID_MARKER;
}


/** Checks if a block header is tagged valid
 *
 */
inline static uint8_t
block_check_valid (const blockheader_t *header)
{
  return(header->validmarker == BLOCKHEADER_VALID_MARKER);
}


/** Set all most recent blocks to NA
 *
 */
inline static
void ramptr_reset_blocks(void){
 /* mark all blocks as invalid (set block address to 0xffff) */
  for (uint8_t i = 0; i < EEPFLASH_NUM_USED_BLOCKS; i ++){
     ramptr.curblock_start[i] = RAMPTR_BLOCK_VOID;
  }
}


/** Initialize ram pointer which points to most recent blocks
 *
 */
inline static
void ramptr_set_blocks(void){
  /* from beginning of the active eepflash section */
  char *p_blk = flssections.active_start;
  /* get first header */
  blockheader_t *header = (blockheader_t *)(p_blk);
  /* for all valid headers */
  while (block_check_valid(header)){
    /* last found is most recent */
    ramptr.curblock_start[header->block_id] = p_blk;
    /* jump to next header. since FEE writes only an even block
       length but the user may have specified an odd length
       we must align to get the true length */
    p_blk += _ALIGN(header->data_size, 2) +
               sizeof(blockheader_t);
    /* get next header */
    header = (blockheader_t *)(p_blk);
  }
  ramptr.queue_end = p_blk;
}


/** Initialize pointer to flash sections
 *
 */
inline static
void flssections_init(void){
  /* check if there are valid markers inside section 2 */
  blockheader_t *header = (blockheader_t *)(_eepflash_section2_start);

  if (block_check_valid(header)){
    /* yes: mark section two as active and section one as alternate */
    flssections.active_start = _eepflash_section2_start;
    flssections.active_end = _eepflash_section2_end;
    flssections.alternate_start = _eepflash_section1_start;
    flssections.alternate_end = _eepflash_section1_end;
  }
  else {
    /* no: maybe flash is empty at all or currently section one is in use,
     * then mark section one as active and section two as alternate
     */
    flssections.active_start = _eepflash_section1_start;
    flssections.active_end = _eepflash_section1_end;
    flssections.alternate_start = _eepflash_section2_start;
    flssections.alternate_end = _eepflash_section2_end;
  };
}


/** Swap alternate and active flash sections
 *
 */
inline static
void flssections_swap(void){
  /* erase current active section (working section) */
  const uint8_t sector_start =
    FLASHADDR_TO_SECTORNO(flssections.active_start, __flash_start__ );
  const uint8_t sector_end =
    FLASHADDR_TO_SECTORNO(flssections.active_end, __flash_start__) - 1;
  flash_erase(sector_start, sector_end);

  /* exchange section pointer address active <-> alternate */
  char *temp;
  temp = flssections.active_start;
  flssections.active_start = flssections.alternate_start;
  flssections.alternate_start = temp;

  temp = flssections.active_end;
  flssections.active_end = flssections.alternate_end;
  flssections.alternate_end = temp;
}


/** Returns address of most recent valid block with respect to BLOCKID
 *
 * Returns false if no valid block was found, true elsewise.
 * Implemented as linear search
 * Note: Returnpointer points to the correct start of block
 * until a new eepflash_write is forced.
 */
inline static int8_t
ramptr_get_block(char **block, const uint8_t block_id){
  if (ramptr.curblock_start[block_id] == RAMPTR_BLOCK_VOID){
    return false;
  }
  else {
    *block = ramptr.curblock_start[block_id];
    return true;
  }
}


/** Perform a cleanup into alternate flash section
 *
 * Expunge block with block_id
 */
inline static void
flssections_cleanup(char **end, const uint8_t expunge_id){
  /* copy most recent and valid block from active section to alternate section */
  char *p_src;
  char *p_dst = flssections.alternate_start;
  /* for each block ID */
  for (uint8_t block_id = 0; block_id < EEPFLASH_NUM_USED_BLOCKS; block_id++){
    if (ramptr_get_block((char **)&p_src, block_id) && (block_id != expunge_id)){
      /* get header */
      blockheader_t *header = (blockheader_t *)(p_src);
       /* since FEE provides only even block length but the user may
          have specified an odd length (and this is stored in flash)
          we must align to get the true length           */
      const uint16_t block_len = _ALIGN(header->data_size, 2) +
                                 sizeof(blockheader_t);
      /* copy most recent block to alternate section */
      flash_write(p_src, p_dst, block_len);
      p_dst += block_len;
    }
    /* else: no block found -> therefore nothing to do */
  }
  /* return pointer to vacant area in active flash section */
  *end = p_dst;
}

/*-----------------------------------------------------------------------------
 * Memory service functions (service layer)
 *-----------------------------------------------------------------------------
 */

/** Returns most recent valid block-data with respect to BLOCKID
 *
 *  Returns false if no valid block was found, true elsewise
 */
int8_t
eepflash_copy_block(char *p_ram, const uint8_t block_id){
  char *p_flash;

  if (ramptr_get_block((char **)&p_flash, block_id)){
    /* get header */
    blockheader_t *header = (blockheader_t *)(p_flash);
    /* point to data stream start (flash) */
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
 * If there is not enough free memory space in the current flash section
 * a cleanup is performed and the flash sections are exchanged
 */
void
eepflash_write(const char *src, const uint16_t user_len, const uint8_t block_id){
  /* seek to the end of eepflash */
  char *p_blk = ramptr.queue_end;
  /* create a new blockheader with block_id and virtual
     user length of data (length may be odd) */
  blockheader_t blockheader;
  blockheader.block_id = block_id;
  blockheader.data_size = user_len;
  block_set_valid(&blockheader);
  /* the fee - flash utilities store only even data length (round up) */
  const uint16_t data_len = _ALIGN(user_len, 2);
  /* if we are running out of space ... */
  if ((p_blk + sizeof(blockheader_t) + data_len) > flssections.active_end){
    /* ... perform a cleanup and seek to the end */
    flssections_cleanup((char **)&p_blk, block_id);
    /* swap working / alternate section   */
    flssections_swap();
    /* mark all blocks as invalid (reset ram pointer table) */
    ramptr_reset_blocks();
    /* initialize ram pointer. points now to most recent data */
    ramptr_set_blocks();
  }
  /* if user wants to write still more data than possible this will crash */
  if ((p_blk + sizeof(blockheader_t) + data_len) > flssections.active_end) {
    /* \todo: cause error handling ARM exception and halt target */
  }
  else {
    /* write header of new block */
    flash_write((char *)&blockheader, p_blk, sizeof(blockheader_t));
    /* write data field of new block */
    flash_write(src, p_blk + sizeof(blockheader_t), user_len);
    /* update ram table pointing to most recent blocks */
    ramptr.curblock_start[block_id] = p_blk;
    ramptr.queue_end = p_blk + sizeof(blockheader_t) + data_len;
  }
}


static
void __init eepflash_init(void){
  /* initialize pointer to active and alternate flash sections */
  flssections_init();
  /* mark all blocks as invalid (reset ram pointer table) */
  ramptr_reset_blocks();
  /* initialize ram pointer. points now to most recent data */
  ramptr_set_blocks();
}

module_init(eepflash_init, 0);



/** @} */
