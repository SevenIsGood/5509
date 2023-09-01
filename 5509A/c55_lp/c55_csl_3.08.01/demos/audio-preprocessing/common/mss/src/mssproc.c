/**
 *  @file   mssproc.c
 *  @brief  Contains Main Processing Routines for Multi-Source Selection 
 *          (MSS) module.
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
#include <ti/mas/util/utl.h>
#include <ti/mas/vpe/svd.h>
#include "string.h"

//#include <ti/mas/aer/mss.h>
//#include <ti/mas/aer/src/mssloc.h>

#include "../mss.h"
#include "mssloc.h"

#define MSS_MIN_POW 2684  /* correpsonds to -50dBm */

/******************************************************************************
 * FUNCTION PURPOSE: Main Processing API of MSS 
 ******************************************************************************
 * DESCRIPTION: This function processes the multi-source inputs and selects one
 *              source as the output. Crossfading is implemented during the 
 *              switch of two sources. 
 *
 *  tint mssProcess (
 *  Input:
 *    void *mssInst,      - pointer to MSS instance
 *    void *rx_out_sync,  - synchronized Rx out, NOT to be selected, but used to
 *                          help make right selection.
 *    void *mic_fix[],    - data from fixed mics, may be selected.
 *    void *mic_rem[],    - data from remote mics, may be selected.
 *    void *mic_cln[],    - data from clean mics, may be selected.
 *    void *mic_arr[],    - data from microphone arrays, NOT to be selected, but
 *                          used to help make right selection.
 *    void *beam[],       - data from beamformers, may be selected.
 *  Output:
 *    void *out,          - data of the selected source, or a mix of two sources
 *                          during the switch. 
 *
 *****************************************************************************/
tint mssProcess(void *mssInst, void *out, void *rx_out_sync, void *mic_fix[],
                void *mic_rem[], void *mic_cln[], void *mic_arr[], void *beam[])
{
  tint src_err; 
  void *first_src;
  mssInst_t * inst = (mssInst_t *)mssInst;
  
  /* Return error if state is not OPENED */  
  if(inst->state != MSS_OPEN) {
    return(mss_ERR_NOTOPENED);
  }

  /* Check if all sources are valid and return error if any one is invalid. */  
  src_err = mss_src_validation(inst, rx_out_sync, mic_fix, mic_rem, mic_cln,
                               mic_arr, beam);                             
  if(src_err == mss_ERR_INVALIDSRC) {
    return(mss_ERR_INVALIDSRC);
  }

  /* The BYPASS mode just gets the first available source passed to the output */
  if(mss_chkbit(inst->modes_bf, mss_CTL_MODES_BYPASS)) {
    first_src = mss_get_first_src(inst, mic_fix, mic_rem, mic_cln);
    memcpy(out, first_src, inst->frame_size*sizeof(linSample));
  }
  else { /* If not bypass, check for FREEZE */
    if(!mss_chkbit(inst->modes_bf, mss_CTL_MODES_FREEZE)) {
      /* Multi-source selection: 
         Currently only supports fixed, remote and clean mics. */
      mss_src_selection(inst, mic_fix, mic_rem, mic_cln); 
    }

    /* Output selected source, crossfading during switch between two sources. */
    mss_src_output(inst, out, mic_fix, mic_rem, mic_cln);
  }
  
  return src_err;
} /* mssProcess */

/******************************************************************************
 * FUNCTION PURPOSE: Get the First Valid Source 
 ******************************************************************************
 * DESCRIPTION: This function orted.
 *
 *  tint mss_get_first_src (
 *  Input:
 *    void *inst,         - pointer to MSS instance
 *    void *mic_fix[],    - data from fixed mics, may be selected.
 *    void *mic_rem[],    - data from remote mics, may be selected.
 *    void *mic_cln[],    - data from clean mics, may be selected.
 *  Output:
 *    void * ,            - address of the first valid source
 *****************************************************************************/
void * mss_get_first_src(mssInst_t *inst, void *mic_fix[], void *mic_rem[], 
                         void *mic_cln[])
{
  void * first_src;
  
  if(inst->num_src_per_group[mss_SRC_MIC_FIXED] > 0) {
    first_src = mic_fix[0];
  }
  else if(inst->num_src_per_group[mss_SRC_MIC_REMOTE] > 0) {
    first_src = mic_rem[0];
  }
  else {
    first_src = mic_cln[0];
  }
  
  return first_src;
} /* mss_get_first_src */                        

/******************************************************************************
 * FUNCTION PURPOSE: Source selection of MSS 
 ******************************************************************************
 * DESCRIPTION: This function processes the multi-source inputs and selects one
 *              source as the output. The selectin criteria is the exponential
 *              average of power. The source with maximum power is selected. 
 *              Currently, only fixed mic, remote mic, and clean mic are supported.
 *
 *  tint mss_src_selection (
 *  Input:
 *    void *inst,         - pointer to MSS instance
 *    void *mic_fix[],    - data from fixed mics, may be selected.
 *    void *mic_rem[],    - data from remote mics, may be selected.
 *    void *mic_cln[],    - data from clean mics, may be selected.
 *  Output:
 *    Instance variables: MSS_STATE_BIT_SWITCH_SRC of inst->state_bf
 *                        temp_src
 *                        switch_hang_cnt
 *                        new_src
 *                        crossfade_phase
 *                        crossface_cnt
 * 
 *****************************************************************************/
