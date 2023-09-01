/******************************************************************************
 * FILE PURPOSE: 
 *               
 ******************************************************************************
 * FILE NAME:   vpesim.h
 *
 * DESCRIPTION: Based on compiler switch, include platform specific profile file
 *
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
 *****************************************************************************/
#ifndef _VPESIM_H
#define _VPESIM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xdc/std.h>

/* RTSC headers */
#include <ti/mas/types/types.h>
#include <ti/mas/fract/fract.h>
#include <ti/mas/types/const.h>
#include <ti/mas/util/ecomem.h>

/* for signal generator */
#include <ti/mas/sdk/sgn.h>

/* VPE component and simulation headers */
#include <ti/mas/vpe/hlc.h>
#include <ti/mas/vpe/slm.h>
#include <ti/mas/vpe/snl.h>
#include <ti/mas/vpe/svd.h>
#include <ti/mas/vpe/sfc.h>
#include <ti/mas/vpe/pcm.h>
#include <ti/mas/vpe/asnr.h>

#ifdef ti_targets_C54
#define _VPESIM_C54 1
#else
#define _VPESIM_C54 0
#endif

#ifdef ti_targets_C55 
#define _VPESIM_C55 1
#else
#define _VPESIM_C55 0
#endif

#ifdef ti_targets_C55_large
#define _VPESIM_C55L 1
#else
#define _VPESIM_C55L 0
#endif

#ifdef ti_targets_C64
#define _VPESIM_C64 1
#else
#define _VPESIM_C64 0
#endif

#ifdef ti_targets_C64_big_endian
#define _VPESIM_C64_BIG_ENDIAN 1
#else
#define _VPESIM_C64_BIG_ENDIAN 0
#endif

#ifdef ti_targets_C64P
#define _VPESIM_C64P 1
#else
#define _VPESIM_C64P 0
#endif

#ifdef ti_targets_C64P_big_endian
#define _VPESIM_C64P_BIG_ENDIAN 1
#else
#define _VPESIM_C64P_BIG_ENDIAN 0
#endif

#ifdef ti_targets_C66
#define _VPESIM_C66 1
#else
#define _VPESIM_C66 0
#endif

#ifdef ti_targets_C66_big_endian
#define _VPESIM_C66_BIG_ENDIAN 1
#else
#define _VPESIM_C66_BIG_ENDIAN 0
#endif

/* Simulation error identifiers */
#define vpe_SIM_ERR_1       ( 1)    // 
#define vpe_SIM_ERR_2       ( 2)    // 
#define vpe_SIM_ERR_3       ( 4)    // 
#define vpe_SIM_ERR_4       ( 5)    // 
#define vpe_SIM_ERR_5       ( 6)    // 
#define vpe_SIM_ERR_6       ( 7)    // 
#define vpe_SIM_ERR_7       ( 8)    // 
#define vpe_SIM_ERR_8       ( 9)    // 
#define vpe_SIM_ERR_9       (10)    // 
#define vpe_SIM_ERR_10      (11)    // 
#define vpe_SIM_ERR_11      (12)    // 
#define vpe_SIM_ERR_12      (13)    // 
#define vpe_SIM_ERR_13      (14)    // 
#define vpe_SIM_ERR_14      (15)    // 
#define vpe_SIM_ERR_15      (16)    // 
#define vpe_SIM_ERR_16      (17)    // 
#define vpe_SIM_ERR_17      (18)    // 
#define vpe_SIM_ERR_18      (19)    // 

/* Simulation constraints */
#define vpe_SIM_MAX_SAMPLES          320    // Maximum frame_size
#define vpe_SIM_MAX_ECOBUFS          12     // Maximum module buffers
#define vpe_SIM_MAX_STRING          256     // Maximum string size

/* Simulation file open actions */
#define vpe_SIM_FILEIO_READ         ( 0)    // Open file for read (binary)
#define vpe_SIM_FILEIO_WRITE        ( 1)    // Open file for write (binary)

#define VPE_SIM_SAMPLE_FREQ_16000 16000    // Sampling frequency
#define VPE_SIM_SAMPLE_FREQ_8000  8000     // Sampling frequency
#define VPE_SIM_FRAME_SIZE_16000   160     /* 160 samples for a 10 msec frame */
#define VPE_SIM_FRAME_SIZE_8000     80     /*  80 samples for a 10 msec frame */

#define VPE_SIM_DECIM_FILT_DEL  (sfc_DEF_FILT_LEN-1)
#define VPE_SIM_DECIM_DELAY_LEN (VPE_SIM_DECIM_FILT_DEL + VPE_SIM_FRAME_SIZE_16000)
#define VPE_SIM_INTERP_FILT_DEL ((sfc_DEF_FILT_LEN-1)/2)
#define VPE_SIM_INTERP_DELAY_LEN (VPE_SIM_INTERP_FILT_DEL + VPE_SIM_FRAME_SIZE_16000)
															  
/* Simulation PCM format enumeration */
typedef enum {
  vpe_SIM_PCM_ALAW = const_COMP_ALAW_8,  // 8-bit ulaw PCM
  vpe_SIM_PCM_ULAW = const_COMP_MULAW_8, // 8-bit alaw PCM
  vpe_SIM_PCM_LIN  = const_COMP_LINEAR   // Linear PCM
} vpeSimPcm_e;

