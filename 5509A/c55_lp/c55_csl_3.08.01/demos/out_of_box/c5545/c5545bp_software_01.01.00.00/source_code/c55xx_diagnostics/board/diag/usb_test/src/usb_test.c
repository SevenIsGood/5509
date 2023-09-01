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

/*! \file usb_test.c
*
*   \brief Implementation for initializing and configuring the USB module for 
*   operating as HID device acting as USB mouse. Uses push buttons on the 
*   BoosterPack to move the mouse pointer.
*
*/

#include "usb_test.h"
#include "stdio.h"
#include "csl_general.h"
#include "csl_sar.h"
#include "csl_gpio.h"
#include "csl_intc.h"
#include "csl_sysctrl.h"

CSL_HidInitStructApp  appHidInitStructApp;
pHidAppClassHandle    pAppClassHandle = & appHidInitStructApp;

CSL_HidClassStruct  appHidClassStruct;
pHidClassHandle     appHidClassHandle = & appHidClassStruct;

CSL_UsbSetupStruct usbSetUp;

CSL_UsbDevHandle gUsbDevHandle;

extern CSL_UsbRegsOvly  usbRegisters;

extern void VECSTART(void);

Uint16 SR_ODD_BYTE_FLAG = 0;

Uint16 preKey = NoKey_MIN_VALUE;
Uint16 keyCnt =0;

/* SAR object Structure    */
CSL_SarHandleObj SarObj;
CSL_SarHandleObj *SarHandle;

/* Global Structure Declaration */
CSL_GpioObj     GpioPinObj;
CSL_GpioObj     *gpioPinHandle;

Uint8 reportDesc[] = {0x05,0x01,0x09,0x02,0xa1,0x01,0x09,0x01,
                      0xa1,0x00,0x05,0x09,0x19,0x01,0x29,0x03,
                      0x15,0x00,0x25,0x01,0x95,0x03,0x75,0x01,
                      0x81,0x02,0x95,0x01,0x75,0x05,0x81,0x01,
                      0x05,0x01,0x09,0x30,0x09,0x31,0x09,0x38,
                      0x15,0x81,0x25,0x7f,0x75,0x08,0x95,0x03,
                      0x81,0x06,0xc0,0xc0};

const unsigned char strDesc[4][64] = {
//string 0
{
           4,  //>> length-in-byte
        0x03,  //   bDescriptorType = string
  0x09, 0x04,  //   English-USA
},

//string 1
{
    36, 0x03, //string-length-in-byte, Manufacture ID
   'T', 0x00,
   'e', 0x00,
   'x', 0x00,
   'a', 0x00,
   's', 0x00,
   ' ', 0x00,
   'I', 0x00,
   'n', 0x00,
   's', 0x00,
   't', 0x00,
   'r', 0x00,
   'u', 0x00,
   'm', 0x00,
   'e', 0x00,
   'n', 0x00,
   't', 0x00,
   's', 0x00,
},

//string 2
{
    12, 0x03, //string-length-in-byte, Product ID
   'C', 0x00,
   '5', 0x00,
   '5', 0x00,
   '0', 0x00,
   '5', 0x00,
},

//string 3
{
    12, 0x03, //string-length-in-byte, Serial# ID
   '0', 0x00,
   '0', 0x00,
   '0', 0x00,
   '0', 0x00,
   '1', 0x00
}
};

const Uint16 deviceQualifierDescr[5] =
{
    0x060A,
    0x0200,
    0x0000,
    0x4000,
    0x0001
};

//USB2.0 test data package, constants from the spec
const unsigned char test_packet[53] = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,
  0xAA,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,
  0xEE,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0xBF,0xDF,
  0xEF,0xF7,0xFB,0xFD,0xFC,0x7E,0xBF,0xDF,
  0xEF,0xF7,0xFB,0xFD,0x7E};

const unsigned char *string_descriptor[STRING_DESCR_SZ] =
{
    &strDesc[0][0],     /* Language String */
    &strDesc[1][0],     /* iManufacturer */
    &strDesc[2][0],     /* iProductName */
    &strDesc[3][0],     /* iSerial No :RomId - F# */
    NULL,               /*  */
    NULL,               /*  */
    NULL
};

