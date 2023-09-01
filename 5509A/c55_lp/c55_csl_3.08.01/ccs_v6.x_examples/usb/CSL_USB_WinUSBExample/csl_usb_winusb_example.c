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


/** @file csl_usb_winusb_example.c
 *
 *  @brief USB functional layer Generic Device Class WinUSB example source file
 *
 *
 * \page    page20  CSL USB EXAMPLE DOCUMENTATION
 *
 * \section USB10   USB EXAMPLE10 - WINUSB EXAMPLE
 *
 * \subsection USB10x    TEST DESCRIPTION:
 * This test is to verify the operation of the CSL USB module's WinUSB Generic
 * Device Class (GDC) driver.This test runs in interrupt mode.
 * USB interrupts are configured and ISR is  registered using
 * CSL INTC module. After initializing and configuring the USB module test waits
 * on a while loop. When there is any request from the USB host application
 * USB ISR is triggered and the requested operation is performed inside the ISR.
 *
 * There are 3 endpoints of concern:
 * 1 Control Endpoint 0
 * 1 Bulk IN Endpoint 3
 * 1 Bulk OUT Endpoint 2
 *
 * A 320x200 bytes image is stored in "image.h" file. When a command is sent
 * from the PC host to the C55 to send an image chunk (command: 0xAA followed
 * by chunk ID - 1 byte for 0xAA, 2 bytes for chunk ID), the same is sent
 * across and when all chunks have been received, the image is displayed on
 * the PC host using the host USB tool.
 *
 * This USB test is verified by a host USB tool which can send/receive
 * data to/from the USB device. This tool should be installed on the host PC
 * for evaluating the CSL USB end point examples.
 * Please contact TI for more details on the USB host tool
 *
 *
 * USB enumeration as a Generic Device Class WinUSB device requires a specific
 * inf file - 'ccs_v6.x_examples/usb/CSL_USB_WinUSBExample/C5517_WINUSB.inf'
 * Commands are received on BULK OUT endpoint 2 and Image is sent out via
 * BULK IN endpoint 3.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSION C5515/35/45/17.
 * MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection USB10y    TEST PROCEDURE:
 * @li Open the CCS and connect the target (C5515/17 EVMs, C5535 EZDSP or
 *     C5545 BoosterPack)
 * @li Open, build and load the CSL_USB_WinUSBExample_Out to the target
 * @li Set the PLL frequency to 100MHz
 * @li Connect the USB device port on target to a Win7 PC (USB Host)
 * @li Run the program loaded on to the target
 * @li When the new USB device is discovered by the USB host, use C5517_WINUSB.inf
 *     to update the new USB device
 * @li The newly discovered USB device will be enumerated as C5517_WINUSB
 * @li Run the USB host application(WinUsbTest.exe) on the Win7 PC.
 *     It displays a dialog window (WINUSB Test Interface)
 * @li In the USB Device GUID field, type in 9f543223-cede-4fa3-b376-a25ce9a30e74
 *     and click 'Find' tab.
 * @li Once the device is detected, 'Attached' is displayed below the 'Find' tab.
 *     Until then, it displays 'Detached'.
 * @li Once the C55xx EVM is 'Attached', Click 'Image Test' tab.
 * @li A vertical bar on a white background is displayed under 'Received Image'.
 * @li The bar sweeps from the left edge to the right edge and then starts sweeping
 *     all over from left edge to the right again. It goes on continuously.
 * @li Clicking the 'Image Test' tab again stops the moving bar.
 * @li Click the 'Image Test' tab once more and the vertical bar resumes the sweep
 *     from where it last stopped.
 * @li The vertical bar also changes colour randomly.
 *
 *
 * \subsection USB10z    TEST RESULT:
 * @li USB device(C5517_WINUSB) should be detected by the host PC and should be accessible
 *     through the USB host application
 * @li Image sent should be properly received and displayed by the PC host
 *     USB tool
 *
 */

