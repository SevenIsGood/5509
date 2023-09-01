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


/*ËøÏà»·µÄÉèÖÃ*/
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
    /*³õÊ¼»¯CSL¿â*/
    CSL_init();
    /*EMIFÎªÈ«EMIF½Ó¿Ú*/
    CHIP_RSET(XBSR,0x0a01);
	/*½ûÖ¹ËùÓÐÖÐ¶Ï*/
	IRQ_globalDisable(); 
    /*È·¶¨·½ÏòÎªÊä³ö*/
    GPIO_RSET(IODIR,0xFF);
	/*ÉèÖÃÏµÍ³µÄÔËÐÐËÙ¶ÈÎª144MHz*/
    PLL_config(&myConfig);
  	
  	/*Ð´ÈëÊý¾Ý*/
  	for(i = 0; i<0x100;)
  	{	
  		/*²âÊÔÊ±£¬½«Êý¾ÝÐ´Íêºó£¬¶Ïµçºó£¬ÖØÐÂÉÏµç´ò¿ªCCS
  			½«´ËÐÐ×¢ÊÍµô£¬Ö±½Ó¶ÁÈ¡Êý¾Ý£¬¹Û²ìÊÇ·ñÕýÈ·*/
  		SPI_WrieSignal(EPROM_address,i);
  		/*¶Á»ØÐ´ÈëµÄÊý¾Ý*/
		EEPROM_data = SPI_ReadSignal(EPROM_address);
		/*±£´æ¶ÁÈ¡µÄÊý¾Ý*/
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
/*Á½´Î²Ù×÷Ö®¼äµÄÑÓÊ±*/
void SPIEND_Delay()
{
	Uint32 j =0;
	for(j= 0;j<0x20;j++)
	{}
}
/*¶ÁÐ´Ê¹ÄÜÓë½ûÖ¹³õÊ¼»¯£¬Êý¾Ý³¤¶ÈÎª8Î»*/
void SPI_writecominit(MCBSP_Handle hMcbsp)
{
   /****************************************************************/
   /* Place port in reset - setting XRST & RRST to 0               */
   /****************************************************************/
   /*½øÐÐ·¢ËÍ¸´Î»*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,0);
   /*½øÐÐ½ÓÊÕ¸´Î»*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,0); 
   /*½øÐÐ·ÖÆµÆ÷¸´Î»*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,0);
   /****************************************************************/
   /* Set values of all control registers                          */
   /****************************************************************/
   /*ÉèÖÃSPCR1¼Ä´æÆ÷,SPIÊ±ÖÓÍ£Ö¹Ä£Ê½*/
   MCBSP_FSETH(hMcbsp, SPCR1,CLKSTP,0x3);
   /*ÉèÖÃXCR1¼Ä´æÆ÷£¬µ¥Êý¾ÝÏà£¬·¢ËÍÊý¾Ý³¤¶ÈÎª8Î»*/
   MCBSP_FSETH(hMcbsp, XCR1, XFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, RCR1, RFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, XCR1, XWDLEN1,0x0);
   MCBSP_FSETH(hMcbsp, RCR1, RWDLEN1,0x0);
   /*ÉèÖÃXCR2¼Ä´æÆ÷£¬·¢ËÍÊý¾ÝÑÓÊ±Ò»¸öÎ»*/
   MCBSP_FSETH(hMcbsp, XCR2, XDATDLY,0x1);
   MCBSP_FSETH(hMcbsp, RCR2, RDATDLY,0x1);
   /*ÉèÖÃPCR¼Ä´æÆ÷£¬ÉèÖÃBLCKÎªÊä³ö£¬¼´MCBSPÎªMaster£»
   		            ÉèÖÃBFSXÎªÊä³ö£¬²¢ÇÒÆä¼«ÐÔÎªµÍÓÐÐ§
   		            ÉèÖÃCLKXPÎª¸ß*/
   MCBSP_FSETH(hMcbsp, PCR,CLKXP, 0x0);
   MCBSP_FSETH(hMcbsp, PCR,CLKRP, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,CLKXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXP, 0x1);
   /*ÉèÖÃSRGR2£¬È·¶¨Ê±ÖÓÀ´Ô´ÎªÄÚ²¿µÄCPU£
              £¬È·¶¨Ö¡Í¬²½ÎªµÍÓÐÐ§¡£*/
   MCBSP_FSETH(hMcbsp, SRGR2,CLKSM,1);
   /*Ê¹ÄÜ²¨ÌØÂÊ¼Ä´æÆ÷*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,1);
   /*µÈ´ýÁ½¸öCLKGÊ±ÖÓÖÜÆÚ*/
   asm(" nop ");
   asm(" nop ");
   /*ÉèÖÃSRGR1£¬È·¶¨·ÖÆµÊýÎª0x20£¬MCBSP2µÄ²¨´ýÂÊÎª100KHz*/
// MCBSP_FSETH(hMcbsp, SRGR1, FWID,0x20);
   MCBSP_FSETH(hMcbsp, SRGR1, CLKGDV,0xF0);
   /*ÉèÖÃFRSTÎ»*/
   MCBSP_FSETH(hMcbsp, SPCR2,FRST,1); 
   /*Ê¹ÄÜMCBSP0·¢ËÍ*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,1);
   /*Ê¹ÄÜMCBSP0½ÓÊÕ*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,1); 
   /*µÈ´ýÁ½¸öCLKGÊ±ÖÓÖÜÆÚ*/
   asm(" nop ");
   asm(" nop ");
}
/*Ð´ÃüÁîÑÓÊ±*/
void WRITECOM_Delay()
{
	Uint32 j =0;
	for(j= 0;j<0x100;j++)
	{}
}
/*Ð´EEPROMÊ¹ÄÜ*/
void SPI_WriteEN()
{
	/*³õÊ¼»¯McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	SPI_writecominit(hMcbsp);
	/*·¢ËÍ»º³åÇøÊÇ·ñÎª¿Õ*/
	while(!MCBSP_xrdy(hMcbsp)){};
	/*Ê¹ÄÜÆ¬Ñ¡£¬Ê¹GPIO4ÎªµÍ*/
	GPIO_RSET(IODATA,0x00);
	/*Ð´ÈëÊý¾Ý*/	
	MCBSP_write16(hMcbsp,SPI_WRITE_EN);	
	/*½ûÖ¹Æ¬Ñ¡£¬Ê¹GPIO4Îª¸ß*/
	WRITECOM_Delay();
	GPIO_RSET(IODATA,0x10);
	SPIEND_Delay();
	MCBSP_close(hMcbsp);			
}
/*Ð´EEPROM½ûÖ¹*/
void SPI_WriteDI()
{
	/*³õÊ¼»¯McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	SPI_writecominit(hMcbsp);
	/*·¢ËÍ»º³åÇøÊÇ·ñÎª¿Õ*/
	while(!MCBSP_xrdy(hMcbsp)){};
	/*Ê¹ÄÜÆ¬Ñ¡£¬Ê¹GPIO4ÎªµÍ*/
	GPIO_RSET(IODATA,0x00);
	/*Ð´ÈëÊý¾Ý*/	
	MCBSP_write16(hMcbsp,SPI_WRITE_DI);	
	/*½ûÖ¹Æ¬Ñ¡£¬Ê¹GPIO4Îª¸ß*/
	WRITECOM_Delay();
	GPIO_RSET(IODATA,0x10);
	SPIEND_Delay();
	MCBSP_close(hMcbsp);			
}
/*************************************************************************************/
/*		write and read the EEPROM's status register									 */
/*************************************************************************************/
/*¶ÁÐ´EPPROM×´Ì¬×Ö³õÊ¼»¯£¬Êý¾Ý³¤¶ÈÎª16Î»*/
void SPI_status_init(MCBSP_Handle hMcbsp)
{
   /****************************************************************/
   /* Place port in reset - setting XRST & RRST to 0               */
   /****************************************************************/
   /*½øÐÐ·¢ËÍ¸´Î»*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,0);
   /*½øÐÐ½ÓÊÕ¸´Î»*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,0); 
   /*½øÐÐ·ÖÆµÆ÷¸´Î»*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,0);
   /****************************************************************/
   /* Set values of all control registers                          */
   /****************************************************************/
   /*ÉèÖÃSPCR1¼Ä´æÆ÷,SPIÊ±ÖÓÍ£Ö¹Ä£Ê½*/
   MCBSP_FSETH(hMcbsp, SPCR1,CLKSTP,0x3);
   /*ÉèÖÃXCR1¼Ä´æÆ÷£¬µ¥Êý¾ÝÏà£¬·¢ËÍÊý¾Ý³¤¶ÈÎª16Î»*/
   MCBSP_FSETH(hMcbsp, XCR1, XFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, RCR1, RFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, XCR1, XWDLEN1,0x2);
   MCBSP_FSETH(hMcbsp, RCR1, RWDLEN1,0x2);
   /*ÉèÖÃXCR2¼Ä´æÆ÷£¬·¢ËÍÊý¾ÝÑÓÊ±Ò»¸öÎ»*/
   MCBSP_FSETH(hMcbsp, XCR2, XDATDLY,0x1);
   MCBSP_FSETH(hMcbsp, RCR2, RDATDLY,0x1);
   /*ÉèÖÃPCR¼Ä´æÆ÷£¬ÉèÖÃBLCKÎªÊä³ö£¬¼´MCBSPÎªMaster£»
   		            ÉèÖÃBFSXÎªÊä³ö£¬²¢ÇÒÆä¼«ÐÔÎªµÍÓÐÐ§
   		            ÉèÖÃCLKXPÎª¸ß*/
   MCBSP_FSETH(hMcbsp, PCR,CLKXP, 0x0);
   MCBSP_FSETH(hMcbsp, PCR,CLKRP, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,CLKXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXP, 0x1);
   /*ÉèÖÃSRGR2£¬È·¶¨Ê±ÖÓÀ´Ô´ÎªÄÚ²¿µÄCPU£
              £¬È·¶¨Ö¡Í¬²½ÎªµÍÓÐÐ§¡£*/
   MCBSP_FSETH(hMcbsp, SRGR2,CLKSM,1);
   /*Ê¹ÄÜ²¨ÌØÂÊ¼Ä´æÆ÷*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,1);
   /*µÈ´ýÁ½¸öCLKGÊ±ÖÓÖÜÆÚ*/
   asm(" nop ");
   asm(" nop ");
   /*ÉèÖÃSRGR1£¬È·¶¨·ÖÆµÊýÎª0x20£¬MCBSP2µÄ²¨´ýÂÊÎª100KHz*/
//   MCBSP_FSETH(hMcbsp, SRGR1, FWID,0x20);
//   MCBSP_FSETH(hMcbsp, SRGR2,FPER,0x20);
   MCBSP_FSETH(hMcbsp, SRGR1, CLKGDV,0xF0);
   /*ÉèÖÃFRSTÎ»*/
   MCBSP_FSETH(hMcbsp, SPCR2,FRST,1); 
   /*Ê¹ÄÜMCBSP0·¢ËÍ*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,1);
   /*Ê¹ÄÜMCBSP0½ÓÊÕ*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,1); 
   /*µÈ´ýÁ½¸öCLKGÊ±ÖÓÖÜÆÚ*/
   asm(" nop ");
   asm(" nop ");
}
/*¶ÁÐ´EEPROM×´Ì¬¼Ä´æµÄÑÓÊ±*/
void WrReg_Delay()
{
	Uint32 j =0;
	for(j= 0;j<0x150;j++)
	{}
}
/*Ð´EEPROM×´Ì¬¼Ä´æÆ÷*/
void SPI_WrieStatusReg(Uint16 data)
{
	/*³õÊ¼»¯McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	SPI_status_init(hMcbsp);
	/*·¢ËÍ»º³åÇøÊÇ·ñÎª¿Õ*/
	while(!MCBSP_xrdy(hMcbsp)){};
	/*Ê¹ÄÜÆ¬Ñ¡£¬Ê¹GPIO4ÎªµÍ*/
	GPIO_RSET(IODATA,0x00);
	/*Ð´ÈëÊý¾Ý*/	
	MCBSP_write16(hMcbsp,(data+SPI_WRITE_REG));	
	/*½ûÖ¹Æ¬Ñ¡£¬Ê¹GPIO4Îª¸ß*/
	WrReg_Delay();
	GPIO_RSET(IODATA,0x10);
	SPIEND_Delay();
	MCBSP_close(hMcbsp);		
}
/*¶ÁEEPROM×´Ì¬¼Ä´æÆ÷*/
Uint16 SPI_ReadStatusReg()
{
	Uint16 vaule = 0;
	/*³õÊ¼»¯McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	SPI_status_init(hMcbsp);
	/*Ê¹ÄÜÆ¬Ñ¡£¬Ê¹GPIO4ÎªµÍ*/
	GPIO_RSET(IODATA,0x00);
	/*Ð´Èë¶ÁÃüÁî*/	
	MCBSP_write16(hMcbsp,SPI_READ_REG);	
	/*½ûÖ¹Æ¬Ñ¡£¬Ê¹GPIO4Îª¸ß*/
	WrReg_Delay();
	GPIO_RSET(IODATA,0x10);
	SPIEND_Delay();
	/*¶Á¼Ä´æÆ÷*/
	vaule = MCBSP_read16(hMcbsp);
	MCBSP_close(hMcbsp);
	return vaule;			
}
/********************************************************************************/
/*      EEROM write and read function											*/
/********************************************************************************/ 	
/*¶ÁÐ´Êý¾Ý³õÊ¼»¯,Êý¾Ý³¤¶ÈÎª32Î»*/
void SPI_wrdatainit(MCBSP_Handle hMcbsp)
{
   /*³õÊ¼»¯McBSP0*/
   /****************************************************************/
   /* Place port in reset - setting XRST & RRST to 0               */
   /****************************************************************/
   /*½øÐÐ·¢ËÍ¸´Î»*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,0);
   /*½øÐÐ½ÓÊÕ¸´Î»*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,0); 
   /*½øÐÐ·ÖÆµÆ÷¸´Î»*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,0);
   /****************************************************************/
   /* Set values of all control registers                          */
   /****************************************************************/
   /*ÉèÖÃSPCR1¼Ä´æÆ÷,SPIÊ±ÖÓÍ£Ö¹Ä£Ê½*/
   MCBSP_FSETH(hMcbsp, SPCR1,CLKSTP,0x3);
   /*ÉèÖÃXCR1¼Ä´æÆ÷£¬µ¥Êý¾ÝÏà£¬·¢ËÍÊý¾Ý³¤¶ÈÎª32Î»*/
   MCBSP_FSETH(hMcbsp, XCR1, XFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, RCR1, RFRLEN1,0x0);
   MCBSP_FSETH(hMcbsp, XCR1, XWDLEN1,0x5);
   MCBSP_FSETH(hMcbsp, RCR1, RWDLEN1,0x5);
   /*ÉèÖÃXCR2¼Ä´æÆ÷£¬·¢ËÍÊý¾ÝÑÓÊ±Ò»¸öÎ»*/
   MCBSP_FSETH(hMcbsp, XCR2, XDATDLY,0x1);
   MCBSP_FSETH(hMcbsp, RCR2, RDATDLY,0x1);
   /*ÉèÖÃPCR¼Ä´æÆ÷£¬ÉèÖÃBLCKÎªÊä³ö£¬¼´MCBSPÎªMaster£»
   		            ÉèÖÃBFSXÎªÊä³ö£¬²¢ÇÒÆä¼«ÐÔÎªµÍÓÐÐ§
   		            ÉèÖÃCLKXPÎª¸ß*/
   MCBSP_FSETH(hMcbsp, PCR,CLKXP, 0x0);
   MCBSP_FSETH(hMcbsp, PCR,CLKRP, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,CLKXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXM, 0x1);
   MCBSP_FSETH(hMcbsp, PCR,FSXP, 0x1);
   /*ÉèÖÃSRGR2£¬È·¶¨Ê±ÖÓÀ´Ô´ÎªÄÚ²¿µÄCPU£
              £¬È·¶¨Ö¡Í¬²½ÎªµÍÓÐÐ§¡£*/
   MCBSP_FSETH(hMcbsp, SRGR2,CLKSM,1);
   /*Ê¹ÄÜ²¨ÌØÂÊ¼Ä´æÆ÷*/
   MCBSP_FSETH(hMcbsp, SPCR2, GRST,1);
   /*µÈ´ýÁ½¸öCLKGÊ±ÖÓÖÜÆÚ*/
   asm(" nop ");
   asm(" nop ");
   /*ÉèÖÃSRGR1£¬È·¶¨·ÖÆµÊýÎª0x20£¬MCBSP2µÄ²¨´ýÂÊÎª100KHz*/
//   MCBSP_FSETH(hMcbsp, SRGR1, FWID,0x20);
//   MCBSP_FSETH(hMcbsp, SRGR2,FPER,0x20);
   MCBSP_FSETH(hMcbsp, SRGR1, CLKGDV,0xF0);
   /*ÉèÖÃFRSTÎ»*/
   MCBSP_FSETH(hMcbsp, SPCR2,FRST,1); 
   /*Ê¹ÄÜMCBSP0·¢ËÍ*/
   MCBSP_FSETH(hMcbsp, SPCR2, XRST,1);
   /*Ê¹ÄÜMCBSP0½ÓÊÕ*/
   MCBSP_FSETH(hMcbsp, SPCR1, RRST,1); 
   /*µÈ´ýÁ½¸öCLKGÊ±ÖÓÖÜÆÚ*/
   asm(" nop ");
   asm(" nop ");
}
/*¶ÁÐ´Êý¾ÝµÄÑÓÊ±*/
void SPIWR_Delay()
{
	Uint32 j =0;
	//for(j= 0;j<0x220;j++)
	for(j= 0;j<0x250;j++)
	{}
}
/*ÏòEEROMÖÐÐ´ÈëÒ»¸öÊý¾Ý*/
void SPI_WrieSignal(Uint32 address,Uint32 data)
{
	Uint32 k =0;
	Uint16 EPROM_status = 0;
	/*Ê¹ÄÜEEPROMµÄÐ´¹¦ÄÜ*/	
  	SPI_WriteEN();
  	/*È·¶¨Ð´±£»¤ÊÇ·ñ´ò¿ª*/
  	EPROM_status = SPI_ReadStatusReg();
  	while(!(EPROM_status & 0x2)){};
	/*³õÊ¼»¯McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	/*½øÐÐEEPROM¶ÁÐ´³õÊ¼»¯*/
  	SPI_wrdatainit(hMcbsp);
	address = address & 0xFFFF;
	data = data & 0xFF;
	/*·¢ËÍ»º³åÇøÊÇ·ñÎª¿Õ*/
	while(!MCBSP_xrdy(hMcbsp)){};
	/*Ê¹ÄÜÆ¬Ñ¡£¬Ê¹GPIO4ÎªµÍ*/
	GPIO_RSET(IODATA,0x00);
	/*Ð´ÈëÊý¾Ý*/	
	MCBSP_write32(hMcbsp,(SPI_WRITE + (address <<8) + data));	
	/*½ûÖ¹Æ¬Ñ¡£¬Ê¹GPIO4Îª¸ß*/
	SPIWR_Delay();
	GPIO_RSET(IODATA,0x10);
	/*µÈ´ýEEPROMÐ´ÈëÍê³É£¬×î¶àÑÓÊ±10¸öms*/
	for(k = 0; k <0x10000;k++)
	{}
	MCBSP_close(hMcbsp);		
}
/*´ÓEEPROMÖÐ¶Á³öÒ»¸öÊý¾Ý,¶Á»ØÊý¾ÝÖ»ÓÐ×îºó8Î»ÓÐÐ§*/
Uint32 SPI_ReadSignal(Uint32 address)
{
	Uint32 vaule = 0;
	/*³õÊ¼»¯McBSP0*/
	hMcbsp = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	/*½øÐÐEEPROM¶ÁÐ´³õÊ¼»¯*/
  	SPI_wrdatainit(hMcbsp);
	address = (address & 0xffff) <<8;
	vaule = SPI_READ + address;	
	/*·¢ËÍ»º³åÇøÊÇ·ñÎª¿Õ*/
	while(!MCBSP_xrdy(hMcbsp)){};
	/*Ê¹ÄÜÆ¬Ñ¡£¬Ê¹GPIO4ÎªµÍ*/
	GPIO_RSET(IODATA,0x00);
	/*Ð´ÈëÊý¾Ý*/	
	MCBSP_write32(hMcbsp,vaule);	
	/*½ûÖ¹Æ¬Ñ¡£¬Ê¹GPIO4Îª¸ß*/
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
