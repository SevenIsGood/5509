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
 */

//*****************************************************************************
//
//! \addtogroup out_of_box
//! @{
//
//*****************************************************************************

/* standard includes */
#include <stdlib.h>
#include <string.h>

/* driverlib Header files */
#include <ti/devices/cc32xx/inc/hw_types.h>
#include <ti/devices/cc32xx/inc/hw_memmap.h>
#include <ti/devices/cc32xx/driverlib/rom.h>
#include <ti/devices/cc32xx/driverlib/rom_map.h>
#include <ti/devices/cc32xx/driverlib/prcm.h>
#include <ti/devices/cc32xx/driverlib/timer.h>

/* TI-DRIVERS Header files */
#include <Board.h>
#include <uart_term.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/net/wifi/wlan.h>

/* Example/Board Header files */
#include "provisioning_task.h"
#include "platform.h"

/* POSIX Header files */
#include <pthread.h>
#include <time.h>
#include <unistd.h>

/* LCD include files */
#include "lcd.h"
#if USE_PROVISIONING
#define ASYNC_EVT_TIMEOUT               	(5000)  /* In msecs. Used to detect good/bad sl_start() */
#define PROFILE_ASYNC_EVT_TIMEOUT			(5)	/*In sec. Used for connecting to stored profile */
#define LED_TOGGLE_CONFIGURATION_TIMEOUT	(1000)	/* In msecs */
#define LED_TOGGLE_CONFIRMATION_TIMEOUT     (500)   /* In msecs */
#define LED_TOGGLE_CONNECTION_TIMEOUT       (250)   /* In msecs */

#define PROVISIONING_INACTIVITY_TIMEOUT		(600)	/* Provisioning inactivity timeout in seconds */

#define ROLE_SELECTION_BY_SL     			(0xFF)

/*!
 *  \brief  Provisioning modes
 */
typedef enum
{
    PrvsnMode_AP,       /* AP provisioning (AP role) */
    PrvsnMode_SC,       /* Smart Config provisioning (STA role) */
    PrvsnMode_APSC      /* AP + Smart Config provisioning (AP role) */

}PrvsnMode;

/** By default, setting the provisioning mode to AP + Smart Config.
  * Other values could be PrvsnMode_SC or PrvsnMode_AP
  */
#define PROVISIONING_MODE   PrvsnMode_APSC

/*!
 *  \brief  Provisioning status
 */
typedef enum
{
    PrvsnStatus_Stopped,
    PrvsnStatus_InProgress

}PrvsnStatus;

/*
 *  \brief  Application state's context
 */
typedef struct	_Provisioning_AppContext_t_
{
    PrvnState  currentState;       /* Current state of provisioning */
    uint32_t        pendingEvents;      /* Events pending to be processed */

    uint8_t             role;               /* SimpleLink's role - STATION/AP/P2P */

    uint8_t             defaultRole;        /* SimpleLink's default role, try not to change this */
    PrvsnMode     provisioningMode;   /* Provisioning Mode */
    PrvsnStatus   provisioningStatus; /* */

    uint32_t            asyncEvtTimeout;    /* Timeout value*/

    uint32_t        ledToggleTimeout;   /* Timeout value */

}Provisioning_AppContext;

/*!
 *  \brief  Function pointer to the event handler
 */
typedef int32_t (*fptr_EventHandler)(void);

/*!
 *  \brief  Entry in the lookup table
 */
typedef struct
{
    fptr_EventHandler   p_evtHndl;  /* Pointer to the event handler */
    PrvnState          nextState;  /* Next state of provisioning */

}s_TblEntry;

/****************************************************************************
                      LOCAL FUNCTION PROTOTYPES
****************************************************************************/

//*****************************************************************************
//
//! \brief This function initializes provisioning process
//!
//! \param[in]  None
//!
//! \return None
//!
//****************************************************************************
void provisioningInit(void);

//*****************************************************************************
//
//! \brief This function starts provisioning process
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
int32_t provisioningStart(void);

//*****************************************************************************
//
//! \brief main provisioning loop
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t provisioningAppTask(void);

//*****************************************************************************
//! \brief This function puts the device in its default state. It:
//!           - Set the mode to AP
//!           - Configures connection policy to Auto
//!           - Deletes all the stored profiles
//!           - Enables DHCP
//!		  - Disable IPV6
//!           - Disables Scan policy
//!           - Sets Tx power to maximum
//!           - Sets power policy to normal
//!           - Unregister mDNS services
//!           - Remove all filters
//!
//!           IMPORTANT NOTE - This is an example reset function, user must
//!           update this function to match the application settings.
//!
//! \param   none
//! \return  On success, zero is returned. On error, negative is returned
//*****************************************************************************
static int32_t ConfigureSimpleLinkToDefaultState(void);

//*****************************************************************************
//
//! \brief This function starts the SimpleLink in the configured role. 
//!		 The device notifies the host asynchronously when the initialization is completed
//!
//! \param[in]  role	Device shall be configured in this role
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t InitSimplelink(uint8_t const role);

//*****************************************************************************
//
//! \brief This function handles 'APP_EVENT_STARTED' event
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t HandleStrtdEvt(void);

//*****************************************************************************
//
//! \brief This function starts the async-event timer
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t StartAsyncEvtTimer(uint32_t timeout);

 //*****************************************************************************
//
//! \brief This function stops the async-event timer
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t StopAsyncEvtTimer(void);

//*****************************************************************************
//
//! \brief internal error detection during provisioning process
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t ReportError(void);

//*****************************************************************************
//
//! \brief internal report current state during provisioning process
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t ReportSM(void);

//*****************************************************************************
//
//! \brief steps following a successful provisioning process
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t ReportSuccess(void);

//*****************************************************************************
//
//! \brief wait for connection following a successful provisioning process
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t WaitForConn(void);

//*****************************************************************************
//
//! \brief checks for local link connecion to peer device
//!		when set as Station, checks for AP connection
//!		when set as Access Point, no check is required
//!
//! \param[out]  deviceRole		role of the device
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t validateLocalLinkConnection(SlWlanMode_e *deviceRole);

//*****************************************************************************
//
//! Notify if device return to factory image
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void NotifyReturnToFactoryImage(void);
//*****************************************************************************
//
//! \brief This function fetches the device type and prints it
//!
//! \param[in]  None
//!
//! \return device type
//!
//****************************************************************************
uint32_t getDeviceType(void);
/****************************************************************************
                      GLOBAL VARIABLES
****************************************************************************/
/*!
 *  \brief  Application state's context
 */
Provisioning_AppContext gAppCtx;
SpeechRecog_CB SpeechRecog_ControlBlock;

timer_t gAsyncEventTimer;
timer_t gLedTimer;

/*!
 *  \brief   Application lookup/transition table
 */
