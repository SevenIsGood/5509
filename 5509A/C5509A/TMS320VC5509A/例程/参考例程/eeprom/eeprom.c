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


/*锁相环的设置*/
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
    /*初始化CSL库*/
    CSL_init();
    /*EMIF为全EMIF接口*/
    CHIP_RSET(XBSR,0x0a01);
	/*禁止所有中断*/
	IRQ_globalDisable(); 
    /*确定方向为输出*/
    GPIO_RSET(IODIR,0xFF);
	/*设置系统的运行速度为144MHz*/
    PLL_config(&myConfig);
  	
  	/*写入数据*/
  	for(i = 0; i<0x100;)
  	{	
  		/*测试时，将数据写完后，断电后，重新上电打开CCS
  			将此行注释掉，直接读取数据，观察是否正确*/
  		SPI_WrieSignal(EPROM_address,i);
  		/*读回写入的数据*/
		EEPROM_data = SPI_ReadSignal(EPROM_address);
		/*保存读取的数据*/
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
/*两次操作之间的延时*/
void SPIEND_Delay()
{
	Uint32 j =0;
	for(j= 0;j<0x20;j++)
	{}
}
/*读写使能与禁止初始化，数据长度为8位*/
void SPI_writecominit(MCBSP_Handle hMcbsp)
{
   /****************************************************************/
   /* Place port in reset - setting XRST & RRST to 0               */
   /****************************************************************/
   /*进行发送复位*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,0);
   /*进行接收复位*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,0); 
   /*进行分频器复位*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,0);
   /****************************************************************/
   /* Set values of all control registers                          */
   /****************************************************************/
   /*设置SPCR1寄存器,SPI时钟停止模式*/
   MCBSP_FSETH(hMcbsp, SPCR1,CLKSTP,0x3);
   /*设置XCR1寄存器，单数据相，发送数据长度为8位*/
   MCBSP_FSETH(hMcbsp, XCR1, XFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, RCR1, RFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, XCR1, XWDLEN1,0x0);
   MCBSP_FSETH(hMcbsp, RCR1, RWDLEN1,0x0);
   /*设置XCR2寄存器，发送数据延时一个位*/
   MCBSP_FSETH(hMcbsp, XCR2, XDATDLY,0x1);
   MCBSP_FSETH(hMcbsp, RCR2, RDATDLY,0x1);
   /*设置PCR寄存器，设置BLCK为输出，即MCBSP为Master；
   		            设置BFSX为输出，并且其极性为低有效
   		            设置CLKXP为高*/
   MCBSP_FSETH(hMcbsp, PCR,CLKXP, 0x0);
   MCBSP_FSETH(hMcbsp, PCR,CLKRP, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,CLKXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXP, 0x1);
   /*设置SRGR2，确定时钟来源为内部的CPU�
              ，确定帧同步为低有效。*/
   MCBSP_FSETH(hMcbsp, SRGR2,CLKSM,1);
   /*使能波特率寄存器*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,1);
   /*等待两个CLKG时钟周期*/
   asm(" nop ");
   asm(" nop ");
   /*设置SRGR1，确定分频数为0x20，MCBSP2的波待率为100KHz*/
// MCBSP_FSETH(hMcbsp, SRGR1, FWID,0x20);
   MCBSP_FSETH(hMcbsp, SRGR1, CLKGDV,0xF0);
   /*设置FRST位*/
   MCBSP_FSETH(hMcbsp, SPCR2,FRST,1); 
   /*使能MCBSP0发送*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,1);
   /*使能MCBSP0接收*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,1); 
   /*等待两个CLKG时钟周期*/
   asm(" nop ");
   asm(" nop ");
}
/*写命令延时*/
void WRITECOM_Delay()
{
	Uint32 j =0;
	for(j= 0;j<0x100;j++)
	{}
}
/*写EEPROM使能*/
void SPI_WriteEN()
{
	/*初始化McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	SPI_writecominit(hMcbsp);
	/*发送缓冲区是否为空*/
	while(!MCBSP_xrdy(hMcbsp)){};
	/*使能片选，使GPIO4为低*/
	GPIO_RSET(IODATA,0x00);
	/*写入数据*/	
	MCBSP_write16(hMcbsp,SPI_WRITE_EN);	
	/*禁止片选，使GPIO4为高*/
	WRITECOM_Delay();
	GPIO_RSET(IODATA,0x10);
	SPIEND_Delay();
	MCBSP_close(hMcbsp);			
}
/*写EEPROM禁止*/
void SPI_WriteDI()
{
	/*初始化McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	SPI_writecominit(hMcbsp);
	/*发送缓冲区是否为空*/
	while(!MCBSP_xrdy(hMcbsp)){};
	/*使能片选，使GPIO4为低*/
	GPIO_RSET(IODATA,0x00);
	/*写入数据*/	
	MCBSP_write16(hMcbsp,SPI_WRITE_DI);	
	/*禁止片选，使GPIO4为高*/
	WRITECOM_Delay();
	GPIO_RSET(IODATA,0x10);
	SPIEND_Delay();
	MCBSP_close(hMcbsp);			
}
/*************************************************************************************/
/*		write and read the EEPROM's status register									 */
/*************************************************************************************/
/*读写EPPROM状态字初始化，数据长度为16位*/
void SPI_status_init(MCBSP_Handle hMcbsp)
{
   /****************************************************************/
   /* Place port in reset - setting XRST & RRST to 0               */
   /****************************************************************/
   /*进行发送复位*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,0);
   /*进行接收复位*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,0); 
   /*进行分频器复位*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,0);
   /****************************************************************/
   /* Set values of all control registers                          */
   /****************************************************************/
   /*设置SPCR1寄存器,SPI时钟停止模式*/
   MCBSP_FSETH(hMcbsp, SPCR1,CLKSTP,0x3);
   /*设置XCR1寄存器，单数据相，发送数据长度为16位*/
   MCBSP_FSETH(hMcbsp, XCR1, XFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, RCR1, RFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, XCR1, XWDLEN1,0x2);
   MCBSP_FSETH(hMcbsp, RCR1, RWDLEN1,0x2);
   /*设置XCR2寄存器，发送数据延时一个位*/
   MCBSP_FSETH(hMcbsp, XCR2, XDATDLY,0x1);
   MCBSP_FSETH(hMcbsp, RCR2, RDATDLY,0x1);
   /*设置PCR寄存器，设置BLCK为输出，即MCBSP为Master；
   		            设置BFSX为输出，并且其极性为低有效
   		            设置CLKXP为高*/
   MCBSP_FSETH(hMcbsp, PCR,CLKXP, 0x0);
   MCBSP_FSETH(hMcbsp, PCR,CLKRP, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,CLKXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXP, 0x1);
   /*设置SRGR2，确定时钟来源为内部的CPU�
              ，确定帧同步为低有效。*/
   MCBSP_FSETH(hMcbsp, SRGR2,CLKSM,1);
   /*使能波特率寄存器*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,1);
   /*等待两个CLKG时钟周期*/
   asm(" nop ");
   asm(" nop ");
   /*设置SRGR1，确定分频数为0x20，MCBSP2的波待率为100KHz*/
//   MCBSP_FSETH(hMcbsp, SRGR1, FWID,0x20);
//   MCBSP_FSETH(hMcbsp, SRGR2,FPER,0x20);
   MCBSP_FSETH(hMcbsp, SRGR1, CLKGDV,0xF0);
   /*设置FRST位*/
   MCBSP_FSETH(hMcbsp, SPCR2,FRST,1); 
   /*使能MCBSP0发送*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,1);
   /*使能MCBSP0接收*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,1); 
   /*等待两个CLKG时钟周期*/
   asm(" nop ");
   asm(" nop ");
}
/*读写EEPROM状态寄存的延时*/
void WrReg_Delay()
{
	Uint32 j =0;
	for(j= 0;j<0x150;j++)
	{}
}
/*写EEPROM状态寄存器*/
void SPI_WrieStatusReg(Uint16 data)
{
	/*初始化McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	SPI_status_init(hMcbsp);
	/*发送缓冲区是否为空*/
	while(!MCBSP_xrdy(hMcbsp)){};
	/*使能片选，使GPIO4为低*/
	GPIO_RSET(IODATA,0x00);
	/*写入数据*/	
	MCBSP_write16(hMcbsp,(data+SPI_WRITE_REG));	
	/*禁止片选，使GPIO4为高*/
	WrReg_Delay();
	GPIO_RSET(IODATA,0x10);
	SPIEND_Delay();
	MCBSP_close(hMcbsp);		
}
/*读EEPROM状态寄存器*/
Uint16 SPI_ReadStatusReg()
{
	Uint16 vaule = 0;
	/*初始化McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	SPI_status_init(hMcbsp);
	/*使能片选，使GPIO4为低*/
	GPIO_RSET(IODATA,0x00);
	/*写入读命令*/	
	MCBSP_write16(hMcbsp,SPI_READ_REG);	
	/*禁止片选，使GPIO4为高*/
	WrReg_Delay();
	GPIO_RSET(IODATA,0x10);
	SPIEND_Delay();
	/*读寄存器*/
	vaule = MCBSP_read16(hMcbsp);
	MCBSP_close(hMcbsp);
	return vaule;			
}
/********************************************************************************/
/*      EEROM write and read function											*/
/********************************************************************************/ 	
/*读写数据初始化,数据长度为32位*/
void SPI_wrdatainit(MCBSP_Handle hMcbsp)
{
   /*初始化McBSP0*/
   /****************************************************************/
   /* Place port in reset - setting XRST & RRST to 0               */
   /****************************************************************/
   /*进行发送复位*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,0);
   /*进行接收复位*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,0); 
   /*进行分频器复位*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,0);
   /****************************************************************/
   /* Set values of all control registers                          */
   /****************************************************************/
   /*设置SPCR1寄存器,SPI时钟停止模式*/
   MCBSP_FSETH(hMcbsp, SPCR1,CLKSTP,0x3);
   /*设置XCR1寄存器，单数据相，发送数据长度为32位*/
   MCBSP_FSETH(hMcbsp, XCR1, XFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, RCR1, RFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, XCR1, XWDLEN1,0x5);
   MCBSP_FSETH(hMcbsp, RCR1, RWDLEN1,0x5);
   /*设置XCR2寄存器，发送数据延时一个位*/
   MCBSP_FSETH(hMcbsp, XCR2, XDATDLY,0x1);
   MCBSP_FSETH(hMcbsp, RCR2, RDATDLY,0x1);
   /*设置PCR寄存器，设置BLCK为输出，即MCBSP为Master；
   		            设置BFSX为输出，并且其极性为低有效
   		            设置CLKXP为高*/
   MCBSP_FSETH(hMcbsp, PCR,CLKXP, 0x0);
   MCBSP_FSETH(hMcbsp, PCR,CLKRP, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,CLKXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXP, 0x1);
   /*设置SRGR2，确定时钟来源为内部的CPU�
              ，确定帧同步为低有效。*/
   MCBSP_FSETH(hMcbsp, SRGR2,CLKSM,1);
   /*使能波特率寄存器*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,1);
   /*等待两个CLKG时钟周期*/
   asm(" nop ");
   asm(" nop ");
   /*设置SRGR1，确定分频数为0x20，MCBSP2的波待率为100KHz*/
//   MCBSP_FSETH(hMcbsp, SRGR1, FWID,0x20);
//   MCBSP_FSETH(hMcbsp, SRGR2,FPER,0x20);
   MCBSP_FSETH(hMcbsp, SRGR1, CLKGDV,0xF0);
   /*设置FRST位*/
   MCBSP_FSETH(hMcbsp, SPCR2,FRST,1); 
   /*使能MCBSP0发送*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,1);
   /*使能MCBSP0接收*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,1); 
   /*等待两个CLKG时钟周期*/
   asm(" nop ");
   asm(" nop ");
}
/*读写数据的延时*/
void SPIWR_Delay()
{
	Uint32 j =0;
	//for(j= 0;j<0x220;j++)
	for(j= 0;j<0x250;j++)
	{}
}
/*向EEROM中写入一个数据*/
void SPI_WrieSignal(Uint32 address,Uint32 data)
{
	Uint32 k =0;
	Uint16 EPROM_status = 0;
	/*使能EEPROM的写功能*/	
  	SPI_WriteEN();
  	/*确定写保护是否打开*/
  	EPROM_status = SPI_ReadStatusReg();
  	while(!(EPROM_status & 0x2)){};
	/*初始化McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	/*进行EEPROM读写初始化*/
  	SPI_wrdatainit(hMcbsp);
	address = address & 0xFFFF;
	data = data & 0xFF;
	/*发送缓冲区是否为空*/
	while(!MCBSP_xrdy(hMcbsp)){};
	/*使能片选，使GPIO4为低*/
	GPIO_RSET(IODATA,0x00);
	/*写入数据*/	
	MCBSP_write32(hMcbsp,(SPI_WRITE + (address <<8) + data));	
	/*禁止片选，使GPIO4为高*/
	SPIWR_Delay();
	GPIO_RSET(IODATA,0x10);
	/*等待EEPROM写入完成，最多延时10个ms*/
	for(k = 0; k <0x10000;k++)
	{}
	MCBSP_close(hMcbsp);		
}
/*从EEPROM中读出一个数据,读回数据只有最后8位有效*/
Uint32 SPI_ReadSignal(Uint32 address)
{
	Uint32 vaule = 0;
	/*初始化McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	/*进行EEPROM读写初始化*/
  	SPI_wrdatainit(hMcbsp);
	address = (address & 0xffff) <<8;
	vaule = SPI_READ + address;	
	/*发送缓冲区是否为空*/
	while(!MCBSP_xrdy(hMcbsp)){};
	/*使能片选，使GPIO4为低*/
	GPIO_RSET(IODATA,0x00);
	/*写入数据*/	
	MCBSP_write32(hMcbsp,vaule);	
	/*禁止片选，使GPIO4为高*/
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
