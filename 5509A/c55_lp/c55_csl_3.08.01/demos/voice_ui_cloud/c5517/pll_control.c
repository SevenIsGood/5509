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


#include "csl_pll.h"
#include "csl_general.h"
#include "csl_pllAux.h"

PLL_Obj pllObj;
PLL_Config pllCfg1;

PLL_Handle hPll;

#if (defined(CHIP_C5517))
const PLL_Config pllCfg_25MHz 		= {0x28AB, 0x0003, 0x0000, 0x0024};
const PLL_Config pllCfg_50MHz 		= {0x3100, 0x0003, 0x0000, 0x0022};
//const PLL_Config pllCfg_100MHz 		= {0x41AB, 0x0003, 0x0000, 0x0000};
const PLL_Config pllCfg_100MHz 		= {0x41AB, 0x0003, 0x0010, 0x0000};
const PLL_Config pllCfg_150MHz 		= {0x3100, 0x0003, 0x0000, 0x0020};
const PLL_Config pllCfg_175MHz 		= {0x3956, 0x0003, 0x0000, 0x0020};
const PLL_Config pllCfg_200MHz 		= {0x41AB, 0x0003, 0x0010, 0x0020};
#else
const PLL_Config pllCfg_40MHz       = {0x8988, 0x8000, 0x0806, 0x0201};
const PLL_Config pllCfg_50MHz       = {0x8BE8, 0x8000, 0x0806, 0x0201};
const PLL_Config pllCfg_60MHz       = {0x8724, 0x8000, 0x0806, 0x0000};
const PLL_Config pllCfg_75MHz       = {0x88ED, 0x8000, 0x0806, 0x0000};
const PLL_Config pllCfg_100MHz      = {0x8BE8, 0x8000, 0x0806, 0x0000};
const PLL_Config pllCfg_120MHz      = {0x8E4A, 0x8000, 0x0806, 0x0000};
#endif

PLL_Config *pConfigInfo;

// Set the PLL to desired MHZ
// mhz:
//    40 - 40Mhz
//    50 - 50Mhz
//    60 - 60Mhz
//    75 - 75Mhz
//    100 - 100Mhz
//    120 - 120Mhz
//    other - 60Mhz
CSL_Status pll_sample(int mhz)
{
    CSL_Status status;
    volatile long i;

    status = PLL_init(&pllObj, CSL_PLL_INST_0);
    if (status != CSL_SOK)
    {
        return status;
    }

    hPll = (PLL_Handle)(&pllObj);

    PLL_reset(hPll);
/*
    status = PLL_bypass(hPll);
    if (status != CSL_SOK)
    {
        return status;
    }
*/
#if (defined(CHIP_C5517))
    /* Configure the PLL */
    switch (mhz)
    {
    case 25:
    	pConfigInfo = (PLL_Config *)&pllCfg_25MHz;
        break;
        
    case 50:
        pConfigInfo = (PLL_Config *)&pllCfg_50MHz;
        break;

    case 100:
        pConfigInfo = (PLL_Config *)&pllCfg_100MHz;
        break;

    case 150:
        pConfigInfo = (PLL_Config *)&pllCfg_150MHz;
        break;
    
    case 175:
        pConfigInfo = (PLL_Config *)&pllCfg_175MHz;
        break;
    
    case 200:
        pConfigInfo = (PLL_Config *)&pllCfg_200MHz;
        break;
    
    default:
        pConfigInfo = (PLL_Config *)&pllCfg_100MHz;
    }

#else
    /* Configure the PLL */
    switch (mhz)
    {
    case 40:
        pConfigInfo = (PLL_Config *)&pllCfg_40MHz;
        break;
        
    case 50:
        pConfigInfo = (PLL_Config *)&pllCfg_50MHz;
        break;

    case 60:
        pConfigInfo = (PLL_Config *)&pllCfg_60MHz;
        break;

    case 75:
        pConfigInfo = (PLL_Config *)&pllCfg_75MHz;
        break;

    case 100:
        pConfigInfo = (PLL_Config *)&pllCfg_100MHz;
        break;

    case 120:
        pConfigInfo = (PLL_Config *)&pllCfg_120MHz;
        break;
    
    default:
        pConfigInfo = (PLL_Config *)&pllCfg_60MHz;
    }
#endif    

    /* Set the PLL using CSL function */
    status = PLL_config (hPll, pConfigInfo);
    if (status != CSL_SOK)
    {
        return(status);
    }

    /* Read the PLL configure back */
    status = PLL_getConfig(hPll, &pllCfg1);
    if (status != CSL_SOK)
    {
        return status;
    }

    /* Wait 4ms for PLL to stabilize */
    for (i=0; i<(mhz*400); i++);

    /* Enable the PLL */
    status = PLL_enable(hPll);
    if (status != CSL_SOK)
    {
        return status;
    }

    return CSL_SOK;
}
