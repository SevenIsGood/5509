/**
 *  @file   mss.c
 *  @brief  Contains external API for Multi-Source Selection (MSS) module 
 *          instantiation.
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

#include <xdc/std.h>
#include <ti/mas/types/types.h>
#include <ti/mas/fract/fract.h>
#include <ti/mas/vpe/svd.h>

//#include <ti/mas/aer/mss.h>
//#include <ti/mas/aer/src/mssloc.h>
#include "../mss.h"
#include "mssloc.h"

#define MSS_DEF_SWITCH_WAIT_TIME  250     /* msec */
#define MSS_DEF_SWITCH_DURATION   500     /* msec */
#define MSS_DEF_SWITCH_HANGOVER   1000    /* msec */
#define MSS_DEF_FADE_PHA_INC_8KHZ 268435L  /* Q31 phase increment of crossfade for 
                                              switch duration of 1 second at 8kHz:
                                              2^31/8000, where 2^31 corresponds to pi. */
/* MSS memory buffer descriptors */
#define MSS_NUM_BUFS 1
#define MSS_INSTANCE_BUF 0
ecomemBuffer_t mssBufs[MSS_NUM_BUFS] = {                             
  /* CLASS, LOG2 ALIGNMENT, BUFFER SIZE, VOLATILITY, BASE */
  ecomem_CLASS_EXTERNAL, TYP_STRUCT_LOG2ALIGN, 0, FALSE, NULL
};

/******************************************************************************
 * FUNCTION PURPOSE: Return buffer sizes for the MSS module
 ******************************************************************************
 * DESCRIPTION: Calculate the worst case buffer sizes for the MSS. Also specify
 *              alignment and volatility requirements.
 *
 *  tint mssGetSizes (
 *    tint              *nbufs,    - address to store number of buffers
 *    const ecomemBuffer_t **bufs, - address to store buffer descriptors
 *    mssSizeConfig_t   *cfg)      - information to be used in calculating the
 *                                   worst case buffer sizes
 *****************************************************************************/
tint mssGetSizes(tint *nbufs, const ecomemBuffer_t **bufs, mssSizeConfig_t *cfg)
{
  tint svd_size, mss_size, mss_inst_size, num_src;

  /* Get memory requirement of SVD */
  svdGetSizes (&svd_size);
  
  /* Calculate total memory of MSS: instance + all SVDs*/
  svd_size = mss_SIZE_ALIGN(svd_size,TYP_STRUCT_LOG2ALIGN);
  mss_inst_size = sizeof(mssInst_t);
  mss_inst_size = mss_SIZE_ALIGN(mss_inst_size,TYP_STRUCT_LOG2ALIGN);
  num_src =  cfg->max_num_mic_fixed + cfg->max_num_mic_remote
           + cfg->max_num_mic_clean + cfg->max_num_beam;
  mss_size = svd_size*num_src + mss_inst_size;
  mssBufs[MSS_INSTANCE_BUF].size = mss_size;

  /* Return number of buffers required by MSS */  
  *nbufs = MSS_NUM_BUFS;
  
  /* Return buffer specifications */
  *bufs = &mssBufs[0];                  
  
  return(mss_NOERR);
  
} /* mssGetSizes */

/******************************************************************************
 * FUNCTION PURPOSE: Create an instance of the MSS module.
 ******************************************************************************
 * DESCRIPTION: Creates an instance of the MSS and initializes memory buffers.
 *
 *  tint mssNew (
 *    void            **mssInst,  - address of the memory location that will
 *                                  receive a pointer to instance structure
 *    tint            nbufs,      - number of memory buffers
 *    ecomemBuffer_t  *bufs,      - a vector of buffer descriptors
 *    mssNewConfig_t  *cfg)       - a pointer to configuration structure
 *
 *  Return value: 
 *          mss_NOERR             - success
 *          mss_ERR_INVALIDPAR    - *mssInst is not NULL or nbufs is not correct 
 *          mss_ERR_NOMEMORY      - MSS instance pointer is not NULL, or
 *                                  number of buffers is not correct, or
 *                                  buffer properties are not correct
 *****************************************************************************/
