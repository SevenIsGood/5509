/* ============================================================================
 * Copyright (c) 2017 Texas Instruments Incorporated.
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


/**
 *  \file   codec_pcm186x.c
 *
 *  \brief  codec configuration function
 *
 *   This file contains the APIs for codec(PCM186x) read and write using I2C
 *
 *  (C) Copyright 2017, Texas Instruments, Inc
 *
 *  \author     PR Mistral
 *
 *  \version    1.0
 *
 */

#include "stdio.h"
#include "stdlib.h"

#include "codec_pcm186x.h"
#include "sample_rate.h"

#define ENABLE_RECORD

#define I2C_OWN_ADDR            (0x2F)
#define I2C_CODEC_ADDR          (0x4A)
#define I2C_CODEC_ADDR2          (0x4B)
#define I2C_BUS_FREQ            (400)   // I2C 400Khz

extern int pll_mhz;

extern pI2cHandle i2cHandle;
extern pI2cHandle    hi2c;
extern CSL_I2cSetup     i2cSetup;

CSL_I2cSetup     i2cSetup_pcm186x;

CSL_Status PCM186x_init(
    Uint16 sampRateRec,
    Uint32 i2cInClk)
{
    CSL_Status result = CSL_SOK;
    volatile Uint32 looper;

	// {0x00, 0x00}, /** ================ Changing to Page 0 ================ */
    result = PCM186x_Write(0x00, 0x00, hi2c);
    if (result != CSL_SOK)
    {
        return result;
    }

    // set the PCM1864 according to the sampling rate
    if (sampRateRec==16000)
    {
#ifdef USE_48KHZ_RATE
		// {0x20, 0x11}, /** SCK_XI_SEL : SCK or XTAL, MST_SCK_SRC : SCK or XI,
				         // MST_MODE : Master, CMB_ADC_CLK_SRC : SCK,DSP2_CLK_SRC : SCK,
				         // DSP1_CLK_SRC : SCK, CLKDET_EN : Disable */
		result = PCM186x_Write(0x20, 0x11, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}
		// {0x26, 0x07}, /** SCK to BCK divider to 8 **/
		result = PCM186x_Write(0x26, 0x07, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}
#else
		// {0x20, 0x3E}, // SCK_XI_SEL= SCK or Xtal, MST_SCK_SRC = PLL, MST_MODE= Master, ADC_CLK_SRC= PLL,
        				 // DSP2_CLK_SRC=PLL, DSP1_CLK_SRC=PLL, CLKDET_EN=Disable
		result = PCM186x_Write(0x20, 0x3E, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x21, 0x02},  // XTAL to DSP1 Divide value = 1/3
		result = PCM186x_Write(0x21, 0x02, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x22, 0x05},  // XTAL to DSP2 Divide value = 1/6
		result = PCM186x_Write(0x22, 0x05, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x23, 0x0B},  // XTAL to ADC Divide value = 1/12
		result = PCM186x_Write(0x23, 0x0B, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x25, 0x17},  // PLL to SCK Divide value = 1/24
		result = PCM186x_Write(0x25, 0x17, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x26, 0x03}, /** SCK to BCK divider to 4 **/
		result = PCM186x_Write(0x26, 0x03, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x29, 0x03} P Divide value = 1/4
		result = PCM186x_Write(0x29, 0x03, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x2a, 0x00} R Divide value = 1
		result = PCM186x_Write(0x2a, 0x00, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x2b, 0x10} J Divide value = 16
		result = PCM186x_Write(0x2b, 0x10, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x28, 0x01} PLL_REF_SEL=SCK; PLL_EN=Enabled
		result = PCM186x_Write(0x28, 0x01, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// wait for PLL to lock
		for (looper = 0; looper < 1000000; looper++)
		{
			; // Wait
		}
#endif
		// {0x01, 0x40}, /** PGA CH1_L to MIC_GAIN_DB */
		result = PCM186x_Write(0x01, (MIC_GAIN_DB*2)&0x7F, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x02, 0x40}, /** PGA CH1_R to MIC_GAIN_DB */
		result = PCM186x_Write(0x02, (MIC_GAIN_DB*2)&0x7F, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x03, 0x40}, /** PGA CH2_L to MIC_GAIN_DB */
		result = PCM186x_Write(0x03, (MIC_GAIN_DB*2)&0x7F, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x04, 0x40}, /** PGA CH2_R to MIC_GAIN_DB */
		result = PCM186x_Write(0x04, (MIC_GAIN_DB*2)&0x7F, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x05, 0x86}, /** SMOOTH : Smooth change,LINK : Independent control
		// 		          DPGA_CLIP_EN : Disable,MAX_ATT : -3dB,START_ATT : 10,
		// 		          AGC_EN : Disable */
		result = PCM186x_Write(0x05, 0x86, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}


		// {0x06, 0x41}, /** Polarity : Normal, Channel : VINL1[SE] */
		result = PCM186x_Write(0x06, 0x41, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x07, 0x41}, /** Polarity : Normal, Channel : VINR1[SE] */
		result = PCM186x_Write(0x07, 0x41, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x08, 0x44}, /** Polarity : Normal, Channel : VINL3[SE] */
		result = PCM186x_Write(0x08, 0x44, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x09, 0x44}, /** Polarity : Normal, Channel : VINR3[SE] */
		result = PCM186x_Write(0x09, 0x44, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}


		result = PCM186x_Write(0x0B, 0x0C, hi2c);

		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x10, 0x03}, /** GPIO0_FUNC - SCK Out, GPIO0_POL - Normal
		//                   GPIO1_FUNC - GPIO1, GPIO1_POL - Normal */
		result = PCM186x_Write(0x10, 0x03, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x11, 0x50}, /** GPIO2_FUNC - GPIO2, GPIO2_POL - Normal
		//                   GPIO3_FUNC - DOUT2, GPIO3_POL - Normal */
		result = PCM186x_Write(0x11, 0x50, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x12, 0x04}, /** GPIO0_DIR - GPIO0 - Output
		//                   GPIO1_DIR - GPIO1 - Input */
		result = PCM186x_Write(0x12, 0x04, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// {0x13, 0x40}, /** GPIO2_DIR - GPIO2 - Input
		//                   GPIO3_DIR - GPIO3 - Output */
		result = PCM186x_Write(0x13, 0x40, hi2c);
		if (result != CSL_SOK)
		{
			return result;
		}

		// wait for coec to be ready
		for (looper = 0; looper < 200; looper++)
		{
			; // Wait
		}

  }

    return result;
}

/**
 *  \brief Codec write function
 *
 *  Function to write a byte of data to a codec register.
 *
 *  \param regAddr  [IN]  Address of the register to write the data
 *  \param regData  [IN]  Data to write into the register
 *
 *  \return CSL_SOK - if successful, else suitable error code
 */
CSL_Status PCM186x_Write(Uint16 regAddr, Uint16 regData, pI2cHandle hi2c)
{
    CSL_Status    status;
    Uint16        writeCount;
    volatile Uint16 looper;
    Uint16        writeBuff[2];
    ///Uint16        readBuff[2];
    Uint16 startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));

    status = CSL_ESYS_INVPARAMS;

    if(hi2c != NULL)
    {
        writeCount  =  2;
        /* Initialize the buffer          */
        /* First byte is Register Address */
        /* Second byte is register data   */
        writeBuff[0] = (regAddr & 0x00FF);
        writeBuff[1] = (regData & 0x00FF);

        /* Write the data */
        status = I2C_write(writeBuff,
                        writeCount,
                        I2C_CODEC_ADDR,
                        TRUE,
                        startStop,
                        CSL_I2C_MAX_TIMEOUT);
        if (status!=CSL_SOK)
        {
            return status;
        }

        /* Give some delay */
        for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}
    }

    return status;
}

