#if !defined(AFX_DLGPGDESC_H__ABAFDC9C_5C20_4F31_8E9A_CB3559FE6550__INCLUDED_)
#define AFX_DLGPGDESC_H__ABAFDC9C_5C20_4F31_8E9A_CB3559FE6550__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPgDesc.h : header file
//
#include "DialogPage.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgPgDesc dialog

class CDlgPgDesc : public CDialogPage
{
// Construction
public:
	CDlgPgDesc(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPgDesc)
	enum { IDD = IDD_DIALOGPAGE_DESC };
	int		m_StringIndex;
	int		m_StringLanguageID;
	//}}AFX_DATA

	BOOL Create(CWnd* pParent = NULL) { return CDialog::Create(IDD,pParent); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPgDesc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPgDesc)
	afx_msg void OnButtonDeviceDesc();
	afx_msg void OnButtonDeviceQualifierDesc();
	afx_msg void OnButtonStringDesc();
	afx_msg void OnChangeEditStringDesc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPGDESC_H__ABAFDC9C_5C20_4F31_8E9A_CB3559FE6550__INCLUDED_)