const s_TblEntry gProvisioningTransitionTable[PrvnState_Max][PrvnEvent_Max] =
{
	/* PrvnState_Init */
	{
		/* Event: PrvnEvent_Triggered */
		{provisioningStart      , PrvnState_Idle			            },
        /* Event: PrvnEvent_Started */
        {ReportError         	, PrvnState_Error    					},
		/* Event: PrvnEvent_StartFailed */
        {ReportError         	, PrvnState_Error				    	},
		/* Event: PrvnEvent_ConfirmationSuccess */
        {ReportSM            , PrvnState_Completed                      },		/* special case where confirmation is received after application is restarted and NWP is still in provisioning */
        																		/* in this case, need to move to COMPLETED state */
		/* Event: PrvnEvent_ConfirmationFailed */
        {ReportError            , PrvnState_Error                      },
		/* Event: PrvnEvent_Stopped */
        {ReportSM            , PrvnState_Init		                },		/* in case of auto provisioning */
		/* Event: PrvnEvent_WaitForConn */
		{ReportSM         			, PrvnState_Error                      	},
        /* Event: PrvnEvent_Timeout */
        {ReportError            , PrvnState_Error		               	},
        /* Event: PrvnEvent_Error */
        {ReportError       		, PrvnState_Error                    	},
    },
    /* PrvnState_Idle */
    {
		/* Event: PrvnEvent_Triggered */
		{ReportError      , PrvnState_Error			            },
        /* Event: PrvnEvent_Started */
        {HandleStrtdEvt         , PrvnState_WaitForConfirmation    	},
		/* Event: PrvnEvent_StartFailed */
        {ReportError         	, PrvnState_Error				    	},
		/* Event: PrvnEvent_ConfirmationSuccess */
        {ReportError            , PrvnState_Error                      },
		/* Event: PrvnEvent_ConfirmationFailed */
        {ReportError            , PrvnState_Error                      },
		/* Event: PrvnEvent_Stopped */
        {ReportSM            	, PrvnState_Init                      	},
		/* Event: PrvnEvent_WaitForConn */
		{ReportSM         			, PrvnState_Error                      	},
        /* Event: PrvnEvent_Timeout */
        {ReportError            , PrvnState_Error		               	},
        /* Event: PrvnEvent_Error */
        {ReportError       		, PrvnState_Error                    	},
    },
	/* PrvnState_WaitForConfirmation */
	{
		/* Event: PrvnEvent_Triggered */
		{ReportError      , PrvnState_Error			            },
		/* Event: PrvnEvent_Started */
		{ReportError         	, PrvnState_Error    					},
		/* Event: PrvnEvent_StartFailed */
		{ReportSM         	, PrvnState_Idle				    	},
		/* Event: PrvnEvent_ConfirmationSuccess */
		{ReportSM            	, PrvnState_Completed                  },
		/* Event: PrvnEvent_ConfirmationFailed */
		{ReportSM            	, PrvnState_WaitForConfirmation     },
		/* Event: PrvnEvent_Stopped */
		{ReportSM            	, PrvnState_Init                       },
		/* Event: PrvnEvent_WaitForConn */
		{ReportSM         			, PrvnState_Error                      	},
		/* Event: PrvnEvent_Timeout */
		{ReportError            , PrvnState_Error		               	},
		/* Event: PrvnEvent_Error */
		{ReportError       		, PrvnState_Error                    	},
	},
	/* PrvnState_Completed */
	{
		/* Event: PrvnEvent_Triggered */
		{ReportError      , PrvnState_Error			            },
		/* Event: PrvnEvent_Started */
		{ReportError         	, PrvnState_Error    					},
		/* Event: PrvnEvent_StartFailed */
		{ReportError         	, PrvnState_Error				    	},
		/* Event: PrvnEvent_ConfirmationSuccess */
		{ReportError            , PrvnState_Error                  	},
		/* Event: PrvnEvent_ConfirmationFailed */
		{ReportError            , PrvnState_Error					    },
		/* Event: PrvnEvent_Stopped */
		{ReportSuccess         	, PrvnState_Init                      	},
		/* Event: PrvnEvent_WaitForConn */
		{WaitForConn         	, PrvnState_Init                      	},	/* this state should cover cases where feedback failed but profile exists */
		/* Event: PrvnEvent_Timeout */
		{ReportError            , PrvnState_Error		               	},
		/* Event: PrvnEvent_Error */
		{ReportError       		, PrvnState_Error                    	},
	},
	/* PrvnState_Error */
	{
		/* Event: PrvnEvent_Triggered */
		{ReportError      , PrvnState_Error			            },
		/* Event: PrvnEvent_Started */
		{ReportError         	  , PrvnState_Error    				},
		/* Event: PrvnEvent_StartFailed */
		{ReportError         	  , PrvnState_Error				   	},
		/* Event: PrvnEvent_ConfirmationSuccess */
		{ReportError              , PrvnState_Error                  	},
		/* Event: PrvnEvent_ConfirmationFailed */
		{ReportError              , PrvnState_Error				    },
		/* Event: PrvnEvent_Stopped */
		{ReportError              , PrvnState_Error                   	},
		/* Event: PrvnEvent_WaitForConn */
		{ReportSM         			, PrvnState_Error                      	},
		/* Event: PrvnEvent_Timeout */
		{ReportError              , PrvnState_Error		           	},
		/* Event: PrvnEvent_Error */
		{ReportError       		  , PrvnState_Error                  	},
	}
};

Provisioning_CB	Provisioning_ControlBlock;

/*****************************************************************************
                  Callback Functions
*****************************************************************************/

//*****************************************************************************
//
//! \brief The device init callback
//!
//! \param[in]  status	Mode the device is configured in
//!
//! \return None
//!
//****************************************************************************
void SimpleLinkInitCallback(uint32_t status, SlDeviceInitInfo_t *DeviceInitInfo)
{
    Provisioning_AppContext *const pCtx = &gAppCtx;

    UART_PRINT("[Provisioning task] Device started in %s role\n\r", (0 == status) ? "Station" :\
                                               ((2 == status) ? "AP" : "P2P"));

    /** While provisioning is ongoing, the appropriate role is choosen by the
      * device itself, and host can remain agnostic to these details
      */
    if(pCtx->role == ROLE_SELECTION_BY_SL)
    {
        SignalProvisioningEvent(PrvnEvent_Started);
    }
    else
    {
        /* Either trigger an error/started event here */
        if(pCtx->role == status)
        {
            SignalProvisioningEvent(PrvnEvent_Started);
        }
        else
        {
            UART_PRINT("[Provisioning task] But the intended role is %s \n\r",\
                                        (0 == pCtx->role) ? "Station" :\
                                        ((2 == pCtx->role) ? "AP" : "P2P"));
            SignalProvisioningEvent(PrvnEvent_Error);
        }
    }
}

//*****************************************************************************
//
//! \brief The interrupt handler for the async-evt timer
//!
//! \param[in]  None
//!
//! \return None
//!
//****************************************************************************
void AsyncEvtTimerIntHandler(sigval val)
{
	/* Clear the timer interrupt. */
	Platform_TimerInterruptClear();

    /* One Shot */
    /* TBD - Check role for One-shot/periodic */
    StopAsyncEvtTimer();
    SignalProvisioningEvent(PrvnEvent_Timeout);
}

//*****************************************************************************
//
//! \brief The interrupt handler for the LED timer
//!
//! \param[in]  None
//!
//! \return None
//!
//****************************************************************************
void LedTimerIntHandler(sigval val)
{
	/* Clear the timer interrupt. */
	Platform_TimerInterruptClear();
#if !ENABLE_C5545_PIN_SETTINGS
	GPIO_toggle(Board_LED0);
#endif
}

//*****************************************************************************
//                 Local Functions
//*****************************************************************************

//*****************************************************************************
//
//! \brief This function initializes provisioning process
//!
//! \param[in]  None
//!
//! \return None
//!
//****************************************************************************
void provisioningInit(void)
{
	Provisioning_AppContext *const pCtx = &gAppCtx;

	Platform_TimerInit(AsyncEvtTimerIntHandler, &gAsyncEventTimer);

    /** By default, setting the provisioning mode to AP + Smart Config.
	  * Other values could be PrvsnMode_SC or PrvsnMode_AP
	  */
	pCtx->provisioningMode = PROVISIONING_MODE;
	switch(pCtx->provisioningMode)
	{
		case PrvsnMode_APSC: pCtx->defaultRole = ROLE_AP;
		break;

		case PrvsnMode_AP: pCtx->defaultRole = ROLE_AP;
		break;

		case PrvsnMode_SC: pCtx->defaultRole = ROLE_STA;
		break;
	}

	/* Provisioning has not started yet */
	pCtx->provisioningStatus = PrvsnStatus_Stopped;
	pCtx->currentState = PrvnState_Init;

}

//*****************************************************************************
//
//! \brief This function starts provisioning process
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
int32_t provisioningStart(void)
{
	int32_t		retVal = 0;
	Provisioning_AppContext *const pCtx = &gAppCtx;
	SlDeviceVersion_t ver = {0};
	uint8_t  configOpt = 0;
	uint16_t configLen = 0;

	/* check if provisioning is running */
	/* if auto provisioning - the command stops it automatically */
	/* in case host triggered provisioning - need to stop it explicitly */
	configOpt = SL_DEVICE_GENERAL_VERSION;
	configLen = sizeof(ver);
	retVal = sl_DeviceGet(SL_DEVICE_GENERAL, &configOpt, &configLen, (uint8_t *)(&ver));
	if(SL_RET_CODE_PROVISIONING_IN_PROGRESS == retVal)
	{
		UART_PRINT("[Provisioning task] Provisioning is already running, stopping it...\r\n");
		retVal = sl_WlanProvisioning(SL_WLAN_PROVISIONING_CMD_STOP,ROLE_STA,0,NULL,0);

		/* return  SL_RET_CODE_PROVISIONING_IN_PROGRESS to indicate the SM to stay in the same state*/
		return SL_RET_CODE_PROVISIONING_IN_PROGRESS;
	}

    /*
        IMPORTANT NOTE - This is an example reset function, user must update
                         this function to match the application settings.
    */    
    retVal = ConfigureSimpleLinkToDefaultState();

    if(retVal < 0)
    {
        UART_PRINT("[Provisioning task] Failed to configure the device in its default state \n\r");
        return retVal;
    }

    UART_PRINT("[Provisioning task] Device is configured in default state \n\r");

	/* Provisioning has not started yet */
	pCtx->provisioningStatus = PrvsnStatus_Stopped;

	/* Set the LED toggling timeout before starting the timer */
	pCtx->ledToggleTimeout = LED_TOGGLE_CONFIGURATION_TIMEOUT;
	StartLedEvtTimer(pCtx->ledToggleTimeout);

	retVal = InitSimplelink(pCtx->defaultRole);
	if(retVal < 0)
	{
		UART_PRINT("[Provisioning task] Failed to initialize the device\n\r");
		return retVal;
	}

	return retVal;
}

