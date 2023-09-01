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
		AfxMessageBox("�رյ�ǰ��USB�豸�ɹ�");
		OpenMark = FALSE;
	}
	else
	{
		AfxMessageBox("������Ч,���豸�Ѿ��ر�");	
	}	
}

void CDlgPgDevice::OnButtonOpen()		
{
	int index = 0;

	// TODO: Add your control notification handler code here
	/* ��ȡѡ���豸������ */ 
	index = m_ListBoxDevices.GetCurSel();

	if(index == LB_ERR)
	{
		AfxMessageBox("��������б����ѡ��Ҫ�򿪵��豸��Ȼ���ٵ���ð�ť������ϵͳ��Ĭ��Ϊ�򿪵�һ����������USB�豸");
		theApp.UsbCurrentNum = ListBoxDevice[0];
	}
	else
	{
		/* ��ѡ����USB�豸��Ϊ��ǰ��USB�豸 */
		theApp.UsbCurrentNum = ListBoxDevice[index];
	}

	OpenMark = OpenDriver(theApp.UsbCurrentNum);

	if(OpenMark)
	{	
		AfxMessageBox("����ѡ����USB�豸�ɹ�");
	}
	else
	{
		AfxMessageBox("����ѡ����USB�豸ʧ��");
	}
}

void CDlgPgDevice::OnButtonScan() 
{
	// TODO: Add your control notification handler code here
	UpdateDeviceList();
}


void CDlgPgDevice::UpdateDeviceList()
{
	/*��Ѱϵͳ���Ƿ�����USB�豸*/
	int i = 0;
	int j = 0;
	int strlong = 0;
	CString deviecestr[15],str;
	PVOID psx2_device= 0;
	
	/* ����豸�б�� */
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

		/* ��ɨ��õ���USB�豸���豸�б������ʾ,�Ա��û�ѡ�� */
		str.Format(_T("decusb-%d"), i);
		ListBoxDevice[j] = i;
		j++;
		m_ListBoxDevices.AddString(str);

		/*��ʾ��ǰ������*/
		theApp.str = "";
		strlong = theApp.str.GetLength(); 
		str.Format("   VC5509USB Device %d is founded.  \n",i);
		theApp.str.Insert(strlong +10, str);
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	
		psx2_device = Sx2GetDeviceDesc(theApp.UsbCurrentNum);

		if(psx2_device== NULL)
		{
			AfxMessageBox("����ʧ�ܣ��븴λSEED-DSK5509ϵͳ");
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


