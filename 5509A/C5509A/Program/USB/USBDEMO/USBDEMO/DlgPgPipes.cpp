// DlgPgPipes.cpp : implementation file
//

#include "stdafx.h"
#include "USBDEMO.h"
#include "DlgPgPipes.h"
#include "seeddecusb.h"
#include "ezusbsys.h"

#include <iostream.h>
#include <fstream.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPgPipes dialog


CDlgPgPipes::CDlgPgPipes(CWnd* pParent /*=NULL*/)
	: CDialogPage(CDlgPgPipes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPgPipes)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgPgPipes::DoDataExchange(CDataExchange* pDX)
{
	CDialogPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPgPipes)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPgPipes, CDialogPage)
	//{{AFX_MSG_MAP(CDlgPgPipes)
	ON_BN_CLICKED(IDC_BUTTON_PIPTOWIN, OnButtonPiptowin)
	ON_BN_CLICKED(IDC_BUTTON_PIPTOFILE, OnButtonPiptofile)
	ON_BN_CLICKED(IDC_BUTTON_FILETOPIP, OnButtonFiletopip)
	ON_BN_CLICKED(IDC_BUTTON_GETPIPINF, OnButtonGetpipinf)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPgPipes message handlers
void CDlgPgPipes::OnButtonGetpipinf() 
{
	// TODO: Add your control notification handler code here
	ULONG i;
	BOOLEAN OperatState;
	USBD_INTERFACE_INFORMATION Sx2InterfaceInfo;
	
	OperatState = Sx2GetPipeInfo(theApp.UsbCurrentNum,(PVOID)&Sx2InterfaceInfo);
	if(OperatState== FALSE)
	{
		AfxMessageBox("操作失败，请复位SEED-DSK5509系统");
		return;
	}

	PUSBD_INTERFACE_INFORMATION pCD = (PUSBD_INTERFACE_INFORMATION)&Sx2InterfaceInfo;
	
	// 显示获取的描述表
	theApp.str = "*********************************************************************\n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	
	theApp.str = "the current usb's pipes information is:  \n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	
	theApp.str = "-----------------------------------------------------------  \n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	
	theApp.str.Format("   The interfaceNumber is: 0x%x  \n",pCD->InterfaceNumber);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str.Format("   The AlternateSetting is: 0x%x  \n",pCD->AlternateSetting);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str.Format("   The Class is: 0x%x  \n",pCD->Class);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str.Format("   The Sub-class is: 0x%x  \n",pCD->SubClass);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	
	theApp.str.Format("   The Protocol is: 0x%x  \n",pCD->Protocol);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str.Format("   The NumberOfPipes is: 0x%x  \n",pCD->NumberOfPipes);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	
	theApp.str = "-----------------------------------------------------------  \n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str = "   The detailed Endpoints information as following: \n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	
	for(i=0; i<pCD->NumberOfPipes; i++)
	{
		theApp.str.Format("-------------The pipe %x information is:-----------   \n",i);
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
		
		theApp.str.Format("   The MaximumPacketSize is: 0x%x  \n",pCD->Pipes[i].MaximumPacketSize);
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

		theApp.str.Format("   The EndpointAddress is: 0x%x  \n",pCD->Pipes[i].EndpointAddress);
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

		theApp.str.Format("   The Interval is: 0x%x  \n",pCD->Pipes[i].Interval);
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
		
		theApp.str.Format("   The PipeType is: 0x%x  \n",pCD->Pipes[i].PipeType);
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

		theApp.str.Format("   The PipeFlags is: 0x%x  \n",pCD->Pipes[i].PipeFlags);
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	}

	theApp.str = "\n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
}

/////////////////////////////////////////////////////////////////////////////////////
////// Change char to data number

int CDlgPgPipes::chartonumber(char str)
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
				case ' ':				// 如果有空格字符,则不做处理
				case 10:
					UsrData = 0xff;
					break;
				default:
					AfxMessageBox("有无效数据，请重新输入");
					UsrData = 0xff;
	}
	return UsrData;
}

