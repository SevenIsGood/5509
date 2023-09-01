/**
 *  @file   sfc31.c
 *
 *  @brief  Contains functions to decimate and interpolate signals.
 *          The decimation and interpolation factors are fixed to 3.
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

#include <ti/mas/types/types.h>                       /* DSP types            */
#include <ti/mas/fract/fract.h>                       /* fractional macros    */

#include "sfc31.h"
#include "codec_pcm186x.h"
/**
 *  @brief Macros representing different Q formats
 *
 *  @remark 
 */
#define SFC_DATA_Q 0          /**< Input and output data Q format             */
#define SFC_COEF_Q 15         /**< Filter coefficients Q format               */
#define SFC_DECIM_CONV_Q 15   /**< Intermediate convolution result Q format for 
                                   decimation: there is no need to use a smaller
                                   Q to prevent overflow during convolution.
                                   According to filter theory, overflow during
                                   convolution will not result in error in final
                                   result as long as filter gain is equal to or
                                   less than 0dB.                             */
#define SFC_INTERP_CONV_Q 14  /**< Intermediate convolution result Q format for
                                   interpolation: a smaller Q is used to prevent
                                   overflow in the multiplication by 2 after
                                   convolution.                               */

/**
 *  @brief Function sfcDecimationBy3()
 *
 *  <B>Description:</B> This function decimates the input signal by 3.
 *
 * Mathematical formulas:
 * M: length of input frame
 * L: filter length
 * N = M+L-1
 * Delay line:
 *   From previous frames (oldest to newest): [x(n-N+1) x(n-N+2) ... x(n-M)]
 *   From current  frame  (oldest to newest): [x(n-M+1) x(n-M+2) ... x(n)]
 * Coefficients: [H(0) H(1) ... H(L-1)]
 * Output frame(oldest to newest): [y(n-M/3+1) y(n-M/3+2) ... y(n)]
 * Convolution of x and H:
 *             y(n-M/2+1) =  x(n-N+1)*H(0) + x(n-N+2)*H(1) + ...
 *                         + x(n-N+1+k)*H(k) + ... + x(n-M+1-k)*H(k) + ...
 *                         + x(n-M)*H(L-2) + x(n-M+1)*H(L-1)
 *                        =  x(n-N+1)  *H(0) + x(n-M+1)  *H(0)
 *                         + x(n-N+2)  *H(1) + x(n-M)    *H(1)
 *                         + x(n-N+1+k)*H(k) + x(n-M+1-k)*H(k) + ...
 *                         + x(n-N+1+(L-1)/2)*H((L-1)/2)
 *
 *             y(n-M/2+2) =  x(n-N+3)*H(0) + x(n-N+4)*H(1) + ...
 *                         + x(n-N+3+k)*H(k) + ... + x(n-M+3-k)*H(k) + ...
 *                         + x(n-M+2)*H(L-2) + x(n-M+3)*H(L-1),
 *                        =  x(n-N+3)  *H(0) + x(n-M+3)  *H(0)
 *                         + x(n-N+4)  *H(1) + x(n-M+2)  *H(1)
 *                         + x(n-N+3+k)*H(k) + x(n-M+3-k)*H(k) + ...
 *                         + x(n-N+3+(L-1)/2)*H((L-1)/2)
 *             ...
 *
 */
