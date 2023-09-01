/******************************************************************************/
/*  Copyright 2004 by SEED Electronic Technology LTD.                         */
/*  All rights reserved. SEED Electronic Technology LTD.                      */
/*  Restricted rights to use, duplicate or disclose this code are             */
/*  granted through contract.                                                 */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
/*----------------------------------------------------------------------------*/
/* MODULE NAME... EMIF														  */
/* FILENAME...... emif.c													  */
/* DATE CREATED.. Wed 2/4/2004 												  */
/* PROJECT....... write and read data between the CPU and SDRAM				  */
/* COMPONENT..... 															  */
/* PREREQUISITS.. 															  */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/* DESCRIPTION:  															  */
/*   																		  */
/* This is an example for EMIF of C5509										  */
/*----------------------------------------------------------------------------*/
#include <csl.h>
#include <csl_pll.h>
#include <csl_emif.h>
#include <csl_chip.h>

#define CESECT1 0x200000
#define CESECT2 0x400000
#define CESECT3 0x600000

Uint16 x;
Uint32 y;
CSLBool b;

unsigned int datacount = 0;
unsigned int databuffer[1000] ={0};
int *souraddr,*deminaddr;
int *fwaddr, *fraddr;
Uint16 addbias;
Uint16 fstatus, fstatus2;
Uint16 success;

/*���໷������*/
PLL_Config  myConfig      = {
  0,    //IAI: the PLL locks using the same process that was underway 
                //before the idle mode was entered
  1,    //IOB: If the PLL indicates a break in the phase lock, 
                //it switches to its bypass mode and restarts the PLL phase-locking 
                //sequence
  12,    //PLL multiply value; multiply 12 times
  1             //Divide by 2 PLL divide value; it can be either PLL divide value 
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
            //        Asynchronous, 16Bit
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

void delay(Uint16 k)
{
	while(k>0)
	{
		k--;
	}
}

void ddelay(Uint16 kt)
{
    Uint16 jj,kk;
	for(jj=0;jj<65535;jj++)
	{
		kk = kt;
		while(kk>0)
		{
			kk--;
		}
	}
}

/*************��������ΪAM29LV800BT�Ķ�д�Ⱥ���**************/
/* �ο���AM29LV800BT��оƬ�ֲ�
         TMS320VC5509 DSP External Memory Interface (EMIF) Reference Guide
		 HX-5509A ������ʹ���ֲ�
*/
void Flash_CS()
{
	deminaddr = (int *)CESECT2;
	*deminaddr = 0x00fd;
    deminaddr = (int *)CESECT3;
	*deminaddr = 0x0040;
	deminaddr = (int *)CESECT2;
	*deminaddr = 0x00ff;
}

void Flash_disCS()
{
	deminaddr = (int *)CESECT2;
	*deminaddr = 0x00fd;
    deminaddr = (int *)CESECT3;
	*deminaddr = 0x00c0;
	deminaddr = (int *)CESECT2;
	*deminaddr = 0x00ff;
}

void Flash_Reset()  //AM29LV800��λ
{
	deminaddr = (int *)CESECT2;
	*deminaddr = 0x00fd;
    deminaddr = (int *)CESECT3;
	*deminaddr = 0x0000;
	delay(1000);
	*deminaddr = 0x0040;
	*deminaddr = 0x00c0;
	deminaddr = (int *)CESECT2;
	*deminaddr = 0x00ff;
}

Uint16 Flash_Erase_all()  //AM29LV800оƬ����
{
    Flash_CS();

	deminaddr = (int *)CESECT1;
	addbias = 0x0555;
	*(deminaddr+addbias) = 0x00aa;
	addbias = 0x02aa;
	*(deminaddr+addbias) = 0x0055;
	addbias = 0x0555;
	*(deminaddr+addbias) = 0x0080;
	addbias = 0x0555;
	*(deminaddr+addbias) = 0x00aa;
	addbias = 0x02aa;
	*(deminaddr+addbias) = 0x0055;
	addbias = 0x0555;
	*(deminaddr+addbias) = 0x0010;
	delay(100);
	fstatus = *(deminaddr+addbias);
	fstatus &= 0x0040;
	fstatus2 = *(deminaddr+addbias);
	if((fstatus&fstatus2)!=0)
	{
		Flash_disCS();
		return 0;
	}
	else
	{
		while(fstatus!=0x00ff)
	    {
			ddelay(500);
			fstatus = *(deminaddr+addbias);
			fstatus &= 0x00ff;
	    }
		Flash_disCS();
		return 1;
	}
}

Uint16 Flash_Erase_sector()  //AM29LV800�ֶβ���
{
	return 1;
}

void Flash_Write_init()  //AM29LV800��д��ʼ��
{
	Flash_CS();

	deminaddr = (int *)CESECT1;
	addbias = 0x0555;
	*(deminaddr+addbias) = 0x00aa;
	addbias = 0x02aa;
	*(deminaddr+addbias) = 0x0055;
	addbias = 0x0555;
	*(deminaddr+addbias) = 0x0020;	
}

Uint16 Flash_Write(Uint16 waddr, Uint16 wdata) //AM29LV800��д
{
    *(fwaddr+waddr) = 0x00a0;
	*(fwaddr+waddr) = wdata;
	delay(10000);
	fstatus = *(fwaddr+waddr);
    while(fstatus!=wdata)
	{
		delay(10000);
		fstatus = *(fwaddr+waddr);
	}
	return 1;
}

void Flash_Write_end()  //AM29LV800��д����
{
	deminaddr = (int *)CESECT1;
	*deminaddr = 0x0090;
	*deminaddr = 0x0000;

	Flash_disCS();
}

Uint16 Flash_Read(Uint16 raddr)  //AM29LV800��
{
	Uint16 frtemp;
	frtemp = *(fraddr+raddr);
	return frtemp;
}

main()
{
	/*��ʼ��CSL��*/	
    CSL_init();
    
    /*EMIFΪȫEMIF�ӿ�*/
    CHIP_RSET(XBSR,0x0a01);
    
    /*����ϵͳ�������ٶ�Ϊ144MHz*/
    PLL_config(&myConfig);
    
    /*��ʼ��DSP���ⲿSDRAM*/
    EMIF_config(&emiffig);

    Flash_Reset();

	//Flash_Erase_all���д�ԼΪ14s���ϣ�Ϊ��Լʱ��ע�͵����û������и�����Ҫȡ��ע��
	success = Flash_Erase_all();


	//���³���Ϊ��дAM29LV800
	Flash_Write_init();
	fwaddr = (int *)CESECT1;  //��ַ����ָ��5509��CE1�ռ䣨AM29LV800���ڣ�
	fwaddr += 0x10000;        //ָ��AM29LV800��1����
	for(datacount=0;datacount<1000;datacount++)
	{
		success = Flash_Write(datacount, datacount);
	}
	Flash_Write_end();

	//���³���Ϊ��AM29LV800
	for(datacount=0;datacount<1000;datacount++)
	{
		databuffer[datacount]=0;
	}
	Flash_CS();
	fraddr = fwaddr;
	for(datacount=0;datacount<1000;datacount++)
	{
		databuffer[datacount] = Flash_Read(datacount);
	}
	Flash_disCS();

	//���е��˴�����view--memory��鿴databuffer��ʼ��ַ�����ݣ����Ƿ���д������
    while(1);
}
/******************************************************************************\
* End of pll2.c
\******************************************************************************/
