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


#ifndef _CODEC_AIC3204_H_
#define _CODEC_AIC3204_H_

#include "tistdtypes.h"

#include "cslr_gpio.h"

#include "csl_i2c.h"
#include "csl_gpio.h"

CSL_Status AIC3254_init(Uint16 sampRatePb, Uint16 sampRateRec, Uint32 i2cInClk);
Bool Set_Mute_State(Bool flag);
Bool Adjust_Volume(Int16 volume, Uint16 channel);
CSL_Status AIC3254_Write(Uint16 regAddr, Uint16 regData, pI2cHandle hi2c);
CSL_Status AIC3254_Write_Two(Uint16 regAddr1, Uint16 regData1, Uint16 regAddr2, Uint16 regData2, pI2cHandle hi2c);
CSL_Status AIC3254_Read(Uint16 regAddr, Uint16 *Data, pI2cHandle  hi2c);
CSL_Status EEPROM_Write(Uint16 regAddr, Uint16 regData, pI2cHandle hi2c);

#endif /* _CODEC_AIC3204_H_ */
