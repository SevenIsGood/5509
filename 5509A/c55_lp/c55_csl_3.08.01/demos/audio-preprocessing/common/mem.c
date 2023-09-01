/*
 *        Copyright (c) 2007 – 2013 Texas Instruments Incorporated                
 *                                                                                
 *              All rights reserved not granted herein.                           
 *                                                                                
 *                         Limited License.                                       
 *                                                                                
 *  Texas Instruments Incorporated grants a world-wide, royalty-free,             
 *  non-exclusive license under copyrights and patents it now or hereafter owns   
 *  or controls to make, have made, use, import, offer to sell and sell           
 *  ("Utilize") this software subject to the terms herein.  With respect to the   
 *  foregoing patent license, such license is granted solely to the extent that   
 *  any such patent is necessary to Utilize the software alone.  The patent       
 *  license shall not apply to any combinations which include this software,      
 *  other than combinations with devices manufactured by or for TI (“TI           
 *  Devices”).  No hardware patent is licensed hereunder.                         
 *                                                                                
 *  Redistributions must preserve existing copyright notices and reproduce this   
 *  license (including the above copyright notice and the disclaimer and (if      
 *  applicable) source code license limitations below) in the documentation       
 *  and/or other materials provided with the distribution                         
 *                                                                                
 *  Redistribution and use in binary form, without modification, are permitted    
 *  provided that the following conditions are met:                               
 *                                                                                
 *    *  No reverse engineering, decompilation, or disassembly of this software   
 *  is permitted with respect to any software provided in binary form.            
 *                                                                                
 *    *  any redistribution and use are licensed by TI for use only with TI       
 *  Devices.                                                                      
 *                                                                                
 *    *  Nothing shall obligate TI to provide you with source code for the        
 *  software licensed and provided to you in object code.                         
 *                                                                                
 *  If software source code is provided to you, modification and redistribution   
 *  of the source code are permitted provided that the following conditions are   
 *  met:                                                                          
 *                                                                                
 *    *  any redistribution and use of the source code, including any resulting   
 *  derivative works, are licensed by TI for use only with TI Devices.            
 *                                                                                
 *    *  any redistribution and use of any object code compiled from the source   
 *  code and any resulting derivative works, are licensed by TI for use only      
 *  with TI Devices.                                                              
 *                                                                                
 *  Neither the name of Texas Instruments Incorporated nor the names of its       
 *  suppliers may be used to endorse or promote products derived from this        
 *  software without specific prior written permission.                           
 *                                                                                
 *  DISCLAIMER.                                                                   
 *                                                                                
 *  THIS SOFTWARE IS PROVIDED BY TI AND TI’S LICENSORS "AS IS" AND ANY EXPRESS    
 *  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED             
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE        
 *  DISCLAIMED. IN NO EVENT SHALL TI AND TI’S LICENSORS BE LIABLE FOR ANY         
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR            
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER    
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT            
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY     
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   
 *  DAMAGE.                                                                       
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xdc/std.h>

#include <ti/mas/types/types.h>
#include <ti/mas/util/ecomem.h>
#include <ti/xdais/ialg.h>
#include <ti/mas/vau/test/src/mem.h>

#define align(Addr, Algnmt) (((Addr)+(Algnmt)-1)&(0xFFFFFFFFUL-(Algnmt)+1))

typedef struct {
  tword *heapBase;
  tulong indx;
  tulong size;
} siuMemHeap_t;

void siu_mem_heap_init(siuMemHeap_t *ptr, tword *heapBase, tulong size)
{
  ptr->heapBase = heapBase;
  ptr->size = size;
  ptr->indx = 0;
  
  /* Note : memory is not initialized deliberately to expose 
            uninitialized memory read problems */
}

tword *siu_mem_heap_alloc (siuMemHeap_t *ptr, tint size)
{
  tword *alloc = NULL;
   
  if ((ptr->indx + size) <= ptr->size) {
    alloc = &ptr->heapBase[ptr->indx];
    ptr->indx += size;
  }
  return(alloc);
}

#define SIU_MEM_VOLATILE_HEAP_SIZE 1000
#define SIU_MEM_NONVOLATILE_HEAP_SIZE 1000

#pragma DATA_SECTION (siuVolatileMemBufs, ".volatileMemBufs");
tword siuVolatileMemBufs[SIU_MEM_VOLATILE_HEAP_SIZE];

#pragma DATA_SECTION (siuNonVolatileMemBufs, ".nonVolatileMemBufs");
tword siuNonVolatileMemBufs[SIU_MEM_NONVOLATILE_HEAP_SIZE];

siuMemHeap_t siuMemHeapVolatile;
siuMemHeap_t siuMemHeapNonVolatile;

