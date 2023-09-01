/******************************************************************************
 * FILE PURPOSE: Multiple heap manager.
 ******************************************************************************
 * FILE NAME:   mhm.c
 *
 * DESCRIPTION: Contains routines for multiple heap management. These functions
 *              may be used in allocating memory from several independent
 *              memory pools.
 *
 *              NOTE: Current implementation does NOT provide deallocation!
 *
 * TABS: NONE
 *
 * $Id: mhm.c 1.1 1997/06/25 11:40:55 BOGDANK Exp $
 *
 * REVISION HISTORY:
 *
 * $Log: mhm.c $
 * Revision 1.1  1997/06/25 11:40:55  BOGDANK
 * Initial revision
 *
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdlib.h>
#include <string.h>                                            /* for memset */

#include <xdc/std.h>
/* System definitions and utilities */
#include <ti/mas/types/types.h>               /* DSP types                   */

//#include <ti/mas/mhm/mhm.h>
//#include <ti/mas/mhm/src/mhmport.h>

#include "../mhm.h"
#include "mhmport.h"

/* Heap header that is placed at the beginning of a memory pool. */
typedef struct {
  tuint  total_size;   /* total number of words in a memory pool */
  tuint  used_size;    /* index of the first available word */
} mhmHead_t;

/* The minimum and maximum number of words in a memory pool. Align gap and
 *  maximum size value are specified in mhmport.h. */
#define MHM_MIN_POOLSIZE  (sizeof(mhmHead_t)+MHM_MAX_ALIGNGAP+1)
#define MHM_MAX_POOLSIZE  MHM_MAX_SIZEVAL

/******************************************************************************
 * FUNCTION PURPOSE: Create and initialize a heap.
 ******************************************************************************
 * DESCRIPTION: Creates and initializes a heap. If successful returns a heap
 *              handle, otherwise returns NULL.
 *
 *  void *mhmCreate (
 *    void *base,         - base address of a memory pool (!= NULL)
 *    tuint size,          - number of words within the memory pool
 *    word initPattern)   - initializing pattern for the heap
 *
 *  RETURN VALUE: A heap handle, or NULL in case of error.
 *
 *****************************************************************************/

void *mhmCreate (void *base, tuint size, tword initPattern)
{
  tuint      used_size;
  mhmHead_t *head;
  tword      *w_base = (tword *)base;
  tword      *block;
  mhmAddress_t  wa_base_new, wa_block_new;

  /* initialize the raw space */
  memset (w_base, initPattern, size);
  
  /* In the first part we align the base address of the memory pool */
  used_size = 0;
  wa_base_new = ((mhmAddress_t)w_base + MHM_MAX_ALIGNGAP) & (~MHM_ALIGN_MASK);
  used_size += wa_base_new - (mhmAddress_t)w_base;
  w_base = (tword *)wa_base_new;        /* this is the new base after any gap */

  if (size < used_size+MHM_MIN_POOLSIZE || size-used_size > MHM_MAX_POOLSIZE ||
      base == NULL)
    return(NULL);

  /* Skip the pool alignment gap and start fresh. Do not forget to reset used size. */
  /* w_base points to aligned address now and used_size is currently the gap size (if any) */
  head = (mhmHead_t*)w_base;            /* This will be the actual head of the heap */
  head->total_size = size - used_size;  /* record total heap size by removing the gap size */

  used_size = sizeof(mhmHead_t);    /* reserve space for the header and make that the used size */
  block     = &w_base[used_size];   /* skip the header */

  /* Remember to align the remaining pool to the minimum alignment required by MHM  */
  wa_block_new = ((mhmAddress_t)block + MHM_MAX_ALIGNGAP) & (~MHM_ALIGN_MASK);
  used_size += wa_block_new - (mhmAddress_t)block;

  head->used_size = used_size;    /* record an index to empty space which is with minimum alignment */
  return(head);                   /* return heap handle */
} /* mhmCreate */

/******************************************************************************
 * FUNCTION PURPOSE: Allocate a block of memory from a heap.
 ******************************************************************************
 * DESCRIPTION: Allocates a block of memory from a heap. Block size must be
 *              bigger than zero. It returns a pointer to block start or a
 *              NULL if unsuccessful.
 *
 *  void *mhmAlloc (
 *    void *handle,     - heap handle obtained in mhmCreate()
 *    tuint size)        - number of words to allocate for the block
 *
 *  RETURN VALUE: A pointer to block start, or NULL when unsuccessful.
 *
 *****************************************************************************/

