/*******************************************************************************
  Filename:       c5545BoostEqualizer.c

  Description:    This file is the Equalizer application's main body.

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
  PROVIDED ``AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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
*/

/*******************************************************************************
 * INCLUDES
 */
#include <string.h>
#include <stdio.h>
#include <xdc/std.h>

#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>

#ifdef POWER_SAVING
#include <ti/sysbios/family/arm/cc26xx/Power.h>
#include <ti/sysbios/family/arm/cc26xx/PowerCC2650.h>
#endif

#include <ICall.h>

#include "gatt.h"
#include "hci.h"
#include "gapgattserver.h"
#include "gattservapp.h"
#include "gapbondmgr.h"
#include "osal_snv.h"
#include "ICallBleAPIMSG.h"
#include "util.h"
#include <c5545BoostEqualizerService.h>
#include <ti/drivers/pin/PINCC26XX.h>

#include "bsp_i2c.h"
#include "bsp_spi.h"

#include "sensor.h"
#include "Board.h"
#include "devinfoservice.h"
#include "movementservice.h"
#ifdef FEATURE_REGISTER_SERVICE
#include "registerservice.h"
#endif

// Sensor devices
#include "c5545BoostEqualizer.h"

// Other devices
#include "ext_flash.h"
#include "ext_flash_layout.h"


/*******************************************************************************
 * CONSTANTS
 */
uint8_t value,count=0;

// How often to perform periodic event (in msec)
#define EB_PERIODIC_EVT_PERIOD               1000

// What is the advertising interval when device is discoverable
// (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL          160

// General discoverable mode advertises indefinitely
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_LIMITED

// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic
// parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     8

// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic
// parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     800

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter
// update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          100

// Whether to enable automatic parameter update request when a
// connection is formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         FALSE

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL         1

// Company Identifier: Texas Instruments Inc. (13)
#define TI_COMPANY_ID                         0x000D
#define TI_EB_DEVICE_ID                       0x03
#define TI_EB_KEY_DATA_ID                     0x00

// Length of board address
#define B_ADDR_STR_LEN                        15

#if defined ( PLUS_BROADCASTER )
  #define ADV_IN_CONN_WAIT                    500 // delay 500 ms
#endif

// Task configuration
#define EB_TASK_PRIORITY                      2
#define EB_TASK_STACK_SIZE                    700


// spi Task configuration
#define SPI_TASK_PRIORITY                      5
#define SPI_TASK_STACK_SIZE                    1000

// Internal Events for RTOS application
#define EB_STATE_CHANGE_EVT                   0x0001
#define EB_CHAR_CHANGE_EVT                    0x0002
#define EB_PERIODIC_EVT                       0x0004


// Misc.
#define INVALID_CONNHANDLE                    0xFFFF
#define TEST_INDICATION_BLINKS                5  // Number of blinks
#define BLINK_DURATION                        20 // Milliseconds
#define OAD_PACKET_SIZE                       18
#define KEY_STATE_OFFSET                      13 // Offset in advertising data

#define SERVAPP_NUM_ATTR_SUPPORTED        19
#define PWRMGT_1  2

static gattAttribute_t eqbandProfileAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED];

// GAP connection handle.
static uint16_t gapConnHandle;

int gNotifyApp = 0;
uint8 gNotifyVal = 0;

/*end*/
/*******************************************************************************
 * TYPEDEFS
 */

// App event passed from profiles.
typedef struct
{
  uint8_t event;  // Which profile's event
  uint8_t serviceID; // New status
  uint8_t paramID;
} stEvt_t;

/*******************************************************************************
 * GLOBAL VARIABLES
 */
// Profile state and parameters
gaprole_States_t gapProfileState = GAPROLE_INIT;

// Semaphore globally used to post events to the application thread
//ICall_Semaphore sem;


// Global pin resources
PIN_State pinGpioState;
PIN_Handle hGpioPin;

/*******************************************************************************
 * LOCAL VARIABLES
 */
uint8_t status;
// Task configuration
Task_Struct equalizerBandTask;
Task_Struct processCmdTask;
static Char equalizerBandTaskStack[EB_TASK_STACK_SIZE];
static Char processCmdTaskStack[EB_TASK_STACK_SIZE];