/* Buffer used to store the HID Report data to send to USB */
#pragma DATA_ALIGN(lbaBufferHidReportApp, 4);
Uint16 lbaBufferHidReportApp[HID_REPORT_SIZE_WORDS+1];

CSL_HidRequestStruct USB_ReqTable[] =
{
  {CSL_HID_REQUEST_GET_STATUS       , HID_reqGetStatus},
  {CSL_HID_REQUEST_CLEAR_FEATURE    , HID_reqClearFeature},
  {CSL_HID_REQUEST_SET_FEATURE      , HID_reqSetFeature},
  {CSL_HID_REQUEST_SET_ADDRESS      , HID_reqSetAddress},
  {CSL_HID_REQUEST_GET_DESCRIPTOR   , HID_reqGetDescriptor},
  {CSL_HID_REQUEST_SET_DESCRIPTOR   , HID_reqUnknown},
  {CSL_HID_REQUEST_GET_CONFIGURATION, HID_reqGetConfiguration},
  {CSL_HID_REQUEST_SET_CONFIGURATION, HID_reqSetConfiguration},
  {CSL_HID_REQUEST_GET_MAX_LUN      , HID_reqGetMaxLUN},
  {CSL_HID_REQUEST_GET_INTERFACE    , HID_reqGetInterface},
  {CSL_HID_REQUEST_SET_INTERFACE    , HID_reqSetInterface},
  {CSL_HID_REQUEST_SYNC_FRAME       , HID_reqUnknown},
  {CSL_HID_REQUEST_SET_CUR          , HID_reqUnknown},
  {CSL_HID_REQUEST_GET_CUR          , HID_reqUnknown},
  {CSL_HID_REQUEST_SET_MIN          , HID_reqUnknown},
  {CSL_HID_REQUEST_GET_MIN          , HID_reqUnknown},
  {CSL_HID_REQUEST_GET_MAX          , HID_reqUnknown},
  {CSL_HID_REQUEST_SET_MAX          , HID_reqUnknown},
  {CSL_HID_REQUEST_GET_RES          , HID_reqUnknown},
  {CSL_HID_REQUEST_SET_RES          , HID_reqUnknown},
  {CSL_HID_REQUEST_HID_GET_REPORT   , HID_reqHidGetReport},
  {CSL_HID_REQUEST_HID_GET_IDLE     , HID_reqUnknown},
  {CSL_HID_REQUEST_HID_GET_PROTOCOL , HID_reqUnknown},
  {CSL_HID_REQUEST_HID_SET_REPORT   , HID_reqUnknown},
  {CSL_HID_REQUEST_HID_SET_IDLE     , HID_reqUnknown},
  {CSL_HID_REQUEST_HID_SET_PROTOCOL , HID_reqUnknown},
  {0, NULL }
};

CSL_Status initializeUsbInstance (void);

CSL_Status CSL_usbHidTest(void);
interrupt void usb_isr(void);

/*
void do_setup(void);
void std_request(void);
void set_address(void);
void class_request(void);
void vendor_request(void);
void stall_bus(void);
*/

CSL_Status completeTransfer(void  *vpContext,
                            void  *vpeps);
CSL_Status startTransfer(void  *vpContext,
                         void  *vpeps);

Uint16 wValue  = 0;
Uint16 wIndex  = 0;
Uint16 wLength = 0;

Uint8  mouseOn = 0;
Uint8  usbOn = 0;
Uint16 keyStore;

//USB Device Descriptor: 18-bytes
const unsigned char deviceDesc[] =
{
    0x12,       // bLength = 18d
    0x01,       // bDeviceDescriptorType=1(defined by USB spec)
    0x00, 0x02, // bcdUSB USB Version 2.0 (L-Byte/H-Byte)
    0x00,       // bDeviceClass
    0x00,       // bDeviceSubclass
    0x00,       // bDeviceProtocol
    0x40,       // bMaxPacketSize 64 Bytes
    0xB3, 0x04, // idVendor(L-Byte/H-Byte)
    0x0B, 0x31, // idProduct(L-Byte/H-Byte)
    0x10, 0x01, // bcdDevice(L-Byte/H-Byte): device's release number
    0,          // iManufacturer String Index
    0,          // iProduct String Index
    0,          // iSerialNumber String Index
    1
};         // bNumberConfigurations