void *mhmAlloc (void *handle, tuint size)
{
  tuint      left;
  mhmHead_t *head = (mhmHead_t*)handle;
  tword      *w_base = (tword *)handle;
  tword      *block;

  if (size == 0 || size > MHM_MAX_SIZE || head->total_size <= head->used_size)
    return(NULL);

  /* Make sure the actual used size will be resulting in minimum required MHM alignment */
  size = (tuint)((size + MHM_MAX_ALIGNGAP) & (~MHM_ALIGN_MASK));

  left = head->total_size - head->used_size;
  if (left < size)                      /* do we have space for this block? */
    return(NULL);
  block = &w_base[head->used_size];
  head->used_size += (tuint)size;       /* increase the used space */

  /* no need for additional alignment since the size was already aligned above and base is
   *  always kept aligned to the minimum requried MHM alignment */

  return(block);                        /* return a pointer to start */
} /* mhmAlloc */

/******************************************************************************
 * FUNCTION PURPOSE: Allocate a block of memory from a heap log2 aligned
 ******************************************************************************
 * DESCRIPTION: Allocates a block of memory from a heap. Block size must be
 *              bigger than zero. It returns a pointer to block start or a
 *              NULL if unsuccessful. The start address will be aligned to pow-2
 *              boundary.
 *
 *  void *mhmAllocAligned (
 *    void *handle,     - heap handle obtained in mhmCreate()
 *    tuint size,       - number of words to allocate for the block
 *    tint  log2align)  - log2 alignment that is required (e.g. 3 for 0xFF..FF8 mask)
 *
 *  RETURN VALUE: A pointer to block start, or NULL when unsuccessful.
 *
 *****************************************************************************/

void *mhmAllocAligned (void *handle, tuint size, tint log2align)
{
  tuint         left, used_size;
  mhmHead_t     *head   = (mhmHead_t*)handle;
  tword         *w_base = (tword *)handle;
  tword         *block;
  tulong        align_mask, align_gap;
  mhmAddress_t  wa_block_new;

  if (size == 0 || size > MHM_MAX_SIZE || head->total_size <= head->used_size)
    return(NULL);

  if (log2align < 1) {                  /* use simple alloc function if no alignment required */
    block = mhmAlloc(handle, size);
    return(block);
  }
  used_size = head->used_size;      /* current used size */
  block     = &w_base[used_size];   /* skip the already used size */

  /* Remember to align the new block address to the requested alignment */
  align_mask = ((~0UL) << log2align);
  align_gap = (~align_mask);

  wa_block_new = (((mhmAddress_t)block + align_gap) & align_mask);
  size += wa_block_new - (mhmAddress_t)block;       /* update size with the gap (if any) */

  /* Make sure the actual used size will be resulting in minimum required MHM alignment */
  /* This in general may be different from the requested start address alignment! */
  size = (tuint)((size + MHM_MAX_ALIGNGAP) & (~MHM_ALIGN_MASK));

  left = head->total_size - used_size;
  if (left < size)                      /* do we have space for this block? (and any gaps) */
    return(NULL);
  block = (tword*)wa_block_new;         /* this is where we would start new block */
  head->used_size = used_size+size;     /* increase the used space */

  /* no need for additional alignment since the size and base were already aligned above */

  return(block);                        /* return a pointer to start */
} /* mhmAllocAligned */

/******************************************************************************
 * FUNCTION PURPOSE: Deallocate a memory block from a heap.
 ******************************************************************************
 * DESCRIPTION: Deallocates a memory block from a heap.
 *
 *  void mhmFree (
 *    void *handle,     - a heap handle
 *    void *base)       - base address of a block
 *
 *****************************************************************************/

void mhmFree (void *handle, void *base)
{
  return;           /* currently NOT supported */
} /* mhmFree */

/******************************************************************************
 * FUNCTION PURPOSE: Reset heap and start allocating form the beginning.
 ******************************************************************************
 * DESCRIPTION: Resets heap and allows for allocation to start from the beginning.
 *
 *  void mhmReset (
 *    void *handle)     - a heap handle
 *
 *****************************************************************************/

void mhmReset (void *handle)
{
  tuint         used_size;
  mhmHead_t     *head = (mhmHead_t*)handle;
  tword         *w_base = (tword*)handle;
  tword         *block;
  mhmAddress_t  wa_block_new;

  /* no need to change the total size */
  /* we start from empty heap */
  used_size = sizeof(mhmHead_t);    /* reserve space for the header and make that the used size */
  block     = &w_base[used_size];   /* skip the header */

  /* Remember to align the remaining pool to the minimum alignment required by MHM  */
  wa_block_new = ((mhmAddress_t)block + MHM_MAX_ALIGNGAP) & (~MHM_ALIGN_MASK);
  used_size += wa_block_new - (mhmAddress_t)block;

  head->used_size = used_size;    /* record an index to empty space which is with minimum alignment */

  /* the head which is the base of the heap is already aligned */
  return;
} /* mhmreset */

/* nothing past this point */

