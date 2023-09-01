/*******************************************************************************
  Filename:       c5545BoostEqualizerService.c

  Description:    C5545 BoosterPack equalizer service


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

/*********************************************************************
 * INCLUDES
 */
#include <stdio.h>
#include "bcomdef.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"
#include "string.h"

#include "c5545BoostEqualizerService.h"
#include "bsp_spi.h"
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>

/*********************************************************************
 * CONSTANTS
 */

#define SERVAPP_NUM_ATTR_SUPPORTED        20



/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
static Semaphore_Struct Sem;

extern Semaphore_Handle process_cmd_sem;


// Equalizer Band GATT Profile Service UUID: 0xEB00
CONST uint8 eqbandProfileServUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(EQU_BAND_SERV_UUID), HI_UINT16(EQU_BAND_SERV_UUID)
};

CONST uint8 eqbandProfile500HXUSERDESCUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(EQU_BAND_500HZ_USERDESC_UUID), HI_UINT16(EQU_BAND_500HZ_USERDESC_UUID)
};

// Characteristic 1 UUID: 0xEB01
CONST uint8 eqbandProfile500HzUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(EQU_BAND_500HZ_UUID), HI_UINT16(EQU_BAND_500HZ_UUID)
};

// Characteristic 2 UUID: 0xEB02
CONST uint8 eqbandProfile1KHzUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(EQU_BAND_1KHZ_UUID), HI_UINT16(EQU_BAND_1KHZ_UUID)
};

// Characteristic 3 UUID: 0xEB03
CONST uint8 eqbandProfile2KHzUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(EQU_BAND_2KHZ_UUID), HI_UINT16(EQU_BAND_2KHZ_UUID)
};

// Characteristic 4 UUID: 0xEB04
CONST uint8 eqbandProfile4KHzUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(EQU_BAND_4KHZ_UUID), HI_UINT16(EQU_BAND_4KHZ_UUID)
};

// Characteristic 5 UUID: 0xEB05
CONST uint8 eqbandProfile8KHzUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(EQU_BAND_8KHZ_UUID), HI_UINT16(EQU_BAND_8KHZ_UUID)
};

// Characteristic 6 UUID: 0xEB06
CONST uint8 eqbandProfilePlaybackUUID[ATT_BT_UUID_SIZE] =
{
  LO_UINT16(EQU_BAND_PLAYBACK_STATUS_UUID), HI_UINT16(EQU_BAND_PLAYBACK_STATUS_UUID)
};

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static eqbandProfileCBs_t *eqbandProfile_AppCBs = NULL;


/*********************************************************************
 * Profile Attributes - variables
 */

// Equalizer Band Profile Service attribute
static CONST gattAttrType_t eqbandProfileService = { ATT_BT_UUID_SIZE, eqbandProfileServUUID };


// Equalizer Band Profile Characteristic 1 Properties
static uint8 eqbandProfile500HzProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 1 Value
int8 eqbandProfile500Hz = 0;

// Equalizer Band Profile Characteristic 1 User Description
static uint8 eqbandProfile500HzUserDesp[20] = "Equalizer Band 500Hz";


// Equalizer Band Profile Characteristic 2 Properties
static uint8 eqbandProfile1KHzProps =  GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 2 Value
int8 eqbandProfile1KHz = 0;

// Equalizer Band Profile Characteristic 2 User Description
static uint8 eqbandProfile1KHzUserDesp[21] = "Equalizer Band 1KHz";

static gattCharCfg_t *eqbandProfileCharConfig;

// Equalizer Band Profile Characteristic 3 Properties
static uint8 eqbandProfile2KHzProps =  GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 3 Value
int8 eqbandProfile2KHz = 0;

// Equalizer Band Profile Characteristic 3 User Description
static uint8 eqbandProfile2KHzUserDesp[19] = "Equalizer Band 2KHz";


// Equalizer Band Profile Characteristic 4 Properties
static uint8 eqbandProfile4KHzProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 4 Value
int8 eqbandProfile4KHz = 0;

// Equalizer Band Profile Characteristic 4 User Description
static uint8 eqbandProfile4KHzUserDesp[19] = "Equalizer Band 4KHz";

// Equalizer Band Profile Characteristic 5 Properties
static uint8 eqbandProfile8KHzProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 5 Value
int8 eqbandProfile8KHz = 0;

