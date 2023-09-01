/******************************************************************************
 * FILE PURPOSE: 
 *               
 ******************************************************************************
 * FILE NAME:   vausim.h
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
#ifndef _VAUSIM_H
#define _VAUSIM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xdc/std.h>

#include <ti/mas/types/types.h>
#include <ti/mas/util/utl.h>
#include <ti/mas/util/debug.h>
#include <ti/mas/iface/ifvau/ifvau.h>
#include <ti/mas/vau/vau.h>
#include <ti/mas/vau/Version.h>
#include <ti/xdais/dm/isphenc1.h>
#include <ti/xdais/ialg.h>
#include <ti/mas/vau/test/src/mem.h>

//#define DBG
		
#ifdef DBG
extern tlong gVauEnergy;
extern tlong gVauThreshold;
#endif

#ifdef USE_CACHE
#include <ti/mas/sdk/memArchCfg.h>
#endif

#ifdef PROFILE_CYCLES
#include <ti/mas/sdk/profile.h>
#endif

#define SIU_MAX_MODULE_NBUFS 100 /**< required for ECO API memory descriptors, not for XDAIS APIs */
#define MAX_FRAME_SIZE 80       /**< 10 ms frame */
    
/** @brief global test configuration structure definition
 */
typedef struct config_s {
  tbool     xdaisFlag; /**< 1 if desired to exercise xdais else ECO interface for components */

  tbool     xdaisDefaultCfgFlag; /**< 1 if desired to use default configuration for XDAIS case,

                                    applicable only when flag xdaisFlag is 1 */

  tint      frameSize;          /**< in 8 KHz samples, only 5 ms (=40) or 10 ms (=80)
                                   allowed */
  tint      samp_rate_8k_multiple; /**< sampling rate, as multiples of 8 KHz, e.g for 16 KHz, set to 2 */                                   

  ifvauConfig_t vauConfig; /**< VAU (setup time) configuration, see ifvau external interface */
  ifvauControl_t vauControl; /**< VAU control, see ifvau external interface */
#ifdef USE_CACHE
  tint  doCacheFlush;
  tint  doCacheEnable;
#endif
} config_t;

/** @brief for accumulation of cycle statistics */
typedef struct cycles_s {
  tulong     max; /**< running maximum of cycles */
  tulong     avg; /**< running average of cycles (using exponential averaging) */
} cycles_t;

/** @brief System Integration Unit (siu) structure for book-keeping of modules
 */
typedef struct siuInstVau_s {
  void     *vauInst;     /**< vau's ECO instance */
  IALG_Handle vauXdaisInst; /**< vau's XDAIS instance */
  tlong     frameCount;
  ISPHENC1_FrameType vauFrameTypePrev;
  tbool    vauAlertFlagPrev;
  FILE     *outVauDecisionFilePtr;

#ifdef PROFILE_CYCLES
  FILE   *outCyclesFilePtr;
#endif
  
#ifdef DBG
  FILE     *vauDbgLogFilePtr;
#endif  
  tlong     cyclesOverhead; /**< to measure fixed overhead in cycle measurement */
  cycles_t  cyclesVau; /**< Vau's cycle statistics */
  tulong   xdaisBufsMallocFreeCounter; /**< to see if malloc and free for module memory allocation in
                                        *   XDAIS are same i.e this should be zero 
                                        *   at the end of the test */
} siuInstVau_t;

/** @brief Configuration structure containing all configuration of the test. This can be set 
  * manually after going to "main" and before letting run to completion.
  */
extern config_t  config;

extern siuInstVau_t siuInstVau;

extern void siu_open(void);

void siu_dealloc_vau_xdais_mem (IALG_Fxns *ialgFxnsPtr, IALG_Handle xdaisInstPtr, 
                                tulong *mallocedSizeCounter);
void siu_close(void);

void siu_new_open_config_modules(config_t * config);

#ifdef AZ_INFO_AVAILABLE
extern tint glob_Az[];
#endif

void siu_process(linSample * framePtr, linSample ** outFramePtr);

void hw_init(void);

extern linSample inFrame[MAX_FRAME_SIZE], outFrame[MAX_FRAME_SIZE];

#endif /* _VAUSIM_H */

/* Nothing past this point */
