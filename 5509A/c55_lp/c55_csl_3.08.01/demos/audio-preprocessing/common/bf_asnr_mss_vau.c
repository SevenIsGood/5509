/*-----------------------------------------------------------------
 * Filename:  bf_asnr_mss_vau.c
 *                            
 * Description: Demo for the BF, ASNR, MSS and VAU API functions.
 * 
 *        Copyright (c) 2017 Texas Instruments Incorporated
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
 *-----------------------------------------------------------------*/

/* ANSI C headers */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* RTSC headers */
#include <ti/mas/types/types.h>
#include <ti/mas/fract/fract.h>
#include <ti/mas/types/const.h>
#include <ti/mas/util/ecomem.h>

/* for signal generator */
#include <ti/mas/sdk/sgn.h>

#include "bf_asnr_mss_vau.h"
#include "codec_pcm186x.h"
#include "IdleLoop.h"
#include "sysbfflt.h"
#include "siubf.h"
#include "siuloc.h"
#include "siuport.h"

#ifdef PROFILE_CYCLES
#include "csl_gpt.h"
#endif

///#if _VPESIM_C64||_VPESIM_C64_BIG_ENDIAN||_VPESIM_C64P||_VPESIM_C64P_BIG_ENDIAN |_VPESIM_C66||_VPESIM_C66_BIG_ENDIAN
#include <ti/mas/sdk/memArchCfg.h>
///#endif

/* Global Varibles */
extern Uint32 i2sDmaWriteBufLeft[];  // left Tx write ping/pong buffer
extern Uint32 i2sDmaWriteBufRight[]; // right Tx write ping/pong buffe
extern Uint32 i2sDmaReadBufLeft[];  // left Rx read ping/pong buffer
extern Uint32 i2sDmaReadBufRight[]; // right Rx read ping/pong buffer
#ifdef INSTANCE3_I2S
extern Uint32 i2sDmaReadBufLeft2[];  // left Rx read ping/pong buffer2
extern Uint32 i2sDmaReadBufRight2[]; // right Rx read ping/pong buffer2
#endif
#ifdef INSTANCE0_I2S
extern Uint32 i2sDmaReadBufLeft3[];  // left Rx read ping/pong buffer3
extern Uint32 i2sDmaReadBufRight3[]; // right Rx read ping/pong buffer3
#endif

#ifdef PROFILE_CYCLES
CSL_GptObj         gptObj;
CSL_Handle         hGpt;
CSL_Status         status;
CSL_Config         hwConfig;
Uint32             timeCnt1;
Uint32             timeCnt2;
#endif

/* Global PCM data buffers */
linSample sigInBuffer[NUM_BF_VIRTUAL_MIC][vpe_SIM_MAX_SAMPLES];
linSample *buf_Inptr[NUM_BF_VIRTUAL_MIC];
linSample *buf_Outptr;
linSample *buf_Outptr2;
linSample buf_Drc[I2S_DMA_BUF_LEN];
linSample *buf_Drcptr=&buf_Drc[0];

/* varibles for BF */
tint fltIdx[3][16] = {
  {0, 1, 3, 4, 5, 7}, /* 6 virtual mics 60 degree apart */
  {0, 2, 4, 6}, /* 4 virtual mics 90 degree apart */
  {2, 6},      /* 2 virtual mics 180 degree apart */
};
void  *bfInst[NUM_BF_VIRTUAL_MIC];
tword bfIntMem[bf_INT_HEAP_SIZE];
tword bfExtMem[bf_EXT_HEAP_SIZE];
bfMemHeap_t bfIntHeap;
bfMemHeap_t bfExtHeap;
tint num_bufs_req_by_bf;
const ecomemBuffer_t *bufs_req_by_bf;
ecomemBuffer_t bufs_aloc_for_bf[MAX_NUM_BF_BUFFS];
bfNewConfig_t bf_new_cfg;
linSample bfInFrame[NUM_BF_ANALOG_MIC][vpe_SIM_MAX_SAMPLES];
linSample *inFramePtr[NUM_BF_ANALOG_MIC];

volatile Uint16 g_curSRC, g_newSRC;

/* varibles for ASNR */
void *asnrInst[NUM_BF_VIRTUAL_MIC];
void *nr_handle[NUM_BF_VIRTUAL_MIC];
linSample vpeIOBufs[vpe_SIM_MAX_SAMPLES];
vpeSimConfig_t  vpeSimC = {
  TRUE,               /* bf_on         */
#ifdef STATIC_BF
  FALSE,               /* nr_on         */
#else
  TRUE,               /* nr_on         */
#endif
  TRUE,               /* mss_on        */
#ifdef STATIC_BF
  FALSE,               /* drc_on        */
#else
  TRUE,               /* drc_on        */
#endif
  0,                  /* sample_cnt    */
  16000,              /* Sampling rate in Hz                 */
  /* BF size configure parameters                            */
  {
    bf_SAMP_RATE_16K,
    NUM_BF_ANALOG_MIC,
    SYS_BF_FILTER_LENGTH,
    bf_TYPE_FIXED,
  },
  /* BF control parameters                            */
  {
    bf_CTL_CONFIG|bf_CTL_MICS, /* Enable BF, operation mode change */
    bf_CTL_CFG_ENABLE|bf_CTL_CFG_OPERATION,
    bf_ENABLE|bf_OPT_NORMAL,
#if NUM_BF_ANALOG_MIC==4
    0x000f, /* 4 analog mics */
#else
    0x0003, /* 2 analog mics */
#endif
  },
  /* ASNR parameters                            */
  {
    1,                        /* Sampling frequency 0: 8Khz; 1: 16Khz */
    5,                        /* ASNR Signal delay                    */
    10,                       /* ASNR Frequency bands boundary bin 1  */
    32,                       /* ASNR Frequency bands boundary bin 2  */
    9,                        /* ASNR Maximum attenuation in band 1   */
    9,                        /* ASNR Maximum attenuation in band 2   */
    9,                        /* ASNR Maximum attenuation in band 3   */
    31129,                    /* ASNR Maximum signal update rate      */
    29818,                    /* ASNR Minimum signal update rate      */
    -75,                      /* ASNR Noise threshold                 */
    150,                      /* ASNR Noise hangover                  */
  },
  /* MSS control parameters */
  {
#ifdef STATIC_BF  //macro set in bf_asnr_mss_vau.h
   mss_CTL_MODES | mss_CTL_SELECT_SRC, /* bitfield */            //static
   {0x0002, mss_CTL_MODES_FREEZE},                               //static
   {mss_SRC_MIC_FIXED, 0},          /* source {group, index} */  //for static case change values 0-3. This line has no effect in the dynamic case.
#else
    //DYNAMIC BF
    mss_CTL_SWITCH_THRESH|mss_CTL_SWITCH_DURATION|mss_CTL_SWITCH_HNAGOVER, /* bitfield */  //dynamic
    {0x0003, mss_CTL_MODES_DEFAULT},    /* mode {mask, value} */                            //dynamic
    {mss_SRC_MIC_FIXED, 0},          /* source {group, index} */  //for static case change values to 0-3. This line has no effect in the dynamic case.
#endif
    250,                      /* switch thresh */
    500,                      /* switch duration */
    500,                      /* switch hangover */
  },
  /* DRC parameters                            */
  {
    -50,                      /* The expansion knee point in dBm0 */
    0,                        /* Expansion and Compression Ratios
                                 from 16 to 255 in Q4 format.
                                Bit 0-7: expansion ratio = E_out/E_in. From 1.0 - 15.9375.
                                Bit 8-15:compression ratio = E_in/E_out. From 1.0 - 15.9375.  */
    6,                        /* Maximum amplification in dB  */
    0,                        /* The compression knee point in dBm0  */
    0,                        /* The energy limiting threshold in dBm0  */
  },
  TRUE,                 /* Flag indicating more samples for simulation*/
  160,                    /* Simulation frame size, input               */
  160,                    /* Simulation frame size, output              */
  0,                      /* to measure fixed overhead in cycle measurement */
  {0, 0},                 /* BF's cycle statistics */
  {0, 0},                 /* ASNR's cycle statistics */
  {0, 0},                 /* MSS's cycle statistics */
  {0, 0},                 /* VAU's cycle statistics */
  0,                   /* cycle output file pointer */
};
vpeSimConfig_t *vpeSim = &vpeSimC;