// SPI Task configuration
Task_Struct SPI_Task;
static Char SPI_TaskStack[SPI_TASK_STACK_SIZE];


// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Clock instances for internal periodic events.
static Clock_Struct periodicClock;

// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;

// events flag for internal application events.
static uint16_t events;

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8_t scanRspData[] =
{
  // complete name
  0xC,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  //Device Name
  0x42,   // 'B'
  0x6F,   // 'o'
  0x6F,   // 'o'
  0x73,   // 's'
  0x74,   // 't'
  0x65,   // 'e'
  0x72,   // 'r'
  0x50,   // 'P'
  0x61,   // 'a'
  0x63,   // 'c'
  0x6B,   // 'k'

  // connection interval range
  0x05,   // length of this data
  GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
  LO_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),
  HI_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),
  LO_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),
  HI_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),

  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertising)
static uint8_t advertData[] =
{
  // Flags; this sets the device to use limited discoverable
  // mode (advertises for 30 seconds at a time) instead of general
  // discoverable mode (advertises indefinitely)
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

  // service UUID, to notify central devices what services are included
  // in this peripheral
  0x03,   // length of this data
  GAP_ADTYPE_16BIT_MORE,      // some of the UUID's, but not all
  LO_UINT16(MOVEMENT_SERV_UUID),
  HI_UINT16(MOVEMENT_SERV_UUID),

  // Manufacturer specific advertising data
  0x06,
  GAP_ADTYPE_MANUFACTURER_SPECIFIC,
  LO_UINT16(TI_COMPANY_ID),
  HI_UINT16(TI_COMPANY_ID),
  TI_EB_DEVICE_ID,
  TI_EB_KEY_DATA_ID,
  0x00                                    // Key state
};

// GAP GATT Attributes
static const uint8_t attDeviceName[12] = {'B','o','o','s','t','e','r','P','a','c','k','\0'};

// Device information parameters
static const uint8_t devInfoModelNumber[] = "C5545 Boost Equalizer";
static const uint8_t devInfoNA[] =          "N.A.";
static const uint8_t devInfoFirmwareRev[] = FW_VERSION_STR;
static const uint8_t devInfoMfrName[] =     "Texas Instruments";
static const uint8_t devInfoHardwareRev[] = "PCB 1.2/1.3";

extern bStatus_t EqBandProfile_RegisterAppCBs( eqbandProfileCBs_t *appCallbacks );
void ProcessCmd_taskFxn(UArg a0, UArg a1);
extern Semaphore_Handle read_status_sem;
uint8_t spiRxBuf[SIZE];

/*******************************************************************************
 * LOCAL FUNCTIONS
 */

static void EqualizerBand_init( void);
static void EqualizerBand_taskFxn( UArg a0, UArg a1);
static void EqualizerBand_processStackMsg( ICall_Hdr *pMsg);
static void EqualizerBand_processGATTMsg(gattMsgEvent_t *pMsg);
static void EqualizerBand_processAppMsg( stEvt_t *pMsg);
static void EqualizerBand_processStateChangeEvt( gaprole_States_t newState ) ;
static void EqualizerBand_processCharValueChangeEvt(uint8_t paramID );
static void EqualizerBand_performPeriodicTask( void);
static void EqualizerBand_stateChangeCB( gaprole_States_t newState);
static void EqualizerBand_resetAllSensors(void);
static void EqualizerBand_clockHandler(UArg arg);
static void EqualizerBand_enqueueMsg(uint8_t event, uint8_t serviceID, uint8_t paramID);
static void EqualizerBand_callback(PIN_Handle handle, PIN_Id pinId);
static bool EqualizerBand_hasFactoryImage(void);
static void EqualizerBand_setDeviceInfo(void);


/*******************************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t EqualizerBand_gapRoleCBs =
{
  EqualizerBand_stateChangeCB,     // Profile State Change Callbacks
};

//characteristic value call back function
static eqbandProfileCBs_t  equalizerProfileCBs =
{
	EqualizerBand_charValueChangeCB // Characteristic value change callback
};


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/*******************************************************************************
 * @fn      SPI_taskFxn
 *
 * @brief   Application task entry point for the spi_task
 *
 * Handles the data coming from DSP on SPI bus
 *
 * @param   a0, a1 (not used)
 *
 * @return  none
 */