void siu_init_heaps(void)
{
  siu_mem_heap_init ( &siuMemHeapVolatile, siuVolatileMemBufs, SIU_MEM_VOLATILE_HEAP_SIZE);
  siu_mem_heap_init ( &siuMemHeapNonVolatile, siuNonVolatileMemBufs, SIU_MEM_NONVOLATILE_HEAP_SIZE);
}

tword *siu_malloc (tint size, tbool volat)
{
  if (volat == TRUE) /* Scratchpad type */ {
    return ( siu_mem_heap_alloc (&siuMemHeapVolatile, size) );
  }
  else {
    return ( siu_mem_heap_alloc (&siuMemHeapNonVolatile, size) );
  }
}

static inline void print_header (void)
{
  printf("+-----------+-----------+-----------+-------------+\n");
  printf("| Buffer ID | Alignment |   Size    | Volatility  |\n");
  printf("|           |  (Bytes)  | (Bytes)   |             |\n");
  printf("+-----------+-----------+-----------+-------------+\n");
/*printf("| %9d | %9d | %9d | %11s |\n");*/
}

static inline void print_buffer (tint ID, tint linAlign, tint sizeMAUs, tbool isVolat)
{
  printf("| %9d | %9d | %9d | %11s |\n", ID, linAlign * (TYP_TWORD_SIZE/8),
        sizeMAUs * (TYP_TWORD_SIZE/8), (isVolat == TRUE)? "scratch" : "non-scratch" );
}

static inline void print_footer (void)
{
  printf("+-----------+-----------+-----------+-------------+\n");
}

void siu_alloc_eco_mem(tint nbufs, const ecomemBuffer_t * sizeBufsPtr,
                   ecomemBuffer_t * ecoBufsPtr)
{
  tint      i;
  void     *allocPtr;
  tulong     linAlign, log2align;
  tbool     isVolat;

  print_header();
  
  for (i = 0; i < nbufs; i++) {
    ecoBufsPtr[i] = sizeBufsPtr[i];

    isVolat = ecoBufsPtr[i].volat;

    log2align = sizeBufsPtr[i].log2align;
    linAlign = (tuint) 1 << log2align;

    print_buffer (i, linAlign, sizeBufsPtr[i].size, isVolat);
           
    if (sizeBufsPtr[i].size == 0) {
      ecoBufsPtr[i].base = NULL;
      continue;
    }

    if ((allocPtr = (void *) siu_malloc((sizeBufsPtr[i].size + linAlign - 1), isVolat)) == NULL) {
      printf("Not enough heap, exiting\n");
      exit(0);
    }
    ecoBufsPtr[i].base = (void *) align((tulong) allocPtr, linAlign);
  }
  
  print_footer();
}

void siu_alloc_xdais_mem(tint nbufs, IALG_MemRec *xdaisBufsPtr, tulong *mallocedSizeCounter)
{
  tint      i;
  void     *allocPtr;
  tulong     linAlign;
  tulong      alignmentAdjustedSize;
  tbool    isVolat;

  print_header();
  
  for (i = 0; i < nbufs; i++) {

    isVolat = (xdaisBufsPtr[i].attrs == IALG_SCRATCH) ? TRUE : FALSE;
    linAlign = (tint) xdaisBufsPtr[i].alignment;
    
    print_buffer (i, linAlign, xdaisBufsPtr[i].size, isVolat);

    if (xdaisBufsPtr[i].size == 0) {
      xdaisBufsPtr[i].base = NULL;
      continue;
    }

    alignmentAdjustedSize = xdaisBufsPtr[i].size + linAlign - 1;
    if ((allocPtr = (void *) siu_malloc(alignmentAdjustedSize, isVolat)
        ) == NULL) {
      printf("Not enough heap, exiting\n");
      exit(0);
    }
    *mallocedSizeCounter += alignmentAdjustedSize;
    xdaisBufsPtr[i].base = (void *) align((tulong) allocPtr, linAlign);
  }
  
  print_footer();  
}

void siu_dealloc_xdais_mem (tint nbufs, IALG_MemRec *xdaisBufsPtr, tulong *mallocedSizeCounter)
{
  tint i;
  
  for(i = 0; i < nbufs; i++) {
    *mallocedSizeCounter -= (xdaisBufsPtr[i].size + xdaisBufsPtr[i].alignment - 1);
    if (xdaisBufsPtr[i].base) {
    
    /* Note the free below may not have the right pointers as were in malloc because
       the module was fed aligned pointers, it is not possible to reverse and derive
       original unaligned pointer, so don't do free */
      //free(xdaisBufsPtr[i].base);
    }  
  }
}

/* Nothing past this point */