/* DRC control parameters */
drcSizeConfig_t drcSizeConfig = {
  drc_SAMP_RATE_16K,          /* sampling rate */
  16*drc_SAMP_RATE_16K        /* 2ms maximum forward looking limiter delay in samples */
};

drcOpenConfig_t drcOpenConfig = {   /* for drcOpen */
  ((1<<drc_CFGBITS_FBAND) |   /* use full-band operation */
   (1<<drc_CFGBITS_LIM)   |   /* use limiter */
   (1<<drc_CFGBITS_VAD)),     /* update gains only during speech */
  drc_SAMP_RATE_16K           /* sampling rate */
};

static void sysDbgInfo (void *handle, tuint par1, tuint par2, tuint par3, tuint *ptr)
{
  return;
} /* dummy sysDbgInfo */

/* Allocate memory for NR buffers - size and alignment, for C55x/C6x */
#pragma DATA_ALIGN  (nr_buff0, NR_SIM_BUF0_ALGN)
tword nr_buff0[NUM_BF_VIRTUAL_MIC][NR_SIM_BUF0_SIZE];
#pragma DATA_ALIGN  (nr_buff1, NR_SIM_BUF1_ALGN)
tword nr_buff1[NUM_BF_VIRTUAL_MIC][NR_SIM_BUF1_SIZE];
#pragma DATA_ALIGN  (nr_buff2, NR_SIM_BUF2_ALGN)
tword nr_buff2[NUM_BF_VIRTUAL_MIC][NR_SIM_BUF2_SIZE];
#pragma DATA_ALIGN  (nr_buff3, NR_SIM_BUF3_ALGN)
tword nr_buff3[NUM_BF_VIRTUAL_MIC][NR_SIM_BUF3_SIZE];
#pragma DATA_ALIGN  (nr_buff4, NR_SIM_BUF4_ALGN)
tword nr_buff4[NUM_BF_VIRTUAL_MIC][NR_SIM_BUF4_SIZE];
#pragma DATA_ALIGN  (nr_buff5, NR_SIM_BUF5_ALGN)
tword nr_buff5[NUM_BF_VIRTUAL_MIC][NR_SIM_BUF5_SIZE];
#pragma DATA_ALIGN  (nr_buff6, NR_SIM_BUF6_ALGN)
tword nr_buff6[NUM_BF_VIRTUAL_MIC][NR_SIM_BUF6_SIZE];

/* varibles for MSS */
void  *mssInst = 0;
#pragma DATA_ALIGN(mss_buf, 8);
mssbuf_t mss_buf;
void *beam_mics[NUM_BF_VIRTUAL_MIC];
linSample data_rx_sync[vpe_SIM_MAX_SAMPLES];  /* reference signal */
/* To be used for debug trace */
mssSrc_t    mssDbgCurSrc = {
  -1, -1                        /* Current source group/index */
};
mssSrc_t    mssDbgNewSrc = {
  -1, -1                        /* New source group/index */
};

/* varibles for DRC */
void  *drcInst = 0;

/* varibles for VAU */
config_t  config = {
  0,  /* xdaisFlag */
  0,  /* don't use default configuration for xdais */
  80, /* 10 ms default frame size at 8 KHz */
  2,  /* 8 KHz */

  /* vauConfig */
  {
   ifvau_VALID_TYPE | ifvau_VALID_THRESH_OFFSET | ifvau_VALID_HOLD_OVER,
   ifvau_TYPE_ADAPTIVE,
   0,
   180},

  /* vauControl */
  {
   ifvau_VALID_CONTROL,
   ifvau_ENABLE
  }
#ifdef USE_CACHE
 ,1, /* doCacheFlush */
  1  /* doCacheEnable */
#endif
};

tint siu_alloc_bf_buffs(const ecomemBuffer_t *bufs_req, ecomemBuffer_t *bufs_aloc,
                         tint num_bufs);

/* Global simulation data */
char vpe_sim_global_str[vpe_SIM_MAX_STRING];
char vpe_sim_print_str[vpe_SIM_MAX_STRING];

/*-----------------------------------------------------------------
 * Function:  vpe_print
 *
 * Description: Utility to provide static progress information.
 *-----------------------------------------------------------------*/
void vpe_print (char *str)
{
    printf (str);
}

/*-----------------------------------------------------------------
 * Function:  vpe_iprint
 *
 * Description: Utility to provide integer progress information.
 *-----------------------------------------------------------------*/
void vpe_iprint (char *str, tlong ivar)
{
  sprintf   (vpe_sim_print_str, str, ivar);
  vpe_print (vpe_sim_print_str);
}

/*-----------------------------------------------------------------
 * Function:  vpe_halt
 *
 * Description: Utility to provide simulation breakpoint.
 *-----------------------------------------------------------------*/
void vpe_halt (tbool *exec)
{
  /* Wait for further actions - exit when user sets "halt" to FALSE */
  while (!(*exec)) {
    asm ("  NOP");
    asm ("Set_BP_Here:  ");
    asm ("  NOP");
    asm ("  NOP");
  }
}

void vpe_sim_flush_cache() {
#if _VPESIM_C64||_VPESIM_C64_BIG_ENDIAN||_VPESIM_C64P||_VPESIM_C64P_BIG_ENDIAN |_VPESIM_C66||_VPESIM_C66_BIG_ENDIAN
  /* flush cache */
  memarchcfg_cacheFlush();
#endif
}

