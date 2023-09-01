// USBDEMODlg.cpp : implementation file
//

#include "stdafx.h"
#include "USBDEMO.h"
#include "USBDEMODlg.h"
#include "DialogPageCtrl.h"
#include "OutputWinDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUSBDEMODlg dialog

CUSBDEMODlg::CUSBDEMODlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUSBDEMODlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUSBDEMODlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	
	// The pointer returned by AfxGetApp() function can be used to access application
	// information such as the main message-dispatch code or the topmost window
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// Loads the icon resource named by lpszResourceName or specified by nIDResource
	// from the executable file

}

void CUSBDEMODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUSBDEMODlg)
	DDX_Control(pDX, IDC_TAB_MAIN, m_TabControlMain);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUSBDEMODlg, CDialog)
	//{{AFX_MSG_MAP(CUSBDEMODlg)
	ON_COMMAND(IDC_OUTPUTSTR,OnOutputstr)	// This macro maps a command message to a member function
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	
	// Notifies a tab control's parent window that the currently selected tab has changed.
	// This message is sent in the form of a WM_NOTIFY message
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, OnSelchangeTabMain)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUT_WINDOW, OnButtonOutputWindow)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_OUTPUT_WINDOW, OnButtonClearOutputWindow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUSBDEMODlg message handlers

BOOL CUSBDEMODlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.

	// Evaluates an expression and, when the result is false,
	// prints a diagnostic message and aborts the program
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);	// Define a CMenu object
											// returns a handle to a copy of the Control menu currently in use
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			// Appends a new item to the end of a menu
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	// create all dialog page instances, parent window = Tab Control
	VERIFY( m_DlgPgDevice.Create(&m_TabControlMain) );
	VERIFY( m_DlgPgDesc.Create(&m_TabControlMain) );
	VERIFY( m_DlgPgPipes.Create(&m_TabControlMain) );
	VERIFY( m_DlgPgDevreq.Create(&m_TabControlMain) );

	// register our dialog page instances with the tab control
	m_TabControlMain.AddDialogPage(&m_DlgPgDevice,IDS_DIALOGPAGE_DEVICE);
	m_TabControlMain.AddDialogPage(&m_DlgPgDesc,IDS_DIALOGPAGE_DESC);
	m_TabControlMain.AddDialogPage(&m_DlgPgPipes,IDS_DIALOGPAGE_PIPES);
	m_TabControlMain.AddDialogPage(&m_DlgPgDevreq,IDS_DIALOGPAGE_DEVREQ);

	// update selection
    m_TabControlMain.UpdatePageSelection();
	
	// set initial state of controls
	UpdateStateOfControls();

	// fill device list
//	m_DlgPgDevice.UpdateDeviceList();

	// move the main window to its initial position
	CRect rcDlg;
	GetWindowRect(&rcDlg); 
	HWND hwndOwner = ::GetDesktopWindow(); 
	CRect rcOwner;
	::GetWindowRect(hwndOwner, &rcOwner); 
	int x = rcOwner.left + ((rcOwner.Width() - rcDlg.Width()) / 2) ;
	int y = rcOwner.top + ((rcOwner.Height() - rcDlg.Height() - INITIAL_OUTPUT_WIN_HEIGHT) / 2) ;
	if ( y >= 20 ) y -= 20;
	SetWindowPos(
      &wndTop, 
      x,
      y,
      0, 0,          // ignores size arguments 
      SWP_NOSIZE); 

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUSBDEMODlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUSBDEMODlg::OnPaint() 
{
	if (IsIconic())			// If window is not minimized
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);		// Default width and height of an icon
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUSBDEMODlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CUSBDEMODlg::OnSelchangeTabMain(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	m_TabControlMain.UpdatePageSelection();
	*pResult = 0;
}