// Equalizer Band Profile Characteristic 5 User Description
static uint8 eqbandProfile8KHzUserDesp[19] = "Equalizer Band 8KHz";

// Equalizer band Profile Characteristic 6 Properties
static uint8 eqbandProfilePlaybackProps = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Characteristic 6 Value
static uint8 eqbandProfilePlayback = 0;

// Equalizer Band Profile Characteristic 6 User Description
static uint8 eqbandProfilePlaybackUserDesp[24] = "Equalizer Band Playback";

BLE_CMD_TYP gCmd;

/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t eqbandProfileAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] =
{
  // Equalizer Band Profile Service
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&eqbandProfileService            /* pValue */
  },

    // Characteristic 1 Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &eqbandProfile500HzProps
    },

      // Characteristic Value 1
      {
        { ATT_BT_UUID_SIZE, eqbandProfile500HzUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &eqbandProfile500Hz
      },

      // Characteristic 1 User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        eqbandProfile500HzUserDesp
      },

    // Characteristic 2 Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &eqbandProfile1KHzProps
    },

      // Characteristic Value 2
      {
        { ATT_BT_UUID_SIZE, eqbandProfile1KHzUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &eqbandProfile1KHz
      },

      // Characteristic 2 User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        eqbandProfile1KHzUserDesp
      },

    // Characteristic 3 Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &eqbandProfile2KHzProps
    },

      // Characteristic Value 3
      {
        { ATT_BT_UUID_SIZE, eqbandProfile2KHzUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &eqbandProfile2KHz
      },

      // Characteristic 3 User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        eqbandProfile2KHzUserDesp
      },

    // Characteristic 4 Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &eqbandProfile4KHzProps
    },

      // Characteristic Value 4
      {
        { ATT_BT_UUID_SIZE, eqbandProfile4KHzUUID },
		GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &eqbandProfile4KHz
      },

      // Characteristic 4 User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        eqbandProfile4KHzUserDesp
      },

    // Characteristic 5 Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &eqbandProfile8KHzProps
    },

      // Characteristic Value 5
      {
        { ATT_BT_UUID_SIZE, eqbandProfile8KHzUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &eqbandProfile8KHz
      },

      // Characteristic 5 User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        eqbandProfile8KHzUserDesp
      },

      // Characteristic 6 Declaration
          {
            { ATT_BT_UUID_SIZE, characterUUID },
            GATT_PERMIT_READ,
            0,
            &eqbandProfilePlaybackProps
          },

            // Characteristic Value 6
            {
              { ATT_BT_UUID_SIZE, eqbandProfilePlaybackUUID },
			  GATT_PERMIT_READ,
              0,
              &eqbandProfilePlayback
            },

		      // Characteristic 6 Client Characteristic Configuration
		      {
		        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
		        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
		        0,
		        (uint8 *) &eqbandProfileCharConfig
		      },

            // Characteristic 6 User Description
            {
              { ATT_BT_UUID_SIZE, charUserDescUUID },
              GATT_PERMIT_READ,
              0,
              eqbandProfilePlaybackUserDesp
            },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t eqbandProfile_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr,
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method);
static bStatus_t eqbandProfile_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Equalizer Band Profile Service Callbacks
CONST gattServiceCBs_t eqbandProfileCBs =
{
  eqbandProfile_ReadAttrCB,  // Read callback function pointer
  eqbandProfile_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

void interrupt_dsp(void);
uint8_t rxbuf[DATA_SIZE];
uint8_t txbuf[DATA_SIZE];
extern uint8_t gSpiTxBuf[DATA_SIZE];
extern uint8_t gSpiRxBuf[DATA_SIZE];
int ProcessCmdAPICnt = 0;

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      EqBandProfile_AddService
 *
 * @brief   Initializes the Equalizer band Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t EqBandProfile_AddService( uint32 services )
{
  uint8 status;
  // Not necessary to read any configuration
  // Allocate Client Characteristic Configuration table
  eqbandProfileCharConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) *
                                                            linkDBNumConns );
  if ( eqbandProfileCharConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, eqbandProfileCharConfig );

  if ( services & EQBANDPROFILE_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( eqbandProfileAttrTbl,
                                          GATT_NUM_ATTRS( eqbandProfileAttrTbl ),
                                          GATT_MAX_ENCRYPT_KEY_SIZE,
                                          &eqbandProfileCBs );
  }
  else
  {
    status = SUCCESS;
  }

  return ( status );
}

/*********************************************************************
 * @fn      EqBandProfile_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */

