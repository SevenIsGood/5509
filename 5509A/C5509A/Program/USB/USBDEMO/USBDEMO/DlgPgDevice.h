#if !defined(AFX_DLGPGDEVICE_H__5AFEAB79_E564_4938_8246_CA41802B40F6__INCLUDED_)
#define AFX_DLGPGDEVICE_H__5AFEAB79_E564_4938_8246_CA41802B40F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPgDevice.h : header file
//
#include "DialogPage.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgPgDevice dialog

class CDlgPgDevice : public CDialogPage
{
// Construction
public:
	void UpdateDeviceList();
	CDlgPgDevice(CWnd* pParent = NULL); // standard constructor
	UINT	ListBoxDevice[8];			// 存储扫描得到的设备索引
	BOOL	OpenMark;					// 当前选定的USB设备是否打开标志

// Dialog Data
	//{{AFX_DATA(CDlgPgDevice)
	enum { IDD = IDD_DIALOGPAGE_DEVICE };
	CListBox	m_ListBoxDevices;
	CButton	m_ButtonOpen;
	//}}AFX_DATA

	BOOL Create(CWnd* pParent = NULL) { return CDialog::Create(IDD,pParent); }
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPgDevice)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPgDevice)
	afx_msg void OnButtonClose();
	afx_msg void OnButtonOpen();
	afx_msg void OnButtonScan();
	afx_msg void OnSelchangeListDevices();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPGDEVICE_H__5AFEAB79_E564_4938_8246_CA41802B40F6__INCLUDED_)
