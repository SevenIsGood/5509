/* "@(#) DSP/BIOS 4.90.270 07-06 (barracuda-m10)" */
// fcgexcellence@163.com
/***********************************************************************
  ��������   ���       ���   ��������
  �����������   ���       ���   ��������
  �             ���       ���   � 
  �             ���       ���   �
  �����������   ���       ���   ��������
  �����������   ���       ���   ��������
  �             ���       ���   �
  �             ���       ���   �
  ��������    ���     ���    ��������
  �����������       ����       ��������
***********************************************************************/

#include <stdio.h>

#include <csl.h>
#include <csl_irq.h>
#include <csl_timer.h>
#include <csl_chiphal.h>

/* This is a simple timer example to set the timer with */
/* an interrupt period of 0x400 cycles.                 */

/* The example uses predefined CSL macros and symbolic  */
/* constants to create the correct register settings    */
/* needed to configure the timer.                       */


/* Reference start of interrupt vector table   */
/* This symbol is defined in file, vectors.s55 */
extern void VECSTART(void);

#define TIMER_CTRL    TIMER_TCR_RMK(\
                      TIMER_TCR_IDLEEN_DEFAULT,    /* IDLEEN == 0 */ \
                      TIMER_TCR_FUNC_OF(0),        /* FUNC   == 0 */ \
                      TIMER_TCR_TLB_RESET,         /* TLB    == 1 */ \
                      TIMER_TCR_SOFT_BRKPTNOW,     /* SOFT   == 0 */ \
                      TIMER_TCR_FREE_WITHSOFT,     /* FREE   == 0 */ \
                      TIMER_TCR_PWID_OF(0),        /* PWID   == 0 */ \
                      TIMER_TCR_ARB_RESET,         /* ARB    == 1 */ \
                      TIMER_TCR_TSS_START,         /* TSS    == 0 */ \
                      TIMER_TCR_CP_PULSE,          /* CP     == 0 */ \
                      TIMER_TCR_POLAR_LOW,         /* POLAR  == 0 */ \
                      TIMER_TCR_DATOUT_0           /* DATOUT == 0 */ \
)  


/* Create a TIMER configuration structure that can be passed */
/* to TIMER_config CSL function for initialization of Timer  */
/* control registers.                                        */
TIMER_Config timCfg0 = {
   TIMER_CTRL,               /* TCR0 */
   0x3400u,                  /* PRD0 */
   0x0000                    /* PRSC */
};


Uint16 eventId0;  

/* Create a TIMER_Handle object for use with TIMER_open */
TIMER_Handle mhTimer0;

volatile Uint16 timer0_cnt = 0;

/* Function/ISR prototypes */
interrupt void timer0Isr(void);
void taskFxn(void);

int old_intm;
Uint16 tim_val;
Uint16 xfchange = 0;
Uint16 ms,f;

void main(void)
{
	 /* Initialize CSL library - This is REQUIRED !!! */
	 /*CLS��ĳ�ʼ�������Ǳ����*/
	 CSL_init();
	
	 /* Set IVPH/IVPD to start of interrupt vector table */
	 /*�޸ļĴ���IVPH��IVPD�����¶����ж�������*/
	 IRQ_setVecs((Uint32)(&VECSTART));
	 
	 /* Temporarily disable all maskable interrupts */
	/*��ֹ���п����ε��ж�Դ*/
	old_intm = IRQ_globalDisable();   
	
	/* Open Timer 0, set registers to power on defaults */
	/*�򿪶�ʱ��0��������Ϊ�ϵ�ĵ�Ĭ��ֵ������������*/
	mhTimer0 = TIMER_open(TIMER_DEV0, TIMER_OPEN_RESET);
	
	/* Get Event Id associated with Timer 0, for use with */
	/* CSL interrupt enable functions.                    */
	/*��ȡ��ʱ��0���ж�ID��*/         
	eventId0 = TIMER_getEventId(mhTimer0);
	
	/* Clear any pending Timer interrupts */
	/*�����ʱ��0���ж�״̬λ*/
	IRQ_clear(eventId0);
	
	/* Place interrupt service routine address at */
	/* associated vector location */
	/*Ϊ��ʱ��0�����жϷ������*/
	IRQ_plug(eventId0,&timer0Isr);
	
	/* Write configuration structure values to Timer control regs */
	/*���ö�ʱ��0�Ŀ��������ڼĴ���*/ 
	TIMER_config(mhTimer0, &timCfg0);  
	
	/* Enable Timer interrupt */
	/*ʹ�ܶ�ʱ�����ж�*/
	IRQ_enable(eventId0);             
	
	/* Enable all maskable interrupts */
	/*���üĴ���ST1��INTMλ��ʹ�����е��ж�*/
	IRQ_globalEnable();      
	
	/* Start Timer */
	/*������ʱ��0*/
	TIMER_start(mhTimer0);
	   
        //������ԭ����TIMER�Ӷ� 
        f=210;
	ms=0;
	for(;;)
	{
		// Wait for at least 10 timer periods
	    //�ȴ�10����ʱ����
		if(xfchange == 0) 
		{
		   //����XF��LED
		   CHIP_FSET(ST1_55,XF,1);  
		}
		else
		{
			//�ص�XF��LED
		  	CHIP_FSET(ST1_55,XF,0); 
		}
	}

/*	f=500;
	ms=0;
	while(1)
        {
             while(ms!=f);    
             ms=0;
             CHIP_FSET(ST1_55,XF,1);  
             while(ms!=f);
             ms=0;
             CHIP_FSET(ST1_55,XF,0);
        //     f=f-1;
         //    if(1==f)
           //     f=500;
        }



	/* Restore old value of INTM */
	/*�ָ�INTM�ɵ�ֵ*/
	IRQ_globalRestore(old_intm);
	
	/* We are through with timer, so close it */
	/*�ص���ʱ��0*/
	TIMER_close(mhTimer0); 
}


/*��ʱ��0���жϳ���*/
/*
interrupt void timer0Isr(void)
{
    ms++;
    ++timer0_cnt;
    if(timer0_cnt == 500)
    {
    	xfchange = 1;
    }
    if(timer0_cnt == 1000)
    {
    	timer0_cnt = 0;
    	xfchange = 0;	
    }
}
*/


interrupt void timer0Isr(void)
{
    ms++;
    ++timer0_cnt;
    if(timer0_cnt == f)
    {
    	xfchange = 1;
    	f=f-3;
    	if(f==0)
    	  f=210;
    }
    if(timer0_cnt == 2*f)
    {
    	timer0_cnt = 0;
    	xfchange = 0;	
    	f=f-3;
    	if(f==0)
    	  f=210;
    }
}
