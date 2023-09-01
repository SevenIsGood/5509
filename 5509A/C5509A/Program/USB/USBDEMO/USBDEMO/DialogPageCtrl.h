#if !defined(AFX_DIALOGPAGECTRL_H__C0DDBDBC_D156_4514_BCD8_83B664CE793E__INCLUDED_)
#define AFX_DIALOGPAGECTRL_H__C0DDBDBC_D156_4514_BCD8_83B664CE793E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogPageCtrl.h : header file
//
#include "DialogPage.h"

#define MAX_DIALOG_PAGE     16
/////////////////////////////////////////////////////////////////////////////
// CDialogPageCtrl window

class CDialogPageCtrl : public CTabCtrl
{
// Construction
public:
	CDialogPageCtrl();

// Attributes
public:

// Operations
public:

	// tab control support functions
  BOOL AddDialogPage(CDialogPage *DlgPg, UINT CaptionID);
  BOOL AddDialogPage(CDialogPage *DlgPg, const char *Caption);
  void UpdatePageSelection();

  // call UpdateData on all dialog pages
  BOOL UpdateAllData( BOOL bSaveAndValidate /*= TRUE*/ );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogPageCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDialogPageCtrl();

//////////////////////////////////////////////////////
protected:
  // dialog page table
  CDialogPage *m_DialogPage[MAX_DIALOG_PAGE];

  int m_IndexCounter;	
	// Generated message map functions
protected:
	//{{AFX_MSG(CDialogPageCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGPAGECTRL_H__C0DDBDBC_D156_4514_BCD8_83B664CE793E__INCLUDED_)
