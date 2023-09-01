/* ============================================================================
 * Copyright (c) 2008-2016 Texas Instruments Incorporated.
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


/** @file csl_usb_hid_example.c
 *
 *  @brief Test code to verify the HID class functionality of the USB
 *
 *
 * \page    page20  CSL USB EXAMPLE DOCUMENTATION
 *
 * \section USB8   USB EXAMPLE8 - USB HID CLASS TEST
 *
 * \subsection USB8x    TEST DESCRIPTION:
 * This test is to verify the operation of the CSL USB HID module. This test
 * initializes the USB HID module, SAR module to read the push button values,
 * the test program then waits in a while loop for the user input through push
 * buttons. The EVM will act as an HID (mouse) device and the PC will act as
 * the Host. Using the push buttons present on the EVM, we can send the mouse
 * commands to the Host PC, from EVM.
 *
 * This test can be verified by pressing the push buttons on the EVM, and
 * observing whether the corresponding mouse operation is performed on the
 * Host PC.
 *
 * @verbatim
   Digital values corresponding to the button voltage will vary slightly
   depending on the system clock value. Below are the range of mouse commands
   corresponding to each push button of the EVM

	For C5517 or C5515 EVM:
    UP:   To move the mouse cursor to up from current cursor position
    DN:   To move the mouse cursor to down from current cursor position
    RWD:  To move the mouse cursor to left from current cursor position
    FWD:  To move the mouse cursor to right from current cursor position
    MENU: To send the Right Click command
    MODE: To send the Left Click command
    STOP: To stop the Program

	For C5535 eZDSP:
    SW1:  To move the mouse cursor to left from current cursor position
    SW2:  To move the mouse cursor to right from current cursor position

   @endverbatim
 *
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515, C5535 AND
 * C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection USB8y    TEST PROCEDURE:
 * @li Connect the C5515/17 EVM with a PC via USB cable
 * @li Open the CCS and connect the target (C5515/C5517 EVM)
 * @li Open, build and load the USB program to the target
 * @li Set the PLL frequency to 100MHz
 * @li Run the program loaded on to the target
 * @li After the proper USB enumeration, check the Control Panel-->System-->
 *     Hardware-->Device Manager-->Ports (COM & LPT) looking for Human Interface
 *     Devices.
 * @li The test program is acting as an HID device(mouse), whatever you input
 *     through the push bottons, it will be sent as mouse commands to the host
 *     PC.
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *      C5535 eZdsp: 60MHz and 100MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection USB8z    TEST RESULT:
 * @li USB HID Class device should be detected by the host PC and the user should
 *     be able to interact to the PC using the EVM as a mouse.
 * @li Push Buttons pressed on the EVM, will be read and the respective mouse
 *     commands will be sent to the Host PC.
 *
 * ============================================================================
 */

#include "stdio.h"
#include "csl_general.h"
#include "csl_usb_hid_example.h"
#include "csl_usb_hid_sar.h"
#include "csl_usb_hid_gpio.h"

#define CSL_USB_TEST_PASSED   (0)
#define CSL_USB_TEST_FAILED   (1)

#define CSL_USB_MAX_CURRENT     (50)
#define HID_REPORT_ID           (1)        /**< HID report ID          */
#define DEV_VID                 (0x04B3 )  /**< Vendor ID (idVendor)   */
#define DEV_PID                 (0x310B )  /**< Product ID (idProduct) */
#define IF_NUM_HID              (3)
#define EP_HID_MAXP             (3)        /**< maximum packet size for HID
                                                endpoint */

CSL_HidInitStructApp  appHidInitStructApp;
pHidAppClassHandle    pAppClassHandle = & appHidInitStructApp;

CSL_HidClassStruct  appHidClassStruct;
pHidClassHandle     appHidClassHandle = & appHidClassStruct;

CSL_UsbSetupStruct usbSetUp;

CSL_UsbDevHandle gUsbDevHandle;

extern CSL_UsbRegsOvly  usbRegisters;

extern void VECSTART(void);

