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


/** @file McBSP_Master_AIC3204_48kbps_POLL.c
 *
 *  @brief Test code to verify the interaction between McBSP and the audio codec
 *  in Polled Mode
 *
 * \page    page9  McBSP EXAMPLE DOCUMENTATION
 *
 * \section McBSP2   McBSP EXAMPLE2 - Audio polling mode
 *
 * \subsection McBSP2x    TEST DESCRIPTION:
 * This test verifies the operation of the C5517 McBSP (Multichannel Buffered
 * Serial Port) module in polling mode along with the AIC3204.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSION C5517.
 * MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection McBSP2y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5517 EVM)
 *  @li Open the project "CSL_McBSP_Master_AIC3204_48kbps_POLL.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Connect the audio source to J9 (stereo in 1)
 *  @li Connect the headphones to J4 (headphone)
 *  @li Run the program, start playing a song, and observe the test result
 *  @li Repeat the test at PLL frequencies 60, 100, 150MHz and 200 MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection McBSP2z    TEST RESULT:
 *  @li The song played on the audio source should play clearly through the EVM
 *
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ================
 * 10-Jul-2012 Added C5517 Documentation
 * 09-Mar-2016 Update the header
 * ============================================================================
 */

#include "csl_general.h"
#include "stdio.h"
//#include "evm5515.h"
#include "tistdtypes.h"
#include <log.h>
#include <tsk.h>
#include <csl_mcbsp.h>

#define CSL_MCBSP_BUF_SIZE (1u)

extern CSL_McbspHandle  hMcbsp;

/** Buffer to store the Audio data that is read from the Input port of the
    Audio Codec */
Uint32 gmcbspReadBuf[CSL_MCBSP_BUF_SIZE]={0};

/** Semaphore used to indicate that the Data is read from the input port of the
    codec */
extern SEM_Obj readInputData;
/** Semaphore used to indicate that the Data is sent to the output port of the
    codec */
extern SEM_Obj sendOutputData;

/** Global Variable to indicate that the McBSP is configured successfully, and
    the transfer of the audio samples can be started to/from the codec */
volatile Int16 startReceiving = 0;

extern void csl_c5517evm_u44(void);
extern Int16 aic3204_test( );

Int16 EVM5515_init( );

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Testing Function                                                        *
 *                                                                          *
 * ------------------------------------------------------------------------ */
/////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
/////  Define PaSs_StAtE variable for catching errors as program executes.
/////  Define PaSs flag for holding final pass/fail result at program completion.
volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
/////                                  program flow reaches expected exit point(s).
/////
void TEST_execute( Int16 ( *funchandle )( ), char *testname, Int16 testid )
{
    Int16 status;

    /* Display test ID */
    printf( "%02d  Testing %s...\n", testid, testname );

    /* Call test function */
    status = funchandle( );

    /* Check for test fail */
    if ( status != 0 )
    {
        /* Print error message */
        printf( "     FAIL... error code %d... quitting\n", status );

    	PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
    }
    else
    {
        /* Print error message */
        printf( "%s Test PASSED\n",testname );
    }
    /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
    /////  At program exit, copy "PaSs_StAtE" into "PaSs".
    PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
    /////                   // pass/fail value determined during program execution.
    /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
    /////   control of a host PC script, will read and record the PaSs' value.
    /////
}

extern Int16 aic3204_test( );

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  main( )                                                                 *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void main( void )
{
    /* Initialize BSL */
#if (defined(CHIP_C5517))
    EVM5515_init( );
    printf( "\n***McBSP Codec Test***\n\n" );
    TEST_execute( aic3204_test, "AIC3204", 1 );

    printf( "\n***ALL Tests Passed***\n" );
#else
	printf( "\n***Test not supported by your chip selection***\n" );
#endif
}

/**
 * \brief Task to start reading Audio Data samples from the Input port of the
 * audio codec
 *
 * \param NONE
 *
 * \return NONE
 */
void startReceivingData()
{
	Uint32 data;

#if (defined(CHIP_C5517))
	while (startReceiving)
	{
		SEM_pend(&readInputData, SYS_FOREVER);

		while ((hMcbsp->Regs->SPCRL & 0x0002) == 0) ;

		data = (((Uint32)hMcbsp->Regs->DRRU) << 16);
		data |= hMcbsp->Regs->DRRL;

		gmcbspReadBuf[0] = data;

		SEM_post(&sendOutputData);
	}
#endif  /* #if (defined(CHIP_C5517)) */
}

/**
 * \brief Task to start sending Audio Data samples to the Output port of the
 * audio codec
 *
 * \param NONE
 *
 * \return NONE
 */
void startSendingData()
{
#if (defined(CHIP_C5517))
	while(1)
	{
		SEM_post(&readInputData);

		SEM_pend(&sendOutputData, SYS_FOREVER);

		while ((hMcbsp->Regs->SPCRU & 0x0002) == 0) ;

		hMcbsp->Regs->DXRL =  (gmcbspReadBuf[0] & 0xFFFF);
		hMcbsp->Regs->DXRU = ((gmcbspReadBuf[0] >> 16) & 0xFFFF);
	}
#endif  /* #if (defined(CHIP_C5517)) */
}
