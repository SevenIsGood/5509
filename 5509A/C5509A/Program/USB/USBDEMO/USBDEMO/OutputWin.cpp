// OutputWin.cpp : implementation file
//

#include "stdafx.h"
#include "USBDEMO.h"
#include "OutputWin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputWin

IMPLEMENT_DYNCREATE(COutputWin, CWinThread)		// 动态的创建用户进程

COutputWin::COutputWin()
{
}

COutputWin::~COutputWin()
{
}

BOOL COutputWin::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	BOOL succ = TRUE;

	// get/create pointer to parent win
    //CWnd* pParent = CWnd::FromHandle(m_hwndParent);
    // use Desktop
    CWnd* pParent = NULL;

	CWnd* m_pMainWnd;	// 我添加的窗口句柄定义

    // create the dialog window, this must be done from this thread context
    succ = m_Dialog.Create(COutputWinDlg::IDD, pParent);

	// It is important to set CWinThread::m_pMainWnd to the user interface
	// window.  This is required so that when the m_pMainWnd is destroyed,
	// the CWinThread is also automatically destroyed.  For insight into
	// how the CWinThread is automatically destroyed when the m_pMainWnd
	// window is destroyed, see the implementation of CWnd::OnNcDestroy
	// in wincore.cpp of the MFC sources.
	if (succ) 
	{
		m_pMainWnd = &m_Dialog;

		// save window handle
		m_hWndMainWindow = m_pMainWnd->m_hWnd;

		// don't delete COutputWin object at thread termination
		// this is done by CloseAndTerminate()
		m_bAutoDelete = FALSE;
	}

	return succ;
}

int COutputWin::ExitInstance()
{
	// invalidate handle
	m_hWndMainWindow = NULL;
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(COutputWin, CWinThread)
	//{{AFX_MSG_MAP(COutputWin)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputWin message handlers

BOOL COutputWin::CloseAndTerminate()
{
	// destroy output window,
	// this terminates the thread, too
	SendMessageToMainWnd(WM_USER_DESTROY_OUTPUT_WIN,0,0);
  
	// wait for termination of the thread
	if ( m_hThread!=NULL ) 
	{
		::WaitForSingleObject(m_hThread,INFINITE);
	}

	// delete this object
	delete this;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
/* 下面是我特意添加的备用代码										*/
//////////////////////////////////////////////////////////////////////
/*
Enables objects of CObject-derived classes to be created dynamically at run time when used with the DECLARE_DYNCREATE macro.
IMPLEMENT_DYNCREATE(class_name, base_class_name )
Parameters
class_name 
The actual name of the class. 
base_class_name 
The actual name of the base class. 
Remarks
The framework uses this ability to create new objects dynamically, for example, when it reads an object from disk during serialization. Add the IMPLEMENT_DYNCREATE macro in the class implementation file. For more information, see CObject Class Topics.
If you use the DECLARE_DYNCREATE and IMPLEMENT_DYNCREATE macros, you can then use the RUNTIME_CLASS macro and the CObject::IsKindOf member function to determine the class of your objects at run time. 
If DECLARE_DYNCREATE is included in the class declaration, then IMPLEMENT_DYNCREATE must be included in the class implementation.

*/