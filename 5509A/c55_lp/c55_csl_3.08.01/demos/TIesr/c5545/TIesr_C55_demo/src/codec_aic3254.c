/*
* $$$MODULE_NAME codec_aic3254.c
*
* $$$MODULE_DESC codec_aic3254.c
*
* Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
*
*  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
*  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
*  software download.
*/

#include "codec_aic3254.h"
#include "sample_rate.h"

#define ENABLE_RECORD

#define I2C_CODEC_ADDR          (0x18)
#define I2C_EEPROM_ADDR         (0x50)

#define BQ_COEF_SZ (5)
#define ORD1_COEF_SZ (3)

// private function prototypes
void aic_writeCoef(Uint32 theCoef, Uint16 pageNum, Uint16 baseRegNumber, pI2cHandle hi2c);
void aic_writeCoef_2(Uint32 theCoef, Uint16 baseRegNumber, pI2cHandle hi2c);
void aic_SetOrd1IIRSection(Uint32 coefs[ORD1_COEF_SZ], Uint16 pageNum, Uint16 baseRegNumber, pI2cHandle hi2c);
void aic_SetBiQuadSection(Uint32 biCoefs[BQ_COEF_SZ], Uint16 pageNum, Uint16 baseRegNumber, pI2cHandle hi2c);
void aic_SetADCBiQuad(Uint32 biCoefs[BQ_COEF_SZ], tAICADCBiQuad BQIdentifier, pI2cHandle hi2c);

tAICInSelect    gAppInputSelect     =AIC_AMIC;  // choices are: AIC_LINE, AIC_AMIC, AIC_DMIC;
//tAICInSelect    gAppInputSelect     =AIC_LINE;  // choices are: AIC_LINE, AIC_AMIC, AIC_DMIC;
Uint16          gAppUseADC_HiPass   =1;         // determines whether ADC high pass filter used for 8 and 16 kHz sampling rates

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
CSL_Status AIC3254_Write(
    Uint16 regAddr, 
    Uint16 regData, 
    pI2cHandle hi2c
)
{
    CSL_Status    status;
    Uint16        writeCount;
    volatile Uint16 looper;
    Uint16        writeBuff[2];
    Uint16        readBuff[2];
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

#if 1
        // Read the data back for verification
        // dummy write for the register address
        status = I2C_write(&regAddr, 
                        1,
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

#endif
    }

    return status;
}

/* Writes two bytes of data to codec registers */
CSL_Status AIC3254_Write_Two(
    Uint16 regAddr1, 
    Uint16 regData1, 
    Uint16 regAddr2, 
    Uint16 regData2, 
    pI2cHandle hi2c
)
{
    CSL_Status    status;
    Uint16        writeCount;
    volatile Uint16 looper;
    Uint16        writeBuff1[2];
    Uint16        writeBuff2[2];
    Uint16        readBuff1[2];
    Uint16        readBuff2[2];
    Uint16 startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));

    status = CSL_ESYS_INVPARAMS;

    if(hi2c != NULL)
    {
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
            return status;
        }

        /* Give some delay */
        for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}

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
            return status;
        }

        /* Give some delay */
        for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}

#if 1
        // Read the data back for verification
        // dummy write for the register address
        status |= I2C_write(&regAddr1, 
                        1,
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

        /* Give some delay */
        for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}

        // Read the data back for verification
        // dummy write for the register address
        status = I2C_write(&regAddr2, 
                        1,
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

#endif
    }

    return status;
}

/**
*  \brief Codec read function
*
*  Function to read a byte of data from a codec register.
*
*  \param regAddr  [IN]  Address of the register to read the data
*  \param data     [IN]  Pointer to the data read from codec register
*
*  \return CSL_SOK - if successful, else suitable error code
*/
CSL_Status AIC3254_Read(
    Uint16 regAddr, 
    Uint16 *data, 
    pI2cHandle  hi2c
)
{
    CSL_Status status  = CSL_ESYS_INVPARAMS;
    Uint16 readCount = 1;
    volatile Uint16 looper;
    Uint16 readBuff[1];
    Uint16 startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));
    
    regAddr = (regAddr & 0x00FF);

    if (hi2c != NULL)
    {
        // dummy write for the register address
        status = I2C_write(&regAddr, 
                        1,
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

        return status;
    } else
    {
        // illegal I2C handle
        return CSL_ESYS_BADHANDLE;
    }
}

