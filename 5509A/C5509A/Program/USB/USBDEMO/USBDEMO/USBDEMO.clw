; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDlgPgDesc
LastTemplate=CWinThread
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "USBDEMO.h"

ClassCount=10
Class1=CUSBDEMOApp
Class2=CUSBDEMODlg
Class3=CAboutDlg

ResourceCount=8
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_USBDEMO_DIALOG
Resource4=IDD_DIALOGPAGE_DEVREQ
Resource5=IDD_DIALOGPAGE_PIPES
Resource6=IDD_DIALOGPAGE_DEVICE
Resource7=IDD_DIALOGPAGE_DESC
Class4=CDlgPgDevice
Class5=CDlgPgDesc
Class6=CDlgPgDevreq
Class7=CDlgPgPipes
Class8=CDialogPageCtrl
Class9=COutputWinDlg
Class10=COutputWin
Resource8=IDD_OUTPUT_WINDOW

[CLS:CUSBDEMOApp]
Type=0
HeaderFile=USBDEMO.h
ImplementationFile=USBDEMO.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=CUSBDEMOApp

[CLS:CUSBDEMODlg]
Type=0
HeaderFile=USBDEMODlg.h
ImplementationFile=USBDEMODlg.cpp
Filter=W
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_BUTTON_CLEAR_OUTPUT_WINDOW

[CLS:CAboutDlg]
Type=0
HeaderFile=USBDEMODlg.h
ImplementationFile=USBDEMODlg.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308481
Control3=IDC_STATIC,static,1342308353
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308353

[DLG:IDD_USBDEMO_DIALOG]
Type=1
Class=CUSBDEMODlg
ControlCount=4
Control1=IDCANCEL,button,1342242816
Control2=IDC_TAB_MAIN,SysTabControl32,1342177280
Control3=IDC_BUTTON_OUTPUT_WINDOW,button,1342242816
Control4=IDC_BUTTON_CLEAR_OUTPUT_WINDOW,button,1342242816

[DLG:IDD_DIALOGPAGE_DEVICE]
Type=1
Class=CDlgPgDevice
ControlCount=8
Control1=IDC_STATIC,button,1342177287
Control2=IDC_BUTTON_SCAN,button,1342242816
Control3=IDC_STATIC,static,1342308353
Control4=IDC_LIST_DEVICES,listbox,1352728835
Control5=IDC_STATIC,button,1342177287
Control6=IDC_STATIC,static,1342308353
Control7=IDC_BUTTON_OPEN,button,1342242816
Control8=IDC_BUTTON_CLOSE,button,1342242816

[DLG:IDD_DIALOGPAGE_DESC]
Type=1
Class=CDlgPgDesc
ControlCount=9
Control1=IDC_STATIC,button,1342177287
Control2=IDC_BUTTON_DEVICE_DESC,button,1342242816
Control3=IDC_BUTTON_STRING_DESC,button,1342242816
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT_STRING_DESC,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_EDIT_STRING_DESC_LID,edit,1350631552
Control8=IDC_BUTTON_DEVICE_QUALIFIER_DESC,button,1342242816
Control9=IDC_STATIC,button,1342177287

[DLG:IDD_DIALOGPAGE_DEVREQ]
Type=1
Class=CDlgPgDevreq
ControlCount=17
Control1=IDOK,button,1342242817
Control2=IDC_COMBO_DIRECTION,combobox,1344340226
Control3=IDC_STATIC,static,1342308353
Control4=IDC_COMBO_TYPE,combobox,1344340226
Control5=IDC_STATIC,static,1342308353
Control6=IDC_COMBO_RECIPIENT,combobox,1344340226
Control7=IDC_STATIC,static,1342308353
Control8=IDC_STATIC,static,1342308353
Control9=IDC_STATIC,static,1342308353
Control10=IDC_STATIC,static,1342308353
Control11=IDC_EDIT_BREQUEST,edit,1350631552
Control12=IDC_EDIT_WVALUE,edit,1350631552
Control13=IDC_EDIT_WINDEX,edit,1350631552
Control14=IDC_EDIT_BUFFER,edit,1350631552
Control15=IDC_STATIC,static,1342308353
Control16=IDC_EDIT_BUFFER_SIZE,edit,1350631552
Control17=IDC_STATIC,static,1342308353

[DLG:IDD_DIALOGPAGE_PIPES]
Type=1
Class=CDlgPgPipes
ControlCount=5
Control1=IDC_BUTTON_GETPIPINF,button,1342242816
Control2=IDC_STATIC,button,1342177287
Control3=IDC_BUTTON_PIPTOWIN,button,1342242816
Control4=IDC_BUTTON_PIPTOFILE,button,1342242816
Control5=IDC_BUTTON_FILETOPIP,button,1342242816

[DLG:IDD_OUTPUT_WINDOW]
Type=1
Class=COutputWinDlg
ControlCount=1
Control1=IDC_EDIT_OUTPUTWIN,edit,1352728708

[CLS:CDlgPgDevice]
Type=0
HeaderFile=DlgPgDevice.h
ImplementationFile=DlgPgDevice.cpp
BaseClass=CDialogPage
Filter=D
VirtualFilter=dWC
LastObject=IDC_LIST_DEVICES

[CLS:CDlgPgDesc]
Type=0
HeaderFile=DlgPgDesc.h
ImplementationFile=DlgPgDesc.cpp
BaseClass=CDialogPage
Filter=D
LastObject=IDC_EDIT_ANY_DESC_INDEX
VirtualFilter=dWC

[CLS:CDlgPgDevreq]
Type=0
HeaderFile=DlgPgDevreq.h
ImplementationFile=DlgPgDevreq.cpp
BaseClass=CDialogPage
Filter=D
LastObject=CDlgPgDevreq
VirtualFilter=dWC

[CLS:CDlgPgPipes]
Type=0
HeaderFile=DlgPgPipes.h
ImplementationFile=DlgPgPipes.cpp
BaseClass=CDialogPage
Filter=D
LastObject=IDC_BUTTON_GETPIPINF
VirtualFilter=dWC

[CLS:CDialogPageCtrl]
Type=0
HeaderFile=DialogPageCtrl.h
ImplementationFile=DialogPageCtrl.cpp
BaseClass=CTabCtrl
Filter=W
LastObject=CDialogPageCtrl

[CLS:COutputWinDlg]
Type=0
HeaderFile=OutputWinDlg.h
ImplementationFile=OutputWinDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=COutputWinDlg

[CLS:COutputWin]
Type=0
HeaderFile=OutputWin.h
ImplementationFile=OutputWin.cpp
BaseClass=CWinThread
Filter=N
LastObject=COutputWin