void siu_print_buff_usage(const ecomemBuffer_t *bufs, tint num_bufs)
{
  int i;

  printf("    Buffer    Size(twords)    Alignment    Volatile   address\n");
  for(i=0; i<num_bufs; i++) {
    printf("     %3d       %7d         %4d       ",
                 i, (int)bufs[i].size, (int)bufs[i].log2align);
    if(bufs[i].volat)
      printf("    yes");
    else
      printf("    no ");

    printf("    0x%x\n", bufs[i].base);
  }
}
#ifdef PROFILE_CYCLES
void siu_print_cycles(void)
{
  printf("-----------------------------------------------------------------\n");
  printf("Performance Data for %d channel analog input and %d virtual mics:\n",
          NUM_BF_ANALOG_MIC, NUM_BF_VIRTUAL_MIC);
  printf("-----------------------------------------------------------------\n");

  siu_print_cycles_util(&vpeSim->cyclesBf, "BF", vpeSim->cyclesOverhead);
  siu_print_cycles_util(&vpeSim->cyclesAsnr, "ASNR", vpeSim->cyclesOverhead);
  siu_print_cycles_util(&vpeSim->cyclesMss, "MSS", vpeSim->cyclesOverhead);
  siu_print_cycles_util(&vpeSim->cyclesDrc, "DRC", vpeSim->cyclesOverhead);

  printf("Sum MaxCycles = %ld AvgCycles = %ld\n",
         ((vpeSim->cyclesBf.max - vpeSim->cyclesOverhead)+
         (vpeSim->cyclesAsnr.max - vpeSim->cyclesOverhead)+
         (vpeSim->cyclesMss.max - vpeSim->cyclesOverhead)+
         (vpeSim->cyclesDrc.max - vpeSim->cyclesOverhead)),
         ((vpeSim->cyclesBf.avg - vpeSim->cyclesOverhead)+
         (vpeSim->cyclesAsnr.avg - vpeSim->cyclesOverhead)+
         (vpeSim->cyclesMss.avg - vpeSim->cyclesOverhead)+
         (vpeSim->cyclesDrc.avg - vpeSim->cyclesOverhead)));

}
#endif
/******************************************************************************
 * FUNCTION PURPOSE: Init heap memory.
 ******************************************************************************
 * DESCRIPTION:
 *
 *  void bf_mem_heap_init (
 *    bfMemHeap_t *ptr,
 *    tword *heapBase,
 *    tulong size)          - channel number (1 to SIU_MAX_CHANNELS)
 *
 *****************************************************************************/

void bf_mem_heap_init (bfMemHeap_t *ptr, tword *heapBase, tulong size)
{
  ptr->base = heapBase;
  ptr->size = size;
  ptr->indx = 0;
  /* Note : memory is not initialized deliberately to expose
            uninitialized memory read problems */
}

void bf_mem_init_heap (void)
{
  /* Internal heap */
  bf_mem_heap_init (&bfIntHeap, bfIntMem, bf_INT_HEAP_SIZE);
  /* External heap */
  bf_mem_heap_init (&bfExtHeap, bfExtMem, bf_EXT_HEAP_SIZE);
}

/******************************************************************************
 * FUNCTION PURPOSE: align memory
 ******************************************************************************
 * DESCRIPTION:
 *
 *  void *bf_mem_align (
 *    void *addr,
 *    tuint lin_align)          - channel number (1 to SIU_MAX_CHANNELS)
 *
 *****************************************************************************/

void *bf_mem_align (void *addr, tuint lin_align)
{
  tulong align_addr;

  align_addr = ((tulong) addr + lin_align - 1UL) & (0xFFFFFFFFUL - lin_align + 1UL);
  return ((void *) align_addr);
}

/******************************************************************************
 * FUNCTION PURPOSE: allocate heap memory
 ******************************************************************************
 * DESCRIPTION:
 *
 *  tword *bf_mem_heap_alloc (
 *    bfMemHeap_t *ptr,
 *    tint size)          - channel number (1 to SIU_MAX_CHANNELS)
 *
 *****************************************************************************/

tword *bf_mem_heap_alloc (bfMemHeap_t *ptr, tint size)
{
  tword *alloc = NULL;

  if ((ptr->indx + size) <= ptr->size) {
    alloc = &ptr->base[ptr->indx];
    ptr->indx += size;
  }
  return (alloc);
}

void *bf_mem_alloc_align (tint size, tint mclass, tint linAlign)
{
  void *allocPtr;
  bfMemHeap_t *ptr;

  if (mclass == ecomem_CLASS_INTERNAL) {
    ptr = &bfIntHeap;
  }
  else if (mclass == ecomem_CLASS_EXTERNAL) {
    ptr = &bfExtHeap;
  }
  else {
    printf("Unrecognized memory class, exiting\n");
    exit (0);
  }

  if ((allocPtr = (void *) bf_mem_heap_alloc (ptr, size + linAlign - 1)) == NULL) {
    printf("Not enough heap, exiting\n");
    exit (0);
  }

  return (bf_mem_align (allocPtr, linAlign));
}

/******************************************************************************
 * FUNCTION PURPOSE: Allocate buffers for bf instance
 *
 *****************************************************************************/
tint siu_alloc_bf_buffs(const ecomemBuffer_t *bufs_req, ecomemBuffer_t *bufs_aloc,
                         tint num_bufs)
{
  tint err_code;
  tint i;
  tuint  linAlign, log2align;

  err_code = bf_NOERR;
  for (i = 0; i < num_bufs; i++) {
    log2align = bufs_req[i].log2align;
    linAlign = (tuint) 1 << log2align;

    bufs_aloc[i].mclass    = bufs_req[i].mclass;   /* internal memory for all  */
    bufs_aloc[i].log2align = bufs_req[i].log2align;/* meet alignment requirement */
    bufs_aloc[i].volat     = FALSE;                /* initialized to permanent */
    bufs_aloc[i].size      = bufs_req[i].size;     /* size of the buffer */
    bufs_aloc[i].base      = bf_mem_alloc_align (bufs_aloc[i].size, bufs_aloc[i].mclass, linAlign);
  }

  bufs_aloc[3].volat     = TRUE;                /* initialized to permanent */
  bufs_aloc[4].volat     = TRUE;                /* initialized to permanent */

  return(err_code);
} /* siu_alloc_bf_buffs */

/******************************************************************************
 * FUNCTION PURPOSE: Allocate buffers for drc instance
 *
 *****************************************************************************/
