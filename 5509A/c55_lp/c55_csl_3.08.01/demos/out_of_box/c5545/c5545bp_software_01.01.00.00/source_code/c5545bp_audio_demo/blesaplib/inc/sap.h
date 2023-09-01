/* --COPYRIGHT--,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
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
 * --/COPYRIGHT--*/
/**
  @headerfile:    sap.h
  $Date: 2015-06-22 20:01:12 -0700 (Mon, 22 Jun 2015) $
  $Revision: 44190 $

  Description:    This file contains the Simple Application Processor API.
                  SAP utilizes the SNP Remote Procedure call APIs to
                  control an external BLE Network Processor

**************************************************************************************************/

#ifndef SAP_H
#define SAP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

//#include "SNP.h"


/*********************************************************************
 * CONSTANTS
 */

#define GAP_ADTYPE_FLAGS                        0x01 //!< Discovery Mode: @ref GAP_ADTYPE_FLAGS_MODES
#define GAP_ADTYPE_FLAGS_GENERAL                0x02 //!< Discovery Mode: LE General Discoverable Mode
#define GAP_ADTYPE_16BIT_MORE                   0x02 //!< Service: More 16-bit UUIDs available
#define GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED    0x04 //!< Discovery Mode: BR/EDR Not Supported
#define GAP_ADTYPE_LOCAL_NAME_COMPLETE          0x09 //!< Complete local name
#define GAP_ADTYPE_SERVICE_DATA                 0x16 //!< Service Data - 16-bit UUID

/* @defgroup SAP_DEFINES Constants and Structures
 * @{
 */

/** @defgroup SAP_PARAM_SUBSYSTEMS SAP Parameter Subsystems
 * @{
 */
#define SAP_PARAM_HCI             0x0001 //!< GAP HCI parameters: @ref SAP_GAP_HCI_PARAMS
#define SAP_PARAM_ADV             0x0002 //!< Advertising parameters: @ref SAP_ADV_PARAMS
#define SAP_PARAM_CONN            0x0003 //!< Connection parameters: @ref SAP_CONN_PARAMS
/** @} End SAP_PARAM_SUBSYSTEMS */


/** @defgroup SAP_ADV_PARAMS SAP Advertising Parameters
 * @{
 */
#define SAP_ADV_DATA_NOTCONN      0x0001 //!< Advertising Data when not in Connection
#define SAP_ADV_DATA_CONN         0x0002 //!< Advertising Data when in a Connection
#define SAP_ADV_DATA_SCANRSP      0x0003 //!< Scan Response data
#define SAP_ADV_STATE             0x0004 //!< Advertising state
/** @} End SAP_ADV_PARAMS */

/** @defgroup SAP_CONN_PARAMS SAP Connection Parameters
 * @{
 */
#define SAP_CONN_PARAM            0x0001 //!< Connection parameters
#define SAP_CONN_STATE            0x0002 //!< Connection state
/** @} End SAP_CONN_PARAMS */

/** @defgroup SAP_PORT_TYPES SAP port types
 * @{
 */
#define SAP_PORT_LOCAL                   0x00 //!< Locally running SAP (not supported)
#define SAP_PORT_REMOTE_UART             0x01 //!< Remote connection w/ SAP over UART
#define SAP_PORT_REMOTE_SPI              0x02 //!< Remote connection w/ SAP over SPI (not supported)
/** @} End SAP_PORT_TYPES */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/** @defgroup SAP_GAP_EVENT_CB SAP GAP Event Callback.
 * @{
 */
#if akhil
typedef void (*pfnEventCB_t)(Uint16 event, snpEventParam_t *param);

typedef void (*pfnAsyncCB_t)(Uint8 cmd1, void *pParams);

/** @} End SAP_GAP_EVENT_CB */

/** @defgroup SAP_GATT_READ_ATTR_CB SAP Write Attribue Callback.
 * @{
 */
typedef Uint8 (*pfnGATTReadAttrCB_t)(void *context,
                                       Uint16 connectionHandle,
                                       Uint16 charHdl, Uint16 offset,
                                       Uint16 size, Uint16 * len,
                                       Uint8 *pData);
/** @} End SAP_GATT_READ_ATTR_CB */

/** @defgroup SAP_GATT_WRITE_ATTR_CB SAP Write Attribute Callback.
 * @{
 */
typedef Uint8 (*pfnGATTWriteAttrCB_t)(void *context,
                                        Uint16 connectionHandle,
                                        Uint16 charHdl, Uint16 len,
                                        Uint8 *pData);
/** @} End SAP_GATT_WRITE_ATTR_CB */

/** @defgroup SAP_CCCD_Req_CB SAP CCCB Request Call back.
 * @{
 */
typedef Uint8 (*pfnCCCDIndCB_t)(void *context,
                                  Uint16 connectionHandle,
                                  Uint16 cccdHdl, Uint8 rspNeeded,
                                  Uint16 value);
