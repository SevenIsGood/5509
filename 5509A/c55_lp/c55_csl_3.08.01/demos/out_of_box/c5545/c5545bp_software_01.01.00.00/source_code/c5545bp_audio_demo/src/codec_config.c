/*
* codec_config.c
*
*
* Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
*
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

#include "codec_config.h"
#include "sample_rate.h"
#include "sys_init.h"

// private function prototypes
void aic_writeCoef(Uint32 theCoef, Uint16 pageNum, Uint16 baseRegNumber);
void aic_writeCoef_2(Uint32 theCoef, Uint16 baseRegNumber);
void aic_SetOrd1IIRSection(Uint32 coefs[ORD1_COEF_SZ], Uint16 pageNum, Uint16 baseRegNumber);
void aic_SetBiQuadSection(Uint32 biCoefs[BQ_COEF_SZ], Uint16 pageNum, Uint16 baseRegNumber);
void aic_SetADCBiQuad(Uint32 biCoefs[BQ_COEF_SZ], tAICADCBiQuad BQIdentifier);

Uint16          gADCDigGain[2] = {ADC_DIG_GAIN, ADC_DIG_GAIN}; // left right digital gain to apply.

// HPF for 8 kHz: PRB_R2, 1st Order IIR + 1 Biquad
//////////////////////////////////////////////////////////
//                     Filter coefficient load             //
//
// Text "* Sample Rate = 8000"
// Text "* Filter 1 High Pass 1st Order Butterworth, 120 Hz Fc 0.0 dB "
// Text "* Filter 2 High Pass 2nd Order Butterworth, 120 Hz Fc 0.0 dB "
//
// Text "* BQ: H(z) = (N0 + 2*N1/z + N2/(z2)) / (8388608 - 2*D1/z - D2/(z2))"
// Text "* Filter Coefficients in format N0, N1, N2, D1, D2"
//
// Text "* 1O: H(z) = (N0 + N1/z) / (8388608 - D1/z)"
// Text "* Filter Coefficients in format N0, N1, D1"
//
// Text "* Filter 1 1O "
// 0x7A3C4A
// 0x85C3B6
// 0x747895
//
//        ADC Coeff Left Channel        ADC Coeff Right Channel
// N0 --> C4 (Pg 8, Reg 24,25,26)       C36 (Pg 9, Reg 32,33,34)
// N1 --> C5 (Pg 8, Reg 28,29,30)       C37 (Pg 9, Reg 36,37,38)
// D1 --> C6 (Pg 8, Reg 32,33,34)       C38 (Pg 9, Reg 40,31,41)
//
//
// Text "* Filter 2 BQ "
// 0x77BF0B
// 0x8840F5
// 0x77BF0B
// 0x777ADF
// 0x8FF98E
Uint32 hpCoefs120Hz_8kHz[BQ_COEF_SZ] = {0x77BF0B, 0x8840F5, 0x77BF0B, 0x777ADF, 0x8FF98E};
Uint32 hpCoefs120HzOrd1_8kHz[ORD1_COEF_SZ] = {0x7A3C4A, 0x85C3B6, 0x747895};

// HPF for 16 kHz: PRB_R2, 1st Order IIR + 1 Biquad
//////////////////////////////////////////////////////////
//                     Filter coefficient load             //
//
// Text "* Sample Rate = 16000"
// Text "* Filter 1 High Pass 1st Order Butterworth, 120 Hz Fc 0.0 dB "
// Text "* Filter 2 High Pass 2nd Order Butterworth, 120 Hz Fc 0.0 dB "
//
// Text "* BQ: H(z) = (N0 + 2*N1/z + N2/(z2)) / (8388608 - 2*D1/z - D2/(z2))"
// Text "* Filter Coefficients in format N0, N1, N2, D1, D2"
//
// Text "* 1O: H(z) = (N0 + N1/z) / (8388608 - D1/z)"
// Text "* Filter Coefficients in format N0, N1, D1"
//
// Text "* Filter 1 1O "
// 0x7D0D8E
// 0x82F272
// 0x7A1B1D
//
//        ADC Coeff Left Channel        ADC Coeff Right Channel
// N0 --> C4 (Pg 8, Reg 24,25,26)       C36 (Pg 9, Reg 32,33,34)
// N1 --> C5 (Pg 8, Reg 28,29,30)       C37 (Pg 9, Reg 36,37,38)
// D1 --> C6 (Pg 8, Reg 32,33,34)       C38 (Pg 9, Reg 40,31,41)
//
//
// Text "* Filter 2 BQ "
// 0x7BCDF2
// 0x84320E
// 0x7BCDF2
// 0x7BBC57
// 0x8840E7
Uint32 hpCoefs120Hz_16kHz[BQ_COEF_SZ] = {0x7BCDF2, 0x84320E, 0x7BCDF2, 0x7BBC57, 0x8840E7};
Uint32 hpCoefs120HzOrd1_16kHz[ORD1_COEF_SZ] = {0x7D0D8E, 0x82F272, 0x7A1B1D};

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
static CSL_Status codec_write_reg(Uint16 regAddr,
                                  Uint16 regData)
{
    CSL_Status    status;
    Uint16        writeCount;
    Uint16        writeBuff[2];
    //Uint16        readBuff[2];
    Uint16 startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));

    status = CSL_ESYS_INVPARAMS;

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

#if 0
	/* Give some delay */
	C55x_delay_msec(1);

	// Read the data back for verification
	status = I2C_read(readBuff,
					1,
					I2C_CODEC_ADDR,
					&regAddr,
					1,
					TRUE,
					startStop,
					CSL_I2C_MAX_TIMEOUT,
					FALSE);
	if (status!=CSL_SOK)
	{
		return status;
	}

	if ((readBuff[0]!=writeBuff[1])&&(regAddr>=2))
		status |= (-1);
#endif

	/* Give some delay to make sure I2C is ready for next transfer */
	C55x_delay_msec(1);

    return (status);
}

