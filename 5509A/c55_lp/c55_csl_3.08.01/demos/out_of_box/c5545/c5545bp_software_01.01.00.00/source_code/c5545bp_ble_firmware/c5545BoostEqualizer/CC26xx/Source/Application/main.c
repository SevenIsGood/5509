/*******************************************************************************
*  Filename:       main.c
*
*  Description:    main entry of the C5545 BoosterPack equalizer application
*
*  Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
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
*    documentation and/or other materials provided with the distribution.
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
*******************************************************************************/

// XDC, RTOS, driverlib
#include <xdc/runtime/Error.h>
#include <ti/sysbios/family/arm/cc26xx/Power.h>
#include <ti/sysbios/family/arm/cc26xx/PowerCC2650.h>
#include <ti/sysbios/BIOS.h>
#include <driverlib/vims.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/drivers/pin/PINCC26XX.h>


// BLE
#include "bcomdef.h"
#include <stdio.h>

#ifndef USE_DEFAULT_USER_CFG

#include "bleUserConfig.h"

// BLE user defined configuration
bleUserCfg_t user0Cfg = BLE_USER_CFG;

#include "c5545BoostEqualizer.h"

#endif // USE_DEFAULT_USER_CFG
void Spi_createTask(void);

ICall_Semaphore sem;
//static Semaphore_Struct sem;
uint16 notify_event;
PIN_State  hState;
PIN_Handle hpin=NULL;

extern Semaphore_Handle read_status_sem;

// Pin interrupt callbackt

int notifyCnt = 0;
int readStatusSemCnt = 0;

/**********Notify callback for GPIO functions *********/
void notifyCb(PIN_Handle handle, PIN_Id pinId)
{
     // read input value from pinid
	 //if(PIN_getInputValue(pinId))
	 if(1)
	 {
		 notifyCnt++;

	    if (read_status_sem != NULL)
		{
	    	readStatusSemCnt++;
			//Set the event and wake up the task
			notify_event |= NOTIFY_TASK_SELECT_EVT;
			//printf("notify_event=%x \n",notify_event);
			Semaphore_post(read_status_sem);
		}
	}
}

/**********main entry*********/
int main()
{

   	PIN_init(BoardGpioInitTable);
	hpin=PIN_open(&hState,BoardGpioInitTable);

		if(hpin==NULL)
			printf("failed to open\n");

	// Setup pin interrupts and register callback
	PIN_registerIntCb(hpin, notifyCb);
	PIN_setInterrupt(hpin, PIN_ID(2)|PIN_IRQ_POSEDGE);



#ifndef POWER_SAVING
  /* Set constraints for Standby and Idle mode */
  Power_setConstraint(Power_SB_DISALLOW);
  Power_setConstraint(Power_IDLE_PD_DISALLOW);
#endif // POWER_SAVING

  /* Initialize ICall module */
  ICall_init();

  /* Start tasks of external images - Priority 5 */
  ICall_createRemoteTasks();

  SPI_init();
  bspSpiOpen();
  //bspSpiRead(spiRxBuf,SIZE);

  Spi_createTask();

  /* Kick off profile - Priority 3 */
  GAPRole_createTask();

  /* Kick off application - Priority 1 */
  EqualizerBand_createTask();

  /* Kick off application - Priority 1 */
  ProcessCmd_createTask();


  BIOS_start();     /* enable interrupts and start SYS/BIOS */

  return 0;
}

/**
 * Error handlers to be hooked into TI-RTOS
 */
Void smallErrorHook(Error_Block *eb)
{
  for (;;);
}

void exceptionHandler()
{
	volatile uint8_t i = 1;

  while(i){}
}
