#if !defined(AFX_OUTPUTWIN_H__31F85839_B903_4AAF_893B_9B9D0627F98B__INCLUDED_)
#define AFX_OUTPUTWIN_H__31F85839_B903_4AAF_893B_9B9D0627F98B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OutputWin.h : header file
//

#include "OutputWinDlg.h"

/////////////////////////////////////////////////////////////////////////////
// COutputWin thread

class COutputWin : public CWinThread
{
	DECLARE_DYNCREATE(COutputWin)
protected:
	COutputWin();           // protected constructor used by dynamic creation
  
// Attributes
public:
  
// Operations
public:
	BOOL CloseAndTerminate();

	// embedded dialog window
    COutputWinDlg m_Dialog;
	// handle of main window (dialog)
	HWND m_hWndMainWindow;

	// send message to output window
    LRESULT SendMessageToMainWnd(UINT Msg, WPARAM wParam, LPARAM lParam)
      { return (m_hWndMainWindow==NULL) ? (-1) : ::SendMessage(m_hWndMainWindow,Msg,wParam,lParam); }


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputWin)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	
// Implementation
protected:
	virtual ~COutputWin();

	// Generated message map functions
	//{{AFX_MSG(COutputWin)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTWIN_H__31F85839_B903_4AAF_893B_9B9D0627F98B__INCLUDED_)
