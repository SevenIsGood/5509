/* ============================================================================
 * Copyright (c) 2008-2017 Texas Instruments Incorporated.
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/


#ifndef IDLELOOP_H_
#define IDLELOOP_H_

/* DSP LDO setting */
//#define DSP_LDO                 ( 105 )
#define DSP_LDO                 ( 130 )

/* PLL output MHz setting */
#define PLL_MHZ                 ( 100 )

#define NUM_SAMP_PER_MS         ( 16 )  // samples per msec
#define NUM_MS_PER_FRAME        ( 10 )  // msec per frame
#define FRAME_LEN               ( NUM_SAMP_PER_MS * NUM_MS_PER_FRAME)   // frame length

#define NUM_FRAMES_OUT_CIRCBUF  ( 10 )  // 10 frames in output circular buffer

/* Ping pong buffer size, one frame for ping, one frame for pong */
#define I2S_DMA_BUF_LEN         ( FRAME_LEN )    // using pack mode, we only need half the frame buffer size

#endif /*IDLELOOP_H_*/
