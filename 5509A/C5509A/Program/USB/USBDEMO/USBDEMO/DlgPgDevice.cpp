// DlgPgDevice.cpp : implementation file
//

#include "stdafx.h"
#include "USBDEMO.h"
#include "DlgPgDevice.h"
#include "ezusbsys.h"
#include "seeddecusb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPgDevice dialog


CDlgPgDevice::CDlgPgDevice(CWnd* pParent /*=NULL*/)
	: CDialogPage(CDlgPgDevice::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPgDevice)
		// NOTE: the ClassWizard will add member initialization here
	OpenMark = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgPgDevice::DoDataExchange(CDataExchange* pDX)
{
	CDialogPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPgDevice)
	DDX_Control(pDX, IDC_LIST_DEVICES, m_ListBoxDevices);
	DDX_Control(pDX, IDC_BUTTON_OPEN, m_ButtonOpen);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPgDevice, CDialogPage)
	//{{AFX_MSG_MAP(CDlgPgDevice)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_SCAN, OnButtonScan)
	ON_LBN_SELCHANGE(IDC_LIST_DEVICES, OnSelchangeListDevices)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPgDevice message handlers

void CDlgPgDevice::OnButtonClose() 
{
	// TODO: Add your control notification handler code here
	if(OpenMark == TRUE)
	{
		CloseDriver(theApp.UsbCurrentNum);
		AfxMessageBox("关闭当前的USB设备成功");
		OpenMark = FALSE;
	}
	else
	{
		AfxMessageBox("命令无效,该设备已经关闭");	
	}	
}

void CDlgPgDevice::OnButtonOpen()		
{
	int index = 0;

	// TODO: Add your control notification handler code here
	/* 获取选定设备的索引 */ 
	index = m_ListBoxDevices.GetCurSel();

	if(index == LB_ERR)
	{
		AfxMessageBox("请从驱动列表框中选择要打开的设备，然后再点击该按钮；否则系统会默认为打开第一个检索到的USB设备");
		theApp.UsbCurrentNum = ListBoxDevice[0];
	}
	else
	{
		/* 将选定的USB设备设为当前的USB设备 */
		theApp.UsbCurrentNum = ListBoxDevice[index];
	}

	OpenMark = OpenDriver(theApp.UsbCurrentNum);

	if(OpenMark)
	{	
		AfxMessageBox("打开所选定的USB设备成功");
	}
	else
	{
		AfxMessageBox("打开所选定的USB设备失败");
	}
}

void CDlgPgDevice::OnButtonScan() 
{
	// TODO: Add your control notification handler code here
	UpdateDeviceList();
}


void CDlgPgDevice::UpdateDeviceList()
{
	/*查寻系统中是否有了USB设备*/
	int i = 0;
	int j = 0;
	int strlong = 0;
	CString deviecestr[15],str;
	PVOID psx2_device= 0;
	
	/* 清除设备列表框 */
	m_ListBoxDevices.ResetContent();
	
	theApp.str = "Scanning for attached USB devices.....  \n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);	// Send a massage to output window

	for(i = 0;;i++)
	{
		if((OpenDriver(i) != TRUE) || (i ==8))
		{
			if(i ==0)
			{
				theApp.str = "Not found the USB device.\n";
				AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
			}
			theApp.UsbDeviecNum = i;
			break;
		}

		/* 将扫描得到的USB设备在设备列表框中显示,以便用户选择 */
		str.Format(_T("decusb-%d"), i);
		ListBoxDevice[j] = i;
		j++;
		m_ListBoxDevices.AddString(str);

		/*显示当前的属性*/
		theApp.str = "";
		strlong = theApp.str.GetLength(); 
		str.Format("   VC5509USB Device %d is founded.  \n",i);
		theApp.str.Insert(strlong +10, str);
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	
		psx2_device = Sx2GetDeviceDesc(theApp.UsbCurrentNum);

		if(psx2_device== NULL)
		{
			AfxMessageBox("操作失败，请复位SEED-DSK5509系统");
		}

		theApp.str = "";
		strlong = theApp.str.GetLength(); 
		str.Format( "   VID:0x%04X PID:0x%04X  \n", 
					((pUsb_Device_Descriptor)psx2_device )->idVendor, 
					((pUsb_Device_Descriptor)psx2_device )->idProduct);
		
		theApp.str.Insert(strlong +10, str);
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
		theApp.str = "\n";
	    AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	}
}

void CDlgPgDevice::OnSelchangeListDevices() 
{
	// TODO: Add your control notification handler code here
	
}


