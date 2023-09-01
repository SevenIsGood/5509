/*
 * $$$MODULE_NAME pll_control.h
 *
 * $$$MODULE_DESC pll_control.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef _PLL_CONTROL_H_
#define _PLL_CONTROL_H_

#include <std.h>
#include "csl_types.h"

#define CSL_PLL_CLOCKIN_RTC    (32768u)     // RTC OSC
#define CSL_PLL_CLOCKIN_EXT    (12000000u)  // 12 MHz external OSC

// Set the PLL to desired MHZ
// mhz:
//	40 - 40Mhz
//	50 - 50Mhz
//	60 - 60Mhz
//	75 - 75Mhz
//	100 - 100Mhz
//	120 - 120Mhz
//	other - 60Mhz
CSL_Status pll_sample(Uint16 mhz);

Uint32 appGetSysClk(void);

#endif /* _PLL_CONTROL_H_ */