tint siu_alloc_drc_buffs(const ecomemBuffer_t *bufs_req, ecomemBuffer_t *bufs_aloc,
                         tint num_bufs)
{
  tint err_code;
  tint i;
  tuint  linAlign, log2align;

  err_code = bf_NOERR;
  for (i = 0; i < num_bufs; i++) {
    log2align = bufs_req[i].log2align;
    linAlign = (tuint) 1 << log2align;

    bufs_aloc[i].mclass    = bufs_req[i].mclass;   /* internal memory for all  */
    bufs_aloc[i].log2align = bufs_req[i].log2align;/* meet alignment requirement */
    bufs_aloc[i].volat     = bufs_req[i].volat;    /* initialized to permanent */
    bufs_aloc[i].size      = bufs_req[i].size;     /* size of the buffer */
    bufs_aloc[i].base      = bf_mem_alloc_align (bufs_aloc[i].size, bufs_aloc[i].mclass, linAlign);
  }

  return(err_code);
} /* siu_alloc_drc_buffs */

Fract spchLevdB = frct_MAX, noiseLevdB = frct_MAX;
UFract Nthresh = frct_MAX;

void bf_init(void)
{
  tint  i;

    // i2sDmaReadBufLeft (I2S2 left) - mic1 */
    // i2sDmaReadBufRight (I2S2 right) - mic2 */
    // i2sDmaReadBufLeft2 (I2S3 left) - mic4 */
    // i2sDmaReadBufRight2 (I2S3 right) - mic3 */
    // i2sDmaReadBufLeft3 (I2S0 left) - mic5 */
    // i2sDmaReadBufRight3 (I2S0 right) - mic6 */
    // set up the input frame pointers
    inFramePtr[0] = (int *)(&i2sDmaReadBufLeft[0]);
    inFramePtr[1] = (int *)(&i2sDmaReadBufRight[0]);
#ifdef INSTANCE3_I2S
    inFramePtr[2] = (int *)(&i2sDmaReadBufRight2[0]);
    inFramePtr[3] = (int *)(&i2sDmaReadBufLeft2[0]);
#endif
#ifdef INSTANCE0_I2S
    inFramePtr[2] = (int *)(&i2sDmaReadBufLeft3[0]);
    inFramePtr[3] = (int *)(&i2sDmaReadBufRight3[0]);
#endif

  for (i=0; i<NUM_BF_VIRTUAL_MIC; i++)
  {
    bfInst[i] = 0;
    asnrInst[i] = 0;
    buf_Inptr[i] = (linSample *)sigInBuffer[i];
  }
  buf_Outptr = (linSample *)NULL;

  bf_mem_init_heap();
}

void bf_setup(tint virMicNum)
{
  tint  i, err_code;
  bfConfig_t bfCfg;
  Fract *filterPtr;

  /* Get buffer size for BF */
  err_code = bfGetSizes(&num_bufs_req_by_bf, &bufs_req_by_bf, &vpeSim->bf_size_cfg);
  if (err_code != bf_NOERR) {
    printf("Error when calling bfGetSizes() with error code %d!\n", err_code);
    exit(1);
  }

  printf("bf required buffers:\n");
  siu_print_buff_usage(bufs_req_by_bf, num_bufs_req_by_bf);

  /* Check if there is enough memory allocated for bf */
  if (num_bufs_req_by_bf > MAX_NUM_BF_BUFFS) {
    printf("Memory allocated by SIU is not enough for bf!\n");
    exit(0);
  }

  /* Provide memory to bf through bfNew() */
  err_code = siu_alloc_bf_buffs(bufs_req_by_bf, bufs_aloc_for_bf,
                               num_bufs_req_by_bf);
  if (err_code != bf_NOERR) {
    printf("Error when allocating buffers for bf with error code %d!\n", err_code);
    exit(1);
  }
  printf("Buffers allocated by SIU for bf:\n");
  siu_print_buff_usage(bufs_aloc_for_bf, num_bufs_req_by_bf);

  /* Set up configurations for bfNew() */
  bf_new_cfg.handle  = (void *) siuMakeID(SIU_MID_BF, virMicNum);
  bf_new_cfg.sizeCfg = vpeSim->bf_size_cfg;

  /* Create a new BF instance */
  err_code = bfNew (&bfInst[virMicNum], num_bufs_req_by_bf, bufs_aloc_for_bf, &bf_new_cfg);
  if (err_code != bf_NOERR) {
    printf("Error when calling bfNew() with error code %d!\n", err_code);
    exit(1);
  }

  /* Open the BF instance */
  bfCfg.sampling_rate = vpeSim->bf_size_cfg.max_sampling_rate;
  bfCfg.num_mics = vpeSim->bf_size_cfg.max_num_mics;

  /* Call bf API to open the bf instance */
  err_code = bfOpen (bfInst[virMicNum], &bfCfg);
  if (err_code != bf_NOERR) {
    printf("Error when calling bfOpen()with error code %d!\n", err_code);
    exit(1);
  }

  /* Put in filter coefficients for BF */
  if (bfCfg.num_mics==6)
  {
    filterPtr = sysBfFilters[fltIdx[0][virMicNum]];
  } else if (bfCfg.num_mics==4)
  {
    filterPtr = sysBfFilters[fltIdx[1][virMicNum]];
  } else if (bfCfg.num_mics==2)
  {
    filterPtr = sysBfFilters[fltIdx[2][virMicNum]];
  } else
  {
    printf("Error incorrect virtual mics(%d)!\n", bfCfg.num_mics);
    exit(1);
  }

  for (i=0; i<bfCfg.num_mics; i++)
  {
    bfPutFilter(bfInst[virMicNum], &filterPtr[SYS_BF_FILTER_LENGTH*i], bf_FG_BF, i, SYS_BF_FILTER_LENGTH);
  }

  /* Enable the BF instance */
  err_code = bfControl(bfInst[virMicNum], &vpeSim->bf_ctl);
  if (err_code != bf_NOERR) {
    printf("Error when configure BF with error code %d!\n", err_code);
    exit(1);
  }
}

void bf_closedown(tint virMicNum)
{
  tint err_code,i;
  ecomemBuffer_t bufs[MAX_NUM_BF_BUFFS];

  /* Close the BF instance */
  err_code = bfClose(bfInst[virMicNum]); /* the instance state is set to bf_CLOSED */
  if (err_code != bf_NOERR) {
    printf("Error when calling bfClose() with error code %d!\n", err_code);
    exit(1);
  }

  err_code = bfDelete (&bfInst[virMicNum], MAX_NUM_BF_BUFFS, bufs);
  if (err_code != bf_NOERR) {
    printf("Error when calling bfDelete() with error code %d!\n", err_code);
    exit(1);
  }

  for(i=0;i<MAX_NUM_BF_BUFFS;i++)
  {
    free(bufs[i].base);
  }
}

