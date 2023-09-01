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

/*! \file diag_config.c
*
*   \brief Defines the configuration parameters for diagnostic tests.
*    Modify the diganostic test table defined in this file to add/delete 
*    tests from the menu.
*
*/ 
 
#include "platform_test.h"
#include "paths.h"

/* Default SD test arguments */
sdCardTestArgs_t gSdCardTestArgs;

/* Default SD test arguments */
ledTestArgs_t gLedTestArgs;

/* Default UART test arguments */
uartTestArgs_t gUartTestArgs =
{
		60000000,
		115200,
		CSL_UART_WORD8,
		0,
		CSL_UART_DISABLE_PARITY,
		CSL_UART_FIFO_DMA1_ENABLE_TRIG14,
		CSL_UART_NO_LOOPBACK,
		CSL_UART_NO_AFE,
		CSL_UART_NO_RTS
};

/* Platform diagnostic test table */
diagTestTable_t gDiagTestTable[PLATFORM_TEST_COUNT] =
{
		{"Auto Run All Tests",NULL, NULL, 0, 0},
		{"SPI Flash Test",spiFlashTest,0,0},
		{"LED Test", ledTest, &gLedTestArgs, 0, 0},
		{"OLED Display Test", oledDisplayTest, NULL, 0, 0},
		{"Audio Playback Test", audioPlaybackTest, NULL,0,0},
		{"Audio LINE IN Loopback Test", audioLineInLoopbackTest, NULL, 0, 0},
		{"Audio On-board MIC Loopback Test", audioMicInLoopbackTest, NULL, 0, 0},
		{"Audio Headset Loopback Test", audioHeadsetLoopbackTest, NULL, 0, 0},
		{"Audio External MIC Loopback Test", externalMicInLoopbackTest, NULL, 0, 0},
		{"Current Monitor Test", currentMonitorTest, NULL, 0, 0},
		{"RTC Test", rtcTest, NULL, 0, 0},
		{"Push Button Test", pushButtonTest, NULL,0, 0},
		{"USB Test", usbTest, NULL, 0, 0},
	//	{"C5545 BP to MSP432 LP Test", gpioDspToLaunchpadMsp432Test, NULL, 0, 0},
		{"I2C DSP Master - MSP Slave", i2cDspMastermspSlaveTest, NULL, 0 , 0},
		{"I2C DSP Slave - MSP Master", dspSlaveMspMasterTest, NULL, 0, 0},
		{"DSP-BLE Interface Test", dspBLETest, NULL, 0, 0}

};