//*****************************************************************************
//
//! \brief main provisioning loop
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t provisioningAppTask(void)
{
    Provisioning_AppContext *const pCtx = &gAppCtx;
    s_TblEntry   *pEntry = NULL;
    uint16_t eventIdx = 0;
    int32_t	retVal = 0;

    for(eventIdx = 0; eventIdx < PrvnEvent_Max; eventIdx++)
    {
        if(0 != (pCtx->pendingEvents & (1 << eventIdx)))
        {
            if (eventIdx != PrvnEvent_Triggered)
            {
                /** Events received - Stop the respective timer if its still
                  * running
                  */
                StopAsyncEvtTimer();

            }

            pEntry = (s_TblEntry *)&gProvisioningTransitionTable[pCtx->currentState][eventIdx];
            if (NULL != pEntry->p_evtHndl)
            {
                retVal = pEntry->p_evtHndl();
                if (retVal == SL_RET_CODE_PROVISIONING_IN_PROGRESS)		/* no state transition is required */
                {
                	pCtx->pendingEvents &= ~(1 << eventIdx);
                	continue;
                }
                else if (retVal < 0)
                {
                    UART_PRINT("[Provisioning task] Event handler failed, error=%d\n\r", retVal);
                    while (1)		/* this is to let other tasks recover by mcu reset, e.g. in case of switching to AP mode */
                    	{
                    		usleep(1000);
                    	}
                }
            }

            if (pEntry->nextState != pCtx->currentState)
            {
                pCtx->currentState = pEntry->nextState;
            }

            pCtx->pendingEvents &= ~(1 << eventIdx);
        }

        /* No more events to handle. Break.! */
        if(0 == pCtx->pendingEvents) 
	{
		break;
        }
    }

    usleep(1000);

    return 0;
}



//*****************************************************************************
//! \brief This function puts the device in its default state. It:
//!           - Set the mode to AP
//!           - Configures connection policy to Auto
//!           - Deletes all the stored profiles
//!           - Enables DHCP
//!		  - Disable IPV6
//!           - Disables Scan policy
//!           - Sets Tx power to maximum
//!           - Sets power policy to normal
//!           - Unregister mDNS services
//!           - Remove all filters
//!
//!           IMPORTANT NOTE - This is an example reset function, user must
//!           update this function to match the application settings.
//!
//! \param   none
//! \return  On success, zero is returned. On error, negative is returned
//*****************************************************************************
static int32_t ConfigureSimpleLinkToDefaultState(void)
{
    SlWlanRxFilterOperationCommandBuff_t  RxFilterIdMask;

    uint8_t 	ucConfigOpt = 0;
    uint16_t 	ifBitmap  = 0;
    uint8_t 	ucPower = 0;

    int32_t 			ret = -1;
    int32_t 			mode = -1;

    memset(&RxFilterIdMask,0,sizeof(SlWlanRxFilterOperationCommandBuff_t));

    /* Start Simplelink - Blocking mode */
    mode = sl_Start(0, 0, 0);
    if (SL_RET_CODE_DEV_ALREADY_STARTED != mode)
	{
			ASSERT_ON_ERROR(mode);
	}

    /* If the device is not in AP mode, try configuring it in AP mode 
     in case device is already started (got SL_RET_CODE_DEV_ALREADY_STARTED error code), then mode would remain -1 and in this case we do not know the role. Move to AP role anyway	*/
    if (ROLE_AP != mode)
    {

        /* Switch to AP role and restart */
        ret = sl_WlanSetMode(ROLE_AP);
        ASSERT_ON_ERROR(ret);

        ret = sl_Stop(SL_STOP_TIMEOUT );
        ASSERT_ON_ERROR(ret);

        ret = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(ret);

        /* Check if the device is in AP again */
        if (ROLE_AP != ret)
        {
            return ret;
        }
    }
    
    /* Set connection policy to Auto (no AutoProvisioning)  */
    ret = sl_WlanPolicySet(SL_WLAN_POLICY_CONNECTION,
                                SL_WLAN_CONNECTION_POLICY(1, 0, 0, 0), NULL, 0);
    ASSERT_ON_ERROR(ret);

    /* Remove all profiles */
    ret = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(ret);

    /* Enable DHCP client */
    ret = sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE,SL_NETCFG_ADDR_DHCP,0,0);
    ASSERT_ON_ERROR(ret);

    /* Disable IPV6 */
    ifBitmap = 0;
	ret = sl_NetCfgSet(SL_NETCFG_IF, SL_NETCFG_IF_STATE, sizeof(ifBitmap), (uint8_t *)&ifBitmap);
	ASSERT_ON_ERROR(ret);


    /* Disable scan */
    ucConfigOpt = SL_WLAN_SCAN_POLICY(0, 0);
    ret = sl_WlanPolicySet(SL_WLAN_POLICY_SCAN , ucConfigOpt, NULL, 0);
    ASSERT_ON_ERROR(ret);

    /* Set Tx power level for station mode
     Number between 0-15, as dB offset from max power - 0 will set max power */
    ucPower = 0;
    ret = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID,
    					SL_WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (uint8_t *)&ucPower);
    ASSERT_ON_ERROR(ret);

    /* Set PM policy to normal */
    ret = sl_WlanPolicySet(SL_WLAN_POLICY_PM , SL_WLAN_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(ret);

    /* Unregister mDNS services */
    ret = sl_NetAppMDNSUnRegisterService(0, 0, 0);
    ASSERT_ON_ERROR(ret);

    /* Remove  all 64 filters (8*8) */
    memset(RxFilterIdMask.FilterBitmap, 0xFF, 8);
    ret = sl_WlanSet(SL_WLAN_RX_FILTERS_ID,
    		   SL_WLAN_RX_FILTER_REMOVE,
        	   sizeof(SlWlanRxFilterOperationCommandBuff_t),
    		   (uint8_t *)&RxFilterIdMask);
    ASSERT_ON_ERROR(ret);

    ret = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(ret);
    
    return ret;
}


//*****************************************************************************
//
//! \brief This function starts the SimpleLink in the configured role. 
//!		 The device notifies the host asynchronously when the initialization is completed
//!
//! \param[in]  role	Device shall be configured in this role
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t InitSimplelink(uint8_t const role)
{
    Provisioning_AppContext *const pCtx = &gAppCtx;
    int32_t retVal = -1;

    pCtx->role = role;
    pCtx->pendingEvents = 0;

    retVal = sl_Start(0, 0, (P_INIT_CALLBACK)SimpleLinkInitCallback);
    ASSERT_ON_ERROR(retVal);

    /* Start timer */
    pCtx->asyncEvtTimeout = ASYNC_EVT_TIMEOUT;
    retVal = StartAsyncEvtTimer(pCtx->asyncEvtTimeout);
    ASSERT_ON_ERROR(retVal);

    return retVal;
}


//*****************************************************************************
//
//! \brief This function handles 'APP_EVENT_STARTED' event
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************