tint mssNew (void **mssInst, tint nbufs, ecomemBuffer_t *bufs, mssNewConfig_t *cfg)
{
  mssInst_t       *inst;
  ecomemBuffer_t  *bufp;
  tint svd_size, mss_size, mss_inst_size, num_src;
  int i;
  
  /* Test instance pointer (must be NULL) */
  if ((*mssInst != NULL) || (nbufs != MSS_NUM_BUFS)) {
    return(mss_ERR_INVALIDPAR);
  }

  /* Check all dynamic buffers (base address != NULL) */
  for (i=0, bufp=&bufs[0]; i<MSS_NUM_BUFS; i++, bufp++) {
    if ((bufp->size>0) && (bufp->base==NULL)) {
      return(mss_ERR_NOMEMORY);
    }
  }

  /* Get the instance buffer descriptor returned by the user */
  bufp = &bufs[MSS_INSTANCE_BUF];

  /* Calculate total memory of MSS: instance + all SVDs */
  svdGetSizes (&svd_size);
  svd_size = mss_SIZE_ALIGN(svd_size,TYP_STRUCT_LOG2ALIGN);
  mss_inst_size = sizeof(mssInst_t);
  mss_inst_size = mss_SIZE_ALIGN(mss_inst_size,TYP_STRUCT_LOG2ALIGN);
  num_src =  cfg->sizeCfg.max_num_mic_fixed + cfg->sizeCfg.max_num_mic_remote
           + cfg->sizeCfg.max_num_mic_clean + cfg->sizeCfg.max_num_beam;
  mss_size = svd_size*num_src + mss_inst_size;
  
  /* Check if the provided memory is good: size, alignment, and volatility */
  if ( (bufp->size < mss_size) 
      || (!typChkAlign(bufp->base, bufp->log2align)) || bufp->volat) {
    return(mss_ERR_NOMEMORY);
  }
  
  /* Assign instance memory and initialize instance */
  inst = (mssInst_t *)bufp->base;
  inst->max_sampling_rate = cfg->sizeCfg.max_sampling_rate;
  inst->svd = (tword *)bufp->base + mss_inst_size; /* SVD address for 1st src */
  inst->svd_size = svd_size;
  inst->handle = cfg->handle;        /* init handle in instance */
  inst->state = MSS_CLOSED;          /* set MSS state to CLOSED */

  /* Return instance address */
  *mssInst = (void *)inst;

  return(mss_NOERR);
  
} /* mssNew() */

/******************************************************************************
 * FUNCTION PURPOSE: Control of MSS
 ******************************************************************************
 * DESCRIPTION: Controls the operation and parameters of the MSS.
 *
 *  tint mssControl (
 *    void  *mssInst,              - a pointer to MSS instance
 *    tint  mssControl *ctl)       - a pointer to the control code structure 
 *
 *  Return value: mss_NOERR           - success
 *                mss_ERR_NOTOPENED   - MSS not opened
 *                mss_ERR_INVALIDSRC  - the manually selected source is invalid 
 *                mss_ERR_INVALIDPAR  - parameter is invalid 
 *****************************************************************************/