bStatus_t EqBandProfile_RegisterAppCBs( eqbandProfileCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    eqbandProfile_AppCBs = appCallbacks;

    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*********************************************************************
 * @fn      EqBandProfile_SetParameter
 *
 * @brief   Set a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to write
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t EqbandProfile_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;

  switch ( param )
  {
    case EQBANDPROFILE_500HZ:
      if ( len == sizeof ( uint8 ) )
      {
        eqbandProfile500Hz = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case EQBANDPROFILE_1KHZ:
      if ( len == sizeof ( uint8 ) )
      {
    	  eqbandProfile1KHz = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case EQBANDPROFILE_2KHZ:
      if ( len == sizeof ( uint8 ) )
      {
    	  eqbandProfile2KHz = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case EQBANDPROFILE_4KHZ:
	  if ( len == sizeof ( uint8 ) )
	  {
		  eqbandProfile4KHz = *((uint8*)value);
	  }
	  else
	  {
		ret = bleInvalidRange;
	  }
	  break;

    case EQBANDPROFILE_8KHZ:
	  if ( len == sizeof ( uint8 ) )
	  {
		  eqbandProfile8KHz = *((uint8*)value);
	  }
	  else
	  {
		ret = bleInvalidRange;
	  }
	  break;

    case EQBANDPROFILE_PLAYBACK:
      if ( len == sizeof ( uint8 ) )
      {
    	  eqbandProfilePlayback = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return ( ret );
}

/*********************************************************************
 * @fn      SimpleProfile_GetParameter
 *
 * @brief   Get a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t SimpleProfile_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case EQBANDPROFILE_500HZ:
      *((uint8*)value) = eqbandProfile500Hz;
      break;

    case EQBANDPROFILE_1KHZ:
      *((uint8*)value) = eqbandProfile1KHz;
      break;

    case EQBANDPROFILE_2KHZ:
      *((uint8*)value) = eqbandProfile2KHz;
      break;

    case EQBANDPROFILE_4KHZ:
      *((uint8*)value) = eqbandProfile4KHz;
      break;

    case EQBANDPROFILE_8KHZ:
	  *((uint8*)value) = eqbandProfile8KHz;
	  break;

    case EQBANDPROFILE_PLAYBACK:
    	*((uint8*)value) = eqbandProfilePlayback;
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return ( ret );
}

/*********************************************************************
 * @fn      enable_client_notification
 *
 * @brief   enable the client notification.
 *
 * @param   connHandle - connection message was received on

 * @return
 */

void enable_client_notification(connHandle)
{
	uint8 cfgVal=0x01;
	//read the configuration value if not enabled then call write function
	if((GATTServApp_ReadCharCfg(connHandle,eqbandProfileCharConfig)!=cfgVal))
	   GATTServApp_WriteCharCfg(connHandle, eqbandProfileCharConfig,1);
}

/*********************************************************************
 * @fn      Equalizer_Notify
 *
 * @brief   send the equalizer notification.
 *
 * @param   connHandle - connection message was received on

 * @return  SUCCESS or Failure
 */
bStatus_t Equalizer_Notify(uint16 connHandle, uint8 notiVal)
{
  uint16 value = GATTServApp_ReadCharCfg(connHandle,eqbandProfileCharConfig);

  eqbandProfilePlayback = notiVal;

  // If notifications enabled
  if (value & GATT_CLIENT_CFG_NOTIFY)
  {
	  attHandleValueNoti_t noti;
	    noti.pValue = GATT_bm_alloc(connHandle, ATT_HANDLE_VALUE_NOTI,
	                                2, NULL);
	    if (noti.pValue != NULL)
	    {
	      // Set the handle.
	      noti.handle = eqbandProfileAttrTbl[17].handle;
	      noti.len = 1;
	      noti.pValue[0] = notiVal;

    	  // Send the Notification.
	      if (GATT_Notification(connHandle, &noti, FALSE) != SUCCESS)
	      	{
	         GATT_bm_free((gattMsg_t *)&noti, ATT_HANDLE_VALUE_NOTI);
	         return FAILURE;
	       }
	    }
	  return SUCCESS;
  	 }
  return FAILURE;
}

/*********************************************************************
 * @fn          ProcessCommand
 *
 * @brief       Handles exchanging equalizer values with DSP
 *
 * @param       Cmd - Command value
 *
 * @return      SUCCESS, blePending or Failure
 */
void ProcessCommand(const BLE_CMD_TYP *Cmd)
{
	size_t len = Cmd->dataLength;
	memcpy(&gSpiTxBuf, Cmd, len);

	//ProcessCmdAPICnt++;  //debug

	switch(Cmd->CmdId)
	{
		case SET_EQ_CONFIG:
		  	bspSpiWrite(txbuf,len);
		  	interrupt_dsp_assert();
			break;

		case GET_EQ_CONFIG:
		 	bspSpiRead(rxbuf,len);
			break;
	}
}

/*********************************************************************
 * @fn          ProcessCmd_taskFxn
 *
 * @brief       Task to initiate exchanging equalizer values with DSP
 *
 * @param       Cmd - Command value
 *
 * @return      SUCCESS, blePending or Failure
 */
void ProcessCmd_taskFxn(UArg a0, UArg a1)
{
	while (1)
	{
		Semaphore_pend(process_cmd_sem, BIOS_WAIT_FOREVER);
		ProcessCommand(&gCmd);
	}
}

static bStatus_t eqbandProfile_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr,
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method)
{
  bStatus_t status = SUCCESS;


  // If attribute permissions require authorization to read, return error
  if ( gattPermitAuthorRead( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }

  // Make sure it's not a blob operation (no attributes in the profile are long)
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }

  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
      // gattserverapp handles those reads

      // characteristics 1 and 2 have read permissions
      // characteritisc 3 does not have read permissions; therefore it is not
      //   included here
      // characteristic 4 does not have read permissions, but because it
      //   can be sent as a notification, it is included here
      case EQU_BAND_500HZ_UUID:
      case EQU_BAND_1KHZ_UUID:
      case EQU_BAND_2KHZ_UUID:
      case EQU_BAND_4KHZ_UUID:
      case EQU_BAND_8KHZ_UUID:
        *pLen = 1;
        pValue[0] = *pAttr->pValue;

        break;
      case EQU_BAND_PLAYBACK_STATUS_UUID:
    	  *pLen = 1;
    	  pValue[0] = *pAttr->pValue;
        break;

      default:
        // Should never get here! (characteristics 3 and 4 do not have read permissions)
        *pLen = 0;
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    *pLen = 0;
    status = ATT_ERR_INVALID_HANDLE;
  }

  return ( status );
}

/*********************************************************************
 * @fn      simpleProfile_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t eqbandProfile_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method)
{
  bStatus_t status = SUCCESS;

  // If attribute permissions require authorization to write, return error
  if ( gattPermitAuthorWrite( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );

  }

  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      case EQU_BAND_500HZ_UUID:
	  case EQU_BAND_1KHZ_UUID:
	  case EQU_BAND_2KHZ_UUID:
	  case EQU_BAND_4KHZ_UUID:
	  case EQU_BAND_8KHZ_UUID:

        //Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len != 1 )
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }

        //Write the value
        if ( status == SUCCESS )
        {
          uint8 *pCurValue = (uint8 *)pAttr->pValue;
          *pCurValue = pValue[0];
          gCmd.CmdId=SET_EQ_CONFIG;
          gCmd.dataLength=SIZE;

    	  gCmd.equalizerVal[0]=eqbandProfile500Hz;
    	  gCmd.equalizerVal[1]=eqbandProfile1KHz;
    	  gCmd.equalizerVal[2]=eqbandProfile2KHz;
    	  gCmd.equalizerVal[3]=eqbandProfile4KHz;
    	  gCmd.equalizerVal[4]=eqbandProfile8KHz;

          ProcessCommand(&gCmd);
          //Semaphore_post(process_cmd_sem);
        }
        else
        {
         	  printf("failed to change\n");
        }

        break;

      case GATT_CLIENT_CHAR_CFG_UUID:
        status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                                 offset, GATT_CLIENT_CFG_NOTIFY );
        break;

      default:
        // Should never get here! (characteristics 2 and 4 do not have write permissions)
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    status = ATT_ERR_INVALID_HANDLE;
  }
#if 0
  // If a characteristic value changed then callback function to notify application of change
   if ( (notifyApp != 0xFF ) && eqbandProfile_AppCBs && eqbandProfile_AppCBs->pfnEqBandProfileChange )
   {
	   eqbandProfile_AppCBs->pfnEqBandProfileChange(notifyApp);
   }
#endif
  return ( status );
}

/*********************************************************************
*********************************************************************/