void asnr_setup(tint virMicNum)
{
  const ecomemBuffer_t *bufs_req_by_nr;
  ecomemBuffer_t        bufs_alloc_for_nr[VPE_NR_NUM_BUFS];
  asnrSizeConfig_t        nr_size_cfg;
  asnrNewConfig_t         nr_new_cfg;
  asnrOpenConfig_t        nr_open_cfg;
  tint                  num_bufs_req_by_nr, status;
  int i;
  tint srate;
  asnrControl_t nr_control_cfg;

  if (vpeSim->Fs == VPE_SIM_SAMPLE_FREQ_8000) {
    srate = asnr_SRATE_8K;
  }
  else {
    srate = asnr_SRATE_16K;
  }

  /* Get NR memory buffer requirements */
  nr_size_cfg.max_sampling_rate=srate;
  status = asnrGetSizes(&num_bufs_req_by_nr, &bufs_req_by_nr, &nr_size_cfg);
  if(status != asnr_NOERR) {
    printf("Error returned by asnrGetSizes() = %d\n",status);
    exit(0);
  }

  if(num_bufs_req_by_nr != VPE_NR_NUM_BUFS) {
    printf("NR needs %d buffers, but %d buffers are allocated!\n",
            num_bufs_req_by_nr, VPE_NR_NUM_BUFS);
    exit(0);
  }

  printf("Buffers requested by NR:\n");
  printf("    Buffer    Size(twords)    Alignment    Volatile\n");
  for(i=0; i<num_bufs_req_by_nr; i++) {
    printf("     %3d       %7d         %4d       ",
                 i, (int)bufs_req_by_nr[i].size, (int)bufs_req_by_nr[i].log2align);
    if(bufs_req_by_nr[i].volat)
      printf("    yes\n");
    else
      printf("    no \n");
  }

  /* Statically allocate buffers for NR */
  for (i=0; i<num_bufs_req_by_nr; i++) {
    bufs_alloc_for_nr[i].mclass = ecomem_CLASS_INTERNAL;
    bufs_alloc_for_nr[i].volat  = FALSE;         /* initialized to permanent */
  }
  bufs_alloc_for_nr[0].base      = &nr_buff0[virMicNum][0];
  bufs_alloc_for_nr[0].size      = NR_SIM_BUF0_SIZE;
  bufs_alloc_for_nr[0].log2align = NR_SIM_BUF0_ALGN_LOG2;

  bufs_alloc_for_nr[1].base      = &nr_buff1[virMicNum][0];
  bufs_alloc_for_nr[1].size      = NR_SIM_BUF1_SIZE;
  bufs_alloc_for_nr[1].log2align = NR_SIM_BUF1_ALGN_LOG2;
  bufs_alloc_for_nr[1].volat     = TRUE;

  bufs_alloc_for_nr[2].base      = &nr_buff2[virMicNum][0];
  bufs_alloc_for_nr[2].size      = NR_SIM_BUF2_SIZE;
  bufs_alloc_for_nr[2].log2align = NR_SIM_BUF2_ALGN_LOG2;
  bufs_alloc_for_nr[2].volat     = TRUE;

  bufs_alloc_for_nr[3].base      = &nr_buff3[virMicNum][0];
  bufs_alloc_for_nr[3].size      = NR_SIM_BUF3_SIZE;
  bufs_alloc_for_nr[3].log2align = NR_SIM_BUF3_ALGN_LOG2;
  bufs_alloc_for_nr[3].volat     = TRUE;

  bufs_alloc_for_nr[4].base      = &nr_buff4[virMicNum][0];
  bufs_alloc_for_nr[4].size      = NR_SIM_BUF4_SIZE;
  bufs_alloc_for_nr[4].log2align = NR_SIM_BUF4_ALGN_LOG2;

  bufs_alloc_for_nr[5].base      = &nr_buff5[virMicNum][0];
  bufs_alloc_for_nr[5].size      = NR_SIM_BUF5_SIZE;
  bufs_alloc_for_nr[5].log2align = NR_SIM_BUF5_ALGN_LOG2;

  bufs_alloc_for_nr[6].base      = &nr_buff6[virMicNum][0];
  bufs_alloc_for_nr[6].size      = NR_SIM_BUF6_SIZE;
  bufs_alloc_for_nr[6].log2align = NR_SIM_BUF6_ALGN_LOG2;

  /* Create NR instance */
  nr_new_cfg.handle  = &nr_handle[virMicNum];
  nr_new_cfg.sizeCfg = nr_size_cfg;
  status = asnrNew(&asnrInst[virMicNum], num_bufs_req_by_nr, bufs_alloc_for_nr, &nr_new_cfg);
  if (status != asnr_NOERR) {
    printf("Error returned by asnrNew() = %d\n",status);
    exit(0);
  }

  printf("Buffers allocated for NR:\n");
  printf("    Buffer    Size(twords)    Alignment    Volatile   address\n");
  for(i=0; i<num_bufs_req_by_nr; i++) {
    printf("     %3d       %7d         %4d       ",
                 i, (int)bufs_alloc_for_nr[i].size, (int)bufs_alloc_for_nr[i].log2align);
    if(bufs_alloc_for_nr[i].volat)
      printf("    yes   ");
    else
      printf("    no    ");

    printf("    0x%x\n", bufs_alloc_for_nr[i].base);
  }

  /* Open newly created NR instance */
  nr_open_cfg.sampling_rate = srate;
  status = asnrOpen(asnrInst[virMicNum], &nr_open_cfg);
  if (status != asnr_NOERR) {
    printf("Error returned by asnrOpen() = %d!\n", status);
    exit(0);
  }

  /* Configure NR parameters */
  nr_control_cfg.valid_bitfield =   asnr_CTL_VALID_ENABLE
                                  | asnr_CTL_VALID_SAMP_RATE
                                  | asnr_CTL_VALID_DELAY
                                  | asnr_CTL_VALID_BAND_BIN1
                                  | asnr_CTL_VALID_BAND_BIN2
                                  | asnr_CTL_VALID_BAND1_MAX_ATTEN
                                  | asnr_CTL_VALID_BAND2_MAX_ATTEN
                                  | asnr_CTL_VALID_BAND3_MAX_ATTEN
                                  | asnr_CTL_VALID_SIG_UPD_RATE_MIN
                                  | asnr_CTL_VALID_SIG_UPD_RATE_MAX
                                  | asnr_CTL_VALID_NOISE_THRESH
                                  | asnr_CTL_VALID_NOISE_HANGOVER;
  nr_control_cfg.enable          = 1;
  nr_control_cfg.samp_rate       = vpeSim->asnrParam.samp_rate;
  nr_control_cfg.delay           = vpeSim->asnrParam.delay;
  nr_control_cfg.band_bin1       = vpeSim->asnrParam.band_bin1;
  nr_control_cfg.band_bin2       = vpeSim->asnrParam.band_bin2;
  nr_control_cfg.band1_max_atten = vpeSim->asnrParam.band1_max_atten;
  nr_control_cfg.band2_max_atten = vpeSim->asnrParam.band2_max_atten;
  nr_control_cfg.band3_max_atten = vpeSim->asnrParam.band3_max_atten;
  nr_control_cfg.sig_upd_rate_max= vpeSim->asnrParam.sig_upd_rate_max;
  nr_control_cfg.sig_upd_rate_min= vpeSim->asnrParam.sig_upd_rate_min;
  nr_control_cfg.noise_thresh    = vpeSim->asnrParam.noise_thresh;
  nr_control_cfg.noise_hangover  = vpeSim->asnrParam.noise_hangover;

  status = asnrControl(asnrInst[virMicNum], &nr_control_cfg);
  if (status != asnr_NOERR) {
    printf("Error returned by asnrControl = %d!\n", status);
    exit(0);
  }
}