/* ============================================================================
 * Revision History
 * ================
 * 30-Apr-2015 Created
 * 10-Mar-2016 Updates to header
 * 02-May-2016 Updates to doxygen header
 * ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include "soc.h"
#include "image.h" // stores the image

#include "csl_usb.h"
#include "csl_gdc.h"
#include "csl_usbAux.h"
#include "csl_intc.h"
#include "csl_general.h"

#define CSL_USB_TEST_PASSED   (0)
#define CSL_USB_TEST_FAILED   (1)

#define CSL_USB_DATA_SIZE     (256)
#define CSL_USB_MAX_CURRENT   (50)
#define CSL_USB_WAKEUP_DELAY  (10)

extern CSL_UsbContext     gUsbContext;
extern CSL_UsbRegsOvly    usbRegisters;

CSL_GdcClassStruct    gdcClassStruct;

CSL_UsbEpHandle       hEp[CSL_USB_ENDPOINT_COUNT+2];
CSL_UsbEpHandle       hEPx;
CSL_UsbConfig         usbConfig;
CSL_UsbSetupStruct    usbSetup;
CSL_UsbBoolean        txRxStatus;
CSL_Status            status;
pUsbContext           pContext = &gUsbContext;


Uint16    outEp2;
Uint16    inEp3;
Uint16    usbDataBuffer1[CSL_USB_DATA_SIZE];

Uint16    usbDataBuffer2[3];

volatile  Uint8     Byte[3];

Uint16    usb_income_num_bytes_ep2;
Uint16    usb_income_num_bytes_ep3 = 240;//0;

Uint16    *deviceDescPtr;
Uint16    *cfgDescPtr;
Uint16    *strDescPtr;
Uint16    *deviceQualDescPtr;
Uint16    *OtherSpeedcfgDescPtr;
Uint16    bytesRem;
Uint16    devAddr;
Uint16    saveIndex;
Uint16    endpt;
volatile Bool      stopRunning = FALSE;
Bool	  usbDevDisconnect = TRUE;



#define USB_VID 0x0451
#define USB_PID 0x9010
// Device Descriptor
Uint16    deviceDesc[9] =   {
    0x0112, // USB Device Descriptor type| descroptor size
    0x0200, // USB Spec Release Number in BCD format
    0x0000, // Generic usb device
    0x4000, // Maximum packet size for EP0 USB 64 | Protocol code
    USB_VID,
    USB_PID,
    0x0100, // Device release number in BCD format
    0x0201, // Product string index | Manufacturer string index
    0x0103  // Number of Configuration | Device serial number string index
};

/*Rajesh: This is anyway same as in the sample.. need not worry about it*/
Uint16    deviceQualDesc[5] = {0x060A, 0x0200, 0x0000, 0x4000, 0x0001};

// Configuration Descriptor for USB HighSpeed
Uint16    cfgDesc[40] = {
    // Configure descriptor
    0x0209, // USB configure descriptor type | size of configuration descriptor
    0x0020, // Total length of data for this configuration
    0x0101, // Index value of this configuration | number of interfaces in this configuration (Rajesh: We need only one interface to define both bulkin and bulkout?)
    0xC003, // Attributs (Bus-Powered and Self-Powered) | Configuration string index
    0x0932, // Size of interface 0 | max power consumption (2X mA)
    // Interface 0 descriptor
    0x0004, // Interface Number | Interface descriptor type
    0x0200, // Number of endpoints in this interface | Alternate setting number
    0x0000, // Sub-class code | Class code
    0x0000, // Interface string index | Protocol code
    // Endpoint descriptor for data interface (DATA EP IN)
    0x0507, // USB endpoint descriptor type | Endpoint descriptor size
    0x0283, //0x0282//0x0281, Attributes (2: Bulk) | endpoint 1 BULK IN
    0x0200, // Maximum packet size 512
    0x0700, // Endpoint descriptor size | Polling interval
    // Endpoint descriptor for data interface (BULK EP OUT)
    0x0205, // 0x0105,BULK EP OUT | USB endpoint descriptor type
    0x0002, // Maximum packet size 512 (lower byte) | Attributes (2: Bulk)
    0x0002  // Polling interval | Maximum packet size 512 (higher byte)
};

//#ifdef FULL_SPEED
// Configuration Descriptor for USB FullSpeed
//Uint16    cfgDescFS[40] =   {
Uint16    OtherSpeedcfgDesc[40] =   {
    // Configure descriptor
    0x0709, // USB configure descriptor type | size of configuration descriptor
    0x0020, // Total length of data for this configuration
    0x0101, // Index value of this configuration | number of interfaces in this configuration (Rajesh: We need only one interface to define both bulkin and bulkout?)
    0xC003, // Attributs (Bus-Powered and Self-Powered) | Configuration string index
    0x0932, // Size of interface 0 | max power consumption (2X mA)
    // Interface 0 descriptor
    0x0004, // Interface Number | Interface descriptor type
    0x0200, // Number of endpoints in this interface | Alternate setting number
    0x0000, // Sub-class code | Class code
    0x0000, // Interface string index | Protocol code
    // Endpoint descriptor for data interface (DATA EP IN)
    0x0507, // USB endpoint descriptor type | Endpoint descriptor size
    0x0283, // Attributes (2: Bulk) | endpoint 3 BULK IN
    0x0040, // Maximum packet size 64
    0x0700, // Endpoint descriptor size | Polling interval
    // Endpoint descriptor for data interface (BULK EP OUT)
    0x0105, // BULK EP OUT | USB endpoint descriptor type
    0x4002, // Maximum packet size 64 (lower byte) | Attributes (2: Bulk)
    0x0000  // Polling interval | Maximum packet size 64 (higher byte)
};
//#endif

