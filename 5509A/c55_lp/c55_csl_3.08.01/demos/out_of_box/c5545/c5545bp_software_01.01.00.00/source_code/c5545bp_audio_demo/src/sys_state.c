/*
* sys_state.c
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

// RTS includes
#include <stdio.h>
#include <stdlib.h>
// DSP/BIOS includes
#include <std.h>
#include "c55AudioDemocfg.h"
// CSL includes
#include "csl_types.h"
#include "csl_error.h"
#include "soc.h"
#include "csl_intc.h"
#include "csl_rtc.h"

// application includes
#include "app_globals.h"
#include "pll_control.h"
#include "gpio_control.h"
#include "lcd_osd.h"
#include "codec_config.h"
#include "audio_data_collection.h"
#include "user_interface.h"
#include "recognizer.h"
#include "sys_init.h"
#include "sys_state.h"
#include "ble_fw_upgrade.h"

#include "csl_spi.h"

Uint8 gSysState = SYS_STATE_BUSY;
Uint8 gSysGoalState;
Uint8 gSysPrevState = SYS_STATE_BUSY;
Uint8 gAudioSrc = AUDIO_SOURCE_SD;
Uint8 gsysBoot = 1;

Uint8 gUpdateDisp = 1;
Uint8 gSkipDispUpdate = 0;

Uint8 gVoiceCmd = 0;
Uint8 gPushButon = 0;
volatile Uint8 gSPINotify = 0;

//CSL_GpioObj     GpioObj;
CSL_GpioObj     *gpioHandle;

extern GPIO_Handle hGpio;
extern Uint8 gFileName[10];

extern CSL_SpiHandle	hSpi;

Uint16	writeBuffer[4] = {0x0001, 0x0002, 0x0003, 0x0004};
Uint16  readBuffer[8] = {0x0000,0x0000,0x0000,0x0000};

Uint16  sysStateInfo[8] = {0, 3, 1, 0, 0, 0, 0, 0};

extern Int gain[5];
extern pI2cHandle    i2cHandle;
extern Uint8 gBleFWFound;

Bool gSysInitDone = 0;

stateLookup gStateLookup[SYS_STATE_MAX_COUNT] =
{
	SYS_STATE_BLE_FW_FOUND, {{SWITCH_SW2, CMD_BLEFW_UPGRADE, SYS_STATE_BLE_FW_UPGRADE},
	                         {SWITCH_SW3, CMD_BLEFW_SKIP, SYS_STATE_AUDSRC_SELECT},
	                         {SWITCH_SW4, CMD_NONE, SYS_STATE_BLE_FW_FOUND}},
	SYS_STATE_AUDSRC_SELECT, {{SWITCH_SW2, CMD_NONE, SYS_STATE_AUDSRC_SELECT},
	                          {SWITCH_SW3, CMD_AUDSRC_SEL, SYS_STATE_IDLE},
	                          {SWITCH_SW4, CMD_AUDSRC_SEL, SYS_STATE_IDLE}},
	SYS_STATE_IDLE, {{SWITCH_SW2, CMD_PLAY_START, SYS_STATE_PLAY},
	                 {SWITCH_SW3, CMD_NONE, SYS_STATE_IDLE},
	                 {SWITCH_SW4, CMD_NONE, SYS_STATE_IDLE}},
	SYS_STATE_PLAY, {{SWITCH_SW2, CMD_PLAY_PAUSE, SYS_STATE_PAUSE},
	                 {SWITCH_SW3, CMD_PLAY_STOP, SYS_STATE_STOP},
	                 {SWITCH_SW4, CMD_NONE, SYS_STATE_IDLE}},
	SYS_STATE_PAUSE, {{SWITCH_SW2, CMD_PLAY_RESUME, SYS_STATE_PLAY},
	                 {SWITCH_SW3, CMD_PLAY_STOP, SYS_STATE_STOP},
	                 {SWITCH_SW4, CMD_NONE, SYS_STATE_IDLE}},
	SYS_STATE_STOP, {{SWITCH_SW2, CMD_PLAY_START, SYS_STATE_PLAY},
	                 {SWITCH_SW3, CMD_NONE, SYS_STATE_IDLE},
	                 {SWITCH_SW4, CMD_NONE, SYS_STATE_IDLE}},
	SYS_STATE_BUSY, {{SWITCH_SW2, CMD_NONE, SYS_STATE_BUSY},
	                 {SWITCH_SW3, CMD_NONE, SYS_STATE_BUSY},
	                 {SWITCH_SW4, CMD_NONE, SYS_STATE_BUSY}},
	SYS_STATE_BLE_FW_UPGRADE, {{SWITCH_SW2, CMD_NONE, SYS_STATE_BLE_FW_UPGRADE},
	                           {SWITCH_SW3, CMD_NONE, SYS_STATE_BLE_FW_UPGRADE},
	                           {SWITCH_SW4, CMD_NONE, SYS_STATE_BLE_FW_UPGRADE}},
};

int spiIntCnt = 0;
volatile Uint8 spiWritePending = 0;
extern volatile Uint32 gptIsrCnt;
/* Reads data from BLE module */
void swi_ReadSPIDataFxn(void)
{
	int i;

	spiIntCnt++;

	SPI_read(hSpi, readBuffer, 8);

	for (i = 3; i < 8; i++)
	{
		if(readBuffer[i] <= 15)
		{
			if((gAudioSrc == AUDIO_SOURCE_SD) && (readBuffer[i] > 8))
			{
				readBuffer[i] = 8;
			}

			gain[i-3] = readBuffer[i];
		}
		else if((readBuffer[i] >= 241) && (readBuffer[i] <= 255))
		{
			if((gAudioSrc == AUDIO_SOURCE_SD) && (readBuffer[i] < 248))
			{
				readBuffer[i] = 248;
			}

			gain[i-3] = readBuffer[i] - 256;
		}
	}
}