/* Writes two bytes of data to codec registers */
static CSL_Status codec_write_reg_two(Uint16 regAddr1,
                                      Uint16 regData1,
                                      Uint16 regAddr2,
                                      Uint16 regData2)
{
    CSL_Status    status;
    Uint16        writeCount;
    Uint16        writeBuff1[2];
    Uint16        writeBuff2[2];
    //Uint16        readBuff1[2];
    //Uint16        readBuff2[2];
    Uint16 startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));

    status = CSL_ESYS_INVPARAMS;

	writeCount  =  2;
	/* Initialize the buffer          */
	/* First byte is Register Address */
	/* Second byte is register data   */
	writeBuff1[0] = (regAddr1 & 0x00FF);
	writeBuff1[1] = (regData1 & 0x00FF);

	/* Write the data */
	status = I2C_write(writeBuff1,
					   writeCount,
					   I2C_CODEC_ADDR,
					   TRUE,
					   startStop,
					   CSL_I2C_MAX_TIMEOUT);
	if (status!=CSL_SOK)
	{
		return (status);
	}

	C55x_delay_msec(4);

	writeBuff2[0] = (regAddr2 & 0x00FF);
	writeBuff2[1] = (regData2 & 0x00FF);

	/* Write the data */
	status |= I2C_write(writeBuff2,
					    writeCount,
					    I2C_CODEC_ADDR,
					    TRUE,
					    startStop,
					    CSL_I2C_MAX_TIMEOUT);
	if (status!=CSL_SOK)
	{
		return (status);
	}

#if 0

	/* Give some delay */
	C55x_delay_msec(1);

	// Read the data back for verification
	status = I2C_read(readBuff1,
					1,
					I2C_CODEC_ADDR,
					&regAddr1,
					1,
					TRUE,
					startStop,
					CSL_I2C_MAX_TIMEOUT,
					FALSE);
	if (status!=CSL_SOK)
	{
		return status;
	}

	if ((readBuff1[0]!=writeBuff1[1])&&(regAddr1>=2))
		status |= (-1);

	/* Give some delay */
	C55x_delay_msec(1);

	// Read the data back for verification
	status = I2C_read(readBuff2,
					1,
					I2C_CODEC_ADDR,
					&regAddr2,
					1,
					TRUE,
					startStop,
					CSL_I2C_MAX_TIMEOUT,
					FALSE);
	if (status!=CSL_SOK)
	{
		return status;
	}

	if ((readBuff2[0]!=writeBuff2[1])&&(regAddr2>=2))
		status |= (-1);
#endif

	/* Give some delay */
	C55x_delay_msec(1);

    return status;
}

/* Function to configure audio codec for loopback */
CSL_Status codecConfigLoopback(void)
{
    CSL_Status status;

    /* Configure AIC3206 */
    status = codec_write_reg(0, 0x00);  // Select page 0
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 1,  0x01 );  // Reset codec
	if (status != CSL_SOK)
	{
		return (status);
	}

    C55x_delay_msec(1);  		// Wait 1ms after reset

    status = codec_write_reg( 0,  0x01 );  // Select page 1
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 1,  0x08 );  // Disable crude AVDD generation from DVDD
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 2,  0 );  // Enable Analog Blocks, use LDO power
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 20, 0x25);     // To avoid pop during power ON
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 123,0x05 );  // Force reference to power up in 40ms
	if (status != CSL_SOK)
	{
		return (status);
	}

    C55x_delay_msec(40); 		// Wait at least 40ms
    status = codec_write_reg( 0,  0x00 );  // Select page 0
	if (status != CSL_SOK)
	{
		return (status);
	}

    /* PLL and Clocks config and Power Up  */
    status = codec_write_reg( 27, 0x0d );  // BCLK and WCLK are set as o/p; AIC3206(Master)
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 28, 0x00 );  // Data ofset = 0
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 4,  0x03 );  // PLL setting: PLLCLK <- MCLK, CODEC_CLKIN <-PLL CLK
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 6,  0x07 );  // PLL setting: J=7
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 7,  0x06 );  // PLL setting: HI_BYTE(D=1680)
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 8,  0x90 );  // PLL setting: LO_BYTE(D=1680)
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 30, 0x88 );  // For 32 bit clocks per frame in Master mode ONLY
                               	// BCLK=DAC_CLK/N =(12288000/8) = 1.536MHz = 32*fs
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 5,  0x91 );  // PLL setting: Power up PLL, P=1 and R=1
	if (status != CSL_SOK)
	{
		return (status);
	}

    C55x_delay_msec(1); 		// Wait for PLL to come up
    status = codec_write_reg( 13, 0x00 );  // Hi_Byte(DOSR) for DOSR = 128 decimal or 0x0080 DAC oversamppling
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 14, 0x80 );  // Lo_Byte(DOSR) for DOSR = 128 decimal or 0x0080
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 20, 0x80 );  // AOSR for AOSR = 128 decimal or 0x0080 for decimation filters 1 to 6
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 11, 0x82 );  // Power up NDAC and set NDAC value to 2
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 12, 0x87 );  // Power up MDAC and set MDAC value to 7
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 18, 0x87 );  // Power up NADC and set NADC value to 7
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 19, 0x82 );  // Power up MADC and set MADC value to 2
	if (status != CSL_SOK)
	{
		return (status);
	}

    /* DAC ROUTING and Power Up */
    status = codec_write_reg( 0,  0x01 );  // Select page 1
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 12, 0x08 );  // LDAC AFIR routed to HPL
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 13, 0x08 );  // RDAC AFIR routed to HPR
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 0,  0x00 );  // Select page 0
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 64, 0x02 );  // Left vol=right vol
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 65, 0xF6 );  // Left DAC gain to -5dB VOL; Right tracks Left
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 63, 0xd4 );  // Power up left,right data paths and set channel
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 0,  0x01 );  // Select page 1
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 16, 0x00 );  // Unmute HPL , 0dB gain
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 17, 0x00 );  // Unmute HPR , 0dB gain
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 9 , 0x30 );  // Power up HPL,HPR
	if (status != CSL_SOK)
	{
		return (status);
	}

    C55x_delay_msec(1 );        // Wait 1 msec

    /* ADC ROUTING and Power Up */
    status = codec_write_reg( 0,  0x01 );  // Select page 1
	if (status != CSL_SOK)
	{
		return (status);
	}

	status = codec_write_reg( 10, 0x00);                // CM (common mode)  = 0.9V (default)
	if (status != CSL_SOK)
	{
		return (status);
	}

	status = codec_write_reg( 51, 0x40);                // enable MICBIAS = 2.5V, from LDOIN
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 52, 0xc0 );  // STEREO 1 Jack
                               	// IN2_L to LADC_P through 40 kohm
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 55, 0xc0 );  // IN2_R to RADC_P through 40 kohmm
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 54, 0xc0 );  // CM_1 (common mode) to LADC_M through 40 kohm
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 57, 0xc0 );  // CM_1 (common mode) to RADC_M through 40 kohm
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 59, 0x00 );  // MIC_PGA_L unmute
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 60, 0x00 );  // MIC_PGA_R unmute
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 0,  0x00 );  // Select page 0
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 81, 0xc0 );  // Powerup Left and Right ADC
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 82, 0x00 );  // Unmute Left and Right ADC
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 83, 0x76 );  // Left ADC volume - 5dB
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 84, 0x76 );  // Right ADC volume - 5dB
	if (status != CSL_SOK)
	{
		return (status);
	}

    status = codec_write_reg( 0,  0x00 );  // Select page 0
	if (status != CSL_SOK)
	{
		return (status);
	}

    C55x_delay_msec(1 );        // Wait 1 msec

    return (CSL_SOK);
}