Uint16    strDesc[4][20] = {
    // string 0 English-USA
    {0x0304, 0x0409},
    // string 1 "Texas Instruments"
    {0x0324, 0x0054, 0x0045, 0x0058, 0x0041, 0x0053, 0x0020, 0x0049,0x004E,
     0x0053, 0x0054, 0x0052, 0x0055, 0x004D, 0x0045, 0x004E, 0x0054, 0x0053},
#if (defined(CHIP_C5535) || defined(CHIP_C5545))
    // string 2 "C5535"
    {0x030C, 0x0043, 0x0035, 0x0035, 0x0033, 0x0035},
#elif (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514))
    // string 2 "C5515"
    {0x030C, 0x0043, 0x0035, 0x0035, 0x0031, 0x0035},
#elif (defined(CHIP_C5517))
    // string 2 "C5517"
    {0x030C, 0x0043, 0x0035, 0x0035, 0x0031, 0x0037},
#else
    // string 2 "C5505"
    {0x030C, 0x0043, 0x0035, 0x0035, 0x0030, 0x0035},
#endif
    // string 3 "00001"
    {0x030C, 0x0030, 0x0030, 0x0030, 0x0030, 0x0031}
};

Uint16 SR_ODD_BYTE_FLAG = 0;

extern void VECSTART(void);
CSL_Status CSL_usbIntcTest(void);
interrupt void usb_isr(void);
void CSL_suspendCallBack(CSL_Status    status);
void CSL_selfWakeupCallBack(CSL_Status    status);
static void USB_delay(Uint32    mSecs);
CSL_Status CSL_startTransferCallback(void    *vpContext,
                                     void    *vpeps);