/* Sends data to BLE module */
void swi_SendSPIDataFxn(void)
{
	volatile Uint16 delay;

	if(gSysState == SYS_STATE_PLAY)
	{
		sysStateInfo[3] = 1;
	}
	else if(gSysState == SYS_STATE_PAUSE)
	{
		sysStateInfo[3] = 2;
	}
	else
	{
		sysStateInfo[3] = 0;
	}

	GPIO_write(hGpio, CSL_GPIO_PIN10, 1);
	gptIsrCnt = 0;
	spiWritePending = 1;
}

Uint8 displayScreen = 0;
/* Updates OLED display */
Void updateDisplay(Void)
{
	clear_lcd(i2cHandle);

	switch(gSysState)
	{
		case SYS_STATE_IDLE:
			print_string("C5545 BP", 1, i2cHandle);
			print_string("Audio Demo", 0, i2cHandle);
			break;

		case SYS_STATE_PLAY:
			print_string("Playing...", 1, i2cHandle);
			if (gAudioSrc == AUDIO_SOURCE_SD)
			{
				print_string((char*)gFileName, 0, i2cHandle);
			}
			break;

		case SYS_STATE_PAUSE:
			print_string("Paused", 1, i2cHandle);
			break;

		case SYS_STATE_STOP:
			print_string("C5545 BP", 1, i2cHandle);
			print_string("Audio Demo", 0, i2cHandle);
			break;

		case SYS_STATE_BUSY:
			print_string("Initializing", 1, i2cHandle);
			print_string("Wait...", 0, i2cHandle);
			break;

		case SYS_STATE_AUDSRC_SELECT:
			print_string("Select", 1, i2cHandle);
			print_string("Audio Source", 0, i2cHandle);
			break;
		default:
			print_string("C5545 BP", 1, i2cHandle);
			print_string("Audio Demo", 0, i2cHandle);
			break;
	}

	displayScreen = 0;
}

/* Error message display functions */
Void sdAccessErrDisp(Void)
{
	clear_lcd(i2cHandle);
	print_string("!ERROR!", 1, i2cHandle);
	print_string("SD Access", 0, i2cHandle);
}

Void fileAccessErrDisp(Void)
{
	clear_lcd(i2cHandle);
	print_string("!ERROR!", 1, i2cHandle);
	print_string("File Access", 0, i2cHandle);
}

Void fileFormatErrDisp(Void)
{
	clear_lcd(i2cHandle);
	print_string("!ERROR!", 1, i2cHandle);
	print_string("No wav Files", 0, i2cHandle);
}

