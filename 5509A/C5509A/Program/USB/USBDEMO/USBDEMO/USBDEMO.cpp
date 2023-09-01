// USBDEMO.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "USBDEMO.h"
#include "USBDEMODlg.h"
#include "OutputWin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUSBDEMOApp

BEGIN_MESSAGE_MAP(CUSBDEMOApp, CWinApp)
	//{{AFX_MSG_MAP(CUSBDEMOApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUSBDEMOApp construction

CUSBDEMOApp::CUSBDEMOApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CUSBDEMOApp object

CUSBDEMOApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CUSBDEMOApp initialization

BOOL CUSBDEMOApp::InitInstance()
{
	AfxEnableControlContainer();	// Call this function in your application object's
									// InitInstance function to enable support 
									// for containment of OLE controls
	
	CWnd* m_pMainWnd;				// 我添加的窗口句柄定义			

	/*			//我添加的备用代码
	DWORD dwVersion = ::GetVersion(); 
	BOOL bWin31 = (dwVersion > 0x80000000 && (BYTE)dwVersion < 4); 
	if (bWin31) 
	{ 
		AfxMessageBox(IDP_WIN32S); 
		return FALSE; 
	} 
	*/

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	
	// 下面这句是我添加的备用代码
	//LoadStdProfileSettings(); // Load standard INI file options (including MRU) 

	CUSBDEMODlg dlg;

	m_pMainWnd = &dlg;

	int nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CUSBDEMOApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWinApp::ExitInstance();
}



