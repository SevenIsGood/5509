/******************************************************************************
 * FILE PURPOSE: 
 *               
 ******************************************************************************
 * FILE NAME:   bf_asnr_mss_vau.h
 *
 * DESCRIPTION: Based on compiler switch, include platform specific profile file
 *
 *        Copyright (c) 2016 Texas Instruments Incorporated                
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
 *****************************************************************************/
#ifndef _BF_ASNR_MSS_VAU_H
#define _BF_ASNR_MSS_VAU_H

/* demo includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
///#include <xdc/std.h>

/* RTSC headers */
#include <ti/mas/types/types.h>
#include <ti/mas/fract/fract.h>
#include <ti/mas/types/const.h>
#include <ti/mas/util/ecomem.h>

/* for signal generator */
#include <ti/mas/sdk/sgn.h>

/* VPE component and simulation headers */
#include <ti/mas/vpe/svd.h>
#include <ti/mas/vpe/asnr.h>
#include <vausim.h>
#include <ti/mas/aer/bf.h>
#include <ti/mas/aer/mss.h>

#include "codec_pcm186x.h"

/* Constants and structures for Demo */
#define VPE_SIM_SAMPLE_FREQ_16000 16000    // Sampling frequency
#define VPE_SIM_SAMPLE_FREQ_8000  8000     // Sampling frequency
#define VPE_SIM_FRAME_SIZE_16000   160     /* 160 samples for a 10 msec frame */
#define VPE_SIM_FRAME_SIZE_8000     80     /*  80 samples for a 10 msec frame */
#define vpe_SIM_MAX_SAMPLES          320    // Maximum frame_size
#define vpe_SIM_MAX_ECOBUFS          12     // Maximum module buffers
#define vpe_SIM_MAX_STRING          256     // Maximum string size

/* ASNR parameters */
typedef struct {
  tint samp_rate;
  tint delay;
  tint band_bin1;
  tint band_bin2;
  tint band1_max_atten;
  tint band2_max_atten;
  tint band3_max_atten;
  tint sig_upd_rate_max;
  tint sig_upd_rate_min;
  tint noise_thresh;
  tint noise_hangover;
} vpeASNRPars_t;

typedef struct {
  tbool  bf_on;                     /* BF enable bit     */
  tbool  nr_on;                     /* NR enable bit     */
  tbool  mss_on;                    /* MSS enable bit    */
  tbool  vau_on;                    /* VAU enable bit    */
  tlong  sample_cnt;                  /* Sample count                               */
  tint   Fs;                          /* Sampling rate in Hz                        */
  bfSizeConfig_t bf_size_cfg;         /* BF size configure                          */
  bfControl_t bf_ctl;                 /* BF control                                */
  vpeASNRPars_t  asnrParam;           /* ASNR parameters                            */
  mssControl_t  mss_ctl_cfg;          /* MSS control                               */
  tbool  data;                        /* Flag indicating more samples for simulation*/
  tuint  frame_size_in;               /* Simulation frame size, input               */
  tuint  frame_size_out;              /* Simulation frame size, output              */
  tlong     cyclesOverhead;           /* to measure fixed overhead in cycle measurement */
  cycles_t  cyclesBf;                 /* BF's cycle statistics */
  cycles_t  cyclesAsnr;               /* ASNR's cycle statistics */
  cycles_t  cyclesMss;                /* MSS's cycle statistics */
  cycles_t  cyclesVau;                /* VAU's cycle statistics */
  FILE *outCyclesFilePtr;             /* cycle output file */
} vpeSimConfig_t;

/* Constants and structures for BF */
#define MAX_NUM_BF_BUFFS 5
#define NUM_BF_ANALOG_MIC NUM_OF_MICS
#define NUM_BF_VIRTUAL_MIC NUM_OF_MICS
#define AUDIO_SMAPLE_FREQ 16000
#define AUDIO_FILE_IN_SEC 40
#define bf_INT_HEAP_SIZE 0x1000
#define bf_EXT_HEAP_SIZE 0x800

/* Constants and structures for ASNR */
#ifdef  __CCS_PROJECT_REL_PATH
#define vpe_SIM_DEF_BASE_DIR       "../../../vectors/"
#else
#ifdef AUTOMATE_PATH
#define vpe_SIM_DEF_BASE_DIR       "./"
#else
#define vpe_SIM_DEF_BASE_DIR       "../../vectors/"
#endif
#endif
/* Buffer sizes for NR - returned by nrGetSizes().
   Buffer size is in unit of tword. For C64x+\C674x\C66x , tword is 8-bit. */