Void fileReadStatusDisp(Void)
{
	clear_lcd(i2cHandle);
	print_string("Reading File...", 1, i2cHandle);
}

/* Function to update the display periodically */
void swi_UpdateDisplayFxn(void)
{
	if(gsysBoot == 1)
	{
		gsysBoot = 0;
		clear_lcd(i2cHandle);
		print_string("Version", 1, i2cHandle);
		print_string(C5545BP_SW_VERSION, 0, i2cHandle);

		if(gBleFWFound == TRUE)
		{
			gSysState = SYS_STATE_BLE_FW_FOUND;
		}
		else
		{
			gSysState = SYS_STATE_AUDSRC_SELECT;
		}
	}
	else if(gSkipDispUpdate)
	{
		gSkipDispUpdate = 0;
		//Nothing else to do, just to skip display update in some special cases
	}
	else
	{
		clear_lcd(i2cHandle);

		switch(gSysState)
		{
			case SYS_STATE_BLE_FW_FOUND:
				if(displayScreen == 1)
				{
					print_string("BLE FW Found", 1, i2cHandle);
					print_string("Upgrade FW?", 0, i2cHandle);
				}
				else
				{
					print_string("SW2 - Upgrade", 1, i2cHandle);
					print_string("SW3 - Skip", 0, i2cHandle);
				}
				break;
			case SYS_STATE_AUDSRC_SELECT:
				if(displayScreen == 1)
				{
					print_string("Select", 1, i2cHandle);
					print_string("Audio Source", 0, i2cHandle);
				}
				else
				{
					print_string("SW3 - SD Card", 1, i2cHandle);
					print_string("SW4 - LINE IN", 0, i2cHandle);
				}
				break;
			case SYS_STATE_IDLE:
				if(displayScreen == 1)
				{
					print_string("C5545 BP", 1, i2cHandle);
					print_string("Audio Demo", 0, i2cHandle);
				}
				else
				{
					print_string("SW2 - Play", 1, i2cHandle);
				}
				break;

			case SYS_STATE_PLAY:
				if(displayScreen == 1)
				{
					print_string("Playing...", 1, i2cHandle);
				}
				else
				{
					print_string("SW2 - Pause", 1, i2cHandle);
					print_string("SW3 - Stop", 0, i2cHandle);
				}
				break;

			case SYS_STATE_PAUSE:
				if(displayScreen == 1)
				{
					print_string("Paused", 1, i2cHandle);
				}
				else
				{
					print_string("SW2 - Resume", 1, i2cHandle);
					print_string("SW3 - Stop", 0, i2cHandle);
				}
				break;

			case SYS_STATE_STOP:
				if(displayScreen == 1)
				{
					print_string("C5545 BP", 1, i2cHandle);
					print_string("Audio Demo", 0, i2cHandle);
				}
				else
				{
					print_string("SW2 - Play", 1, i2cHandle);
				}
				break;
			case SYS_STATE_BUSY:
				break;

			default:
				break;
		}
	}

	displayScreen ^= 1;
}

/* Software interrupt to upgrade BLE firmware */
void swi_updateBleFWFxn(void)
{
	Uint16 status;

	gSysState = SYS_STATE_BLE_FW_UPGRADE;

	// Turn-off display flipping during BLE Firmware upgrade
	gUpdateDisp = 0;
	clear_lcd(i2cHandle);
	print_string("Upgrading BLE FW", 1, i2cHandle);
    print_string("Do Not Power off", 0, i2cHandle);

	status = bleFWUpgrade();
	if(status == BLE_FWUP_STATUS_SUCESS)
	{
		clear_lcd(i2cHandle);
		print_string("BLE FW Upgrade", 1, i2cHandle);
		print_string("Successful!!", 0, i2cHandle);
	}
	else
	{
		clear_lcd(i2cHandle);
		print_string("Upgrade Failed", 1, i2cHandle);

		if(status == BLE_FWUP_STATUS_FAIL_PING)
		{
			print_string("Ping Failed!", 1, i2cHandle);
		}
		else if(status == BLE_FWUP_STATUS_FAIL_NACK)
		{
			print_string("NACK!!", 1, i2cHandle);
		}
		else if(status == BLE_FWUP_STATUS_FAIL_CMD)
		{
			print_string("Cmd Failed!", 1, i2cHandle);
		}
		else if(status == BLE_FWUP_STATUS_FAIL_DATA)
		{
			print_string("Data Failed!", 1, i2cHandle);
		}
		else if(status == BLE_FWUP_STATUS_FAIL_FILE_READ)
		{
			print_string("File Read Err!", 1, i2cHandle);
		}
		else if(status == BLE_FWUP_STATUS_FAIL_REC)
		{
			print_string("Unknown Record!", 1, i2cHandle);
		}
		else
		{
			print_string("Unknown Error!", 1, i2cHandle);
		}
	}

	gSkipDispUpdate = 1;
	displayScreen   = 0;
	gSysState = SYS_STATE_AUDSRC_SELECT;
	// Turn-on display flipping
	gUpdateDisp     = 1;
}

