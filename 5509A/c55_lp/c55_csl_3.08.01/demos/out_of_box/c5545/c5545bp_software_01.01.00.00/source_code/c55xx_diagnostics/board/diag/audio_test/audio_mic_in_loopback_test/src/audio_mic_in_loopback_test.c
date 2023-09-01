/*
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*! \file audio_mic_in_loopback_test.c
*
*   \brief Functions that configure the audio codec regsiters for the audio
*          mic in loopback operation.
*
*/

#include "audio_mic_in_loopback_test.h"
#include "audio_common.h"

extern TEST_STATUS AIC3206_loop_mic_in(void *testArgs);

/**
 *
 * \brief This function configures all audio codec registers for
 *        onboard mic in loopback test
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
extern TEST_STATUS AIC3206_loop_mic_in(void *testArgs)
{
	/* Pre-generated sine wave data, 16-bit signed samples */
	Int16 j, i = 0;
	Int16 sample, data1, data2;

	C55x_msgWrite("Connect headset to the HEADPHONE port of the BoosterPack\n\n\r");

	C55x_msgWrite("This test loops back the audio input from the on-board MIC \n\r"
			      "of the BoosterPack to the headset connected to HEADPHONE port\n\n\n\r");

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *   Configure AIC3206                                                      *
 *      CODEC_CLKIN = PLL CLK                                               *
 *      CODEC_CLKIN = NADC * MADC * AOSR * ADCFS                            *
 *      PLL_CLK     = PLL_CLKIN * R * J.D / P                               *
 *      PLL_CLKIN   = BCLK = 1.536MHz                                       *
 *      ADCFS       = 48KHz                                                 *
 *                                                                          *
 * ------------------------------------------------------------------------ */
	AIC3206_write(  0, 0x00 );      // Select page 0
	AIC3206_write(  1, 0x01 );      // Reset codec
	AIC3206_write(  0, 0x01 );      // Select page 1
	AIC3206_write(  1, 0x08 );      // Disable crude AVDD generation from DVDD
	AIC3206_write(  2, 0x00 );      // Enable Analog Blocks
	// PLL and Clocks config and Power Up
	AIC3206_write(  0, 0x00 );      // Select page 0
	AIC3206_write( 27, 0x00 );      // BCLK and WCLK is set as i/p to AIC3206(Slave)
	AIC3206_write(  4, 0x07 );      // PLL setting: PLLCLK <- BCLK and CODEC_CLKIN <-PLL CLK
	AIC3206_write(  6, 0x20 );      // PLL setting: J = 32
	AIC3206_write(  7, 0 );         // PLL setting: HI_BYTE(D)
	AIC3206_write(  8, 0 );         // PLL setting: LO_BYTE(D)
	// For 48 KHz sampling
	AIC3206_write(  5, 0x92 );      // PLL setting: Power up PLL, P=1 and R=2
	AIC3206_write( 13, 0x00 );      // Hi_Byte(DOSR) for DOSR = 128 decimal or 0x0080 DAC oversamppling
	AIC3206_write( 14, 0x80 );      // Lo_Byte(DOSR) for DOSR = 128 decimal or 0x0080
	AIC3206_write( 20, 0x80 );      // AOSR for AOSR = 128 decimal or 0x0080 for decimation filters 1 to 6
	AIC3206_write( 11, 0x84 );      // Power up NDAC and set NDAC value to 4
	AIC3206_write( 12, 0x82 );      // Power up MDAC and set MDAC value to 2
	AIC3206_write( 18, 0x84 );      // Power up NADC and set NADC value to 4
	AIC3206_write( 19, 0x82 );      // Power up MADC and set MADC value to 2
	// DAC ROUTING and Power Up
	AIC3206_write(  0, 0x01 );      // Select page 1
	AIC3206_write( 12, 0x08 );      // LDAC AFIR routed to HPL
	AIC3206_write( 13, 0x08 );      // RDAC AFIR routed to HPR
	AIC3206_write(  0, 0x00 );      // Select page 0
	AIC3206_write( 64, 0x02 );      // Left vol=right vol
	AIC3206_write( 65, 0x00 );      // Left DAC gain to 0dB VOL; Right tracks Left
	AIC3206_write( 63, 0xd4 );      // Power up left,right data paths and set channel
	AIC3206_write(  0, 0x01 );      // Select page 1
	AIC3206_write( 16, 0x06 );      // Unmute HPL , 6dB gain
	AIC3206_write( 17, 0x06 );      // Unmute HPR , 6dB gain
	AIC3206_write(  9, 0x30 );      // Power up HPL,HPR
	AIC3206_write(  0, 0x00 );      // Select page 0
	C55x_delay_msec( 5);            // Wait 5 msec
	// ADC ROUTING and Power Up
	AIC3206_write(  0, 0x01 );      // Select page 1
	AIC3206_write( 51, 0x40 );      // SetMICBIAS
	AIC3206_write( 52, 0x30 );      // STEREO 1 Jack
	// IN2_L to LADC_P through 40 kohm
	AIC3206_write( 55, 0x03 );      // IN2_R to RADC_P through 40 kohmm
	AIC3206_write( 54, 0x03 );      // CM_1 (common mode) to LADC_M through 40 kohm
	AIC3206_write( 57, 0xc0 );      // CM_1 (common mode) to RADC_M through 40 kohm
	AIC3206_write( 59, 0x5f );      // MIC_PGA_L unmute
	AIC3206_write( 60, 0x5f );      // MIC_PGA_R unmute
	AIC3206_write(  0, 0x00 );      // Select page 0
	AIC3206_write( 81, 0xc0 );      // Powerup Left and Right ADC
	AIC3206_write( 82, 0x00 );      // Unmute Left and Right ADC

	AIC3206_write( 0,  0x00 );
	C55x_delay_msec( 2 );           // Wait 2 msec

	/* I2S settings */
	I2S2_SRGR = 0x0015;
	I2S2_ICMR = 0x0028;    // Enable interrupts
	I2S2_CR   = 0x8012;    // 16-bit word, Master, enable I2C

    gpio_interrupt_initiliastion();

	/* Play Tone */
	for( i = 0 ; i < 5 ; i++ )
	{
		for ( j = 0 ; j < 5000 ; j++ )
		{
			for ( sample = 0 ; sample < 48 ; sample++ )
			{
				/* Read Digital audio */
				while((Rcv & I2S2_IR) == 0);   // Wait for receive interrupt to be pending
				data1 = I2S2_W0_MSW_R;  // 16 bit left channel received audio data
				data2 = I2S2_W1_MSW_R;  // 16 bit right channel received audio data

				/* Write Digital audio */
				while((Xmit & I2S2_IR) == 0);  // Wait for receive interrupt to be pending
				I2S2_W0_MSW_W = data1;  // 16 bit left channel transmit audio data
				I2S2_W1_MSW_W = data2;  // 16 bit right channel transmit audio data
			}
            if(sw3Pressed == TRUE)
            {
            	break;
            }

		}
        if(sw3Pressed == TRUE)
        {
        	break;
        }

	}
	/* Disble I2S */
	I2S0_CR = 0x00;

	return 0;

}