typedef struct {
  tbool fio;                            // TRUE: Get/put samples from/to file
  tbool eof;                            // TRUE: End of file reached
  FILE  *fptr;                          // File pointer for read/write
  char  fname[vpe_SIM_MAX_STRING];      // File name to open/close
} vpeSimFileIo_t;

/* Signal Generator Parameters */
typedef struct {
  tbool     state;                  /* TRUE: generator is on.                 */
  linSample dcoffset;               /* dc offset (Q0 format)                  */
  tint      wftype;                 /* one of sgn_WFT_'s                      */
  tint      f1;                     /* the first frequency in Hz              */
  tint      f2;                     /* the second frequency in Hz             */
  linSample amp1;                   /* amplitude of the first component (Q0)  */
  linSample amp2;                   /* amplitude of the second component (Q0) */
  tint      nlevel;                 /* idle noise level in dBm0               */
  tulong    nseed;                  /* idle noise seed                        */
  tint      ntype;                  /* noise type SGN_NT_xxx                  */
  tint      pow_level;              /* Hoth or CSS power level in dBm0        */
  tulong    hoth_seed;              /* Hoth noise seed                        */
} vpeSGNPars_t;

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

/* Simulation configuration structure */
typedef struct {
  tbool  exec;                        /* TRUE: Provide verbose progress info        */
  tbool  hlc_on;                      /* Hlc enable bit                             */
  tbool  slm_on;                      /* Slim enable bit                            */
  tbool  snl_on;                      /* SNL enable bit                             */
  tbool  svd_on;                      /* SVD enable bit                             */
  tbool  nr_on;                       /* NR enable bit                              */
  tbool  decim_on;
  tbool  interp_on;
  tbool  ulaw_enc_on;
  tbool  ulaw_dec_on;
  tbool  alaw_enc_on;
  tbool  alaw_dec_on;
  tlong  sample_cnt;                  /* Sample count                               */
  tulong sample_bp;                   /* Break (halt) after this sample is reached  */
  tbool  sgn;                         /* whether to use sgn, TRUE: use sgn          */
  tbool  cfg_sgn;                     /* TRUE: configure SGN                        */
  tbool  cfg_hlc;                     /* TRUE: configure HLC                        */
  tbool  cfg_slm;                     /* TRUE: configure SLM                        */
  tint   slm_mod;                     /* SLM mode, 0: disable, 1~5: enable          */
  tint   Fs;                          /* Sampling rate in Hz                        */    
  hlcControl_t   hlc_ctrl;            /* HLC control structure                      */
  vpeASNRPars_t  asnrParam;           /* ASNR parameters                            */
  vpeSGNPars_t   sgnParam;            /* Parameters used to configure sgn           */
  vpeSimFileIo_t sigIn;               /* Signal in file I/O structure               */
  vpeSimFileIo_t sigOut;              /* Signal out file I/O structure              */
  vpeSimFileIo_t config;              /* VPE simulation configuration I/O           */
  vpeSimPcm_e    pcmin_format;        /* Input PCM format                           */
  vpeSimPcm_e    pcmout_format;       /* Output PCM format                          */
  tbool  data;                        /* Flag indicating more samples for simulation*/
  tuint  error_id;                    /* Error identifier                           */
  tuint  frame_size_in;               /* Simulation frame size, input               */
  tuint  frame_size_out;              /* Simulation frame size, output              */
  char   baseDir[vpe_SIM_MAX_STRING]; /* Base directory for file I/O                */
} vpeSimConfig_t;
						

extern void  vpe_sim_fileio_init   (vpeSimFileIo_t *fioptr, tuint code);
extern void  vpe_sim_fileio_end    (vpeSimFileIo_t *fioptr);
extern void  vpe_sim_init          ();
extern void  vpe_init              (void);
extern tbool vpe_sim_fread         (linSample *buf, tint buf_size, vpeSimFileIo_t *fioptr);
extern void  vpe_sim_fwrite        (linSample *buf, tint buf_size, vpeSimFileIo_t *fioptr);
extern void  vpe_sim_input_mux     (void);
extern void  vpe_sim_output_mux    (void);
extern void  vpe_print             (char *str);
extern void  vpe_iprint            (char *str, tlong ivar);
extern void  vpe_sprint            (char *str, char *svar);
extern void  vpe_sim_open_sgn      (void *inst);
extern void  vpe_sim_set_sgn (void *inst, vpeSGNPars_t *sgn, tint Fs);
extern void  vpe_sim_gen_frame     (void *inst, tint frame_size, linSample *frame);
extern sgnFileIo_t *vpe_sim_in_file(sgnFileIo_t *dsc);
extern sgnFileIo_t *vpe_sim_in_file(sgnFileIo_t *dsc);
extern void  vpe_instantiate_nr(tint srate);
extern void  vpe_config_nr(vpeASNRPars_t *asnr_params);
extern void  vpe_deinstantiate_nr();
extern void  vpe_halt (tbool *exec);
extern void  vpe_sim_flush_cache();
extern void  vpe_sim_read_cfg();
						
#endif /* _VPESIM_H */

/* Nothing past this point */