/* Initializes codec */
CSL_Status codecConfigPlayback(Uint16 sampRate,
                               Uint8  input,
                               Bool   useADCHiPass)
{
    CSL_Status result;

    result = codec_write_reg(0, 0); // write 0 to page register to select page 0
    if (result != CSL_SOK)
    {
        return result;
    }

    result = codec_write_reg(1, 1); // reset codec
    if (result != CSL_SOK)
    {
        return result;
    }

    if ((sampRate==SAMP_RATE_16KHZ) || (sampRate==SAMP_RATE_8KHZ))
    {
        /* Set filter coefficients */
        if ((sampRate==SAMP_RATE_16KHZ) && (1==useADCHiPass))
        {
            aic_SetOrd1IIRSection(hpCoefs120HzOrd1_16kHz, ADC_IIR1_PAGE_NUM_LEFT, ADC_IIR1_START_REG_LEFT);
            aic_SetOrd1IIRSection(hpCoefs120HzOrd1_16kHz, ADC_IIR1_PAGE_NUM_RIGHT, ADC_IIR1_START_REG_RIGHT);
            aic_SetADCBiQuad(hpCoefs120Hz_16kHz, ADC_BIQUAD_A);
        }
        else if ((sampRate==SAMP_RATE_8KHZ) && (1==useADCHiPass))
        {
            aic_SetOrd1IIRSection(hpCoefs120HzOrd1_8kHz, ADC_IIR1_PAGE_NUM_LEFT, ADC_IIR1_START_REG_LEFT);
            aic_SetOrd1IIRSection(hpCoefs120HzOrd1_8kHz, ADC_IIR1_PAGE_NUM_RIGHT, ADC_IIR1_START_REG_RIGHT);
            aic_SetADCBiQuad(hpCoefs120Hz_8kHz, ADC_BIQUAD_A);
        }

        //////////////////////////////////////////////////////////
        //         Signal Processing Settings
        //        Page 0
        ///////////////////////////////////////////////////////////
        result = codec_write_reg(0, 0); // write 0 to page register to select page 0
        if (result != CSL_SOK)
        {
            return result;
        }

        /* Page 0 / Register 61 */
        result = codec_write_reg(61, 2); // PRB_R2: Stereo CH, A Decimation Filter, 1st Order IIR, and 5 BiQuads
        if (result != CSL_SOK)
        {
            return result;
        }
    }

    /* Page 0 / Register 4 */
    /* Select the PLL input and CODEC_CLKIN */
    /* PLL input is assumed as 12MHz */
    result = codec_write_reg(4, 0x03);
    if (result != CSL_SOK)
    {
        return result;
    }

    /*
      PLL_CLK = (PLL_CLKIN * R * J.D)/P
      PLL_CLK(CODEC_CLKIN) = 12MHz *(R * J.D)/P

      DAC_FS = PLL_CLKIN/(NDAC * MDAC * DOSR)
      DAC_FS = (12MHz *(R * J.D)/P)/(NDAC * MDAC * DOSR)

      DAC_CLK = PLL_CLK/NDAC

      BCLK = DAC_CLK/BDIV_CLKIN
      BCLK = (12MHz *(R * J.D)/P)/(NDAC * BCLK N)

      ADC_FS = PLL_CLKIN/(NADC * MADC * AOSR)


      DAC_FS, BCLK:
          48 kHz: P=1, R=1, J=8, D=1920 (0x780)
                  NDAC=2, MDAC=8, DOSR=128
                  BCLK = 32
                  PLL_CLK = (12e6 * 1 * 8.192)/1 = 98304000
                  DAC_FS = PLL_CLK/(2 * 8 * 128) = 48000
                  BCLK = PLL_CLK/NDAC/BCLK = 98304000/2/32 = 6144000
          44.1 kHz: P=6, R=1, J=8, D=4672 (0x1240)
                  NDAC=12, MDAC=2, DOSR=16
                  BCLK = 1
                  PLL_CLK = (12e6 * 1 * 8.4672)/6 = 16934400
                  DAC_FS = PLL_CLK/(12 * 2 * 16) = 44100
                  BCLK = PLL_CLK/NDAC/BCLK = 16934400/6/1 = 2822400
          32 kHz: P=3, R=2, J=8, D=1920 (0x780)
                  NDAC=2, MDAC=8, DOSR=128
                  BCLK = 32
                  PLL_CLK = (12e6 * 1 * 8.192)/1 = 98304000
                  DAC_FS = PLL_CLK/(3 * 8 * 128) = 32000
                  BCLK = PLL_CLK/NDAC/BCLK = 98304000/3/32 = 1024000
          16 kHz: P=1, R=1, J=8, D=1920 (0x780)
                  NDAC=8, MDAC=3, DOSR=256
                  BCLK = 24
                  PLL_CLK = (12e6 * 1 * 8.1920)/1 = 98304000
                  DAC_FS = PLL_CLK/(8 * 3 * 256) = 16000
                  BCLK = PLL_CLK/NDAC/BCLK = 98304000/8/24 = 512000
          8 kHz: P=1, R=1, J=8, D=1920 (0x780)
                  NDAC=8 MDAC=3, DOSR=512
                  BCLK = 24
                  PLL_CLK = (12e6 * 1 * 8.1920)/1 = 98304000
                  DAC_FS = PLL_CLK/(8 * 3 * 512) = 8000
                  BCLK = PLL_CLK/NDAC/BCLK = 98304000/8/24 = 512000
      ADC_FS:
          48 kHz: P=1, R=1, J=8, D=1920 (0x780)
                  NADC=2, MADC=8, AOSR=128
                  ADC_FS = PLL_CLK/(2 * 8 * 128) = 48000
          44.1 kHz: P=6, R=1, J=8, D=4672 (0x1240)
                  NADC=12, MADC=2, AOSR=16
                  ADC_FS = PLL_CLK/(12 * 2 * 16) = 44100
          32 kHz: P=1, R=1, J=8, D=1920 (0x780)
                  NADC=3, MADC=8, AOSR=128
                  ADC_FS = PLL_CLK/(3 * 8 * 128) = 32000
          16 kHz: P=1, R=1, J=8, D=1920 (0x780)
                  NADC=16, MADC=3, AOSR=128
                  ADC_FS = PLL_CLK/(16 * 3 * 128) = 16000
          8 kHz: P=1, R=1, J=8, D=1920 (0x780)
                  NADC=32, MADC=3, AOSR=128
                  ADC_FS = PLL_CLK/(32 * 3 * 128) = 8000
    */

    if (sampRate==SAMP_RATE_48KHZ)
    {
        // Power up the PLL and set P = 1 & R = 1
        result = codec_write_reg(5, 0x91); // 48khz
        if (result != CSL_SOK)
        {
            return result;
        }
        // Set J value to 8
        result = codec_write_reg(6, 0x08); // 48khz
        if (result != CSL_SOK)
        {
            return result;
        }
        // Set D value(MSB) = 0x7
        //result = codec_write_reg_Only(7, 0x7); // 48khz
        //if (result != PSP_SOK)
        //{
        //    return result;
        //}
        // Set D value(LSB) = 0x80
        // Set D value = 0x780 => .1920 => J.D = 8.1920
        // Set CODEC_CLKIN = 12Mhz*1*8.192/1 = 98.304Mhz
        result = codec_write_reg_two(7, 0x07, 8, 0x80);
        if (result != CSL_SOK)
        {
            return result;
        }
        // Set NDAC to 2 - this along with BCLK N configures BCLK
        result = codec_write_reg(11,0x82);
        if (result != CSL_SOK)
        {
            return result;
        }
        // Set MDAC to 8
        result = codec_write_reg(12,0x88);
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Set DAC OSR MSB value to 0 */
        result = codec_write_reg(13, 0x0);
        if (result != CSL_SOK)
        {
            return result;
        }
        // Set DAC OSR LSB value to 128
        // Set DOSR = 128, along with NDAC = 2 and MDAC = 8
        // Set DAC_FS = (12MHz *(R * J.D)/P)/(NDAC * MDAC * DOSR) = 48Khz
        result = codec_write_reg(14, 128);
        if (result != CSL_SOK)
        {
            return result;
        }
        // Set BCLK N value to 32
        // This along with NDAC generates the BCLK = 6.144 MHz
        result = codec_write_reg(30,0xA0);
    }
    else if (sampRate==SAMP_RATE_44_1KHZ)
    {
        /* Power up the PLL and set P = 1 & R = 1 */
        result = codec_write_reg(5, 0x91);
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Set J value to 7 */
        result = codec_write_reg(6, 0x07);
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Set D value(MSB) = 0x14 */
        //result = codec_write_reg_Only(7, 0x14);
        //if (result != CSL_SOK)
        //{
        //    return result;
        //}
        /* Set D value(LSB) = 0x90 */
        // Set the D value = 0x1490 = 0.5264, J.D = 7.5264
        // Set CODEC_CLKIN = 12Mhz*1*7.5264/1 = 90.3168MHz
        result = codec_write_reg_two(7, 0x14, 8, 0x90);
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Set NDAC to 2 */
        result = codec_write_reg(11,0x82);
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Set MDAC to 8 */
        result = codec_write_reg(12,0x88);
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Set DAC OSR MSB value to 0 */
        result = codec_write_reg(13, 0x0 );
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Set DAC OSR LSB value to 128 */
        // Set DOSR = 128, along with NDAC = 8 and MDAC = 2
        // Set DAC_FS = (12MHz *(R * J.D)/P)/(NDAC * MDAC * DOSR) = 44.1Khz
        result = codec_write_reg(14, 128 );
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Set BCLK N value to 32 */
        // This along with NDAC generates the BCLK = 1.4112MHz
        result = codec_write_reg(30,0xA0);
    }
    else if (sampRate==SAMP_RATE_32KHZ)
    {
        // Power up the PLL and set P = 1 & R = 1
        result = codec_write_reg(5, 0x91); // 32khz
        if (result != CSL_SOK)
        {
            return result;
        }
        // Set J value to 8
        result = codec_write_reg(6, 0x08); // 32khz
        if (result != CSL_SOK)
        {
            return result;
        }
        // Set D value(MSB) = 0x7
        //result = codec_write_reg_Only(7, 0x07); // 32khz
        //if (result != CSL_SOK)
        //{
        //    return result;
        //}
        // Set D value(LSB) = 0x80
        // Set D value = 0x780 => .1920 => J.D = 8.1920
        // Set CODEC_CLKIN = 12Mhz*1*8.192/1 = 98.304Mhz
        result = codec_write_reg_two(7, 0x07, 8, 0x80); // 32khz
        if (result != CSL_SOK)
        {
            return result;
        }
        // Set NDAC to 3 - this along with BCLK N configures BCLK
        result = codec_write_reg(11,0x83); // 32khz
        if (result != CSL_SOK)
        {
            return result;
        }
        // Set MDAC to 8
        result = codec_write_reg(12,0x88); // 32khz
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Set DAC OSR MSB value to 0 */
        result = codec_write_reg(13, 0x0 );
        if (result != CSL_SOK)
        {
            return result;
        }
        // Set DAC OSR LSB value to 128
        // Set DOSR = 128, along with NDAC = 3 and MDAC = 8
        // Set DAC_FS = (12MHz *(R * J.D)/P)/(NDAC * MDAC * DOSR) = 32Khz
        result = codec_write_reg(14, 128 ); // 32khz
        if (result != CSL_SOK)
        {
            return result;
        }
        // Set BCLK N value to 32
        // This along with NDAC generates the BCLK = 1.4112MHz
        result = codec_write_reg(30,0xA0); // 32khz
    }
    else if (sampRate==SAMP_RATE_16KHZ)
    {
        /* Page 0 / Register 5 */
        // Power up the PLL and set P & R
        result = codec_write_reg(5, 0x91); // 16khz
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Page 0 / Register 6 */
        // Set J value
        result = codec_write_reg(6, 0x08); // 16khz
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Page 0 / Register 7 */
        /* Page 0 / Register 8 */
        // Set D value(MSB)
        // Set D value(LSB)
        result = codec_write_reg_two(7, 0x07, 8, 0x80); // 16khz
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Page 0 / Register 11 */
        // Set NDAC
        //result = codec_write_reg(11, 0x86); // 16khz
        result = codec_write_reg(11, 0x88); // 16khz
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Page 0 / Register 12 */
        // Set MDAC
        //result = codec_write_reg(12,0x88); // 16khz
        result = codec_write_reg(12, 0x83); // 16khz
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Page 0 / Register 13 */
        // Set DAC OSR MSB value
        result = codec_write_reg(13, 0x01);
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Page 0 / Register 14 */
        // Set DAC OSR LSB value
        //result = codec_write_reg(14, 128 ); // 16khz
        result = codec_write_reg(14, 0x00 ); // 16khz
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Page 0 / Register 30 */
        // Set BCLK N
        //result = codec_write_reg(30, 0xA0); // 16khz
        result = codec_write_reg(30, 0x98); // 16khz
    }
    else if (sampRate==SAMP_RATE_8KHZ)
    {
        /* Page 0 / Register 5 */
        // Power up the PLL and set P & R
        result = codec_write_reg(5, 0x91); // 8khz
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Page 0 / Register 6 */
        // Set J value
        result = codec_write_reg(6, 0x08); // 8khz
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Page 0 / Register 7 */
        /* Page 0 / Register 8 */
        // Set D value(MSB)
        // Set D value(LSB)
        result = codec_write_reg_two(7, 0x07, 8, 0x80); // 8khz
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Page 0 / Register 11 */
        // Set NDAC
        //result = codec_write_reg(11, 0x8C); // 8khz
        result = codec_write_reg(11, 0x88); // 8khz
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Page 0 / Register 12 */
        // Set MDAC
        result = codec_write_reg(12, 0x83); // 8khz
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Page 0 / Register 13 */
        /* Set DAC OSR MSB value */
        result = codec_write_reg(13, 0x02);
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Page 0 / Register 14 */
        // Set DAC OSR LSB value
        //result = codec_write_reg(14, 128 ); // 16khz
        result = codec_write_reg(14, 0x00 ); // 16khz
        if (result != CSL_SOK)
        {
            return result;
        }
        /* Page 0 / Register 30 */
        // Set BCLK N
        result = codec_write_reg(30, 0x98); // 8khz
        if (result != CSL_SOK)
        {
            return result;
        }
    }
    else
    {
        result = CSL_ESYS_INVPARAMS;
    }

    // Set DAC Signal Processing Block to PRB_R1
    result = codec_write_reg(60, 0x01);
    if (result != CSL_SOK)
    {
        return result;
    }

	/* Configure ADC sampling rate */
    if (sampRate==SAMP_RATE_48KHZ)
    {
        //
        // Set ADC_FS to 48 kHZ
        //

        // Set NADC to 2
        result = codec_write_reg(18,0x82);
        if (result != CSL_SOK)
        {
            return result;
        }

        // Set MADC to 8
        result = codec_write_reg(19, 0x88);
        if (result != CSL_SOK)
        {
            return result;
        }

        // Set ADC OSR LSB value to 128
        // This generates the ADC_FS = 48KHz
        // ADC_FS = (12MHz *(R * J.D)/P)/(NADC * MADC * AOSR)
        result = codec_write_reg(20, 128 );
        if (result != CSL_SOK)
        {
            return result;
        }
    }
    else if (sampRate==SAMP_RATE_44_1KHZ)
    {
        //
        // Set ADC_FS to 44.1 kHZ

        // Set NADC to 2
        result = codec_write_reg(18, 0x82);
        if (result != CSL_SOK)
        {
            return result;
        }

        // Set MADC to 8
        result = codec_write_reg(19, 0x88);
        if (result != CSL_SOK)
        {
            return result;
        }

        // Set ADC OSR LSB value to 128
        // This generates the ADC_FS = 44.1KHz
        // ADC_FS = (12MHz *(R * J.D)/P)/(NADC * MADC * AOSR)
        result = codec_write_reg(20, 128 );
        if (result != CSL_SOK)
        {
            return result;
        }
    }
    else if (sampRate==SAMP_RATE_32KHZ)
    {
        //
        // Set ADC_FS to 32 kHZ

        // Set NADC to 3
        result = codec_write_reg(18, 0x83);
        if (result != CSL_SOK)
        {
            return result;
        }

        // Set MADC to 8
        result = codec_write_reg(19, 0x88);
        if (result != CSL_SOK)
        {
            return result;
        }

        // Set ADC OSR LSB value to 128
        // This generates the ADC_FS = 32KHz
        // ADC_FS = (12MHz *(R * J.D)/P)/(NADC * MADC * AOSR)
        result = codec_write_reg(20, 128 );
        if (result != CSL_SOK)
        {
            return result;
        }
    }
    else if (sampRate==SAMP_RATE_16KHZ)
    {
        //
        // Set ADC_FS to 16 kHZ

        /* Page 0 / Register 18 */
        // Set NADC
        //result = codec_write_reg(18, 0x86);
        result = codec_write_reg(18, 0x90);
        if (result != CSL_SOK)
        {
            return result;
        }

        /* Page 0 / Register 19 */
        // Set MADC
        //result = codec_write_reg(19, 0x88);
        result = codec_write_reg(19, 0x83);
        if (result != CSL_SOK)
        {
            return result;
        }

        /* Page 0 / Register 20 */
        // Set ADC OSR LSB
        result = codec_write_reg(20, 128);
        if (result != CSL_SOK)
        {
            return result;
        }
    }
    else if (sampRate==SAMP_RATE_8KHZ)
    {
        //
        // Set ADC_FS to 8 kHZ

        /* Page 0 / Register 18 */
        // Set NADC
        //result = codec_write_reg(18, 0x8C);
        result = codec_write_reg(18, 0xa0);
        if (result != CSL_SOK)
        {
            return result;
        }

        /* Page 0 / Register 19 */
        // Set MADC
        result = codec_write_reg(19, 0x83);
        if (result != CSL_SOK)
        {
            return result;
        }

        /* Page 0 / Register 20 */
        // Set ADC OSR LSB
        result = codec_write_reg(20, 128);
        if (result != CSL_SOK)
        {
            return result;
        }
    }
    else
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "RECORD FORMAT 0x%x NOT SUPPORTED\n", sampRate);
#endif
    }

    result = codec_write_reg(27,0xd); // BCLK and WCLK is set as op to codec(Master)
    if (result != CSL_SOK)
    {
        return result;
    }

    result = codec_write_reg(0,1);// select page 1
    if (result != CSL_SOK)
    {
        return result;
    }

    result = codec_write_reg(0x1,0x8);// Disable crude AVDD generation from DVDD
    if (result != CSL_SOK)
    {
        return result;
    }

    result = codec_write_reg(0x2,0);// Enable Analog Blocks
    if (result != CSL_SOK)
    {
        return result;
    }

    result = codec_write_reg(20, 0x25);// Depop reg R=6K,t=8RC(2.256ms),ramp time=0ms
    if (result != CSL_SOK)
    {
        return result;
    }

    result = codec_write_reg(0xc, 0x8);// LDAC AFIR routed to HPL
    if (result != CSL_SOK)
    {
        return result;
    }

    result = codec_write_reg(0xd, 0x8);// RDAC AFIR routed to HPR
    if (result != CSL_SOK)
    {
        return result;
    }

	/* Configure Audio input */
    result = codecInputSelect((tAICInSelect)input);
    if (result)
    {
        return result;
    }

    result = codec_write_reg(0, 0); // write 0 to page register to select page 0
    if (result != CSL_SOK)
    {
        return result;
    }

    /* Page 0 / Register 83 */
    /* Left ADC Channel Volume Control Register */
    result = codec_write_reg(83, gADCDigGain[0]);
    if (result != CSL_SOK)
    {
        return result;
    }

    /* Page 0 / Register 84 */
    /* Right ADC Channel Volume Control Register */
    result = codec_write_reg(84, gADCDigGain[1]);
    if (result != CSL_SOK)
    {
        return result;
    }

    result = codec_write_reg(0, 0); // write 0 to page register to select page 0
    if (result != CSL_SOK)
    {
        return result;
    }

    result = codec_write_reg(63,0xd4); // power up left,right data paths and set channel
    if (result != CSL_SOK)
    {
        return result;
    }

    result = codec_write_reg(64,0xc); // left vol=right vol; muted
    if (result != CSL_SOK)
    {
        return result;
    }

    result = codec_write_reg(0,1);// select page 1
    if (result != CSL_SOK)
    {
        return result;
    }

    result = codec_write_reg(0x9,0x30);// power up HPL,HPR
    if (result != CSL_SOK)
    {
        return result;
    }

    result = codec_write_reg(0x0,0x0);// select page 0
    if (result != CSL_SOK)
    {
        return result;
    }

	result = codec_write_reg(65, 0xEC);// set DAC volume to -10dB
	if (result != CSL_SOK)
	{
		return result;
	}

    result = codec_write_reg(0x40,0x2);// unmute DAC with right vol=left vol
    if (result != CSL_SOK)
    {
        return result;
    }

    result = codec_write_reg(0x0,0x1);// select page 1
    if (result != CSL_SOK)
    {
        return result;
    }

    result = codec_write_reg(0x10,0);// unmute HPL , 0dB gain
    if (result != CSL_SOK)
    {
        return result;
    }

    result = codec_write_reg(0x11,0);// unmute HPR , 0dB gain
    if (result != CSL_SOK)
    {
        return result;
    }

    // write 1 to page register to select page 1 - prepare for next headset volume change
    result = codec_write_reg(0, 1);
    if (result != CSL_SOK)
    {
        return FALSE;
    }

    return result;
}

