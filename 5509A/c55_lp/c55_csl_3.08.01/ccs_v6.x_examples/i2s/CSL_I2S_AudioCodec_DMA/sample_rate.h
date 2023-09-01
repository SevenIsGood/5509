/* ============================================================================
 * Copyright (c) 2008-2012 Texas Instruments Incorporated.
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


#ifndef _SAMPLE_RATE_H_
#define _SAMPLE_RATE_H_

#define RATE_1KHZ               ( 1000 )

/* Audio sampling rates */
#define SAMP_RATE_8KHZ          ( 0x1F40 )
#define SAMP_RATE_16KHZ         ( 0x3E80 )
#define SAMP_RATE_22_05KHZ      ( 0x5622 )
#define SAMP_RATE_24KHZ         ( 0x5DC0 )
#define SAMP_RATE_32KHZ         ( 0x7D00 )
#define SAMP_RATE_44_1KHZ       ( 0xAC44 )
#define SAMP_RATE_48KHZ         ( 0xBB80 )

/* Audio sampling rates -- samples/msec. */
#define SAMPS_PER_MSEC_8KHZ     ( SAMP_RATE_8KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_16KHZ    ( SAMP_RATE_16KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_22_05KHZ ( SAMP_RATE_22_05KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_24KHZ    ( SAMP_RATE_24KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_32KHZ    ( SAMP_RATE_32KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_44_1KHZ  ( SAMP_RATE_44_1KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_48KHZ    ( SAMP_RATE_48KHZ / RATE_1KHZ )

#endif /* _SAMPLE_RATE_H_ */
