/*
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 *
 * \file   platform_internals.h
 *
 * \brief  This file contains all the includes of the csl library and platform.h
 *
 *****************************************************************************/

#ifndef _PLATFORM_INTERNAL_H_
#define _PLATFORM_INTERNAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csl_general.h"
#include "csl_audioClass.h"
#include "csl_audioClassAux.h"
#include "csl_cdc.h"
//#include "csl_cdcAux.h"
#include "csl_dat.h"
#include "csl_dma.h"
//#include "csl_emif.h"
#include "csl_error.h"
#include "csl_gpio.h"
#include "csl_gpt.h"
#include "csl_hid.h"
#include "csl_hidAux.h"
#include "csl_i2c.h"
#include "csl_i2s.h"
#include "csl_intc.h"
#include "csl_lcdc.h"
#include "csl_lcdcAux.h"
#include "csl_mcbsp.h"
#include "csl_mcbsp_tgt.h"
#include "csl_mcspi.h"
#include "csl_mem.h"
#include "csl_mmcsd.h"
#include "csl_mmcsd_ataIf.h"
#include "csl_msc.h"
#include "csl_mscAux.h"
//#include "csl_nand.h"
#include "csl_pll.h"
#include "csl_pllAux.h"
#include "csl_rtc.h"
#include "csl_sar.h"
#include "csl_sdio.h"
#include "csl_spi.h"
#include "csl_sysctrl.h"
#include "csl_types.h"
#include "csl_uart.h"
#include "csl_uartAux.h"
#include "csl_uhpi.h"
#include "csl_uhpiAccess.h"
#include "csl_uhpiAux.h"
#include "csl_usb.h"
#include "csl_usbAux.h"
#include "csl_wdt.h"

#include "cslr.h"
#include "cslr_cpu.h"
#include "cslr_dma.h"
#include "cslr_emif.h"
#include "cslr_gpio.h"
#include "cslr_i2c.h"
#include "cslr_i2s.h"
#include "cslr_idle.h"
#include "cslr_lcdc.h"
//#include "cslr_ido.h"
#include "cslr_mcbsp.h"
#include "cslr_mcspi.h"
#include "cslr_mmcsd.h"
#include "cslr_rtc.h"
#include "cslr_sar.h"
#include "cslr_spi.h"
#include "cslr_sysctrl.h"
#include "cslr_tim.h"
#include "cslr_uart.h"
#include "cslr_uhpi.h"
#include "cslr_usb.h"
#include "cslr_wdt.h"

#include "platform.h"

#define PLATFORM_READ_IN              1


#endif


