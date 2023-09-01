// DlgPgDevreq.cpp : implementation file
//

#include "stdafx.h"
#include "USBDEMO.h"
#include "DlgPgDevreq.h"

#include "ezusbsys.h"
#include "seeddecusb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPgDevreq dialog


CDlgPgDevreq::CDlgPgDevreq(CWnd* pParent /*=NULL*/)
	: CDialogPage(CDlgPgDevreq::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPgDevreq)
	m_strdirection = _T("");
	m_strrecipient = _T("");
	m_strtype = _T("");
	m_brequest = _T(176);
	m_windex = 0;
	m_wvalue = _T(1);
	m_strbuffer = _T("");
	m_buffersize = _T(8);
	//}}AFX_DATA_INIT
	m_strdirection = _T("1:IN");
	m_strrecipient = _T("2:Endpoint");
	m_strtype = _T("1:Class");
}


void CDlgPgDevreq::DoDataExchange(CDataExchange* pDX)
{
	CDialogPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPgDevreq)
	DDX_Control(pDX, IDC_EDIT_WVALUE, m_ctrlwvalue);
	DDX_Control(pDX, IDC_EDIT_WINDEX, m_ctrlwindex);
	DDX_Control(pDX, IDC_EDIT_BUFFER_SIZE, m_ctrlbuffersize);
	DDX_Control(pDX, IDC_EDIT_BUFFER, m_ctrlbuffer);
	DDX_Control(pDX, IDC_EDIT_BREQUEST, m_ctrlbrequest);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_ctrltype);
	DDX_Control(pDX, IDC_COMBO_RECIPIENT, m_ctrlrecipient);
	DDX_Control(pDX, IDC_COMBO_DIRECTION, m_ctrldirection);
	DDX_CBString(pDX, IDC_COMBO_DIRECTION, m_strdirection);
	DDX_CBString(pDX, IDC_COMBO_RECIPIENT, m_strrecipient);
	DDX_CBString(pDX, IDC_COMBO_TYPE, m_strtype);
	DDX_Text(pDX, IDC_EDIT_BREQUEST, m_brequest);
	DDX_Text(pDX, IDC_EDIT_WINDEX, m_windex);
	DDX_Text(pDX, IDC_EDIT_WVALUE, m_wvalue);
	DDX_Text(pDX, IDC_EDIT_BUFFER, m_strbuffer);
	DDV_MaxChars(pDX, m_strbuffer, 130);
	DDX_Text(pDX, IDC_EDIT_BUFFER_SIZE, m_buffersize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPgDevreq, CDialogPage)
	//{{AFX_MSG_MAP(CDlgPgDevreq)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPgDevreq message handlers

BOOL CDlgPgDevreq::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctrldirection.AddString(_T("1:IN"));
	m_ctrldirection.AddString(_T("0:OUT"));

	m_ctrlrecipient.AddString(_T("0:Device"));
	m_ctrlrecipient.AddString(_T("1:Interface"));
	m_ctrlrecipient.AddString(_T("2:Endpoint"));
	m_ctrlrecipient.AddString(_T("3:Other"));
    
	m_ctrltype.AddString(_T("0:Standard"));
	m_ctrltype.AddString(_T("1:Class"));
	m_ctrltype.AddString(_T("2:Vendor"));

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
/////// Change input sting to data 

