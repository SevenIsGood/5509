/*
 * csl_gdc.h
 *
 *  Created on: Feb 13, 2015
 *      Author: M1009381
 */
/** @file csl_gdc.h
 *
 *  @brief  USB Generic Device Class (GDC) functional layer API header file
 *
 *  Path: \(CSLPATH)\ inc
 */

/* ============================================================================
 * Revision History
 * ================
 * 13-Feb-2015 Created
 * ============================================================================
 */

#ifndef CSL_GDC_H_
#define CSL_GDC_H_

#define CSL_GDC_INTR_IN_EP									CSL_USB_EP1
#define CSL_GDC_BULK_OUT_EP									CSL_USB_OUT_EP2//CSL_USB_EP2
#define CSL_GDC_BULK_IN_EP									CSL_USB_IN_EP3//CSL_USB_EP3

#define CSL_USB_GDC_DATA_BUF_SIZE							512

typedef struct CSL_GdcObject {
    CSL_UsbEpHandle          bulkInEpHandle;
    /** \brief  Bulk Out Endpoint Object                                     */
	CSL_UsbEpHandle          bulkOutEpHandle;
    /** \brief  Bulk In Endpoint Object                                      */
    CSL_UsbEpHandle          intrInEpHandle;
    /** \brief  Bulk Out Endpoint Object                                     */
	CSL_UsbEpHandle          intrOutEpHandle;
} CSL_GdcObject;

typedef struct CSL_GdcCtrlObject {
	/** \brief  Control In Endpoint Object                                   */
	CSL_UsbEpHandle             ctrlInEpHandle;
	/** \brief  Object of Type Device Number                                 */
	CSL_UsbDevNum            devNum;
	/** \brief  Control Out Endpoint Object                                  */
	CSL_UsbEpHandle             ctrlOutEpHandle;
} CSL_GdcCtrlObject;

/**
 *  \brief GDC Control Object structure
 *
 *  Holds all the components for the GDC Control Object
 */
typedef struct CSL_GdcClassStruct {
	/** \brief Handle to the selected USB Device instance                     */
	CSL_UsbDevHandle    usbDevHandle;
	/** \brief  Handle to Control Object                                      */
	CSL_GdcCtrlObject    ctrlHandle;
	/** \brief  Handle to CDC Transfer Object                                */
	CSL_GdcObject        gdcHandle;
} CSL_GdcClassStruct;

/** \brief  CDC class handle                                                  */
typedef CSL_GdcClassStruct  *pGdcClassHandle;

extern CSL_UsbContext     gUsbContext;
extern CSL_UsbRegsOvly    usbRegisters;

//CSL_Status GDC_Ep_Setup(pCdcAppClassHandle    pAppClassHandle,
CSL_Status GDC_Ep_Setup(pGdcClassHandle    pHandle,
                               Bool                  usbSpeedCfg);

#endif /* CSL_GDC_H_ */
