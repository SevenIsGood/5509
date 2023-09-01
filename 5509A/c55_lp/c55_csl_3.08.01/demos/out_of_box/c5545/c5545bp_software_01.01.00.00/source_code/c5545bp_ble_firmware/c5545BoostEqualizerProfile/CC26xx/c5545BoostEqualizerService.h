/*******************************************************************************
  Filename:       c5545BoostEqualizerService.h

  Description:    C5545 BoosterPack equalizer service definitions and prototypes

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

#ifndef _C5545BOOSTEQUALIZERSERVICE_H_
#define _C5545BOOSTEQUALIZERSERVICE_H_


#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */
// Profile Parameters
#define EQBANDPROFILE_500HZ                  0  // RW uint8 - Profile Characteristic 1 value
#define EQBANDPROFILE_1KHZ                   1  // RW uint8 - Profile Characteristic 2 value
#define EQBANDPROFILE_2KHZ                   2  // RW uint8 - Profile Characteristic 3 value
#define EQBANDPROFILE_4KHZ                   3  // RW uint8 - Profile Characteristic 4 value
#define EQBANDPROFILE_8KHZ                   4  // RW uint8 - Profile Characteristic 5 value
#define EQBANDPROFILE_PLAYBACK               5  // RW uint8 - Profile Characteristic 6 value

// Service UUID (base F000****-0451-4000-B000-00000000-0000)
#define EQU_BAND_SERV_UUID				0xEB00

#define EQU_BAND_500HZ_UUID				0xEB01
#define EQU_BAND_1KHZ_UUID				0xEB02
#define EQU_BAND_2KHZ_UUID				0xEB03
#define EQU_BAND_4KHZ_UUID				0xEB04
#define EQU_BAND_8KHZ_UUID				0xEB05
#define EQU_BAND_PLAYBACK_STATUS_UUID	0xEB06

#define EQU_BAND_500HZ_USERDESC_UUID				0xEB00

#define EQBANDPROFILE_SERVICE               0x00000001
#define SIZE 8
#define DATA_SIZE 50
#define SET_EQ_CONFIG   1
#define GET_EQ_CONFIG   2
#define GET_PLAY_STATUS 3

/*********************************************************************
 * TYPEDEFS
 */
/* COMMAND PACKET BODY */
typedef struct
{
	int16_t	CmdId;
	uint8_t	dataLength;
	uint8_t equalizerVal[5];
}BLE_CMD_TYP;

typedef enum  {

    AUDIO_PLAY=0,
	AUDIO_PAUSE,
	AUDIO_STOP
} Audio_Play_status;
extern ICall_Semaphore sem;
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed

typedef void (*eqbandProfileChange_t)(uint8 paramID );


typedef struct
{
	eqbandProfileChange_t        pfnEqBandProfileChange;  // Called when characteristic value changes
} eqbandProfileCBs_t;

/*********************************************************************
 * API FUNCTIONS
 */


/*
 * EqBand_addService- Initializes the Equalizer GATT Profile service
 *          by registering GATT attributes with the GATT server.
 *
 */
void ProcessCommand(const BLE_CMD_TYP *Cmd);
static bStatus_t eqbandProfile_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr,
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method);
static bStatus_t eqbandProfile_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method);

extern bStatus_t EqbandProfile_AddService(void);

extern bStatus_t Equalizer_Notify(uint16 connHandle, uint8 value);

/*
 * EqBand_registerAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t EqbandProfile_RegisterAppCBs(eqbandProfileCBs_t *appCallbacks);

/*
 * EqBand_setParameter - Set a EqBand GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to write
 *    value - pointer to data to read.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 */
extern bStatus_t EqbandProfile_SetParameter(uint8_t param, uint8_t len, void *value);

/*
 * EqBand_getParameter - Get a EqBand GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 */
extern bStatus_t EqbandProfile_GetParameter(uint8_t param, void *value);

/*********************************************************************
*********************************************************************/



#ifdef __cplusplus
}
#endif

#endif /* _C5545BOOSTEQUALIZERSERVICE_H_ */
