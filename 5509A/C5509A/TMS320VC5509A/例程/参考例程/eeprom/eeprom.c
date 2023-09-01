/******************************************************************************/
/*  Copyright 2004 by SEED Electronic Technology LTD.                         */
/*  All rights reserved. SEED Electronic Technology LTD.                      */
/*  Restricted rights to use, duplicate or disclose this code are             */
/*  granted through contract.                                                 */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
/*----------------------------------------------------------------------------*/
/* DESCRIPTION:  															  */
/*   																		  */
/* This is an example for EERPOM of C5509										  */
/*----------------------------------------------------------------------------*/
/* MODULE NAME... SPI EEPROM write and read 										  */
/* FILENAME...... boot.c    												  */
/* DATE CREATED.. Mon 03/5/2004	    										  */
/* COMPONENT..... 															  */
/* PREREQUISITS.. 															  */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION:																  */  
/*   																		  */
/* this example is that write the SPI EEPROM  		  */
/*----------------------------------------------------------------------------*/

#include <csl.h>
#include <stdio.h>
#include <csl_pll.h>
#include <csl_mcbsp.h>
#include <csl_gpio.h>
#include <csl_irq.h>
#include <csl_chip.h>

#define SPI_WRITE 	  0x02000000
#define SPI_READ  	  0x03000000
#define SPI_WRITE_EN  0x06
#define SPI_WRITE_DI  0x04
#define SPI_WRITE_REG 0x0100
#define SPI_READ_REG  0x0500


/*���໷������*/
PLL_Config  myConfig      = {
  0,    //IAI: the PLL locks using the same process that was underway 
                //before the idle mode was entered
  1,    //IOB: If the PLL indicates a break in the phase lock, 
                //it switches to its bypass mode and restarts the PLL phase-locking 
                //sequence
  2,    //PLL multiply value; multiply 12 times
  0             //Divide by 2 PLL divide value; it can be either PLL divide value 
                //(when PLL is enabled), or Bypass-mode divide value
                //(PLL in bypass mode, if PLL multiply value is set to 1)
};


MCBSP_Handle hMcbsp;
Uint32 data_read[0x100] = {0};

Uint32 SPI_ReadSignal(Uint32 address);
void SPI_WrieSignal(Uint32 address,Uint32 data);

void main(void)
{
    Uint32 EEPROM_data = 0; 
    Uint32 i = 0;
    Uint32 j;
    Uint32 EPROM_address = 0;
    /* Initialize CSL library - This is REQUIRED !!! */
    /*��ʼ��CSL��*/
    CSL_init();
    /*EMIFΪȫEMIF�ӿ�*/
    CHIP_RSET(XBSR,0x0a01);
	/*��ֹ�����ж�*/
	IRQ_globalDisable(); 
    /*ȷ������Ϊ���*/
    GPIO_RSET(IODIR,0xFF);
	/*����ϵͳ�������ٶ�Ϊ144MHz*/
    PLL_config(&myConfig);
  	
  	/*д������*/
  	for(i = 0; i<0x100;)
  	{	
  		/*����ʱ��������д��󣬶ϵ�������ϵ��CCS
  			������ע�͵���ֱ�Ӷ�ȡ���ݣ��۲��Ƿ���ȷ*/
  		SPI_WrieSignal(EPROM_address,i);
  		/*����д�������*/
		EEPROM_data = SPI_ReadSignal(EPROM_address);
		/*�����ȡ������*/
		data_read[i] = EEPROM_data;
  		if(i != (data_read[i] & 0xFF))
  		{
 			for(;;){}
  		}
  		EPROM_address++;
  		i++;
  	} 
  	for(;;)
  	{
  	 asm(" bset xf");
  	 for(i=1000;i>0;i--)
  	  for(j=100;j>0;j--);
  	  
  	    asm(" bclr xf");
  	 for(i=1000;i>0;i--)
  	  for(j=100;j>0;j--);
  	}    	   	    	   
}