Int16 codecInputSelect(tAICInSelect input)
{
    switch (input)
    {
        case AIC_LINE:
            codec_write_reg(  0, 0x01 );                // Select page 1
            codec_write_reg( 10, 0x00 );                // CM (common mode)  = 0.9V (default)
            codec_write_reg( 51, 0x40 );                // enable MICBIAS = 2.5V, from LDOIN
            codec_write_reg( 52, 0xc0 );                // MICROPHONE IN Jack
                                                                      // IN2_L to LADC_P through 40 kohm
            codec_write_reg( 55, 0xc0 );                // IN2_R to RADC_P through 10 kohmm
            codec_write_reg( 54, 0xc0 );                // CM1L (common mode) to LADC_M through 40 kohm
            codec_write_reg( 57, 0xc0 );                // CM1R (common mode) to RADC_M through 40 kohm
            codec_write_reg( 59, 0 );                   // MIC_PGA_L unmute
            codec_write_reg( 60, 0 );                   // MIC_PGA_R unmute
            //codec_write_reg( 59, 0x0f );                // MIC_PGA_L enabled 7.5db
            //codec_write_reg( 60, 0x0f );                // MIC_PGA_R enabled 7.5db
            codec_write_reg(  0, 0x00 );                // Select page 0
            codec_write_reg( 81, 0xc0 );                // Powerup Left and Right ADC
            codec_write_reg( 82, 0x00 );                // Unmute Left and Right ADC

            break;
        case AIC_AMIC:
            /* ADC ROUTING and Power Up */
            codec_write_reg(  0, 0x01 );                // Select page 1
            codec_write_reg( 10, 0x00 );                // CM (common mode)  = 0.9V (default)
            //codec_write_reg( 51, 0x40 );                // enable MICBIAS = 1.25V, from AVdd
            //codec_write_reg( 51, 0x48 );                // enable MICBIAS = 1.25V, from LDOIN
           // codec_write_reg( 51, 0x68 );                // enable MICBIAS = 2.5V, from LDOIN
            //codec_write_reg( 52, 0xc0 );                // MICROPHONE IN Jack
                                                            // IN1_L to LADC_P through 40 kohm
            //codec_write_reg( 55, 0xc0 );                // IN1_R to RADC_P through 40 kohmm
            //codec_write_reg( 52, 0x30 );                // MICROPHONE IN Jack
                                                            // IN2_L to LADC_P through 40 kohm
            //codec_write_reg( 55, 0x30 );                // IN2_R to RADC_P through 40 kohmm

            codec_write_reg( 51, 0x40 );                // enable MICBIAS = 2.5V, from LDOIN
            codec_write_reg( 52, 0x30 );                // MICROPHONE IN Jack
                                                            // IN2_L to LADC_P through 10 kohm
            codec_write_reg( 55, 0x03 );                // IN2_R to RADC_P through 10 kohmm
            codec_write_reg( 54, 0x03 );                // CM1L (common mode) to LADC_M through 40 kohm
            codec_write_reg( 57, 0xc0 );                // CM1R (common mode) to RADC_M through 40 kohm

            codec_write_reg( 59, 0x5f );                // MIC_PGA_L enabled 47.5db
            codec_write_reg( 60, 0x5f );                // MIC_PGA_R enabled 47.5db
            //codec_write_reg( 59, 0x0f );                // MIC_PGA_L enabled 7.5db
            //codec_write_reg( 60, 0x0f );                // MIC_PGA_R enabled 7.5db
            codec_write_reg(  0, 0x00 );                // Select page 0
            codec_write_reg( 81, 0xc0 );                // Powerup Left and Right ADC
            codec_write_reg( 82, 0x00 );                // Unmute Left and Right ADC
            break;
        case AIC_DMIC:
            /* ADC ROUTING and Power Up */
            codec_write_reg(  0, 0x00 );                // Select page 0
            //codec_write_reg( 10, 0x00 );                // CM (common mode)  = 0.9V (default)
            codec_write_reg( 52, 0x04 );                // Set GPIO for DMIC input = 0x04
            //codec_write_reg( 56, 0x02 );              // SCLK - Digital Mic input
            codec_write_reg( 55, 0x0E );                // MISO - CLK out for Digital Mic

            codec_write_reg(  0, 0x01 );                // Select page 1
            codec_write_reg( 51, 0x00 );                // MICBIAS powered Down
            codec_write_reg( 52, 0x00 );                // no signal routed to Left PGA+
            codec_write_reg( 55, 0x00 );                // no signal routed to Right PGA+
            codec_write_reg( 54, 0x00 );                // no signal Routed to Left PGA-
            codec_write_reg( 57, 0x00 );                // no signal Routed to Right PGA-
            codec_write_reg( 59, 0x80 );                // MIC_PGA_L set to 0db
            codec_write_reg( 60, 0x80 );                // MIC_PGA_R set to 0db
            codec_write_reg(  0, 0x00 );                // Select page 0
            codec_write_reg( 81, 0xCC );                // Powerup Left and Right ADC, L & R ADC configured for Dig Mic, GPIO = Dig Mic input
            //codec_write_reg( 81, 0xDC );                // Powerup Left and Right ADC, L & R ADC configured for Dig Mic, SCLK = Dig Mic input
            codec_write_reg( 82, 0x00 );                // unmute Left and Right ADC
            break;
        default:
            break;

    }
    return 0;
}