//Configuration Descriptors
const unsigned char cfgDesc[] =
{
   // CONFIGURATION Descriptor
   0x09,      // bLength
   0x02,      // bDescriptorType = Config (constant)
   0x22,  0x00, // wTotalLength(L/H)
   0x01,      // bNumInterfaces
   0x01,      // bConfigValue
   0x00,      // iConfiguration
   0xC0,      // bmAttributes;
   0x28,      // MaxPower is 80mA
   // INTERFACE Descriptor
   0x09,      // length = 9
   0x04,      // type = IF; constant 4 for INTERFACE
   0x00,      // IF #0; Interface Identification #
   0x00,      // bAlternate Setting
   0x01,      // bNumber of Endpoints = 1 for 5515 mouse
   0x03,      // bInterfaceClass = HID
   0x01,      // bInterfaceSubClass = Boot Interface
   0x02,      // bInterfaceProtocol = Mouse
   0x00,      // iInterface
   // HID Descriptor
   0x09,      // length = 9
   0x21,      // bDescriptorType = HID
   0x00,      // bcdHID = HID Class Spec version
   0x01,      //
   0x00,      //
   0x01,      // bNumDescriptors = Number of Decriptors
   0x22,      // bDescriptorType = 34 (REPORT)
   0x34,      // wDescriptorLength = 52
   0x00,      //
   // EndPoint_1_IN Descriptor
   0x07,      // bLength
   0x05,      // bDescriptorType (Endpoint)
   0x81,      // bEndpointAddress and direction(IN)
   0x03,      // bmAttributes: B1B0->transfer-type: control=00; Iso=01; bulk=10; interrupt=11
   0x04,0x00, // wMaxPacketSize(L/H)=4
   0x0A,      // bInterval: Max latency
};

const unsigned char OtherSpeedcfgDesc[] =
{
   // CONFIGURATION Descriptor
   0x09,      // bLength
   0x07,      // bDescriptorType = Config (constant)
   0x22,  0x00, // wTotalLength(L/H)
   0x01,      // bNumInterfaces
   0x01,      // bConfigValue
   0x00,      // iConfiguration
   0xC0,      // bmAttributes;
   0x28,      // MaxPower is 80mA
   // INTERFACE Descriptor
   0x09,      // length = 9
   0x04,      // type = IF; constant 4 for INTERFACE
   0x00,      // IF #0; Interface Identification #
   0x00,      // bAlternate Setting
   0x01,      // bNumber of Endpoints = 1 for 5515 mouse
   0x03,      // bInterfaceClass = HID
   0x01,      // bInterfaceSubClass = Boot Interface
   0x02,      // bInterfaceProtocol = Mouse
   0x00,      // iInterface
   // HID Descriptor
   0x09,      // length = 9
   0x21,      // bDescriptorType = HID
   0x00,      // bcdHID = HID Class Spec version
   0x01,      //
   0x00,      //
   0x01,      // bNumDescriptors = Number of Decriptors
   0x22,      // bDescriptorType = 34 (REPORT)
   0x34,      // wDescriptorLength = 52
   0x00,      //
   // EndPoint_1_IN Descriptor
   0x07,      // bLength
   0x05,      // bDescriptorType (Endpoint)
   0x81,      // bEndpointAddress and direction(IN)
   0x03,      // bmAttributes: B1B0->transfer-type: control=00; Iso=01; bulk=10; interrupt=11
   0x04,0x00, // wMaxPacketSize(L/H)=4
   0x0A,      // bInterval: Max latency
};

/**
 *  \brief  Function to read the keys pressed and send the corresponding event
 *  through USB
 *
 *  \param  None
 *
 *  \return None
 */
