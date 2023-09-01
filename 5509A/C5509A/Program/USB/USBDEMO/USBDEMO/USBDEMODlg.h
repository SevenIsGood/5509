// USBDEMODlg.h : header file
//

#if !defined(AFX_USBDEMODLG_H__44AF1FCD_4B43_4111_B13F_329D9CA13238__INCLUDED_)
#define AFX_USBDEMODLG_H__44AF1FCD_4B43_4111_B13F_329D9CA13238__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// initial height of output window
#define INITIAL_OUTPUT_WIN_HEIGHT		200

#include "winuser.h"
#include "DialogPageCtrl.h"
#include "DlgPgDevice.h"
#include "DlgPgDesc.h"
#include "DlgPgPipes.h"
#include "DlgPgDevreq.h"
#include "OutputWinDlg.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CUSBDEMODlg dialog

class CUSBDEMODlg : public CDialog
{
// Construction
public:
	bool OutputWopen;

	COutputWinDlg MyOutputDlg;			// define a output window object

	CUSBDEMODlg(CWnd* pParent = NULL);	// standard constructor
	
	// update enable/disable state of controls
	void UpdateStateOfControls();
// Dialog Data
	//{{AFX_DATA(CUSBDEMODlg)
	enum { IDD = IDD_USBDEMO_DIALOG };
	CDialogPageCtrl	m_TabControlMain;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUSBDEMODlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	HICON m_hIcon;			// HICON cannot find definition

	CDlgPgDevice      m_DlgPgDevice;
	CDlgPgDesc        m_DlgPgDesc;
	CDlgPgPipes       m_DlgPgPipes;
	CDlgPgDevreq      m_DlgPgDevreq;
    

	// Generated message map functions
	//{{AFX_MSG(CUSBDEMODlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnOutputstr();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeTabMain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonOutputWindow();
	afx_msg void OnButtonClearOutputWindow();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USBDEMODLG_H__44AF1FCD_4B43_4111_B13F_329D9CA13238__INCLUDED_)
