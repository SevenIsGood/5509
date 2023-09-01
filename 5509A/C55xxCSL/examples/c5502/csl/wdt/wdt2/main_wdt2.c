/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_wdt2.c---------
 * This examples configures GPT to service WDTIM for first few times in GPT ISR.
 * After this WDTIM is not serviced and it times-out.
 */

#include <stdio.h>

#include <csl.h>
#include <csl_wdtim.h>
#include <csl_gpt.h>
#include <csl_irq.h>

//---------Global constants---------
//WDTIM and GPT period, GPT period is slightly lesser than WDTIM period
#define WDTIM_PERIOD			0x01F0
#define GPT_PERIOD				0x00FF

//No of times WDTIM is to be serviced in GPT ISR
#define SERVE_CNT				3

//---------Global data definition---------

Uint16 success = TRUE;
Uint16 oldIntm, wdtEvt_Id, gpt12Evt_Id;
volatile Uint16 hitIsr = FALSE;
volatile Uint16 hitCnt = 0;

WDTIM_Handle myhWdt;

//Config structure for WDTIM
WDTIM_Config myWdtCfg = 
{
	0,
	WDTIM_WDTGPINT_RMK(
		WDTIM_WDTGPINT_TIN1INV_DEFAULT,
		WDTIM_WDTGPINT_TIN1INT_DEFAULT
	),
	WDTIM_WDTGPEN_RMK(
		WDTIM_WDTGPEN_TOUT1EN_DEFAULT,
		WDTIM_WDTGPEN_TIN1EN_DEFAULT	
	),
	WDTIM_WDTGPDIR_RMK(
		WDTIM_WDTGPDIR_TOUT1DIR_DEFAULT,
		WDTIM_WDTGPDIR_TIN1DIR_DEFAULT
	),
	WDTIM_WDTGPDAT_RMK(
		WDTIM_WDTGPDAT_TOUT1DAT_DEFAULT,
		WDTIM_WDTGPDAT_TIN1DAT_DEFAULT
	),
	WDTIM_PERIOD,	//PRD1
	0x0000,	//PRD2
	0x0000,	//PRD3
	0x0000,	//PRD4
	WDTIM_WDTCTL1_RMK(
		WDTIM_WDTCTL1_TIEN_DEFAULT,
		WDTIM_WDTCTL1_CLKSRC_DEFAULT,
		WDTIM_WDTCTL1_ENAMODE_CONTINUOUS,
		WDTIM_WDTCTL1_PWID_DEFAULT,
		WDTIM_WDTCTL1_CP_DEFAULT,
		WDTIM_WDTCTL1_INVIN_DEFAULT,
		WDTIM_WDTCTL1_INVOUT_DEFAULT			
	),
	WDTIM_WDTCTL2_RMK(
		WDTIM_WDTCTL2_TIEN_DEFAULT,
		WDTIM_WDTCTL2_CLKSRC_DEFAULT,
		WDTIM_WDTCTL2_ENAMODE_ONCE,
		WDTIM_WDTCTL2_PWID_DEFAULT,
		WDTIM_WDTCTL2_CP_DEFAULT,
		WDTIM_WDTCTL2_INVIN_DEFAULT,
		WDTIM_WDTCTL2_INVOUT_DEFAULT
	),
	WDTIM_WDTGCTL1_RMK(
		WDTIM_WDTGCTL1_TDDR34_DEFAULT,
		WDTIM_WDTGCTL1_PSC34_DEFAULT,
		WDTIM_WDTGCTL1_TIMMODE_64BIT_WDTIM,
		WDTIM_WDTGCTL1_TIM34RS_IN_RESET,
		WDTIM_WDTGCTL1_TIM12RS_IN_RESET
	),
	WDTIM_WDTWCTL1_RMK(
		WDTIM_WDTWCTL1_WDFLAG_TIMEOUT,
		WDTIM_WDTWCTL1_WDEN_DISABLE,
		WDTIM_WDTWCTL1_WDIKEY_DEFAULT
	),
	WDTIM_WDTWCTL2_RMK(
		WDTIM_WDTWCTL2_WDKEY_DEFAULT
	)
};

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
	GPT_PERIOD,	//PRD1
	0x0000,	//PRD2
	0x0000,	//PRD3
	0x0000,	//PRD4
	GPT_GPTCTL1_RMK(
		GPT_GPTCTL1_TIEN_DEFAULT,
		GPT_GPTCTL1_CLKSRC_DEFAULT,
		GPT_GPTCTL1_ENAMODE_CONTINUOUS,
		GPT_GPTCTL1_PWID_DEFAULT,
		GPT_GPTCTL1_CP_DEFAULT,
		GPT_GPTCTL1_INVIN_DEFAULT,
		GPT_GPTCTL1_INVOUT_DEFAULT			
	),
	GPT_GPTCTL2_RMK(
		GPT_GPTCTL2_TIEN_DEFAULT,
		GPT_GPTCTL2_CLKSRC_DEFAULT,
		GPT_GPTCTL2_ENAMODE_DEFAULT,
		GPT_GPTCTL2_PWID_DEFAULT,
		GPT_GPTCTL2_CP_DEFAULT,
		GPT_GPTCTL2_INVIN_DEFAULT,
		GPT_GPTCTL2_INVOUT_DEFAULT
	),
	GPT_GPTGCTL1_RMK(
		GPT_GPTGCTL1_TDDR34_DEFAULT,
		GPT_GPTGCTL1_PSC34_DEFAULT,
		GPT_GPTGCTL1_TIMMODE_32BIT_DUAL,
		GPT_GPTGCTL1_TIM34RS_IN_RESET,
		GPT_GPTGCTL1_TIM12RS_IN_RESET
	)		
};

