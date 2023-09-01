
/****************************************************************/
/*	�ļ���:		DSK5509_USB.c 									*/
/* 	����:		ʵ��5509Ƭ��usb�����ü���PC����ͨ��				*/
/*  �������:	10/20/2004										*/
/*	��Ȩ:		�������ڴ���Ӽ����������ι�˾					*/
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

/* ���໷������ */
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

/* ����USBʱ��Ƶ�� */
void USB_initPLL(Uint16 inclk, Uint16 outclk, Uint16 plldiv);

void main(void)
{
	 /* Initialize CSL library - This is REQUIRED !!! */
	 CSL_init();
	 
	 /* Temporarily disable all maskable interrupts */
	 IRQ_globalDisable();
	 
	 /* ����ϵͳ�������ٶ�Ϊ144MHz */
     PLL_config(&myConfig);
     
     /* ��USB���г�ʼ������ */
     USB_InitConfig(hEpObjArray,EpIn0Event_mask,EpOut0Event_mask);
     
     /* ����ϵͳ��ѭ�� */
     for(;;)
     { 
     	/* �ڸú�����������ж��Ƿ�����ϵͳ���õ�Ĭ��״̬,	*/
     	/* �������ͽ�����Ӧ�Ĵ���,���򲻽����κδ���		*/
     	USB_DefaultDeal();
     	
     	/* ����Ϊ�û���չ��״̬,�û�Ҳ�����ڴ�����Լ���״̬������ */
     	/* �鴫��ڵ�1��4ϵͳ����������ʾ����						 */
     	if(usbflag == USB_CTRL0_READ)
     	{
     		usbflag = 0;
     		
     		/* �û�ʹ������Ľ��պ���ʱ,����ʹ��CtrlDataLen���ϵͳ�����ȫ�ֱ���,	*/
     		/* ��Ϊ���������������ݳ���ֱ�ӱ����ڸñ����� 							*/
     		/* ע��ý��պ��������ݻ������ĵ�һ���������洢���ճ��ȵ�,�����û��ڶ���*/
     		/* ����������ʱΪʵ�����ݳ��ȼ�1										*/
     		
     		//�ⲿ�������ʱ���������õ�,�û����Բ���ʹ��
     	    USB_ReceivCtrlData((Uint16 *)&usbbuffer, CtrlDataLen, USB_IOFLAG_SWAP);
     	    
     	}
     	
     	if(usbflag == USB_BULK3_READ)		 						
    	{
    		usbflag = 0;
    		// �����PC���������ڵ�3��������,���û�����Լ��Ĵ������
   		}
   		
   		if(usbflag == USB_BULK4_READ)								
    	{
    		usbflag = 0;
    		// �����PC���������ڵ�4��������,���û�����Լ��Ĵ������
   		}
   		
   		if(usbflag == USB_BULK5_READ)								
    	{
    		usbflag = 0;
    		// �����PC���������ڵ�5��������,���û�����Լ��Ĵ������
   		}
   		
   		if(usbflag == USB_BULK6_READ)								
    	{
    		usbflag = 0;
    		// �����PC���������ڵ�6��������,���û�����Լ��Ĵ������
   		}
   		
   		if(usbflag == USB_BULK7_READ)								
    	{
    		usbflag = 0;
    		// �����PC���������ڵ�7��������,���û�����Լ��Ĵ������
   		}
   		
   		if(usbflag == USB_CTRL0_WRITE)
   		{
			usbflag = 0; 
			
			/* �û�ʹ������ķ��ͺ���ʱ,����ʹ��CtrlDataLen���ϵͳ�����ȫ�ֱ���,	*/
     		/* ��Ϊ���������������ݳ���ֱ�ӱ����ڸñ�����,							*/
     		/* �������������յĳ�������ƽڵ�In0���͵ĳ��ȱ���һ��					*/
     		/*	
     		�ⲿ�������ʱ���������õ�,�û����Բ���ʹ��
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
    		// �����PC���������ڵ�3��ȡ����,���û�����Լ��Ĵ������
   		}
   		
   		if(usbflag == USB_BULK4_WRITE)
   		{
   			usbflag = 0;
    		// �����PC���������ڵ�4��ȡ����,���û�����Լ��Ĵ������
   		}
   		
   		if(usbflag == USB_BULK5_WRITE)
   		{
   			usbflag = 0;
    		// �����PC���������ڵ�5��ȡ����,���û�����Լ��Ĵ������
   		}
   		
   		if(usbflag == USB_BULK6_WRITE)
   		{
   			usbflag = 0;
    		// �����PC���������ڵ�6��ȡ����,���û�����Լ��Ĵ������
   		}
   		
   		if(usbflag == USB_BULK7_WRITE)
   		{
   			usbflag = 0;
    		// �����PC���������ڵ�7��ȡ����,���û�����Լ��Ĵ������
   		}
     }	
}


/* USB�жϺ��� */
interrupt void USB_ISR()
{
	/* ����USB�¼��ķ����� */
	USB_evDispatch();
}

/********************************************************************/
/*	No	More														*/
/********************************************************************/