void SPI_taskFxn(UArg a0, UArg a1)
{
	while(1)
	{
		Semaphore_pend(read_status_sem, BIOS_WAIT_FOREVER);
		bspSpiRead(spiRxBuf, SIZE);
	}
}

/*******************************************************************************
 * @fn      EqualizerBand_createTask
 *
 * @brief   Task creation function for the EqualizerBand.
 *
 * @param   none
 *
 * @return  none
 */

void EqualizerBand_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = equalizerBandTaskStack;
  taskParams.stackSize = EB_TASK_STACK_SIZE;
  taskParams.priority = EB_TASK_PRIORITY;

  Task_construct(&equalizerBandTask, EqualizerBand_taskFxn, &taskParams, NULL);
}

void ProcessCmd_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = processCmdTaskStack;
  taskParams.stackSize = EB_TASK_STACK_SIZE;
  taskParams.priority = 5;

  Task_construct(&processCmdTask, ProcessCmd_taskFxn, &taskParams, NULL);
}

/*******************************************************************************
 * @fn      Spi_createTask
 *
 * @brief   Task creation function for the spi.
 *
 * @param   none
 *
 * @return  none
 */
void Spi_createTask(void)
{

  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = SPI_TaskStack;
  taskParams.stackSize = SPI_TASK_STACK_SIZE;
  taskParams.priority = SPI_TASK_PRIORITY;

  Task_construct(&SPI_Task, SPI_taskFxn, &taskParams, NULL);

}
/*******************************************************************************
 * @fn      EqualizerBand_init
 *
 * @brief   Called during initialization and contains application
 *          specific initialization (ie. hardware initialization/setup,
 *          table initialization, power up notification, etc), and
 *          profile initialization/setup.
 *
 * @param   none
 *
 * @return  none
 */
static void EqualizerBand_init(void)
{
  // ***************************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ***************************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntity, &sem);

  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueue = Util_constructQueue(&appMsg);

  // Create one-shot clocks for internal periodic events.
  Util_constructClock(&periodicClock, EqualizerBand_clockHandler,
                      EB_PERIODIC_EVT_PERIOD, 0, false, EB_PERIODIC_EVT);

  // Setup the GAP
  GAP_SetParamValue(TGAP_CONN_PAUSE_PERIPHERAL, DEFAULT_CONN_PAUSE_PERIPHERAL);

  // Setup the GAP Peripheral Role Profile
  {
    // For all hardware platforms, device starts advertising upon initialization
    uint8_t initialAdvertEnable = TRUE;

    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until the enabler is set back to TRUE
    uint16_t advertOffTime = 0;

    uint8_t enableUpdateRequest = DEFAULT_ENABLE_UPDATE_REQUEST;
    uint16_t desiredMinInterval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16_t desiredMaxInterval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16_t desiredSlaveLatency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16_t desiredConnTimeout = DEFAULT_DESIRED_CONN_TIMEOUT;

    // Set the GAP Role Parameters
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                         &initialAdvertEnable);
    GAPRole_SetParameter(GAPROLE_ADVERT_OFF_TIME, sizeof(uint16_t),
                         &advertOffTime);

    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData),
                         scanRspData);
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);

    GAPRole_SetParameter(GAPROLE_PARAM_UPDATE_ENABLE, sizeof(uint8_t),
                         &enableUpdateRequest);
    GAPRole_SetParameter(GAPROLE_MIN_CONN_INTERVAL, sizeof(uint16_t),
                         &desiredMinInterval);
    GAPRole_SetParameter(GAPROLE_MAX_CONN_INTERVAL, sizeof(uint16_t),
                         &desiredMaxInterval);
    GAPRole_SetParameter(GAPROLE_SLAVE_LATENCY, sizeof(uint16_t),
                         &desiredSlaveLatency);
    GAPRole_SetParameter(GAPROLE_TIMEOUT_MULTIPLIER, sizeof(uint16_t),
                         &desiredConnTimeout);
  }

  // Set the GAP Characteristics
  GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN,
                   (void*)attDeviceName);

  // Set advertising interval
  {
    uint16_t advInt = DEFAULT_ADVERTISING_INTERVAL;

    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, advInt);
  }

  // Setup the GAP Bond Manager
	{
	  uint32_t passkey = 0; // passkey "000000"
	  uint8_t pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
	  uint8_t mitm = TRUE;
	  uint8_t ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
	  uint8_t bonding = TRUE;

	  GAPBondMgr_SetParameter(GAPBOND_DEFAULT_PASSCODE, sizeof(uint32_t),
							  &passkey);
	  GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t), &pairMode);
	  GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof(uint8_t), &mitm);
	  GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
	  GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof(uint8_t), &bonding);
	}


   // Initialize GATT attributes
  GGS_AddService(GATT_ALL_SERVICES);           // GAP
  GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT attributes
  DevInfo_AddService();                        // Device Information Service

  EqBandProfile_AddService(GATT_ALL_SERVICES);

  EqBandProfile_RegisterAppCBs(&equalizerProfileCBs);

  // Add application specific device information
  EqualizerBand_setDeviceInfo();

  // Setup the Equalizer Band Characteristic Values
    {
      uint8_t charValue1 = 0;
      uint8_t charValue2 = 0;
      uint8_t charValue3 = 0;
      uint8_t charValue4 = 0;
      uint8_t charValue5 = 0;
      uint8_t charValue6 = 0;

      EqbandProfile_SetParameter(EQBANDPROFILE_500HZ, sizeof(uint8_t),
                                 &charValue1);
      EqbandProfile_SetParameter(EQBANDPROFILE_1KHZ, sizeof(uint8_t),
                                 &charValue2);
      EqbandProfile_SetParameter(EQBANDPROFILE_2KHZ, sizeof(uint8_t),
                                 &charValue3);
      EqbandProfile_SetParameter(EQBANDPROFILE_4KHZ, sizeof(uint8_t),
                                 &charValue4);
      EqbandProfile_SetParameter(EQBANDPROFILE_8KHZ, sizeof(uint8_t),
                                 &charValue5);
      EqbandProfile_SetParameter(EQBANDPROFILE_PLAYBACK, sizeof(uint8_t),
                                      &charValue6);
    }

  // Start the Device

  status=GAPRole_StartDevice(&EqualizerBand_gapRoleCBs);

  // Start Bond Manager
  VOID GAPBondMgr_Register(NULL);
}

