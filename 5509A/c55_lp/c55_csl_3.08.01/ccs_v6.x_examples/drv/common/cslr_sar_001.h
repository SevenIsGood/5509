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


#ifndef _CSLR__SAR_1_H_
#define _CSLR__SAR_1_H_

#include <cslr.h>
/* Minimum unit = 2 bytes */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint16 RSVD0[18];
    volatile Uint16 SARCTRL;
    volatile Uint16 RSVD1;
    volatile Uint16 SARDATA;
    volatile Uint16 RSVD2;
    volatile Uint16 SARCLKCTRL;
    volatile Uint16 RSVD3;
    volatile Uint16 SARPINCTRL;
    volatile Uint16 RSVD4;
    volatile Uint16 SARGPOCTRL;
} CSL_SarRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* SARCTRL */

#define CSL_SAR_SARCTRL_ADCSTART_MASK (0x8000u)
#define CSL_SAR_SARCTRL_ADCSTART_SHIFT (0x000Fu)
#define CSL_SAR_SARCTRL_ADCSTART_RESETVAL (0x0000u)

#define CSL_SAR_SARCTRL_CHANSEL_MASK (0x7000u)
#define CSL_SAR_SARCTRL_CHANSEL_SHIFT (0x000Cu)
#define CSL_SAR_SARCTRL_CHANSEL_RESETVAL (0x0000u)

#define CSL_SAR_SARCTRL_MULTICHANNEL_MASK (0x0800u)
#define CSL_SAR_SARCTRL_MULTICHANNEL_SHIFT (0x000Bu)
#define CSL_SAR_SARCTRL_MULTICHANNEL_RESETVAL (0x0000u)

#define CSL_SAR_SARCTRL_SINGLECYCLE_MASK (0x0400u)
#define CSL_SAR_SARCTRL_SINGLECYCLE_SHIFT (0x000Au)
#define CSL_SAR_SARCTRL_SINGLECYCLE_RESETVAL (0x0000u)

#define CSL_SAR_SARCTRL_TESTDATA_MASK (0x03FFu)
#define CSL_SAR_SARCTRL_TESTDATA_SHIFT (0x0000u)
#define CSL_SAR_SARCTRL_TESTDATA_RESETVAL (0x0000u)

#define CSL_SAR_SARCTRL_RESETVAL (0x0000u)

/* SARDATA */

#define CSL_SAR_SARDATA_ADCBSY_MASK (0x8000u)
#define CSL_SAR_SARDATA_ADCBSY_SHIFT (0x000Fu)
#define CSL_SAR_SARDATA_ADCBSY_RESETVAL (0x0000u)

#define CSL_SAR_SARDATA_CHANSEL_MASK (0x7000u)
#define CSL_SAR_SARDATA_CHANSEL_SHIFT (0x000Cu)
#define CSL_SAR_SARDATA_CHANSEL_RESETVAL (0x0000u)


#define CSL_SAR_SARDATA_ADCDATA_MASK (0x03FFu)
#define CSL_SAR_SARDATA_ADCDATA_SHIFT (0x0000u)
#define CSL_SAR_SARDATA_ADCDATA_RESETVAL (0x0000u)

#define CSL_SAR_SARDATA_RESETVAL (0x0000u)

/* SARCLKCTRL */


#define CSL_SAR_SARCLKCTRL_CLKDIV_MASK (0x7FFFu)
#define CSL_SAR_SARCLKCTRL_CLKDIV_SHIFT (0x0000u)
#define CSL_SAR_SARCLKCTRL_CLKDIV_RESETVAL (0x0000u)

#define CSL_SAR_SARCLKCTRL_RESETVAL (0x0000u)

/* SARPINCTRL */

#define CSL_SAR_SARPINCTRL_TESTCOMPOUT_MASK (0x8000u)
#define CSL_SAR_SARPINCTRL_TESTCOMPOUT_SHIFT (0x000Fu)
#define CSL_SAR_SARPINCTRL_TESTCOMPOUT_RESETVAL (0x0000u)

#define CSL_SAR_SARPINCTRL_STATUSMASK_MASK (0x4000u)
#define CSL_SAR_SARPINCTRL_STATUSMASK_SHIFT (0x000Eu)
#define CSL_SAR_SARPINCTRL_STATUSMASK_RESETVAL (0x0000u)

#define CSL_SAR_SARPINCTRL_PWRUPBIAS_MASK (0x2000u)
#define CSL_SAR_SARPINCTRL_PWRUPBIAS_SHIFT (0x000Du)
#define CSL_SAR_SARPINCTRL_PWRUPBIAS_RESETVAL (0x0000u)

#define CSL_SAR_SARPINCTRL_SARPWRUP_MASK (0x1000u)
#define CSL_SAR_SARPINCTRL_SARPWRUP_SHIFT (0x000Cu)
#define CSL_SAR_SARPINCTRL_SARPWRUP_RESETVAL (0x0000u)


#define CSL_SAR_SARPINCTRL_REFBUFFEN_MASK (0x0400u)
#define CSL_SAR_SARPINCTRL_REFBUFFEN_SHIFT (0x000Au)
#define CSL_SAR_SARPINCTRL_REFBUFFEN_RESETVAL (0x0000u)

#define CSL_SAR_SARPINCTRL_REFLVSEL_MASK (0x0200u)
#define CSL_SAR_SARPINCTRL_REFLVSEL_SHIFT (0x0009u)
#define CSL_SAR_SARPINCTRL_REFLVSEL_RESETVAL (0x0000u)