void get_mouse()
{
    Uint16 key, event1, event2;

#ifdef C5545_BSTPCK
    key = GPIO_GetKey();   /* Read the pressed key */
#else
    key = SAR_GetKey();   /* Read the pressed key */
#endif

    if (key == RIGHT_KEY)  // Right
	{
		event1 = 0x0500;
        event2 = 0x0000;
	}
    else if (key == LEFT_KEY)  // Left
	{
		event1 = 0xfb00;
        event2 = 0x0000;
	}

	else if(key == STOP_KEY)  // End mouse session
	{
        mouseOn = 0;
        usbOn   = 0;
	}
	else
	{
	    return;
	}

    while(usbRegisters->EPCSR[0].PERI_TXCSR & 2);  // wait for empty FIFO for EP1

    usbRegisters->FIFO1R1 = event1 ;  // Load lower data into FIFO
	usbRegisters->FIFO1R1 = event2 ;  // Load upper data into FIFO

    // Set TxPktRdy bit of EP1
    usbRegisters->EPCSR[0].PERI_TXCSR |= 0x0001;

    usbRegisters->PERI_CSR0 |= 0x08; // Set DATAEND(B3 of PERI_CSR0)
    keyStore = key;        // Store previous key

	return;
}

/**
 *  \brief  Function to send HID report
 *
 *  \param  dataLength    - Length of the report data (16-bit words)
 *  \param  hidReport     - Report buffer pointer
 *
 *  \return CSL status
 */
CSL_Status appGetHidReport(
    Uint16 dataLength, // bytes
    Uint16 *hidReport
)
{
    /* First word is length in bytes */
    *hidReport = dataLength;
    hidReport++;

    /* Get report data */
//    memcpy(hidReport, gHidReport, (dataLength+1)/2);

    return CSL_SOK;
}

/**
 *  \brief  Application call back function for control transactions
 *
 *  \param  None
 *
 *  \return None
 */
void appCtrlFxn(void)
{
    HID_Ctrl(pAppClassHandle->pHidObj);
}

/**
 *  \brief  Application call back function for HID transactions
 *
 *  \param  None
 *
 *  \return None
 */
void appHidFxn(void)
{
    HID_reportHandler(pAppClassHandle->pHidObj);
}

/**
 *  \brief  USB suspend call back function
 *
 *  \param  none
 *
 *  \return Test result
 */
void CSL_suspendCallBack(CSL_Status    status)
{
}

/**
 *  \brief  USB self wake up call back function
 *
 *  \param  none
 *
 *  \return Test result
 */
void selfWakeupCallBack(CSL_Status    status)
{
}

/**
 *  \brief  Function to start USB data transfer
 *
 *  \param  pContext     - Pointer to the global MUSB controller context structure
 *  \param  peps         - Endpoint status structure pointer
 *
 *  \return TRUE  - Operation successful
 *          FALSE - Invalid input parameters
 */
CSL_Status startTransfer(void  *vpContext,
                         void  *vpeps)
{
	pUsbContext      pContext;
	pUsbEpStatus     peps;
	pUsbTransfer     pTransfer;
	CSL_Status       status;
	Uint16           dwFlags;

	status = CSL_SOK;

	pContext  = (pUsbContext)vpContext;
	peps      = (pUsbEpStatus)vpeps;

    if((pContext == NULL) || (peps == NULL))
    {
        return(CSL_ESYS_INVPARAMS);
	}

	if(!pContext->fMUSBIsReady)
	{
		return(CSL_ESYS_INVPARAMS);
	}

	/* The endpoint should be initialized */
	if(!peps->fInitialized)
	{
		return(CSL_ESYS_INVPARAMS);
	}

    pTransfer = peps->pTransfer;
    pTransfer->fComplete=FALSE;

    dwFlags = pTransfer->dwFlags;

	if(dwFlags == (Uint16)CSL_USB_OUT_TRANSFER)
	{
		if(peps->dwEndpoint == (Uint16)CSL_USB_EP0)
		{
			status = USB_processEP0Out(pContext);
		}
		else
		{
			status = USB_handleRx(pContext, CSL_USB_EP2);
		}
	}
	else if(dwFlags == (Uint16)CSL_USB_IN_TRANSFER)
	{
		if(peps->dwEndpoint == (Uint16)CSL_USB_EP0 )
		{
			status = USB_processEP0In(pContext);
		}
		else
		{
			status = USB_handleTx(pContext, CSL_USB_EP1);
		}
	}
	else
	{
		status = CSL_ESYS_INVPARAMS;
	}

	return(status);
}

/**
 *  \brief  Function to complete the Audio Class data transfer
 *
 *  \param  pContext  - USB context structure pointer
 *  \param  peps      - End point status structure pointer
 *
 *  \return None
 */