void CDlgPgDevreq::strtodata()
 {
	int i;
	int j= 0;
	int UsrData = 0;
	LPCTSTR StrBuffer;
	int longth = 0;
	StrBuffer = m_strbuffer.GetBuffer(100);
	for(i = 0; i<m_buffersize; )
	{
		if(StrBuffer[i] !=' ')
		{
			UsrData = chartonumber(StrBuffer[i]);
			if(UsrData == 0xFF)
			{
				return;
			}
			m_Endpoint0data[j] = UsrData;
		}
		i = i +1;
		if(StrBuffer[i] !=' ')
		{
			UsrData = chartonumber(m_Endpoint0data[i]);
			if(UsrData == 0xFF)
			{
				return;
			}
			m_Endpoint0data[j] = UsrData+ m_strbuffer[j]* (0x10);
			j++;
		}
		i = i+1;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
////// Change char to data number

int CDlgPgDevreq::chartonumber(char str)
{
			int UsrData;
			switch(str)
			{
				case '0':
					UsrData = 0;
					break;
				case '1':
					UsrData = 1;
					break;
				case '2':
					UsrData = 2;
					break;
				case '3':
					UsrData = 3;
					break;
				case '4':
					UsrData = 4;
					break;
				case '5':
					UsrData = 5;
					break;
				case '6':
					UsrData = 6;
					break;
				case '7':
					UsrData = 7;
					break;
				case '8':
					UsrData = 8;
					break;
				case '9':
					UsrData = 9;
					break;
				case 'A':
				case 'a':
					UsrData = 0xa;
					break;
				case 'B':
				case 'b':
					UsrData = 0xb;
					break;
				case 'C':
				case 'c':
					UsrData = 0xc;
					break;
				case 'D':
				case 'd':
					UsrData = 0xd;
					break;
				case 'E':
				case 'e':
					UsrData = 0xe;
					break;
				case 'F':
				case 'f':
					UsrData = 0xf;
					break;
				default:
					AfxMessageBox("有无效数据，请重新输入");
					UsrData = 0xff;
			}
			return UsrData;
}

void CDlgPgDevreq::OnOK() 
{
	// TODO: Add extra validation here	
	int i = 0;
	int recnBytes;
	char buffer[64];
	CString str;

	VENDOR_OR_CLASS_REQUEST_CONTROL	myRequest;

	UpdateData(TRUE);
	UpdateData(FALSE);
	
	m_intdirection = m_ctrldirection.GetCurSel();
	m_intrecipient = m_ctrlrecipient.GetCurSel();
	m_inttype = m_ctrltype.GetCurSel();
	
	
	CDialogPage::OnOK();
	if(m_intdirection ==0)		// 如果是输入,没有数据项
	strtodata();

	// 测试命令输入
	myRequest.direction = m_intdirection;
	myRequest.index = m_windex;
	myRequest.value = m_wvalue;
	myRequest.request = m_brequest;
	
	// vendor specific request type 
	myRequest.requestType = m_inttype; 
    
	// specify the recepient  
	myRequest.recepient = m_intrecipient;	

	for(i = 0 ;i <m_buffersize; i++)
	{
		buffer[i] = m_Endpoint0data[i];
	}
	/*									// 用来测试通过控制节点0与dsp数据传输
	buffer[0] = 0x11;
	buffer[1] = 0x22;
	buffer[2] = 0x33;
	buffer[3] = 0x44;
	buffer[4] = 0x55;
	buffer[5] = 0x66;
	buffer[6] = 0x77;
	buffer[7] = 0x88;

	myRequest.direction = 1;			// 如果为IN传送,设为1; OUT传送设为0
	myRequest.index = m_windex;
	myRequest.value = m_wvalue;
	myRequest.request = 0xb3;			// 这个域的值是固定的,用户不要更改,否则出错
	
	// vendor specific request type 
	myRequest.requestType = 1; 
    
	// specify the recepient  
	myRequest.recepient = 2;
	*/

	Sx2SendVendorReq(theApp.UsbCurrentNum,
					 &myRequest,
					 &buffer[0],		// 接收或发送数据缓冲区
					 m_buffersize,		// 欲接收或发送的字节长度
					 &recnBytes);

	/* 在输出窗口中显示接收到的数据 */
	if(recnBytes !=0)
	{
		theApp.str = "*********************************************************************\n";
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
		
		theApp.str = "The data that recieved from the Endpoint0 as following:  \n";
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
		
		theApp.str = "-------------------------------------------------------  \n";
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
		for(i = 0;i<recnBytes;i++)
		{
			theApp.str.Format("0x%x    ",buffer[i]);
			AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
		}
		
		theApp.str = "\n";
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	}
}

