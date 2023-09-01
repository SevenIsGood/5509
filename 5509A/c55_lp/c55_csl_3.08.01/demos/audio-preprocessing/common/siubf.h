#ifndef _SIUBF_H
#define _SIUBF_H

/****************************************************************************** 
 * FILE PURPOSE: SIU functions to support simulation testing of BF
 ****************************************************************************** 
 * FILE NAME:   siubf.h
 * 
 * DESCRIPTION: Contains constants and arrays for simulation testing of BF
 *
 *        Copyright (c) 2007 – 2013 Texas Instruments Incorporated                
 *                     ALL RIGHTS RESERVED
 *
 *****************************************************************************/


#include <ti/mas/types/types.h>   
#include <ti/mas/util/ecomem.h>                   
#include <ti/mas/aer/test/siusim/siuloc.h>  

typedef struct {
  tword *base;
  tulong indx;
  tulong size;
} bfMemHeap_t;

typedef struct {
  tbool pre_init_bufs; 
  tword init_pattern;     
  tuint mclass;        
} bfMemConfig_t;

typedef enum {
  bf_MEM_INT_HEAP = ecomem_CLASS_INTERNAL,
  bf_MEM_EXT_HEAP = ecomem_CLASS_EXTERNAL
} bfMem_e;

#define SIU_BF_MAX_NUM_MICS           4
#define SIU_BF_FLT_LEN               24

void *bf_mem_align       (void *addr, tuint lin_align);
void  bf_mem_init_heap   (void);
void *bf_mem_alloc_align (tint size, tint mclass, tint linAlign);
         
void siu_new_bf(siuInst_t *inst, tint samp_rates);                   
void siu_open_bf(siuInst_t *inst, tint samp_rates, tint num_mic);
void siu_config_bf (siuInst_t *inst, bfControl_t *ctl, Fract coeffs[][SIU_BF_FLT_LEN], 
                    tint sampling_rates, tint num_mics);
void siu_close_bf(siuInst_t *inst);
void siu_delete_bf(siuInst_t *inst);
            
#endif /* _SIUBF_H */

/* nothing past this point */