CSL_Status completeTransfer(void  *vpContext,
                            void  *vpeps)
{
	return(CSL_SOK);
}

/**
 *  \brief  Tests USB HID Module operation
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status CSL_usbHidTest(void)
{
	CSL_Status      status;
	CSL_IRQ_Config  config;

	C55x_msgWrite("Press SW2 to move the mouse cursor to LEFT\n\r");
	C55x_msgWrite("Press SW4 to move the mouse cursor to RIGHT\n\r");
	C55x_msgWrite("Press SW3 to exit from the test\n\r");

	status = GPIO_initInstance();
	if(status != CSL_SOK)
	{
		C55x_msgWrite("GPIO_initInstance() API failed\n");
		return(status);
	}

	/* Initialize USB Module */
	status = initializeUsbInstance();
	if(status != CSL_SOK)
	{
		C55x_msgWrite("initializeUsbInstance() API failed\n");
		return(status);
	}

	usbOn = 1;

	/* Set the interrupt vector start address */
	IRQ_setVecs((Uint32)(&VECSTART));

	/* Plug the USB Isr into vector table */
	config.funcAddr = &usb_isr;
	IRQ_plug(USB_EVENT, config.funcAddr);

	/* Enable USB Interrupts */
	IRQ_enable(USB_EVENT);
	/* Enable CPU Interrupts */
	IRQ_globalEnable();

#ifdef C5535_EZDSP
	C55x_msgWrite("   Moving the mouse cursor to LEFT = SW1 button\n");
	C55x_msgWrite("   Moving the mouse cursor to RIGHT = SW2 button\n");
#else
#ifdef C5545_BSTPCK

#else
	C55x_msgWrite("\n   Moving the mouse cursor UP = UP button\n");
	C55x_msgWrite("   Moving the mouse cursor DOWN = DN button\n");
	C55x_msgWrite("   Moving the mouse cursor to LEFT = RWD button\n");
	C55x_msgWrite("   Moving the mouse cursor to RIGHT = FWD button\n");
	C55x_msgWrite("   Sending RIGHT CLICK Command = MODE button\n");
	C55x_msgWrite("   Sending LEFT CLICK Command = MENU button\n");
	C55x_msgWrite("   Stopping the Test Program = STOP button\n");
#endif
#endif

    while (usbOn == 1)
	{
        while (mouseOn == 1)
        {
			get_mouse();
		}
    }

    status = USB_disconnectDev(gUsbDevHandle);
	if(status != CSL_SOK)
	{
		C55x_msgWrite("USB_disconnectDev() API failed\n");
	}

    status = GPIO_closeInstance();
	if(status != CSL_SOK)
	{
		C55x_msgWrite("GPIO_closeInstance() API failed\n");
	}

	CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE1);

	return (status);
}

/**
 *  \brief  USB interrupt service routine
 *
 *  \param  None
 *
 *  \return None
 */