/*********************************************************************************/
/*		EEPROM write enable and write disable  									 */
/*********************************************************************************/
/*���β���֮�����ʱ*/
void SPIEND_Delay()
{
	Uint32 j =0;
	for(j= 0;j<0x20;j++)
	{}
}
/*��дʹ�����ֹ��ʼ�������ݳ���Ϊ8λ*/
void SPI_writecominit(MCBSP_Handle hMcbsp)
{
   /****************************************************************/
   /* Place port in reset - setting XRST & RRST to 0               */
   /****************************************************************/
   /*���з��͸�λ*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,0);
   /*���н��ո�λ*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,0); 
   /*���з�Ƶ����λ*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,0);
   /****************************************************************/
   /* Set values of all control registers                          */
   /****************************************************************/
   /*����SPCR1�Ĵ���,SPIʱ��ֹͣģʽ*/
   MCBSP_FSETH(hMcbsp, SPCR1,CLKSTP,0x3);
   /*����XCR1�Ĵ������������࣬�������ݳ���Ϊ8λ*/
   MCBSP_FSETH(hMcbsp, XCR1, XFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, RCR1, RFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, XCR1, XWDLEN1,0x0);
   MCBSP_FSETH(hMcbsp, RCR1, RWDLEN1,0x0);
   /*����XCR2�Ĵ���������������ʱһ��λ*/
   MCBSP_FSETH(hMcbsp, XCR2, XDATDLY,0x1);
   MCBSP_FSETH(hMcbsp, RCR2, RDATDLY,0x1);
   /*����PCR�Ĵ���������BLCKΪ�������MCBSPΪMaster��
   		            ����BFSXΪ����������伫��Ϊ����Ч
   		            ����CLKXPΪ��*/
   MCBSP_FSETH(hMcbsp, PCR,CLKXP, 0x0);
   MCBSP_FSETH(hMcbsp, PCR,CLKRP, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,CLKXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXP, 0x1);
   /*����SRGR2��ȷ��ʱ����ԴΪ�ڲ���CPU�
              ��ȷ��֡ͬ��Ϊ����Ч��*/
   MCBSP_FSETH(hMcbsp, SRGR2,CLKSM,1);
   /*ʹ�ܲ����ʼĴ���*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,1);
   /*�ȴ�����CLKGʱ������*/
   asm(" nop ");
   asm(" nop ");
   /*����SRGR1��ȷ����Ƶ��Ϊ0x20��MCBSP2�Ĳ�����Ϊ100KHz*/
// MCBSP_FSETH(hMcbsp, SRGR1, FWID,0x20);
   MCBSP_FSETH(hMcbsp, SRGR1, CLKGDV,0xF0);
   /*����FRSTλ*/
   MCBSP_FSETH(hMcbsp, SPCR2,FRST,1); 
   /*ʹ��MCBSP0����*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,1);
   /*ʹ��MCBSP0����*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,1); 
   /*�ȴ�����CLKGʱ������*/
   asm(" nop ");
   asm(" nop ");
}
/*д������ʱ*/
void WRITECOM_Delay()
{
	Uint32 j =0;
	for(j= 0;j<0x100;j++)
	{}
}
/*дEEPROMʹ��*/
void SPI_WriteEN()
{
	/*��ʼ��McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	SPI_writecominit(hMcbsp);
	/*���ͻ������Ƿ�Ϊ��*/
	while(!MCBSP_xrdy(hMcbsp)){};
	/*ʹ��Ƭѡ��ʹGPIO4Ϊ��*/
	GPIO_RSET(IODATA,0x00);
	/*д������*/	
	MCBSP_write16(hMcbsp,SPI_WRITE_EN);	
	/*��ֹƬѡ��ʹGPIO4Ϊ��*/
	WRITECOM_Delay();
	GPIO_RSET(IODATA,0x10);
	SPIEND_Delay();
	MCBSP_close(hMcbsp);			
}
/*дEEPROM��ֹ*/
void SPI_WriteDI()
{
	/*��ʼ��McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	SPI_writecominit(hMcbsp);
	/*���ͻ������Ƿ�Ϊ��*/
	while(!MCBSP_xrdy(hMcbsp)){};
	/*ʹ��Ƭѡ��ʹGPIO4Ϊ��*/
	GPIO_RSET(IODATA,0x00);
	/*д������*/	
	MCBSP_write16(hMcbsp,SPI_WRITE_DI);	
	/*��ֹƬѡ��ʹGPIO4Ϊ��*/
	WRITECOM_Delay();
	GPIO_RSET(IODATA,0x10);
	SPIEND_Delay();
	MCBSP_close(hMcbsp);			
}
/*************************************************************************************/
/*		write and read the EEPROM's status register									 */
/*************************************************************************************/
/*��дEPPROM״̬�ֳ�ʼ�������ݳ���Ϊ16λ*/
void SPI_status_init(MCBSP_Handle hMcbsp)
{
   /****************************************************************/
   /* Place port in reset - setting XRST & RRST to 0               */
   /****************************************************************/
   /*���з��͸�λ*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,0);
   /*���н��ո�λ*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,0); 
   /*���з�Ƶ����λ*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,0);
   /****************************************************************/
   /* Set values of all control registers                          */
   /****************************************************************/
   /*����SPCR1�Ĵ���,SPIʱ��ֹͣģʽ*/
   MCBSP_FSETH(hMcbsp, SPCR1,CLKSTP,0x3);
   /*����XCR1�Ĵ������������࣬�������ݳ���Ϊ16λ*/
   MCBSP_FSETH(hMcbsp, XCR1, XFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, RCR1, RFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, XCR1, XWDLEN1,0x2);
   MCBSP_FSETH(hMcbsp, RCR1, RWDLEN1,0x2);
   /*����XCR2�Ĵ���������������ʱһ��λ*/
   MCBSP_FSETH(hMcbsp, XCR2, XDATDLY,0x1);
   MCBSP_FSETH(hMcbsp, RCR2, RDATDLY,0x1);
   /*����PCR�Ĵ���������BLCKΪ�������MCBSPΪMaster��
   		            ����BFSXΪ����������伫��Ϊ����Ч
   		            ����CLKXPΪ��*/
   MCBSP_FSETH(hMcbsp, PCR,CLKXP, 0x0);
   MCBSP_FSETH(hMcbsp, PCR,CLKRP, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,CLKXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXP, 0x1);
   /*����SRGR2��ȷ��ʱ����ԴΪ�ڲ���CPU�
              ��ȷ��֡ͬ��Ϊ����Ч��*/
   MCBSP_FSETH(hMcbsp, SRGR2,CLKSM,1);
   /*ʹ�ܲ����ʼĴ���*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,1);
   /*�ȴ�����CLKGʱ������*/
   asm(" nop ");
   asm(" nop ");
   /*����SRGR1��ȷ����Ƶ��Ϊ0x20��MCBSP2�Ĳ�����Ϊ100KHz*/
//   MCBSP_FSETH(hMcbsp, SRGR1, FWID,0x20);
//   MCBSP_FSETH(hMcbsp, SRGR2,FPER,0x20);
   MCBSP_FSETH(hMcbsp, SRGR1, CLKGDV,0xF0);
   /*����FRSTλ*/
   MCBSP_FSETH(hMcbsp, SPCR2,FRST,1); 
   /*ʹ��MCBSP0����*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,1);
   /*ʹ��MCBSP0����*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,1); 
   /*�ȴ�����CLKGʱ������*/
   asm(" nop ");
   asm(" nop ");
}
/*��дEEPROM״̬�Ĵ����ʱ*/
void WrReg_Delay()
{
	Uint32 j =0;
	for(j= 0;j<0x150;j++)
	{}
}
/*дEEPROM״̬�Ĵ���*/
void SPI_WrieStatusReg(Uint16 data)
{
	/*��ʼ��McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	SPI_status_init(hMcbsp);
	/*���ͻ������Ƿ�Ϊ��*/
	while(!MCBSP_xrdy(hMcbsp)){};
	/*ʹ��Ƭѡ��ʹGPIO4Ϊ��*/
	GPIO_RSET(IODATA,0x00);
	/*д������*/	
	MCBSP_write16(hMcbsp,(data+SPI_WRITE_REG));	
	/*��ֹƬѡ��ʹGPIO4Ϊ��*/
	WrReg_Delay();
	GPIO_RSET(IODATA,0x10);
	SPIEND_Delay();
	MCBSP_close(hMcbsp);		
}
/*��EEPROM״̬�Ĵ���*/
Uint16 SPI_ReadStatusReg()
{
	Uint16 vaule = 0;
	/*��ʼ��McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	SPI_status_init(hMcbsp);
	/*ʹ��Ƭѡ��ʹGPIO4Ϊ��*/
	GPIO_RSET(IODATA,0x00);
	/*д�������*/	
	MCBSP_write16(hMcbsp,SPI_READ_REG);	
	/*��ֹƬѡ��ʹGPIO4Ϊ��*/
	WrReg_Delay();
	GPIO_RSET(IODATA,0x10);
	SPIEND_Delay();
	/*���Ĵ���*/
	vaule = MCBSP_read16(hMcbsp);
	MCBSP_close(hMcbsp);
	return vaule;			
}
/********************************************************************************/
/*      EEROM write and read function											*/
/********************************************************************************/ 	
/*��д���ݳ�ʼ��,���ݳ���Ϊ32λ*/
void SPI_wrdatainit(MCBSP_Handle hMcbsp)
{
   /*��ʼ��McBSP0*/
   /****************************************************************/
   /* Place port in reset - setting XRST & RRST to 0               */
   /****************************************************************/
   /*���з��͸�λ*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,0);
   /*���н��ո�λ*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,0); 
   /*���з�Ƶ����λ*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,0);
   /****************************************************************/
   /* Set values of all control registers                          */
   /****************************************************************/
   /*����SPCR1�Ĵ���,SPIʱ��ֹͣģʽ*/
   MCBSP_FSETH(hMcbsp, SPCR1,CLKSTP,0x3);
   /*����XCR1�Ĵ������������࣬�������ݳ���Ϊ32λ*/
   MCBSP_FSETH(hMcbsp, XCR1, XFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, RCR1, RFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, XCR1, XWDLEN1,0x5);
   MCBSP_FSETH(hMcbsp, RCR1, RWDLEN1,0x5);
   /*����XCR2�Ĵ���������������ʱһ��λ*/
   MCBSP_FSETH(hMcbsp, XCR2, XDATDLY,0x1);
   MCBSP_FSETH(hMcbsp, RCR2, RDATDLY,0x1);
   /*����PCR�Ĵ���������BLCKΪ�������MCBSPΪMaster��
   		            ����BFSXΪ����������伫��Ϊ����Ч
   		            ����CLKXPΪ��*/
   MCBSP_FSETH(hMcbsp, PCR,CLKXP, 0x0);
   MCBSP_FSETH(hMcbsp, PCR,CLKRP, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,CLKXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXP, 0x1);
   /*����SRGR2��ȷ��ʱ����ԴΪ�ڲ���CPU�
              ��ȷ��֡ͬ��Ϊ����Ч��*/
   MCBSP_FSETH(hMcbsp, SRGR2,CLKSM,1);
   /*ʹ�ܲ����ʼĴ���*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,1);
   /*�ȴ�����CLKGʱ������*/
   asm(" nop ");
   asm(" nop ");
   /*����SRGR1��ȷ����Ƶ��Ϊ0x20��MCBSP2�Ĳ�����Ϊ100KHz*/
//   MCBSP_FSETH(hMcbsp, SRGR1, FWID,0x20);
//   MCBSP_FSETH(hMcbsp, SRGR2,FPER,0x20);
   MCBSP_FSETH(hMcbsp, SRGR1, CLKGDV,0xF0);
   /*����FRSTλ*/
   MCBSP_FSETH(hMcbsp, SPCR2,FRST,1); 
   /*ʹ��MCBSP0����*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,1);
   /*ʹ��MCBSP0����*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,1); 
   /*�ȴ�����CLKGʱ������*/
   asm(" nop ");
   asm(" nop ");
}
/*��д���ݵ���ʱ*/
void SPIWR_Delay()
{
	Uint32 j =0;
	//for(j= 0;j<0x220;j++)
	for(j= 0;j<0x250;j++)
	{}
}
/*��EEROM��д��һ������*/
void SPI_WrieSignal(Uint32 address,Uint32 data)
{
	Uint32 k =0;
	Uint16 EPROM_status = 0;
	/*ʹ��EEPROM��д����*/	
  	SPI_WriteEN();
  	/*ȷ��д�����Ƿ��*/
  	EPROM_status = SPI_ReadStatusReg();
  	while(!(EPROM_status & 0x2)){};
	/*��ʼ��McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	/*����EEPROM��д��ʼ��*/
  	SPI_wrdatainit(hMcbsp);
	address = address & 0xFFFF;
	data = data & 0xFF;
	/*���ͻ������Ƿ�Ϊ��*/
	while(!MCBSP_xrdy(hMcbsp)){};
	/*ʹ��Ƭѡ��ʹGPIO4Ϊ��*/
	GPIO_RSET(IODATA,0x00);
	/*д������*/	
	MCBSP_write32(hMcbsp,(SPI_WRITE + (address <<8) + data));	
	/*��ֹƬѡ��ʹGPIO4Ϊ��*/
	SPIWR_Delay();
	GPIO_RSET(IODATA,0x10);
	/*�ȴ�EEPROMд����ɣ������ʱ10��ms*/
	for(k = 0; k <0x10000;k++)
	{}
	MCBSP_close(hMcbsp);		
}
/*��EEPROM�ж���һ������,��������ֻ�����8λ��Ч*/
Uint32 SPI_ReadSignal(Uint32 address)
{
	Uint32 vaule = 0;
	/*��ʼ��McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	/*����EEPROM��д��ʼ��*/
  	SPI_wrdatainit(hMcbsp);
	address = (address & 0xffff) <<8;
	vaule = SPI_READ + address;	
	/*���ͻ������Ƿ�Ϊ��*/
	while(!MCBSP_xrdy(hMcbsp)){};
	/*ʹ��Ƭѡ��ʹGPIO4Ϊ��*/
	GPIO_RSET(IODATA,0x00);
	/*д������*/	
	MCBSP_write32(hMcbsp,vaule);	
	/*��ֹƬѡ��ʹGPIO4Ϊ��*/
	SPIWR_Delay();
	GPIO_RSET(IODATA,0x10);
	SPIEND_Delay();
	vaule = 0;
	vaule = MCBSP_read32(hMcbsp);
	MCBSP_close(hMcbsp);
	return vaule;			
}

/******************************************************************************\
* End of eeprom.c
\******************************************************************************/
