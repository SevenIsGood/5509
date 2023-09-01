// DialogPage.cpp : implementation file
//

#include "stdafx.h"
#include "USBDEMO.h"
#include "DialogPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogPage dialog

CDialogPage::CDialogPage( UINT nIDTemplate, CWnd* pParentWnd /*= NULL*/)
  : CDialog(nIDTemplate, pParentWnd)
{

	m_TabControlIndex = -1;


}


void CDialogPage::OnOK()
{
}


void CDialogPage::OnCancel()
{
}



void CDialogPage::OnSelectionChange(BOOL bSelected)
{
	if ( bSelected ) 
	{
		// page is shown on screen
	} 
	else 
	{
		// page was hidden
	}
}




void CDialogPage::UpdateStateOfControls()
{
	// get pointer to array of control IDs by calling virtual function
	const int* pID = GetControlIdArray();
	// BOOL enable = g_UsbIo.IsOpen();
	BOOL enable = TRUE;

	while ( (*pID) != -1 )		
	{
		CWnd* wnd = GetDlgItem(*pID);
		if ( wnd!=NULL ) 
		{
			wnd->EnableWindow(enable);
		} 
    pID++;
	}
}



const int* CDialogPage::GetControlIdArray()
{
	static const int ControlIds[] = 
	{
		-1 // end marker
	};
	 return ControlIds;
}