#define NR_SIM_BUF0_SIZE   88
#define NR_SIM_BUF1_SIZE   1548+4
#define NR_SIM_BUF2_SIZE   1032
#define NR_SIM_BUF3_SIZE   774+2
#define NR_SIM_BUF4_SIZE   384
#define NR_SIM_BUF5_SIZE   1032
#define NR_SIM_BUF6_SIZE   28+4

/* Buffer alignment for NR - returned by nrGetSizes().
   Buffers must be aligned on boundary of 2^(NR_SIM_BUFx_ALGN_LOG2) twords.
   Also used to define macros used by #pragma DATA_ALIGN in nrbufs.c.
   The pragma DATA_ALIGN (symbol, constant) aligns the symbol to an alignment
   boundary. For C64x+\C674x\C66x, the boundary is the value of the constant in
   bytes. For example, a constant of 4 specifies a 4-byte or 32-bit alignment.*/
#define NR_SIM_BUF0_ALGN_LOG2  3
#define NR_SIM_BUF1_ALGN_LOG2  3
#define NR_SIM_BUF2_ALGN_LOG2  3
#define NR_SIM_BUF3_ALGN_LOG2  3
#define NR_SIM_BUF4_ALGN_LOG2  2
#define NR_SIM_BUF5_ALGN_LOG2  2
#define NR_SIM_BUF6_ALGN_LOG2  3
/* common to all targets */
#define VPE_NR_NUM_BUFS   7

#define NR_SIM_BUF0_ALGN  (1<<NR_SIM_BUF0_ALGN_LOG2)
#define NR_SIM_BUF1_ALGN  (1<<NR_SIM_BUF1_ALGN_LOG2)
#define NR_SIM_BUF2_ALGN  (1<<NR_SIM_BUF2_ALGN_LOG2)
#define NR_SIM_BUF3_ALGN  (1<<NR_SIM_BUF3_ALGN_LOG2)
#define NR_SIM_BUF4_ALGN  (1<<NR_SIM_BUF4_ALGN_LOG2)
#define NR_SIM_BUF5_ALGN  (1<<NR_SIM_BUF5_ALGN_LOG2)
#define NR_SIM_BUF6_ALGN  (1<<NR_SIM_BUF6_ALGN_LOG2)

/* Constants and structures for MSS */
#define AER_SIM_MSS_INST_SIZE 50
#define AER_SIM_SVD_INST_SIZE 10
#define SIU_NUM_MSS_BUFFS 1        /* static buffer allocation */

typedef struct {
  LFract buf[AER_SIM_MSS_INST_SIZE];
} mssinstbuf_t;

typedef struct {
  LFract buf[AER_SIM_SVD_INST_SIZE];
} svdinstbuf_t;

typedef struct {
  mssinstbuf_t mss_inst;
  svdinstbuf_t svd_inst[NUM_BF_VIRTUAL_MIC];
} mssbuf_t;

/* Constants and structures for VAU */
#ifdef  __CCS_PROJECT_REL_PATH
#define nmu_SIM_OUTDEC_FNAME       "../../../vectors/outVauDecisions.txt"
#define nmu_SIM_LOGCYC_FNAME       "../../../logs/cycles.txt"
#define nmu_SIM_LOG_FNAME          "../../../logs/log.txt"
#else
#define nmu_SIM_OUTDEC_FNAME       "../../vectors/outVauDecisions.txt"
#define nmu_SIM_LOGCYC_FNAME       "../../logs/cycles.txt"
#define nmu_SIM_LOG_FNAME          "../../logs/log.txt"
#endif

extern vpeSimConfig_t *vpeSim;

#ifdef PROFILE_CYCLES
static inline void siu_process_cycles(tlong cycles, cycles_t * cyclesPtr)
{
  if (cycles > cyclesPtr->max) {
    cyclesPtr->max = cycles;
  }

  cyclesPtr->avg = ((cyclesPtr->avg) * 255 + cycles) >> 8;
}

static inline void siu_print_cycles_util(cycles_t * cyclesPtr, char *name,
                                         tlong overhead)
{
  printf("%s MaxCycles = %ld AvgCycles = %ld\n", name,
         (long) cyclesPtr->max - (long) overhead,
         (long) cyclesPtr->avg - (long) overhead);
}

void siu_print_cycles(void);
#endif

extern void  vpe_print             (char *str);
extern void  vpe_iprint            (char *str, tlong ivar);
extern void  vpe_sprint            (char *str, char *svar);

void bf_main (void);
tbool bf_process(void);

#endif /* _BF_ASNR_MSS_VAU_H */

/* Nothing past this point */