void CDlgPgPipes::OnButtonPiptowin() 
{
	// TODO: Add your control notification handler code here
	// BULK数据的读操作
	ULONG i = 0;
	char *buffer;
	char recievedat[1024] = {0};
    ULONG length;
    BULK_TRANSFER_CONTROL bulkControl;
	int recnBytes = 0;
	int m_USBbufferlong = 64;

	/* 命令请求定义 */
	char buffer1[64];
	VENDOR_OR_CLASS_REQUEST_CONTROL	myRequest;
	ULONG venderlong;

	/* 发送命令，使DSP先定入固定的数据 */
	myRequest.direction = 0;
	myRequest.index = 0;
	myRequest.value = USB_BULK1_WRITE;
	myRequest.request =0xb6;
	// vendor specific request type (2)
	myRequest.requestType=2; 
    // recepient is device (0)
	myRequest.recepient=0;
	venderlong = 0;

	/* 发送命令 */
	Sx2SendVendorReq(theApp.UsbCurrentNum,
					 &myRequest,
					 &buffer1[0],
					 venderlong,
					 &recnBytes);

	/* 按默认的配置，读回Endpoint1的数据并显示 */
	bulkControl.pipeNum = 1;			// 一定要选对管道	1
	buffer = &recievedat[0];
	length = 48;
	Sx2BulkdataTrans( theApp.UsbCurrentNum,
					  &bulkControl,
					  buffer,
					  length,
					  &recnBytes);

	/* 显示读回数据 */
	theApp.str = "*********************************************************************\n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	
	theApp.str.Format("The data read from the pipe %d is:  \n",bulkControl.pipeNum);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str.Format("The data received is from  %d to %d \n",(buffer[0]&0xff),(buffer[length-1]&0xff));
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str = "-----------------------------------------------------------  \n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	
	for(i=0; i<length; i++)
	{
		if(i%6 ==5)
		{
			theApp.str.Format("  %d  \n",(buffer[i]&0xff));
		}
		else
		{
			theApp.str.Format("  %d    ",(buffer[i]&0xff));
		}
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	}	

	theApp.str = "\n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
}

void CDlgPgPipes::OnButtonPiptofile() 
{
	// TODO: Add your control notification handler code here
	// BULK数据的读操作
	ULONG i = 0;
	char *buffer;
	char recievedat[1024] = {0};
    ULONG length;
    BULK_TRANSFER_CONTROL bulkControl;
	int recnBytes = 0;
	//int m_USBbufferlong = 512;
	int m_USBbufferlong = 64;
	
	/* 命令请求定义 */
	char buffer1[64];
	VENDOR_OR_CLASS_REQUEST_CONTROL	myRequest;
	ULONG venderlong;
	
	/* 选择并打开输出信息文件 */
	fstream Myoutputfile;		
	CString strpathname;
	CString filter;
	unsigned short int Data[64];

	filter = "文本文件(*.txt)||";
	CFileDialog	FileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,filter);
	
	if(FileDlg.DoModal() ==IDOK)
	{
		/* 选择文件 */
		strpathname = FileDlg.GetPathName();			
		
		/* 打开所选定的文件 */
		Myoutputfile.open(strpathname, ios::out|ios::trunc);
		if(!Myoutputfile.rdbuf( )->is_open())
		{
			AfxMessageBox("错误！所选定的输出文件不能打开，请检查文件属性");
			return;
		}
	}
	
	/* 发送命令，使DSP先定入固定的数据 */
	myRequest.direction = 0;
	myRequest.index = 0;
	myRequest.value = USB_BULK1_WRITE;
	myRequest.request =0xb6;
	// vendor specific request type (2)
	myRequest.requestType=2; 
    // recepient is device (0)
	myRequest.recepient=0;
	venderlong = 0;
	
	/* 发送命令 */
	Sx2SendVendorReq(theApp.UsbCurrentNum,
		&myRequest,
		&buffer1[0],
		venderlong,
		&recnBytes);
	
	/* 按默认的配置，读回Endpoint1的数据并显示 */
	bulkControl.pipeNum = 1;
	buffer = &recievedat[0];
	length = 48;
	Sx2BulkdataTrans( theApp.UsbCurrentNum,
		&bulkControl,
		buffer,
		length,
		&recnBytes);

	/* 将字符转换成整形数*/
	for(i=0; i<length; i++)
	{
		Data[i] = buffer[i]&0x00FF;
	}

	/* 显示读回数据 */
	Myoutputfile.setf(ios::hex|ios::showbase|ios::uppercase);
	Myoutputfile << "*********************************************************************\n";
	Myoutputfile <<  "The data read from the endpiont "<<bulkControl.pipeNum<<" is:  \n";
	Myoutputfile <<  "The data received is from "<<Data[0]<<" to "<<Data[length-1]<<".\n";
	Myoutputfile <<  "-----------------------------------------------------------  \n";
	
	for(i=0; i<length; i++)
	{
		if(i%8 ==7)
		{
			Myoutputfile<<"    "<<Data[i]<<endl;
		}
		else
		{
			Myoutputfile<<"    "<<Data[i];			
		}
	}	
	Myoutputfile.close();
}