tint mssControl (void *mssInst, mssControl_t *ctl)
{
  mssInst_t *inst = (mssInst_t *)mssInst;
  tint ret_val;

  /* Make sure that MSS is open */
  if (inst->state != MSS_OPEN) {
    return (mss_ERR_NOTOPENED);
  }  
  
  ret_val = mss_NOERR;

  /* Change the operation modes bitfield */
  if(mss_chkbit(ctl->valid_bitfield, mss_CTL_MODES)) {
    inst->modes_bf =  (inst->modes_bf & (~ctl->modes.mask))
                    | (ctl->modes.value & ctl->modes.mask);
  } /* mss_CTL_MODES */
  
  /* Manually select source */
  if(mss_chkbit(ctl->valid_bitfield, mss_CTL_SELECT_SRC)) {
    /* Check if selected source is valid:
         - source group must be one of the groups defined in mss.h,      
         - source index must be less than the number of sources in the group. */
    if(   (ctl->source.group > mss_SRC_MAX)
       || (ctl->source.group < mss_SRC_MIC_FIXED) ){
      ret_val = mss_ERR_INVALIDSRC;
    } 
    else if (ctl->source.index > inst->num_src_per_group[ctl->source.group]) {
      ret_val = mss_ERR_INVALIDSRC;
    }
    else if (   (ctl->source.group != inst->cur_src.group) 
             || (ctl->source.index != inst->cur_src.index) ) {      
      /* Prepare to switch source if selected source is valid. If in the middle of a 
         source switch initiated by MSS internally, internal switch will be stopped. */
      inst->switch_src_cnt  = 0;
      inst->crossfade_phase = MSS_FADE_PHA_INIT;
      inst->switch_hang_cnt = inst->params.switch_hangover;
      inst->new_src = ctl->source;
      mss_setbit(inst->state_bf, MSS_STATE_BIT_SWITCH_SRC);
    }
  } /* mss_CTL_SELECT_SRC */

  /* Change the threshold of source switch */
  if(mss_chkbit(ctl->valid_bitfield, mss_CTL_SWITCH_THRESH)) {
    /* Convert parameter from msec to # of samples */
    inst->params.switch_threshold = MSS_NUM_SAMP_1MS_8KHZ * inst->sampling_rate
                                    * ctl->switch_thresh;
  }
  
  /* Change the duration of source switch */
  if(mss_chkbit(ctl->valid_bitfield, mss_CTL_SWITCH_DURATION)) {
    /* Convert parameter from msec to # of samples */
    inst->params.switch_duration = MSS_NUM_SAMP_1MS_8KHZ * inst->sampling_rate
                                   * ctl->switch_duration;
    /* Calculate per sample Q31 phase increment for crossfade implementation: 
       2^31/duration, where 2^31 corresponds to pi. */                                     
    inst->crossfade_phase_inc = frct_LFRCT_MAX/inst->params.switch_duration;                                     
  } /* mss_CTL_SWITCH_DURATION */

  /* Change hangover of source switch */
  if(mss_chkbit(ctl->valid_bitfield, mss_CTL_SWITCH_HNAGOVER)) {
    /* Convert parameter from msec to # of samples */
    inst->params.switch_hangover = MSS_NUM_SAMP_1MS_8KHZ * inst->sampling_rate
                                   * ctl->switch_hangover;
  } /* mss_CTL_SWITCH_HNAGOVER */
  
  return ret_val;
} /* mssControl */
  
/******************************************************************************
 * FUNCTION PURPOSE: Open and configure an MSS instance.
 ******************************************************************************
 * DESCRIPTION: This function configures an instance of the MSS.
 *
 *  tint mssOpen (
 *    void        *mssInst,   - a pointer to MSS instance
 *    mssConfig_t *cfg)       - a pointer to MSS configuration structure
 *
 *****************************************************************************/
tint mssOpen (void *mssInst, mssConfig_t *cfg)
{
  mssInst_t * inst = (mssInst_t *)mssInst;
  tword * svd_ptr;
  tint num_src, svd_size;
  int i;
  
  /* Return error if state is not CLOSED */  
  if(inst->state != MSS_CLOSED) {
    return(mss_ERR_NOTCLOSED);
  }
  
  /* Return error if no configuration parameters */
  if(cfg == NULL){
    return(mss_ERR_INVALIDPAR);
  }
  
  /* Read configuration */
  num_src = cfg->num_mic_fixed+cfg->num_mic_remote+cfg->num_mic_clean+cfg->num_beam;
  if(num_src <= 0) {
    return(mss_ERR_INVALIDPAR);
  }
  
  inst->sampling_rate  = cfg->sampling_rate;
  inst->num_src_per_group[mss_SRC_MIC_FIXED]  = cfg->num_mic_fixed;
  inst->num_src_per_group[mss_SRC_MIC_REMOTE] = cfg->num_mic_remote;
  inst->num_src_per_group[mss_SRC_MIC_CLEAN]  = cfg->num_mic_clean;
  inst->num_src_per_group[mss_SRC_BEAM]       = cfg->num_beam;
  inst->num_mic_array  = cfg->num_mic_array; /* not considered as source to select */
  
  /* Init SVDs */
  svd_size  = inst->svd_size;
  svd_ptr = (tword *)inst->svd;
  for(i=0; i<num_src; i++) {
    if (!typChkAlign(svd_ptr, TYP_STRUCT_LOG2ALIGN)) {
      return(mss_ERR_NOMEMORY);
    }
    svdInit((void *)svd_ptr, inst->sampling_rate);
    svd_ptr += svd_size;
  }    
  
  /* Set parameters to default */
  inst->params.switch_threshold = MSS_DEF_SWITCH_WAIT_TIME * MSS_NUM_SAMP_1MS_8KHZ
                                 * inst->sampling_rate; /* convert msec to # of samples */
  inst->params.switch_duration = MSS_DEF_SWITCH_DURATION * MSS_NUM_SAMP_1MS_8KHZ
                                 * inst->sampling_rate; /* convert msec to # of samples */
  inst->crossfade_phase_inc    = MSS_DEF_FADE_PHA_INC_8KHZ / inst->sampling_rate;
  inst->params.switch_hangover = MSS_DEF_SWITCH_HANGOVER * MSS_NUM_SAMP_1MS_8KHZ 
                                 * inst->sampling_rate; /* convert msec to # of samples */
  inst->modes_bf = mss_CTL_MODES_DEFAULT;
  
  /* Set initially selected source - the first valid source */
  if(cfg->num_mic_fixed > 0) {
    inst->cur_src.group = mss_SRC_MIC_FIXED;
    inst->cur_src.index = 0;
  }
  else if(cfg->num_mic_remote > 0) {
    inst->cur_src.group = mss_SRC_MIC_REMOTE;
    inst->cur_src.index = 0;
  }
  else if(cfg->num_mic_clean > 0) {
    inst->cur_src.group = mss_SRC_MIC_CLEAN;
    inst->cur_src.index = 0;
  }
  else {
    inst->cur_src.group = mss_SRC_BEAM;
    inst->cur_src.index = 0;
  }
  
  /* Initialize instance */ 
  inst->frame_size = inst->sampling_rate * MSS_FRAME_SIZE_8KHZ;
  inst->switch_hang_cnt = inst->params.switch_hangover;
  inst->crossfade_cnt  = 0;
  inst->state_bf = 0;
  
  /* Declare the instance OPEN */
  inst->state = MSS_OPEN;   

  return(mss_NOERR);
} /* mssOpen */

