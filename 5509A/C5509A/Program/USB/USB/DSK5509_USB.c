
/****************************************************************/
/*	文件名:		DSK5509_USB.c 									*/
/* 	功能:		实现5509片上usb的配置及与PC机的通信				*/
/*  设计日期:	10/20/2004										*/
/*	版权:		北京合众达电子技术有限责任公司					*/
/****************************************************************/

#include <stdio.h>
#include <csl.h>
#include <csl_irq.h>
#include <csl_pll.h>
#include <csl_usb.h>
#include <csl_chiphal.h>
#include "Descriptors.h"
#include "USB_FUNCTION.h"
#include "Global_Maco.h"


/* The example uses predefined CSL macros and symbolic  */
/* constants to create the correct register settings    */
/* needed to configure the usb.                       	*/

/* 锁相环的设置 */
PLL_Config  myConfig      = {
  0,    //IAI: the PLL locks using the same process that was underway 
                //before the idle mode was entered
  1,    //IOB: If the PLL indicates a break in the phase lock, 
                //it switches to its bypass mode and restarts the PLL phase-locking 
                //sequence
  10,    //PLL multiply value; multiply 24 times
  0             //Divide by 2 PLL divide value; it can be either PLL divide value 
                //(when PLL is enabled), or Bypass-mode divide value
                //(PLL in bypass mode, if PLL multiply value is set to 1)
};

interrupt void USB_ISR();

/* 配置USB时钟频率 */
void USB_initPLL(Uint16 inclk, Uint16 outclk, Uint16 plldiv);

void main(void)
{
	 /* Initialize CSL library - This is REQUIRED !!! */
	 CSL_init();
	 
	 /* Temporarily disable all maskable interrupts */
	 IRQ_globalDisable();
	 
	 /* 设置系统的运行速度为144MHz */
     PLL_config(&myConfig);
     
     /* 对USB进行初始化设置 */
     USB_InitConfig(hEpObjArray,EpIn0Event_mask,EpOut0Event_mask);
     
     /* 进入系统死循环 */
     for(;;)
     { 
     	/* 在该函数里面进行判断是否满足系统设置的默认状态,	*/
     	/* 如果满足就进行相应的处理,否则不进行任何处理		*/
     	USB_DefaultDeal();
     	
     	/* 下面为用户扩展的状态,用户也可以在此添加自己的状态处理函数 */
     	/* 块传输节点1和4系统保留用来演示功能						 */
     	if(usbflag == USB_CTRL0_READ)
     	{
     		usbflag = 0;
     		
     		/* 用户使用下面的接收函数时,必须使用CtrlDataLen这个系统定义的全局变量,	*/
     		/* 因为从主机传来的数据长度直接保存在该变量里 							*/
     		/* 注意该接收函数的数据缓冲区的第一个字用来存储接收长度的,所以用户在定义*/
     		/* 缓冲区长度时为实际数据长度加1										*/
     		
     		//这部分是设计时用作测试用的,用户可以参照使用
     	    USB_ReceivCtrlData((Uint16 *)&usbbuffer, CtrlDataLen, USB_IOFLAG_SWAP);
     	    
     	}
     	
     	if(usbflag == USB_BULK3_READ)		 						
    	{
    		usbflag = 0;
    		// 如果是PC机向块输出节点3传送数据,请用户添加自己的处理代码
   		}
   		
   		if(usbflag == USB_BULK4_READ)								
    	{
    		usbflag = 0;
    		// 如果是PC机向块输出节点4传送数据,请用户添加自己的处理代码
   		}
   		
   		if(usbflag == USB_BULK5_READ)								
    	{
    		usbflag = 0;
    		// 如果是PC机向块输出节点5传送数据,请用户添加自己的处理代码
   		}
   		
   		if(usbflag == USB_BULK6_READ)								
    	{
    		usbflag = 0;
    		// 如果是PC机向块输出节点6传送数据,请用户添加自己的处理代码
   		}
   		
   		if(usbflag == USB_BULK7_READ)								
    	{
    		usbflag = 0;
    		// 如果是PC机向块输出节点7传送数据,请用户添加自己的处理代码
   		}
   		
   		if(usbflag == USB_CTRL0_WRITE)
   		{
			usbflag = 0; 
			
			/* 用户使用下面的发送函数时,必须使用CtrlDataLen这个系统定义的全局变量,	*/
     		/* 因为从主机传来的数据长度直接保存在该变量里,							*/
     		/* 这样主机欲接收的长度与控制节点In0发送的长度保持一致					*/
     		/*	
     		这部分是设计时用作测试用的,用户可以参照使用
     		*/	
			usbbuffer[0] =0x0;
			usbbuffer[1] =0x0301;
			usbbuffer[2] =0x0705;
			usbbuffer[3] =0x0b09;
			usbbuffer[4] =0x0f0d; 
		    USB_SendCtrlData((Uint16 *)&usbbuffer, CtrlDataLen, USB_IOFLAG_NONE); 
		    		
   		}
   		
   		if(usbflag == USB_BULK3_WRITE)
   		{
   			usbflag = 0;
    		// 如果是PC机向块输入节点3读取数据,请用户添加自己的处理代码
   		}
   		
   		if(usbflag == USB_BULK4_WRITE)
   		{
   			usbflag = 0;
    		// 如果是PC机向块输入节点4读取数据,请用户添加自己的处理代码
   		}
   		
   		if(usbflag == USB_BULK5_WRITE)
   		{
   			usbflag = 0;
    		// 如果是PC机向块输入节点5读取数据,请用户添加自己的处理代码
   		}
   		
   		if(usbflag == USB_BULK6_WRITE)
   		{
   			usbflag = 0;
    		// 如果是PC机向块输入节点6读取数据,请用户添加自己的处理代码
   		}
   		
   		if(usbflag == USB_BULK7_WRITE)
   		{
   			usbflag = 0;
    		// 如果是PC机向块输入节点7读取数据,请用户添加自己的处理代码
   		}
     }	
}


/* USB中断函数 */
interrupt void USB_ISR()
{
	/* 调用USB事件的分配器 */
	USB_evDispatch();
}

/********************************************************************/
/*	No	More														*/
/********************************************************************/


