/**********************************************************************/
/* this file is the function for test the seeddec5416 driver and devic*/
/*	author lifeng Duan												  */
/*	Lt.d	SEED													  */
/**********************************************************************/
#include "stdafx.h"
#include "winioctl.h"
#include "stdio.h"
#include <conio.h>
#include <malloc.h>
#include <assert.h>
#include "ezusbsys.h"
#include "seeddecusb.h"

PCHAR StaticDriverName[10] = {"\\\\.\\decusb-0",
							  "\\\\.\\decusb-1",
							  "\\\\.\\decusb-2",
							  "\\\\.\\decusb-3",
							  "\\\\.\\decusb-4",
							  "\\\\.\\decusb-5",
							  "\\\\.\\decusb-6",
							  "\\\\.\\decusb-7",
							  "\\\\.\\decusb-8",
							  "\\\\.\\decusb-9"
};
const PCHAR StaticDriverName1 ="\\\\.\\decusb-1";

/*全局的驱动句柄*/
HANDLE  hDevice[15] = { NULL,NULL,NULL,NULL,NULL,
						NULL,NULL,NULL,NULL,NULL,
						NULL,NULL,NULL,NULL,NULL};
DWORD Sx2_devdesc[200];
DWORD Sx2_strdesc[200];
DWORD Sx2_configdesc[200];
DWORD Sx2_devqualdesc[200];

/*USB的接口信息*/
USBD_INTERFACE_INFORMATION Sx2InterfaceInfo;
PUSBD_INTERFACE_INFORMATION	pSx2InterfaceInfo = &Sx2InterfaceInfo;
PUSBD_PIPE_INFORMATION pPipe;

BOOLEAN	Sx2BulkdataTrans(PVOID bulkControl,
						 char *buffer,
						 int bufferSize,
						 int *recnBytes);
/**************************************************/
/* OpenDriver()		                              */
/* Purpose:                                       */ 
/*      Opens the device driver using symbolic    */
/*      name provided                             */
/* Input:                                         */
/*			none								  */
/* Return Value:                                  */
/*      Boolean that indicates if the driver was  */
/*      successfully opened or not.               */
/**************************************************/
BOOLEAN	OpenDriver(int Usbnum)
{
	/*确定打开哪个USB设备*/
	/*打开驱动程序*/
	hDevice[Usbnum] = CreateFile(StaticDriverName[Usbnum],/*驱动程序的符号连接*/
		                    GENERIC_READ | GENERIC_WRITE,
		                    FILE_SHARE_WRITE,
		                    NULL,
		                    OPEN_EXISTING,
		                    0,
		                    NULL);
    if (hDevice[Usbnum] == INVALID_HANDLE_VALUE) 
	{
        return (FALSE);
    }
	/*获得USB的接口信息*/
	Sx2GetPipeInfo(Usbnum,(PVOID)&Sx2InterfaceInfo);
	return (TRUE);
}
/**********************************************************************/
/* CloseDriver()													  */
/* Purpose:															  */ 
/*      Close the device driver using symbolic						  */
/* Input:															  */
/*			none													  */
/* Return Value:													  */
/*			none													  */
/**********************************************************************/
void CloseDriver(int Usbnum)
{
	// Close the handle
	CloseHandle (hDevice[Usbnum]);
	//清空状态区
	pSx2InterfaceInfo= NULL;
}
/**********************************************************************/
/*	get the device descriptor										  */
/*	purpose:														  */
/*			get the device descriptor for the CY7c68001			      */
/*	Input:															  */
/*			none													  */
/* Return Value:												      */
/*      the point for device descriptor								  */
/**********************************************************************/
PVOID Sx2GetDeviceDesc(int Usbnum)
{
	DWORD * pvBuffer = 0;
	int     nBytes   = 0;
	BOOLEAN bResult  = FALSE;
	int     i        = 0;
	// Get some memory, plus some guardband area
	pvBuffer = (DWORD *)malloc (sizeof (Usb_Device_Descriptor) + 128);
	// Perform the Get-Descriptor IOCTL
	bResult = DeviceIoControl ( hDevice[Usbnum],
								IOCTL_Ezusb_GET_DEVICE_DESCRIPTOR,
								pvBuffer,
								sizeof (Usb_Device_Descriptor),
								pvBuffer,
								sizeof (Usb_Device_Descriptor),
								(unsigned long *)&nBytes,
								NULL);
	if(bResult ==FALSE)
	{
		free (pvBuffer); 
		return NULL;
	}
	for(i =0;i<sizeof(Usb_Device_Descriptor);i++)
	{
		Sx2_devdesc[i] =pvBuffer[i];
	}
	// Free the memory
	free (pvBuffer); 
	return (PVOID)&Sx2_devdesc[0];
}


