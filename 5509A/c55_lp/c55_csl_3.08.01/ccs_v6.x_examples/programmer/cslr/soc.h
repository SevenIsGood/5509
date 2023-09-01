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


/** @file soc.h
 *
 *  @brief This file contains the Chip Description for VC5505
 *
 *  Path: \(CSLPATH)\inc
 */

/* ============================================================================
 * Revision History
 * ================
 * 11-Aug-2008 Created
 * 29 -Sept -2008 Added cslr_gpio.h header file ,
 *                GPIO Overlay structure and Base Address
 * 15 -Oct -2008 Added cslr_cpu.h header file ,
 *                CPU Overlay structure and Base Address
 * 27 -Oct -2010 Added cslr_idle.h header file ,
 *                IDLE Overlay structure and Base Address
 *				 Removed cslr_lcd.h header file
 * 5 -Nov -2010 Changed cslr_anactrl.h to cslr_sar.h,
 								Added cslr_ldo.h header file, LDO Overlay structure and Base Address
 * ============================================================================
 */

#ifndef _SOC_H
#define _SOC_H

#include "cslr.h"
#include "tistdtypes.h"

/*****************************************************************************\
* Include files for all the modules in the device
\*****************************************************************************/

#include "cslr_i2c_1.h"
#include "cslr_i2s_1.h"
#include "cslr_idle_1.h"
#include "cslr_ldo_1.h"
#include "cslr_emif_1.h"
#include "cslr_uart_1.h"
#include "cslr_spi_1.h"
#include "cslr_mmcsd_1.h"
#include "cslr_rtc_1.h"
#include "cslr_dma_1.h"
#include "cslr_sar_1.h"
#include "cslr_usb_1.h"
#include "cslr_gpio_1.h"
#include "cslr_tim_1.h"
#include "cslr_wdt_1.h"
#include "cslr_sys_1.h"
#include "cslr_cpu_1.h"
#include "cslr_uhpi_1.h"
#include "cslr_mcspi_1.h"

/*****************************************************************************\
* Peripheral Instance counts
\*****************************************************************************/
#define CSL_DMA_PER_CNT		4
#define CSL_EMIF_PER_CNT	1
#define CSL_I2C_PER_CNT		1
#define CSL_I2S_PER_CNT		3
#define CSL_MMCSD_PER_CNT	2
#define CSL_PLL_PER_CNT  	1
#define CSL_RTC_PER_CNT		1
#define CSL_SAR_PER_CNT		1
#define CSL_SPI_PER_CNT		1
#define CSL_UART_PER_CNT	1
#define CSL_USB_PER_CNT		1
#define CSL_TIM_PER_CNT		3
#define CSL_WDT_PER_CNT		1
#define CSL_HPI_PER_CNT		1
#define CSL_MCSPI_PER_CNT	1

/*****************************************************************************\
* Peripheral Overlay Structures
\*****************************************************************************/
typedef volatile ioport CSL_UsbRegs				* CSL_UsbRegsOvly;
typedef volatile ioport CSL_I2cRegs				* CSL_I2cRegsOvly;
typedef volatile ioport CSL_I2sRegs				* CSL_I2sRegsOvly;
typedef volatile ioport CSL_EmifRegs			* CSL_EmifRegsOvly;
typedef volatile ioport CSL_UartRegs			* CSL_UartRegsOvly;
typedef volatile ioport CSL_SpiRegs				* CSL_SpiRegsOvly;
typedef volatile ioport CSL_MmcsdRegs			* CSL_MmcsdRegsOvly;
typedef volatile ioport CSL_RtcRegs				* CSL_RtcRegsOvly;
typedef volatile ioport CSL_SarRegs				* CSL_SarRegsOvly;
typedef volatile ioport CSL_GpioRegs			* CSL_GpioRegsOvly;
typedef volatile ioport CSL_IdleRegs			* CSL_IdleRegsOvly;
typedef volatile ioport CSL_LdoRegs				* CSL_LdoRegsOvly;
typedef volatile ioport CSL_SysRegs				* CSL_SysRegsOvly;
typedef volatile ioport CSL_DmaRegs				* CSL_DmaRegsOvly;
typedef volatile CSL_CpuRegs					* CSL_CpuRegsOvly;
typedef volatile ioport CSL_TimRegs				* CSL_TimRegsOvly;
typedef volatile ioport CSL_WdtRegs				* CSL_WdtRegsOvly;
typedef volatile ioport CSL_UhpiRegs			* CSL_UhpiRegsOvly;
typedef volatile ioport CSL_McspiRegs			* CSL_McspiRegsOvly;

