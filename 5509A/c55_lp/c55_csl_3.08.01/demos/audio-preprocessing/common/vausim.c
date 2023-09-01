/**
 *  @file   vausim.c
 *  @brief  VAU test
 *  @remarks This test feeds an input speech file to vau and creates one output file 
 *           of vau decision and another output speech samples going through the vau.
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
 */

/** @defgroup  VAUTEST */

/** @ingroup VAUTEST */
/* @{ */

#include <bf_asnr_mss_vau.h>

siuInstVau_t siuInstVau;

void siu_open(void)
{
  memset(&siuInstVau, 0, sizeof(siuInstVau_t));

  siu_init_heaps();

  if ((siuInstVau.outVauDecisionFilePtr =
    fopen(nmu_SIM_OUTDEC_FNAME, "wt")) == NULL) {
    printf(" Error opening out.raw\n");
    exit(0);
  }

#ifdef DBG
  if ((siuInstVau.vauDbgLogFilePtr =
    fopen(nmu_SIM_LOG_FNAME, "wt")) == NULL) {
    printf(" Error opening log.txt\n");
    exit(0);
  }
#endif  
}

void siu_dealloc_vau_xdais_mem (IALG_Fxns *ialgFxnsPtr, IALG_Handle xdaisInstPtr, 
                                tulong *mallocedSizeCounter)
{
  Int nbufs;
  IALG_MemRec *xdaisBufsPtr;
  
  nbufs = ialgFxnsPtr->algNumAlloc();
  xdaisBufsPtr = (IALG_MemRec *) malloc(nbufs * sizeof(IALG_MemRec));

  nbufs = ialgFxnsPtr->algFree(xdaisInstPtr, xdaisBufsPtr);
  
  siu_dealloc_xdais_mem ((tint) nbufs, xdaisBufsPtr, mallocedSizeCounter);
  
  free(xdaisBufsPtr);
}

void siu_close(void)
{
  fclose(siuInstVau.outVauDecisionFilePtr);
  

#ifdef DBG
  fclose(siuInstVau.vauDbgLogFilePtr);
#endif
  
  if (config.xdaisFlag) {
    siu_dealloc_vau_xdais_mem (&vauXdaisCallTable.ialg, siuInstVau.vauXdaisInst,
                           &siuInstVau.xdaisBufsMallocFreeCounter);
    if (siuInstVau.xdaisBufsMallocFreeCounter) {
      printf(" Error: mismatched malloc-free\n");
    }
  }
  
  memset(&siuInstVau, 0, sizeof(siuInstVau_t));
}

void siu_new_open_config_modules(config_t * config)
{
  tint      nbufs, ret;
  const ecomemBuffer_t *sizeBufs;
  ecomemBuffer_t ecoBufs[SIU_MAX_MODULE_NBUFS];
  const IALG_Params *paramsPtr;
  IALG_MemRec *xdaisBufsPtr;

  vauSizeConfig_t vauSizeCfg;
  vauOpenConfig_t vauOpenCfg;

  vauXdaisInitParams_t vauXdaisInitParams;

  vauXdaisInitParams.param.size = (Int) sizeof(vauXdaisInitParams_t);
                                                        
  printf("Querying VAU Memory Requirements\n");

  vauSizeCfg.max_frame_size = config->frameSize;
  vauSizeCfg.max_sampling_rate_8k_multiple = config->samp_rate_8k_multiple;
  
  if (config->xdaisFlag) {
    nbufs = (tint) vauXdaisCallTable.ialg.algNumAlloc();
    if (nbufs == 0) {
      printf(" algNumAlloc error, exiting\n");
      exit(0);
    }
    
    xdaisBufsPtr = (IALG_MemRec *) malloc(nbufs * sizeof(IALG_MemRec));
    if (xdaisBufsPtr == NULL) {
      printf("Not enough heap for VAU memory records, exiting\n");
      exit(0);    
    }
    if (config->xdaisDefaultCfgFlag) {
      paramsPtr = NULL;
    }
    else {
      /* currently no config supported in vau for alloc, but if introduce in
         future, refer to that configuration */
      paramsPtr = NULL;
    }    
    vauXdaisCallTable.ialg.algAlloc (paramsPtr, NULL, xdaisBufsPtr); 
  }
  else {
    if (vauGetSizes(&nbufs, &sizeBufs, &vauSizeCfg) != vau_NOERR) {
      printf(" Vau get sizes error, exiting \n");
      exit(0);
    }
  }

  printf("Allocating VAU memory..\n");
  
  if (config->xdaisFlag) {
    siu_alloc_xdais_mem (nbufs, xdaisBufsPtr, &siuInstVau.xdaisBufsMallocFreeCounter);
    siuInstVau.vauXdaisInst = (IALG_Handle) xdaisBufsPtr[0].base;
  }
  else {
    siu_alloc_eco_mem(nbufs, sizeBufs, ecoBufs);
  }  

  vauOpenCfg.frame_size = config->frameSize;
  vauOpenCfg.sampling_rate_8k_multiple = config->samp_rate_8k_multiple;
  vauOpenCfg.config = config->vauConfig;

  if (config->xdaisFlag) {
    if (config->xdaisDefaultCfgFlag) {
      paramsPtr = NULL;
    }
    else {
      vauXdaisInitParams.cfgOpen = vauOpenCfg;
      paramsPtr = (IALG_Params *) &vauXdaisInitParams;
    }
    ret = vauXdaisCallTable.ialg.algInit(siuInstVau.vauXdaisInst,
                                   xdaisBufsPtr, NULL, paramsPtr); 

    free(xdaisBufsPtr);

    if (ret == IALG_EFAIL) {
      printf("algInit() failure, exiting\n");
      exit(0);
    }
  }
  else {
    if (vauNew(&(siuInstVau.vauInst), nbufs, ecoBufs) != vau_NOERR) {
      printf(" Vau New error, exiting \n");
      exit(0);
    }
    vauOpen(siuInstVau.vauInst, &vauOpenCfg);
  }
  
  if (config->xdaisFlag) {
    vauXdaisCallTable.control (siuInstVau.vauXdaisInst, &(config->vauConfig), &(config->vauControl));
  }
  else {
    vauControl(siuInstVau.vauInst, &(config->vauConfig), &(config->vauControl));
  }  
}

