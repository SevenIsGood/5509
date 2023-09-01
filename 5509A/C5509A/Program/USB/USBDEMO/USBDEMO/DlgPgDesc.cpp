// DlgPgDesc.cpp : implementation file
//

#include "stdafx.h"
#include "USBDEMO.h"
#include "DlgPgDesc.h"
#include "ezusbsys.h"
#include "seeddecusb.h"
#include "USBDEMODlg.h"	

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPgDesc dialog


CDlgPgDesc::CDlgPgDesc(CWnd* pParent /*=NULL*/)
	: CDialogPage(CDlgPgDesc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPgDesc)
	m_StringIndex = 0;
	m_StringLanguageID = 0;
	//}}AFX_DATA_INIT
}


void CDlgPgDesc::DoDataExchange(CDataExchange* pDX)
{
	CDialogPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPgDesc)
	DDX_Text(pDX, IDC_EDIT_STRING_DESC, m_StringIndex);
	DDX_Text(pDX, IDC_EDIT_STRING_DESC_LID, m_StringLanguageID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPgDesc, CDialogPage)
	//{{AFX_MSG_MAP(CDlgPgDesc)
	ON_BN_CLICKED(IDC_BUTTON_DEVICE_DESC, OnButtonDeviceDesc)
	ON_BN_CLICKED(IDC_BUTTON_DEVICE_QUALIFIER_DESC, OnButtonDeviceQualifierDesc)
	ON_BN_CLICKED(IDC_BUTTON_STRING_DESC, OnButtonStringDesc)
	ON_EN_CHANGE(IDC_EDIT_STRING_DESC, OnChangeEditStringDesc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPgDesc message handlers

void CDlgPgDesc::OnButtonDeviceDesc() 
{
	// TODO: Add your control notification handler code here
	//CUSBDEMODlg  MyUsbDemoDlg;	

	PVOID psx2_device= 0;
	psx2_device = Sx2GetDeviceDesc(theApp.UsbCurrentNum);
	
	if(psx2_device== NULL)
	{
		AfxMessageBox("操作失败，请复位SEED-DSK5509系统");
		return;
	}
	
	/*显示获取的描述表*/
    
	theApp.str = "*********************************************************************\n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str = "the current usb's device descriptor is:  \n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str = "-----------------------------------------------------------  \n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str.Format("  Vendor ID is: 0x%x  \n",((pUsb_Device_Descriptor)psx2_device )->idVendor);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str.Format("  Product ID is: 0x%x  \n",((pUsb_Device_Descriptor)psx2_device )->idProduct);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	
	theApp.str.Format("  Device ID is: 0x%x  \n",((pUsb_Device_Descriptor)psx2_device )->bcdDevice);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str.Format("  Maximum Packet Size is: 0x%x  \n",((pUsb_Device_Descriptor)psx2_device )->bMaxPacketSize0);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str.Format("  Device class is: 0x%x  \n",((pUsb_Device_Descriptor)psx2_device )->bDeviceClass);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str.Format("  Device sub-class is: 0x%x  \n",((pUsb_Device_Descriptor)psx2_device )->bDeviceSubClass);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str.Format("  Manufacturer string index is: 0x%x  \n",((pUsb_Device_Descriptor)psx2_device )->iManufacturer);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str.Format("  Product string index is: 0x%x  \n",((pUsb_Device_Descriptor)psx2_device )->idProduct);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str.Format("  Serial number string index is: 0x%x  \n",((pUsb_Device_Descriptor)psx2_device )->iSerialNumber);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str.Format("  Number of configurations is: 0x%x  \n",((pUsb_Device_Descriptor)psx2_device )->bNumConfigurations);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	
	theApp.str = "\n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
}

void CDlgPgDesc::OnButtonDeviceQualifierDesc() 
{
	// TODO: Add your control notification handler code here
	AfxMessageBox("操作无效，高速设备才支持该类型");
	return;
}


void CDlgPgDesc::OnButtonStringDesc() 
{
	// TODO: Add your control notification handler code here
	ULONG i;
	PVOID psx2_string = 0;
	pUsb_String_Descriptor pcd;

	char viewstr[100];
	UpdateData(TRUE);
	UpdateData(FALSE);
    CDialogPage::OnOK();
	
	psx2_string = Sx2GetStringDesc(theApp.UsbCurrentNum,m_StringIndex,m_StringLanguageID);

	if(psx2_string == NULL)
	{
		AfxMessageBox("操作失败，请复位SEED-DSK5509系统");
		return;
	}

	pcd = (pUsb_String_Descriptor)psx2_string;

	/*显示获取的描述表*/
	theApp.str = "*********************************************************************\n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	if(m_StringIndex == 0)				// 取描述表的语言ID阵列
	{
		theApp.str = " The array of two-byte LANGID codes supported by the device is:  \n";
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
		
		theApp.str = "-----------------------------------------------------------  \n";
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
		
		//for(i=0; i<pcd->bLength; i++)
		//{
		theApp.str.Format("0x%x  ", (pcd->bLength + pcd->bDescriptorType*0x100));
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

		theApp.str.Format("0x%x  \n", (pcd->bString[0] + pcd->bString[1]*0x100));
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
		//}
	}
	else								// 取描述表的字符串 
	{
		theApp.str = "the current usb's string descriptor is:  \n";
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
		
		theApp.str = "-----------------------------------------------------------  \n";
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
		
		theApp.str.Format("  The size of this descriptor in bytes is :0x%x  \n",pcd->bLength);
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
		
		theApp.str.Format("  The transfer type is :0x%x  \n",pcd->bDescriptorType);
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

		for(i=0; i<pcd->bLength-1; i++)
		{
			viewstr[i] = pcd->bString[i*2];
		}
		viewstr[i] = '\0';
		
		theApp.str = "  The bString field of string descriptor is:\n";
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
		
		theApp.str.Format("	%s \n\n",viewstr);
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	}
}

void CDlgPgDesc::OnChangeEditStringDesc() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}
