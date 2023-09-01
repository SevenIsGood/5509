/* ============================================================================
 * Copyright (c) 2008-2016 Texas Instruments Incorporated.
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/** @file csl_uhpi_example.c
 *
 *  @brief UHPI functional layer sample source file
 *
 * \page    page19   UHPI EXAMPLE DOCUMENTATION
 *
 * \section UHPI2   UHPI EXAMPLE2 - UHPI GPIO EXAMPLE
 *
 * \subsection UHPI2x    TEST DESCRIPTION:
 * This test makes use of the GPIO functionality of the UHPI. The UHPI lines are
 * enabled as GPIO using GPIO_ENL register, direction set as output or input depending
 * upon if the test is being used for testing the output gpio functionality or the
 * input using DIR1L and DIR2L and driven using DAT1L and DAT2L.
 *
 * \subsection UHPI2y    TEST PROCEDURE and EXPECTED RESULT:
 * NOTE: HPI_ON switch of SW4 should be in OFF position (HPI_ON signal should be high)
 *       HPI_SEL_IF switch should be in ON position (HPI_SEL_IF signal should be low)
 *
 *
 * Note: ----------------  Testing GPIO OUTPUT Functionality ------------------------
 *
 * The Address/Data Lines direction between MSP430 to Phoenix is controlled by HR/W pin in u64
 * Run C5517 first
 *
 * C5517:
 * STEP 1: Open and Config UHPI peripheral controller
 * STEP 2: Set all pins in UHPI controller to GPIO mode using UHPI GPIO_EN Register
 * STEP 3: Set all pins to output and logic HIGH.
 * STEP 4: Set pins one bye one from HIGH to LOW
 * STEP 5: Close UHPI PORT
 *
 * MSP430:
 * STEP 1: Set up the clock and wait till CLOCK is stabilized.
 * STEP 2: Set all pins to input.
 * STEP 3: Check if Pin state is changed from HIGH to LOW in any of the pins.
 *
 * Validating
 * STEP 1: Load C5517 and put breakpoints on line 96,line 126.
 * STEP 2: Uncomment C5517_UHPI_OUTPUT_TEST and comment C5517_UHPI_INPUT_TEST macro in MSP430 code, HPI_Host.c, C5517 code.
 * STEP 3: Build and load MSP430 code and put breakpoint on line 113.
 * STEP 4: Run code in C5517 till line 96, run code in MSP till line 113 and check if Status_Buffer values
 *         are 0xFF,0xFF,0x33 which indicates all lines are logic HIGH.
 * STEP 5: Run code in C5517 till line 126, run code again in MSP till line 113 and check if Status_Buffer values
 *         are 0x00,0x00,0x00 which indicates all lines are logic LOW.
 *
 *
 * Note: ----------------  Testing GPIO INPUT Functionality ------------------------
 *
 * The Address/Data Lines direction between MSP430 to Phoenix is controlled by HR/W pin in u64
 * Run C5517 first
 *
 * C5517:
 * STEP 1: Open and Config UHPI peripheral controller
 * STEP 2: Set all pins in UHPI controller to GPIO mode using UHPI GPIO_EN Register
 * STEP 3: Set all pins to inout.
 * STEP 4: Read all pins status and display it.
 * STEP 5: Close UHPI PORT
 *
 * MSP430:
 * STEP 1: Set up the clock and wait till CLOCK is stabilized.
 * STEP 2: Set all pins to output.
 * STEP 3: Set pins state to HIGH snd change from HIGH to LOW.
 *
 *
 * Validating:
 * STEP 1: Load C5517 and run code
 * STEP 2: Uncomment C5517_UHPI_INPUT_TEST and comment C5517_UHPI_OUTPUT_TEST macro in MSP430 code, HPI_Host.c and C5517 code.
 * STEP 3: Build and load MSP430 code and put breakpoints on lines 85,89 and 94.
 * STEP 4: Run code in MSP till line 118 and check value displayed in output console.
 * STEP 5: Run code in MSP till line 122 and check value displayed in output console.
 * STEP 6: Run code in MSP till line 127 and check value displayed in output console.
 * STEP 7: The output pin state of MSP pins should reflect in displayed values.
*/


#include <stdio.h>
#include "csl_uhpi.h"
#include "csl_uhpiAux.h"
#include "csl_intc.h"
#include "csl_general.h"

CSL_UhpiSetup UhpiSetup;
CSL_UhpiHandle  hUhpi;
CSL_UhpiObj 	Uhpiobj;

#define C5517_UHPI_OUTPUT_TEST
//#define C5517_UHPI_INPUT_TEST

#define GPIO_EN *((volatile ioport unsigned short *)0x2E0C)

#define GPIO_DIR1 *((volatile ioport unsigned short *)0x2E10)
#define GPIO_DIR2 *((volatile ioport unsigned short *)0x2E18)

