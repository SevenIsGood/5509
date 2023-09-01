// USBDEMO.h : main header file for the USBDEMO application
//

#if !defined(AFX_USBDEMO_H__B1891941_AFD1_475D_B9EE_BC4B38624758__INCLUDED_)
#define AFX_USBDEMO_H__B1891941_AFD1_475D_B9EE_BC4B38624758__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "global.h"
#include "USBDEMODlg.h"
#include "OutputWin.h"

/////////////////////////////////////////////////////////////////////////////
// CUSBDEMOApp:
// See USBDEMO.cpp for the implementation of this class
//

class CUSBDEMOApp : public CWinApp
{
public:
	CUSBDEMOApp();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUSBDEMOApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL


public:
	int UsbCurrentNum;
	int UsbDeviecNum;
	CString str;
	
	//{{AFX_MSG(CUSBDEMOApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// The one and only CUSBIOAPPApp object

extern CUSBDEMOApp theApp;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USBDEMO_H__B1891941_AFD1_475D_B9EE_BC4B38624758__INCLUDED_)
