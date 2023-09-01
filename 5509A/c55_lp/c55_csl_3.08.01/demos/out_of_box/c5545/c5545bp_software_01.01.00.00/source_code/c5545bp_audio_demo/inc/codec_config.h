/*
* codec_config.h
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

#ifndef _CODEC_CONFIG_H_
#define _CODEC_CONFIG_H_

#include "tistdtypes.h"
#include "csl_i2c.h"
#include "csl_gpio.h"

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
#define ADC_DIG_GAIN                ( 0x76 )                       /* ADC Channel Volume, -5dB  */
//#define ADC_DIG_GAIN                ( 2*6 )                       /* ADC Channel Volume, (2*6)*0.5 = 6 dB  */
//#define ADC_DIG_GAIN                ( 2*20 )                       /* ADC Channel Volume, (2*20)*0.5 = 20 dB  */

/* IN2L is routed to Left MICPGA with selected resistance */
#define ADC_MICPGA_POST_INR         ( ADC_IN_FF_RESIST << 4 )
/* CM is routed to Left MICPGA via CM1L with selected resistance */
#define ADC_MICPGA_NEGT_INR         ( ADC_IN_FF_RESIST << 6 )

#define I2C_CODEC_ADDR          (0x18)
#define I2C_EEPROM_ADDR         (0x50)

#define BQ_COEF_SZ (5)
#define ORD1_COEF_SZ (3)

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

/* Function to configure audio codec for loopback */
CSL_Status codecConfigLoopback(void);

/* Initializes codec */
CSL_Status codecConfigPlayback(Uint16 sampRate,
                               Uint8  input,
                               Bool   useADCHiPass);

Int16 codecInputSelect(tAICInSelect input);

#endif /* _CODEC_CONFIG_H_ */