/** @} End SAP_CCCD_REQ_CB */

//!< UUID type
typedef struct
{
  Uint8 len;    //!< Length of UUID buffer
  Uint8 *pUUID; //!< UUID buffer
} UUIDType_t;

//!< User Description Attribute type
typedef snpAddAttrCccd_t     SAP_UserCCCDAttr_t;
typedef snpAddAttrFormat_t   SAP_FormatAttr_t;
typedef snpAddAttrUserDesc_t SAP_UserDescAttr_t ;
typedef snpAddAttrGenShortUUID_t        SAP_ShortUUID_t;
typedef snpAddAttrGenLongUUID_t         SAP_LongUUID_t;

// Characteristic handle type
typedef struct
{
  Uint16 valueHandle;    //!< Handle of characteristic value
  Uint16 userDescHandle; //!< Handle of characteristic user description
  Uint16 cccdHandle;     //!< Handle of characteristic CCCD
  Uint16 formatHandle;   //!< Handle of characteristic format
  Uint16 sUUIDHandle;    //!< Handle of Generic Characteristic Short UUID
  Uint16 lUUIDHandle;    //!< Handle of Generic Characteristic Long UUID
} SAP_CharHandle_t;

//!< Characteristic Value Declration type.
typedef struct
{
  UUIDType_t         UUID;        //!< Identity of the characteristic
  Uint8            properties;  //!< Characteristic value properties
  Uint8            permissions; //!< Characteristic value permissions
  SAP_UserDescAttr_t *pUserDesc;  //!< User descriptor characteristic
  SAP_UserCCCDAttr_t *pCccd;      //!< User CCCD
  SAP_FormatAttr_t   *pFormat;    //!< User format.
  SAP_ShortUUID_t    *pShortUUID; //!< Generic Attribute Descriptor (Short UUID)
  SAP_LongUUID_t     *pLongUUID;  //!< Generic Attribute Descriport (Long UUID)
} SAP_Char_t;

/** @defgroup SAP_GATT_SERV_REG SAP GATT service registration parameter
 * @{
 */
typedef struct
{
  UUIDType_t           serviceUUID;       //!< Identity of the service
  Uint8              serviceType;       //!< Primary, Secondary, Included.
  Uint16             charTableLen;      //!< Length of custom characteristic array
  SAP_Char_t           *charTable;        //!< Array of custom characters to add to the service
  void*                context;           //!< The context is passed by the callbacks
  pfnGATTReadAttrCB_t  charReadCallback;  //!< Read callback function pointer: @ref SAP_GATT_WRITE_ATTR_CB
  pfnGATTWriteAttrCB_t charWriteCallback; //!< Write callback function pointer: @ref SAP_GATT_WRITE_ATTR_CB
  pfnCCCDIndCB_t       cccdIndCallback;   //!< Write callback function pointer: @ref SAP_CCCB_REQ_CB
  Uint16             serviceHandle;     //!< Handle of the service characteristic
  SAP_CharHandle_t     *charAttrHandles;  //!< Array of handles for the characteristics
} SAP_Service_t;
/** @} End SAP_GATT_SERV_REG */
#endif

//!< Port structure when running a local version of SAP (not currently supported)
typedef struct
{
  Uint8              reserved;
} SAP_LocalPort_t;

//!< Port structure when using a remote SNP
typedef struct
{
  Uint16             stackSize;      //!< Configurable size of stack for SAP Port
  Uint16             bufSize;        //!< Size of Tx/Rx Transport layer buffers
  Uint32             mrdyPinID;      //!< Pin ID Mrdy (only with Power Saving enabled)
  Uint32             srdyPinID;      //!< Pin ID Srdy (only with Power Saving enabled)
  Uint8              boardID;        //!< Board ID for physical port, i.e. CC2650_UART0
  Uint32             bitRate;        //!< Baud/Bit Rate for physical port
} SAP_RemotePort_t;

//!< SAP Parameters
typedef struct
{
  Uint8               portType;
  union
  {
    SAP_LocalPort_t      local;
    SAP_RemotePort_t     remote;
  } port;
} SAP_Params;

/** @} End SAP_DEFINES */

/*********************************************************************
*  EXTERNAL VARIABLES
*/

extern const SAP_RemotePort_t default_remSPIPort;
extern const SAP_RemotePort_t default_remUARTPort;

/*********************************************************************
 * FUNCTIONS
 */

/**
 * @defgroup SAP_API High Level SAP API Functions
 *
 * @{
 */

/**
 * @fn      SAP_initParams
 *
 * @brief   Initializes network processor port parameters
 *
 * @param   None.
 *
 * @return  None.
 */
extern void SAP_initParams(Uint8 portType, SAP_Params *params);

/**
 * @fn      SAP_open
 *
 * @brief   Opens serial port with the network processor
 *
 * @param   params - list of parameters needed to initialize serial port used
 *                   with network processor
 *
 * @return  Uint8 - SNP_SUCCESS if NPI is open
 */
