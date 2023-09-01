/*
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

#ifndef _MSSLOC_H
#define _MSSLOC_H

//#include <ti/mas/aer/mss.h>

#include "../mss.h"

/* MSS states */
enum {
  MSS_CLOSED  = 0,
  MSS_OPEN    = 1
};


#define MSS_STATE_BIT_SWITCH_SRC 0x0001

#define MSS_FRAME_SIZE_8KHZ 80   /* samples */
#define MSS_NUM_SAMP_1MS_8KHZ 8  /* samples per msec at 8kHz */
#define MSS_FADE_PHA_INIT      0xC0000000  /* Q31 initial phase of crossfade:
                                              -1/2 corresponding to -pi/2  */

typedef struct mssParams_s {
  tint   switch_threshold;
  tint   switch_duration;
  tint   switch_hangover;
} mssParams_t;

typedef struct mssInst_s {
  tint      state;                          /* MSS_CLOSED or MSS_OPEN         */
  void *    handle;                         /* MSS instance handle            */
  
  mssParams_t params;
  mssSrc_t    cur_src;
  mssSrc_t    new_src;
  tword       *svd;
  
  tint   num_src_per_group[mss_MAX_NUM_SRC_TYPES];
  tint   modes_bf;
  tint   state_bf;

  LFract crossfade_phase;
  LFract crossfade_phase_inc;
  tint   crossfade_cnt;
  tint   switch_hang_cnt;
  tint   svd_size;
  tint   max_sampling_rate;
  tint   sampling_rate;
  tint   frame_size;
  tint   num_mic_array;

  tint   switch_src_cnt;
  tuint  ramp_up;
  Fract  ramp_alpha;
  
  /* for debugging only */
  mssSrc_t temp_src;  
  Fract    gain_in;   
  Fract    gain_out;  
} mssInst_t;

/* MSS control bitfield macros */
#define mss_setbit(field,b) ( (field) |= (  b))
#define mss_clrbit(field,b) ( (field) &= (~(b)))
#define mss_chkbit(field,b) (((field) &  (  b)) == (b))

/* min, max macros */
#define mss_MAX(x,y)        ( ((x) > (y)) ? (x) : (y) )
#define mss_MIN(x,y)        ( ((x) < (y)) ? (x) : (y) )

/* Size align to L2 power of 2: for L2=3 floor((x+7)/8)*8   */
#define mss_SIZE_ALIGN(x,L2)  (((L2) < 1) ? (x) : (((x) + ( (1<<(L2)) - 1 )) & ((~0uL)<<(L2))))

void mss_src_selection(mssInst_t *inst, void *mic_fix[], void *mic_rem[], 
                        void *mic_cln[]);
void mss_src_output(mssInst_t *inst, void *out, void *mic_fix[], 
                     void *mic_rem[], void *mic_cln[]);
tint mss_src_validation(mssInst_t *inst, void *rx_out_sync, void *mic_fix[],
                void *mic_rem[], void *mic_cln[], void *mic_arr[], void *beam[]);
void * mss_get_first_src(mssInst_t *inst, void *mic_fix[], void *mic_rem[], 
                         void *mic_cln[]);

#endif
/* Nothing past this point */
