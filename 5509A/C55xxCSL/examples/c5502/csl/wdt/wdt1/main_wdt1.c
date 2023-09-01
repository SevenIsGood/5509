/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_wdt1.c---------
 * This example configures watchdog timer to time-out at certain period.
 * On time-out of WDTIM, it jumps to an ISR that flags the time-out and
 * clears the interrupt.
 */

#include <stdio.h>

#include <csl.h>
#include <csl_wdtim.h>
#include <csl_irq.h>

//---------Global constants---------
#define PERIOD			0x8000

//---------Global data definition---------

Uint16 success = TRUE;
Uint16 oldIntm, wdtEvt_Id;
volatile Uint16 hitIsr = FALSE;

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
	PERIOD,	//PRD1
	0x0000,	//PRD2
	0x0000,	//PRD3
	0x0000,	//PRD4
	WDTIM_WDTCTL1_RMK(
		WDTIM_WDTCTL1_TIEN_DEFAULT,
		WDTIM_WDTCTL1_CLKSRC_DEFAULT,
		WDTIM_WDTCTL1_ENAMODE_ONCE,
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

// Reference the start of the interrupt vector table
// This symbol is defined in file vectors_wdt1.s55
extern Uint32 vecs;

//---------Function prototypes---------
interrupt void wdtIsr(void);
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
	
	//Setup ISR for WDTIM		
	setupInterrupts();	
	
	//Configure WDTIM and verfiy that it is running and generating interrupt
	success &= taskFxn(myhWdt);
	
	//Close WDTIM
	WDTIM_close(myhWdt);
	
	//Clear WDTIM interrupt
	clearInterrupts();
	
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
	Uint32 i, loopCnt;
	
	//Configure WDTIM	
	WDTIM_config(hWdt, &myWdtCfg);
	
	//Load WDTIMx counter (Start counting closer to PRDx)
	WDTIM_RSETH(hWdt,WDTCNT1, 0x0000);
	WDTIM_RSETH(hWdt,WDTCNT2, 0x0000);
	WDTIM_RSETH(hWdt,WDTCNT3, 0x0000);
	WDTIM_RSETH(hWdt,WDTCNT4, 0x0000);
	
	//Remove WDTIM : TIM12, TIM34 from Reset
	WDTIM_start(hWdt);		
	
	//Start WDTIM
	WDTIM_wdStart(hWdt);
	
	//Wait for Watchdog to Time Out (Without Servicing)
	//Assume that the loop overhead is 5 cycles
	loopCnt = PERIOD/5;
	
	for (i = 0; hitIsr != TRUE; i++)
	{
		if (i >= loopCnt)
		{
			success = FALSE;
			break;
		}
	}
		
	//Reset GPTx Interrupt Flag
	if(hitIsr != TRUE)	success = FALSE;
	
	//Stop Watchdog Timer
	WDTIM_FSET(WDTCTL1, ENAMODE, WDTIM_WDTCTL1_ENAMODE_DISABLED);
	WDTIM_stop(hWdt);
	
	return success;
}

//WDTIM ISR
interrupt void wdtIsr(void)
{
	hitIsr = TRUE;
    IRQ_clear(wdtEvt_Id);
	return;	
}

//Function to set up interrupts for the opened WDTIM
void setupInterrupts(void)
{
	//------Setup WDTIM ISR------
	IRQ_clear(wdtEvt_Id);
	
	IRQ_plug(wdtEvt_Id, &wdtIsr);
	IRQ_enable(wdtEvt_Id);
	
	//Enable interrupts globally
	IRQ_globalEnable();	
}

//Function to clear interrupts for WDTIM after its usage is over
void clearInterrupts(void)
{
	IRQ_clear(wdtEvt_Id);
    IRQ_disable(wdtEvt_Id);
}	