CSL_Status CSL_completeTransferCallback(void    *vpContext,
                                        void    *vpeps);


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

	printf("USB Interrupt Test!!\n");


	result = CSL_usbIntcTest();

	if(result == CSL_USB_TEST_PASSED)
	{
		printf("USB Test Passed!!\n");
	}
	else
	{
		printf("USB Test Failed!!\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
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

Uint16  gConfiguration = 0;
Bool    sentLongEp0Pkt = 0;

/**
 *  \brief  Tests USB interrupt mode operation
 *
 *  \param  none
 *
 *  \return Test result
 */
extern Uint16 tracker=0;
volatile Uint16 i=0,j=0,k=0;

CSL_Status CSL_usbIntcTest(void)
{
	CSL_IRQ_Config    config;
	CSL_Status        result;


    // clear the GDC class structure
    memset((void*)&gdcClassStruct, 0x00, sizeof(gdcClassStruct));

	result = CSL_USB_TEST_FAILED;

	*(ioport volatile int *)0x1c02 = 0x0;
	*(ioport volatile int *)0x1c03 = 0x0;

	usbConfig.opMode             = CSL_USB_OPMODE_POLLED;
	usbConfig.maxCurrent         = CSL_USB_MAX_CURRENT;
	usbConfig.appSuspendCallBack = (CSL_USB_APP_CALLBACK)CSL_suspendCallBack;
	usbConfig.appWakeupCallBack  = (CSL_USB_APP_CALLBACK)CSL_selfWakeupCallBack;
	usbConfig.startTransferCallback  = CSL_startTransferCallback;
	usbConfig.completeTransferCallback = CSL_completeTransferCallback;

	outEp2   = CSL_USB_OUT_EP2;

	inEp3    = CSL_USB_IN_EP3;


	/* Set the interrupt vector start address */
	IRQ_setVecs((Uint32)(&VECSTART));

	/* Plug the USB Isr into vector table */
	config.funcAddr = &usb_isr;
	IRQ_plug(USB_EVENT, config.funcAddr);

	/* Enable USB Interrupts */
	IRQ_enable(USB_EVENT);
	/* Enable CPU Interrupts */
	IRQ_globalEnable();

	/* Initialize the USB module */
	USB_init();



    gdcClassStruct.usbDevHandle = USB_open(CSL_USB0);
    if(gdcClassStruct.usbDevHandle == NULL)
    {
    	printf("USB Open failed");
        return (CSL_ESYS_BADHANDLE);
    }

    status = USB_config(gdcClassStruct.usbDevHandle, &usbConfig);
    if(status != CSL_SOK)
    {
    	printf("USB config failed");
        return(status);
    }

    /* Reset the USB device */
    status = USB_resetDev(gdcClassStruct.usbDevHandle);
    if(status != CSL_SOK)
    {
    	printf("USB Reset failed");
        return(status);
    }



    // Setup a GDC handle
    status = GDC_Ep_Setup(&gdcClassStruct, TRUE);
    if(status != CSL_SOK)
    {
        if(status == CSL_ESYS_BADHANDLE)
        {
        	printf("GDC Endpoint Setup Failed!");
            printf("GDC_Ep_Setup Returned - BAD HANDLE");
        } else
        {
        	printf("GDC_Ep_Setup Failed");
        }

        return(status);
    }


    hEp[0] = gdcClassStruct.ctrlHandle.ctrlOutEpHandle;
    hEp[1] = gdcClassStruct.ctrlHandle.ctrlInEpHandle;
    hEp[2] = gdcClassStruct.gdcHandle.bulkOutEpHandle;
    hEp[3] = gdcClassStruct.gdcHandle.bulkInEpHandle;


    /* Set the parameters */
    status = USB_setParams(gdcClassStruct.usbDevHandle, hEp, FALSE);
    if(status != CSL_SOK)
    {
    	printf("USB Set params failed");
        return(status);
    }

    /* Connect the USB device */
    status = USB_connectDev(gdcClassStruct.usbDevHandle);
    if(status != CSL_SOK)
    {
    	printf("USB Connect failed");
        return(status);
    }


	deviceDescPtr = (Uint16 *)deviceDesc;
	cfgDescPtr    = (Uint16 *)cfgDesc;
	strDescPtr    = (Uint16 *)strDesc;
    deviceQualDescPtr = (Uint16 *)deviceQualDesc;
    OtherSpeedcfgDescPtr = (Uint16 *)OtherSpeedcfgDesc;



	while(stopRunning != TRUE)
	{
		if ((usbDevDisconnect == TRUE) &&
			(CSL_FEXT(usbRegisters->DEVCTL, USB_DEVCTL_VBUS) !=
			CSL_USB_DEVCTL_VBUS_ABOVEVBUSVALID))
		{
			printf("\nUSB Cable Disconnected!!\n");

			while(CSL_FEXT(usbRegisters->DEVCTL, USB_DEVCTL_VBUS) !=
			     CSL_USB_DEVCTL_VBUS_ABOVEVBUSVALID);

			printf("USB Cable Connection Detected!!\n");

			usbDevDisconnect = FALSE;
			USB_connectDev(gdcClassStruct.usbDevHandle);
		}


	}

	result = CSL_USB_TEST_PASSED;
	return(result);
}

/**
 *  \brief  USB interrupt service routine
 *
 *  \param  None
 *
 *  \return None
 */
interrupt void usb_isr(void)
{
	CSL_UsbEpHandle tempEpH;
	Uint16          tempLen;

	/* Read the masked interrupt status register */
	pContext->dwIntSourceL = usbRegisters->INTMASKEDR1;
	pContext->dwIntSourceH = usbRegisters->INTMASKEDR2;

	/* Clear the interrupts */
	if(pContext->dwIntSourceL != FALSE)
	{
		usbRegisters->INTCLRR1 = pContext->dwIntSourceL;
	}

	if(pContext->dwIntSourceH != FALSE)
	{
		usbRegisters->INTCLRR2 = pContext->dwIntSourceH;
	}

	if(!SR_ODD_BYTE_FLAG)
	{
	if((pContext->dwIntSourceL != FALSE) || (pContext->dwIntSourceH != FALSE))
	{
		/* Reset interrupt */
		if(pContext->dwIntSourceH & CSL_USB_GBL_INT_RESET)
		{
			saveIndex = usbRegisters->INDEX_TESTMODE;

			for(endpt = CSL_USB_EP1; endpt <= CSL_USB_EP4; endpt++)
			{
				CSL_FINS(usbRegisters->INDEX_TESTMODE,
						 USB_INDEX_TESTMODE_EPSEL, endpt);

				CSL_FINS(usbRegisters->PERI_RXCSR_INDX,
						 USB_PERI_RXCSR_INDX_DISNYET, TRUE);
			}

			usbRegisters->INDEX_TESTMODE = saveIndex;
		}

		/* Resume interrupt */
		if(pContext->dwIntSourceH & CSL_USB_GBL_INT_RESUME)
		{
			USB_setRemoteWakeup(gdcClassStruct.usbDevHandle, CSL_USB_TRUE);
			status = USB_issueRemoteWakeup(gdcClassStruct.usbDevHandle, TRUE);
			/* Give 10 msecs delay before resetting resume bit */
			USB_delay(CSL_USB_WAKEUP_DELAY);
			status = USB_issueRemoteWakeup(gdcClassStruct.usbDevHandle, FALSE);
			if(status != CSL_SOK)
			{
				printf("USB Resume failed\n");
			}
		}

		/* Check end point0 interrupts */
		if(pContext->dwIntSourceL & CSL_USB_TX_RX_INT_EP0)
		{
			if(sentLongEp0Pkt == 1)
			{
				USB_processEP0In(pContext);
			}
			else
			{
				saveIndex = usbRegisters->INDEX_TESTMODE;
					CSL_FINS(usbRegisters->INDEX_TESTMODE,
							 USB_INDEX_TESTMODE_EPSEL, CSL_USB_EP0);

				USB_getSetupPacket(gdcClassStruct.usbDevHandle, &usbSetup, TRUE);

				if((usbRegisters->PERI_CSR0_INDX & CSL_USB_PERI_CSR0_INDX_RXPKTRDY_MASK)
					== CSL_USB_PERI_CSR0_INDX_RXPKTRDY_MASK)
				{
					/* Service the RXPKTRDY after reading the FIFO */
					CSL_FINS(usbRegisters->PERI_CSR0_INDX,
					         USB_PERI_CSR0_INDX_SERV_RXPKTRDY, TRUE);

					/* GET DESCRIPTOR Req */
					switch(usbSetup.bRequest)
					{
						/* zero data */
						case CSL_USB_SET_FEATURE:
							switch(usbSetup.wValue)
							{
								case CSL_USB_FEATURE_ENDPOINT_STALL:
									/* updated set and clear endpoint stall
									 * to work with logical endpoint num
									 */
									endpt = (usbSetup.wIndex) & 0xFF;
									hEPx = USB_epNumToHandle(gdcClassStruct.usbDevHandle, endpt);
									if(!(USB_getEndptStall(hEPx, &status)))
									{
										USB_stallEndpt(hEPx);
									}
									break;

								case CSL_USB_FEATURE_REMOTE_WAKEUP:
									if(!(USB_getRemoteWakeupStat(gdcClassStruct.usbDevHandle)))
									{
										USB_setRemoteWakeup(gdcClassStruct.usbDevHandle, CSL_USB_TRUE);
									}
									break;

								default:
									break;
							}

							CSL_FINS(usbRegisters->PERI_CSR0_INDX,
							         USB_PERI_CSR0_INDX_SERV_RXPKTRDY, TRUE);
							CSL_FINS(usbRegisters->PERI_CSR0_INDX,
							         USB_PERI_CSR0_INDX_DATAEND, TRUE);
							break;

						case CSL_USB_CLEAR_FEATURE:
							switch(usbSetup.wValue)
							{
								case CSL_USB_FEATURE_ENDPOINT_STALL:
									endpt = (usbSetup.wIndex) & 0xFF;
									hEPx = USB_epNumToHandle(gdcClassStruct.usbDevHandle, endpt);
									if(USB_getEndptStall(hEPx, &status))
									{
										USB_clearEndptStall(hEPx);
									}
									break;

								case CSL_USB_FEATURE_REMOTE_WAKEUP:
									if(USB_getRemoteWakeupStat(gdcClassStruct.usbDevHandle))
									{
										USB_setRemoteWakeup(gdcClassStruct.usbDevHandle,
										                    CSL_USB_FALSE);
									}
									break;

								default:
									 break;
							}
							break;

						case CSL_USB_SET_CONFIGURATION :
						case CSL_USB_SET_INTERFACE:
							endpt = (usbSetup.wIndex) & 0xFF;
							hEPx = USB_epNumToHandle(gdcClassStruct.usbDevHandle, endpt);
							USB_postTransaction(hEPx, 0, NULL,
							                    CSL_USB_IOFLAG_NONE);

							/* DataEnd + ServicedRxPktRdy */
							usbRegisters->PERI_CSR0_INDX |= (CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK |
															 CSL_USB_PERI_CSR0_INDX_DATAEND_MASK);

							gConfiguration = usbSetup.wValue;

							break;

						case CSL_USB_GET_CONFIGURATION :
						case CSL_USB_GET_INTERFACE:
							usbDataBuffer1[0] = gConfiguration;

							status = USB_postTransaction(hEp[1],
							                 1, usbDataBuffer1,
							                 CSL_USB_IN_TRANSFER);
							break;

						case CSL_USB_GET_DESCRIPTOR :

							switch(usbSetup.wValue >> 8)
							{
								case CSL_USB_DEVICE_DESCRIPTOR_TYPE:
									deviceDescPtr = (Uint16 *)deviceDesc;
									status = USB_postTransaction(hEp[1],
									                 deviceDesc[0]&0xFF, deviceDescPtr,
									                 CSL_USB_IN_TRANSFER);
									break;

						    	case CSL_USB_OTHERSPEED_CFG_DESCRIPTOR_TYPE:
									if(usbSetup.wLength == 0x0009)
									{
										status = USB_postTransaction(hEp[1],
										                 9, OtherSpeedcfgDescPtr,
										                 CSL_USB_IN_TRANSFER);
									}
									else
									{
										if((OtherSpeedcfgDescPtr[1] & 0xFF) > 0x40)
										{
											sentLongEp0Pkt = 1;
										}

										status = USB_postTransaction(hEp[1],
										                 OtherSpeedcfgDescPtr[1], OtherSpeedcfgDescPtr,
										                 CSL_USB_IN_TRANSFER);
									}
						         	break;

								case CSL_USB_CONFIGURATION_DESCRIPTOR_TYPE:
									if(usbSetup.wLength == 0x0009)
									{
										cfgDescPtr = cfgDesc;
										status = USB_postTransaction(hEp[1],
										                 9, cfgDescPtr,
										                 CSL_USB_IN_TRANSFER);
									}
									else
									{
										if((cfgDesc[1] & 0xFF) > 0x40)
										{
											sentLongEp0Pkt = 1;
										}

										cfgDescPtr = cfgDesc;
										status = USB_postTransaction(hEp[1],
										                 cfgDesc[1], cfgDescPtr,
										                 CSL_USB_IN_TRANSFER);
									}

									break;

								case CSL_USB_STRING_DESCRIPTOR_TYPE:
									if((usbSetup.wValue & 0xFF) == 0x00)
									{
										strDescPtr = (Uint16 *)strDesc[0];
										status = USB_postTransaction(hEp[1],
										                 strDesc[0][0]&0xFF, strDescPtr,
										                 CSL_USB_IN_TRANSFER);
									}
									if((usbSetup.wValue & 0xFF) == 0x01)
									{
										strDescPtr = (Uint16 *)strDesc[1];
										status = USB_postTransaction(hEp[1],
										                 strDesc[1][0]&0xFF, strDescPtr,
										                 CSL_USB_IN_TRANSFER);
									}
									if((usbSetup.wValue & 0xFF) == 0x02)
									{
										strDescPtr = (Uint16 *)strDesc[2];
										status = USB_postTransaction(hEp[1],
										                 strDesc[2][0]&0xFF, strDescPtr,
										                 CSL_USB_IN_TRANSFER);
									}
									if((usbSetup.wValue & 0xFF) == 0x03)
									{
										strDescPtr = (Uint16 *)strDesc[3];
										status = USB_postTransaction(hEp[1],
										                 strDesc[3][0]&0xFF, strDescPtr,
										                 CSL_USB_IN_TRANSFER);
									}
									break;

								case CSL_USB_DEVICE_QUAL_DESCRIPTOR_TYPE:

						    	    tempLen = deviceQualDesc[0]&0xFF;
						    	    /* select the smaller of two  */
						            tempLen = (tempLen < usbSetup.wLength) ? tempLen : usbSetup.wLength;

									status = USB_postTransaction(hEp[1],
									                 tempLen, deviceQualDescPtr,
									                 CSL_USB_IN_TRANSFER);
									break;

								default:
                                    status = USB_stallEndpt(hEp[0]);
									break;
							}

							deviceDescPtr = (Uint16 *)deviceDesc;
							cfgDescPtr    = (Uint16 *)cfgDesc;
							strDescPtr    = (Uint16 *)strDesc[0];

							if(sentLongEp0Pkt != 1)
							{
							    usbRegisters->PERI_CSR0_INDX |= (CSL_USB_PERI_CSR0_INDX_TXPKTRDY_MASK |
															     CSL_USB_PERI_CSR0_INDX_DATAEND_MASK);
							}

							break;

						case CSL_USB_GET_STATUS:

							switch(usbSetup.bmRequestType)
							{
								/* Device Status to be returned */
								case CSL_USB_REQUEST_TYPE_DEVICE_STATUS:
									// return the remote wake up status and the self power status
									usbDataBuffer1[0] =
									(((Uint16)USB_getRemoteWakeupStat(gdcClassStruct.usbDevHandle))<<1)|((cfgDescPtr[3]>>14)&0x0001);
									USB_postTransaction(hEp[1], 2, usbDataBuffer1,
									                    CSL_USB_IOFLAG_NONE);
									break;

								/* Interface status is to be returned */
								case CSL_USB_REQUEST_TYPE_INTERFACE_STATUS:
									// return 0x0000 (reserved for future use)
									usbDataBuffer1[0] = 0x0000;
									USB_postTransaction(hEp[1], 2, usbDataBuffer1,
									                    CSL_USB_IOFLAG_NONE);
									break;

								/* Endpoint status to be returned */
								case CSL_USB_REQUEST_TYPE_ENDPOINT_STATUS:
									// get endpoint from setup packet
									endpt = (usbSetup.wIndex) & 0xFF;
									// get the endpoint handle
									hEPx   =  USB_epNumToHandle(gdcClassStruct.usbDevHandle, endpt);
									// return the stall status
									usbDataBuffer1[0] = (Uint16)USB_getEndptStall(hEPx, &status);
									USB_postTransaction(hEp[1], 2, usbDataBuffer1, CSL_USB_IOFLAG_NONE);
									break;

								default:
									status = USB_stallEndpt(hEp[0]);

									break;
							}

							break;

						case CSL_USB_SET_ADDRESS :
							devAddr = usbSetup.wValue;
							CSL_FINS(usbRegisters->PERI_CSR0_INDX,
							         USB_PERI_CSR0_INDX_SERV_RXPKTRDY, TRUE);
							CSL_FINS(usbRegisters->PERI_CSR0_INDX,
							         USB_PERI_CSR0_INDX_DATAEND, TRUE);
							break;

						default:
							break;
					}
				}
				else
				{
					if(usbSetup.bRequest == 0x05)
					{
						USB_setDevAddr(gdcClassStruct.usbDevHandle, devAddr);
					}
				}

				usbRegisters->INDEX_TESTMODE  = saveIndex;
            }
		}


		if(pContext->dwIntSourceL & CSL_USB_RX_INT_EP2)
		{


			// read the RX packet size
			// validate the RX packet first
			if (USB_isValidDataInFifoOut(&pContext->pEpStatus[CSL_USB_EP2]))
			{
				// get the EP2 handle
				tempEpH = USB_epNumToHandle(gdcClassStruct.usbDevHandle, CSL_USB_EP2);
				usb_income_num_bytes_ep2 = USB_getDataCountReadFromFifo(tempEpH);
			}


			status = USB_postTransaction(hEp[2], usb_income_num_bytes_ep2,
			                             usbDataBuffer2, CSL_USB_OUT_TRANSFER);
			if(status != CSL_SOK)
			{
				printf("\nUSB Transaction failed\n");
			}

			/* Check the transaction status */

			txRxStatus = USB_isTransactionDone(hEp[2], &status);
			if(txRxStatus != CSL_USB_TRUE)
			{

				bytesRem = USB_bytesRemaining(hEp[2], &status);
				if(bytesRem)
				{
					printf("\nBytes remaining: %d\n", bytesRem);
				}
			}


			Byte[0] = usbDataBuffer2[0] & 0xFF;
			Byte[1] = (usbDataBuffer2[0] & 0xFF00) >> 8;
			Byte[2] = usbDataBuffer2[1];


			if(Byte[0] == 0xAA)
			{
				if( Byte[1] == 0x00)
				{
					i=Byte[2];
				}
				else
				{
					i = (Uint16) (0x100 | Byte[2]);
				}
			}

			/*************************EP3 IN Txn initiation***********************************/
			/* Flush the FIFO to clean any data already present in the Tx FIFO */
						USB_flushFifo(CSL_USB_EP3, 1);


						if(i==0)
						{
							for(j=0;j<200;j++)
							{
								usbDataBuffer3[k][j]=0x0000;
								usbDataBuffer3[k+1][j]=0x0000;
								usbDataBuffer3[k+2][j]=0x0000;
							}
						}
						status = USB_postTransaction(hEp[3], 200,
								usbDataBuffer3[i], CSL_USB_IN_TRANSFER);


						if(status != CSL_SOK)
						{
							printf("\nUSB Transaction failed\n");
						}

						/* Check the transaction status */

						txRxStatus = USB_isTransactionDone(hEp[3], &status);
						if(txRxStatus != CSL_USB_TRUE)
						{

							bytesRem = USB_bytesRemaining(hEp[3], &status);
							if(bytesRem)
							{
								printf("\nBytes remaining: %d\n", bytesRem);
							}
						}

						if(i==319)
						{

							for(j=0;j<200;j++)
							{
								usbDataBuffer3[k][j]=0xFFFF;
								usbDataBuffer3[k+1][j]=0xFFFF;
								usbDataBuffer3[k+2][j]=0xFFFF;
							}

							k+=4;
							if((k+4)==320)
								k=0;
						}
						tracker++;


			/************************************************************/



		}


		if(pContext->dwIntSourceL & CSL_USB_TX_INT_EP3)
		{
			usbRegisters->INTCLRR1 = pContext->dwIntSourceL;
		}

		/* Connect interrupt */
		if(pContext->dwIntSourceH & CSL_USB_GBL_INT_DEVCONN)
		{
			status = USB_connectDev(gdcClassStruct.usbDevHandle);
			if(status != CSL_SOK)
			{
				printf("USB Connect failed\n");
			}
		}

		/* Disconnect interrupt */
		if(pContext->dwIntSourceH & CSL_USB_GBL_INT_DEVDISCONN)
		{
			status = USB_disconnectDev(gdcClassStruct.usbDevHandle);
			usbDevDisconnect = TRUE;
			if(status != CSL_SOK)
			{
				printf("USB Disconnect failed\n");
			}
		}

		/* Suspend interrupt */
		if(pContext->dwIntSourceH & CSL_USB_GBL_INT_SUSPEND)
		{
			status = USB_suspendDevice(gdcClassStruct.usbDevHandle);
			if(status != CSL_SOK)
			{
				printf("USB Suspend failed\n");
			}
		}
		}

		CSL_FINS(usbRegisters->EOIR, USB_EOIR_EOI_VECTOR, CSL_USB_EOIR_RESETVAL);
	}
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
	printf("\nUSB SUSPEND Callback\n");

}

/**
 *  \brief  USB self wakeup call back function
 *
 *  \param  none
 *
 *  \return Test result
 */
void CSL_selfWakeupCallBack(CSL_Status    status)
{
	printf("\nUSB Self Wakeup CallBack\n");
}

/**
 *  \brief  USB delay function
 *
 *  \param  mSecs - Delay in millisecs
 *
 *  \return None
 */
static void USB_delay(Uint32    mSecs)
{
	volatile Uint32    delay;
	volatile Uint32    msecCount;
	volatile Uint32    sysClk;  /* System clock value in KHz */

	sysClk = 100000;  /* It is assumed that system is running at 100MHz */

	for (msecCount = 0; msecCount < mSecs; msecCount++)
	{
		for (delay = 0; delay < sysClk; delay++)
		{
			asm ("\tNOP");
		}
	}
}


/**
 *  \brief  Start transfer call back function
 *
 *  \param  vpContext - USB context structure
 *  \param  vpeps     - End point status strucure pointer
 *
 *  \return CSL_Status
 */
CSL_Status CSL_startTransferCallback(void    *vpContext,
                                     void    *vpeps)
{
	pUsbContext      pContext;
	pUsbEpStatus     peps;
	pUsbTransfer     pTransfer;
	CSL_Status       status;

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

	if(pTransfer->dwFlags == CSL_USB_OUT_TRANSFER)
	{
		if(peps->dwEndpoint == CSL_USB_EP0)
		{
			status = USB_processEP0Out(pContext);
		}

		else if(peps->dwEndpoint == CSL_USB_EP2)
		{
			status = USB_handleRx(pContext, CSL_USB_EP2);
		}

	}
	else if(pTransfer->dwFlags == CSL_USB_IN_TRANSFER)
	{
		if(peps->dwEndpoint == CSL_USB_EP0 )
		{
			status = USB_processEP0In(pContext);
		}

		else if(peps->dwEndpoint == CSL_USB_EP3)
		{
			status = USB_handleTx(pContext, CSL_USB_EP3);
		}

	}
	else
	{
		status = CSL_ESYS_INVPARAMS;
	}

	return(status);
}


/**
 *  \brief  Complete transfer call back function
 *
 *  \param  vpContext - USB context structure
 *  \param  vpeps     - End point status structure pointer
 *
 *  \return CSL_Status
 */
CSL_Status CSL_completeTransferCallback(void    *vpContext,
                                        void    *vpeps)
{
	if(sentLongEp0Pkt == 1)
	{
		sentLongEp0Pkt = 0;
	}

	return(CSL_SOK);
}