/* Function to process audio control commands */
Void processCommand(Uint16 cmd)
{
	gSPINotify = 0;

	switch(cmd)
	{
		case CMD_PLAY_START:
			startPlay();
			gSPINotify = 1;
			break;
		case CMD_PLAY_STOP:
			stopPlay();
			gSPINotify = 1;
			break;
		case CMD_PLAY_PAUSE:
			pausePlay();
			gSPINotify = 1;
			break;
		case CMD_PLAY_RESUME:
			resumePlay();
			gSPINotify = 1;
			break;
		case CMD_AUDSRC_SEL:
			selectAudioSource(gPushButon % SWITCH_SW3);
			break;
		case CMD_BLEFW_UPGRADE:
			SWI_post(&SWI_updateBleFW);
			break;
		case CMD_BLEFW_SKIP:
			displayScreen = 0;
			/* Nothing to do */
			break;
		default:
			break;
	}

	gSysPrevState = gSysState;
	gSysState     = gSysGoalState;

	if((gSysState != gSysPrevState) && (gSPINotify == 1))
	{
		gSPINotify = 0;
		SWI_post(&SWI_sendSPIData);
	}
	updateDisplay();
}

/* Function to process user input from push buttons */
Void processPushButton(Void)
{
	if(gStateLookup[gSysState].sw2CmdMap[gPushButon - 1].command != CMD_NONE)
	{
		gSysGoalState = gStateLookup[gSysState].sw2CmdMap[gPushButon - 1].newState;
		processCommand(gStateLookup[gSysState].sw2CmdMap[gPushButon - 1].command);
	}
}

Void processVoiceCmd(Void)
{

}

Void swi_sysStateFxn(Void)
{
	if(gVoiceCmd == TRUE)
	{
		processVoiceCmd();
	}
	else
	{
		processPushButton();
	}
}

int gpio_isr_cnt = 0;
void GPIO_Isr(void)
{
   	Int16 retVal;
   	gpioHandle = hGpio;

   	gPushButon = 0;
   	gpio_isr_cnt++;

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(gpioHandle,CSL_GPIO_PIN11,&retVal)))
    {
		GPIO_clearInt(gpioHandle, CSL_GPIO_PIN11);
        gPushButon = BLE_INT_IN;
        SWI_post(&SWI_readSPIData);
    }

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(gpioHandle,CSL_GPIO_PIN12,&retVal)))
    {
		GPIO_clearInt(gpioHandle,CSL_GPIO_PIN12);
        gPushButon = SWITCH_SW2;
    }

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(gpioHandle,CSL_GPIO_PIN13,&retVal)))
    {
		gPushButon = SWITCH_SW3;
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(gpioHandle,CSL_GPIO_PIN13);
    }

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(gpioHandle,CSL_GPIO_PIN14,&retVal)))
    {
		gPushButon = SWITCH_SW4;
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(gpioHandle,CSL_GPIO_PIN14);
    }

    if ((gPushButon != 0) && (gPushButon != BLE_INT_IN))
    {
		SWI_post(&SWI_SysState);
	}
}

Uint32 rtcIntCnt = 0;
Bool ledState = 0;
void RTC_isr(void)
{
	Uint16 intFlag;

	intFlag = CSL_RTC_REGS->RTCINTFL;
	CSL_RTC_REGS->RTCINTFL = intFlag;

	rtcIntCnt++;

	if(!(rtcIntCnt%2))
	{
		if(gUpdateDisp)
			SWI_post(&SWI_updateDisplay);
	}
}

