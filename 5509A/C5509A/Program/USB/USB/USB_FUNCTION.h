
/****************************************************************/
/*	文件名:		USB_FUNCTION.h 									*/
/* 	功能:		声明5509片上usb的配置和操作函数					*/
/*	设计者		陈湘和											*/
/*  设计日期:	10/20/2004										*/
/****************************************************************/

#ifndef USB_FUNCTION_h
#define USB_FUNCTION_h

/* 节点和数据结构初始化 */
extern USB_SetupStruct USB0_SetupPkt;				// 初始化一个Setup数据结构

/* 控制节点0的事件处理设置 */
extern Uint16 EpIn0Event_mask;

extern Uint16 EpOut0Event_mask; 

extern Uint16 CtrlDataLen;							// 控制节点0的数据域长度	
				   	  
extern USB_EpObj 	EndptObjOut0,	EndptObjIn0, 	// 初始化控制endpoint0 
					EndptObjOut1,	EndptObjIn1, 	// 初始化块操作节点1-7
					EndptObjOut2,	EndptObjIn2, 
					EndptObjOut3,	EndptObjIn3,
					EndptObjOut4,	EndptObjIn4,
					EndptObjOut5,	EndptObjIn5,
					EndptObjOut6,	EndptObjIn6,
					EndptObjOut7,	EndptObjIn7;
					
/* 声明全局变量 */
extern Uint16 usbbuffer[200];
extern Uint16 usbflag;
extern Uint16 usbreceilen;							//	用来暂存从PC机传来的数据,以便读回			
extern USB_EpHandle hEpObjArray[];

/****************************************************************/
/*	函数名:		USB_EndpointInit() 								*/
/* 	功能:		对5509片上的USB节点进行初始化					*/
/*  参数:														*/
/* 				EpIn0EventMark---输入控制节点中断事件的或组合	*/
/* 				EpOut0EventMark---输出控制节点中断事件的或组合	*/
/****************************************************************/
void USB_EndpointInit(Uint16 EpIn0EventMark, Uint16 EpOut0EventMark);

/****************************************************************/
/*	函数名:		USB_EnableDInt() 								*/
/* 	功能:		使能块输出节点(OUT1-OUT7)中断					*/
/*  参数:														*/
/*  			hEp[]---指向一个NULL终止的已经初始化的节点对象	*/
/*						句柄数组								*/
/****************************************************************/
extern void USB_EnableDInt(USB_EpHandle hEp[]);

/****************************************************************/
/*	函数名:		USB_ReceivData() 								*/
/* 	功能:		接收指定块输出传输节点的数据					*/
/*  参数:														*/
/*  			EpNum----USB CSL定义的节点号					*/
/*				*DataPtr-指向接收数据缓冲区的指针				*/
/*				*ByteCnt-指向存储接收到的总字节个数的变量地址	*/
/*	返回值:		如果成功,返回USB_TURE,否则返回USB_FALSE			*/
/****************************************************************/
extern USB_Boolean USB_ReceivData(USB_EpNum EpNum, Uint16 *DataPtr, Uint16 *ByteCnt);

/****************************************************************/
/*	函数名:		USB_InitConfig() 								*/
/* 	功能:		对5509片上的USB进行初始化配置					*/
/*  参数:														*/
/*  			hEp[]---指向一个NULL终止的已经初始化的节点对象	*/
/*						句柄数组								*/
/* 				EpIn0EventMark----输入控制节点中断事件的或组合	*/
/* 				EpOut0EventMark---输出控制节点中断事件的或组合	*/
/****************************************************************/
extern void USB_InitConfig(USB_EpHandle hEp[], Uint16 EpIn0EventMark, Uint16 EpOut0EventMark);

/****************************************************************/
/*	函数名:		USB_DefaultDeal() 								*/
/* 	功能:		5509片上的USB进入死循环后,不断地检测状态信息,	*/
/*				如果满足相应的系统默认状态设置,就执行相应的动作	*/
/*  参数:														*/
/* 				无												*/
/****************************************************************/
extern void USB_DefaultDeal(void);

/****************************************************************/
/*	函数名:		USB_SendData() 									*/
/* 	功能:		向指定块输入传输节点发送数据					*/
/*  参数:														*/
/*  			hEp------指向一个已经初始化的节点对象的句柄		*/
/*				ByteCnt--欲发送的总字节个数						*/
/*				*DataPtr-指向数据缓冲区的指针					*/
/*				Flags----USB数据传输标志的或组合				*/	
/*	返回值:		如果成功,返回USB_TURE,否则返回USB_FALSE			*/
/****************************************************************/
extern USB_Boolean USB_SendData(USB_EpHandle hEp, Uint16 ByteCnt, void *DataPtr, USB_IOFLAGS Flags);

/****************************************************************/
/*	函数名:		USB_SendCtrlData() 								*/
/* 	功能:		向控制节点0发送指定长度的数据					*/
/*  参数:														*/
/*				*DataPtr-指向发送数据缓冲区的指针				*/
/*				ByteCnt-发送数据长度				        	*/
/*				Flags----USB数据传输标志的或组合				*/
/*	返回值:		如果成功,返回USB_TURE,否则返回USB_FALSE			*/
/****************************************************************/
extern USB_Boolean USB_SendCtrlData(Uint16 *DataPtr, Uint16 ByteCnt, USB_IOFLAGS Flags);

/****************************************************************/
/*	函数名:		USB_ReceivCtrlData() 							*/
/* 	功能:		从控制节点0接收指定长度的数据					*/
/*  参数:														*/
/*				*DataPtr-指向接收数据缓冲区的指针				*/
/*				ByteCnt-接收数据长度				        	*/
/*				Flags----USB数据传输标志的或组合				*/
/*	返回值:		如果成功,返回USB_TURE,否则返回USB_FALSE			*/
/****************************************************************/
extern USB_Boolean USB_ReceivCtrlData(Uint16 *DataPtr, Uint16 ByteCnt, USB_IOFLAGS Flags);

#endif

/****************************************************************/
/*	No more													 	*/
/****************************************************************/
