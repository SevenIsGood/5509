#if !defined(AFX_OUTPUTWINDLG_H__A41B1D41_92FF_49FF_AD53_EC2689FF16B5__INCLUDED_)
#define AFX_OUTPUTWINDLG_H__A41B1D41_92FF_49FF_AD53_EC2689FF16B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OutputWinDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COutputWinDlg dialog

class COutputWinDlg : public CDialog
{
// Construction
public:
	COutputWinDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COutputWinDlg)
	enum { IDD = IDD_OUTPUT_WINDOW };
	CEdit	m_EditControl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputWinDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Interface
public:
  
	void UpdateDisplayOn()  { m_EditControl.SetRedraw(TRUE); }
	void UpdateDisplayOff() { m_EditControl.SetRedraw(FALSE); }

	void ScrollDisplay()
    { m_EditControl.LineScroll(m_EditControl.GetLineCount(),0); }

	void Clear();

	void OutputString(const char *s);


// Implementation
protected:
	void UpdateClientWindow();
	CFont m_font;

	// Generated message map functions
	//{{AFX_MSG(COutputWinDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnChangeEditOutputwin();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTWINDLG_H__A41B1D41_92FF_49FF_AD53_EC2689FF16B5__INCLUDED_)

//////////////////////////////////////////////////////////////////////
/* 下面是我添加的备用代码											*/
//////////////////////////////////////////////////////////////////////
/*
// To set and retrieve text from a CEdit object, use the CWnd member
// functions SetWindowText and GetWindowText, which set or get the
// entire contents of an edit control, even if it is a multiline control

//An application calls SetRedraw to allow changes to be redrawn or to 
//prevent changes from being redrawn.
void SetRedraw(
   BOOL bRedraw = TRUE 
);
//bRedraw: Specifies the state of the redraw flag. If this parameter
//		is TRUE, the redraw flag is set; if FALSE, the flag is cleared. 
//Remarks:
// This member function sets or clears the redraw flag. While the redraw 
// flag is cleared, the contents will not be updated after each change 
// and will not be repainted until the redraw flag is set. For example, 
// an application that needs to add several items to a list box can clear
// the redraw flag, add the items, and then set the redraw flag. Finally,
// the application can call the Invalidate or InvalidateRect member 
// function to cause the list box to be repainted
*/