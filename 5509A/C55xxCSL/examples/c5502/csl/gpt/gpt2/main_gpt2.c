/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_gpt2.c---------
 * This example configures GPT in 64 bit mode and also demonstrates 
 * how to plug an ISR for it.
 */

#include <stdio.h>

#include <csl.h>
#include <csl_gpt.h>
#include <csl_irq.h>

//---------Global constants---------

//---------Global data definition---------

Uint16 success12 = TRUE;
Uint16 oldIntm, gpt12Evt_Id;
volatile Uint16 hitIsr12 = FALSE;

GPT_Handle myhGpt;

//Config structure setup for GPTx in DUAL-32 bit un-chained mode
GPT_Config myGptCfg = 
{
	0,
	GPT_GPTGPINT_RMK(
		GPT_GPTGPINT_TIN1INV_DEFAULT,
		GPT_GPTGPINT_TIN1INT_DEFAULT
	),
	GPT_GPTGPEN_RMK(
		GPT_GPTGPEN_TOUT1EN_DEFAULT,
		GPT_GPTGPEN_TIN1EN_DEFAULT	
	),
	GPT_GPTGPDIR_RMK(
		GPT_GPTGPDIR_TOUT1DIR_DEFAULT,
		GPT_GPTGPDIR_TIN1DIR_DEFAULT
	),
	GPT_GPTGPDAT_RMK(
		GPT_GPTGPDAT_TOUT1DAT_DEFAULT,
		GPT_GPTGPDAT_TIN1DAT_DEFAULT
	),
	0xFFFF,	//PRD1
	0xFFFF,	//PRD2
	0xFFFF,	//PRD3
	0xFFFF,	//PRD4
	GPT_GPTCTL1_RMK(
		GPT_GPTCTL1_TIEN_DEFAULT,
		GPT_GPTCTL1_CLKSRC_DEFAULT,
		GPT_GPTCTL1_ENAMODE_ONCE,
		GPT_GPTCTL1_PWID_DEFAULT,
		GPT_GPTCTL1_CP_DEFAULT,
		GPT_GPTCTL1_INVIN_DEFAULT,
		GPT_GPTCTL1_INVOUT_DEFAULT			
	),
	0,
	GPT_GPTGCTL1_RMK(
		GPT_GPTGCTL1_TDDR34_DEFAULT,
		GPT_GPTGCTL1_PSC34_DEFAULT,
		GPT_GPTGCTL1_TIMMODE_64BIT_GPTIM,
		GPT_GPTGCTL1_TIM34RS_IN_RESET,
		GPT_GPTGCTL1_TIM12RS_IN_RESET
	)		
};

// Reference the start of the interrupt vector table
// This symbol is defined in file vectors_gpt1.s55
extern Uint32 vecs;

//---------Function prototypes---------
interrupt void gpt0Isr(void);
Uint16 taskFxn(GPT_Handle, Uint16);
void setupInterrupts(GPT_Handle, Uint16);
void clearInterrupts(GPT_Handle, Uint16);

//---------main routine---------
void main(void)
{
	//Initialize CSL
	CSL_init();    

	//Initialize interrupt vectors   
	IRQ_setVecs((Uint32)&vecs);

	//Disable interrupts globally
	oldIntm = IRQ_globalDisable();
	
	//Open GPT0 and verify if it is valid
	myhGpt = GPT_open(GPT_DEV0, GPT_OPEN_RESET);
	if (myhGpt == (GPT_Handle)INV)
		printf("\nERROR: Invalid Handle, GPT_open failed\n");

	//Get GPTx EventId
	gpt12Evt_Id = GPT_getEventId(myhGpt);
	
	//Setup ISR for GPT0		
	setupInterrupts(myhGpt, 0);	
	
	//Configure GPT0 and verfiy that it is running and generating interrupt
	success12 &= taskFxn(myhGpt, 0);
	
	//Close GPT0
	GPT_close(myhGpt);
	
	//Clear GPT0 interrupt
	clearInterrupts(myhGpt, 0);
	
	//PASS/FAIL status
	printf("\nTEST %s \n", 
		((success12 == TRUE) ? "PASSED" : "FAILED"));
	
	//Restore global interrupt mask
	IRQ_globalRestore(oldIntm);
}

//Function to configure and use GPT, check if it is running and generating
//  interrupt
Uint16 taskFxn(GPT_Handle hGpt, Uint16 timerIdx)
{
	Uint16 success = TRUE;
	
	int i;
	Uint32 tim12ad, tim34ad, tim12bd, tim34bd;
	Uint32 *tim12a = (Uint32 *)&tim12ad;
	Uint32 *tim34a = (Uint32 *)&tim34ad;
	Uint32 *tim12b = (Uint32 *)&tim12bd;
	Uint32 *tim34b = (Uint32 *)&tim34bd;
	
	//Load GPTx counter (Start counting closer to PRDx)
	GPT_RSETH(hGpt,GPTCNT1, 0x0000);
	GPT_RSETH(hGpt,GPTCNT2, 0xFFFE);
	GPT_RSETH(hGpt,GPTCNT3, 0xFFFF);
	GPT_RSETH(hGpt,GPTCNT4, 0xFFFF);
	
	GPT_config(hGpt, &myGptCfg);
	GPT_start(hGpt);
	
	//Take a snapshot of the count
	GPT_getCnt(hGpt, tim34a, tim12a);
	
	//Delay
	for(i=0; i < 0x2; i++);

	//Take another snapshot of the count
	GPT_getCnt(hGpt, tim34b, tim12b);
	
	//Verify GPTx is running (compare snapshots)
	if(*tim12a == *tim12b)	success = FALSE;
	
	while(hitIsr12 == FALSE);

	//Check Interrupt Flag
	if(hitIsr12 != TRUE)	success = FALSE;
	
	//Stop GPTx
	GPT_stop(hGpt);
	return(success);
}

//GPT0 ISR
interrupt void gpt0Isr(void)
{
	hitIsr12 = TRUE;
    IRQ_clear(gpt12Evt_Id);
	return;	
}

//Function to set up interrupts for the opened GPT
void setupInterrupts(GPT_Handle hGpt, Uint16 timerIdx)
{
	Uint16 evt_Id;
	
	evt_Id = gpt12Evt_Id;
			
	//------Setup GPTx ISR------
	IRQ_clear(evt_Id);
	
	IRQ_plug(evt_Id, &gpt0Isr);

    IRQ_enable(evt_Id);
	
	//Enable interrupts globally
	IRQ_globalEnable();	
}

//Function to clear interrupts for GPTx after its usage is over
void clearInterrupts(GPT_Handle hGpt, Uint16 timerIdx)
{
	Uint16 evt_Id;
	
	evt_Id = gpt12Evt_Id;
		
	IRQ_clear(evt_Id);
	
    IRQ_disable(evt_Id);
}	