/******************************************************************************
 * FUNCTION PURPOSE: Returns debuging information for an MSS instance.
 ******************************************************************************
 * DESCRIPTION: This function returns debuging information.
 *
 *  tint mssDebugStat (
 *    void        *mssInst,   - a pointer to MSS instance
 *    mssDebugStat_t *mssDbg) - a pointer to MSS debug structure
 *
 *****************************************************************************/
tint mssDebugStat(void *mssInst, mssDebugStat_t *mssDbg)
{
  tint svd_decision;
  tword *svd_ptr;
  LFract sig_pow;
  Fract spch_db, noise_db;
  UFract svd_thresh;
  mssInst_t *inst = (mssInst_t *)mssInst;

  /* Make sure that MSS is open */
  if (inst->state != MSS_OPEN) {
    return (mss_ERR_NOTOPENED);
  }  
  
  mssDbg->cur_src = inst->cur_src;
  mssDbg->new_src = inst->new_src;
  mssDbg->states[0] = (tuint)(inst->crossfade_phase>>16);
  mssDbg->states[1] = (tuint)inst->gain_in;
  mssDbg->states[2] = (tuint)inst->gain_out;
  mssDbg->states[3] = (tuint)inst->switch_src_cnt; 
  mssDbg->states[4] = (tuint)inst->state_bf;

  /* only debug fixed mics for now */
  if(inst->num_src_per_group[mss_SRC_MIC_FIXED] > 1) {
    svd_ptr = inst->svd;
    svd_decision = svdGetDecision(svd_ptr, &sig_pow);
    mssDbg->states[5] = (tuint)svd_decision;
    
    svdGetLevels (svd_ptr, &spch_db, &noise_db, &svd_thresh);
    mssDbg->states[6] = (tuint)spch_db;
    mssDbg->states[7] = (tuint)noise_db;
  
    svd_ptr += inst->svd_size;
    svd_decision = svdGetDecision(svd_ptr, &sig_pow);
    mssDbg->states[8] = (tuint)svd_decision;
    
    svdGetLevels (svd_ptr, &spch_db, &noise_db, &svd_thresh);
    mssDbg->states[9] = (tuint)spch_db;
    mssDbg->states[10] = (tuint)noise_db;
  }
  
  return(mss_NOERR);
  
} /* mssDebugStat */

