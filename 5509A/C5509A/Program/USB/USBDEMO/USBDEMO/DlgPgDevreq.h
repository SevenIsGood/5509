#if !defined(AFX_DLGPGDEVREQ_H__83DAC00F_BAD6_4773_945C_707083DE5DDC__INCLUDED_)
#define AFX_DLGPGDEVREQ_H__83DAC00F_BAD6_4773_945C_707083DE5DDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPgDevreq.h : header file
//
#include "DialogPage.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgPgDevreq dialog

class CDlgPgDevreq : public CDialogPage
{
// Construction
public:
	char m_Endpoint0data[64];
	CDlgPgDevreq(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPgDevreq)
	enum { IDD = IDD_DIALOGPAGE_DEVREQ };
	CEdit	m_ctrlwvalue;
	CEdit	m_ctrlwindex;
	CEdit	m_ctrlbuffersize;
	CEdit	m_ctrlbuffer;
	CEdit	m_ctrlbrequest;
	CComboBox	m_ctrltype;
	CComboBox	m_ctrlrecipient;
	CComboBox	m_ctrldirection;
	CString	m_strdirection;
	CString	m_strrecipient;
	CString	m_strtype;
	int m_intdirection;
	int m_intrecipient;
	int m_inttype;
	int		m_brequest;
	BYTE	m_windex;
	BYTE	m_wvalue;
	CString	m_strbuffer;
	int		m_buffersize;
	//}}AFX_DATA

	BOOL Create(CWnd* pParent = NULL) { return CDialog::Create(IDD,pParent); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPgDevreq)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPgDevreq)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnEditchangeComboDirection();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int chartonumber(char str);
	CString m_sDirSelection;
	void strtodata();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPGDEVREQ_H__83DAC00F_BAD6_4773_945C_707083DE5DDC__INCLUDED_)