/*****************************************************************************\
* Peripheral Base Address
\*****************************************************************************/
#define CSL_USB_REGS                    ((CSL_UsbRegsOvly)  0x8000)
#define CSL_SAR_REGS                    ((CSL_SarRegsOvly)  0x7000)
#define CSL_LDO_REGS                    ((CSL_LdoRegsOvly)  0x7000)
#define CSL_EMIF_REGS                   ((CSL_EmifRegsOvly) 0x1000)
#define CSL_I2C_0_REGS                  ((CSL_I2cRegsOvly)  0x1A00)
#define CSL_I2S0_REGS              		((CSL_I2sRegsOvly) 	0x2800)
#define CSL_I2S2_REGS              		((CSL_I2sRegsOvly) 	0x2A00)
#define CSL_I2S3_REGS              		((CSL_I2sRegsOvly) 	0x2B00)
#define CSL_UART_REGS                   ((CSL_UartRegsOvly) 0x1B00)
#define CSL_SPI_REGS                   	((CSL_SpiRegsOvly)  0x3000)
#define CSL_MMCSD0_REGS           		((CSL_MmcsdRegsOvly)0x3A00)
#define CSL_MMCSD1_REGS           		((CSL_MmcsdRegsOvly)0x3B00)
#define CSL_RTC_REGS              		((CSL_RtcRegsOvly)  0x1900)
#define CSL_DMA0_REGS              		((CSL_DmaRegsOvly)	0x0C00)
#define CSL_DMA1_REGS              		((CSL_DmaRegsOvly)	0x0D00)
#define CSL_DMA2_REGS              		((CSL_DmaRegsOvly)	0x0E00)
#define CSL_DMA3_REGS              		((CSL_DmaRegsOvly)	0x0F00)
#define CSL_GPIO_REGS                   ((CSL_GpioRegsOvly)	0x1C00)
#define CSL_SYSCTRL_REGS                ((CSL_SysRegsOvly)	0x1C00)
#define CSL_IDLECTRL_REGS               ((CSL_IdleRegsOvly)	0x0000)
#define CSL_CPU_REGS 		            ((CSL_CpuRegsOvly)  0x0000)
#define CSL_WDT_REGS                    ((CSL_WdtRegsOvly)  0x1880)
#define CSL_TIM_0_REGS                  ((CSL_TimRegsOvly)  0x1810)
#define CSL_TIM_1_REGS                  ((CSL_TimRegsOvly)  0x1850)
#define CSL_TIM_2_REGS                  ((CSL_TimRegsOvly)  0x1890)
#define CSL_UHPI_REGS                   ((CSL_UhpiRegsOvly) 0x2E00)
#define CSL_MCSPI_REGS                  ((CSL_McspiRegsOvly)0x3400)


/** \brief EMIF CS2 Base address                                             */
#define CSL_EMIF_CS2_DATA_BASE_ADDR                 (0x400000u)
/** \brief EMIF CS3 Base address                                             */
#define CSL_EMIF_CS3_DATA_BASE_ADDR                 (0x600000u)
/** \brief EMIF CS4 Base address                                             */
#define CSL_EMIF_CS4_DATA_BASE_ADDR                 (0x700000u)
/** \brief EMIF CS5 Base address                                             */
#define CSL_EMIF_CS5_DATA_BASE_ADDR                 (0x780000u)
/** \brief EMIF CSx address offset - Default A11 used to drive ALE           */
#define CSL_EMIF_CSx_ADDR_OFFSET                    (0x1000u)
/** \brief EMIF CS2 command offset - Default A12 used to drive CLE           */
#define CSL_EMIF_CSx_CMD_OFFSET                     (0x2000u)
/** \brief NAND CE0 address for DMA access - EMIF CS2                        */
#define CSL_NAND_CE0_ADDR                           (0x2000000u)
/** \brief NAND CE1 address for DMA access - EMIF CS3                        */
#define CSL_NAND_CE1_ADDR                           (0x3000000u)
/** \brief ASYNC CE0 address for DMA access - EMIF CS4                       */
#define CSL_ASYNC_CE0_ADDR                          (0x4000000u)
/** \brief ASYNC CE1 address for DMA access - EMIF CS5                       */
#define CSL_ASYNC_CE1_ADDR                          (0x5000000u)
#endif   // _SOC_H