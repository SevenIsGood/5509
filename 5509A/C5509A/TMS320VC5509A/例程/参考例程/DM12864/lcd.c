/*----------------------------------------------------------------------------*/
/* MODULE NAME... LCD       												  */
/* FILENAME...... lcd.c			    										  */
/* DATE CREATED.. Wed 2/4/2004 												  */
/* COMPONENT..... 															  */
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

//#define LCD_COM (*((unsigned int *)0x400000))
//#define LCD_DAT (*((unsigned int *)0x400002))

#define LCD_COM (*((unsigned int *)0x600800))
#define LCD_DAT (*((unsigned int *)0x600802))

#define comm 0
#define dat  1

Uint16 x;
Uint32 y;
CSLBool b;
unsigned int datacount = 0;
int counter;
int databuffer[1000] ={0};
int *LCDOUT, *LCDCOM, *LCDIN, *addr;
Uint16 button;
unsigned char k1_down, k2_down, k3_down, k4_down;
/*���໷������*/
PLL_Config  myConfig      = {
  0,    //IAI: the PLL locks using the same process that was underway 
                //before the idle mode was entered
  1,    //IOB: If the PLL indicates a break in the phase lock, 
                //it switches to its bypass mode and restarts the PLL phase-locking 
                //sequence
  12,    //PLL multiply value; multiply 12 times
  12             //Divide by 2 PLL divide value; it can be either PLL divide value 
                //(when PLL is enabled), or Bypass-mode divide value
                //(PLL in bypass mode, if PLL multiply value is set to 1)
};
/*SDRAM��EMIF����*/
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


/*************��������ΪLCD�Ķ�д�Ⱥ���**************/
/* �ο���LCD˵����
         TMS320VC5509 DSP External Memory Interface (EMIF) Reference Guide
		 ��ΰ5509A�������û��ֲ�
*/
//*addrΪCPLD�Ĵ�������ר��ָ�룬�������ΰ5509A�������û��ֲ�

void  delay(unsigned int d_time)
{
	while(d_time--);
}


/*----------------LCDæ���----------------*/
void chk_busy()
{
	unsigned int temp=0x80;

	while((temp&0x080)==0x080)
	{
		delay(10);
		temp = LCD_COM;
	}

counter++;
}

/*------------------дLCD------------------*/
void  wr_lcd(unsigned char dat_comm, unsigned int content)
{
//	chk_busy();
	unsigned int i,j;
	for(i=0;i<1000;i++)
		for(j=0;j<100;j++);
	if(dat_comm)
	{
		///DATA
		LCD_DAT=content;
	}
	else
	{
	   ///com
	   LCD_COM=content;
	}
}


/*----------------��DDRAM------------------*/
void clrram (void)
{
    wr_lcd (comm,0x30);
    wr_lcd (comm,0x01);
}


/*------------------��ʼ��-----------------*/
void  lcd_init(void)
{
    wr_lcd (comm,0x30);  /*30---����ָ���*/  
    wr_lcd (comm,0x01);  /*��������ַָ��ָ��00H*/
    wr_lcd (comm,0x06);  /*�����ƶ�����*/
    wr_lcd (comm,0x0c);  /*����ʾ�����α�*/		
}


/*---------------��ӭ����------------------*/
void Smenu()
{
	wr_lcd (comm,0x30);
	wr_lcd (comm,0x92);
	wr_lcd (dat,0xc3);          //"��"
	wr_lcd (dat,0xf7);
    wr_lcd (dat,0xce);          //"ΰ"
	wr_lcd (dat,0xb0);
    wr_lcd (dat,0xb5);          //"��"
	wr_lcd (dat,0xe7);
    wr_lcd (dat,0xd7);          //"��"
	wr_lcd (dat,0xd3);
}


main()
{
	counter=0;
	/*��ʼ��CSL��*/	
    CSL_init();
    
    /*EMIFΪȫEMIF�ӿ�*/
    CHIP_RSET(XBSR,0x0a01);
    
    /*����ϵͳ�������ٶ�Ϊ144MHz*/
    PLL_config(&myConfig);
    
    /*��ʼ��DSP��EMIF*/
    EMIF_config(&emiffig);

   	addr =  (int *)CESECT2;
    LCDCOM =  (int *)CESECT3;
	LCDOUT =  (int *)CESECT3;
	LCDIN  =  (int *)CESECT2;

	lcd_init();
	clrram();
	Smenu();
	for(;;);

	/*
	*addr = 0x00fe;
	while(1)
	{
		button = *addr;
		//scan K1
		if(!(button&0x0001))  
		{
			if(k1_down==0)
			{
				printf("K1 press\n");
				k1_down = 1;
			}
		}
		else
		{
			k1_down = 0;
		}
		//scan K2
		if(!(button&0x0002))  
		{
			if(k2_down==0)
			{
				printf("K2 press\n");
				k2_down = 1;
			}
		}
		else
		{
			k2_down = 0;
		}
		//scan K3
		if(!(button&0x0004))  
		{
			if(k3_down==0)
			{
				printf("K3 press\n");
				k3_down = 1;
			}
		}
		else
		{
			k3_down = 0;
		}
		//scan K4
		if(!(button&0x0008))  
		{
			if(k4_down==0)
			{
				printf("K4 press\n");
				k4_down = 1;
			}
		}
		else
		{
			k4_down = 0;
		}
	}*/
}
/******************************************************************************\
* End of lcd.c
\******************************************************************************/