linSample g_workBufferDown[sfc_DEF_FRAME_LEN];
void sfcDecimationBy3(linSample *delay, linSample *sig_out,
		const Fract *coeffs, tint filt_len, tint frame_len_in)
{
#ifdef USE_SIMPLE_FILTER	// using 3 sample average
	int i, j;                    /* loop index */

	for (i=0, j=0; i<frame_len_in; i+=3, j++)
	{
	  sig_out[j] = (delay[filt_len+i]+delay[filt_len+i+1]+delay[filt_len+i+2])/3;
	}
#else
	int i, k;                    /* loop index */
	tint filt_len_half;             /* half of the symmetric filter length */
	LFract conv_sum_1, conv_sum_2;  /* storing convolution result */

	/* Use default filter if user-defined filter is not provided */
	if(coeffs == NULL) {
		coeffs   = sfc_def_decim_interp_filt;
		filt_len = sfc_DEF_FILT_LEN;
	}

	filt_len_half = (filt_len)/2;

	/* Outer loop - generating one output sample per iteratioin. Loop counter is
	 incremented by 3 for downsampling.  */
	for(i=0; i<frame_len_in; i++)
	{
		conv_sum_1 = (LFract)0;
		conv_sum_2 = (LFract)0;

		/* Inner loop - convolve input samples with filter coefficients */
		for(k=0; k<filt_len_half; k++) {
			/* Not utilizing coefficients symmetry by first adding two samples and then
			 multiplying the sum with coefficient. Need overflow protection for
			 addition of two samples, which may consume more cycles or lose precision.
			 Coefficients symmetry may be utilized in optimized code for specific
			 processors.
			*/
			conv_sum_1 += frctMul(delay[i+k], SFC_DATA_Q,
								coeffs[k], SFC_COEF_Q, SFC_DECIM_CONV_Q);

			conv_sum_2 += frctMul(delay[i+filt_len-1-k], SFC_DATA_Q,
								coeffs[k], SFC_COEF_Q, SFC_DECIM_CONV_Q);
		}

		/* Add center coefficient */
		conv_sum_1 += frctMul(delay[i+filt_len_half], SFC_DATA_Q,
							  coeffs[filt_len_half], SFC_COEF_Q, SFC_DECIM_CONV_Q);
		conv_sum_1 += conv_sum_2;

		/* Save convolution result to output buffer */
		conv_sum_1 = frctRoundLF(conv_sum_1, SFC_DECIM_CONV_Q, SFC_DECIM_CONV_Q);
		g_workBufferDown[i] = (linSample)frctLF2FS(conv_sum_1, SFC_DECIM_CONV_Q, SFC_DATA_Q);
	}

	/* down sampling by pick 1 out of 3 from the filtered audio samples */
	for(i=0; i<frame_len_in/3; i++)
	{
		sig_out[i] = g_workBufferDown[i*3];
	}
#endif

  return;
} /* sfcDecimationBy3*/

/**
 *  @brief Function sfcInterpolationBy3()
 * 
 *  <B>Description:</B> This function interpolates the input signal by 3. 
 *
 * Mathematical formulas:
 * M: length of input frame
 * L: filter length or delay line length, must be multiple of 3 plus 1. 
 * N: total number of samples in input buffer, including delay line and crrent frame.
 *    - delay line: (L-1)/3 samples
 *    - current frame: M samples
 *    - N = M+(L-1)/3, or N-M = (L-1)/3
 * Delay line:
 *   From previous frames (oldest to newest): [x(n-N+1) x(n-N+2) ... x(n-M)] - provided through argument delay, (L-1)/3 samples
 *   From current  frame  (oldest to newest): [x(n-M+1) x(n-M+2) ... x(n)]   - provided through argument delay, M samples
 * Coefficients: [H(0) H(1) ... H(L-1)]
 * Output frame(oldest to newest): [y(n-3*M+1) y(n-3*M+2) y(n-3*M+3) ... y(n)] - 3*M samples
 * Convolution steps:
 *   Insert 0's to input (not actually performed in the code):
 *     u = [x(n-N+1) 0 0 x(n-N+2) 0 0 ... x(n-M) 0 0             // (L-1)/3*3 or L-1 samples 
 *          x(n-M+1) 0 0 x(n-M+2) 0 0 ... x(n-1) 0 0 x(n) 0 0]   // M*3 samples
 *   Convolve u with H and get:
 *            z(n-3*M+1) =   x(n-N+1)*H(0) + 0*H(1) + 0*H(2)+ x(n-N+2)*H(3) + ... + x(n-M)*H(L-4) + 0*H(L-3) + 0*H(L-2)
 *                         + x(n-M+1)*H(L-1) 
 *
 *            z(n-3*M+2) =  0*H(0) + 0*H(1) + x(n-N+2)*H(2) + 0*H(3) + ... + x(n-M)*H(L-5) + 0*H(L-4) + 0*H(L-3) 
 *                        + x(n-M+1)*H(L-2) + 0*H(L-1)
 *
 *            z(n-3*M+3) =  0*H(0) + x(n-N+2)*H(1) + 0*H(2) + 0*H(3) + ... + x(n-M)*H(L-6) + 0*H(L-5) + 0*H(L-4) 
 *                        + x(n-M+1)*H(L-3) + 0*H(L-2) + 0*H(L-1)
 *            ...
 *   Multiply convoluation result by 3:
 *            y(n) = z(n) * 3
 */  
