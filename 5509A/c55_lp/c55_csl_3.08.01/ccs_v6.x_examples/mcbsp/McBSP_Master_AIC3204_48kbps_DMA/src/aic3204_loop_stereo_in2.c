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


/*
 *  AIC3204 Tone
 *
 */

#include "csl_general.h"

#if (defined(CHIP_C5517))

#include <csl_mcbsp.h>
#include "stdio.h"
#include "evm5515.h"
#define CSL_MCBSP_BUF_SIZE (1u)

extern Int16 AIC3204_rset( Uint16 regnum, Uint16 regval);
extern CSL_McbspHandle hMcbsp;
extern Uint32 gmcbspDmaWriteBuf[];
extern Uint32 gmcbspDmaReadBuf[];

CSL_Status Configure_McBSP(void);

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  AIC3204 Tone                                                            *
 *      Output input from STEREO IN 2 through the HEADPHONE jack            *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 aic3204_loop_stereo_in2( )
{
    Int16 j, i,k = 0;
    Int16 sample;

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *   Configure AIC3204                                                      *
 *      CODEC_CLKIN = PLL CLK                                               *
 *      CODEC_CLKIN = NADC * MADC * AOSR * ADCFS                            *
 *      PLL_CLK     = PLL_CLKIN * R * J.D / P                               *
 *      PLL_CLKIN   = BCLK = 1.536MHz                                       *
 *      ADCFS       = 48KHz                                                 *
 *                                                                          *
 * ------------------------------------------------------------------------ */
    AIC3204_rset(  0, 0x00 );      // Select page 0
    AIC3204_rset(  1, 0x01 );      // Reset codec
    AIC3204_rset(  0, 0x01 );      // Select page 1
    AIC3204_rset(  1, 0x08 );      // Disable crude AVDD generation from DVDD
    AIC3204_rset(  2, 0x00 );      // Enable Analog Blocks
    // PLL and Clocks config and Power Up
    AIC3204_rset(  0, 0x00 );      // Select page 0
    #ifdef I2S_DATA_FRMT
    AIC3204_rset( 27, 0x00 );      // BCLK and WCLK is set as i/p to AIC3204(Slave)
	#else
	AIC3204_rset( 27, 0x40 );      // BCLK and WCLK is set as i/p to AIC3204(Slave)
    #endif
    AIC3204_rset(  4, 0x07 );      // PLL setting: PLLCLK <- BCLK and CODEC_CLKIN <-PLL CLK
    AIC3204_rset(  6, 0x20 );      // PLL setting: J = 32
    AIC3204_rset(  7, 0 );         // PLL setting: HI_BYTE(D)
    AIC3204_rset(  8, 0 );         // PLL setting: LO_BYTE(D)
    // For 48 KHz sampling
    AIC3204_rset(  5, 0x92 );      // PLL setting: Power up PLL, P=1 and R=2
    AIC3204_rset( 13, 0x00 );      // Hi_Byte(DOSR) for DOSR = 128 decimal or 0x0080 DAC oversamppling
    AIC3204_rset( 14, 0x80 );      // Lo_Byte(DOSR) for DOSR = 128 decimal or 0x0080
    AIC3204_rset( 20, 0x80 );      // AOSR for AOSR = 128 decimal or 0x0080 for decimation filters 1 to 6
    AIC3204_rset( 11, 0x84 );      // Power up NDAC and set NDAC value to 4
    AIC3204_rset( 12, 0x82 );      // Power up MDAC and set MDAC value to 2
    AIC3204_rset( 18, 0x84 );      // Power up NADC and set NADC value to 4
    AIC3204_rset( 19, 0x82 );      // Power up MADC and set MADC value to 2
    // DAC ROUTING and Power Up
    AIC3204_rset(  0, 0x01 );      // Select page 1
    AIC3204_rset( 14, 0x08 );      // LDAC AFIR routed to LOL
    AIC3204_rset( 15, 0x08 );      // RDAC AFIR routed to LOR
    AIC3204_rset(  0, 0x00 );      // Select page 0
    AIC3204_rset( 64, 0x02 );      // Left vol=right vol
    AIC3204_rset( 65, 0x00 );      // Left DAC gain to 0dB VOL; Right tracks Left
    AIC3204_rset( 63, 0xd4 );      // Power up left,right data paths and set channel
    AIC3204_rset(  0, 0x01 );      // Select page 1
    AIC3204_rset( 18, 0x00 );      // Unmute LOL , 0dB gain
    AIC3204_rset( 19, 0x00 );      // Unmute LOR , 0dB gain
    AIC3204_rset(  9, 0x0C );      // Power up LOL,LOR
    AIC3204_rset(  0, 0x00 );      // Select page 0
    EVM5515_wait( 500 );         // Wait
    // ADC ROUTING and Power Up
    AIC3204_rset(  0, 0x01 );      // Select page 1
    AIC3204_rset( 52, 0x0C );      // STEREO 1 Jack
		                           // IN2_L to LADC_P through 40 kohm
    AIC3204_rset( 55, 0x0C );      // IN2_R to RADC_P through 40 kohmm
    AIC3204_rset( 54, 0x03 );      // CM_1 (common mode) to LADC_M through 40 kohm
    AIC3204_rset( 57, 0xC0 );      // CM_1 (common mode) to RADC_M through 40 kohm
    AIC3204_rset( 59, 0x0f );      // MIC_PGA_L unmute
    AIC3204_rset( 60, 0x0f );      // MIC_PGA_R unmute
    AIC3204_rset(  0, 0x00 );      // Select page 0
    AIC3204_rset( 81, 0xc0 );      // Powerup Left and Right ADC
    AIC3204_rset( 82, 0x00 );      // Unmute Left and Right ADC

    AIC3204_rset( 0,  0x00 );
    EVM5515_wait( 200 );  // Wait


    /* McBSP settings */
    Configure_McBSP();

    /* Play Tone */
    for ( i = 0 ; i < 5 ; i++ )
    {
        for ( j = 0 ; j < 1000 ; j++ )
        {
            for ( sample = 0 ; sample < 100 ; sample++ )
			//while(1)
			{
			    /*Wait till Interrupt completion bit is set*/
				while (0x3000 != (CSL_SYSCTRL_REGS->DMAIFR & 0x3000));  /*Poll for the interrupt of rx_dma completions*/

				//for ( k = 0 ; k < CSL_MCBSP_BUF_SIZE ; k++ )
                gmcbspDmaWriteBuf[k]= gmcbspDmaReadBuf[k];

				CSL_SYSCTRL_REGS->DMAIFR |= 0x2000;
				CSL_SYSCTRL_REGS->DMAIFR |= 0x1000;
			}
        }
    }
    /* Disble MCBSP */
    MCBSP_close(hMcbsp);

    return 0;
}

#endif /* #if (defined(CHIP_C5517)) */