static int32_t HandleStrtdEvt(void)
{
    Provisioning_AppContext *const pCtx = &gAppCtx;
    int32_t retVal     = 0;

    /** If provisioning has already started, don't do anything here
      * The state-machine shall keep waiting for the provisioning status
      */
    if(PrvsnStatus_Stopped == pCtx->provisioningStatus)
    {
        SlDeviceVersion_t   firmwareVersion = {0};

        uint8_t  ucConfigOpt = 0;
        uint16_t ucConfigLen = 0;

		/* Get the device's version-information */
		ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
		ucConfigLen = sizeof(firmwareVersion);
		retVal = sl_DeviceGet(SL_DEVICE_GENERAL, &ucConfigOpt,\
						   &ucConfigLen, (unsigned char *)(&firmwareVersion));
		ASSERT_ON_ERROR(retVal);

        UART_PRINT("[Provisioning task] Host Driver Version: %s\n\r", SL_DRIVER_VERSION);
        UART_PRINT("[Provisioning task] Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d\n\r",\
                        firmwareVersion.NwpVersion[0],  \
                        firmwareVersion.NwpVersion[1],  \
                        firmwareVersion.NwpVersion[2],  \
                        firmwareVersion.NwpVersion[3],  \
                        firmwareVersion.FwVersion[0],   \
                        firmwareVersion.FwVersion[1],   \
                        firmwareVersion.FwVersion[2],   \
                        firmwareVersion.FwVersion[3],   \
                        firmwareVersion.PhyVersion[0],  \
                        firmwareVersion.PhyVersion[1],  \
                        firmwareVersion.PhyVersion[2],  \
                        firmwareVersion.PhyVersion[3]);

        /* Start provisioning process */
        UART_PRINT("[Provisioning task] Starting Provisioning - ");
        UART_PRINT("[Provisioning task] in mode %d (0 = AP, 1 = SC, 2 = AP+SC)\r\n",
                                                    pCtx->provisioningMode);

        retVal = sl_WlanProvisioning(pCtx->provisioningMode, ROLE_STA,
                                     PROVISIONING_INACTIVITY_TIMEOUT, NULL,0);
        ASSERT_ON_ERROR(retVal);

        pCtx->provisioningStatus = PrvsnStatus_InProgress;
        UART_PRINT("[Provisioning task] Provisioning Started. Waiting to be provisioned..!! \r\n");
        LCD_showMessageLine("Please provision device", 45);
    }

    return retVal;
}


//*****************************************************************************
//
//! \brief This function starts the async-event timer
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t StartAsyncEvtTimer(uint32_t timeout)
{
    Provisioning_AppContext *const pCtx = &gAppCtx;

    pCtx->asyncEvtTimeout = timeout;
    Platform_TimerStart(pCtx->asyncEvtTimeout, gAsyncEventTimer, 0);
    return 0;
}

//*****************************************************************************
//
//! \brief This function stops the async-event timer
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t StopAsyncEvtTimer(void)
{
    Provisioning_AppContext *const pCtx = &gAppCtx;

    if(0 != pCtx->asyncEvtTimeout)
    {
        Platform_TimerStop(gAsyncEventTimer);
        pCtx->asyncEvtTimeout = 0;
    }

    return 0;
}


//*****************************************************************************
//
//! \brief internal error detection during provisioning process
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t ReportError(void)
{
    Provisioning_AppContext *const pCtx = &gAppCtx;
    uint16_t eventIdx = 0;

    for(eventIdx = 0; eventIdx < PrvnEvent_Max; eventIdx++)
    {
        if(0 != (pCtx->pendingEvents & (1 << eventIdx))) 
	{
		break;
        }
    }

    UART_PRINT("[Provisioning task] Unexpected SM: State = %d, Event = %d\n\r",\
                                    pCtx->currentState, eventIdx);
    return -1; 
}

//*****************************************************************************
//
//! \brief internal report current state during provisioning process
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t ReportSM(void)
{
    Provisioning_AppContext *const pCtx = &gAppCtx;
    uint16_t eventIdx = 0;

    for(eventIdx = 0; eventIdx < PrvnEvent_Max; eventIdx++)
    {
        if(0 != (pCtx->pendingEvents & (1 << eventIdx))) 
	{
		break;
        }
    }

    if (PrvnEvent_Stopped == eventIdx)
    {
    	StopLedEvtTimer();
#if !ENABLE_C5545_PIN_SETTINGS
    	GPIO_write(Board_LED0, Board_LED_OFF);
#endif
    }

    return 0;
}

//*****************************************************************************
//
//! \brief steps following a successful provisioning process
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t ReportSuccess(void)
{
    Provisioning_AppContext *const pCtx = &gAppCtx;
    uint16_t ConfigOpt;
	uint16_t ipLen;
	SlNetCfgIpV4Args_t ipV4 = {0};
    int32_t retVal;

    UART_PRINT("[Provisioning task] Provisioning completed successfully..!\n\r");
    LCD_showMessageLine("Device Provisioning Successful", 45);
    pCtx->provisioningStatus = PrvsnStatus_Stopped;
    StopLedEvtTimer();

    /* Get the device's IP address */
	ipLen = sizeof(SlNetCfgIpV4Args_t);
	ConfigOpt = 0;
	retVal = sl_NetCfgGet(SL_NETCFG_IPV4_STA_ADDR_MODE,&ConfigOpt,&ipLen,(uint8_t *)&ipV4);
	if (retVal == 0)
	{
		UART_PRINT("[Provisioning task] IP address is %d.%d.%d.%d\n\r",\
					SL_IPV4_BYTE(ipV4.Ip,3),  \
					SL_IPV4_BYTE(ipV4.Ip,2),  \
					SL_IPV4_BYTE(ipV4.Ip,1),  \
					SL_IPV4_BYTE(ipV4.Ip,0));
	}
#if !ENABLE_C5545_PIN_SETTINGS
	GPIO_write(Board_LED0, Board_LED_ON);
#endif

	/* signal to linklocal task */
	sem_post(&Provisioning_ControlBlock.provisioningDoneSignal);

	/* signal to report server task */
	sem_post(&Provisioning_ControlBlock.provisioningConnDoneToOtaServerSignal);

    return 0;
}


//*****************************************************************************
//
//! \brief wait for connection following a successful provisioning process
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t WaitForConn(void)
{
    Provisioning_AppContext *const pCtx = &gAppCtx;
    struct timespec ts;
    int32_t retVal;

    while (((!IS_IPV6L_ACQUIRED(SpeechRecog_ControlBlock.status) ||
			!IS_IPV6G_ACQUIRED(SpeechRecog_ControlBlock.status)) &&
			!IS_IP_ACQUIRED(SpeechRecog_ControlBlock.status)) ||
			!IS_CONNECTED(SpeechRecog_ControlBlock.status))
	{
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += PROFILE_ASYNC_EVT_TIMEOUT;

		retVal = sem_timedwait(&Provisioning_ControlBlock.connectionAsyncEvent, &ts);
		if((retVal == 116) || (retVal == -1))	/* freertos return -1 in case of timeout */
		{
			UART_PRINT("[Provisioning task] Cannot connect to AP or profile does not exist\n\r");
#if !ENABLE_C5545_PIN_SETTINGS
			GPIO_write(Board_LED0, Board_LED_OFF);
#endif
			pCtx->currentState = PrvnState_Init;	/* this state is set so that PrvnEvent_Triggered would invoke provisioning again */
			SignalProvisioningEvent(PrvnEvent_Triggered);

			return 0;
		}
	}

	UART_PRINT("[Provisioning task] Connection to AP succeeded\n\r");

	return ReportSuccess();
}