linSample g_workBufferUp[sfc_DEF_FRAME_LEN];
void sfcInterpolationBy3(linSample *delay, linSample *sig_out, const Fract *coeffs, 
                         tint filt_len, tint frame_len_in)
{
#ifdef USE_SIMPLE_FILTER	// using linear interpolation
	int i;                    /* loop index */

	for (i=0; i<(frame_len_in-1); i++)
	{
	  sig_out[i*3] = delay[(filt_len)/3+i];
	  sig_out[i*3+1] = delay[(filt_len)/3+i]+(delay[(filt_len)/3+i+1]-delay[(filt_len)/3+i])/3;
	  sig_out[i*3+2] = delay[(filt_len)/3+i]+(delay[(filt_len)/3+i+1]-delay[(filt_len)/3+i])*2/3;
	}
	sig_out[i*3] = delay[(filt_len)/3+i];
	sig_out[i*3+1] = delay[(filt_len)/3+i];
	sig_out[i*3+2] = delay[(filt_len)/3+i];
#else
	int i, k;                    /* loop index */
	tint filt_len_half;             /* half of the symmetric filter length */
	LFract conv_sum_1, conv_sum_2;  /* storing convolution result */

	/* Use default filter if user-defined filter is not provided */
	if(coeffs == NULL) {
	coeffs   = sfc_def_decim_interp_filt;
	filt_len = sfc_DEF_FILT_LEN;
	}

	filt_len_half = (filt_len)/2;

	/* copy the delay buffer to work buffer with 1 to 3 up sampling
	* by adding 2 0s per input sample */
	for(i=0; i<(frame_len_in+(filt_len)/3); i++) {
	  g_workBufferUp[i*3] = delay[i];
	  g_workBufferUp[i*3+1] = 0;
	  g_workBufferUp[i*3+2] = 0;
	}

  /* Apply the filter */
  /* Outer loop - generating one samples per iteration. */
  for(i=0; i<frame_len_in*3; i++) {
	conv_sum_1 = (LFract)0;
	conv_sum_2 = (LFract)0;

	/* Inner loop - convolve input samples with filter coefficients */
	for(k=0; k<filt_len_half; k++) {
		/* Not utilizing coefficients symmetry by first adding two samples and then
		 multiplying the sum with coefficient. Need overflow protection for
		 addition of two samples, which may consume more cycles or lose precision.
		 Coefficients symmetry may be utilized in optimized code for specific
		 processors.
		*/
		conv_sum_1 += frctMul(g_workBufferUp[i+k], SFC_DATA_Q,
							coeffs[k], SFC_COEF_Q, SFC_DECIM_CONV_Q);

		conv_sum_2 += frctMul(g_workBufferUp[i+filt_len-1-k], SFC_DATA_Q,
							coeffs[k], SFC_COEF_Q, SFC_DECIM_CONV_Q);
	}

	/* Add center coefficient */
	conv_sum_1 += frctMul(g_workBufferUp[i+filt_len_half], SFC_DATA_Q,
						  coeffs[filt_len_half], SFC_COEF_Q, SFC_DECIM_CONV_Q);
	conv_sum_1 += conv_sum_2;

	/* Multiply convolution results by 3 due to 0 insertion */
	conv_sum_1 *= 3;
    
    /* Save convolution result to output buffer */
    conv_sum_1 = frctRoundLF(conv_sum_1, SFC_DECIM_CONV_Q, SFC_DECIM_CONV_Q);
    sig_out[i] = (linSample)frctLF2FS(conv_sum_1, SFC_DECIM_CONV_Q, SFC_DATA_Q);
  }
#endif

  return;
} /* sfcInterpolationBy3 */

/* Nothing past this point */                             
