/*
 * $$$MODULE_NAME codec_aic3254.h
 *
 * $$$MODULE_DESC codec_aic3254.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef _CODEC_AIC3204_H_
#define _CODEC_AIC3204_H_

#include "tistdtypes.h"
#include "csl_i2c.h"
#include "csl_gpio.h"

#define AIC_Write   ( AIC3254_Write )
#define AIC_Read    ( AIC3254_Read )
#define AIC_Init    ( AIC3254_Init )

// pagenumber and register defintions for the adc 1st order IIR coeficients.
// From the "user programable filters" section of the AIC data sheet.
#define ADC_IIR1_PAGE_NUM_LEFT      ( 8 )
#define ADC_IIR1_START_REG_LEFT     ( 24 )  /* left adc coefs start at page 8, register 24 */
#define ADC_IIR1_PAGE_NUM_RIGHT     ( 9 )
#define ADC_IIR1_START_REG_RIGHT    ( 32 )  /* left adc coefs start at page 9, register 32 */

#define ADC_IN_FF_RESIST_10K        ( 0x1 ) /* ADC feed forward resistance 10 kOhm => 0 dB to mic PGA */
#define ADC_IN_FF_RESIST_20K        ( 0x2 ) /* ADC feed forward resistance 20 kOhm => -6 dB to mic PGA */
#define ADC_IN_FF_RESIST_40K        ( 0x3 ) /* ADC feed forward resistance 40 kOhm => -12 dB to mic PGA */

//#define ADC_IN_FF_RESIST            ( ADC_IN_FF_RESIST_10K )    /* +0 dB to micPGA */
#define ADC_IN_FF_RESIST            ( ADC_IN_FF_RESIST_40K )    /* -12 dB to micPGA */
#define MIC_PGA_GAIN                ( 0x0F )                    /* MIC PGA 7.5 dB gain */
#define ADC_DIG_GAIN                ( 0 )                       /* ADC Channel Volume, 0*0.5 = 0 dB  */
//#define ADC_DIG_GAIN                ( 2*6 )                       /* ADC Channel Volume, (2*6)*0.5 = 6 dB  */
//#define ADC_DIG_GAIN                ( 2*20 )                       /* ADC Channel Volume, (2*20)*0.5 = 20 dB  */

/* IN2L is routed to Left MICPGA with selected resistance */
#define ADC_MICPGA_POST_INR         ( ADC_IN_FF_RESIST << 4 )
/* CM is routed to Left MICPGA via CM1L with selected resistance */
#define ADC_MICPGA_NEGT_INR         ( ADC_IN_FF_RESIST << 6 )

// enums for selecting ADC input source.
typedef enum aicInSelect{
    AIC_LINE,
    AIC_AMIC,
    AIC_DMIC
} tAICInSelect;

// enums which identify which BIQuad section to put coefficients into.    
typedef enum aicADCBiQuad{
    ADC_BIQUAD_A=0,
    ADC_BIQUAD_B,
    ADC_BIQUAD_C,
    ADC_BIQUAD_D,
    ADC_BIQUAD_E,
    ADC_MAX_BIQUADS
} tAICADCBiQuad;

/* Writes byte of data to codec register */
CSL_Status AIC3254_Write(
    Uint16 regAddr, 
    Uint16 regData, 
    pI2cHandle hi2c
);

/* Writes two bytes of data to codec registers */
CSL_Status AIC3254_Write_Two(
    Uint16 regAddr1, 
    Uint16 regData1, 
    Uint16 regAddr2, 
    Uint16 regData2, 
    pI2cHandle hi2c
);

/* Reads byte of data from codec register */
CSL_Status AIC3254_Read(
    Uint16 regAddr, 
    Uint16 *Data, 
    pI2cHandle hi2c
);

/* Initializes codec */
CSL_Status AIC3254_init(
    Uint16 sampRatePb, 
    Uint16 sampRateRec, 
    pI2cHandle hi2c
);

Int16 aic3254_InputSelect(
    tAICInSelect input, 
    pI2cHandle hi2c
);

#if 0
/* Adjusts playback gains for codec */
Bool Adjust_Volume(
    Int16 volume, 
    Uint16 channel
);

/* Controls playback mute for codec */
Bool Set_Mute_State(
    Bool flag
);
#endif

#if 0
CSL_Status EEPROM_Write(
    Uint16 regAddr, 
    Uint16 regData, 
    pI2cHandle hi2c
);
#endif

#endif /* _CODEC_AIC3204_H_ */