// support function
// write the coefs to 3 connescutive starting at the base register at provided page.
void aic_writeCoef(
    Uint32 theCoef,
    Uint16 pageNum,
    Uint16 baseRegNumber)
{
    //coef as |byte3 byte2 byte1 byte0| -->  |XX byte2 byte1 byte0|
    Uint32 isoByte;
    Uint16 theCoefByte;
    // write the coefs to 4 consecutive register locations starting at pageNum,baseRegNumber
    codec_write_reg(  0, pageNum );  //select the page;
    isoByte=theCoef&(0x00ff0000);
    isoByte=isoByte>>16;
    theCoefByte=((Int16)isoByte) & (0xff);
    codec_write_reg(baseRegNumber++,theCoefByte);

    isoByte=theCoef&(0x0000ff00);
    isoByte=isoByte>>8;
    theCoefByte=((Int16)isoByte) & (0xff);
    codec_write_reg(baseRegNumber++,theCoefByte);

    isoByte=theCoef&(0x000000ff);
  //isoByte=isoByte>>8;
    theCoefByte=((Int16)isoByte) & (0xff);
    codec_write_reg(baseRegNumber++,theCoefByte);
}

// support function
// write the coefs to the 3 consecutive registers at the base register
// assumes page has already been set
void aic_writeCoef_2(
    Uint32 theCoef,
    Uint16 baseRegNumber)
{
    //coef as |byte3 byte2 byte1 byte0| -->  |XX byte2 byte1 byte0|
    Uint32 isoByte;
    Uint16 theCoefByte;
    // write the 24 bit coef to 3 consecutive 8 bit register locations starting at baseRegNumber
    isoByte=theCoef&(0x00ff0000);
    isoByte=isoByte>>16;
    theCoefByte=((Int16)isoByte) & (0xff);
    codec_write_reg(baseRegNumber++,theCoefByte);

    isoByte=theCoef&(0x0000ff00);
    isoByte=isoByte>>8;
    theCoefByte=((Int16)isoByte) & (0xff);
    codec_write_reg(baseRegNumber++,theCoefByte);

    isoByte=theCoef&(0x000000ff);
    theCoefByte=((Int16)isoByte) & (0xff);
    codec_write_reg(baseRegNumber++,theCoefByte);
}

