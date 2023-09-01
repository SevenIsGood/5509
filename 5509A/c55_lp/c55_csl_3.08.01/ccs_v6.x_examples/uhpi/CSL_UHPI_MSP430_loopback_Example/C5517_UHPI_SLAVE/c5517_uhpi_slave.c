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
 * \section UHPI3   UHPI EXAMPLE3 - UHPI MSP430 HOST LOOPBACK
 *
 * \subsection UHPI3x    TEST DESCRIPTION:
 *        This test configures C5517 UHPI as slave and MSP430 host as the master.
 * The host sends data to the UHPI interface of C5517 and the same is collected
 * and looped back to the host by the slave UHPI interface.
 * Data integrity is confirmed by checking the SARAM locations being accessed by
 * the host and the read buffer back at the host end.
 *
 * \subsection UHPI3y    TEST PROCEDURE AND EXPECTED RESULT:
 *
 * @filename : MSP_Communication
 *
 * Run C5517 first
 *
 * C5517:
 * STEP 1: Open and Config UHPI peripheral controller
 * STEP 2: Inititalize SARAM with data 0xAAAA 0x5555 from 0x8000 location of SARAM.
 * STEP 3: Wait for DSP_INT(Host to DSP interrupt) to occur
 * STEP 4: If interrupt is received, exit from loop
 * STEP 5: Close UHPI PORT
 *
 * MSP430:
 * STEP 1: Set up the clock and wait till CLOCK is stabilized.
 * STEP 2: Acess HPIA Register in write mode and Set some adrress value.
 * STEP 3: Acess HPIA Register in Read mode.
 * STEP 4: Check if Written and READ values are correct.
*/

#include <stdio.h>
#include "csl_uhpi.h"
#include "csl_uhpiAux.h"
#include "csl_intc.h"
#include "csl_general.h"

/* Update the UHPI configuration structure */
CSL_UhpiSetup UhpiSetup= {
  	/*  UHPI Operating mode(only this mode is possible)	                     */
	CSL_UHPI_16BIT_MUXED_SINGLE_FULLWORD_CYCLES,
	/*  UHPI GPIO interrupt mode - disable			        		         */
	CSL_UHPI_GPIO_INTERRUPT_DISABLE
};     

CSL_UhpiHandle  hUhpi;
CSL_UhpiObj 	Uhpiobj;

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

 char status; //This variable is for the DV

 Uint32 tx_data[1024],value=0;
 Uint16 i;
 Uint32 rd_data[1024];


	/* Enable clocks to all peripherals */
	//EVM5517_init( );

	CSL_SYSCTRL_REGS->PCGCR2 &= 0xFFFE;//just uhpi
	//Desassert clk stop request to uhpi
	CSL_SYSCTRL_REGS->CLKSTOP1 &= 0xF3FF;
	  // Wait until the clk stop ack is deasserted
	while ((CSL_SYSCTRL_REGS->CLKSTOP1 & 0x0800) != (Uint16) 0x0000 )  {
	     __asm("    NOP");
	     }

	CSL_SYSCTRL_REGS->EBSR &= 0x8FFF;

 i         = 0;
 status    = 'F';
 //saram_array = (Uint32 *) 0x00027000;
 saram_array = (Uint32 *) 0x00008000;

 printf("\r\nUHPI test for Data Transfer\r\n");

 for( i = 0 ; i < 2048 ; i++ )
 {
  saram_array[i] = 0xAAAA5555; 
 }
 for( i = 0 ; i < 64 ; i++ )
 {
  tx_data[i] = (value<<16);
  tx_data[i] |= value+1; 
  
  value+=2;
 }
 
 printf("\r\nSARAM Init Done \n");
  
  
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

  //UHPI_HPIRST_Clear(hUhpi);
//*((volatile ioport unsigned short *)0x1c00) = 0x0000;
//UHPI Config register -- To define UHPI functional mode
*((volatile ioport unsigned short *)0x1c4E) = 0x0000;


  Status = UHPI_Config(hUhpi,&UhpiSetup);
  if( Status != CSL_SOK )
  {
	printf("\r\nUHPI_Config failed\r\n");
  }
  else
  {
   printf("\r\nUHPI_Config success\r\n");
  }

  // Generate the Host interrupt to make the BFM functional
  //UHPI_dspTOhostInterrupt_Generate(hUhpi);
  *((volatile ioport unsigned short *)0x2E30) = 0x0004;
//  for(i=0;i<200;i++);
  /* Check for the CPU interrupt -- End of BFM data transfer to the SARAM  */
  while(!UHPI_hostTOdspInterrupt_Status(hUhpi));

  Status = UHPI_Close(hUhpi);
  if( Status != CSL_SOK )
  {
	printf("\r\nUHPI_Close failed\r\n");
  }
  else
  {
   printf("\r\nUHPI_Close success\r\n");
  }
 /***************************************************************/
// Data Integrity Check
/***************************************************************/

 printf("HPI data \r\n");
 for( i = 0 ; i < 32 ; i++ )
 {
   rd_data[i] = saram_array[i];
   printf("%x\r\n",((unsigned short)rd_data[i]));
 }
 

 status = 'P';
 for (i=0; i<32; i++) 
 {
   if (rd_data[i] != tx_data[i])
   {
       status = 'F';
	   break;
   }
 }

                            
	if( status == 'F' )
	{
	printf("\r\nUHPI test for Data Transfer is failed\r\n");
	PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
	}
	else
	{
	printf("\r\nUHPI test for Data Transfer is Passed\r\n");
	}
   
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