#define CSL_SAR_SARPINCTRL_REFAVDDSEL_MASK (0x0100u)
#define CSL_SAR_SARPINCTRL_REFAVDDSEL_SHIFT (0x0008u)
#define CSL_SAR_SARPINCTRL_REFAVDDSEL_RESETVAL (0x0000u)


#define CSL_SAR_SARPINCTRL_SERVOTESTMODE_MASK (0x0020u)
#define CSL_SAR_SARPINCTRL_SERVOTESTMODE_SHIFT (0x0005u)
#define CSL_SAR_SARPINCTRL_SERVOTESTMODE_RESETVAL (0x0000u)

#define CSL_SAR_SARPINCTRL_TOUCHSCREENMODE_MASK (0x0010u)
#define CSL_SAR_SARPINCTRL_TOUCHSCREENMODE_SHIFT (0x0004u)
#define CSL_SAR_SARPINCTRL_TOUCHSCREENMODE_RESETVAL (0x0000u)

#define CSL_SAR_SARPINCTRL_AVDDMEAS_MASK (0x0008u)
#define CSL_SAR_SARPINCTRL_AVDDMEAS_SHIFT (0x0003u)
#define CSL_SAR_SARPINCTRL_AVDDMEAS_RESETVAL (0x0000u)

#define CSL_SAR_SARPINCTRL_NOHV_MASK (0x0004u)
#define CSL_SAR_SARPINCTRL_NOHV_SHIFT (0x0002u)
#define CSL_SAR_SARPINCTRL_NOHV_RESETVAL (0x0000u)

#define CSL_SAR_SARPINCTRL_GNDON_MASK (0x0002u)
#define CSL_SAR_SARPINCTRL_GNDON_SHIFT (0x0001u)
#define CSL_SAR_SARPINCTRL_GNDON_RESETVAL (0x0000u)

#define CSL_SAR_SARPINCTRL_HALF_MASK (0x0001u)
#define CSL_SAR_SARPINCTRL_HALF_SHIFT (0x0000u)
#define CSL_SAR_SARPINCTRL_HALF_RESETVAL (0x0000u)

#define CSL_SAR_SARPINCTRL_RESETVAL (0x0000u)

/* SARGPOCTRL */


#define CSL_SAR_SARGPOCTRL_PENIRQSTAT_MASK (0x0200u)
#define CSL_SAR_SARGPOCTRL_PENIRQSTAT_SHIFT (0x0009u)
#define CSL_SAR_SARGPOCTRL_PENIRQSTAT_RESETVAL (0x0000u)

#define CSL_SAR_SARGPOCTRL_PENIRQEN_MASK (0x0100u)
#define CSL_SAR_SARGPOCTRL_PENIRQEN_SHIFT (0x0008u)
#define CSL_SAR_SARGPOCTRL_PENIRQEN_RESETVAL (0x0000u)

#define CSL_SAR_SARGPOCTRL_GPO3EN_MASK (0x0080u)
#define CSL_SAR_SARGPOCTRL_GPO3EN_SHIFT (0x0007u)
#define CSL_SAR_SARGPOCTRL_GPO3EN_RESETVAL (0x0000u)

#define CSL_SAR_SARGPOCTRL_GPO2EN_MASK (0x0040u)
#define CSL_SAR_SARGPOCTRL_GPO2EN_SHIFT (0x0006u)
#define CSL_SAR_SARGPOCTRL_GPO2EN_RESETVAL (0x0000u)

#define CSL_SAR_SARGPOCTRL_GPO1EN_MASK (0x0020u)
#define CSL_SAR_SARGPOCTRL_GPO1EN_SHIFT (0x0005u)
#define CSL_SAR_SARGPOCTRL_GPO1EN_RESETVAL (0x0000u)

#define CSL_SAR_SARGPOCTRL_GPO0EN_MASK (0x0010u)
#define CSL_SAR_SARGPOCTRL_GPO0EN_SHIFT (0x0004u)
#define CSL_SAR_SARGPOCTRL_GPO0EN_RESETVAL (0x0000u)

#define CSL_SAR_SARGPOCTRL_GPO3_MASK (0x0008u)
#define CSL_SAR_SARGPOCTRL_GPO3_SHIFT (0x0003u)
#define CSL_SAR_SARGPOCTRL_GPO3_RESETVAL (0x0000u)

#define CSL_SAR_SARGPOCTRL_GPO2_MASK (0x0004u)
#define CSL_SAR_SARGPOCTRL_GPO2_SHIFT (0x0002u)
#define CSL_SAR_SARGPOCTRL_GPO2_RESETVAL (0x0000u)

#define CSL_SAR_SARGPOCTRL_GPO1_MASK (0x0002u)
#define CSL_SAR_SARGPOCTRL_GPO1_SHIFT (0x0001u)
#define CSL_SAR_SARGPOCTRL_GPO1_RESETVAL (0x0000u)

#define CSL_SAR_SARGPOCTRL_GPO0_MASK (0x0001u)
#define CSL_SAR_SARGPOCTRL_GPO0_SHIFT (0x0000u)
#define CSL_SAR_SARGPOCTRL_GPO0_RESETVAL (0x0000u)

#define CSL_SAR_SARGPOCTRL_RESETVAL (0x0000u)

#endif