//*****************************************************************************
//
//! \brief checks for local link connecion to peer device
//!		when set as Station, checks for AP connection
//!		when set as Access Point, no check is required
//!
//! \param[out]  deviceRole		role of the device
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
static int32_t validateLocalLinkConnection(SlWlanMode_e *deviceRole)
{
	uint16_t ConfigOpt;
	uint16_t ipLen;
	SlNetCfgIpV4Args_t ipV4 = {0};
	int32_t retVal;
	uint32_t ocpRegVal;

	struct timespec ts;

	retVal = sl_Start(0, 0, 0);
	ASSERT_ON_ERROR(retVal);

	/* if in AP role
	 * 1) check OCP register value
	 * 2) if set, it means user set AP mode via switch.
	 *  	 check for IP_ACQUIRED to indicate NWP is running
	 * 4) if not set, procede with STATION role
	 */
	if (retVal == ROLE_AP)
	{
		*deviceRole = ROLE_AP;
		ocpRegVal = MAP_PRCMOCRRegisterRead(OCP_REGISTER_INDEX);
		ocpRegVal &= (1<<OCP_REGISTER_OFFSET);
		if (ocpRegVal)
		{
			if (IS_IP_ACQUIRED(SpeechRecog_ControlBlock.status))
			{
				return 0;
			}
			else
			{
				clock_gettime(CLOCK_REALTIME, &ts);
       			ts.tv_sec += PROFILE_ASYNC_EVT_TIMEOUT;
				
				retVal = sem_timedwait(&Provisioning_ControlBlock.connectionAsyncEvent, &ts);
				if((retVal == 116) || (retVal == -1))	/* freertos return -1 in case of timeout */
				{
					UART_PRINT("[Provisioning task] AP role failed to initialize\n\r");
#if !ENABLE_C5545_PIN_SETTINGS
					GPIO_write(Board_LED0, Board_LED_OFF);
#endif
					retVal = -1;
				}
				return retVal;
			}
		}
	}

	if(retVal != ROLE_STA)
	{
		retVal = sl_WlanSetMode(ROLE_STA);
		ASSERT_ON_ERROR(retVal);

		retVal = sl_Stop(SL_STOP_TIMEOUT );
		ASSERT_ON_ERROR(retVal);

		retVal = sl_Start(0, 0, 0);
		if (retVal < 0 || (retVal != ROLE_STA))
		{
			ASSERT_ON_ERROR(retVal);
		}
		UART_PRINT("[Provisioning task] Device started as STATION \n\r");
	}

	*deviceRole = ROLE_STA;

	while (((!IS_IPV6L_ACQUIRED(SpeechRecog_ControlBlock.status) ||
			!IS_IPV6G_ACQUIRED(SpeechRecog_ControlBlock.status)) &&
			!IS_IP_ACQUIRED(SpeechRecog_ControlBlock.status)) ||
			!IS_CONNECTED(SpeechRecog_ControlBlock.status))
	{
		clock_gettime(CLOCK_REALTIME, &ts);
       	ts.tv_sec += PROFILE_ASYNC_EVT_TIMEOUT;
				
		retVal = sem_timedwait(&Provisioning_ControlBlock.connectionAsyncEvent, &ts);
		if((retVal == 116) || (retVal == -1))	/* freertos return -1 in case of timeout */
		{
			UART_PRINT("[Provisioning task] Cannot connect to AP or profile does not exist\n\r");
#if !ENABLE_C5545_PIN_SETTINGS
			GPIO_write(Board_LED0, Board_LED_OFF);
#endif
			retVal = -1;

			return retVal;
		}
	}

	UART_PRINT("[Provisioning task] Connection to AP succeeded\n\r");

	/* Get the device's IP address */
	ipLen = sizeof(SlNetCfgIpV4Args_t);
	ConfigOpt = 0;
	sl_NetCfgGet(SL_NETCFG_IPV4_STA_ADDR_MODE,&ConfigOpt,&ipLen,(uint8_t *)&ipV4);
	if (retVal < 0)
	{
		return retVal;
	}

	UART_PRINT("[Provisioning task] IP address is %d.%d.%d.%d\n\r",\
			SL_IPV4_BYTE(ipV4.Ip,3),  \
			SL_IPV4_BYTE(ipV4.Ip,2),  \
			SL_IPV4_BYTE(ipV4.Ip,1),  \
			SL_IPV4_BYTE(ipV4.Ip,0));

    return 0;
}

//*****************************************************************************
//
//! Notify if device return to factory image
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void NotifyReturnToFactoryImage(void)
{
    if(((HWREG(HIB3P3_BASE + 0x00000418) & (1<<7)) !=0) &&
        ((HWREG(0x4402F0C8) &0x01) != 0))
    {
        UART_PRINT("Return To Factory Image successful, Do a power cycle(POR)"
                    " of the device using switch SW1-Reset\n\r");
        while(1);
    }
}


//****************************************************************************
//                            MAIN FUNCTION
//****************************************************************************

//*****************************************************************************
//
//! \brief This function signals the application events
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
int16_t SignalProvisioningEvent(PrvnEvent event)
{
    Provisioning_AppContext *const pCtx = &gAppCtx;
    pCtx->pendingEvents |= (1 << event);

    return 0;
}

//*****************************************************************************
//
//! \brief This function gets the current provisioning state
//!
//! \param[in]  None
//!
//! \return provisioning state
//!
//****************************************************************************
PrvnState GetProvisioningState()
{
	Provisioning_AppContext *const pCtx = &gAppCtx;

	return pCtx->currentState;
}


//*****************************************************************************
//
//! \brief This function starts the led toggling timer
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
int32_t StartLedEvtTimer(uint32_t timeout)
{
    Provisioning_AppContext *const pCtx = &gAppCtx;

    pCtx->ledToggleTimeout = timeout;
	Platform_TimerStart(pCtx->ledToggleTimeout, gLedTimer, 1);
	
    return 0;
}

//*****************************************************************************
//
//! \brief This function stops the led toggling timer
//!
//! \param[in]  None
//!
//! \return 0 on success, negative value otherwise
//!
//****************************************************************************
int32_t StopLedEvtTimer(void)
{
    Provisioning_AppContext *const pCtx = &gAppCtx;

	if(0 != pCtx->ledToggleTimeout)
    {
        Platform_TimerStop(gLedTimer);
        pCtx->ledToggleTimeout = 0;
    }

    return 0;
}


//*****************************************************************************
//
//! \brief This function stops provisioning process
//!
//! \param[in]  None
//!
//! \return SL_RET_CODE_PROVISIONING_IN_PROGRESS if provisioning was running, otherwise 0
//!
//****************************************************************************
int32_t provisioningStop(void)
{
	Provisioning_AppContext *const pCtx = &gAppCtx;
	int32_t						retVal;
	PrvnState 				provisioningState;
	SlDeviceVersion_t ver = {0};
	uint8_t  configOpt = 0;
	uint16_t configLen = 0;
	
	/* check if provisioning is running */
	configOpt = SL_DEVICE_GENERAL_VERSION;
	configLen = sizeof(ver);
	retVal = sl_DeviceGet(SL_DEVICE_GENERAL, &configOpt, &configLen, (uint8_t *)(&ver));
	if(SL_RET_CODE_PROVISIONING_IN_PROGRESS == retVal)
	{
		UART_PRINT("[Provisioning task]  Provisioning is already running, stopping it...\r\n");
		retVal = sl_WlanProvisioning(SL_WLAN_PROVISIONING_CMD_STOP,ROLE_STA,0,NULL,0);

		/* wait for the stopped event to arrive - wait for PrvnState_Init */
		do
		{
			provisioningState = GetProvisioningState();
			usleep(1000);
		} while (provisioningState != PrvnState_Init);

		pCtx->provisioningStatus = PrvsnStatus_Stopped;

		retVal = SL_RET_CODE_PROVISIONING_IN_PROGRESS;
	}
	else if (retVal < 0)
	{
		return retVal;
	}
	else
	{
		retVal = 0;
	}

	return retVal;
}