/**
 *  \brief Codec write function 2
 *
 *  Function to write a byte of data to a codec 2 register.
 *
 *  \param regAddr  [IN]  Address of the register to write the data
 *  \param regData  [IN]  Data to write into the register
 *
 *  \return CSL_SOK - if successful, else suitable error code
 */
CSL_Status PCM186x_Write2(Uint16 regAddr, Uint16 regData, pI2cHandle hi2c)
{
    CSL_Status    status;
    Uint16        writeCount;
    volatile Uint16 looper;
    Uint16        writeBuff[2];
    ///Uint16        readBuff[2];
    Uint16 startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));

    status = CSL_ESYS_INVPARAMS;

    if(hi2c != NULL)
    {
        writeCount  =  2;
        /* Initialize the buffer          */
        /* First byte is Register Address */
        /* Second byte is register data   */
        writeBuff[0] = (regAddr & 0x00FF);
        writeBuff[1] = (regData & 0x00FF);

        /* Write the data */
        status = I2C_write(writeBuff,
                        writeCount,
                        I2C_CODEC_ADDR2,
                        TRUE,
                        startStop,
                        CSL_I2C_MAX_TIMEOUT);
        if (status!=CSL_SOK)
        {
            return status;
        }

        /* Give some delay */
        for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}
    }

        return status;
}