void CDlgPgPipes::OnButtonFiletopip() 
{
	// TODO: Add your control notification handler code here
		//BULK数据的写操作
	ULONG i = 0;
	int j = 0;
	char *buffer;
	char Databuffer[512] = {0};
	char Receidata[512] = {0};
    ULONG length;
    BULK_TRANSFER_CONTROL bulkControl;
	int recnBytes = 0;
	int m_USBbufferlong = 64;

	/* 命令请求定义 */
	char buffer1[64];
	VENDOR_OR_CLASS_REQUEST_CONTROL	myRequest;
	ULONG venderlong;
	
	fstream Mydatafile;		
	CString strpathname;
	CString filter;
	char Temp1,Temp2;
	ULONG Datalen;							// 数据长度

	filter = "文本文件(*.txt)||";
	CFileDialog	FileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,filter);
	
	if(FileDlg.DoModal() ==IDOK)
	{
		/* 选择文件 */
		strpathname = FileDlg.GetPathName();			
		
		/* 打开所选定的文件 */
		Mydatafile.open(strpathname, ios::in|ios::nocreate);
		if(!Mydatafile.rdbuf( )->is_open())
		{
			AfxMessageBox("错误！所选定的数据文件不能打开，命令无效");
			return;
		}
		
		/* 读取文件的数据内容 */
		i = 0;
        do
		{
			Mydatafile.read(&Temp1,1);	
			if((Temp1!=13)&&(Temp1!=9)&&(Temp1!=32)&&(Temp1!=10)&&(Temp1!=-52))	// 如果输入的字符不是换行,TAB,空格键
			{
				Databuffer[i] = chartonumber(Temp1);
				i++;
			}
		}
		while(!Mydatafile.eof());
	}

	else							// 如果取消选择文件,则退出
	{
		return;
	}

	/* 关闭文件 */
	Mydatafile.close();
	
	/* 如果是空文件,给出文件为空信息 */
	if(i==0)
	{
		AfxMessageBox("错误！ 所选定数据文件内容为空");
		return;
	}	
	
	/* 因为最后一个数据是重复的,应该除去 */
	Datalen = i-1;	
	
	/* 对接收到的数据进行整字节化处理 */
	if((Datalen%2) != 0)		// 如果是奇数个半字节
	{
		Databuffer[Datalen] = 0;
		Datalen++;
	}
	
	/* 将获取的数字组成数据字节 */			
	for(i=0,j=0; i<Datalen/2; i++,j++)
	{
		Temp1 = Databuffer[2*j];
		Temp2 = Databuffer[2*j+1];
		Databuffer[i] = Temp2+Temp1* (0x10);
	}

	/* 发送字节长度 */
	length = Datalen/2;

	/*写Endpoint2*/
	buffer = &Databuffer[0];
	bulkControl.pipeNum = 2;
	Sx2BulkdataTrans( theApp.UsbCurrentNum,
					  &bulkControl,
					  buffer,
					  length,
					  &recnBytes);

	/* 显示发送的数据 */
	theApp.str = "*********************************************************************\n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	
	theApp.str.Format("The data transfered from file to the pipe %X is:  \n",bulkControl.pipeNum);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str.Format("The data transfered is from  %X to %X \n",(Databuffer[0]&0xff),(Databuffer[length-1]&0xff));
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str = "-----------------------------------------------------------  \n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	
	for(i=0; i<length; i++)
	{
		if(i%6 ==5)
		{
			theApp.str.Format("  %X  \n",(Databuffer[i]&0xff));
		}
		else
		{
			theApp.str.Format("  %X    ",(Databuffer[i]&0xff));
		}
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	}	

	theApp.str = "\n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	/* 发送命令，使DSP先定入固定的数据 */
	myRequest.direction = 0;
	myRequest.index = 0;
	myRequest.value = USB_BULK2_WRITE;			// USB_BULK_BACK
	myRequest.request =0xb6;
	// vendor specific request type (2)
	myRequest.requestType=2; 
    // recepient is device (0)
	myRequest.recepient=0;
	venderlong = 0;

	/* 发送命令 */
	Sx2SendVendorReq(theApp.UsbCurrentNum,
					 &myRequest,
					 &buffer1[0],
					 venderlong,
					 &recnBytes);

	/* 按默认的配置，读回Endpoint2的数据并显示 */
	bulkControl.pipeNum = 3;			// 一定要选对管道	3
	buffer = &Receidata[0];
	Sx2BulkdataTrans( theApp.UsbCurrentNum,
					  &bulkControl,
					  buffer,
					  length,
					  &recnBytes);
	
	theApp.str.Format("The data read back from the pipe %X is:  \n",bulkControl.pipeNum);
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str.Format("The data received is from  %X to %X \n",(buffer[0]&0xff),(buffer[length-1]&0xff));
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);

	theApp.str = "-----------------------------------------------------------  \n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	
	for(i=0; i<length; i++)
	{
		if(i%6 ==5)
		{
			theApp.str.Format("  %X  \n",(buffer[i]&0xff));
		}
		else
		{
			theApp.str.Format("  %X    ",(buffer[i]&0xff));
		}
		AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
	}	

	theApp.str = "\n";
	AfxGetMainWnd()->SendMessage(WM_COMMAND,IDC_OUTPUTSTR,NULL);
}