// Writes the provided 1st order IIR coefs registers at pageNum:baseRegNumber
// H(z) = (N0 + N1/z) / (8388608 - D1/z)"
// Filter Coefficients in format N0, N1, D1"
//
// Although the passed coefs are 32bit, the aic coefs are 24 bit. I write the bottom three nibbles.
// The coefs are typically generated by ti BQ tool.

void aic_SetOrd1IIRSection(
    Uint32 coefs[ORD1_COEF_SZ],
    Uint16 pageNum,
    Uint16 baseRegNumber)
{
    Uint16 k;
    codec_write_reg(  0, pageNum );  //select the page that this set is on.
    for (k=0;k<ORD1_COEF_SZ;k++)
    {
        aic_writeCoef_2(coefs[k], baseRegNumber);
        baseRegNumber+=4; // 4 registers per coef, specify the start register of the next coef
    }
}


// Writes the provided BiQuad coefs to coefs registers at pageNum:baseRegNumber
//
// send the provided coefs to the BIQUAD A section DAC of the AIC.
// biCoefs =[N0 N1 N2 D1 D2] for H(z) of
// H(z) = N0 + 2*N1z^(-1) + N2z^(-2)
//        -------------------------
//        2^23 - 2*D1z^(-1) - D2z^(-2)
//
// Although the passed coefs are 32bit, the aic coefs are 24 bit. I write the bottom three nibbles.
// The coefs are typically generated by ti BQ tool.
//
// Here is an example output from the tool:
//Text "* Sample Rate = 16000"
//Text "* Filter 1 High Pass 2nd Order Butterworth, 250 Hz Fc 0.0 dB "

