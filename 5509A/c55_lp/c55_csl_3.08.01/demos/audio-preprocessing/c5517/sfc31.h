#ifndef _SFC_H
#define _SFC_H

/** 
 *  @file  sfc31.h
 *  @brief Contains external API for the Sample Frequency Converter (SFC) module.
 *
 *        Copyright (c) 2007 – 2017 Texas Instruments Incorporated
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

 #include <ti/mas/types/types.h>               /* DSP types                   */

/**
 *  
 *  @defgroup SFC Source Frequency Converter (SFC) 
 */
 
/** @ingroup SFC */
/* @{ */

/**
 * @name External APIs for SFC
 *
 */
/* @{ */
/**
 *  @brief Sample frequency converter sfcDecimationBy3 performs decimation by 3.
 *
 *  @param[in]  filt_len      Length of the decimation filter. It MUST be integer
 *                            multiple of 4 plus 1, ie., 4*k+1. This parameter 
 *                            is ignored if parameter coeffs is NULL. 
 *  @param[in]  frame_len_in  Length of the data frame to be processed. 
 *  @param[in]  delay_line    Filter delay line. It must have filt_len-1 samples 
 *                            from previous frames and frame_len_in samples from
 *                            current frame. The samples in this buffer must
 *                            be packed as illustrated below:
 *  @verbatim
        |   previous frames   |   current frame   | 
        x(n-N+1), ..., x(n-M),  x(n-M+1), ..., x(n)
        where N=M+L-1, M is input frame length, L is filter length.
    \endverbatim
 *  @param[in]   coeffs       Left half of the decimation filter coefficients 
 *                            including the center. Total number of coefficients
 *                            provided by user must be (filt_len+1)/2.
 *                            The decimation filter must be a symetric FIR filter
 *                            with length filt_len. The gain of the filter must be 
 *                            less than or equal to 0dB. If this pointer is NULL, 
 *                            the \ref sfc_default_filter will be used.
 *  @param[out]  sig_out      Output signal after decimation. Number of output 
 *                            samples is frame_len_in/2.
 */ 
void sfcDecimationBy3(linSample *delay_line, linSample *sig_out,
                      const Fract *coeffs, tint filt_len, tint frame_len_in);

/**
 *  @brief Sample frequency converter sfcInterpolationBy3 performs
 *         interpolation by 3.
 *
 *  @param[in]  filt_len      Length of the interpolation filter. It MUST be 
 *                            integer multiple of 4 plus 1, ie., 4*k+1. This  
 *                            parameter is ignored if parameter coeffs is NULL. 
 *  @param[in]  frame_len_in  Length of the data frame to be processed. 
 *  @param[in]  delay_line    Filter delay line. It must have (filt_len-1)/2
 *                            samples from previous frames and frame_len_in 
 *                            samples from current frame. The samples in this 
 *                            buffer must be packed as illustrated below:
 *  @verbatim
        |   previous frames   |   current frame   | 
        x(n-N+1), ..., x(n-M),  x(n-M+1), ..., x(n)
        where N=M+(L-1)/2, M is input frame length, L is filter length.
    \endverbatim
 *  @param[in]   coeffs      Left half of the interpolation filter coefficients 
 *                           including the center. Total number of coefficients
 *                           provided by user must be (filt_len+1)/2.
 *                           The interpolation filter must be a symetric FIR filter
 *                           with length filt_len. The gain of the filter must be 
 *                           less than or equal to 0dB. If this pointer is NULL, 
 *                           the \ref sfc_default_filter will be used.
 *  @param[out]  sig_out     Output signal after interpolation. Number of output 
 *                           samples is frame_len_in*2.
 */ 
void sfcInterpolationBy3(linSample *delay_line, linSample *sig_out,
                         const Fract *coeffs, tint filt_len, tint frame_len_in);
/* @} */

/** @defgroup sfc_default_filter SFC Default Filter
 *  
 *  @brief Default decimation/interpolation filter coefficients. This filter 
 *         will be used if the user doesn't provide coefficients for 
 *         decimation/interpolation. 
 *
 *         This filter has cutoff frequency at 0.426 PI, or 3.4kHz for 16kHz 
 *         sampling frequency. It has 59dB out-of-band rejection above 0.548 PI, 
 *         or 4.34kHz for 16kHz sampling frequency. 
 *
 *  @verbatim
 Fract sfc_def_decim_interp_filt[sfc_NUM_DEF_COEFS] = {
        -976,
         -54,
          92,
         182,
         124,
         -38,
        -154,
        -105,
          73,
         216,
         180,
         -14,
        -190,
        -173,
          37,
         249,
         255,
          30,
        -226,
        -262,
         -23,
         282,
         356,
         104,
        -259,
        -383,
        -121,
         310,
         500,
         232,
        -286,
        -565,
        -293,
         333,
         737,
         471,
        -306,
        -898,
        -658,
         349,
        1263,
        1095,
        -320,
       -1927,
       -2046,
         357,
        4691,
        8873,
       10598, //  center coefficient
        8873,
        4691,
         357,
       -2046,
       -1927,
        -320,
        1095,
        1263,
         349,
        -658,
        -898,
        -306,
         471,
         737,
         333,
        -293,
        -565,
        -286,
         232,
         500,
         310,
        -121,
        -383,
        -259,
         104,
         356,
         282,
         -23,
        -262,
        -226,
          30,
         255,
         249,
          37,
        -173,
        -190,
         -14,
         180,
         216,
          73,
        -105,
        -154,
         -38,
         124,
         182,
          92,
         -54,
        -976
};
\endverbatim
 */
/* @{ */
#define sfc_DEF_FRAME_LEN 480
#define sfc_DEF_FILT_LEN 97
#define sfc_NUM_DEF_COEFS sfc_DEF_FILT_LEN
extern const Fract sfc_def_decim_interp_filt[sfc_NUM_DEF_COEFS];
/* @} */
                              
/* @} */ /* ingroup SFC */
#endif
/* Nothing past this point */
