#include <stdio.h>

#include <csl.h>
#include <csl_irq.h>
#include <csl_pll.h>
#include <csl_timer.h>
#include <csl_gpio.h>
#include <csl_chiphal.h>
#include "timerUser.h"
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

typedef (*CallBack)(void);

CallBack handle;

void timerSetCallbcak(CallBack ptr)
{
	handle = ptr;
}

void timerCreated(Uint16 ms)
{
	Uint16 prd ;

	/*定时器为主频一半，系统板主频144M， 定时器定时时间为 72M/prd*/
	prd = 1000/ms;
	prd = 72000/prd;

	 /* Set IVPH/IVPD to start of interrupt vector table */
	 /*修改寄存器IVPH，IVPD，重新定义中断向量表*/
	IRQ_setVecs((Uint32)(&VECSTART));

	 /* Temporarily disable all maskable interrupts */
	/*禁止所有可屏蔽的中断源*/
	old_intm = IRQ_globalDisable();

	/* Open Timer 0, set registers to power on defaults */
	/*打开定时器0，设置其为上电的的默认值，并返回其句柄*/
	mhTimer0 = TIMER_open(TIMER_DEV0, TIMER_OPEN_RESET);

	/* Get Event Id associated with Timer 0, for use with */
	/* CSL interrupt enable functions.                    */
	/*获取定时器0的中断ID号*/
	eventId0 = TIMER_getEventId(mhTimer0);

	/* Clear any pending Timer interrupts */
	/*清除定时器0的中断状态位*/
	IRQ_clear(eventId0);

	/* Place interrupt service routine address at */
	/* associated vector location */
	/*为定时器0设置中断服务程序*/
	IRQ_plug(eventId0,&timer0Isr);

	/* Write configuration structure values to Timer control regs */
	/*设置定时器0的控制与周期寄存器*/
	timCfg0.prd = prd;
	TIMER_config(mhTimer0, &timCfg0);

	/* Enable Timer interrupt */
	/*使能定时器的中断*/
	IRQ_enable(eventId0);

	/* Enable all maskable interrupts */
	/*设置寄存器ST1的INTM位，使能所有的中断*/
	IRQ_globalEnable();

	/* Start Timer */
	/*启动定时器0*/
	TIMER_start(mhTimer0);

	GPIO_RSET(IODIR,0x40);

	/* Restore old value of INTM */
	/*恢复INTM旧的值*/
	IRQ_globalRestore(old_intm);

	/* We are through with timer, so close it */
	/*关掉定时器0*/
	TIMER_close(mhTimer0);
}
/*定时器0的中断程序*/
interrupt void timer0Isr(void)
{
	if(handle != NULL)
	{
		handle();
	}
}