void asnr_closedown(tint virMicNum)
{
  ecomemBuffer_t bufs_returned_by_nr[VPE_NR_NUM_BUFS];
  tint           status, error;

  status = asnrClose(asnrInst[virMicNum]);
  if(status != asnr_NOERR) {
    printf("Error returned by asnrClose() = %d!\n", status);
    exit(0);
  }

  status = asnrDelete(&asnrInst[virMicNum], VPE_NR_NUM_BUFS, bufs_returned_by_nr);
  if(status != asnr_NOERR) {
    printf("Error returned by asnrDelete() = %d!\n", status);
    exit(0);
  }

  error =    (bufs_returned_by_nr[0].base != nr_buff0[virMicNum])
           | (bufs_returned_by_nr[1].base != nr_buff1[virMicNum])
           | (bufs_returned_by_nr[2].base != nr_buff2[virMicNum])
           | (bufs_returned_by_nr[3].base != nr_buff3[virMicNum])
           | (bufs_returned_by_nr[4].base != nr_buff4[virMicNum])
           | (bufs_returned_by_nr[5].base != nr_buff5[virMicNum]);
///        | (bufs_returned_by_nr[6].base != nr_buff6[virMicNum]);
  if(error) {
    printf("Buffers returned by asnrDelete() are wrong!\n");
    exit(0);
  }
}

void mss_setup(void)
{
  const ecomemBuffer_t *bufs_req_by_mss;
  ecomemBuffer_t        bufs_aloc_by_siu[SIU_NUM_MSS_BUFFS];
  mssSizeConfig_t   mss_size_cfg;
  mssNewConfig_t    mss_new_cfg;
  tint err_code, num_bufs_req_by_mss;
  mssControl_t  mss_ctl_cfg;
  mssConfig_t   mss_open_cfg;

  /* Set up configurations for mssGetSizes() */
  mss_size_cfg.max_sampling_rate  = mss_SAMP_RATE_16K;
  mss_size_cfg.max_num_mic_fixed  = NUM_BF_VIRTUAL_MIC;
  mss_size_cfg.max_num_mic_remote = 0;
  mss_size_cfg.max_num_mic_clean  = 0;
  mss_size_cfg.max_num_beam       = 0;

  /* Call API function mssGetSizes() to get memory requirements:
     - num_bufs_req_by_mss: number of buffers required by MSS
     - bufs_req_by_mss:   pointer to buffer descriptors defined inside MSS */
  err_code = mssGetSizes(&num_bufs_req_by_mss, &bufs_req_by_mss, &mss_size_cfg);

  printf("MSS required buffers:\n");
  siu_print_buff_usage(bufs_req_by_mss, num_bufs_req_by_mss);

  /* Check if there is enough memory allocated for MSS */
  if(   num_bufs_req_by_mss > SIU_NUM_MSS_BUFFS
     || bufs_req_by_mss[0].size > sizeof(mssbuf_t)) {
    printf("Memory allocated by SIU is not enough for MSS!\n");
    exit(0);
  }

  /* Provide memory to MSS through mssNew() */
  bufs_aloc_by_siu[0].mclass    = ecomem_CLASS_INTERNAL;
  bufs_aloc_by_siu[0].log2align = bufs_req_by_mss[0].log2align;
  bufs_aloc_by_siu[0].volat     = FALSE;
  bufs_aloc_by_siu[0].size      = sizeof(mssbuf_t);
  bufs_aloc_by_siu[0].base      = &mss_buf;

  printf("Buffers allocated by SIU for MSS:\n");
  siu_print_buff_usage(bufs_aloc_by_siu, num_bufs_req_by_mss);

  /* Set up configurations for mssNew() */
  mss_new_cfg.handle  = (void *) siuMakeID(SIU_MID_MSS, 0);
  mss_new_cfg.sizeCfg = mss_size_cfg;

  /* Call API function mssNew() to create an MSS instance:
     - pass buffer descriptors that are set by SIU
     - pass number of buffer descriptors   */
  err_code = mssNew (&mssInst, num_bufs_req_by_mss, bufs_aloc_by_siu, &mss_new_cfg);
  if (err_code != mss_NOERR) {
    printf("Error when calling mssNew() with error code %d!\n", err_code);
    exit(1);
  }

  /* Set up configuration for mssOpen() */
  mss_open_cfg.sampling_rate  = (vpeSim->asnrParam.samp_rate==1) ? mss_SAMP_RATE_16K : mss_SAMP_RATE_8K;
  mss_open_cfg.num_mic_fixed  = NUM_BF_VIRTUAL_MIC;
  mss_open_cfg.num_mic_remote = 0;
  mss_open_cfg.num_mic_clean  = 0;
  mss_open_cfg.num_mic_array  = 0;
  mss_open_cfg.num_beam       = 0;

  err_code = mssOpen (mssInst, &mss_open_cfg);
  if (err_code != mss_NOERR) {
    printf("Error when calling mssOpen() with error code %d!\n", err_code);
    exit(1);
  }

  /* Set 1st hands-free mic as the selected source for MSS */
  mss_ctl_cfg.valid_bitfield = vpeSim->mss_ctl_cfg.valid_bitfield;                //
  mss_ctl_cfg.modes          = vpeSim->mss_ctl_cfg.modes;
  mss_ctl_cfg.source.group   = vpeSim->mss_ctl_cfg.source.group;
  mss_ctl_cfg.source.index   = vpeSim->mss_ctl_cfg.source.index;

  mss_ctl_cfg.switch_thresh   = vpeSim->mss_ctl_cfg.switch_thresh;
  mss_ctl_cfg.switch_duration = vpeSim->mss_ctl_cfg.switch_duration;
  mss_ctl_cfg.switch_hangover = vpeSim->mss_ctl_cfg.switch_hangover;
  err_code = mssControl (mssInst, &mss_ctl_cfg);

  if (err_code != mss_NOERR) {
    printf("Error when calling mssControl() to configure MSS with error code %d!\n",
    err_code);
    exit(1);
  }
}

void mss_closedown(void)
{
  ecomemBuffer_t bufs_returned_by_siu[SIU_NUM_MSS_BUFFS];
  tint err_code, error;

  err_code = mssClose(mssInst);
  if (err_code != mss_NOERR) {
    printf("Error when calling mssClose() with error code %d!\n", err_code);
    exit(1);
  }

  err_code = mssDelete(&mssInst, SIU_NUM_MSS_BUFFS, bufs_returned_by_siu);
  if (err_code != mss_NOERR) {
    printf("Error when calling mssDelete() with error code %d!\n", err_code);
    exit(1);
  }

  error = (bufs_returned_by_siu[0].base != &mss_buf);
  if(error) {
    printf("Buffers returned by mssDelete() are wrong!\n");
    exit(0);
  }
}

