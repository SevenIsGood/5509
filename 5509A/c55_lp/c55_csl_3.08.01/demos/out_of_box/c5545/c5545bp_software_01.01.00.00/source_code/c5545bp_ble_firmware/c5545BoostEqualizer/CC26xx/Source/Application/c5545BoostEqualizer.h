/*******************************************************************************
  Filename:       c5545BoostEqualizer.h

  Description:    This file contains the equalizer application's
                  definitions and prototypes.

  Copyright 2015-2016 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
*******************************************************************************/

#ifndef _C5545BOOSTEQUALIZER_H_
#define _C5545BOOSTEQUALIZER_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ICall.h"
#include "peripheral.h"
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/PIN.h>

/*********************************************************************
 * CONSTANTS
 */


 /*********************************************************************
 * MACROS
 */
#define FW_VERSION_STR TOSTRING(FW_VERSION)" ("__DATE__")";
#define EQUALIZER_MEAS_NOTI_ENABLED         1

 /*********************************************************************
 * VARIABLES
 */
extern ICall_Semaphore sem;


extern gaprole_States_t gapProfileState;

extern PIN_State pinGpioState;
extern PIN_Handle hGpioPin;
#define NOTIFY_TASK_SELECT_EVT 1
extern uint16 notify_event;



/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task creation function for SensorTag
 */
extern void EqualizerBand_createTask(void);

/*
 * Function to call when a characteristic value has changed
 */
//extern void EqualizerBand_charValueChangeCB(uint8_t sensorID, uint8_t paramID);
extern void EqualizerBand_charValueChangeCB(uint8_t paramID);

/*
 * Function to check the program stack
 */
extern void EqualizerBand_checkStack(void);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _C5545BOOSTEQUALIZER_H_ */