//Text "* BQ: H(z) = (N0 + 2*N1/z + N2/(z2)) / (8388608 - 2*D1/z - D2/(z2))"
//Text "* Filter Coefficients in format N0, N1, N2, D1, D2"

//Text "* 1O: H(z) = (N0 + N1/z) / (8388608 - D1/z)"
//Text "* Filter Coefficients in format N0, N1, D1"

//Text "* Filter 1 BQ "
//0x776A04
//0x8895FC
//0x776A04
//0x77203D
//0x909867
void aic_SetBiQuadSection(
    Uint32 biCoefs[BQ_COEF_SZ],
    Uint16 pageNum,
    Uint16 baseRegNumber)
{
    Uint16 kk;
    codec_write_reg(  0, pageNum );  //select the page that this set is on.
    for (kk=0;kk<BQ_COEF_SZ;kk++)
    {
        aic_writeCoef_2(biCoefs[kk], baseRegNumber);
        baseRegNumber+=4; // 4 registers per coef, specify the start register of the next coef
    }
}

// mapping of biquad section to coef register, ADC, table 5-6 of AIC3204 data sheet.
Uint16 aicADCBiQuadPageNumsLeft[ADC_MAX_BIQUADS] ={8,8,8,8,8};  // page number of each register for each biquad section
Uint16 aicADCBiQuadPageNumsRight[ADC_MAX_BIQUADS]={9,9,9,9,9};