interrupt void usb_isr (void)
{
	Uint16 txIntStat;
	Uint16 usbIntStat;
	Uint16 intrReg1;
	Uint16 intrReg2;
	Uint16 ep0_rx_tx;

	unsigned char suspend;
	unsigned char reset_usb;
	unsigned char resume;

	intrReg1   = 0;
	intrReg2   = 0;
	txIntStat  = 0;
	usbIntStat = 0;
	ep0_rx_tx  = 0;
	suspend    = 0;
	reset_usb  = 0;
	resume     = 0;

    intrReg1 = usbRegisters->INTSRCR1; /* Read USB TX/RX interrupts  */
    intrReg2 = usbRegisters->INTSRCR2; /* Read common USB interrupts */

    /* Clear interrupts */
    if((intrReg1) != 0)
    {
		usbRegisters->INTCLRR1 = intrReg1;
    }

    if((intrReg2) != 0)
    {
		usbRegisters->INTCLRR2 = intrReg2;
    }

    if(!SR_ODD_BYTE_FLAG)
    {
    /* Get TX interrupt status */
    txIntStat = intrReg1 & 0x001F;

    /* Get USB interrupt status */
    usbIntStat = ((Uint8)(intrReg2) & 0x00FF);

    suspend   = (usbIntStat & 0x01);
    resume    = (usbIntStat & 0x02);
    reset_usb = (usbIntStat & 0x04);

    ep0_rx_tx = (txIntStat & 0x0001);

    /* Reset interrupt */
    if(reset_usb == 0x04) //(usbIntStat & 0x04)
    {
		USB_resetDev(gUsbDevHandle);
    }

    /* Suspend interrupt */
    if(suspend == 0x01) // (usbIntStat & 0x01) //if suspend
    {
		USB_suspendDevice(gUsbDevHandle);
    }

    /* Resume interrupt */
    if(resume == 0x02) // (usbIntStat & 0x02)
    {
        usbRegisters->INDEX_TESTMODE = 0x0000;
    }

    /* EP0 interrupt occurred */
    if (ep0_rx_tx == 0x0001)
    {
		gUsbDevHandle->dwIntSourceL = intrReg1;
		gUsbDevHandle->dwIntSourceH = intrReg2;

		HID_eventHandler(gUsbDevHandle, pAppClassHandle, &usbSetUp);

		if ((usbSetUp.wValue >> CSL_HID_8BIT_SHIFT) == CSL_USB_HID_REPORT_DESCRIPTOR_TYPE)
		{
			mouseOn = 1;
		}
    }//end of "if (ep0_rx_tx == 0x0001)"
    }

	/* Send out End Of Interrupt */
	CSL_FINS(usbRegisters->EOIR, USB_EOIR_EOI_VECTOR, CSL_USB_EOIR_RESETVAL);
}

CSL_Status initializeUsbInstance (void)
{
	CSL_Status    status;
	CSL_UsbConfig usbConfig;

	usbConfig.opMode             = CSL_USB_OPMODE_POLLED;
	usbConfig.maxCurrent         = CSL_USB_MAX_CURRENT;
	usbConfig.appSuspendCallBack = (CSL_USB_APP_CALLBACK)CSL_suspendCallBack;
	usbConfig.appWakeupCallBack  = (CSL_USB_APP_CALLBACK)selfWakeupCallBack;
	usbConfig.startTransferCallback  = startTransfer;
	usbConfig.completeTransferCallback = completeTransfer;

	appHidClassHandle->ctrlHandle.devNum       = CSL_USB0;
	appHidClassHandle->ctrlHandle.suspendFlag  = FALSE;

	appHidClassHandle->ctrlHandle.strDescr     = (char **)strDesc;
	appHidClassHandle->ctrlHandle.hidReqTable  = USB_ReqTable;
	appHidClassHandle->ctrlHandle.hidIfNum     = IF_NUM_HID;
	appHidClassHandle->ctrlHandle.hidReportId  = HID_REPORT_ID;
	appHidClassHandle->ctrlHandle.hidReportLen = HID_REPORT_SIZE_BYTES;

	// Assign values to all the members in pAppClassHandle struct
	pAppClassHandle->pHidObj   = (void *)appHidClassHandle;
    pAppClassHandle->pId       = DEV_PID;  /* Product ID (idProduct) */
    pAppClassHandle->vId       = DEV_PID;  /* Vendor ID (idVendor) */
    pAppClassHandle->numLun    = 2;
	pAppClassHandle->txPktSize = 0;
	pAppClassHandle->rxPktSize = 0;

    pAppClassHandle->hidTxPktSize = 0x0003; // max packet size for HID output report

    /* Calling init routine */
    /* Giving all the table handles and the buffers to the Audio Class module */
    pAppClassHandle->strDescrApp = (char **)strDesc;

    pAppClassHandle->lbaBufferHidReportApp = &lbaBufferHidReportApp[0];
    pAppClassHandle->hidReqTableApp = USB_ReqTable;

    /* All Function Handlers need to be Initialized */
    pAppClassHandle->ctrlHandler  = appCtrlFxn;
    pAppClassHandle->hidHandler = appHidFxn;
    pAppClassHandle->getHidReportApp = appGetHidReport;

    pAppClassHandle->hidIfNum     = IF_NUM_HID; // HID interface number
    pAppClassHandle->hidReportId  = HID_REPORT_ID; // HID report ID
    pAppClassHandle->hidReportLen = HID_REPORT_SIZE_BYTES;

	status = HID_Open(pAppClassHandle, &usbConfig);
	if (status != CSL_SOK)
	{
		C55x_msgWrite("HID_Open failed\n");
		return(status);
	}

	gUsbDevHandle = appHidClassHandle->usbDevHandle;

	appHidClassHandle->ctrlHandle.hEpHandleArray[0] = appHidClassHandle->ctrlHandle.ctrlOutEpHandle;
	appHidClassHandle->ctrlHandle.hEpHandleArray[1] = appHidClassHandle->ctrlHandle.ctrlInEpHandle;
	appHidClassHandle->ctrlHandle.hEpHandleArray[2] = appHidClassHandle->hidHandle.hidIntOutEpHandle;
	appHidClassHandle->ctrlHandle.hEpHandleArray[3] = appHidClassHandle->hidHandle.hidIntInEpHandle;

	gUsbDevHandle->hEpHandleArray[0] = appHidClassHandle->ctrlHandle.ctrlOutEpHandle;
	gUsbDevHandle->hEpHandleArray[1] = appHidClassHandle->ctrlHandle.ctrlInEpHandle;
	gUsbDevHandle->hEpHandleArray[2] = appHidClassHandle->hidHandle.hidIntOutEpHandle;
    gUsbDevHandle->hEpHandleArray[3] = appHidClassHandle->hidHandle.hidIntInEpHandle;

    /* Initialize End point descriptors */
    HID_initDescriptors(appHidInitStructApp.pHidObj, (Uint16 *)deviceDesc,
                        CSL_HID_DEVICE_DESCR, sizeof(deviceDesc));

    HID_initDescriptors(appHidInitStructApp.pHidObj, (Uint16 *)deviceQualifierDescr,
                        CSL_HID_DEVICE_QUAL_DESCR, 10);

    HID_initDescriptors(appHidInitStructApp.pHidObj, (Uint16 *)cfgDesc,
                        CSL_HID_CONFIG_DESCR, sizeof(cfgDesc));

    HID_initDescriptors(appHidInitStructApp.pHidObj, (Uint16 *)OtherSpeedcfgDesc,
                        CSL_HID_OTHER_SPEED_CONFIG_DESCR, sizeof(OtherSpeedcfgDesc));

    HID_initDescriptors(appHidInitStructApp.pHidObj, (Uint16 *)reportDesc,
                        CSL_HID_REPORT_DESC, sizeof(reportDesc));

	return (status);
}

