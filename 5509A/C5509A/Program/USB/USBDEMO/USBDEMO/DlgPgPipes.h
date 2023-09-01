#if !defined(AFX_DLGPGPIPES_H__111DF1D4_FC67_4AC6_8AC3_C3AFE23DB64C__INCLUDED_)
#define AFX_DLGPGPIPES_H__111DF1D4_FC67_4AC6_8AC3_C3AFE23DB64C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPgPipes.h : header file
//
#include "DialogPage.h"

#define USB_BULK1_WRITE		0x02
#define USB_BULK2_WRITE		0x03
#define USB_BULK3_WRITE		0x04
#define USB_BULK4_WRITE		0x05
#define USB_BULK5_WRITE		0x06
#define USB_BULK6_WRITE		0x07
#define USB_BULK7_WRITE		0x08

/////////////////////////////////////////////////////////////////////////////
// CDlgPgPipes dialog

class CDlgPgPipes : public CDialogPage
{
// Construction
public:
	CDlgPgPipes(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPgPipes)
	enum { IDD = IDD_DIALOGPAGE_PIPES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	BOOL Create(CWnd* pParent = NULL) { return CDialog::Create(IDD,pParent); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPgPipes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPgPipes)
	afx_msg void OnButtonPiptowin();
	afx_msg void OnButtonPiptofile();
	afx_msg void OnButtonFiletopip();
	afx_msg void OnButtonGetpipinf();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int chartonumber(char str);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPGPIPES_H__111DF1D4_FC67_4AC6_8AC3_C3AFE23DB64C__INCLUDED_)