/* Initializes codec */
CSL_Status AIC3254_init(
    Uint16 sampRatePb, 
    Uint16 sampRateRec, 
    pI2cHandle hi2c
)
{
    CSL_Status result;
    volatile Uint32 looper;

#ifdef PF_C5515_EVM
    /* Reset AIC3254 */
    /* NOTE: Assumes EBSR and GPIO are set correctly before function is called */
    CSL_GPIO_REGS->IOOUTDATA1 = 0x0000; /* reset active low */
    for(looper=0; looper<10; looper++ )
        asm("    nop");
    CSL_GPIO_REGS->IOOUTDATA1 = 0x0400;
#endif

    result = AIC3254_Write(0, 0, hi2c); // write 0 to page register to select page 0
    if (result != CSL_SOK) 
    {
        return result;
    }

    result = AIC3254_Write(1, 1, hi2c); // reset codec
    if (result != CSL_SOK) 
    {
        return result;
    }

    if ((sampRateRec==SAMP_RATE_16KHZ) || (sampRateRec==SAMP_RATE_8KHZ))
    {
        /* Set filter coefficients */
        if ((sampRateRec==SAMP_RATE_16KHZ) && (1==gAppUseADC_HiPass))
        {
            aic_SetOrd1IIRSection(hpCoefs120HzOrd1_16kHz, ADC_IIR1_PAGE_NUM_LEFT, ADC_IIR1_START_REG_LEFT, hi2c);
            aic_SetOrd1IIRSection(hpCoefs120HzOrd1_16kHz, ADC_IIR1_PAGE_NUM_RIGHT, ADC_IIR1_START_REG_RIGHT, hi2c);
            aic_SetADCBiQuad(hpCoefs120Hz_16kHz, ADC_BIQUAD_A, hi2c);
        }
        else if ((sampRateRec==SAMP_RATE_8KHZ) && (1==gAppUseADC_HiPass))
        {
            aic_SetOrd1IIRSection(hpCoefs120HzOrd1_8kHz, ADC_IIR1_PAGE_NUM_LEFT, ADC_IIR1_START_REG_LEFT, hi2c);
            aic_SetOrd1IIRSection(hpCoefs120HzOrd1_8kHz, ADC_IIR1_PAGE_NUM_RIGHT, ADC_IIR1_START_REG_RIGHT, hi2c);
            aic_SetADCBiQuad(hpCoefs120Hz_8kHz, ADC_BIQUAD_A, hi2c);
        }

        //////////////////////////////////////////////////////////
        //         Signal Processing Settings
        //        Page 0
        ///////////////////////////////////////////////////////////
        result = AIC3254_Write(0, 0, hi2c); // write 0 to page register to select page 0
        if (result != CSL_SOK)
        {
            return result;
        }

        /* Page 0 / Register 61 */
        result = AIC3254_Write(61, 2, hi2c); // PRB_R2: Stereo CH, A Decimation Filter, 1st Order IIR, and 5 BiQuads
        if (result != CSL_SOK)
        {
            return result;
        }
    }

    /* Page 0 / Register 4 */
    /* Select the PLL input and CODEC_CLKIN */
    /* PLL input is assumed as 12MHz */
    result = AIC3254_Write(4, 0x03, hi2c);
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

    if (sampRatePb==SAMP_RATE_48KHZ)
    {
        // Power up the PLL and set P = 1 & R = 1
        result = AIC3254_Write(5, 0x91, hi2c); // 48khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        // Set J value to 8
        result = AIC3254_Write(6, 0x08, hi2c); // 48khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        // Set D value(MSB) = 0x7
        //result = AIC3254_Write_Only(7, 0x7, hi2c); // 48khz
        //if (result != PSP_SOK) 
        //{
        //    return result;
        //}
        // Set D value(LSB) = 0x80
        // Set D value = 0x780 => .1920 => J.D = 8.1920 
        // Set CODEC_CLKIN = 12Mhz*1*8.192/1 = 98.304Mhz
        result = AIC3254_Write_Two(7, 0x07, 8, 0x80, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }
        // Set NDAC to 2 - this along with BCLK N configures BCLK
        result = AIC3254_Write(11,0x82, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }
        // Set MDAC to 8
        result = AIC3254_Write(12,0x88, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Set DAC OSR MSB value to 0 */
        result = AIC3254_Write(13, 0x0, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }
        // Set DAC OSR LSB value to 128
        // Set DOSR = 128, along with NDAC = 2 and MDAC = 8
        // Set DAC_FS = (12MHz *(R * J.D)/P)/(NDAC * MDAC * DOSR) = 48Khz
        result = AIC3254_Write(14, 128, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }
        // Set BCLK N value to 32
        // This along with NDAC generates the BCLK = 6.144 MHz
        result = AIC3254_Write(30,0xA0, hi2c);
    } 
    else if (sampRatePb==SAMP_RATE_44_1KHZ)
    {
        /* Power up the PLL and set P = 1 & R = 1 */
        result = AIC3254_Write(5, 0x91, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Set J value to 7 */
        result = AIC3254_Write(6, 0x07, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Set D value(MSB) = 0x14 */
        //result = AIC3254_Write_Only(7, 0x14, hi2c);
        //if (result != CSL_SOK) 
        //{
        //    return result;
        //}
        /* Set D value(LSB) = 0x90 */
        // Set the D value = 0x1490 = 0.5264, J.D = 7.5264
        // Set CODEC_CLKIN = 12Mhz*1*7.5264/1 = 90.3168MHz
        result = AIC3254_Write_Two(7, 0x14, 8, 0x90, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Set NDAC to 2 */
        result = AIC3254_Write(11,0x82, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Set MDAC to 8 */
        result = AIC3254_Write(12,0x88, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Set DAC OSR MSB value to 0 */
        result = AIC3254_Write(13, 0x0, hi2c );
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Set DAC OSR LSB value to 128 */
        // Set DOSR = 128, along with NDAC = 8 and MDAC = 2
        // Set DAC_FS = (12MHz *(R * J.D)/P)/(NDAC * MDAC * DOSR) = 44.1Khz
        result = AIC3254_Write(14, 128, hi2c );
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Set BCLK N value to 32 */
        // This along with NDAC generates the BCLK = 1.4112MHz
        result = AIC3254_Write(30,0xA0, hi2c);
    } 
    else if (sampRatePb==SAMP_RATE_32KHZ)
    {
        // Power up the PLL and set P = 1 & R = 1
        result = AIC3254_Write(5, 0x91, hi2c); // 32khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        // Set J value to 8
        result = AIC3254_Write(6, 0x08, hi2c); // 32khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        // Set D value(MSB) = 0x7
        //result = AIC3254_Write_Only(7, 0x07, hi2c); // 32khz
        //if (result != CSL_SOK) 
        //{
        //    return result;
        //}
        // Set D value(LSB) = 0x80
        // Set D value = 0x780 => .1920 => J.D = 8.1920 
        // Set CODEC_CLKIN = 12Mhz*1*8.192/1 = 98.304Mhz
        result = AIC3254_Write_Two(7, 0x07, 8, 0x80, hi2c); // 32khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        // Set NDAC to 3 - this along with BCLK N configures BCLK
        result = AIC3254_Write(11,0x83, hi2c); // 32khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        // Set MDAC to 8
        result = AIC3254_Write(12,0x88, hi2c); // 32khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Set DAC OSR MSB value to 0 */
        result = AIC3254_Write(13, 0x0, hi2c );
        if (result != CSL_SOK) 
        {
            return result;
        }
        // Set DAC OSR LSB value to 128
        // Set DOSR = 128, along with NDAC = 3 and MDAC = 8
        // Set DAC_FS = (12MHz *(R * J.D)/P)/(NDAC * MDAC * DOSR) = 32Khz
        result = AIC3254_Write(14, 128, hi2c ); // 32khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        // Set BCLK N value to 32
        // This along with NDAC generates the BCLK = 1.4112MHz
        result = AIC3254_Write(30,0xA0, hi2c); // 32khz
    } 
    else if (sampRatePb==SAMP_RATE_16KHZ)
    {
        /* Page 0 / Register 5 */
        // Power up the PLL and set P & R
        result = AIC3254_Write(5, 0x91, hi2c); // 16khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Page 0 / Register 6 */
        // Set J value
        result = AIC3254_Write(6, 0x08, hi2c); // 16khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Page 0 / Register 7 */
        /* Page 0 / Register 8 */
        // Set D value(MSB)
        // Set D value(LSB)
        result = AIC3254_Write_Two(7, 0x07, 8, 0x80, hi2c); // 16khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Page 0 / Register 11 */
        // Set NDAC
        //result = AIC3254_Write(11, 0x86, hi2c); // 16khz
        result = AIC3254_Write(11, 0x88, hi2c); // 16khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Page 0 / Register 12 */
        // Set MDAC
        //result = AIC3254_Write(12,0x88, hi2c); // 16khz
        result = AIC3254_Write(12, 0x83, hi2c); // 16khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Page 0 / Register 13 */
        // Set DAC OSR MSB value
        result = AIC3254_Write(13, 0x01, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Page 0 / Register 14 */
        // Set DAC OSR LSB value
        //result = AIC3254_Write(14, 128, hi2c ); // 16khz
        result = AIC3254_Write(14, 0x00, hi2c ); // 16khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Page 0 / Register 30 */
        // Set BCLK N
        //result = AIC3254_Write(30, 0xA0, hi2c); // 16khz
        result = AIC3254_Write(30, 0x98, hi2c); // 16khz
    } 
    else if (sampRatePb==SAMP_RATE_8KHZ)
    {
        /* Page 0 / Register 5 */
        // Power up the PLL and set P & R
        result = AIC3254_Write(5, 0x91, hi2c); // 8khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Page 0 / Register 6 */
        // Set J value
        result = AIC3254_Write(6, 0x08, hi2c); // 8khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Page 0 / Register 7 */
        /* Page 0 / Register 8 */
        // Set D value(MSB)
        // Set D value(LSB)
        result = AIC3254_Write_Two(7, 0x07, 8, 0x80, hi2c); // 8khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Page 0 / Register 11 */
        // Set NDAC
        //result = AIC3254_Write(11, 0x8C, hi2c); // 8khz
        result = AIC3254_Write(11, 0x88, hi2c); // 8khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Page 0 / Register 12 */
        // Set MDAC
        result = AIC3254_Write(12, 0x83, hi2c); // 8khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Page 0 / Register 13 */
        /* Set DAC OSR MSB value */
        result = AIC3254_Write(13, 0x02, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Page 0 / Register 14 */
        // Set DAC OSR LSB value
        //result = AIC3254_Write(14, 128, hi2c ); // 16khz
        result = AIC3254_Write(14, 0x00, hi2c ); // 16khz
        if (result != CSL_SOK) 
        {
            return result;
        }
        /* Page 0 / Register 30 */
        // Set BCLK N
        result = AIC3254_Write(30, 0x98, hi2c); // 8khz
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
    result = AIC3254_Write(60, 0x01, hi2c);
    if (result != CSL_SOK) 
    {
        return result;
    }

#ifdef ENABLE_RECORD
    if (sampRateRec==SAMP_RATE_48KHZ)
    {
        //
        // Set ADC_FS to 48 kHZ
        //

        // Set NADC to 2
        result = AIC3254_Write(18,0x82, hi2c); 
        if (result != CSL_SOK) 
        {
            return result;
        }

        // Set MADC to 8
        result = AIC3254_Write(19, 0x88, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }

        // Set ADC OSR LSB value to 128
        // This generates the ADC_FS = 48KHz
        // ADC_FS = (12MHz *(R * J.D)/P)/(NADC * MADC * AOSR)
        result = AIC3254_Write(20, 128, hi2c );
        if (result != CSL_SOK) 
        {
            return result;
        }
    } 
    else if (sampRateRec==SAMP_RATE_44_1KHZ)
    {
        //
        // Set ADC_FS to 44.1 kHZ

        // Set NADC to 2
        result = AIC3254_Write(18, 0x82, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }

        // Set MADC to 8
        result = AIC3254_Write(19, 0x88, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }

        // Set ADC OSR LSB value to 128
        // This generates the ADC_FS = 44.1KHz
        // ADC_FS = (12MHz *(R * J.D)/P)/(NADC * MADC * AOSR)
        result = AIC3254_Write(20, 128, hi2c );
        if (result != CSL_SOK) 
        {
            return result;
        }
    } 
    else if (sampRateRec==SAMP_RATE_32KHZ)
    {
        //
        // Set ADC_FS to 32 kHZ

        // Set NADC to 3
        result = AIC3254_Write(18, 0x83, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }

        // Set MADC to 8
        result = AIC3254_Write(19, 0x88, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }

        // Set ADC OSR LSB value to 128
        // This generates the ADC_FS = 32KHz
        // ADC_FS = (12MHz *(R * J.D)/P)/(NADC * MADC * AOSR)
        result = AIC3254_Write(20, 128, hi2c );
        if (result != CSL_SOK) 
        {
            return result;
        }
    } 
    else if (sampRateRec==SAMP_RATE_16KHZ)
    {
        //
        // Set ADC_FS to 16 kHZ

        /* Page 0 / Register 18 */
        // Set NADC
        //result = AIC3254_Write(18, 0x86, hi2c);
        result = AIC3254_Write(18, 0x90, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }

        /* Page 0 / Register 19 */
        // Set MADC
        //result = AIC3254_Write(19, 0x88, hi2c);
        result = AIC3254_Write(19, 0x83, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }

        /* Page 0 / Register 20 */
        // Set ADC OSR LSB
        result = AIC3254_Write(20, 128, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }
    } 
    else if (sampRateRec==SAMP_RATE_8KHZ)
    {
        //
        // Set ADC_FS to 8 kHZ

        /* Page 0 / Register 18 */
        // Set NADC
        //result = AIC3254_Write(18, 0x8C, hi2c);
        result = AIC3254_Write(18, 0xa0, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }

        /* Page 0 / Register 19 */
        // Set MADC
        result = AIC3254_Write(19, 0x83, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }

        /* Page 0 / Register 20 */
        // Set ADC OSR LSB
        result = AIC3254_Write(20, 128, hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }
    } 
    else
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "RECORD FORMAT 0x%x NOT SUPPORTED\n", sampRateRec);
#endif
    } 

    // Set ADC Signal Processing Block to PRB_R1
    ///result = AIC3254_Write(61, 0x01, hi2c);
    ///if (result != CSL_SOK) 
    ///{
    ///    return result;
    ///}
#endif // ENABLE_RECORD

    result = AIC3254_Write(27,/*0xC*/0xd, hi2c); // BCLK and WCLK is set as op to AIC3254(Master)
    if (result != CSL_SOK) 
    {
        return result;
    }

    result = AIC3254_Write(0,1,hi2c);// select page 1
    if (result != CSL_SOK) 
    {
        return result;
    }

#if 0
#ifdef PF_C5535_EZDSP        
    // power up Mic Bias using LDO-IN
    result = AIC3254_Write(51,0x48,hi2c);
    if (result != CSL_SOK) 
    {
        return result;
    }
#endif
#endif

    result = AIC3254_Write(0x1,0x8,hi2c);// Disable crude AVDD generation from DVDD
    if (result != CSL_SOK) 
    {
        return result;
    }

#ifdef PF_C5535_EZDSP
    result = AIC3254_Write(0x2,1,hi2c);// Enable Analog Blocks and internal LDO
#else
    result = AIC3254_Write(0x2,0,hi2c);// Enable Analog Blocks
#endif
    if (result != CSL_SOK) 
    {
        return result;
    }

    result = AIC3254_Write(20, 32, hi2c);// Depop reg R=6K,t=8RC(2.256ms),ramp time=0ms
    if (result != CSL_SOK) 
    {
        return result;
    }

    result = AIC3254_Write(0xc, 0x8, hi2c);// LDAC AFIR routed to HPL
    if (result != CSL_SOK) 
    {
        return result;
    }

    result = AIC3254_Write(0xd, 0x8, hi2c);// RDAC AFIR routed to HPR
    if (result != CSL_SOK) 
    {
        return result;
    }

#ifdef ENABLE_RECORD
    aic3254_InputSelect(gAppInputSelect, hi2c);

    result = AIC3254_Write(0, 0, hi2c); // write 0 to page register to select page 0
    if (result != CSL_SOK)
    {
        return result;
    }

    /* Page 0 / Register 83 */
    /* Left ADC Channel Volume Control Register */
    result = AIC3254_Write(83, gADCDigGain[0], hi2c);
    if (result != CSL_SOK)
    {
        return result;
    }

    /* Page 0 / Register 84 */
    /* Right ADC Channel Volume Control Register */
    result = AIC3254_Write(84, gADCDigGain[1], hi2c);
    if (result != CSL_SOK)
    {
        return result;
    }

#endif // ENABLE_RECORD

    result = AIC3254_Write(0, 0, hi2c); // write 0 to page register to select page 0
    if (result != CSL_SOK) 
    {
        return result;
    }

    result = AIC3254_Write(64,0x2,hi2c); // left vol=right vol
    if (result != CSL_SOK) 
    {
        return result;
    }

    result = AIC3254_Write(63,0xd4, hi2c); // power up left,right data paths and set channel
    if (result != CSL_SOK) 
    {
        return result;
    }

    result = AIC3254_Write(64,0xc,hi2c); // left vol=right vol; muted
    if (result != CSL_SOK) 
    {
        return result;
    }

    result = AIC3254_Write(0,1,hi2c);// select page 1
    if (result != CSL_SOK) 
    {
        return result;
    }

    result = AIC3254_Write(0x10,0,hi2c);// unmute HPL , 0dB gain
    if (result != CSL_SOK) 
    {
        return result;
    }

    result = AIC3254_Write(0x11,0,hi2c);// unmute HPR , 0dB gain
    if (result != CSL_SOK) 
    {
        return result;
    }

    result = AIC3254_Write(0x9,0x30,hi2c);// power up HPL,HPR
    if (result != CSL_SOK) 
    {
        return result;
    }

    result = AIC3254_Write(0x0,0x0,hi2c);// select page 0
    if (result != CSL_SOK) 
    {
        return result;
    }

    result = AIC3254_Write(0x40,0x2,hi2c);// unmute DAC with right vol=left vol
    if (result != CSL_SOK) 
    {
        return result;
    }

    result = AIC3254_Write(65,/*20 48*/ 0,hi2c);// set DAC gain to 0dB
    if (result != CSL_SOK) 
    {
        return result;
    }

#if 0
#ifdef ENABLE_RECORD
    //Powerup left and right ADC
    result = AIC3254_Write(81,0xc0,hi2c);
    if (result != CSL_SOK) 
    {
        return result;
    }

    //Unmute left and right ADC
    result = AIC3254_Write(82,0x00,hi2c);
    if (result != CSL_SOK) 
    {
        return result;
    }
    #endif // ENABLE_RECORD
#endif

    result = AIC3254_Write(0x0,0x1,hi2c);// select page 1
    if (result != CSL_SOK) 
    {
        return result;
    }

    result = AIC3254_Write(0x10,0,hi2c);// unmute HPL , 0dB gain
    if (result != CSL_SOK) 
    {
        return result;
    }

    result = AIC3254_Write(0x11,0,hi2c);// unmute HPR , 0dB gain
    if (result != CSL_SOK) 
    {
        return result;
    }

#if 0 // debug
    // route ADC_FS to WCLK (I2S FS)
    result = AIC3254_Write(33, 0x10, hi2c);
    if (result != CSL_SOK) 
    {
        return result;
    }
#endif

    // write 1 to page register to select page 1 - prepare for next headset volume change
    result = AIC3254_Write(0, 1, hi2c);
    if (result != CSL_SOK) 
    {
        return FALSE;
    }

    return result;
}

#if 0
#define HEADPHONE_DRIVER_GAIN_MUTE_ENABLE  0x40    // bit6 =1 mute headphone driver
#define VOLUME_STEP_SIZE                   256
#define VOLUME_TABLE_MAX_GAIN_INDEX        29      // headphone gain setting = 29 -> 29 dB gain
#define VOLUME_TABLE_MAX_ATTNEUATION_INDEX 35      // headphone gain setting = 0x3A -> -6dB gain
#define USB_MAX_ATTENUATION_VALUE          -32768
#define VOLUME_TABLE_MUTE_HEADPHONE_INDEX  36      // headphone gain setting = 0x7B set gain to -5dB with headphone driver muted

// table has both gain and attenuation settings for headphone output of the codec.
// 0 : no gain/no attenuation, gain : 1 - 29, attenuation : 0x3F - 0x3A, muted: 0x7B
const Uint16 volume_table[] =  {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,
    0x3F,0x3E,0x3D,0x3C,0x3B,0x3A,(0x3B | HEADPHONE_DRIVER_GAIN_MUTE_ENABLE)
};

/*
* Change gain setting of headphone output of codec
* volume = gain setting received from USB
* channel = 0:left channel, =1 right channel
***********************************************************************/
Bool Adjust_Volume(
    Int16 volume, 
    Uint16 channel
)
{
    CSL_Status    result = CSL_SOK;
    Uint16        gain;

    // goto max attentuation
    if (volume == USB_MAX_ATTENUATION_VALUE)
    {
        // the max attenuation for the headpphone  is -6dB so we will mute the headphone driver
        // and set the codec gain to the lowest value(-5dB) that allows the headphone driver
        // to be muted. any volume change other than the max attenuation will turn off the
        // headphone driver mute
        gain = VOLUME_TABLE_MUTE_HEADPHONE_INDEX;
    }
    else if (volume >= 0)
    {
        // determine gain index
        gain = volume/VOLUME_STEP_SIZE;

        // check range
        if(gain > VOLUME_TABLE_MAX_GAIN_INDEX)
        {
            // set to max gain
            gain = VOLUME_TABLE_MAX_GAIN_INDEX;
        }
    }
    else
    {
        // determine attenuation index
        gain = (-volume)/VOLUME_STEP_SIZE;
        if (gain !=0)
        {
            //index from start of attentuation values in table
            gain += VOLUME_TABLE_MAX_GAIN_INDEX;
            if (gain > VOLUME_TABLE_MAX_ATTNEUATION_INDEX)
            {
                // set to max attenuation
                gain = VOLUME_TABLE_MAX_ATTNEUATION_INDEX;
            }
        }

    }

    if (channel == 0)
    {
        //adjust volume setting of left headphone
        result = AIC3254_Write(0x10,volume_table[gain],hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }
    }
    else
    {
        //adjust volume setting of right headphone
       result = AIC3254_Write(0x11,volume_table[gain],hi2c);
        if (result != CSL_SOK) 
        {
            return result;
        }
    }
    return TRUE;
}

/*
* Mute control for codec output
* TRUE = Mute codec output
* FALSE = UnMute codec output
***********************************************************************/
Bool Set_Mute_State(
    Bool flag
)
{
    CSL_Status    result = CSL_SOK;
    Bool retval;

    retval = TRUE;

    // write 0 to page register to select page 0
    result = AIC3254_Write(0, 0, hi2c);
    if (result != CSL_SOK) 
    {
        retval = FALSE;
    }
    else
    {
        if (flag == TRUE)
        {
            //mute output
            result = AIC3254_Write(64,0xd,hi2c);
            if (result != CSL_SOK) 
            {
                retval = FALSE;
            }
        }
        else
        {
            //unmute output
            result = AIC3254_Write(64,0x1,hi2c);
            if (result != CSL_SOK) 
            {
                retval = FALSE;
            }
        }
    }
#if 1
    // write 1 to page register to select page 1 - prepare for next headset volume change
    result = AIC3254_Write(0, 1, hi2c);
    if (result != CSL_SOK) 
    {
        retval = FALSE;
    }
#endif
    return retval;
}

#endif

#if 0
CSL_Status EEPROM_Write(Uint16 regAddr, Uint16 regData, pI2cHandle hi2c)
{
    CSL_Status    status;
    Uint16        writeCount;
    volatile Uint16 looper;
    Uint16        writeBuff[4];
    Uint16        readBuff[4];
    Uint16 startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));

    status = CSL_ESYS_INVPARAMS;

    if(hi2c != NULL)
    {
        writeCount  =  4;
        /* Initialize the buffer          */
        /* First two bytes are Register Address */
        /* Second two bytes are register data   */
        writeBuff[0] = (regAddr & 0x00FF);
        writeBuff[1] = ((regAddr>>8) & 0x00FF);
        writeBuff[2] = (regData & 0x00FF);
        writeBuff[3] = ((regData>>8) & 0x00FF);
        readBuff[0] = (regAddr & 0x00FF);
        readBuff[1] = ((regAddr>>8) & 0x00FF);
        readBuff[2] = 0;
        readBuff[3] = 0;

        /* Write the data */
        status = I2C_write(writeBuff, 
                        writeCount,
                        I2C_EEPROM_ADDR, 
                        TRUE, 
                        startStop,
                        CSL_I2C_MAX_TIMEOUT);
        if (status!=CSL_SOK)
        {
            return status;
        }

        /* Give some delay */
        for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}

        // Read the data back for verification
        // dummy write for the register address
        status = I2C_write(&readBuff[0], 
                        2,
                        I2C_EEPROM_ADDR, 
                        TRUE, 
                        startStop,
                        CSL_I2C_MAX_TIMEOUT);
        if (status!=CSL_SOK)
        {
            return status;
        }
        
        /* Give some delay */
        for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}

        // read the data
        status = I2C_read(&readBuff[2], 
                        2,
                        I2C_EEPROM_ADDR,
                        TRUE, 
                        startStop, 
                        CSL_I2C_MAX_TIMEOUT, 
                        FALSE);
        if (status!=CSL_SOK)
        {
            return status;
        }

        if ((readBuff[2]!=writeBuff[2])||(readBuff[3]!=writeBuff[3]))
            status |= (-1);
    }

    return status;
}
#endif

Int16 aic3254_InputSelect(
    tAICInSelect input, 
    pI2cHandle hi2c
)
{
    switch (input)
    {
        case AIC_LINE:
            /* ADC ROUTING and Power Up */
            AIC3254_Write(  0, 0x01, hi2c);                 // Select page 1
            AIC3254_Write( 10, 0x00, hi2c);                 // CM (common mode)  = 0.9V (default)
            AIC3254_Write( 51, 0x00, hi2c );                // MICBIAS off
            AIC3254_Write( 52, ADC_MICPGA_POST_INR, hi2c ); // STEREO IN Jack
                                                            // IN2_L to LADC_P through selected resistance
            AIC3254_Write( 55, ADC_MICPGA_POST_INR, hi2c ); // IN2_R to RADC_P through selected resistance
            AIC3254_Write( 54, ADC_MICPGA_NEGT_INR, hi2c ); // CM1L (common mode) to LADC_M through selected resistance
            AIC3254_Write( 57, ADC_MICPGA_NEGT_INR, hi2c ); // CM1R (common mode) to RADC_M through selected resistance
            AIC3254_Write( 59, MIC_PGA_GAIN, hi2c );        // MIC_PGA_L enabled w/ selected gain
            AIC3254_Write( 60, MIC_PGA_GAIN, hi2c );        // MIC_PGA_R enabled w/ selected gain
            AIC3254_Write(  0, 0x00, hi2c );                // Select page 0
            AIC3254_Write( 81, 0xc0, hi2c );                // Powerup Left and Right ADC
            AIC3254_Write( 82, 0x00, hi2c );                // Unmute Left and Right ADC
            break;
        case AIC_AMIC:
            /* ADC ROUTING and Power Up */
            AIC3254_Write(  0, 0x01, hi2c );                // Select page 1
            AIC3254_Write( 10, 0x00, hi2c );                // CM (common mode)  = 0.9V (default)
            //AIC3254_Write( 51, 0x40, hi2c );                // enable MICBIAS = 1.25V, from AVdd
            //AIC3254_Write( 51, 0x48, hi2c );                // enable MICBIAS = 1.25V, from LDOIN
            AIC3254_Write( 51, 0x68, hi2c );                // enable MICBIAS = 2.5V, from LDOIN
            //AIC3254_Write( 52, 0xc0, hi2c );                // MICROPHONE IN Jack
                                                            // IN1_L to LADC_P through 40 kohm
            //AIC3254_Write( 55, 0xc0, hi2c );                // IN1_R to RADC_P through 40 kohmm
            //AIC3254_Write( 52, 0x30, hi2c );                // MICROPHONE IN Jack
                                                            // IN2_L to LADC_P through 40 kohm
            //AIC3254_Write( 55, 0x30, hi2c );                // IN2_R to RADC_P through 40 kohmm
            AIC3254_Write( 52, 0x10, hi2c );                // MICROPHONE IN Jack
                                                            // IN2_L to LADC_P through 10 kohm
            AIC3254_Write( 55, 0x10, hi2c );                // IN2_R to RADC_P through 10 kohmm
            AIC3254_Write( 54, 0xc0, hi2c );                // CM1L (common mode) to LADC_M through 40 kohm
            AIC3254_Write( 57, 0xc0, hi2c );                // CM1R (common mode) to RADC_M through 40 kohm
            AIC3254_Write( 59, 0x5f, hi2c );                // MIC_PGA_L enabled 47.5db
            AIC3254_Write( 60, 0x5f, hi2c );                // MIC_PGA_R enabled 47.5db
            //AIC3254_Write( 59, 0x0f, hi2c );                // MIC_PGA_L enabled 7.5db
            //AIC3254_Write( 60, 0x0f, hi2c );                // MIC_PGA_R enabled 7.5db
            AIC3254_Write(  0, 0x00, hi2c );                // Select page 0
            AIC3254_Write( 81, 0xc0, hi2c );                // Powerup Left and Right ADC
            AIC3254_Write( 82, 0x00, hi2c );                // Unmute Left and Right ADC
            break;
        case AIC_DMIC:
            /* ADC ROUTING and Power Up */
            AIC3254_Write(  0, 0x00, hi2c );                // Select page 0
            //AIC3254_Write( 10, 0x00, hi2c );                // CM (common mode)  = 0.9V (default)
            AIC3254_Write( 52, 0x04, hi2c );                // Set GPIO for DMIC input = 0x04            
            //AIC3254_Write( 56, 0x02, hi2c );              // SCLK - Digital Mic input
            AIC3254_Write( 55, 0x0E, hi2c );                // MISO - CLK out for Digital Mic
            
            AIC3254_Write(  0, 0x01, hi2c );                // Select page 1
            AIC3254_Write( 51, 0x00, hi2c );                // MICBIAS powered Down
            AIC3254_Write( 52, 0x00, hi2c );                // no signal routed to Left PGA+
            AIC3254_Write( 55, 0x00, hi2c );                // no signal routed to Right PGA+
            AIC3254_Write( 54, 0x00, hi2c );                // no signal Routed to Left PGA-
            AIC3254_Write( 57, 0x00, hi2c );                // no signal Routed to Right PGA-
            AIC3254_Write( 59, 0x80, hi2c );                // MIC_PGA_L set to 0db
            AIC3254_Write( 60, 0x80, hi2c );                // MIC_PGA_R set to 0db
            AIC3254_Write(  0, 0x00, hi2c );                // Select page 0
            AIC3254_Write( 81, 0xCC, hi2c );                // Powerup Left and Right ADC, L & R ADC configured for Dig Mic, GPIO = Dig Mic input            
            //AIC3254_Write( 81, 0xDC, hi2c );                // Powerup Left and Right ADC, L & R ADC configured for Dig Mic, SCLK = Dig Mic input            
            AIC3254_Write( 82, 0x00, hi2c );                // unmute Left and Right ADC
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
    Uint16 baseRegNumber, 
    pI2cHandle hi2c
)
{
    //coef as |byte3 byte2 byte1 byte0| -->  |XX byte2 byte1 byte0|
    Uint32 isoByte;
    Uint16 theCoefByte;
    // write the coefs to 4 consecutive register locations starting at pageNum,baseRegNumber
    AIC3254_Write(  0, pageNum, hi2c );  //select the page;
    isoByte=theCoef&(0x00ff0000);
    isoByte=isoByte>>16;
    theCoefByte=((Int16)isoByte) & (0xff);
    AIC3254_Write(baseRegNumber++,theCoefByte, hi2c);
    
    isoByte=theCoef&(0x0000ff00);
    isoByte=isoByte>>8;
    theCoefByte=((Int16)isoByte) & (0xff);
    AIC3254_Write(baseRegNumber++,theCoefByte, hi2c);
   
    isoByte=theCoef&(0x000000ff);
  //isoByte=isoByte>>8;
    theCoefByte=((Int16)isoByte) & (0xff);
    AIC3254_Write(baseRegNumber++,theCoefByte, hi2c);                        
}

// support function
// write the coefs to the 3 consecutive registers at the base register 
// assumes page has already been set
void aic_writeCoef_2(
    Uint32 theCoef, 
    Uint16 baseRegNumber, 
    pI2cHandle hi2c
)
{
    //coef as |byte3 byte2 byte1 byte0| -->  |XX byte2 byte1 byte0|
    Uint32 isoByte;
    Uint16 theCoefByte;
    // write the 24 bit coef to 3 consecutive 8 bit register locations starting at baseRegNumber
    isoByte=theCoef&(0x00ff0000);
    isoByte=isoByte>>16;
    theCoefByte=((Int16)isoByte) & (0xff);
    AIC3254_Write(baseRegNumber++,theCoefByte, hi2c);
    
    isoByte=theCoef&(0x0000ff00);
    isoByte=isoByte>>8;
    theCoefByte=((Int16)isoByte) & (0xff);
    AIC3254_Write(baseRegNumber++,theCoefByte, hi2c);
   
    isoByte=theCoef&(0x000000ff);
    theCoefByte=((Int16)isoByte) & (0xff);
    AIC3254_Write(baseRegNumber++,theCoefByte, hi2c);
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
    Uint16 baseRegNumber, 
    pI2cHandle hi2c
)
{   
    Uint16 k;
    AIC3254_Write(  0, pageNum, hi2c );  //select the page that this set is on.    
    for (k=0;k<ORD1_COEF_SZ;k++)
    {
        aic_writeCoef_2(coefs[k], baseRegNumber, hi2c);
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
    Uint16 baseRegNumber, 
    pI2cHandle hi2c
)
{
    Uint16 kk;
    AIC3254_Write(  0, pageNum, hi2c );  //select the page that this set is on.    
    for (kk=0;kk<BQ_COEF_SZ;kk++)
    {
        aic_writeCoef_2(biCoefs[kk], baseRegNumber, hi2c);
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
    tAICADCBiQuad BQIdentifier, 
    pI2cHandle hi2c
)
{
    // for biquad sections A thru E, all the registers are contiguous:
    switch (BQIdentifier)
    {
        case ADC_BIQUAD_A:
        case ADC_BIQUAD_B:
        case ADC_BIQUAD_C:
        case ADC_BIQUAD_D:
            aic_SetBiQuadSection(biCoefs, aicADCBiQuadPageNumsLeft[BQIdentifier], aicADCBiQuadStartRegLeft[BQIdentifier], hi2c);
            aic_SetBiQuadSection(biCoefs, aicADCBiQuadPageNumsRight[BQIdentifier], aicADCBiQuadStartRegRight[BQIdentifier], hi2c);
            break;
        case ADC_BIQUAD_E:
            // this section spans two pages, special case
            // Write to LEFT ADC CHANNEL, BIQUAD E
            aic_writeCoef(biCoefs[0], 8, 116, hi2c);    // write the first (N0) coef, starting at page 8, register 116,
            aic_writeCoef(biCoefs[1], 8, 120, hi2c);    // N1
            aic_writeCoef(biCoefs[2], 8, 124, hi2c);    // N2
            aic_writeCoef(biCoefs[3], 9, 8, hi2c);      // D1
            aic_writeCoef(biCoefs[4], 9, 12, hi2c);     // D2
            // Write to RIGHT ADC CHANNEL, BIQUAD E
            aic_writeCoef(biCoefs[0], 9, 124, hi2c);    // write the first (N0) coef, starting at page 9, register 124,
            aic_writeCoef(biCoefs[1], 10, 8, hi2c);     // N1
            aic_writeCoef(biCoefs[2], 10, 12, hi2c);    // N2
            aic_writeCoef(biCoefs[3], 10, 16, hi2c);    // D1
            aic_writeCoef(biCoefs[4], 10, 20, hi2c);    // D2
            break;
        default:
            break;        
    }
}

