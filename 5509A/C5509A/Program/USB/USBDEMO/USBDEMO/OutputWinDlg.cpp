// OutputWinDlg.cpp : implementation file
//

#include "stdafx.h"
#include "USBDEMO.h"
#include "OutputWinDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputWinDlg dialog

COutputWinDlg::COutputWinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COutputWinDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COutputWinDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void COutputWinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COutputWinDlg)
	DDX_Control(pDX, IDC_EDIT_OUTPUTWIN, m_EditControl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COutputWinDlg, CDialog)
	//{{AFX_MSG_MAP(COutputWinDlg)
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_EN_CHANGE(IDC_EDIT_OUTPUTWIN, OnChangeEditOutputwin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputWinDlg message handlers

BOOL COutputWinDlg::OnInitDialog() 
{
	RECT rect;
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// init font
	BOOL succ = m_font.CreatePointFont(
                100,		//int nPointSize, 
                "Courier",  //LPCTSTR lpszFaceName,
                NULL        // CDC* pDC = NULL
                ); 
	if ( succ ) 
	{
		m_EditControl.SetFont(&m_font);
	} 
	else 
	{
		AfxMessageBox("Unable to initialize font.");
	}
  
	// set text limit to 512K
	m_EditControl.SetLimitText(512*1024);

	// initial position: 1,1
	GetWindowRect(&rect);
	MoveWindow(1,1,rect.right,rect.bottom,true); 
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COutputWinDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	// adjust client window (edit control)
	UpdateClientWindow();
}

void COutputWinDlg::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	
	// TODO: Add your message handler code here
	
}


void COutputWinDlg::UpdateClientWindow()
{
	CRect cr;
	// Copies the client coordinates of the CWnd client area into the structure pointed to by lpRect
	GetClientRect(cr);
	m_EditControl.MoveWindow(cr);
}


void COutputWinDlg::OutputString(const char *s)
{
	int len;
	int limit;
  
	// get current length
	len=m_EditControl.GetWindowTextLength(); 
	// get limit
	limit = m_EditControl.GetLimitText() - 2048;

	if ( len > limit ) 
	{
		// delete 8K text
		m_EditControl.SetSel(0,8192);
		m_EditControl.Clear();
		len=m_EditControl.GetWindowTextLength(); 
	}
	// append string
	m_EditControl.SetSel(len,len);
	// Call this function to replace the current selection in an edit control 
	// with the text specified by s
	m_EditControl.ReplaceSel(s, FALSE);
} 


void COutputWinDlg::Clear()
{ 
	// set new contents
	m_EditControl.SetWindowText("");
}