/*=================================================================
 *  void drc_setup(void)     DRC Setup
 *=================================================================*/
void drc_setup(void)
{
  int   err;
  const ecomemBuffer_t *bufs_req_by_drc;
  ecomemBuffer_t        bufs_alloc_for_drc[SIU_NUM_DRC_BUFFS];
  tint                  num_bufs_req_by_drc;
  drcNewConfig_t drcNewConfig;
  drcControl_t   drcCtl;

  drcCreateConfig_t drcCreateConfig = {sysDbgInfo}; /* pointer to a dummy debug info (not used in DRC) */

  printf("...Initializing DRC\n");

  /* Create DRC Context */
  err = drcCreate (&drcCreateConfig);
  if (err != drc_NOERR) {
    printf("*** drcCreate() error: %d\n", err);
  }

  /* Configure DRC */
  err = drcGetSizes(&num_bufs_req_by_drc, &bufs_req_by_drc, &drcSizeConfig);
  if (err != drc_NOERR) {
    printf("*** drcGetSizes error: %d\n", err);
  }

  /* do we have enough buffers for DRC */
  if (num_bufs_req_by_drc > SIU_NUM_DRC_BUFFS) {
    printf("*** not enough buffer descriptors");
  }

  drcNewConfig.sizeCfg = drcSizeConfig;

  /* Provide memory buffers to drc */
  err = siu_alloc_drc_buffs(bufs_req_by_drc, bufs_alloc_for_drc,
                               num_bufs_req_by_drc);
  if (err != bf_NOERR) {
    printf("Error when allocating buffers for drc with error code %d!\n", err);
    exit(1);
  }
  printf("Buffers allocated by SIU for drc:\n");
  siu_print_buff_usage(bufs_alloc_for_drc, num_bufs_req_by_drc);

  /* Give memory to DRC */
  drcNewConfig.handle = (void*)0;     /* Indicate instance #0 */
  drcInst = NULL;
  err = drcNew(&drcInst, num_bufs_req_by_drc, bufs_alloc_for_drc, &drcNewConfig);
  if (err != drc_NOERR) {
    printf("*** drcNew() error: %d\n", err);
  }

  /* Open DRC for business */
  err = drcOpen(drcInst, &drcOpenConfig);
  if (err != drc_NOERR) {
    printf("*** drcOpen() error: %d\n", err);
  }
  /* At this point DRC is open, but DISABLED! */
  /* We need to do a few additional configurations through drcControl() */

  /* Set the full band companding curve */
  drcCtl.ctl_code = drc_CTL_SET_FBAND;
  drcCtl.u.band.valid_bitfield    = (1u<<drc_BAND_CFG_VALID_CURVE_PARAM_BIT);
  drcCtl.u.band.curve.exp_knee    = vpeSimC.drc_Param.exp_knee;    /* in dBm0! */
  drcCtl.u.band.curve.ratios      = frctAdjustQ(2,0,4) | (frctAdjustQ(2,0,4) << 8);
  drcCtl.u.band.curve.max_amp     = vpeSimC.drc_Param.max_amp;     /* in dB */
  drcCtl.u.band.curve.com_knee    = -15;    /* -15dBm0 */
  drcCtl.u.band.curve.energy_lim  = -3;     /* -3dBm0 */
  err = drcControl (drcInst, &drcCtl);
  if (err != drc_NOERR) {
    printf("*** drcControl() config full band error: %d\n", err);
  }

  /* Set the limiter configuration */
  drcCtl.ctl_code = drc_CTL_SET_LIM;
  drcCtl.u.limiter.valid_bitfield = (1u<<drc_LIM_CFG_VALID_THRESH_BIT) |
                                    (1u<<drc_LIM_CFG_VALID_DELAY_BIT);
  drcCtl.u.limiter.thresh_dBm = -3;       /* -3dBm0 */
  drcCtl.u.limiter.delay_len  = (8*drc_SAMP_RATE_16K);    /* just the current value! */
  err = drcControl (drcInst, &drcCtl);
  if (err != drc_NOERR) {
    printf("*** drcControl() config limiter error: %d\n", err);
  }

  /* Turn DRC on */
  if (vpeSimC.drc_on) {
    drcCtl.ctl_code = drc_CTL_DRC_ON;
    err = drcControl (drcInst, &drcCtl);
    if (err != drc_NOERR) {
      printf("*** drcControl() enable error: %d\n", err);
    }
  }

  printf("Done with DRC\n");

} /* drc_setup */

/*-----------------------------------------------------------------
 * Filename:  main
 *                            
 * Description: Test the vpe API functions.
 *-----------------------------------------------------------------*/
void bf_main (void)
{
  int i;
  volatile tulong cycles;       /* for profiling */

#ifdef PROFILE_CYCLES
  // Open GPT 0 for profiling
  hGpt = GPT_open (GPT_2, &gptObj, &status);
  if((NULL == hGpt) || (CSL_SOK != status))
  {
    printf("GPT Open Failed\n");
  }
  else
  {
    printf("GPT Open Successful\n");
  }

  /* Reset the GPT module */
  status = GPT_reset(hGpt);
  if(CSL_SOK != status)
  {
    printf("GPT Reset Failed\n");
  }
  else
  {
    printf("GPT Reset Successful\n");
  }

  /* Configure GPT module */
  hwConfig.autoLoad      = GPT_AUTO_ENABLE;
  hwConfig.ctrlTim   = GPT_TIMER_ENABLE;
  hwConfig.preScaleDiv = GPT_PRE_SC_DIV_0; // Pre scale Divide input clock by 2
  hwConfig.prdLow    = 0xFFFF;
  hwConfig.prdHigh   = 0xFFFF;

  status =  GPT_config(hGpt, &hwConfig);
  if(CSL_SOK != status)
  {
    printf("GPT Config Failed\n");
  }
  else
  {
    printf("GPT Config Successful\n");
  }

  // measure CPU cycles by timer
  /* Start the Timer */
  status = GPT_start(hGpt);
  if(CSL_SOK != status)
  {
    printf("GPT Start Failed\n");
  }

  /* compute the Timer Count overhead*/
  status = GPT_getCnt(hGpt, &timeCnt1);
  status = GPT_getCnt(hGpt, &timeCnt2);
  vpeSim->cyclesOverhead = timeCnt2 - timeCnt1;
#endif

  /* BF initialization */
  bf_init();

  if (vpeSim->bf_on)
  {
    /* setup BF */
    for (i=0; i<NUM_BF_VIRTUAL_MIC; i++)
        bf_setup(i);
  }

  if (vpeSim->nr_on)
  {
    /* setup VPE for ASNR */
    for (i=0; i<NUM_BF_VIRTUAL_MIC; i++)
        asnr_setup(i);
  }

  if (vpeSim->mss_on)
  {
    /* setup MSS */
    mss_setup();
  }

  if (vpeSim->drc_on)
  {
    /* setup DRC */
    drc_setup();
  }
} /* bf_setup */