Uint16 SR_ODD_BYTE_FLAG = 0;

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

    if (key == UP_KEY) // UP
    {
	    event1 = 0x0000;
        event2 = 0x00fb;
	}
    else if (key == DOWN_KEY)  // Down
	{
	    event1 = 0x0000;
        event2 = 0x0005;
	}
    else if (key == RIGHT_KEY)  // Right
	{
		event1 = 0x0500;
        event2 = 0x0000;
	}
    else if (key == LEFT_KEY)  // Left
	{
		event1 = 0xfb00;
        event2 = 0x0000;
	}
    else if (key == LEFT_CLICK_KEY)    // Mouse 1
	{
		 event1 = 0x0001;
         event2 = 0x0000;
	}
    else if (key == RIGHT_CLICK_KEY)    // Mouse 2
	{
		 event1 = 0x0002;
         event2 = 0x0000;
	}
	else if ((keyStore ==  LEFT_CLICK_KEY) && (key == NO_KEY)) // Mouse 1 release
	{
		event1 = 0x0000;
        event2 = 0x0000;
		keyStore = 0;
	}
	else if ((keyStore == RIGHT_CLICK_KEY) && (key == NO_KEY)) // Mouse 2 release
	{
		event1 = 0x0000;
        event2 = 0x0000;
		keyStore = 0;
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
 *  \brief  main function
 *
 *  \param  None
 *
 *  \return None
 */
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
   /////  Define PaSs_StAtE variable for catching errors as program executes.
   /////  Define PaSs flag for holding final pass/fail result at program completion.
        volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
        volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
   /////                                  program flow reaches expected exit point(s).
   /////
void main(void)
{
	CSL_Status    result;

	printf("USB HID Module Test!!\n");

	result = CSL_usbHidTest();

	if(result == CSL_USB_TEST_PASSED)
	{
		printf("\nUSB HID Test Passed!!\n");
	}
	else
	{
		printf("\nUSB HID Test Failed!!\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Resetting ZPaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was initialized to 1 at declaration.
   /////
	}
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
   /////  At program exit, copy "PaSs_StAtE" into "PaSs".
        PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
   /////                   // pass/fail value determined during program execution.
   /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
   /////   control of a host PC script, will read and record the PaSs' value.
   /////
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

#ifdef C5545_BSTPCK
	status = GPIO_initInstance();
	if(status != CSL_SOK)
	{
		printf("GPIO_initInstance() API failed\n");
		return(status);
	}
#else
	status = SAR_initInstance();
	if(status != CSL_SOK)
	{
		printf("SAR_initInstance() API failed\n");
		return(status);
	}
#endif

	/* Initialize USB Module */
	status = initializeUsbInstance();
	if(status != CSL_SOK)
	{
		printf("initializeUsbInstance() API failed\n");
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
	printf("   Moving the mouse cursor to LEFT = SW1 button\n");
	printf("   Moving the mouse cursor to RIGHT = SW2 button\n");
#else
#ifdef C5545_BSTPCK
	printf("   Moving the mouse cursor to LEFT = SW2 button\n");
	printf("   Moving the mouse cursor to RIGHT = SW4 button\n");
#else
	printf("\n   Moving the mouse cursor UP = UP button\n");
	printf("   Moving the mouse cursor DOWN = DN button\n");
	printf("   Moving the mouse cursor to LEFT = RWD button\n");
	printf("   Moving the mouse cursor to RIGHT = FWD button\n");
	printf("   Sending RIGHT CLICK Command = MODE button\n");
	printf("   Sending LEFT CLICK Command = MENU button\n");
	printf("   Stopping the Test Program = STOP button\n");
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
		printf("USB_disconnectDev() API failed\n");
	}

#ifdef C5545_BSTPCK
    status = GPIO_closeInstance();
	if(status != CSL_SOK)
	{
		printf("GPIO_closeInstance() API failed\n");
	}
#else
    status = SAR_closeInstance();
	if(status != CSL_SOK)
	{
		printf("SAR_closeInstance() API failed\n");
	}
#endif

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
		printf("HID_Open failed\n");
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