extern Uint8 SAP_open(SAP_Params *params);

/**
 * @fn      SAP_close
 *
 * @brief   Tears down serial port with the network processor
 *
 * @param   None.
 *
 * @return  Uint8 - SNP_SUCCESS if able to close
 */
extern Uint8 SAP_close(void);

/**
 * @fn      SAP_setAsyncCB
 *
 * @brief   setup Applications' asynchronous callbacks.  This must be called before
 *          using any other calls to SAP.  This function may be called multiple times
 *          to register multiple Callbacks.
 *
 * @param   asyncCB - the asynchronous callback.
 *
 * @return  None.
 */
//commented:akhil
//extern Uint8 SAP_setAsyncCB(pfnAsyncCB_t asyncCB);
//end
/**
 * @brief       Initialize the SAP module
 *
 * @param       None.
 *
 * @return      SNP_SUCCESS: the device correctly initialized.<BR>
 *              SAP_FAILRUE: the device failed to initialize.<BR>
 */
extern Uint8 SAP_initDevice(void);

/**
 * @brief       Reset the SAP and return only when it has restarted.
 *
 * @param       none
 *
 * @return      SNP_SUCCESS: the device correctly reset.<BR>
 */
extern Uint8 SAP_reset(void);

/**
 * @brief       Register a GATT service with the GATT Server.
 *
 * @param       serviceParams - a pointer to service data: @ref SAP_GATT_SERV_REG
 *                              It is the callers responsibility to allocate
 *                              this structure.
 *
 * @return      SNP_SUCCESS: the service is registered.<BR>
 *              SAP_FAILRUE: service registration failed.<BR>
 */
//commented:akhil
//extern Uint8 SAP_registerService(SAP_Service_t *serviceParams);
//end
/**
 * @brief       Register a callback to receive a GAP event.  This shall be
 *              called once for each callback to be registered.
 *
 * @param       eventCB   - a Callback function to register: @ref SAP_GAP_EVENT_CB
 * @param       eventMask - the mask of events which trigger this
 *                          callback. Events types: @ref SAP_GAP_EVENTS
 *
 * @return      SNP_SUCCESS: the device correctly reset.<BR>
 *              SAP_FAILRUE: callback registration failed.<BR>
 */
//commented:akhil
//extern Uint8 SAP_registerEventCB(pfnEventCB_t eventCB, Uint16 eventMask);
//end
/**
 * @brief       Read a characteristic value of a service.
 *
 * @param       serviceID - the UUID of the service
 * @param       charID    - the unique handle of the characteristic
 * @param       len       - length of the data read
 * @param       pData     - pointer to buffer to write to
 *
 * @return      SNP_SUCCESS: the read completed successfully.<BR>
 *              SAP_FAILRUE: service param request failed.<BR>
 */
extern Uint8 SAP_getServiceParam(Uint8 serviceID, Uint8 charID,
                                   Uint16 *len, Uint8 *pData);

/**
 * @brief       Write a characteristic value of a service.
 *
 * @param       serviceID - the UUID of the service
 * @param       charID    - the unique handle of the characteristic
 * @param       len       - length of the data to write
 * @param       pData     - pointer to buffer of data to write
 *
 * @return      SNP_SUCCESS: the write completed successfully.<BR>
 *              SAP_FAILRUE: service param write request failed.<BR>
 */
extern Uint8 SAP_setServiceParam(Uint8 serviceID, Uint8 charID,
                                   Uint16 len, Uint8 *pData);

/**
 * @brief       Write to a stack parameter on the SAP. Some responses will
 *              Return immediately, others will generate an event for which
 *              a callback must be registered with the correct event mask.
 *
 * @param       subystemID  - the subsystem ID: @ref SAP_PARAM_SUBSYSTEMS
 * @param       paramID     - the parameter within the subsystem to write
 * @param       len         - length of the data to write
 * @param       pData       - pointer to buffer of data to write
 *
 * @return      SNP_SUCCESS: the write completed successfully.<BR>
 *              SAP_FAILRUE: stack parameter write failed.<BR>
 */
extern Uint8 SAP_setParam(Uint8 subsystemID, Uint16 paramID,
                            Uint16 len, Uint8 *pData);

/**
 * @brief       Read a stack parameter on the SAP. Some responses will
 *              Return immediately, others will generate an event for which
 *              a callback must be registered with the correct event mask.
 *
 * @param       subystemID - the subsystem ID: @ref SAP_PARAM_SUBSYSTEMS
 * @param       paramID    - the parameter within the substem to read
 * @param       len        - length of the data to read
 * @param       pData      - pointer to buffer to write to
 *
 * @return      SNP_SUCCESS: the read completed successfully.<BR>
 *              SAP_FAILRUE: stack param read failed.<BR>
 */
extern Uint8 SAP_getParam(Uint8 subsystemID, Uint8 paramID,
                            Uint16 len, Uint8 *pData);



/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SAP_H */

