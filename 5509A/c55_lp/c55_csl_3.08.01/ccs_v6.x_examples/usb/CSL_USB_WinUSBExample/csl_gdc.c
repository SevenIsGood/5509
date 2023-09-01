/*
 * csl_gdc.c
 *
 *  Created on: Feb 13, 2015
 *      Author: M1009381
 */

/* Generic Device Class */
/** @file csl_gdc.c
 *
 *  @brief USB Generic Device Class (GDC) functional layer API source file
 *
 *  Path: \(CSLPATH)\ src
 */

/* ============================================================================
 * Revision History
 * ================
 * 13-Feb-2015 Created
 * ============================================================================
 */

//#include <csl_cdc.h>
#include <csl_usb.h>
#include "csl_gdc.h"

//CSL_Status GDC_Ep_Setup(pCdcAppClassHandle    pAppClassHandle,
CSL_Status GDC_Ep_Setup(pGdcClassHandle    pHandle,
                            Bool                  usbSpeedCfg)
{
	//pCdcClassHandle       pHandle;
	CSL_Status            status;
	Uint16                eventMask;
	Uint16                maxPktSize;

	CSL_UsbEpConfig		usbEpConfig;

	status  = CSL_SOK;
	//pHandle = (pCdcClassHandle)(pAppClassHandle->pCdcObj);

	//if(pAppClassHandle != NULL)
	if(pHandle != NULL)
	{

		/* Initialized the Control Endpoint OUT 0 */
		eventMask = (CSL_USB_EVENT_RESET | CSL_USB_EVENT_SETUP |
		             CSL_USB_EVENT_SUSPEND | CSL_USB_EVENT_RESUME |
		             CSL_USB_EVENT_RESET | CSL_USB_EVENT_EOT);

		if(usbSpeedCfg == TRUE)
		{
			maxPktSize = CSL_USB_EP1_PACKET_SIZE_HS;
		}
		else
		{
			maxPktSize = CSL_USB_EP1_PACKET_SIZE_FS;
		}


		pHandle->ctrlHandle.ctrlOutEpHandle = USB_requestEndpt (pHandle->usbDevHandle, CSL_USB_OUT_EP0, &status);
		if (CSL_SOK != status)
		{
			return (status);
		}
		usbEpConfig.opMode = CSL_USB_OPMODE_POLLED;
		usbEpConfig.epNum = CSL_USB_OUT_EP0;
		usbEpConfig.xferType = CSL_USB_CTRL;
		usbEpConfig.maxPktSize = CSL_USB_EP0_PACKET_SIZE;
		usbEpConfig.eventMask = eventMask;
		usbEpConfig.dblPktBuf = FALSE;
		usbEpConfig.asyncTxfer = FALSE;
		usbEpConfig.fxn = NULL;

		/*status = USB_initEndptObj(CSL_USB0,
		                          &pHandle->ctrlHandle.ctrlOutEpObj,
 				                  CSL_USB_OUT_EP0,
 				  				  CSL_USB_CTRL,
				  				  CSL_USB_EP0_PACKET_SIZE,
				  				  eventMask,
				  				  NULL);
		*/
		status = USB_configEndpt(pHandle->ctrlHandle.ctrlOutEpHandle,
								&usbEpConfig);
		if(status != CSL_SOK)
		{
			return (status);
		}

		pHandle->ctrlHandle.ctrlInEpHandle = USB_requestEndpt (pHandle->usbDevHandle, CSL_USB_IN_EP0, &status);
		if (CSL_SOK != status)
		{
			return (status);
		}
		usbEpConfig.epNum = CSL_USB_IN_EP0;
		usbEpConfig.xferType = CSL_USB_CTRL;
		usbEpConfig.maxPktSize = CSL_USB_EP0_PACKET_SIZE;
		usbEpConfig.eventMask = CSL_USB_EVENT_EOT;
		/* Initialized the Control Endpoint IN 0 */
		/*status = USB_initEndptObj(CSL_USB0,
				                  &pHandle->ctrlHandle.ctrlInEpObj,
				                  CSL_USB_IN_EP0,
				                  CSL_USB_CTRL,
				                  CSL_USB_EP0_PACKET_SIZE,
				                  CSL_USB_EVENT_EOT,
				                  NULL);
		 */
		status = USB_configEndpt(pHandle->ctrlHandle.ctrlInEpHandle,
										&usbEpConfig);
		if(status != CSL_SOK)
		{
			return (status);
		}



		pHandle->gdcHandle.bulkOutEpHandle = USB_requestEndpt (pHandle->usbDevHandle, (CSL_UsbEpNum)CSL_GDC_BULK_OUT_EP, &status);
		if (CSL_SOK != status)
		{
			return (status);
		}
		usbEpConfig.epNum = (CSL_UsbEpNum)CSL_GDC_BULK_OUT_EP;
		usbEpConfig.xferType = CSL_USB_BULK;
		usbEpConfig.maxPktSize = maxPktSize;
		usbEpConfig.eventMask = CSL_USB_EVENT_EOT;//(CSL_USB_EVENT_RESET | CSL_USB_EVENT_EOT);
		/* Initialized the Bulk Endpoint OUT */
		/*status = USB_initEndptObj(CSL_USB0,
				                  //&pHandle->cdcHandle.bulkOutEpObj,
								  &pHandle->gdcHandle.bulkOutEpObj,
				                  (CSL_UsbEpNum)CSL_GDC_BULK_OUT_EP,
				                  CSL_USB_BULK,
				                  maxPktSize,
				                  CSL_USB_EVENT_EOT,
				                  NULL);
		*/
		status = USB_configEndpt(pHandle->gdcHandle.bulkOutEpHandle,
												&usbEpConfig);
		if(status != CSL_SOK)
		{
			return (status);
		}



		pHandle->gdcHandle.bulkInEpHandle = USB_requestEndpt (pHandle->usbDevHandle,(CSL_UsbEpNum)(CSL_GDC_BULK_IN_EP), &status);
		if (CSL_SOK != status)
		{
			return (status);
		}
		eventMask = (CSL_USB_EVENT_RESET | CSL_USB_EVENT_EOT);
		usbEpConfig.epNum = (CSL_UsbEpNum)(CSL_GDC_BULK_IN_EP);
		usbEpConfig.xferType = CSL_USB_BULK;
		usbEpConfig.maxPktSize = maxPktSize;
		usbEpConfig.eventMask = eventMask;//CSL_USB_EVENT_EOT;
		/* Initialized the Bulk Endpoint IN */
		/*status = USB_initEndptObj(CSL_USB0,
				                  //&pHandle->cdcHandle.bulkInEpObj,
								  &pHandle->gdcHandle.bulkInEpObj,
				                  (CSL_UsbEpNum)(CSL_GDC_BULK_IN_EP+CSL_USB_IN_EP0),
				                  CSL_USB_BULK,
				                  maxPktSize,
				                  eventMask,
				                  NULL);
		*/
		status = USB_configEndpt(pHandle->gdcHandle.bulkInEpHandle,
												&usbEpConfig);
		if(status != CSL_SOK)
		{
			return (status);
		}

	}
	else
	{
		status = CSL_ESYS_BADHANDLE;
	}

	return(status);
}

