/*----------------------------------------------------------------------------*/
/* MODULE NAME... LCD       												  */
/* FILENAME...... lcd.c			    										  */
/* DATE CREATED.. Wed 2/4/2004 											      */
/* DATE REVISION. Mon 28/9/2009                                               */
/* COMPANY....... HongXiang Electronic Co,LTD				    	    	  */
/* PREREQUISITS.. 															  */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/* DESCRIPTION:  															  */
/*   																		  */
/* This is an example for lcd of C5509A board								  */
/*----------------------------------------------------------------------------*/
#include <csl.h>
#include <csl_pll.h>
#include <csl_emif.h>
#include <csl_chip.h>
#include <stdio.h>

#define CESECT1 0x200000
#define CESECT2 0x400000
#define CESECT3 0x600000

#define comm 0
#define dat  1

Uint16 x;
Uint32 y;
CSLBool b;
unsigned int datacount = 0;
int databuffer[1000] ={0};
int *LCDOUT, *LCDCOM, *LCDIN, *addr;
Uint16 button;
unsigned char k1_down, k2_down, k3_down, k4_down;
/*锁相环的设置*/
PLL_Config  myConfig      = {
  0,    //IAI: the PLL locks using the same process that was underway 
                //before the idle mode was entered
  1,    //IOB: If the PLL indicates a break in the phase lock, 
                //it switches to its bypass mode and restarts the PLL phase-locking 
                //sequence
  12,    //PLL multiply value; multiply 12 times
  2             //Divide by 2 PLL divide value; it can be either PLL divide value 
                //(when PLL is enabled), or Bypass-mode divide value
                //(PLL in bypass mode, if PLL multiply value is set to 1)
};
/*SDRAM的EMIF设置*/
EMIF_Config emiffig = {
  0x221, 	//EGCR  : the MEMFREQ = 00,the clock for the memory is equal to cpu frequence
  			//		  the WPE = 0 ,forbiden the writing posting when we debug the EMIF
  			//        the MEMCEN = 1,the memory clock is reflected on the CLKMEM pin
  			//        the NOHOLD = 1,HOLD requests are not recognized by the EMIF 
  0xFFFF,	//EMI_RST: any write to this register resets the EMIF state machine
  0x3FFF,	//CE0_1:  CE0 space control register 1
  			//        MTYPE = 011,Synchronous DRAM(SDRAM),16-bit data bus width
  0xFFFF,   //CE0_2:  CE0 space control register 2
  0x00FF,   //CE0_3:  CE0 space control register 3
  			//        TIMEOUT = 0xFF;
  0x1FFF,	//CE1_1:  CE0 space control register 1
            //        Asynchronous, 16Bit
  0xFFFF,	//CE1_2:  CE0 space control register 2
  0x00FF,	//CE1_3:  CE0 space control register 3
  0x1FFF,	//CE2_1:  CE0 space control register 1
            //        Asynchronous, 16Bit
  0xFFFF,	//CE2_2:  CE0 space control register 2
  0x00FF,	//CE2_3:  CE0 space control register 3
  
  0x1FFF,	//CE3_1:  CE0 space control register 1
  0xFFFF,	//CE3_2:  CE0 space control register 2
  0x00FF,	//CE3_3:  CE0 space control register 3
  
  0x2911,   //SDC1:   SDRAM control register 1
  			//		  TRC = 8 
  			//        SDSIZE = 0;SDWID = 0
  			//        RFEN = 1
  			//        TRCD = 2
  			//        TRP  = 2
  0x0410,	//SDPER : SDRAM period register
  			//		  7ns *4096
  0x07FF,    //SDINIT: SDRAM initialization register
  			//        any write to this register to init the all CE spaces,
  			//        do it after hardware reset or power up the C55x device
  0x0131	//SDC2:	  SDRAM control register 2
  			//        SDACC = 0;
  			//        TMRD = 01;
  			//        TRAS = 0101;
  			//        TACTV2ACTV = 0001;								
  };


