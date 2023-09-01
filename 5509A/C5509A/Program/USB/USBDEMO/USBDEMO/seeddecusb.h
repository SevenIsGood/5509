/**********************************************************************/
/* this file is the function for test the seeddec5416 driver and devic*/
/*	author lifeng Duan												  */
/*	Lt.d	SEED													  */
/**********************************************************************/

#ifndef _SEEDDECUSB_H_
#define _SEEDDECUSB_H_

typedef PVOID USBD_PIPE_HANDLE;
typedef PVOID USBD_CONFIGURATION_HANDLE;
typedef PVOID USBD_INTERFACE_HANDLE;

/*定义通道的类型*/
typedef enum _USBD_PIPE_TYPE {
    /*控制类型*/
	UsbdPipeTypeControl,
	/*等时传送类型*/
    UsbdPipeTypeIsochronous,
	/*批量传送类型*/
    UsbdPipeTypeBulk,
	/*中断传送类型*/
    UsbdPipeTypeInterrupt
} USBD_PIPE_TYPE;

/*定义管道的描述表节构*/
typedef struct _USBD_PIPE_INFORMATION {
	// Maximum packet size for this pipe
	//节点数据包的最大长度
    USHORT MaximumPacketSize;
	// 8 bit USB endpoint address (includes direction)
	//USB节点的地址（包括方向）
	UCHAR EndpointAddress;
	// Polling interval in ms if interrupt pipe，taken from endpoint descriptor 
    //中断检测间隔； 
    UCHAR Interval; 
	// PipeType identifies type of transfer valid for this pipe
	//有效节点的类型
	USBD_PIPE_TYPE PipeType;
	  
               
    USBD_PIPE_HANDLE PipeHandle;
    // INPUT
    // These fields are filled in by the client driver
    // Maximum size for a single request in bytes.
    ULONG MaximumTransferSize; 
    //通路标识                           
    ULONG PipeFlags;                                   
} USBD_PIPE_INFORMATION, *PUSBD_PIPE_INFORMATION;

/*USB接口的定义节构*/
typedef struct _USBD_INTERFACE_INFORMATION {
    //节构的长度，包括所有节点的信息长度
	USHORT Length; 
    // INPUT
    // Interface number and Alternate setting this
    // structure is associated with
    UCHAR InterfaceNumber;
    UCHAR AlternateSetting;
    // OUTPUT读接收描述表得到
    // These fields are filled in by USBD
    UCHAR Class;
    UCHAR SubClass;
    UCHAR Protocol;
    UCHAR Reserved;
    
    USBD_INTERFACE_HANDLE InterfaceHandle;
    ULONG NumberOfPipes; 
	//Endpoint status
    USBD_PIPE_INFORMATION Pipes[16];		// The length must be equal or more than the endpoints configued 
} USBD_INTERFACE_INFORMATION, *PUSBD_INTERFACE_INFORMATION;

/*设备描述符节构*/
typedef struct __usb_Dev_Descriptor__ {
    UCHAR  bLength;
    UCHAR  bDescriptorType;
    USHORT bcdUSB;
    UCHAR  bDeviceClass;
    UCHAR  bDeviceSubClass;
    UCHAR  bDeviceProtocol;
    UCHAR  bMaxPacketSize0;
    USHORT idVendor;
    USHORT idProduct;
    USHORT bcdDevice;
    UCHAR  iManufacturer;
    UCHAR  iProduct;
    UCHAR  iSerialNumber;
    UCHAR  bNumConfigurations;
} Usb_Device_Descriptor, *pUsb_Device_Descriptor;

/*设备限定描述表结构*/
typedef struct __usb_Dev_Quali_Descriptor__ {
    UCHAR  bLength;
    UCHAR  bDescriptorType;
    USHORT bcdUSB;
    UCHAR  bDeviceClass;
    UCHAR  bDeviceSubClass;
    UCHAR  bDeviceProtocol;
    UCHAR  bMaxPacketSize0;
    UCHAR  bNumConfigurations;
	UCHAR  bReserved;
} Usb_Device_QualiDescriptor, *pUsb_Device_QualiDescriptor;

/*配置描述表*/
typedef struct __usb_Config_Descriptor__ {
    UCHAR bLength;
    UCHAR bDescriptorType;
    USHORT wTotalLength;
    UCHAR bNumInterfaces;
    UCHAR bConfigurationValue;
    UCHAR iConfiguration;
    UCHAR bmAttributes;
    UCHAR MaxPower;
} Usb_Configuration_Descriptor, *pUsb_Configuration_Descriptor;

/*字符串描述表*/
typedef struct __usb_String_Descriptor__ { //TPM added the usb_String_Descriptor
    UCHAR bLength;
    UCHAR bDescriptorType;
    char bString[100];
} Usb_String_Descriptor, *pUsb_String_Descriptor;

typedef struct _USB_ENDPOINT_DESCRIPTOR 
{
    UCHAR	bLength;
    UCHAR	bDescriptorType;
    UCHAR	bEndpointAddress;
    UCHAR	bmAttributes;
    USHORT	wMaxPacketSize;			// 2个字节长度
    UCHAR	bInterval;
} USB_ENDPOINT_DESCRIPTOR, *PUSB_ENDPOINT_DESCRIPTOR;