/* Reference the start of the interrupt vector table */
/* This symbol is defined in file vectors.asm       */
extern void VECSTART(void);

/**
 *  \brief    This function is used to configures the GPIO pins of the
 *            switches as input and generata ans interrupt while the
 *            switch is pressed
 */
CSL_Status GPIO_initInstance(void)
{
	Int16 retVal;

	CSL_GpioPinConfig    config;
	volatile Uint32 	 loop;

	/* Set Bus for GPIOs */
	CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE6);

    /* Disable CPU interrupt */
    IRQ_globalDisable();

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

    /* Initialize Interrupt Vector table */
    IRQ_setVecs((Uint32)(&VECSTART));

	/* Open GPIO module */
    gpioPinHandle = GPIO_open(&GpioPinObj,&retVal);
    if((NULL == gpioPinHandle) || (CSL_SOK != retVal))
    {
    	C55x_msgWrite("GPIO_open failed\n");
        return(retVal);
    }

	/* Reset the GPIO module */
    GPIO_reset(gpioPinHandle);

    for (loop=0; loop < 0x5F5E10; loop++){}

	/* GPIO_config API to make PIN12 as input pin */
	config.pinNum    = CSL_GPIO_PIN12;
    config.direction = CSL_GPIO_DIR_INPUT;
    config.trigger   = CSL_GPIO_TRIG_RISING_EDGE;
    retVal = GPIO_configBit(gpioPinHandle,&config);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed - GPIO_configBit\n");
		return(retVal);
	}

    /** test GPIO_config API to make PIN13 as i/p */
	config.pinNum    = CSL_GPIO_PIN13;
    config.direction = CSL_GPIO_DIR_INPUT;
    config.trigger   = CSL_GPIO_TRIG_RISING_EDGE;
    retVal = GPIO_configBit(gpioPinHandle,&config);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed - GPIO_configBit\n");
		return(retVal);
	}

    /** test GPIO_config API to make PIN14 as i/p */
	config.pinNum    = CSL_GPIO_PIN14;
    config.direction = CSL_GPIO_DIR_INPUT;
    config.trigger   = CSL_GPIO_TRIG_RISING_EDGE;
    retVal = GPIO_configBit(gpioPinHandle,&config);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed - GPIO_configBit\n");
		return(retVal);
	}

	/* Enable GPIO interrupts */
    retVal = GPIO_enableInt(gpioPinHandle,CSL_GPIO_PIN12);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed- GPIO_enableInt\n");
		return(retVal);
	}

	/* Enable GPIO interrupts */
    retVal = GPIO_enableInt(gpioPinHandle,CSL_GPIO_PIN13);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed- GPIO_enableInt\n");
		return(retVal);
	}

	/* Enable GPIO interrupts */
    retVal = GPIO_enableInt(gpioPinHandle,CSL_GPIO_PIN14);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed- GPIO_enableInt\n");
		return(retVal);
	}

	/* Clear any pending Interrupt */
    IRQ_clear(GPIO_EVENT);

    IRQ_plug(GPIO_EVENT,&gpioPinISR);

     /* Enabling Interrupt */
    IRQ_enable(GPIO_EVENT);
    ///IRQ_globalEnable();

    preKey = NO_KEY;

    return 0;
}

