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


/*********************************************************************
*    sysctrl.c
**********************************************************************
*   C5505 System-Control routines.
**********************************************************************/
#include "registers.h"
#include "sysctrl.h"

/*******************************************************************
* PeripheralPinMapping
* - Serial0 = Serial-Port-0 pin mapping (PERIPH_BUS_S0_*)
* - Serial1 = Serial-Port-1 pin mapping (PERIPH_BUS_S1_*)
* - Parallel = Parallel-Port pin mapping (PERIPH_BUS_PP_*)
* This function sets the pin-mapping for each peripheral bus.
********************************************************************/
void PeripheralPinMapping(unsigned short Serial0, unsigned short Serial1, unsigned short Parallel)
{
   /* Setup Peripheral (External) Bus Selection */
   IOPORT_REG_PERIPHSEL0 = Serial0 | Serial1 | Parallel;
}

/*******************************************************************
* PeripheralReset
* - PeripheralResets = Peripherals to reset (PERIPH_RESET_*)
* - ResetHoldCycles = # sys-clk cycles to hold reset signal low
* - DelayCycles = # of delay cycles to wait for peripheral-ready
* This function resets the specified peripheral(s).
********************************************************************/
void PeripheralReset(unsigned short PeripheralResets, unsigned short ResetHoldCycles, unsigned long DelayCycles)
{
   int i;

   /* Reset Counter value */
   IOPORT_REG_PER_RSTCOUNT = ResetHoldCycles;

   /* Reset the specifed peripherals */
   IOPORT_REG_PER_RESET = PeripheralResets;

   for (i=0; i<DelayCycles; i++);
}

/*******************************************************************
* PeripheralClkEnableOnly
* - PeripheralClks = PeripheralClks to enable (PERIPH_CLK_*)
* This function enables the specified peripheral-clks ONLY.
* (all other peripheral-clks are disabled).
********************************************************************/
void PeripheralClkEnableOnly(unsigned long PeripheralClks)
{
   /* Enable the specifed peripheral-clocks (disable all others) */
   IOPORT_REG_IDLE_PCGCR_LSW = ~((unsigned short)(PeripheralClks & 0xFFFF));
   IOPORT_REG_IDLE_PCGCR_MSW = ~((unsigned short)((PeripheralClks >> 16) & 0x007F));
}

/*******************************************************************
* PeripheralClkEnable
* - PeripheralClks = PeripheralClks to enable (PERIPH_CLK_*)
* This function enables the specified peripheral-clks.
* (all other peripheral-clks are unchanged).
********************************************************************/
void PeripheralClkEnable(unsigned long PeripheralClks)
{
   /* Enable the specifed peripheral-clocks (clear the disable-bits) */
   IOPORT_REG_IDLE_PCGCR_LSW &= ~((unsigned short)(PeripheralClks & 0xFFFF));
   IOPORT_REG_IDLE_PCGCR_MSW &= ~((unsigned short)((PeripheralClks >> 16) & 0x007F));
}

