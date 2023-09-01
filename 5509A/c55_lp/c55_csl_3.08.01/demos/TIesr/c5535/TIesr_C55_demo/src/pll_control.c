/*
* pll_control.c
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

#include "csl_pll.h"
#include "csl_general.h"
#include "csl_pllAux.h"
#include "pll_control.h"

PLL_Obj pllObj;
PLL_Config pllCfg1;

PLL_Handle hPll;

const PLL_Config pllCfg_40MHz       = {0x8988, 0x8000, 0x0806, 0x0201};
const PLL_Config pllCfg_50MHz       = {0x8BE8, 0x8000, 0x0806, 0x0201};
const PLL_Config pllCfg_60MHz       = {0x8724, 0x8000, 0x0806, 0x0000};
const PLL_Config pllCfg_75MHz       = {0x88ED, 0x8000, 0x0806, 0x0000};
const PLL_Config pllCfg_100MHz      = {0x8BE8, 0x8000, 0x0806, 0x0000};
const PLL_Config pllCfg_120MHz      = {0x8E4A, 0x8000, 0x0806, 0x0000};

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
CSL_Status pll_sample(Uint16 mhz)
{
    CSL_Status status;
    volatile int i;

    status = PLL_init(&pllObj, CSL_PLL_INST_0);
    if (status != CSL_SOK)
    {
        return status;
    }

    hPll = (PLL_Handle)(&pllObj);

    PLL_reset(hPll);

    status = PLL_bypass(hPll);
    if (status != CSL_SOK)
    {
        return status;
    }

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
        break;
    }
    
    // set the PLL using CSL function
    status = PLL_config(hPll, pConfigInfo);
    if (status != CSL_SOK)
    {
        return(status);
    }

    // read the PLL configure back
    status = PLL_getConfig(hPll, &pllCfg1);
    if (status != CSL_SOK)
    {
        return status;
    }

    /* Wait for PLL to stabilize */
    for (i=0; i<100; i++);

    // enable the PLL
    status = PLL_enable(hPll);
    if (status != CSL_SOK)
    {
        return status;
    }

    return CSL_SOK;
}

//*****************************************************************************
// appGetSysClk
//*****************************************************************************
//  Description:
//      Computes system clock by reading PLL registers.
//      Input clock is currently a symbolic constant.
//
//  Parameters:
//      none
//
// Return:
//      PLL output frequency in Hz
//
//*****************************************************************************
Uint32 appGetSysClk(void)
{
    Bool      pllRDBypass;
    Bool      pllOutDiv;
    Uint32    sysClk;
    Uint16    pllM;
    Uint16    pllRD;
    Uint16    pllVO;

    pllM = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR1, SYS_CGCR1_M);

    pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDRATIO);
    //pllVO = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR3, SYS_CGCR4_ODRATIO);
    pllVO = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_ODRATIO);

    pllRDBypass = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDBYPASS);
    pllOutDiv   = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_OUTDIVEN);

	if(CSL_FEXT(CSL_SYSCTRL_REGS->CCR2, SYS_CCR2_CLKSELSTAT) == 1)
	{
		sysClk = CSL_PLL_CLOCKIN_EXT;
	}
	else
	{
		sysClk = CSL_PLL_CLOCKIN_RTC;
	}

    if (0 == pllRDBypass)
    {
        sysClk = sysClk/(pllRD + 4);
    }

    sysClk = (sysClk * (pllM + 4));

    if (1 == pllOutDiv)
    {
        sysClk = sysClk/(pllVO + 1);
    }

    /* Return the value of system clock in KHz */
    return(sysClk);
}