#define GPIO_DAT1 *((volatile ioport unsigned short *)0x2E14)
#define GPIO_DAT2 *((volatile ioport unsigned short *)0x2E1C)

volatile Uint32* saram_array;


/////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
/////  Define PaSs_StAtE variable for catching errors as program executes.
/////  Define PaSs flag for holding final pass/fail result at program completion.
volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
/////                                  program flow reaches expected exit point(s).
/////

void main()
{
#if (defined(CHIP_C5517))
 CSL_Status Status; //This is for the CSL status return

 Uint16 i, DATA_Line=0,COTROL_Line=0,flag1=0,flag2=0;

 	CSL_SYSCTRL_REGS->PCGCR2 &= 0xFFFE;//just uhpi
	//Desassert clk stop request to uhpi
	CSL_SYSCTRL_REGS->CLKSTOP1 &= 0xF3FF;
	  // Wait until the clk stop ack is deasserted
	while ((CSL_SYSCTRL_REGS->CLKSTOP1 & 0x0800) != (Uint16) 0x0000 )  {
	     __asm("    NOP");
	     }
	CSL_SYSCTRL_REGS->EBSR &= 0x8FFF;

 printf("\r\nUHPI GPIO Functionality test\r\n");
  
  /* Initialize the UHPI using the CSL*/
   hUhpi = UHPI_open(CSL_UHPI_INST_0,&Uhpiobj,&Status);
  if ( (hUhpi == NULL) || (Status != CSL_SOK) )
  {
   printf("UHPI_open Failed \n");
  }
  else
  {
   printf("\r\nUHPI_Open success\r\n");
  }

   //Setup UHPI Config register -- To define UHPI functional mode  
  UhpiSetup.op_mode   = CSL_UHPI_16BIT_MUXED_SINGLE_FULLWORD_CYCLES;
  UhpiSetup.gpio_intr = CSL_UHPI_GPIO_INTERRUPT_DISABLE;

  Status = UHPI_Config(hUhpi,&UhpiSetup);
  if( Status != CSL_SOK )
  {
	printf("\r\nUHPI_Config failed\r\n");
  }
  else
  {
   printf("\r\nUHPI_Config success\r\n");
  }

  UHPI_hostTOdspInterrupt_Generate(hUhpi); // Clear Pending interrupts    
  UHPI_dspTOhostInterrupt_Generate(hUhpi); // Generate the DSP to Host interrupt to notify Host
  
  for( i = 0 ; i < 100 ; i++ );

  hUhpi->Regs->GPIO_ENL = 0x07FF;

#ifdef C5517_UHPI_OUTPUT_TEST
  hUhpi->Regs->GPIO_DIR1L =0xFFFF;
  hUhpi->Regs->GPIO_DIR2L =0xFFFF;

  hUhpi->Regs->GPIO_DAT1L =0xFFFF;
  hUhpi->Regs->GPIO_DAT2L =0xFFFF;
  
  for(i=0;i<500;i++);

  hUhpi->Regs->GPIO_DAT1L =0x0000;
  hUhpi->Regs->GPIO_DAT2L =0x0000;
#endif
  
#ifdef C5517_UHPI_INPUT_TEST
  for(i=0;i<500;i++);
  GPIO_DIR1 = 0x0000; // Set Direction for All ADDR/Data Lines to Input
  GPIO_DIR2 = 0x0000; // Set Direction for  All Control Lines to Input

  flag1 = DATA_Line;
  flag2 = COTROL_Line;
  while(1)
  {
      DATA_Line   = GPIO_DAT1;
      COTROL_Line = GPIO_DAT2;
      if((DATA_Line!=flag1)||(COTROL_Line!=flag2))
	  {
	   printf("\r\nDATA_PORT = %x\r\n\r\nCONTROL_LINES = %x\r\n",DATA_Line,COTROL_Line);
       flag1=DATA_Line;
	   flag2 = COTROL_Line;
      }

      for(i=0;i<50;i++);
  }
#endif
  
  Status = UHPI_Close(hUhpi);
  if( Status != CSL_SOK )
  {
	printf("\r\nUHPI_Close failed\r\n");
  }
  else
  {
   printf("\r\nUHPI_Close success\r\n");
  }

  printf("\r\nThe UHPI GPIO test Finished\r\n");                          
  PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
  for( i = 0 ; i < 100 ; i++ );
   
  /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
  /////  At program exit, copy "PaSs_StAtE" into "PaSs".
  PaSs =PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
  /////                   // pass/fail value determined during program execution.
  /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
  /////   control of a host PC script, will read and record the PaSs' value.
  /////
#else
  printf("\nEnsure CHIP_C5517 is #defined. C5504/05/14/15 don't have UHPI\n");
#endif
}