/*******************************************************************************
 * @fn      EqualizerBand_taskFxn
 *
 * @brief   Application task entry point for the EqualizerBand
 *
 * @param   a0, a1 (not used)
 *
 * @return  none
 */

static void EqualizerBand_taskFxn(UArg a0, UArg a1)
{
  // Initialize application
  EqualizerBand_init();
  uint8_t dioreadPin=0;
  BLE_CMD_TYP cmd;


  // Application main loop
  for (;;)
  {
    // Waits for a signal to the semaphore associated with the calling thread.
    // Note that the semaphore associated with a thread is signalled when a
    // message is queued to the message receive queue of the thread or when
    // ICall_signal() function is called onto the semaphore.
      ICall_Errno errno = ICall_wait(ICALL_TIMEOUT_FOREVER);

      dioreadPin=PIN_getInputValue(PWRMGT_1);
      if(dioreadPin)
      {
    	  cmd.CmdId=GET_EQ_CONFIG;
    	  cmd.dataLength=SIZE;
    	  ProcessCommand(&cmd);
      }

      if(gNotifyApp)
      {
    	  gNotifyApp = 0;
    	  //gNotifyVal ^= 1;
    	  Equalizer_Notify(gapConnHandle, gNotifyVal);
      }

    if (errno == ICALL_ERRNO_SUCCESS)
    {
      ICall_EntityID dest;
      ICall_ServiceEnum src;
      ICall_HciExtEvt *pMsg = NULL;

      if (ICall_fetchServiceMsg(&src, &dest,
                                (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
      {
        if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
        {
          // Process inter-task message
          EqualizerBand_processStackMsg((ICall_Hdr *)pMsg);
        }

        if (pMsg)
        {
          ICall_freeMsg(pMsg);
        }
      }

      // If RTOS queue is not empty, process app message.
      while (!Queue_empty(appMsgQueue))
      {
        stEvt_t *pMsg = (stEvt_t *)Util_dequeueMsg(appMsgQueue);
        if (pMsg)
        {
          // Process message.
          EqualizerBand_processAppMsg(pMsg);

          // Free the space from the message.
          ICall_free(pMsg);
        }
      }
    }


    if (!!(events & EB_PERIODIC_EVT))
    {
      events &= ~EB_PERIODIC_EVT;

      Util_startClock(&periodicClock);

     }
  } // task loop
}


/*******************************************************************************
 * @fn      EqualizerBand_setDeviceInfo
 *
 * @brief   Set application specific Device Information
 *
 * @param   none
 *
 * @return  none
 */
static void EqualizerBand_setDeviceInfo(void)
{
  DevInfo_SetParameter(DEVINFO_MODEL_NUMBER, sizeof(devInfoModelNumber),
                       (void*)devInfoModelNumber);
  DevInfo_SetParameter(DEVINFO_SERIAL_NUMBER, sizeof(devInfoNA),
                       (void*)devInfoNA);
  DevInfo_SetParameter(DEVINFO_SOFTWARE_REV, sizeof(devInfoNA),
                       (void*)devInfoNA);
  DevInfo_SetParameter(DEVINFO_FIRMWARE_REV, sizeof(devInfoFirmwareRev),
                       (void*)devInfoFirmwareRev);
  DevInfo_SetParameter(DEVINFO_HARDWARE_REV, sizeof(devInfoHardwareRev),
                       (void*)devInfoHardwareRev);
  DevInfo_SetParameter(DEVINFO_MANUFACTURER_NAME, sizeof(devInfoMfrName),
                       (void*)devInfoMfrName);
}

/*******************************************************************************
 * @fn      EqualizerBand_processAppMsg
 *
 * @brief   Process an incoming callback from a profile.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void EqualizerBand_processAppMsg(stEvt_t *pMsg)
{
  switch (pMsg->event)
  {
    case EB_STATE_CHANGE_EVT:
      EqualizerBand_processStateChangeEvt((gaprole_States_t)pMsg->serviceID);
      break;

    case EB_CHAR_CHANGE_EVT:
      EqualizerBand_processCharValueChangeEvt(pMsg->paramID);
      break;

    default:
      // Do nothing.
      break;
  }
}

/*******************************************************************************
 * @fn      EqualizerBand_stateChangeCB
 *
 * @brief   Callback from GAP Role indicating a role state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void EqualizerBand_stateChangeCB(gaprole_States_t newState)
{
  EqualizerBand_enqueueMsg(EB_STATE_CHANGE_EVT, newState, NULL);
  //printf("EqualizerBand_stateChangeCB\n");
}

/*******************************************************************************
 * @fn      EqualizerBand_processStateChangeEvt
 *
 * @brief   Process a pending GAP Role state change event.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void EqualizerBand_processStateChangeEvt(gaprole_States_t newState)
{
#ifdef PLUS_BROADCASTER
  static bool firstConnFlag = false;
#endif // PLUS_BROADCASTER
  switch (newState)
  {
  case GAPROLE_STARTED:
    {
      uint8_t ownAddress[B_ADDR_LEN];
      uint8_t systemId[DEVINFO_SYSTEM_ID_LEN];
      GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddress);

      // use 6 bytes of device address for 8 bytes of system ID value
      systemId[0] = ownAddress[0];
      systemId[1] = ownAddress[1];
      systemId[2] = ownAddress[2];

      // set middle bytes to zero
      systemId[4] = 0x00;
      systemId[3] = 0x00;

      // shift three bytes up
      systemId[7] = ownAddress[5];
      systemId[6] = ownAddress[4];
      systemId[5] = ownAddress[3];

      DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);
    }
    break;

  case GAPROLE_ADVERTISING:
    // Start the clock
    if (!Util_isActive(&periodicClock))
    {
      Util_startClock(&periodicClock);
    }
    break;

  case GAPROLE_CONNECTED:
    {
        Util_startClock(&periodicClock);
        GAPRole_GetParameter(GAPROLE_CONNHANDLE, &gapConnHandle);

#ifdef PLUS_BROADCASTER
      // Only turn advertising on for this state when we first connect
      // otherwise, when we go from connected_advertising back to this state
      // we will be turning advertising back on.
      if (firstConnFlag == false)
      {
        uint8_t advertEnabled = TRUE; // Turn on Advertising

        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                             &advertEnabled);
        firstConnFlag = true;
      }
#endif // PLUS_BROADCASTER
    }
   //LCD_WRITES_STATUS("Connected");
    break;

  case GAPROLE_CONNECTED_ADV:
    break;

  case GAPROLE_WAITING:
  case GAPROLE_WAITING_AFTER_TIMEOUT:
    EqualizerBand_resetAllSensors();
   // LCD_WRITES_STATUS("Waiting...");
    break;

  case GAPROLE_ERROR:
    EqualizerBand_resetAllSensors();
    //PIN_setOutputValue(hGpioPin,Board_LED1, Board_LED_ON);
    //LCD_WRITES_STATUS("Error");
    break;

  default:
    break;
  }

  gapProfileState = newState;
}

/*******************************************************************************
 * @fn      EqualizerBand_charValueChangeCB
 *
 * @brief   Callback from Sensor Profile indicating a characteristic
 *          value change.
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  none
 */
void EqualizerBand_charValueChangeCB(uint8_t paramID)
{
  EqualizerBand_enqueueMsg(EB_CHAR_CHANGE_EVT, 0, paramID);
}

/*******************************************************************************
 * @fn      EqualizerBand_applyFactoryImage
 *
 * @brief   Load the factory image from external flash and reboot
 *
 * @return  none
 */
void EqualizerBand_applyFactoryImage(void)
{
  if (EqualizerBand_hasFactoryImage())
  {
    // Load and launch factory image; page 31 must be omitted
    ((void (*)(uint32_t, uint32_t, uint32_t))BL_OFFSET)(EFL_ADDR_RECOVERY,
                                                        EFL_SIZE_RECOVERY-0x1000,
                                                        0);
  }
}

/*******************************************************************************
 * @fn      EqualizerBand_processCharValueChangeEvt
 *
 * @brief   Process pending Profile characteristic value change
 *          events. The events are generated by the network task (BLE)
 *
 * @param   serviceID - ID of the affected service
 * @param   paramID - ID of the affected parameter
 *
 * @return  none
 */
static void EqualizerBand_processCharValueChangeEvt(uint8_t paramID)
{

}

/*******************************************************************************
 * @fn      EqualizerBand_processStackMsg
 *
 * @brief   Process an incoming stack message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void EqualizerBand_processStackMsg(ICall_Hdr *pMsg)
{
  switch (pMsg->event)
  {
    case GATT_MSG_EVENT:
      // Process GATT message
      EqualizerBand_processGATTMsg((gattMsgEvent_t *)pMsg);
      break;

    default:
      // do nothing
      break;
  }
}

/*******************************************************************************
 * @fn      EqualizerBand_processGATTMsg
 *
 * @brief   Process GATT messages
 *
 * @return  none
 */
static void EqualizerBand_processGATTMsg(gattMsgEvent_t *pMsg)
{
  GATT_bm_free(&pMsg->msg, pMsg->method);
}

/*******************************************************************************
 * @fn      EqualizerBand_performPeriodicTask
 *
 * @brief   Perform a periodic application task.
 *
 * @param   none
 *
 * @return  none
 */
static void EqualizerBand_performPeriodicTask( void)
{
	uint16_t connHandle =eqbandProfileAttrTbl[17].handle;
    void *value;
    uint8 var=1;
    uint8 param=EQBANDPROFILE_PLAYBACK;
    enable_client_notification(connHandle);

    while(var)
    {
    	SimpleProfile_GetParameter(param, value);
    	if(*(uint8_t *)value!=0)
    	{
    	 Equalizer_Notify(connHandle, 0);
    	 var=0;
    	}
    }
}

/*******************************************************************************
 * @fn      EqualizerBand_clockHandler
 *
 * @brief   Handler function for clock time-outs.
 *
 * @param   arg - event type
 *
 * @return  none
 */
static void EqualizerBand_clockHandler(UArg arg)
{
  // Store the event.
  events |= arg;

  // Wake up the application.
  Semaphore_post(sem);
}

/*******************************************************************************
 * @fn      EqualizerBand_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   serviceID - service identifier
 * @param   paramID - parameter identifier
 *
 * @return  none
 */
static void EqualizerBand_enqueueMsg(uint8_t event, uint8_t serviceID, uint8_t paramID)
{
  stEvt_t *pMsg;

  // Create dynamic pointer to message.
  if (pMsg = ICall_malloc(sizeof(stEvt_t)))
  {
    pMsg->event = event;
    pMsg->serviceID = serviceID;
    pMsg->paramID = paramID;

    // Enqueue the message.
    Util_enqueueMsg(appMsgQueue, sem, (uint8_t*)pMsg);
  }
}

/*********************************************************************
 * @fn      EqualizerBand_resetAllSensors
 *
 * @brief   Reset all sensors, typically when a connection is intentionally
 *          terminated.
 *
 * @param   none
 *
 * @return  none
 */
static void EqualizerBand_resetAllSensors(void)
{

}

/*!*****************************************************************************
 *  @fn         EqualizerBand_callback
 *
 *  Interrupt service routine for buttons, relay and MPU
 *
 *  @param      handle PIN_Handle connected to the callback
 *
 *  @param      pinId  PIN_Id of the DIO triggering the callback
 *
 *  @return     none
 ******************************************************************************/
static void EqualizerBand_callback(PIN_Handle handle, PIN_Id pinId)
{

}

/*******************************************************************************
 * @fn      sensorTag_updateAdvertisingData
 *
 * @brief   Update the advertising data with the latest key press status
 *
 * @return  none
 */
void sensorTag_updateAdvertisingData(uint8_t keyStatus)
{
  // Record key state in advertising data
  advertData[KEY_STATE_OFFSET] = keyStatus;
  GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);
}


#ifdef FACTORY_IMAGE
/*******************************************************************************
 * @fn      EqualizerBand_saveFactoryImage
 *
 * @brief   Save the current image to external flash as a factory image
 *
 * @return  none
 */
static bool EqualizerBand_saveFactoryImage(void)
{
  bool success;

  success = extFlashOpen();

  if (success)
  {
    uint32_t address;

    // Erase external flash
    for (address= 0; address<EFL_FLASH_SIZE; address+=EFL_PAGE_SIZE)
    {
      extFlashErase(address,EFL_PAGE_SIZE);
    }

    // Install factory image
    for (address=0; address<EFL_SIZE_RECOVERY && success; address+=EFL_PAGE_SIZE)
    {
      success = extFlashErase(EFL_ADDR_RECOVERY+address, EFL_PAGE_SIZE);
      if (success)
      {
        size_t offset;
        static uint8_t buf[256]; // RAM storage needed due to SPI/DMA limitation

        for (offset=0; offset<EFL_PAGE_SIZE; offset+=sizeof(buf))
        {
          const uint8_t *pIntFlash;

          // Copy from internal to external flash
          pIntFlash = (const uint8_t*)address + offset;
          memcpy(buf,pIntFlash,sizeof(buf));
          success = extFlashWrite(EFL_ADDR_RECOVERY+address+offset,
                                  sizeof(buf), buf);

          // Verify first few bytes
          if (success)
          {
            extFlashRead(EFL_ADDR_RECOVERY+address+offset, sizeof(buf), buf);
            success = buf[2] == pIntFlash[2] && buf[3] == pIntFlash[3];
          }
        }
      }
    }

    extFlashClose();
  }

  return success;
}
#endif

/*******************************************************************************
 * @fn      EqualizerBand_hasFactoryImage
 *
 * @brief   Determine if the EqualizerBand has a pre-programmed factory image
 *          in external flash. Criteria for deciding if a factory image is
 *          present is that the reset vector stored in external flash is valid.
 *
 * @return  none
 */
static bool EqualizerBand_hasFactoryImage(void)
{
  bool valid;

  valid = extFlashOpen();

  if (valid)
  {
    uint16_t buffer[2];

    // 1. Check reset vector
    valid = extFlashRead(EFL_ADDR_RECOVERY,sizeof(buffer),(uint8_t*)buffer);
    if (valid)
    {
      valid = (buffer[0] != 0xFFFF && buffer[1] != 0xFFFF) &&
        (buffer[0] != 0x0000 && buffer[1] != 0x0000);
    }

    extFlashClose();
  }

  return valid;
}

void interrupt_dsp_assert(void)
{
	volatile int delay;
	PINCC26XX_setOutputValue(3, 1);
	for (delay = 0; delay < 0x1000; delay++);
	PINCC26XX_setOutputValue(3, 0);
}

void interrupt_dsp_deassert(void)
{
	PINCC26XX_setOutputValue(3, 0);
}


/*******************************************************************************
*******************************************************************************/
