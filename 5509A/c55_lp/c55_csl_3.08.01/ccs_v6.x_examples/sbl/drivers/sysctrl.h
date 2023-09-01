/********************************************************************* 
*    Property of Texas Instruments Incorporated, Copyright 2007
*    All rights reserved
**********************************************************************
*    sysctrl.h
**********************************************************************/
#ifndef SYSCTRL_H
#define SYSCTRL_H

/********************************************************************* 
* Function prototypes...
**********************************************************************/

/*******************************************************************
* PeripheralPinMapping
* - Serial0 = Serial-Port-0 pin mapping (PERIPH_BUS_S0_*)
* - Serial1 = Serial-Port-1 pin mapping (PERIPH_BUS_S1_*)
* - Parallel = Parallel-Port pin mapping (PERIPH_BUS_PP_*)
* This function sets the pin-mapping for each peripheral bus.
********************************************************************/
void PeripheralPinMapping(unsigned short Serial0, unsigned short Serial1, unsigned short Parallel);

/*******************************************************************
* PeripheralReset
* - PeripheralResets = Peripherals to reset (PERIPH_RESET_*)
* - ResetHoldCycles = # sys-clk cycles to hold reset signal low
* - DelayCycles = # of delay cycles to wait for peripheral-ready
* This function resets the specified peripheral(s).
********************************************************************/
void PeripheralReset(unsigned short PeripheralResets, unsigned short ResetHoldCycles, unsigned long DelayCycles);

/*******************************************************************
* PeripheralClkEnableOnly
* - PeripheralClks = PeripheralClks to enable (PERIPH_CLK_*)
* This function enables the specified peripheral-clks ONLY.
* (all other peripheral-clks are disabled).
********************************************************************/
void PeripheralClkEnableOnly(unsigned long PeripheralClks);

/*******************************************************************
* PeripheralClkEnable
* - PeripheralClks = PeripheralClks to enable (PERIPH_CLK_*)
* This function enables the specified peripheral-clks.
* (all other peripheral-clks are unchanged).
********************************************************************/
void PeripheralClkEnable(unsigned long PeripheralClks);

#endif // SYSCTRL_H