/******************************************************************************
 * FUNCTION PURPOSE: Close an instance of MSS.
 ******************************************************************************
 * DESCRIPTION: This function closes an instance of MSS by simply changing
 *              the state to CLOSED.
 *
 *  tint mssClose (
 *    void *mssInst)    - a pointer to MSS instance
 *
 *****************************************************************************/
tint mssClose (void *mssInst)
{
  mssInst_t *inst = (mssInst_t *)mssInst;

  if(inst->state != MSS_OPEN) {
    return(mss_ERR_NOTOPENED);
  }
  
  /*Change the MSS state to CLOSED */
  inst->state = MSS_CLOSED;     
  
  return(mss_NOERR);
} /* mssClose */

/******************************************************************************
 * FUNCTION PURPOSE: Delete an MSS instance.
 ******************************************************************************
 * DESCRIPTION: Deletes an instance of MSS. Clears the instance pointer.
 *              Actual "deallocation" of instance should be done elsewhere
 *              after this function is called. Hence, one has to save the
 *              buffer descriptors to make "deallocation" possible.
 *
 *  tint mssDelete (
 *    void           **mssInst,  - an address of memory location that contains
 *                                 a pointer to instance structure
 *    tint           nbufs,      - number of memory buffers
 *    ecomemBuffer_t *bufs)      - vector of buffer descriptors
 *
 *****************************************************************************/
tint mssDelete (void **mssInst, tint nbufs, ecomemBuffer_t *bufs)
{
  mssInst_t *inst = (mssInst_t *)*mssInst;

  /* check if instance is already closed */
  if (inst->state != MSS_CLOSED) {      
    return (mss_ERR_NOTCLOSED);
  }
  
  /* check if enough descriptors are provided to store all buffer addresses */
  if (nbufs < MSS_NUM_BUFS) { 
    return (mss_ERR_NOMEMORY);
  }
 
  /* return buffer addresses */
  bufs[MSS_INSTANCE_BUF].base      = inst;   /* instance structure */

  /* clear the instance pointer */
  *mssInst = NULL;                              

  return(mss_NOERR);
} /* mssDelete */

/******************************************************************************
 * FUNCTION PURPOSE: Validates the input sources to MSS.
 ******************************************************************************
 * DESCRIPTION: This function examines the input sources to MSS to make sure
 *              all sources are valid.
 *
 *****************************************************************************/

tint mss_src_validation(mssInst_t *inst, void *rx_out_sync, void *mic_fix[],
                void *mic_rem[], void *mic_cln[], void *mic_arr[], void *beam[])
{                
  int i;
  
  /* Return error if synchronized Rx out doesn't have valid data (NOT USED!!!) */
  /*
   *  if ( rx_out_sync == NULL ) {  
   *    return (mss_ERR_INVALIDSRC); 
   *  }
   */

  /* Check if all sources have valid data.
     Currently MSS only supports fixed mic, remote mic, and clean mic.
  */
  if(inst->num_src_per_group[mss_SRC_MIC_FIXED] > 0) {
    if(mic_fix == NULL) {
      return (mss_ERR_INVALIDSRC);
    }
    else {
      for (i=0; i<inst->num_src_per_group[mss_SRC_MIC_FIXED]; i++) {
        if(mic_fix[i] == NULL) {
          return (mss_ERR_INVALIDSRC);
        }
      }
    }
  } /* mss_SRC_MIC_FIXED */
  
  if(inst->num_src_per_group[mss_SRC_MIC_REMOTE] > 0) {
    if(mic_rem == NULL) {
      return (mss_ERR_INVALIDSRC);
    }
    else {
      for (i=0; i<inst->num_src_per_group[mss_SRC_MIC_REMOTE]; i++) {
        if(mic_rem[i] == NULL) {
          return (mss_ERR_INVALIDSRC);
        }
      }
    }
  } /* mss_SRC_MIC_REMOTE */
  
  if(inst->num_src_per_group[mss_SRC_MIC_CLEAN] > 0) {
    if(mic_cln == NULL) {
      return (mss_ERR_INVALIDSRC);
    }
    else {
      for (i=0; i<inst->num_src_per_group[mss_SRC_MIC_CLEAN]; i++) {
        if(mic_cln[i] == NULL) {
          return (mss_ERR_INVALIDSRC);
        }
      }
    }
  } /* mss_SRC_MIC_CLEAN */
 
  return (mss_NOERR);  
} /* mss_src_validation */

/* Nothing past this point */