void mss_src_selection(mssInst_t *inst, void *mic_fix[], void *mic_rem[], 
                        void *mic_cln[])
{
  int i, max_pow_src_index, svd_decision;
  LFract max_pow, sig_pow;
  tword *svd_ptr;
  mssSrcType_t max_pow_src_group;
  tint frm_len;

  /* Initialization */
  svd_ptr = inst->svd;
  frm_len = inst->frame_size;
  max_pow = (LFract)0;
  max_pow_src_group = mss_SRC_INVALID;
  
  /*
   * Task 1: Analyze sources to select the best one.
   * Current selection criteria is the exponential average power. Whichever
   * source has the largest power will be selected. 
   */
  /* Analyze fixed microphones */ 
  for (i=0; i<inst->num_src_per_group[mss_SRC_MIC_FIXED]; i++) {
    svdProcess(svd_ptr, mic_fix[i], frm_len);
    svd_decision = svdGetDecision(svd_ptr, &sig_pow);
    
    /* only look at power of mics having speech (MSS_MIN_POW is -50.0109dBm) */
    if(   (svd_decision != svd_VOICE_INACTIVE)  /* even when undefined? */
       && (sig_pow > MSS_MIN_POW) && (sig_pow > max_pow)) {
      max_pow = sig_pow;
      max_pow_src_group = mss_SRC_MIC_FIXED;
      max_pow_src_index = i;
    }
    
    svd_ptr += inst->svd_size;
  } /* num_mic_fix */

  /* Analyze remote microphones */ 
  for (i=0; i<inst->num_src_per_group[mss_SRC_MIC_REMOTE]; i++) {
    svdProcess(svd_ptr, mic_rem[i], frm_len);
    svd_decision = svdGetDecision(svd_ptr, &sig_pow);
    
    /* only look at power of mics having speech */
    if(   (svd_decision != svd_VOICE_INACTIVE) 
       && (sig_pow > MSS_MIN_POW) && (sig_pow > max_pow)) {
      max_pow = sig_pow;
      max_pow_src_group = mss_SRC_MIC_REMOTE;
      max_pow_src_index = i;
    }
    
    svd_ptr += inst->svd_size;
  } /* num_mic_remote */

  /* Analyze clean microphones */ 
  for (i=0; i<inst->num_src_per_group[mss_SRC_MIC_CLEAN]; i++) {
    svdProcess(svd_ptr, mic_cln[i], frm_len);
    svd_decision = svdGetDecision(svd_ptr, &sig_pow);
    
    /* only look at power of mics having speech */
    if(   (svd_decision != svd_VOICE_INACTIVE) 
       && (sig_pow > MSS_MIN_POW) && (sig_pow > max_pow)) {
      max_pow = sig_pow;
      max_pow_src_group = mss_SRC_MIC_CLEAN;
      max_pow_src_index = i;
    }
    
    svd_ptr += inst->svd_size;
  } /* num_mic_clean */

  /* for debugging only */
  inst->temp_src.group = max_pow_src_group;
  inst->temp_src.index = max_pow_src_index;
  
  /*
   * Task 2: Decide whether to switch source based on the selection obtained above.
   */  
  /* Don's consider switching source in the middle of a switch */
  if(!mss_chkbit(inst->state_bf, MSS_STATE_BIT_SWITCH_SRC)) {
    /* If not in the middle of a switch, wait until hangover expires. */  
    if(inst->switch_hang_cnt > 0) {
      inst->switch_hang_cnt -= frm_len;  /* expiring hangover */
    }
    else { /* hangover expires and we can look for switch again */
      /* Don't consider switching source if:
         1. speech is not present in any source (max_pow_src_group == mss_SRC_MAX), or 
         2. source with max power is the current selected source.
      */
      if(  (max_pow_src_group == mss_SRC_INVALID)  
        || (   (max_pow_src_group == inst->cur_src.group) 
            && (max_pow_src_index == inst->cur_src.index) )
        ) {
        if (inst->switch_src_cnt > frm_len) {
          inst->switch_src_cnt -= frm_len;     /* Count in opposite direction with same speed */
        }
        else {  /* give up on "new src" */
          inst->switch_src_cnt = 0;
          inst->new_src = inst->cur_src;
        }
      }
      /* Consider switching source if the source with max power is not the 
         current active source. */
      else if(   (max_pow_src_group == inst->new_src.group)
              && (max_pow_src_index == inst->new_src.index) 
             ) {
        /* Increment counter since the source with max power doesn't change 
           from previous frame. */
        inst->switch_src_cnt += frm_len;
        if(inst->switch_src_cnt >= inst->params.switch_threshold) {
          /* Confirm and switch source because the same source had max power long enough */
          inst->switch_src_cnt  = 0;
          inst->crossfade_phase = MSS_FADE_PHA_INIT;
          inst->crossfade_cnt  = 0;
          inst->switch_hang_cnt = inst->params.switch_hangover;
          mss_setbit(inst->state_bf, MSS_STATE_BIT_SWITCH_SRC);
        }
      }
      else {  /* Source with max power might be changing to something other then current/new */
        if (inst->switch_src_cnt > frm_len) {
          inst->switch_src_cnt -= frm_len/2;      /* Lower speed, this should smooth the "new" selection */
        }
        else {
          /* Don't switch since source with max power is changing. */
          inst->new_src.group  = max_pow_src_group;
          inst->new_src.index  = max_pow_src_index;
          inst->switch_src_cnt = 0;
        }
      }
    } /* hangover expires */
  } /* !mss_chkbit(inst->state_bf, MSS_STATE_BIT_SWITCH_SRC) */ 
} /* mss_src_selection */                       