CSL_Status GPIO_closeInstance(void)
{
	return 0;
}

/**
 *  \brief  GPIO Interrupt Service Routine
 *
 *  \param void
 *
 *  \return void
 */
Uint32 gpioIntCount = 0;
interrupt void gpioPinISR(void)
{

   	Int16 retVal;

   	gpioIntCount++;

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(gpioPinHandle,CSL_GPIO_PIN12,&retVal)))
    {
		preKey = LEFT_KEY;
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(gpioPinHandle,CSL_GPIO_PIN12);
    }

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(gpioPinHandle,CSL_GPIO_PIN13,&retVal)))
    {
		preKey = DOWN_KEY;
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(gpioPinHandle,CSL_GPIO_PIN13);
    }

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(gpioPinHandle,CSL_GPIO_PIN14,&retVal)))
    {
		preKey = RIGHT_KEY;
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(gpioPinHandle,CSL_GPIO_PIN14);
    }

}

/**
 *  \brief  This function used to Read the pressed key
 *
 *  \param void
 *
 *  \return none
 */
Uint16 GPIO_GetKey(void)
{
	Uint16 curKey;

	curKey = preKey;
	preKey = NO_KEY;
	return (curKey);
}

/**
 * \brief    This function is used to verify usb operation through HID module
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n        TEST_PASS  - Test Passed
 * \n        TEST_FAIL  - Test Failed
 */
static TEST_STATUS run_usb_test(void *testArgs)
{
	Int16 retVal;
	Uint8 c = 0;

	CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE1);

	retVal = CSL_usbHidTest();
	if(retVal != TEST_PASS)
	{
		C55x_msgWrite("\nUSB HID Test Failed!!\n");
		return (TEST_FAIL);
	}

#ifdef USE_USER_INPUT

    C55x_msgWrite("\n\n\rPress Y/y if the corresponding events are sent properly\n\r"
    		      "through USB, any other key for failure\n\r");

    C55x_msgRead(&c, 1);
    if((c != 'y') && (c != 'Y'))
    {
    	C55x_msgWrite("Events are not sent properly through USB\n\r");
    	return (TEST_FAIL);
    }
#endif

	return (TEST_PASS);
}

/**
 * \brief This function performs usb test
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS usbTest(void *testArgs)
{
    TEST_STATUS testStatus;

    C55x_msgWrite("\n****************************\n\r");
    C55x_msgWrite(  "          USB Test       \n\r");
    C55x_msgWrite(  "****************************\n\r");

    testStatus = run_usb_test(testArgs);
    if(testStatus != TEST_PASS)
    {
    	C55x_msgWrite("\nUSB Test Failed!\n\r");
    	return (TEST_FAIL);
    }
    else
    {
    	C55x_msgWrite("\nUSB Test Passed!\n\r");
    }

    C55x_msgWrite("\n\rUSB Test Completed!!\n\r");

    return testStatus;
}