#ifdef AZ_INFO_AVAILABLE
extern tint glob_Az[];
#endif

void siu_process(linSample * framePtr, linSample ** outFramePtr)
{
  linSample *vauOutSamplesPtr = *outFramePtr;
  ISPHENC1_FrameType vauFrameType;
  tbool     vauAlertFlag = FALSE;

#ifdef USE_CACHE
  if (config.doCacheFlush) {
    memarchcfg_cacheFlush();
  }
#endif

  if (config.xdaisFlag) {
    vauAlertFlag =
    vauXdaisCallTable.process(siuInstVau.vauXdaisInst, framePtr, &vauOutSamplesPtr,
                &vauFrameType);
  }
  else {
    vauAlertFlag =
    vauProcess(siuInstVau.vauInst, framePtr, &vauOutSamplesPtr,
              &vauFrameType);
  }

  /* output when something changes and on first occurrence */
  if ( (siuInstVau.frameCount == 1) || (vauFrameType != siuInstVau.vauFrameTypePrev) ||
       (vauAlertFlag != siuInstVau.vauAlertFlagPrev)) {

    fprintf(siuInstVau.outVauDecisionFilePtr, "%ld, %d, %d\n", (long) siuInstVau.frameCount,
            vauFrameType, vauAlertFlag);
    
    switch (vauFrameType) {
      case ISPHENC1_FTYPE_SPEECH:
        printf(" At frame %ld, %s, %s\n", (long) siuInstVau.frameCount, "SPEECH",
                             (vauAlertFlag == TRUE)? "ALERT ON": "ALERT OFF");       
      break;
      
      case ISPHENC1_FTYPE_NODATA:
        if ((siuInstVau.frameCount > 1) && (siuInstVau.vauAlertFlagPrev == FALSE)) {
          printf(" Noise frame arrived without a previous alert flag on SPEECH, exiting");
          exit(0);
        }
        else {
          printf(" At frame %ld, %s\n", (long) siuInstVau.frameCount, "NOISE");
        }  
      break;
      
      default:
      	printf(" Invalid Frame Type %d\n detected, exiting", vauFrameType);
        exit(0);
      break;    
    }

    siuInstVau.vauFrameTypePrev = vauFrameType;
    siuInstVau.vauAlertFlagPrev = vauAlertFlag;
  }

#ifdef DBG
  fprintf(siuInstVau.vauDbgLogFilePtr, "%ld %ld\n", (long) gVauEnergy, (long) gVauThreshold);
#endif

  *outFramePtr = vauOutSamplesPtr;
}

void hw_init(void)
{
#ifdef PROFILE_CYCLES
  if (!profile_initCycleProfiling()) {
    printf("Could not acquire profiling resources..Exiting\n");
    exit(0);
  }
#endif

#ifdef USE_CACHE
  if (config.doCacheEnable) {
    memarchcfg_cacheEnable();
  }
#endif
}

linSample inFrame[MAX_FRAME_SIZE], outFrame[MAX_FRAME_SIZE];

/* @} */ /* ingroup */

/* Nothing past this point */