typedef struct _USB_INTERFACE_DESCRIPTOR 
{
    UCHAR	bLength;
    UCHAR	bDescriptorType;
    UCHAR	bInterfaceNumber;
    UCHAR	bAlternateSetting;
    UCHAR	bNumEndpoints;
    UCHAR	bInterfaceClass;
    UCHAR	bInterfaceSubClass;
    UCHAR	bInterfaceProtocol;
    UCHAR	iInterface;
} USB_INTERFACE_DESCRIPTOR, *PUSB_INTERFACE_DESCRIPTOR;


#define GET_STRING_DESCRIPTOR_LENGTH(pv) \
    ((pUsb_String_Descriptor)pv)->bLength

#define GET_CONFIG_DESCRIPTOR_LENGTH(pv) \
    ((pUsb_Configuration_Descriptor)pv)->wTotalLength

BOOL DoIoctl(
  HANDLE hDevice,              // handle to device of interest
  DWORD dwIoControlCode,       // control code of operation to perform
  LPVOID lpInBuffer,           // pointer to buffer to supply input data
  DWORD nInBufferSize,         // size, in bytes, of input buffer
  LPVOID lpOutBuffer,          // pointer to buffer to receive output data
  DWORD nOutBufferSize,        // size, in bytes, of output buffer
  LPDWORD lpBytesReturned,     // pointer to variable to receive byte count
  LPOVERLAPPED lpOverlapped,   // pointer to structure for asynchronous operation
  CString& hOutputBox
);

/************************************************************/
/*	专门为5509定义配置描述表								*/
/************************************************************/
typedef struct USB_DataStructDef
{
	USHORT                    Bytes;        // Total number of bytes in the buf 
	USHORT                    *pBuffer;     // pointer to the start of buffer   
	struct USB_DataStructDef  *pNextBuffer; // pointer to the next structure    
	
}USB_DataStruct;

typedef struct Bulk_In2_link
{				// Endpoint 2 in descriptor
	USHORT 	Bytes;
	USB_ENDPOINT_DESCRIPTOR *EndIn2Ptr;
	USB_DataStructDef *Ptr;		
} BulkIn2Link, *PBulkIn2Link;

typedef struct Bulk_Out2_link 
{			// Endpoint 2 out descriptor
		USHORT 	Bytes;
		USB_ENDPOINT_DESCRIPTOR *EndOut2Ptr;;
		Bulk_In2_link	*BulkInPtr;	
} BulkOut2Link, *PBulkOut2Link;

typedef struct usb_interface_link 
{		// Interface descriptor
		USHORT 	Bytes;
		USB_INTERFACE_DESCRIPTOR *InterPtr;
		Bulk_Out2_link	*BulkOutPtr;
} UsbInterfaceLink, *PUsbInterfaceLink;

typedef struct usb_configuration_link 
{	// Device descriptor
		USHORT 	Bytes;
		Usb_Configuration_Descriptor *ConfigPtr;
		usb_interface_link	*UsbIntPtr;	
} UsbConfigurationLink, *PUsbConfigurationLink;

/*-------------------------------------------------------------------------------*/
/**********************************************************************/
/* OpenDriver()		                                                  */
/*                                                                    */
/* Purpose:													          */ 
/*      Opens the device driver using symbolic name provided          */
/*															          */
/* Input:  none                                                       */
/*															          */
/* Return Value:                                                      */
/*      Boolean that indicates if the driver was                      */
/*      successfully opened or not.									  */
/**********************************************************************/
BOOLEAN	OpenDriver(int Usbnum);
/**********************************************************************/
/* CloseDriver()													  */
/* Purpose:															  */ 
/*      Close the device driver using symbolic						  */
/* Input:															  */
/*			none													  */
/* Return Value:													  */
/*			none													  */
/**********************************************************************/
void CloseDriver(int Usbnum);
/**********************************************************************/
/*	get the device descriptor										  */
/*	purpose:														  */
/*			get the device descriptor for the CY7c68001			      */
/*	Input:															  */
/*			none													  */
/* Return Value:												      */
/*      Boolean that indicates if the driver was					  */
/*      successfully opened or not.									  */
/**********************************************************************/
PVOID Sx2GetDeviceDesc(int Usbnum);
/**********************************************************************/
/*	get the string descriptor										  */
/*	purpose:														  */
/*			get the string descriptor for the CY7c68001			      */
/*	Input:															  */
/*			none													  */
/* Return Value:												      */
/*      the point for string descriptor								  */
/**********************************************************************/
PVOID Sx2GetStringDesc(int Usbnum,int stringindex,int languageID);
/**********************************************************************/
/*	get the configuration descriptor    							  */
/*	purpose:														  */
/*			get the configuration descriptor for the CY7c68001	      */
/*	Input:															  */
/*			none													  */
/* Return Value:												      */
/*      the point for string descriptor								  */
/**********************************************************************/
PVOID Sx2GetconfigDesc(int Usbnum);
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
BOOLEAN Sx2GetPipeInfo(int Usbnum,PVOID pInterface);
/**********************************************************************/
/*	send the vendor request			    							  */
/*	purpose:														  */
/*				send the vendor request to the CY7c68001		      */
/*	Input:															  */
/*			myRest struct											  */
/* Return Value:												      */
/*      the point for string descriptor								  */
/**********************************************************************/
BOOLEAN Sx2SendVendorReq(int Usbnum,
						 PVOID myRequest,
						 char *buffer,
						 int bufferSize,
						 int *recnBytes);
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
						 int *recnBytes);


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
							int *recnBytes);

#endif