/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_timer1.c---------
 * This is a simple timer example to set the timer with
 * an interrupt period of 0x400 cycles.                
 * The example uses predefined CSL macros and symbolic 
 * constants to create the correct register settings   
 * needed to configure the timer. 
 */

#include <stdio.h>

#include <csl.h>
#include <csl_irq.h>
#include <csl_timer.h>

//---------Global data definition---------
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
   0x0400u,                  /* PRD0 */
   0x0000                    /* PRSC */
};

/* Create a TIMER_Handle object for use with TIMER_open */
TIMER_Handle mhTimer0;

volatile Uint16 timer0_cnt = 0;
Uint16 eventId0;  
int old_intm;
Uint16 tim_val;

//---------Function prototypes---------

/* Reference start of interrupt vector table */
/* This symbol is defined in vectors_timer1.s55     */
extern void VECSTART(void);

/* Function/ISR prototypes */
interrupt void timer0Isr(void);
void taskFxn(void);

//---------main routine---------
void main(void)
{
	/* Initialize CSL library */
	CSL_init();
	
	/* Set IVPH/IVPD to start of interrupt vector table */
	IRQ_setVecs((Uint32)(&VECSTART));
	
	/* Call function to set timer */
	taskFxn();
}

void taskFxn(void)
{
    /* Temporarily disable all maskable interrupts */
    old_intm = IRQ_globalDisable();   

    /* Open Timer 0, set registers to power on defaults */
    mhTimer0 = TIMER_open(TIMER_DEV0, TIMER_OPEN_RESET);

    /* Get Event Id associated with Timer 0, for use with */
    /* CSL interrupt enable functions.                    */         
    eventId0 = TIMER_getEventId(mhTimer0);

    /* Clear any pending Timer interrupts */
    IRQ_clear(eventId0);
    
    /* Place interrupt service routine address at */
    /* associated vector location */
    IRQ_plug(eventId0,&timer0Isr);              

    /* Write configuration structure values to Timer control regs */ 
    TIMER_config(mhTimer0, &timCfg0);  
 
    /* Enable Timer interrupt */
    IRQ_enable(eventId0);             
   
    /* Enable all maskable interrupts */
    IRQ_globalEnable();      
 
    /* Start Timer */
    TIMER_start(mhTimer0);

    /* Wait for at least 10 timer periods */
    while(timer0_cnt != 10) {
        ;  
    }
	
	/* Consider TEST PASSED if 10 interrupts occur */
    printf("\nTEST PASSED\n");
   
    /* Restore old value of INTM */
    IRQ_globalRestore(old_intm);
    
    /* We are through with timer, so close it */
    TIMER_close(mhTimer0);
}

interrupt void timer0Isr(void)
{
    ++timer0_cnt;
}