// update enable/disable state of controls
void CUSBDEMODlg::UpdateStateOfControls()
{
  m_DlgPgDevice.UpdateStateOfControls();
  m_DlgPgDesc.UpdateStateOfControls();
  m_DlgPgPipes.UpdateStateOfControls();
  m_DlgPgDevreq.UpdateStateOfControls();

}

// create output window
void CUSBDEMODlg::OnButtonOutputWindow() 
{
	// show output window on screen
	MyOutputDlg.Create(IDD_OUTPUT_WINDOW,this);
	OutputWopen = TRUE;
	
}

// clear output window
void CUSBDEMODlg::OnButtonClearOutputWindow() 
{
	// TODO: Add your control notification handler code here
	MyOutputDlg.Clear();	
}

// output string 
void CUSBDEMODlg::OnOutputstr()
{
	if(OutputWopen == TRUE)
	{
		MyOutputDlg.OutputString(theApp.str);	// 输出换行在控件的属性栏中指定
	}
	
}

// cancel operation 
void CUSBDEMODlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
//////////////////////////////////////////////////////////////////////////////////////////////
/* 下面是我添加的备用代码																	*/
//////////////////////////////////////////////////////////////////////////////////////////////
/*
Allows the application to access the Control menu for copying and modification.
CMenu* GetSystemMenu(
   BOOL bRevert 
) const;
Parameters
bRevert 
Specifies the action to be taken. If bRevert is FALSE, GetSystemMenu returns a handle to a copy of the Control menu currently in use. This copy is initially identical to the Control menu but can be modified. If bRevert is TRUE, GetSystemMenu resets the Control menu back to the default state. The previous, possibly modified, Control menu, if any, is destroyed. The return value is undefined in this case. 
Return Value
Identifies a copy of the Control menu if bRevert is FALSE. If bRevert is TRUE, the return value is undefined. 
The returned pointer may be temporary and should not be stored for later use


Copies the dimensions of the bounding rectangle of the CWnd object to the structure pointed to by lpRect.
void GetWindowRect(
   LPRECT lpRect 
) const;
Parameters
lpRect 
Points to a CRect object or a RECT structure that will receive the screen coordinates of the upper-left and lower-right corners. 
Remarks
The dimensions are given in screen coordinates relative to the upper-left corner of the display screen. The dimensions of the caption, border, and scroll bars, if present, are included.

The GetDesktopWindow function returns a handle to the desktop window. The desktop window covers the entire screen. The desktop window is the area on top of which all icons and other windows are painted. 
Syntax
HWND GetDesktopWindow(VOID);
Return Value
The return value is a handle to the desktop window

Call this member function to invoke the modal dialog box and return the dialog-box result when done.
virtual INT_PTR DoModal( );
Return Value
An int value that specifies the value of the nResult parameter that was passed to the CDialog::EndDialog member function, which is used to close the dialog box. The return value is –1 if the function could not create the dialog box, or IDABORT if some other error occurred, in which case the Output window will contain error information from GetLastError.
Remarks
This member function handles all interaction with the user while the dialog box is active. This is what makes the dialog box modal; that is, the user cannot interact with other windows until the dialog box is closed.
If the user clicks one of the pushbuttons in the dialog box, such as OK or Cancel, a message-handler member function, such as OnOK or OnCancel, is called to attempt to close the dialog box. The default OnOK member function will validate and update the dialog-box data and close the dialog box with result IDOK, and the default OnCancel member function will close the dialog box with result IDCANCEL without validating or updating the dialog-box data. You can override these message-handler functions to alter their behavior.

Specifies whether CWnd is minimized (iconic).
BOOL IsIconic( ) const;
Return Value
Nonzero if CWnd is minimized; otherwise 0

The GetSystemMetrics function retrieves various system metrics (widths and heights of display elements) and system configuration settings. All dimensions retrieved by GetSystemMetrics are in pixels.
int GetSystemMetrics(
  int nIndex
);

Draws an icon on the device represented by the current CDC object.
BOOL DrawIcon(
   int x,
   int y,
   HICON hIcon 
);



 */