// Reference the start of the interrupt vector table
// This symbol is defined in file vectors_wdt2.s55
extern Uint32 vecs;

//---------Function prototypes---------
interrupt void wdtIsr(void);
interrupt void gpt0Isr(void);
Uint16 taskFxn(WDTIM_Handle);
void setupInterrupts(void);
void clearInterrupts(void);

//---------main routine---------
void main(void)
{
	//Initialize CSL
	CSL_init();    

	//Initialize interrupt vectors   
	IRQ_setVecs((Uint32)&vecs);

	//Disable interrupts globally
	oldIntm = IRQ_globalDisable();
	
	//Open WDTIM and verify if it is valid
	myhWdt = WDTIM_open();
	if (myhWdt == (WDTIM_Handle)INV)
		printf("\nERROR: Invalid Handle, WDTIM_open failed\n");

	//Route WDTIM Interrupt to INT3_pin (Required to get WDT Interrupt)
	CHIP_FSET(TSSR, IWCON, 0x3);
	
	//Get WDTIMx EventId
	wdtEvt_Id = WDTIM_getEventId;
	
	//Open GPT0 and verify if it is valid
	myhGpt = GPT_open(GPT_DEV0, GPT_OPEN_RESET);
	if (myhGpt == (GPT_Handle)INV)
		printf("\nERROR: Invalid Handle, GPT_open failed\n");

	//Get GPTx EventId
	gpt12Evt_Id = GPT_getEventId(myhGpt);
	
	//Setup ISR for WDTIM		
	setupInterrupts();	
	
	//Configure WDTIM and verfiy that it is running and generating interrupt
	success &= taskFxn(myhWdt);
	
	//Close WDTIM
	WDTIM_close(myhWdt);
	
	//Close GPT0
	GPT_close(myhGpt);
	
	//Clear WDTIM interrupt
	clearInterrupts();
	
	//Since GPT period is lesser than WDTIM period, hitCnt for GPT ISR
	//  is greater than SERVE_CNT
	if (hitCnt <= SERVE_CNT) success = FALSE;
	
	//PASS/FAIL status
	printf("\nTEST %s \n", 
		((success == TRUE) ? "PASSED" : "FAILED"));
	
	//Restore global interrupt mask
	IRQ_globalRestore(oldIntm);
}

//Function to configure and use WDTIM, check if it is generating
//  time-out interrupt
Uint16 taskFxn(WDTIM_Handle hWdt)
{
	Uint16 success = TRUE;
	
	//Configure WDTIM	
	WDTIM_config(hWdt, &myWdtCfg);
	
	//Load WDTIMx counter
	WDTIM_RSETH(hWdt,WDTCNT1, 0x0000);
	WDTIM_RSETH(hWdt,WDTCNT2, 0x0000);
	WDTIM_RSETH(hWdt,WDTCNT3, 0x0000);
	WDTIM_RSETH(hWdt,WDTCNT4, 0x0000);
	
	//Load GPT0 counter
	GPT_RSETH(myhGpt,GPTCNT1, 0x0000);
	GPT_RSETH(myhGpt,GPTCNT2, 0x0000);
	GPT_config(myhGpt, &myGptCfg);	
	
	//Remove WDTIM : TIM12, TIM34 from Reset
	WDTIM_start(hWdt);		
	
	//Start WDTIM and GPT0
	WDTIM_wdStart(hWdt);	
	GPT_start12(myhGpt);
	
	//Wait for Watchdog to Time Out (Without Servicing)
	while (hitIsr != TRUE);
	
	//Reset WDTIM Interrupt Flag
	if(hitIsr != TRUE)	success = FALSE;
	
	//Stop Watchdog Timer
	WDTIM_FSET(WDTCTL1, ENAMODE, WDTIM_WDTCTL1_ENAMODE_DISABLED);
	WDTIM_stop(hWdt);
	
	//Stop GPT0
	GPT_stop12(myhGpt);

	return success;
}

//WDTIM ISR when it times out
interrupt void wdtIsr(void)
{
	//This ISR is reached after WDTIM is serviced SERVE_CNT times
	hitIsr = TRUE;
    IRQ_clear(wdtEvt_Id);
	return;	
}

//GPT0 ISR
interrupt void gpt0Isr(void)
{
	hitCnt++;
	IRQ_clear(gpt12Evt_Id);
	
	//Service watchdog timer for the first SERVE_CNT times
	if (hitCnt <= SERVE_CNT) WDTIM_service(myhWdt);    
	return;	
}

//Function to set up interrupts for the opened WDTIM
void setupInterrupts(void)
{
	//------Setup WDTIM ISR------
	IRQ_clear(wdtEvt_Id);
	
	IRQ_plug(wdtEvt_Id, &wdtIsr);
	IRQ_enable(wdtEvt_Id);
	
	//------Setup GPT0 ISR------
	IRQ_clear(gpt12Evt_Id);
	
	IRQ_plug(gpt12Evt_Id, &gpt0Isr);
	IRQ_enable(gpt12Evt_Id);
	
	//Enable interrupts globally
	IRQ_globalEnable();
}

//Function to clear interrupts for WDTIM and GPT after their usage is over
void clearInterrupts(void)
{
	IRQ_clear(wdtEvt_Id);
    IRQ_disable(wdtEvt_Id);
    IRQ_clear(gpt12Evt_Id);
    IRQ_disable(gpt12Evt_Id);
    
}	