//*****************************************************************************
//
//! \brief This is the main provisioning task
//!
//! \param[in]  None
//!
//! \return None
//!
//****************************************************************************
void * provisioningTask(void *pvParameters)
{
	int32_t			retVal = -1;
	Provisioning_AppContext 	*const pCtx = &gAppCtx;
	SlWlanMode_e 	deviceRole;
	//SlFsControl_t 	FsControl;
    //int32_t 			status;
	
	/* Check the wakeup source. If first time entry or wakeup from HIB */
	if(MAP_PRCMSysResetCauseGet() == 0)
	{
		UART_PRINT("[Provisioning task] Wake up on Power ON\n\r");
	}
	else if(MAP_PRCMSysResetCauseGet() == PRCM_HIB_EXIT)
	{
		UART_PRINT("[Provisioning task] Woken up from Hibernate\n\r");
	}

	getDeviceType();

    /*
     	Following function first try to connect to a stored profile.
	 If fails, it first configure the device to default state by cleaning
     	the persistent settings stored in NVMEM (viz. connection profiles &
     	policies, power policy etc)
    	
     	Applications may choose to skip this step if the developer is sure
     	that the device is in its desired state at start of applicaton
    	
     	Note that all profiles and persistent settings that were done on the
     	device will be lost	*/
    
    
	/* initialize one-time parameters for provisioning */
	provisioningInit();

    /* Configure Provisioning Toggle LED  */
#if !ENABLE_C5545_PIN_SETTINGS
	GPIO_write(Board_LED0, Board_LED_OFF);
#endif

	Platform_TimerInit(LedTimerIntHandler, &gLedTimer);

	/* Set the LED toggling timeout before starting the timer */
	pCtx->ledToggleTimeout = LED_TOGGLE_CONFIRMATION_TIMEOUT;

	StartLedEvtTimer(pCtx->ledToggleTimeout);
	
	/* check whether return-to-default occured */
	NotifyReturnToFactoryImage();

	/* 1) try to connect to a stored profile if in STATION mode
	 * 2) if successful, procede and validate ota bundle and then try to connect to cloud
	 * 3) if unsuccessful when in STATION mode, procede to provisioning
	 * 4) if in AP mode, procede and validate ota bundle
	 */
	retVal = validateLocalLinkConnection(&deviceRole);

	/* at this point, provisioning has not started yet, unless auto provisioning is running */
	/* in this case, if provisioning from mobile app is running, it would not be possible to send most of the commands to the device */
	/* need to stop provisioning */
	provisioningStop();

	StopLedEvtTimer();

	/* 1) in case coming from ota procedure, a profile should reside on serial flash and connect immediately
	 * 2) check if pending commit and set commit the bundle
	 * 3) On failure, reset the MCU to rollback */
	 
//	if (OtaArchive_GetPendingCommit())	/* it means OtaArchive is in SL_FS_BUNDLE_STATE_PENDING_COMMIT */
//	{
//		if (retVal != 0)	/* validation of new ota bundle failed, reverting to previous bundle */
//		{
//			/* rollback is not required for production devices - but it is not harmfull */
//			FsControl.IncludeFilters = 0;
//		    	status = sl_FsCtl(SL_FS_CTL_BUNDLE_ROLLBACK, 0, NULL ,(uint8_t *)&FsControl, sizeof(SlFsControl_t), NULL, 0 , NULL);
//		    	if( status < 0 )
//		    	{
//		        	UART_PRINT("[Provisioning task] Rollback error sl_FsCtl, status=%d\n\r", status);
//		    	}
//
//			mcuReboot();
//		}
//		else	/* validation of new ota bundle succeeded, commit the new ota bundle */
//		{
//			UART_PRINT("[Provisioning task] committing new ota download... \n\r");
//			if (OtaArchive_Commit() < 0)
//			{
//				UART_PRINT("[Provisioning task] failed to commit new download, reverting to previous copy by reseting the device \n\r");
//				mcuReboot();
//			}
//			UART_PRINT("[Provisioning task] commit succeeded \n\r");
//
//			/* need to stop the WDT so MCU is not reset */
//			PRCMPeripheralReset(PRCM_WDT);
//		}
//	}

	if (deviceRole == ROLE_STA)		
	{
		/* it means a connection to AP has been established, no need to trigger provisioning */
		if (retVal == 0)
		{
#if !ENABLE_C5545_PIN_SETTINGS
			GPIO_write(Board_LED0, Board_LED_ON);
#endif

			/* signal to linklocal task */
			sem_post(&Provisioning_ControlBlock.provisioningDoneSignal);

			/* signal to report server task */
			sem_post(&Provisioning_ControlBlock.provisioningConnDoneToOtaServerSignal);
		}
		/* it means a connection to AP failed, trigger provisioning */
		else if (retVal < 0)
		{
			SignalProvisioningEvent(PrvnEvent_Triggered);
		}
	}

	if ((retVal == 0) && (deviceRole == ROLE_AP))		/* it means device is initialized as AP, no need to trigger provisioning */
	{
#if !ENABLE_C5545_PIN_SETTINGS
	    GPIO_write(Board_LED0, Board_LED_ON);
#endif
		
		/* signal to linklocal task */
		sem_post(&Provisioning_ControlBlock.provisioningDoneSignal);

		/* signal to report server task */
		sem_post(&Provisioning_ControlBlock.provisioningConnDoneToOtaServerSignal);
	}

    do
	{
        retVal = provisioningAppTask();

    }while(!retVal); /* Exit on failure */

	return(0);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
//*****************************************************************************
//
//! The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    switch(pWlanEvent->Id)
    {
        case SL_WLAN_EVENT_CONNECT:
        {
            SET_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_Connection);
            CLR_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_IpAcquired);
            CLR_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_Ipv6lAcquired);
            CLR_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_Ipv6gAcquired);

            /*
             Information about the connected AP (like name, MAC etc) will be
             available in 'slWlanConnectAsyncResponse_t'-Applications
             can use it if required:

              slWlanConnectAsyncResponse_t *pEventData = NULL;
              pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
            */

            /* Copy new connection SSID and BSSID to global parameters */
            memcpy(SpeechRecog_ControlBlock.connectionSSID,
                    pWlanEvent->Data.Connect.SsidName,
                    pWlanEvent->Data.Connect.SsidLen);

            SpeechRecog_ControlBlock.ssidLen = pWlanEvent->Data.Connect.SsidLen;

            memcpy(SpeechRecog_ControlBlock.connectionBSSID,
                    pWlanEvent->Data.Connect.Bssid,
                    SL_WLAN_BSSID_LENGTH);

            UART_PRINT("[WLAN EVENT] STA Connected to the AP: %s ,"
                        "BSSID: %x:%x:%x:%x:%x:%x\n\r",
                            SpeechRecog_ControlBlock.connectionSSID,
                            SpeechRecog_ControlBlock.connectionBSSID[0], SpeechRecog_ControlBlock.connectionBSSID[1],
                            SpeechRecog_ControlBlock.connectionBSSID[2], SpeechRecog_ControlBlock.connectionBSSID[3],
                            SpeechRecog_ControlBlock.connectionBSSID[4], SpeechRecog_ControlBlock.connectionBSSID[5]);

            sem_post(&Provisioning_ControlBlock.connectionAsyncEvent);
        }
        break;

        case SL_WLAN_EVENT_DISCONNECT:
        {
            SlWlanEventDisconnect_t*    pEventData = NULL;

            CLR_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_Connection);
            CLR_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_IpAcquired);
            CLR_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_Ipv6lAcquired);
            CLR_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_Ipv6gAcquired);

            pEventData = &pWlanEvent->Data.Disconnect;

            /*  If the user has initiated 'Disconnect' request,
                'reason_code' is SL_WLAN_DISCONNECT_USER_INITIATED.
             */
            if(SL_WLAN_DISCONNECT_USER_INITIATED == pEventData->ReasonCode)
            {
                UART_PRINT("[WLAN EVENT]Device disconnected from the "
                            "AP: %s, BSSID: %x:%x:%x:%x:%x:%x "
                            "on application's request \n\r",
                            SpeechRecog_ControlBlock.connectionSSID,
                            SpeechRecog_ControlBlock.connectionBSSID[0], SpeechRecog_ControlBlock.connectionBSSID[1],
                            SpeechRecog_ControlBlock.connectionBSSID[2], SpeechRecog_ControlBlock.connectionBSSID[3],
                            SpeechRecog_ControlBlock.connectionBSSID[4], SpeechRecog_ControlBlock.connectionBSSID[5]);
            }
            else
            {
                UART_PRINT("[WLAN ERROR]Device disconnected from the AP AP: %s,"
                            "BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r",
                            SpeechRecog_ControlBlock.connectionSSID,
                            SpeechRecog_ControlBlock.connectionBSSID[0], SpeechRecog_ControlBlock.connectionBSSID[1],
                            SpeechRecog_ControlBlock.connectionBSSID[2], SpeechRecog_ControlBlock.connectionBSSID[3],
                            SpeechRecog_ControlBlock.connectionBSSID[4], SpeechRecog_ControlBlock.connectionBSSID[5]);
            }
            memset(SpeechRecog_ControlBlock.connectionSSID, 0, sizeof(SpeechRecog_ControlBlock.connectionSSID));
            memset(SpeechRecog_ControlBlock.connectionBSSID, 0, sizeof(SpeechRecog_ControlBlock.connectionBSSID));
        }
        break;

     case SL_WLAN_EVENT_STA_ADDED:
        {
            UART_PRINT("[WLAN EVENT] External Station connected to SimpleLink AP\r\n");

            UART_PRINT("[WLAN EVENT] STA BSSID: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                                   pWlanEvent->Data.STAAdded.Mac[0],
                                   pWlanEvent->Data.STAAdded.Mac[1],
                                   pWlanEvent->Data.STAAdded.Mac[2],
                                   pWlanEvent->Data.STAAdded.Mac[3],
                                   pWlanEvent->Data.STAAdded.Mac[4],
                                   pWlanEvent->Data.STAAdded.Mac[5]);

        }
        break;

        case SL_WLAN_EVENT_STA_REMOVED:
        {
            UART_PRINT("[WLAN EVENT] External Station disconnected from SimpleLink AP\r\n");
        }
        break;

        case SL_WLAN_EVENT_PROVISIONING_PROFILE_ADDED:
        {
            UART_PRINT("[WLAN EVENT] Profile Added\r\n");
        }
        break;

        case SL_WLAN_EVENT_PROVISIONING_STATUS:
        {
            uint16_t status = pWlanEvent->Data.ProvisioningStatus.ProvisioningStatus;
            switch(status)
            {
                case SL_WLAN_PROVISIONING_GENERAL_ERROR:
                case SL_WLAN_PROVISIONING_ERROR_ABORT:
                {
                    UART_PRINT("[WLAN EVENT] Provisioning Error status=%d\r\n",status);
                    SignalProvisioningEvent(PrvnEvent_Error);
                }
                break;
                case SL_WLAN_PROVISIONING_ERROR_ABORT_INVALID_PARAM:
                case SL_WLAN_PROVISIONING_ERROR_ABORT_HTTP_SERVER_DISABLED:
                case SL_WLAN_PROVISIONING_ERROR_ABORT_PROFILE_LIST_FULL:
                {
                    UART_PRINT("[WLAN EVENT] Provisioning Error status=%d\r\n",status);
                    SignalProvisioningEvent(PrvnEvent_StartFailed);
                }
                break;
                case SL_WLAN_PROVISIONING_ERROR_ABORT_PROVISIONING_ALREADY_STARTED:
                {
                    UART_PRINT("[WLAN EVENT] Provisioning already started");
                }
                break;

                case SL_WLAN_PROVISIONING_CONFIRMATION_STATUS_FAIL_NETWORK_NOT_FOUND:
                {
                    UART_PRINT("[WLAN EVENT] Confirmation fail: network not found\r\n");
                    SignalProvisioningEvent(PrvnEvent_ConfirmationFailed);
                }
                break;

                case SL_WLAN_PROVISIONING_CONFIRMATION_STATUS_FAIL_CONNECTION_FAILED:
                {
                    UART_PRINT("[WLAN EVENT] Confirmation fail: Connection failed\r\n");
                    SignalProvisioningEvent(PrvnEvent_ConfirmationFailed);
                }
                break;

                case SL_WLAN_PROVISIONING_CONFIRMATION_STATUS_CONNECTION_SUCCESS_IP_NOT_ACQUIRED:
                {
                    UART_PRINT("[WLAN EVENT] Confirmation fail: IP address not acquired\r\n");
                    SignalProvisioningEvent(PrvnEvent_ConfirmationFailed);
                }
                break;

                case SL_WLAN_PROVISIONING_CONFIRMATION_STATUS_SUCCESS_FEEDBACK_FAILED:
                {
                    UART_PRINT("[WLAN EVENT] Connection Success (feedback to Smartphone app failed)\r\n");
                    SignalProvisioningEvent(PrvnEvent_ConfirmationFailed);
                }
                break;

                case SL_WLAN_PROVISIONING_CONFIRMATION_STATUS_SUCCESS:
                {
                    UART_PRINT("[WLAN EVENT] Confirmation Success!\r\n");
                    SignalProvisioningEvent(PrvnEvent_ConfirmationSuccess);
                }
                break;

                case SL_WLAN_PROVISIONING_AUTO_STARTED:
                {
                    UART_PRINT("[WLAN EVENT] Auto-Provisioning Started\r\n");
                    /* stop auto provisioning - may trigger in case of returning to default */
                    SignalProvisioningEvent(PrvnEvent_Stopped);
                }
                break;

                case SL_WLAN_PROVISIONING_STOPPED:
                {
                    UART_PRINT("[WLAN EVENT] Provisioning stopped\r\n");
                    if(ROLE_STA == pWlanEvent->Data.ProvisioningStatus.Role)
                    {
                        UART_PRINT(" [WLAN EVENT] - WLAN Connection Status:%d\r\n",pWlanEvent->Data.ProvisioningStatus.WlanStatus);

                        if(SL_WLAN_STATUS_CONNECTED == pWlanEvent->Data.ProvisioningStatus.WlanStatus)
                        {
                            UART_PRINT(" [WLAN EVENT] - Connected to SSID:%s\r\n",pWlanEvent->Data.ProvisioningStatus.Ssid);

                            memcpy (SpeechRecog_ControlBlock.connectionSSID, pWlanEvent->Data.ProvisioningStatus.Ssid, pWlanEvent->Data.ProvisioningStatus.Ssidlen);
                            SpeechRecog_ControlBlock.ssidLen = pWlanEvent->Data.ProvisioningStatus.Ssidlen;

                            /* Provisioning is stopped by the device and provisioning is done successfully */
                            SignalProvisioningEvent(PrvnEvent_Stopped);

                            break;
                        }
                        else
                        {
                            CLR_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_Connection);
                            CLR_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_IpAcquired);
                            CLR_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_Ipv6lAcquired);
                            CLR_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_Ipv6gAcquired);

                            /* Provisioning is stopped by the device and provisioning is not done yet, still need to connect to AP */
                            SignalProvisioningEvent(PrvnEvent_WaitForConn);

                            break;
                        }
                    }
                }

                SignalProvisioningEvent(PrvnEvent_Stopped);

                break;

                case SL_WLAN_PROVISIONING_SMART_CONFIG_SYNCED:
                {
                    UART_PRINT("[WLAN EVENT] Smart Config Synced!\r\n");
                }
                break;

                case SL_WLAN_PROVISIONING_CONFIRMATION_WLAN_CONNECT:
                {
                    SET_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_Connection);
                    CLR_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_IpAcquired);
                    CLR_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_Ipv6lAcquired);
                    CLR_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_Ipv6gAcquired);

                    UART_PRINT("[WLAN EVENT] Connection to AP succeeded\r\n");
                }
                break;

                case SL_WLAN_PROVISIONING_CONFIRMATION_IP_ACQUIRED:
                {
                    SET_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_IpAcquired);

                    UART_PRINT("[WLAN EVENT] IP address acquired\r\n");
                }
                break;

                case SL_WLAN_PROVISIONING_SMART_CONFIG_SYNC_TIMEOUT:
                {
                    UART_PRINT("[WLAN EVENT] Smart Config Sync timeout\r\n");
                }
                break;

                default:
                {
                    UART_PRINT("[WLAN EVENT] Unknown Provisioning Status: %d\r\n",pWlanEvent->Data.ProvisioningStatus.ProvisioningStatus);
                }
                break;
            }
        }
        break;

        default:
        {
            UART_PRINT("[WLAN EVENT] Unexpected event [0x%x]\n\r",
                                                            pWlanEvent->Id);

            SignalProvisioningEvent(PrvnEvent_Error);
        }
        break;
    }
}