// mapping of biquad section to coef register, ADC, table 5-6 of AIC3204 data sheet.
Uint16 aicADCBiQuadStartRegLeft[ADC_MAX_BIQUADS]={36,56,76,96,116}; // starting register number for the first coef of that section.
Uint16 aicADCBiQuadStartRegRight[ADC_MAX_BIQUADS]={44,64,84,104,124}; // starting register number for the first coef of that section.

void aic_SetADCBiQuad(
    Uint32 biCoefs[BQ_COEF_SZ],
    tAICADCBiQuad BQIdentifier)
{
    // for biquad sections A thru E, all the registers are contiguous:
    switch (BQIdentifier)
    {
        case ADC_BIQUAD_A:
        case ADC_BIQUAD_B:
        case ADC_BIQUAD_C:
        case ADC_BIQUAD_D:
            aic_SetBiQuadSection(biCoefs, aicADCBiQuadPageNumsLeft[BQIdentifier], aicADCBiQuadStartRegLeft[BQIdentifier]);
            aic_SetBiQuadSection(biCoefs, aicADCBiQuadPageNumsRight[BQIdentifier], aicADCBiQuadStartRegRight[BQIdentifier]);
            break;
        case ADC_BIQUAD_E:
            // this section spans two pages, special case
            // Write to LEFT ADC CHANNEL, BIQUAD E
            aic_writeCoef(biCoefs[0], 8, 116);    // write the first (N0) coef, starting at page 8, register 116,
            aic_writeCoef(biCoefs[1], 8, 120);    // N1
            aic_writeCoef(biCoefs[2], 8, 124);    // N2
            aic_writeCoef(biCoefs[3], 9, 8);      // D1
            aic_writeCoef(biCoefs[4], 9, 12);     // D2
            // Write to RIGHT ADC CHANNEL, BIQUAD E
            aic_writeCoef(biCoefs[0], 9, 124);    // write the first (N0) coef, starting at page 9, register 124,
            aic_writeCoef(biCoefs[1], 10, 8);     // N1
            aic_writeCoef(biCoefs[2], 10, 12);    // N2
            aic_writeCoef(biCoefs[3], 10, 16);    // D1
            aic_writeCoef(biCoefs[4], 10, 20);    // D2
            break;
        default:
            break;
    }
}

