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
//******************************************************************************
//! \file           npi_task.h
//! \brief          NPI is a TI RTOS Application Thread that provides a
//! \brief          common Network Processor Interface framework.
//
//  Revised:        $Date: 2015-01-28 17:22:05 -0800 (Wed, 28 Jan 2015) $
//  Revision:       $Revision: 42106 $
//
//******************************************************************************
#ifndef NPI_TASK_H
#define NPI_TASK_H

#ifdef __cplusplus
extern "C"
{
#endif

// ****************************************************************************
// includes
// ****************************************************************************
#include "npi_data.h"

// ****************************************************************************
// defines
// ****************************************************************************

// ****************************************************************************
// typedefs
// ****************************************************************************

//! \brief Call back function that a subsystem must register with NPI Task to
//         recieve messages from the Host Processor
typedef void (*npiFromHostCBack_t)(_npiFrame_t *pNPIMsg);

//! \brief Call back function that a subsystem must register with NPI Task to
//         recieve forwarded messages from ICall
typedef void (*npiFromICallCBack_t)(Uint8 *pGenMsg);

typedef struct
{
  Uint16              stackSize;      //!< Configurable size of stack for NPI Task
  Uint16              bufSize;        //!< Buffer size of Tx/Rx Transport layer buffers
  Uint32              mrdyPinID;      //!< Pin ID Mrdy (only with Power Saving enabled)
  Uint32              srdyPinID;      //!< Pin ID Srdy (only with Power Saving enabled)
  Uint8               portType;       //!< NPI_SERIAL_TYPE_[UART,SPI]
  Uint8               portBoardID;    //!< Board ID for HW, i.e. CC2650_UART0
  npiInterfaceParams    portParams;     //!< Params to initialize NPI port
} NPI_Params;

//*****************************************************************************
// globals
//*****************************************************************************
extern const NPI_Params NPI_defaultParams;

//*****************************************************************************
// function prototypes
//*****************************************************************************

// -----------------------------------------------------------------------------
//! \brief      Initialize a NPI_Params struct with default values
//!
//! \param[in]  portType  NPI_SERIAL_TYPE_[UART,SPI]
//! \param[in]  params    Pointer to NPI params to be initialized
//!
//! \return     Uint8   Status NPI_SUCCESS, NPI_TASK_INVALID_PARAMS
// -----------------------------------------------------------------------------
extern Uint8 NPITask_Params_init(Uint8 portType, NPI_Params *params);

// -----------------------------------------------------------------------------
//! \brief      Task creation function for NPI
//!
//! \param[in]  params    Pointer to NPI params which will be used to
//!                       initialize the NPI Task
//!
//! \return     Uint8   Status NPI_SUCCESS, or NPI_TASK_FAILURE
// -----------------------------------------------------------------------------
extern Uint8 NPITask_open(NPI_Params *params);

// -----------------------------------------------------------------------------
//! \brief      NPI Task close and tear down. Cannot be used with ICall becasue
//!             ICall service cannot be un-enrolled
//!
//! \return     Uint8   Status NPI_SUCCESS, or NPI_TASK_FAILURE
// -----------------------------------------------------------------------------
extern Uint8 NPITask_close(void);

// -----------------------------------------------------------------------------
//! \brief      API for application task to send a message to the Host.
//!             NOTE: It's assumed all message traffic to the stack will use
//!             other (ICALL) APIs/Interfaces.
//!
//! \param[in]  pMsg    Pointer to "unframed" message buffer.
//!
//! \return     Uint8 Status NPI_SUCCESS, or NPI_SS_NOT_FOUND
// -----------------------------------------------------------------------------
extern Uint8 NPITask_sendToHost(_npiFrame_t *pMsg);

// -----------------------------------------------------------------------------
//! \brief      API for subsystems to register for NPI messages recieved with
//!             the specific ssID. All NPI messages will be passed to callback
//!             provided
//!
//! \param[in]  ssID    The subsystem ID of NPI messages that should be routed
//!                     to pCB
//! \param[in]  pCB     The call back function that will recieve NPI messages
//!
//! \return     Uint8 Status NPI_SUCCESS, or NPI_ROUTING_FULL
// -----------------------------------------------------------------------------
extern Uint8 NPITask_regSSFromHostCB(Uint8 ssID, npiFromHostCBack_t pCB);

// -----------------------------------------------------------------------------
//! \brief      API for subsystems to register for ICall messages recieved from
//!             the specific source entity ID. All ICall messages will be passed
//!             to the callback provided
//!
//! \param[in]  icallID Source entity ID whose messages should be sent to pCB
//!             pCB     The call back function that will recieve ICall messages
//!
//! \return     Uint8 Status NPI_SUCCESS, or NPI_ROUTING_FULL
// -----------------------------------------------------------------------------
extern Uint8 NPITask_regSSFromICallCB(Uint8 icallID, npiFromICallCBack_t pCB);

// -----------------------------------------------------------------------------
//! \brief      API to allocate an NPI frame of a given data length
//!
//! \param[in]  len             Length of data field of frame
//!
//! \return     _npiFrame_t *   Pointer to newly allocated frame
// -----------------------------------------------------------------------------
extern _npiFrame_t * NPITask_mallocFrame(Uint16 len);

// -----------------------------------------------------------------------------
//! \brief      API to de-allocate an NPI frame
//!
//! \param[in]  frame   Pointer to NPI frame to be de-allocated
//!
//! \return     void
// -----------------------------------------------------------------------------
extern void NPITask_freeFrame(_npiFrame_t *frame);

#ifdef __cplusplus
}
#endif // extern "C"

#endif // end of NPI_TASK_H definition