/*************以下内容为LCD的读写等函数**************//*************以下内容为LCD的读写等函数**************/
/* 参考：LCD说明书
         TMS320VC5509 DSP External Memory Interface (EMIF) Reference Guide
		 明伟5509A开发板用户手册
*/
//*addr为CPLD寄存器操作专用指针，详情见明伟5509A开发板用户手册

void  delay(unsigned int d_time)
{
	while(d_time--);
}


/*----------------LCD忙检测----------------*/
void chk_busy()
{
	unsigned int temp;
	*addr = 0x00ef;
	*LCDCOM = 0x0005;  //RS=0(comm), RW=1(read), RST=1
	temp = 0x0080;
	while((temp&0x0080)!=0)
	{
		*LCDCOM = 0x000d;  //EN=1
		*addr = 0x00f7;
		delay(10);
		temp = *LCDIN;
		*addr = 0x00ef;
		*LCDCOM = 0x0005;  //EN=0
	}
	*addr = 0x00ff;
	//while((LCD_DAT&0x80)!=0); 
}

/*------------------写LCD------------------*/
void  wr_lcd(unsigned char dat_comm, unsigned int content)
{
	chk_busy();
	if(dat_comm)
	{
		*addr = 0x00ef;
		*LCDCOM = 0x0003;  //RS=1(data), RW=0(write), RST=1
		*addr = 0x00d7;
		*LCDOUT = content;
		*addr = 0x00cf;
		*LCDCOM = 0x000b;  //EN=1
		delay(30);
		*LCDCOM = 0x0003;  //EN=0
		*addr = 0x00ff;
	}
	else
	{
		*addr = 0x00ef;
		*LCDCOM = 0x0001;  //RS=0(data), RW=0(write), RST=1
		*addr = 0x00d7;
		*LCDOUT = content;
		*addr = 0x00cf;
		*LCDCOM = 0x0009;  //EN=1
		delay(30);
		*LCDCOM = 0x0001;  //EN=0
		*addr = 0x00ff;
	}
}


/*----------------清DDRAM------------------*/
void clrram (void)
{
    wr_lcd (comm,0x30);
    wr_lcd (comm,0x01);
}


/*------------------初始化-----------------*/
void  lcd_init(void)
{
    *addr = 0x00ef;
	delay(10);
	*LCDCOM = 0x0000;         //RESET
	delay(1000);
	*LCDCOM = 1;
    wr_lcd (comm,0x30);  /*30---基本指令动作*/  
    wr_lcd (comm,0x01);  /*清屏，地址指针指向00H*/
    wr_lcd (comm,0x06);  /*光标的移动方向*/
    wr_lcd (comm,0x0c);  /*开显示，关游标*/		
}


/*---------------欢迎界面------------------*/
void Smenu()
{
	wr_lcd (comm,0x30);
	wr_lcd (comm,0x92);
	wr_lcd (dat,0xc3);          //"明"
	wr_lcd (dat,0xf7);
    wr_lcd (dat,0xce);          //"伟"
	wr_lcd (dat,0xb0);
    wr_lcd (dat,0xb5);          //"电"
	wr_lcd (dat,0xe7);
    wr_lcd (dat,0xd7);          //"子"
	wr_lcd (dat,0xd3);
}


main()
{
	/*初始化CSL库*/	
    CSL_init();
    
    /*EMIF为全EMIF接口*/
    CHIP_RSET(XBSR,0x0a01);
    
    /*设置系统的运行速度为144MHz*/
    PLL_config(&myConfig);
    
    /*初始化DSP的EMIF*/
    EMIF_config(&emiffig);

   	addr =  (int *)CESECT2;
    LCDCOM =  (int *)CESECT3;
	LCDOUT =  (int *)CESECT3;
	LCDIN  =  (int *)CESECT2;

	lcd_init();
	clrram();
	Smenu();
	for(;;);

}
/******************************************************************************\
* End of lcd.c
\******************************************************************************/
