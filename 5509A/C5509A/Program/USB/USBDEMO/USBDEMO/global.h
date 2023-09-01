#if !defined(_GLOBAL_H_)
#define _GLOBAL_H_



// class CUsbIo
//#include "usbio.h"
//#include "dump.h"

struct EnumBoxEntry;


/////////////////////////////////////////////////////////////////////////////
// global defines

// averaging interval to be set for mean data rate calculation
#define MEANRATE_AVERAGING_INTERVAL_MS		1000	// ms
// timer interval used for updating pipe statistics
#define STATISTICS_UPDATE_INTERVAL_MS			500		// ms


#define NL "\r\n"
#define MAX_ERROR_MSG_LENGTH 256
#define MAX_DESCRIPTOR_BUFFER_SIZE 2047


// error flags
#define E_FATAL     0x00000001
#define E_ERROR     0x00000002
#define E_WARNING   0x00000004
#define E_INFO      0x00000008



//
// private windows messages
//

// output window
#define WM_USER_PING_OUTPUT_WIN			(WM_USER+1)
#define WM_USER_PRINT_OUT           (WM_USER+3)
#define WM_USER_CLEAR_OUTPUT_WIN    (WM_USER+4)
#define WM_USER_SHOW_OUTPUT_WIN     (WM_USER+5)
#define WM_USER_MOVE_OUTPUT_WIN     (WM_USER+6)
#define WM_USER_SIZE_OUTPUT_WIN     (WM_USER+7)
#define WM_USER_DOCK_OUTPUT_WIN		(WM_USER+8)
#define WM_USER_DESTROY_OUTPUT_WIN  (WM_USER+10)

// worker threads
#define WM_USER_THREAD_TERMINATED   (WM_USER+100)


// initial height of output window
//#define INITIAL_OUTPUT_WIN_HEIGHT		200



enum DIRECTION {
  HostToDevice,
  DeviceToHost
};


/////////////////////////////////////////////////////////////////////////////
// global variables

//extern CUsbIo g_UsbIo;

extern int g_DeviceNumber;

//extern const GUID g_UsbioID;
//extern HDEVINFO g_DevList;

// list containing the open pipe dialogs
//extern CObList g_PipeDialogList;

//extern EnumBoxEntry g_DirectionTab[];
//extern EnumBoxEntry g_RequestTypeTab[];
//extern EnumBoxEntry g_RecipientTab[];
//extern EnumBoxEntry g_PowerTab[];


/////////////////////////////////////////////////////////////////////////////
// global functions

// print to output window
//void PrintOut(const char *format, ...);

// print error message according to ErrorCode, returns ErrorCode
//DWORD PrintError(DWORD ErrorCode, DWORD Flags = E_ERROR, const char *Prefix=NULL);

// message box
//void DisplayMessageBox(const char *format, ...);

// return string for pipe type
//const char* PipeTypeStr(USBIO_PIPE_TYPE PipeType);

// return string for endpoint direction
//const char* EndpointDirectionStr(UCHAR EndpointAddress);



#define _MAKE_STR(x)  __MAKE_STR(x)
#define __MAKE_STR(x)   #x


#ifdef _DEBUG
#define OD(s) OutputDebugString(s)
#else
#define OD(s)
#endif



#endif