tbool bf_process(void)
{
    tuint num_tuint_in, num_tuint_out;
    tint  i, j, retVal;
    volatile tulong cycles;       /* for profiling */
    mssDebugStat_t  mssDbg;

    // assign the number of samples in 16 bit word
    num_tuint_out = I2S_DMA_BUF_LEN;
    num_tuint_in = I2S_DMA_BUF_LEN;

    if (vpeSim->bf_on)
    {
#ifdef PROFILE_CYCLES
      status = GPT_getCnt(hGpt, &timeCnt1);;
#endif
      /* BF processing the currenct frame  for each virtual mic */
      for (i=0; i<NUM_BF_VIRTUAL_MIC; i++)
      {
        retVal = bfProcess(bfInst[i], (void **)inFramePtr, buf_Inptr[i]);
        if (retVal!=bf_NOERR)
        {
          printf("Error returned by bfProcess() = %d!\n", retVal);
          exit(0);
        }
      }
#ifdef PROFILE_CYCLES
      status = GPT_getCnt(hGpt, &timeCnt2);
      siu_process_cycles((timeCnt1-timeCnt2)<<(hwConfig.preScaleDiv+1), &vpeSim->cyclesBf);
#endif
    } else
    {
        /* copy analog mics instead */
        for (j=0; j<NUM_BF_ANALOG_MIC; j++)
        {
          for (i=0;i<num_tuint_in;i++)
            buf_Inptr[j][i] = inFramePtr[j][i];
        }
    }

    if(vpeSim->nr_on) {
#ifdef PROFILE_CYCLES
        status = GPT_getCnt(hGpt, &timeCnt1);;
#endif    /* BF processing the currenct frame  for each virtual mic */
        /* Run NR if enabled */
        for (i=0; i<NUM_BF_VIRTUAL_MIC; i++)
        {
          retVal = asnrProcess(asnrInst[i], buf_Inptr[i], buf_Inptr[i]);
          if (retVal != asnr_NOERR) {
            printf("Error returned by asnrProcessn() = %d!\n", retVal);
            exit(0);
          }
        }
#ifdef PROFILE_CYCLES
        status = GPT_getCnt(hGpt, &timeCnt2);
        siu_process_cycles((timeCnt1-timeCnt2)<<(hwConfig.preScaleDiv+1), &vpeSim->cyclesAsnr);
#endif
    }

    if (vpeSim->mss_on)
    {
      /* MSS processing here */
      /* prepare for MSS processing - only one soure for the case of mic-array */
      for (i=0; i<NUM_BF_VIRTUAL_MIC; i++)
        beam_mics[i] = (void *)&buf_Inptr[i][0];

      if (vpeSim->drc_on)
      {
#ifdef PROFILE_CYCLES
      status = GPT_getCnt(hGpt, &timeCnt1);
#endif    /* BF processing the currenct frame  for each virtual mic */
        /* multi-source selection module to select the optimum source (mic) */
        retVal = mssProcess(mssInst, buf_Drcptr, data_rx_sync, beam_mics, NULL, NULL, NULL, NULL);
#ifdef PROFILE_CYCLES
      status = GPT_getCnt(hGpt, &timeCnt2);
      siu_process_cycles((timeCnt1-timeCnt2)<<(hwConfig.preScaleDiv+1), &vpeSim->cyclesMss);
#endif
        if (retVal != mss_NOERR) {
           printf("Error returned by mssProcess() = %d!\n", retVal);
           exit(0);
        }
#ifdef PROFILE_CYCLES
      status = GPT_getCnt(hGpt, &timeCnt1);;
#endif
        // run DRC
        retVal = drcProcess(drcInst, buf_Drcptr,       /* instance and input frame pointers */
                                    buf_Outptr);      /* pointer to output buffer pointer */
#ifdef PROFILE_CYCLES
      status = GPT_getCnt(hGpt, &timeCnt2);
      siu_process_cycles((timeCnt1-timeCnt2)<<(hwConfig.preScaleDiv+1), &vpeSim->cyclesDrc);
#endif
        if (retVal != drc_NOERR) {
           printf("Error returned by drcProcess() = %d!\n", retVal);
           exit(0);
        }
      } else
      {
#ifdef PROFILE_CYCLES
      status = GPT_getCnt(hGpt, &timeCnt1);
#endif    /* BF processing the currenct frame  for each virtual mic */
        /* multi-source selection module to select the optimum source (mic) */
        retVal = mssProcess(mssInst, buf_Outptr, data_rx_sync, beam_mics, NULL, NULL, NULL, NULL);
#ifdef PROFILE_CYCLES
      status = GPT_getCnt(hGpt, &timeCnt2);
      siu_process_cycles((timeCnt1-timeCnt2)<<(hwConfig.preScaleDiv+1), &vpeSim->cyclesMss);
#endif
        if (retVal != mss_NOERR) {
           printf("Error returned by mssProcess() = %d!\n", retVal);
           exit(0);
        }
      }
      /* Trace source selection */
      /*    Write Args:
       *      arg2: (value) Angle in degrees
       *      arg3: (aux1)  0 - current source, 1 - new source
       *      arg4: (aux2)  source index
       */
      retVal = mssDebugStat(mssInst, &mssDbg);
        if (retVal !=mss_NOERR) {
            printf("Error returned by mssDebugStat() = %d!\n", retVal);
            exit(0);
        }
        /* mssDbg.cur_src.group/.index has the current source */
        /* mssDbg.new_src.group/.index has "proposed" source */
        if (mssDbg.cur_src.group != mssDbgCurSrc.group ||
            mssDbg.cur_src.index != mssDbgCurSrc.index)
        {
          mssDbgCurSrc = mssDbg.cur_src;
          g_curSRC = mssDbg.cur_src.index;  //variable to observe g_curSRC. Put in CCS watch expression with continuous refresh
        }
        if (mssDbg.new_src.group != mssDbgNewSrc.group ||
            mssDbg.new_src.index != mssDbgNewSrc.index)
        {
          mssDbgNewSrc = mssDbg.new_src;
          g_newSRC = mssDbg.new_src.index; //variable to observe g_newSRC. Put in CCS watch expression with continuous refresh
        }

      /* copy to the virtual mic 2 to right channel */
      for (i=0;i<num_tuint_out;i++){
        buf_Outptr2[i] = inFramePtr[1][i];
      }
    } else
    {
      /* copy to the virtual mic 1 & 2 to output left and righ channels */
      for (i=0;i<num_tuint_out;i++){
        buf_Outptr[i] = buf_Inptr[0][i];
        buf_Outptr2[i] = buf_Inptr[1][i];
      }
    }

    return 0;
}
/* Nothing past this point */
