
#include <csl.h>
#include <csl_pll.h>
#include <csl_chip.h>
#include <csl_gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <csl_irq.h>
#include <csl_mcbsp.h>
#include "EEPROM.h"

MCBSP_Handle hMcbsp0;


extern void VECSTART(void);
interrupt void Ext0Isr(void);
/*锁相环的设置*/
PLL_Config  myConfig      = {
  0,    //IAI: the PLL locks using the same process that was underway 
                //before the idle mode was entered
  1,    //IOB: If the PLL indicates a break in the phase lock, 
                //it switches to its bypass mode and restarts the PLL phase-locking 
                //sequence
  12,    //PLL multiply value; multiply 24 times
  1             //Divide by 2 PLL divide value; it can be either PLL divide value 
                //(when PLL is enabled), or Bypass-mode divide value
                //(PLL in bypass mode, if PLL multiply value is set to 1)
};
Uint16 IstFlag=0;
FILE  *fptr;
Uint16 FileDat=0xbb;
Uint16 DeviceAddr=0;
Uint16 ReturnLenth=64; 
Uint16 EepromRdTmp[64]={0};
main()
{
    Uint16 kk,mm,nn;
	/*初始化CSL库*/	
    CSL_init();
    IRQ_setVecs((Uint32)(&VECSTART));
    /*设置系统的运行速度为204MHz*/
    PLL_config(&myConfig);
    IRQ_globalDisable();
    	/*初始化McBSP0*/
	hMcbsp0 = MCBSP_open(MCBSP_PORT0,MCBSP_OPEN_RESET);
	  MCBSP_FSET(PCR0,XIOEN,1);
	  MCBSP_FSET(PCR0,RIOEN,1);  
	  MCBSP_FSET(PCR0,FSXM,1);
	  MCBSP_FSET(PCR0,FSRM,0); 	
	  MCBSP_FSET(PCR0,CLKXM,1);
	  MCBSP_FSET(PCR0,CLKRM,0);
	//IRQ_plug(IRQ_EVT_INT0,&Ext0Isr);
    //IRQ_enable(IRQ_EVT_INT0);
    //IRQ_globalEnable();     
    /*确定方向为输出*/
    GPIO_FSET(IODIR,IO4DIR,1);
      BootSyn(1);
      BootClk(0);
      BootTx(0); 
      EepromWrReg(0x02); 
      DeviceAddr=0;
      fptr = fopen("a5509gpio.bin", "rb");
      ReturnLenth=64;
      kk=0;
      while(ReturnLenth==64)
       {
            ReturnLenth=fread(BlockWr,sizeof(short),64,fptr);
            DeviceAddr=kk*0x40;
            EepromWr(DeviceAddr);
            delay();  
            kk++;            
         }
      DeviceAddr=kk*0x40;
      EepromWr(DeviceAddr);
    fclose(fptr);          
      //EepromWrDi();
      delay();
      delay();
       for(kk=0;kk<100;kk++)
        {     
           nn=kk*64;    
           for(mm=0;mm<64;mm++)
            {EepromRdTmp[mm]=EepromRd(nn+mm);}       
         } 
      printf("Burning CSI25C256 is Good.......\n");
      while(1);	    				  
}
void delay()
{
	Uint32 j = 0,k = 0;
	for(j = 0;j<10000;j++)
	{
		for(k= 0;k<100;k++)
		{}
	}
}
