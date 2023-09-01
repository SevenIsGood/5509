/********************************************************************* 
*    Property of Texas Instruments Incorporated, Copyright 2007
*    All rights reserved
**********************************************************************
*    sysctrl.c
**********************************************************************
*   C5505 System-Control routines.
**********************************************************************/
#include "soc.h"
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
   CSL_SYSCTRL_REGS->EBSR = Serial0 | Serial1 | Parallel;
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
   CSL_SYSCTRL_REGS->PSRCR = ResetHoldCycles;
   /* Reset the specifed peripherals */
   CSL_SYSCTRL_REGS->PRCR = PeripheralResets;

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
   CSL_SYSCTRL_REGS->PCGCR1 = ~((unsigned short)(PeripheralClks & 0xFFFF));
   CSL_SYSCTRL_REGS->PCGCR2 = ~((unsigned short)((PeripheralClks >> 16) & 0x007F));
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
   CSL_SYSCTRL_REGS->PCGCR1 &= ~((unsigned short)(PeripheralClks & 0xFFFF));
   CSL_SYSCTRL_REGS->PCGCR2 &= ~((unsigned short)((PeripheralClks >> 16) & 0x007F));
}

