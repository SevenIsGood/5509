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
#include <csl_irq.h>
#include <csl_pll.h>
#include <csl_emif.h>
#include <csl_chip.h>
#include <stdio.h>

CSLBool b;

Uint16 eventId0;
int old_intm;
interrupt void int1(void);

//---------Function prototypes---------
/* Reference start of interrupt vector table   */
/* This symbol is defined in file, vectors_IP.s55 */
extern void VECSTART(void);

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


/***************5509A�ж����ã�ʹ��INT1�ж�***********************/
/*�ο����ϣ� TMS320C55x Chip Support Library API Reference Guide (Rev. J)
             TMS320VC5509A Data Sheet
			 ��ΰ5509A�������û��ֲ�                            */
void INTconfig()
{
    /* Temporarily disable all maskable interrupts */
    IRQ_setVecs((Uint32)(&VECSTART));

	/* Temporarily disable all maskable interrupts */
    old_intm = IRQ_globalDisable();

	/* Get Event Id associated with External INT1(8019), for use with */
	eventId0 = IRQ_EVT_INT1;

	/* Clear any pending INT1 interrupts */
	IRQ_clear(eventId0);

	/* Place interrupt service routine address at */
    /* associated vector location */
    IRQ_plug(eventId0,&int1);

	/* Enable INT1(8019) interrupt */
    IRQ_enable(eventId0); 

	/* Enable all maskable interrupts */
    IRQ_globalEnable();    
}

main()
{
	/*��ʼ��CSL��*/	
    CSL_init();
    
    /*EMIFΪȫEMIF�ӿ�*/
    CHIP_RSET(XBSR,0x0a01);
    
    /*����ϵͳ�������ٶ�Ϊ144MHz*/
    PLL_config(&myConfig);  

	//���ò�ʹ��5509AоƬ��INT0�ж�(EXINT�ж�)
	INTconfig();

	while(1);
}

//External INT0(EXINT)�жϴ�����
interrupt void int1()
{
    printf("EXINT ouccers\n");
}

/******************************************************************************\
* End of pll2.c
\******************************************************************************/