/**
 *
 * \brief This function used to to initialise i2c interface and run
 *        audio onboard mic in loopback test
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
static TEST_STATUS run_audio_mic_in_loopback_test(void *testArgs)
{
	Int16 retVal;
	Uint8  c = 0;

	/* Enable clocks to all peripherals */
	CSL_SYSCTRL_REGS->PCGCR1 = 0x0000;
	CSL_SYSCTRL_REGS->PCGCR2 = 0x0000;

    retVal =  SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                          CSL_EBSR_PPMODE_1);    // Configure Parallel Port for I2S2
    retVal |= SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,
                          CSL_EBSR_SP1MODE_1);  // Serial Port mode 1 (I2S1 and GP[11:10]).

	retVal = initialise_i2c_interface(testArgs);
	if(retVal != 0)
	{
		C55x_msgWrite("I2C initialisation failed\n\r");
		return (TEST_FAIL);
	}

	retVal = AIC3206_loop_mic_in(testArgs);
	if(retVal != 0)
	{
		return (TEST_FAIL);
	}

#ifdef USE_USER_INPUT

	C55x_msgWrite("Press Y/y if the Audio received from on-board MIC of the BoosterPack is \n\r"
   			      "observed at the headset connected at the HEADPHONE port properly,\n\r"
			      "any other key for failure:\n\r");

	C55x_msgRead(&c, 1);
	if((c != 'y') && (c != 'Y'))
	{
		 C55x_msgWrite("Audio Loopback from the on-board MIC IN to\n\r"
				       "to the HEADPHONE port is not proper\n\r");
	    return (TEST_FAIL);
	}
#endif

	return (TEST_PASS);

}

/**
 *
 * \brief This function performs audio onboard mic in loopback test
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
 TEST_STATUS audioMicInLoopbackTest(void *testArgs)
 {
     TEST_STATUS testStatus;

     C55x_msgWrite("\n********************************************\n\r");
     C55x_msgWrite(  "         AUDIO MIC IN LOOPBACK TEST    \n\r");
     C55x_msgWrite(  "********************************************\n\r");

     testStatus = run_audio_mic_in_loopback_test(testArgs);
     if(testStatus != TEST_PASS)
     {
     	C55x_msgWrite("\n\nAudio MIC IN Loopback Test Failed!\n\r");
     	return (TEST_FAIL);
     }
     else
     {
     	C55x_msgWrite("\n\nAudio MIC IN Loopback Test Passed!\n\r");
     }

  	C55x_msgWrite("\nAudio MIC IN Loopback Test Completed!\n\r");

     return testStatus;

 }