/******************************************************************************
 * FUNCTION PURPOSE: Generating Single Output from Multiple Sources 
 ******************************************************************************
 * DESCRIPTION: This function generates single output from multiple input sources.
 *              Generally, it passes the selected source to the output, but when
 *              there is a switch from one source to another, it will mix the
 *              the previously selected and newly selected sources. 
 *
 *  tint mss_src_output (
 *  Input:
 *    void *inst,         - pointer to MSS instance
 *    void *mic_fix[],    - data from fixed mics, may be selected.
 *    void *mic_rem[],    - data from remote mics, may be selected.
 *    void *mic_cln[],    - data from clean mics, may be selected.
 *  Output:
 *    void *out           - single source or mixed sources
 *****************************************************************************/
#define MSS_SIN_PIOVER2 16388
void mss_src_output(mssInst_t *inst, void *out, void *mic_fix[], 
                     void *mic_rem[], void *mic_cln[])
{
  int i;
  linSample * cur_sel_src;
  linSample * new_sel_src;
  linSample * output;
  Fract fade_in_alpha, fade_out_alpha, sin_phase;
  LFract phase, phase_inc;
  
  /* Identify the buffer of the current selected source. */
  if(inst->cur_src.group == mss_SRC_MIC_FIXED) {
    cur_sel_src = mic_fix[inst->cur_src.index];
  }
  else if (inst->cur_src.group == mss_SRC_MIC_REMOTE) {
    cur_sel_src = mic_rem[inst->cur_src.index];
  }
  else if (inst->cur_src.group == mss_SRC_MIC_CLEAN) {
    cur_sel_src = mic_cln[inst->cur_src.index];
  }
  else {
    /* error message */
  }

  /* Check if in the middle of a switch. */
  if(!mss_chkbit(inst->state_bf, MSS_STATE_BIT_SWITCH_SRC)) {
    /* If it is not during a switch, copy the current selected source to output 
       without any modification. */
    memcpy(out, cur_sel_src, inst->frame_size*sizeof(linSample));
  }  
  else {
    /* If it is during a switch, identify the buffer of the new selected source. */
    if(inst->new_src.group == mss_SRC_MIC_FIXED) {
      new_sel_src = mic_fix[inst->new_src.index];
    }
    else if (inst->new_src.group == mss_SRC_MIC_REMOTE) {
      new_sel_src = mic_rem[inst->new_src.index];
    }
    else if (inst->new_src.group == mss_SRC_MIC_CLEAN) {
      new_sel_src = mic_cln[inst->new_src.index];
    }
    else {
      /* error message */
    }   
  
    /* Mix the previously selected and newly selected sources by crossfading: 
       fading out previously selected source and fading in new selected source. */
    phase = inst->crossfade_phase; 
    phase_inc = inst->crossfade_phase_inc;
    output = (linSample *)out;

    for(i=0; i<inst->frame_size; i++) {
//      alpha = (1-utlSin(phase>>16)) / 2; /* to use macros */
      sin_phase = utlSin(phase>>16);   /* Q.31 to Q.15 */
      fade_out_alpha = MSS_SIN_PIOVER2/2 - sin_phase/2; /* (1-sin(pha))/2 in Q.14 */
      fade_in_alpha  = MSS_SIN_PIOVER2/2 + sin_phase/2; /* (1+sin(pha))/2 in Q.14 */
      fade_out_alpha = mss_MIN(16383, fade_out_alpha);
      fade_in_alpha  = mss_MIN(16383, fade_in_alpha);
      output[i] = frctMul(cur_sel_src[i], 0, fade_out_alpha, 14, 0) 
                 +frctMul(new_sel_src[i], 0, fade_in_alpha,  14, 0);
      phase += phase_inc;
    }
    inst->crossfade_phase = phase;

    /* for debugging only */
    inst->gain_in  = fade_in_alpha;
    inst->gain_out = fade_out_alpha;
     
    inst->crossfade_cnt += inst->frame_size;
    if(inst->crossfade_cnt >= inst->params.switch_duration) {
      inst->crossfade_cnt  = 0;
      inst->cur_src = inst->new_src;
      mss_clrbit(inst->state_bf, MSS_STATE_BIT_SWITCH_SRC);
    }    
  } 
} /* mss_src_output */                    

/* Nothing past this point */
