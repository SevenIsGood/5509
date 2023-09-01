#if !defined(_DIALOGPAGE_H_)
#define _DIALOGPAGE_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogPage dialog

class CDialogPage : public CDialog
{
// Construction
public:
  CDialogPage( UINT nIDTemplate, CWnd* pParentWnd = NULL );

// interface
public:

  void SetTabControlIndex(int Index) { m_TabControlIndex = Index; }
  int  GetTabControlIndex() { return m_TabControlIndex; }
  

  void UpdateStateOfControls();


  // this handler is called with:
  //   TRUE  when the dialog page is selected
  //   FALSE when the dialog page is deselected
  virtual void OnSelectionChange(BOOL bSelected);


// Implementation
protected:

  // get pointer to array of control IDs, terminated with -1
  virtual const int* GetControlIdArray();

  // overloaded from CDialog
  void OnOK();
  void OnCancel();



  int m_TabControlIndex;

};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(_DIALOGPAGE_H_)

//////////////////////////////////////////////////////////////////////////
/* 下面是我添加的备用代码												*/
//////////////////////////////////////////////////////////////////////////
/*
// Called by the framework to exchange and validate dialog data，Never call
// this function directly. It is called by the UpdateData member function
virtual void DoDataExchange(
   CDataExchange* pDX 
);

//Call this member function to initialize data in a dialog box, 
//or to retrieve and validate dialog data.
BOOL UpdateData(
   BOOL bSaveAndValidate = TRUE 
);
//bSaveAndValidate: Flag that indicates whether dialog box is
// being initialized (FALSE) or data is being retrieved (TRUE). 
*/