//*****************************************************************************
//
//! The Function Handles the Fatal errors
//!
//! \param[in]  slFatalErrorEvent - Pointer to Fatal Error Event info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkFatalErrorEventHandler(SlDeviceFatal_t *slFatalErrorEvent)
{

    switch (slFatalErrorEvent->Id)
    {
        case SL_DEVICE_EVENT_FATAL_DEVICE_ABORT:
        {
            UART_PRINT("[ERROR] - FATAL ERROR: Abort NWP event detected: "
                        "AbortType=%d, AbortData=0x%x\n\r",
                        slFatalErrorEvent->Data.DeviceAssert.Code,
                        slFatalErrorEvent->Data.DeviceAssert.Value);
        }
        break;

        case SL_DEVICE_EVENT_FATAL_DRIVER_ABORT:
        {
            UART_PRINT("[ERROR] - FATAL ERROR: Driver Abort detected. \n\r");
        }
        break;

        case SL_DEVICE_EVENT_FATAL_NO_CMD_ACK:
        {
            UART_PRINT("[ERROR] - FATAL ERROR: No Cmd Ack detected "
                       "[cmd opcode = 0x%x] \n\r",
                       slFatalErrorEvent->Data.NoCmdAck.Code);
        }
        break;

        case SL_DEVICE_EVENT_FATAL_SYNC_LOSS:
        {
            UART_PRINT("[ERROR] - FATAL ERROR: Sync loss detected n\r");
        }
        break;

        case SL_DEVICE_EVENT_FATAL_CMD_TIMEOUT:
        {
            UART_PRINT("[ERROR] - FATAL ERROR: Async event timeout detected "
                       "[event opcode =0x%x]  \n\r",
                       slFatalErrorEvent->Data.CmdTimeout.Code);
        }
        break;

        default:
            UART_PRINT("[ERROR] - FATAL ERROR: Unspecified error detected \n\r");
            break;
    }


}