void COutputWinDlg::OnChangeEditOutputwin() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}
//////////////////////////////////////////////////////////////////////////
/* 下面代码是我特意添加的备用代码										*/
//////////////////////////////////////////////////////////////////////////
/*
// Copies the dimensions of the bounding rectangle of the CWnd object
// to the structure pointed to by lpRect.
void GetWindowRect(
   LPRECT lpRect 
) const;
//lpRect: Points to a CRect object or a RECT structure that will receive
//		  the screen coordinates of the upper-left and lower-right corners

// Changes the position and dimensions.
void MoveWindow(
   int x,
   int y,
   int nWidth,
   int nHeight,
   BOOL bRepaint = TRUE 
);
//bRepaint: Specifies whether CWnd is to be repainted. If TRUE, 
//			CWnd receives a WM_PAINT message in its OnPaint message
//			handler as usual. If this parameter is FALSE, no repainting 
//			of any kind occurs

//Copies the client coordinates of the CWnd client area into 
//the structure pointed to by lpRect.
void GetClientRect(
   LPRECT lpRect 
) const;
//lpRect: Points to a RECT structure or a CRect object to receive the 
//		client coordinates. The left and top members will be 0. 
//		The right and bottom members will contain the width and 
//		height of the window

//Creates a Windows child window and attaches it to the CWnd object.
virtual BOOL Create(
   LPCTSTR lpszClassName,
   LPCTSTR lpszWindowName,
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID,
   CCreateContext* pContext = NULL
);
//lpszClassName: Points to a null-terminated character string that names
//		 the Windows class (a WNDCLASS structure). The class name can
//		 be any name registered with the global AfxRegisterWndClass
//		 function or any of the predefined control-class names. 
//		 If NULL, uses the default CWnd attributes. 
//lpszWindowName: Points to a null-terminated character string that 
//		contains the window name. 
//dwStyle: Specifies the window style attributes. WS_POPUP cannot be used.
//		 If you wish to create a pop-up window, use CWnd::CreateEx instead. 
//rect: The size and position of the window, in client coordinates of 
//		pParentWnd. 
//pParentWnd:	The parent window. 
//nID:			The ID of the child window. 
//pContext:		The create context of the window. 
//Return Value:	Nonzero if successful; otherwise 0

//Centers a window relative to its parent.
void CenterWindow(
   CWnd* pAlternateOwner = NULL 
);
//pAlternateOwner:	Pointer to an alternate window relative to 
//					which it will be centered 

//Returns the window or control ID value for any child window, 
//not only that of a control in a dialog box.
int GetDlgCtrlID( ) const;
//	The numeric identifier of the CWnd child window if the function 
//	is successful; otherwise 0.

The DDX_Control function subclasses the control, specified by nIDC, of the dialog box, form view, or control view object.
void AFXAPI DDX_Control(
   CDataExchange* pDX,
   int nIDC,
   CWnd& rControl 
);
Parameters
pDX 
A pointer to a CDataExchange object. 
nIDC 
The resource ID of the control to be subclassed. 
rControl 
A reference to a member variable of the dialog box, form view, or control view object related to the specified control. 
Remarks
The pDX object is supplied by the framework when the DoDataExchange function is called. Therefore, DDX_Control should only be called within your override of DoDataExchange.
For more information about DDX, see Dialog Data Exchange and Validation

Copies the client coordinates of the CWnd client area into the structure pointed to by lpRect.
void GetClientRect(
   LPRECT lpRect 
) const;
Parameters
lpRect 
Points to a RECT structure or a CRect object to receive the client coordinates. The left and top members will be 0. The right and bottom members will contain the width and height of the window. 
Remarks
The client coordinates specify the upper-left and lower-right corners of the client area. Since client coordinates are relative to the upper-left corners of the CWnd client area, the coordinates of the upper-left corner are (0,0)

Changes the position and dimensions
void MoveWindow(
   LPCRECT lpRect,
   BOOL bRepaint = TRUE 
);
bRepaint 
Specifies whether CWnd is to be repainted. If TRUE, CWnd receives a WM_PAINT message in its OnPaint message handler as usual. If this parameter is FALSE, no repainting of any kind occurs. This applies to the client area, to the nonclient area (including the title and scroll bars), and to any part of the parent window uncovered as a result of CWnd's move. When this parameter is FALSE, the application must explicitly invalidate or redraw any parts of CWnd and parent window that must be redrawn. 
lpRect 
The CRect object or RECT structure that specifies the new size and position

Call this function to get the starting and ending character positions of the current selection (if any) in an edit control, using either the return value or the parameters.
DWORD GetSel( ) const;
void GetSel(
   int& nStartChar,
   int& nEndChar 
) const;
Parameters
nStartChar 
Reference to an integer that will receive the position of the first character in the current selection. 
nEndChar 
Reference to an integer that will receive the position of the first nonselected character past the end of the current selection. 

Call this function to replace the current selection in an edit control with the text specified by lpszNewText.
void ReplaceSel(
   LPCTSTR lpszNewText,
   BOOL bCanUndo = FALSE 
);
Parameters
lpszNewText 
Points to a null-terminated string containing the replacement text. 
bCanUndo 
To specify that this function can be undone, set the value of this parameter to TRUE . The default value is FALSE

Sets the window's title to the specified text.
void SetWindowText(
   LPCTSTR lpszString 
);
Parameters
lpszString 
Points to a CString object or null-terminated string to be used as the new title or control text. 
Remarks
If the window is a control, the text within the control is set. 
This function causes a WM_SETTEXT message to be sent to this window

*/

