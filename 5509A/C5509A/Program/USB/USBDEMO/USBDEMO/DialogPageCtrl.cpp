// DialogPageCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "USBDEMO.h"
#include "DialogPageCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogPageCtrl

CDialogPageCtrl::CDialogPageCtrl()
{
	int i;

	// init table
	for ( i=0; i<MAX_DIALOG_PAGE; i++ ) 
	{
		m_DialogPage[i] = NULL;		//让对话页指针都指向空
	}

	m_IndexCounter = 0;				//页计数从0开始

}

CDialogPageCtrl::~CDialogPageCtrl()
{
}

BOOL CDialogPageCtrl::AddDialogPage(CDialogPage *DlgPg, UINT CaptionID)
{
	CString caption;
  
	caption.LoadString(CaptionID);		//Reads a Windows string resource, identified by nID, 
										//into an existing CStringT object.

	return AddDialogPage(DlgPg,(const char *)caption);
}


BOOL CDialogPageCtrl::AddDialogPage(CDialogPage *DlgPg, const char *Caption)
{
	TC_ITEM tc;
	int i;
	BOOL succ = FALSE;

	ZeroMemory(&tc,sizeof(tc));
  
	tc.mask = TCIF_TEXT|TCIF_STATE;
	tc.pszText = (char*)Caption;		// Pointer to a null-terminated string that contains 
										// the tab text when item information is being set. 
										// If item information is being retrieved, this member specifies 
										// the address of the buffer that receives the tab text
	
	tc.dwState = TCIS_BUTTONPRESSED;	// The tab control item is selected. This state is only 
										// meaningful if the TCS_BUTTONS style flag has been set

	i = InsertItem(m_IndexCounter,&tc);	// If operation correctly ,return no-zero value

	if ( i>=0 && i<MAX_DIALOG_PAGE )
	{
		m_DialogPage[i] = DlgPg;
		DlgPg->SetTabControlIndex(i);	// 没有发现该成员函数
		m_IndexCounter++;
		succ = TRUE;
	} 

	return succ;
}



void CDialogPageCtrl::UpdatePageSelection()
{
	int sel,i;
  
	sel = GetCurSel();		// Determines the currently selected tab in a tab control
							// Returns the index of the selected tab if successful

	if ( sel < 0 )
	{
		// no selection
		return;
	}

	// hide all pages
	for ( i=0; i<MAX_DIALOG_PAGE; i++ )
	{
		CDialogPage *pg = m_DialogPage[i];
		if ( pg == NULL ) 
		{
			break;			// 如果是空页则跳过
		}
		else				// The IsWindow function determines whether the specified window handle identifies an existing window
		{					// Determines whether the window is visible
			if ( ::IsWindow(pg->m_hWnd) && pg->IsWindowVisible() ) 
			{
				pg->ShowWindow(SW_HIDE);
				pg->OnSelectionChange(FALSE);
			}
		}
	}
  
	// get rect of text item
	CRect rc;
	GetItemRect(sel, &rc);
	// show selected page
	if ( sel>=0 && sel<MAX_DIALOG_PAGE ) 
	{
		CDialogPage *pg = m_DialogPage[sel];
		if ( pg != NULL ) 
		{
			if ( ::IsWindow(pg->m_hWnd) ) 
			{
				// Changes the size, position,
				// and ordering of child, pop-up, and top-level windows
				pg->SetWindowPos(NULL, 4, rc.Height()+4, 0,0, 
				SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW);

				pg->OnSelectionChange(TRUE);	// 这个函数的代码还没有编写完成
			}
		}
	}
}


BOOL CDialogPageCtrl::UpdateAllData( BOOL bSaveAndValidate /*= TRUE*/ )
{

	for ( int i=0; i<MAX_DIALOG_PAGE; i++ ) 
	{
		CDialogPage *pg = m_DialogPage[i];
		if ( pg == NULL )
		{
			break;
		} 
		else 
		{			// Call this member function to initialize data in a dialog box, 
					// or to retrieve and validate dialog data
			if ( !pg->UpdateData(bSaveAndValidate) ) 
			{
				// display failed tab
				SetCurSel(pg->GetTabControlIndex());
				return FALSE;
			}
		}
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CDialogPageCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CDialogPageCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogPageCtrl message handlers