//*****************************************************************************
//
//! This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    SlNetAppEventData_u *pNetAppEventData = NULL;

    if (NULL == pNetAppEvent)
    {
        return;
    }

    pNetAppEventData = &pNetAppEvent->Data;

     switch(pNetAppEvent->Id)
    {
        case SL_NETAPP_EVENT_IPV4_ACQUIRED:
        {
            SlIpV4AcquiredAsync_t   *pEventData = NULL;

            SET_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_IpAcquired);

            /* Ip Acquired Event Data */
            pEventData = &pNetAppEvent->Data.IpAcquiredV4;

            /* Gateway IP address */
            SpeechRecog_ControlBlock.gatewayIP = pEventData->Gateway;

            UART_PRINT("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
                        "Gateway=%d.%d.%d.%d\n\r",
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,3),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,2),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,1),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,0),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,3),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,2),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,1),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,0));

            sem_post(&Provisioning_ControlBlock.connectionAsyncEvent);
        }
        break;

        case SL_NETAPP_EVENT_IPV6_ACQUIRED:
        {
            if(!GET_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_Ipv6lAcquired))
            {
                SET_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_Ipv6lAcquired);
                UART_PRINT("[NETAPP EVENT] Local IPv6 Acquired\n\r");
            }
            else
            {
                SET_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_Ipv6gAcquired);
                UART_PRINT("[NETAPP EVENT] Global IPv6 Acquired\n\r");
            }

            sem_post(&Provisioning_ControlBlock.connectionAsyncEvent);
        }
        break;

        case SL_NETAPP_EVENT_DHCPV4_LEASED:
        {
            SET_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_IpLeased);

            UART_PRINT("[NETAPP EVENT] IPv4 leased %d.%d.%d.%d for device %02x:%02x:%02x:%02x:%02x:%02x\n\r",\
                (uint8_t)SL_IPV4_BYTE(pNetAppEventData->IpLeased.IpAddress,3),\
                (uint8_t)SL_IPV4_BYTE(pNetAppEventData->IpLeased.IpAddress,2),\
                (uint8_t)SL_IPV4_BYTE(pNetAppEventData->IpLeased.IpAddress,1),\
                (uint8_t)SL_IPV4_BYTE(pNetAppEventData->IpLeased.IpAddress,0),\
                pNetAppEventData->IpLeased.Mac[0],\
                pNetAppEventData->IpLeased.Mac[1],\
                pNetAppEventData->IpLeased.Mac[2],\
                pNetAppEventData->IpLeased.Mac[3],\
                pNetAppEventData->IpLeased.Mac[4],\
                pNetAppEventData->IpLeased.Mac[5]);

        }
        break;

        case SL_NETAPP_EVENT_DHCPV4_RELEASED:
        {
            CLR_STATUS_BIT(SpeechRecog_ControlBlock.status, AppStatusBits_IpLeased);

            UART_PRINT("[NETAPP EVENT] IPv4 released %d.%d.%d.%d for device %02x:%02x:%02x:%02x:%02x:%02x\n\r",\
                (uint8_t)SL_IPV4_BYTE(pNetAppEventData->IpReleased.IpAddress,3),\
                (uint8_t)SL_IPV4_BYTE(pNetAppEventData->IpReleased.IpAddress,2),\
                (uint8_t)SL_IPV4_BYTE(pNetAppEventData->IpReleased.IpAddress,1),\
                (uint8_t)SL_IPV4_BYTE(pNetAppEventData->IpReleased.IpAddress,0),\
                pNetAppEventData->IpReleased.Mac[0],\
                pNetAppEventData->IpReleased.Mac[1],\
                pNetAppEventData->IpReleased.Mac[2],\
                pNetAppEventData->IpReleased.Mac[3],\
                pNetAppEventData->IpReleased.Mac[4],\
                pNetAppEventData->IpReleased.Mac[5]);

            UART_PRINT("Reason: ");
            switch(pNetAppEventData->IpReleased.Reason)
            {
                case SL_IP_LEASE_PEER_RELEASE: UART_PRINT("Peer released\n\r");
                break;

                case SL_IP_LEASE_PEER_DECLINE: UART_PRINT("Peer declined\n\r");
                break;

                case SL_IP_LEASE_EXPIRED: UART_PRINT("Lease expired\n\r");
                break;
            }
        }
        break;

        case SL_NETAPP_EVENT_DHCP_IPV4_ACQUIRE_TIMEOUT:
        {
            UART_PRINT("[NETAPP EVENT] DHCP IPv4 Acquire timeout\n\r");
        }
        break;

        default:
        {
            UART_PRINT("[NETAPP EVENT] Unexpected event [0x%x] \n\r",
                       pNetAppEvent->Id);
        }
        break;
    }
}


//*****************************************************************************
//
//! This function handles HTTP server events
//!
//! \param[in]  pServerEvent    - Contains the relevant event information
//! \param[in]  pServerResponse - Should be filled by the user with the
//!                               relevant response information
//!
//! \return None
//!
//****************************************************************************
void SimpleLinkHttpServerEventHandler(SlNetAppHttpServerEvent_t *pHttpEvent,
                                SlNetAppHttpServerResponse_t *pHttpResponse)
{
    /* Unused in this application */
    UART_PRINT("[HTTP SERVER EVENT] Unexpected HTTP server event \n\r");
}

//*****************************************************************************
//
//! This function handles General Events
//!
//! \param[in]  pDevEvent - Pointer to General Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{

    /*
        Most of the general errors are not FATAL are are to be handled
        appropriately by the application.
    */
    if(NULL == pDevEvent) return;
    switch(pDevEvent->Id)
    {
        case SL_DEVICE_EVENT_RESET_REQUEST:
        {
            UART_PRINT("[GENERAL EVENT] Reset Request Event\r\n");
        }
        break;

        default:
        {
            UART_PRINT("[GENERAL EVENT] - ID=[%d] Sender=[%d]\n\n",
                                                pDevEvent->Data.Error.Code,
                                                pDevEvent->Data.Error.Source);


        }
        break;
    }
}


//*****************************************************************************
//
//! This function handles socket events indication
//!
//! \param[in]  pSock - Pointer to Socket Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    if(SL_SOCKET_ASYNC_EVENT == pSock->Event)
    {
        UART_PRINT("[SOCK ERROR] an event received on socket %d\r\n",pSock->SocketAsyncEvent.SockAsyncData.Sd);
        switch(pSock->SocketAsyncEvent.SockAsyncData.Type)
        {
        case SL_SSL_NOTIFICATION_CONNECTED_SECURED:
        UART_PRINT("[SOCK ERROR] SSL handshake done");
            break;
        case SL_SSL_NOTIFICATION_HANDSHAKE_FAILED:
        UART_PRINT("[SOCK ERROR] SSL handshake failed with error %d\r\n", pSock->SocketAsyncEvent.SockAsyncData.Val);
            break;
        case SL_SSL_ACCEPT:
            UART_PRINT("[SOCK ERROR] Recoverable error occurred during the handshake %d\r\n",pSock->SocketAsyncEvent.SockAsyncData.Val);
            break;
        case SL_OTHER_SIDE_CLOSE_SSL_DATA_NOT_ENCRYPTED:
            UART_PRINT("[SOCK ERROR] Other peer terminated the SSL layer.\r\n");
            break;
        case SL_SSL_NOTIFICATION_WRONG_ROOT_CA:
            UART_PRINT("[SOCK ERROR] Used wrong CA to verify the peer.\r\n");

            break;
        default:
            break;
        }
    }

    /* This application doesn't work w/ socket - Events are not expected */
     switch( pSock->Event )
     {
       case SL_SOCKET_TX_FAILED_EVENT:
           switch( pSock->SocketAsyncEvent.SockTxFailData.Status)
           {
              case SL_ERROR_BSD_ECLOSE:
                   UART_PRINT("[SOCK ERROR] - close socket (%d) operation "
                                "failed to transmit all queued packets\n\r",
                                pSock->SocketAsyncEvent.SockTxFailData.Sd);
               break;
               default:
                    UART_PRINT("[SOCK ERROR] - TX FAILED  :  socket %d , "
                                "reason (%d) \n\n",
                                pSock->SocketAsyncEvent.SockTxFailData.Sd,
                                pSock->SocketAsyncEvent.SockTxFailData.Status);
               break;
           }
           break;

       default:
            UART_PRINT("[SOCK EVENT] - Unexpected Event [%x0x]\n\n",pSock->Event);
            break;
     }
}

void SimpleLinkNetAppRequestMemFreeEventHandler (uint8_t *buffer)
{
    /* Unused in this application */
}
void SimpleLinkNetAppRequestEventHandler(SlNetAppRequest_t *pNetAppRequest, SlNetAppResponse_t *pNetAppResponse)
{
  // do nothing...
}
//*****************************************************************************
//
//! \brief This function fetches the device type and prints it
//!
//! \param[in]  None
//!
//! \return device type
//!
//****************************************************************************
uint32_t getDeviceType(void)
{
    uint32_t            deviceType;

    deviceType = PRCMDeviceTypeGet();

    if ((deviceType & PRCM_DEV_TYPE_CC3220FS) == PRCM_DEV_TYPE_CC3220FS)
    {
        UART_PRINT("[Provisioning task] detected device is CC3220SF\n\r");

        return deviceType;
    }

    if ((deviceType & PRCM_DEV_TYPE_CC3220RS) == PRCM_DEV_TYPE_CC3220RS)
    {
        UART_PRINT("[Provisioning task] detected device is CC3220RS\n\r");

        return deviceType;
    }

    if ((deviceType & PRCM_DEV_TYPE_CC3220R) == PRCM_DEV_TYPE_CC3220R)
    {
        UART_PRINT("[Provisioning task] detected device is CC3220R\n\r");

        return deviceType;
    }

    return deviceType;
}
#endif