/**********************************************************************/
/*	get the string descriptor										  */
/*	purpose:														  */
/*			get the string descriptor for the CY7c68001			      */
/*	Input:															  */
/*			none													  */
/* Return Value:												      */
/*      the point for string descriptor								  */
/**********************************************************************/
PVOID Sx2GetStringDesc(int Usbnum,int stringindex,int languageID)
{
	DWORD * pvBuffer = 0;
	int     nBytes   = 0;
	BOOLEAN bResult  = FALSE;
	int     i        = 0;
	int   ulLength = 0;
	GET_STRING_DESCRIPTOR_IN input;
	input.Index = stringindex;
	// NOTE: frameworks ignores it anyway
	input.LanguageId = languageID; 
	
	// Get some memory, plus some guardband area
	pvBuffer = (DWORD *)malloc (sizeof (Usb_String_Descriptor) + 128);
	// The string descriptor is obtained using two separate calls.  
	// The first call is done to determine the size of the entire string descriptor,
	// The second call is done with that total size specified. 

	// Get the first bytes of the descriptor to determine the size of the entire descriptor.
	// Perform the Get-Descriptor IOCTL
	bResult = DeviceIoControl (	hDevice[Usbnum],
								IOCTL_Ezusb_GET_STRING_DESCRIPTOR,
								&input,
								sizeof (GET_STRING_DESCRIPTOR_IN),
								pvBuffer,
								sizeof (Usb_String_Descriptor),
								(unsigned long *)&nBytes,
								NULL);
	if(bResult ==FALSE)
	{
		free(pvBuffer);
		return NULL;
	}
	ulLength = GET_STRING_DESCRIPTOR_LENGTH(pvBuffer);
	/*
	assert (ulLength >= 0);
	// Now get the entire descriptor
	//重新分配内存
	pvBuffer = (DWORD *)realloc (pvBuffer, ulLength);
	// Perform the Get-Descriptor IOCTL
	bResult = DeviceIoControl ( hDevice[Usbnum],
								IOCTL_Ezusb_GET_STRING_DESCRIPTOR,
								&input,
								ulLength,
								pvBuffer,
								ulLength,
								(unsigned long *)&nBytes,
								NULL);
	if(bResult ==FALSE)
	{
		free (pvBuffer); 
		return NULL;
	}
	*/
	for(i =0;i<ulLength;i++)
	{
		Sx2_strdesc[i] =pvBuffer[i];
	}
	free (pvBuffer); // Free the memory
	return (PVOID)&Sx2_strdesc[0];
}
/**********************************************************************/
/*	get the configuration descriptor    							  */
/*	purpose:														  */
/*			get the configuration descriptor for the CY7c68001	      */
/*	Input:															  */
/*			none													  */
/* Return Value:												      */
/*      the point for string descriptor								  */
/**********************************************************************/
PVOID Sx2GetconfigDesc(int Usbnum)
{
	DWORD * pvBuffer = 0;
	int     nBytes   = 0;
	BOOLEAN bResult  = FALSE;
	int     i        = 0;
	int   ulLength = 0;
	// The configuration descriptor is obtained using two separate calls.  
	// The first call is done to determine the size of the entire configuration descriptor,
	// The second call is done with that total size specified.
	pvBuffer = (DWORD *)malloc (sizeof (Usb_Configuration_Descriptor) + 128);
	// Get the first bytes of the configuration descriptor to determine the size of
	// the entire configuration descriptor.
	// Perform the Get-Descriptor IOCTL
	bResult = DeviceIoControl (	hDevice[Usbnum],
								IOCTL_Ezusb_GET_CONFIGURATION_DESCRIPTOR,
								pvBuffer,
								sizeof (Usb_Configuration_Descriptor),
								pvBuffer,
								sizeof (Usb_Configuration_Descriptor),
								(unsigned long *)&nBytes,
								NULL);
	if(bResult ==FALSE)
	{
		free(pvBuffer);
		return NULL;
	}
	ulLength = GET_CONFIG_DESCRIPTOR_LENGTH(pvBuffer);
	assert (ulLength >= 0);
	
	// Now get the entire descriptor
	//重新分配内存
	pvBuffer = (DWORD *)realloc (pvBuffer, ulLength);
	// Perform the Get-Descriptor IOCTL
	bResult = DeviceIoControl ( hDevice[Usbnum],
								IOCTL_Ezusb_GET_CONFIGURATION_DESCRIPTOR,
								pvBuffer,
								ulLength,
								pvBuffer,
								ulLength,
								(unsigned long *)&nBytes,
								NULL);
	if(bResult ==FALSE)
	{
		free (pvBuffer); 
		return NULL;
	}
	
	//for(i =0;i<ulLength;i++)
	//for(i =0;i<200;i++)
	//{
		//Sx2_configdesc[i] =pvBuffer[i];
	//}
	free (pvBuffer); // Free the memory
	return (PVOID)pvBuffer;
}
/**********************************************************************/
/*	get the USB's interface 		    							  */
/*	purpose:														  */
/*				get the interface desc of the CY7c68001  		      */
/*	Input:															  */
/*			the interface's struct  								  */
/*																	  */
/* Return Value:												      */
/*      the point for string descriptor								  */
/**********************************************************************/
BOOLEAN Sx2GetPipeInfo(int Usbnum,PVOID pInterface)
{
	BOOLEAN bResult  = FALSE;
	int     nBytes   = 0;

	bResult = DeviceIoControl ( hDevice[Usbnum],
								IOCTL_Ezusb_GET_PIPE_INFO,
								NULL,
								0,
								pInterface,
								sizeof(USBD_INTERFACE_INFORMATION),
								(unsigned long *)&nBytes,
								NULL);
	if(bResult ==FALSE)
	{ 
		return FALSE;
	}
	return TRUE;
}
/**********************************************************************/
/*	send the vendor request			    							  */
/*	purpose:														  */
/*				send the vendor request to the CY7c68001		      */
/*	Input:															  */
/*			the point of myRest struct								  */
/*																	  */
/* Return Value:												      */
/*      the point for string descriptor								  */
/**********************************************************************/
BOOLEAN Sx2SendVendorReq(int Usbnum,
						 PVOID myRequest,
						 char *buffer,
						 int bufferSize,
						 int *recnBytes)
{
	int nBytes = 0;
	bool bResult = FALSE;
	bResult = DeviceIoControl (	hDevice[Usbnum],
								IOCTL_EZUSB_VENDOR_OR_CLASS_REQUEST,
								myRequest,
								sizeof(VENDOR_OR_CLASS_REQUEST_CONTROL),
								buffer,
								bufferSize,
								(unsigned long *)&nBytes,
								NULL);
	if(bResult == FALSE)
	{
		return FALSE;
	}
	*recnBytes = nBytes;
	return TRUE;
}
/**********************************************************************/
/*	bulk date translate  			    							  */
/*	purpose:														  */
/*				stranslate the data between PC and the CY7c68001      */
/*	Input:															  */
/*			the point of myRest struct								  */
/*																	  */
/* Return Value:												      */
/*      the point for string descriptor								  */
/**********************************************************************/
BOOLEAN	Sx2BulkdataTrans(int Usbnum,
						 PVOID bulkControl,
						 char *buffer,
						 int bufferSize,
						 int *recnBytes)
{
	int nBytes = 0;
	bool bResult = FALSE;
	PUSBD_PIPE_INFORMATION pPipe = pSx2InterfaceInfo->Pipes;
	DWORD ioctl_val = IOCTL_EZUSB_BULK_WRITE;
	if(pPipe[((PBULK_TRANSFER_CONTROL)bulkControl)->pipeNum].EndpointAddress >> 7)
	{
		ioctl_val = IOCTL_EZUSB_BULK_READ;
	}
	bResult = DeviceIoControl ( hDevice[Usbnum],
								ioctl_val, 
								bulkControl,
								sizeof (BULK_TRANSFER_CONTROL),
								buffer,
								bufferSize,
								(unsigned long *)&nBytes,
								NULL);
	if(bResult == FALSE)
	{
		return FALSE;
	}
	*recnBytes = nBytes;
	return TRUE;
}

/**********************************************************************/
/*	send the get configuration request			    				  */
/*	purpose:														  */
/*				send the get configuration request to the CY7c68001	  */
/*	Input:															  */
/*			the point of myRest struct								  */
/*																	  */
/* Return Value:												      */
/*      the point for string descriptor								  */
/**********************************************************************/
BOOLEAN Sx2SendGetConfigReq(int Usbnum,
						 PVOID myRequest,
						 char *buffer,
						 int bufferSize,
						 int *recnBytes)
{
	int nBytes = 0;
	bool bResult = FALSE;
	bResult = DeviceIoControl (	hDevice[Usbnum],
								IOCTL_Ezusb_VENDOR_REQUEST,
								myRequest,
								sizeof(VENDOR_OR_CLASS_REQUEST_CONTROL),
								buffer,
								bufferSize,
								(unsigned long *)&nBytes,
								NULL);
	if(bResult == FALSE)
	{
		return FALSE;
	}
	*recnBytes = nBytes;
	return TRUE;
}	
