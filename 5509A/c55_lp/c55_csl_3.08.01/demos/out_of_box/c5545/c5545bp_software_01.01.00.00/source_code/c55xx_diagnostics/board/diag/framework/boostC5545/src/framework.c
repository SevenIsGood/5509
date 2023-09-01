/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*! \file framework.c
*
*   \brief Framework implementation for integrating the diagnostic tests.
*   Diagnostic tests are invoked from this file based on user input.
*
*/ 
 
#include "platform_test.h"
#include "platform_internals.h"

extern diagTestTable_t gDiagTestTable[PLATFORM_TEST_COUNT];

/**
 *  \brief    Displays diagnostic test menu
 *
 *  \return   - None
 */
static void displayMenu(void)
{
	Uint16 count;
	C55x_msgWrite("\n\n\r");
	C55x_msgWrite("*******************************************************************\n\r");
	C55x_msgWrite("               C5545 BoosterPack HW Diagnostic Tests                \n\r");
	C55x_msgWrite("*******************************************************************\n\r");
	C55x_msgWrite("\n\nDiagnostic Tests       ");
	C55x_msgWrite("\t\t\t Pass");
	C55x_msgWrite("\t\t Fail\n\r");
	C55x_msgWrite("-----------------------\t\t\t------\t\t------\n\r");

	for (count = 0; count < PLATFORM_TEST_COUNT; count++)
	{

		C55x_msgWrite(" %2d - ",count);
		C55x_msgWrite("%-30s", (const char*)gDiagTestTable[count].testName);
		C55x_msgWrite("\t%3d", gDiagTestTable[count].pass);
		C55x_msgWrite("\t\t%3d", gDiagTestTable[count].fail);
		C55x_msgWrite("\n\r");
	}
	C55x_msgWrite("\n\r");
	C55x_msgWrite("  q - Quit\n\r");
	C55x_msgWrite("\n\rEnter Desired Option:");
}

/**
 *  \brief    Executes all the diagnostic tests
 *
 *  \return   None
 */
static void run_all_tests(void)
{
	Uint16 index;

	C55x_msgWrite("\n\n\rAuto Running All the Diagnostic Tests!\n\n\r");

	for (index = PLATFORM_TEST_START; index <= PLATFORM_AUTO_TEST_COUNT; index++)
	{
		if(gDiagTestTable[index].testFunction != NULL)
		{
			if(gDiagTestTable[index].testFunction(gDiagTestTable[index].args))
			{
				gDiagTestTable[index].fail++;
			}
			else
			{
				gDiagTestTable[index].pass++;
			}
		}
		else
		{
			C55x_msgWrite("\n\nInvalid Test Handle for %s\n",
			               (const char*)gDiagTestTable[index].testName);
		}
	}

	C55x_msgWrite("\nDiagnostic Tests Completed!!\n\n");
}

/**
 *  \brief    Diagnostic test main function
 *
 *  \return   Platform_STATUS
 */
Platform_STATUS diag_main (void)
{
	Platform_STATUS status;
	Uint8       exitTest;
	Uint8         inputErr;
	Uint32        diagInput;
	Uint8 		  testInput[3];

	/* Initialize the platform */
	status = initPlatform();
	if(status == Platform_EOK)
	{
		exitTest = 0;
		inputErr = 0;

		/*Reading the input from the user*/

		while(!exitTest)
		{
			/* Display menu */
			displayMenu();

			testInput[0] = '\0';
			testInput[1] = '\0';
			testInput[2] = '\0';

			status = C55x_msgRead(testInput, 3);
			if(status == CSL_SOK)
			{
				/* Check user input */
				if((testInput[0] == 'q') || (testInput[0] == 'Q'))
				{
					exitTest = 1;
					C55x_msgWrite("\n\nExiting the Diagnostic Tests!!\n\r");
					status = 0;
					break;
				}
				if((testInput[0] == 0xD) || (testInput[0] == 0x1B))
				{
					continue;
				}
				else
				{
					C55x_msgWrite("\n\r");
					diagInput = strtoul((const char*)testInput, NULL, 10);
					switch(diagInput)
					{
						case PLATFORM_TEST_AUTO_ALL:
							run_all_tests();
							break;
						default:
							if(diagInput < PLATFORM_TEST_COUNT)
							{
								if(gDiagTestTable[diagInput].testFunction != NULL)
								{
									if(gDiagTestTable[diagInput].testFunction(gDiagTestTable[diagInput].args))
									{
										gDiagTestTable[diagInput].fail++;
									}
									else
									{
										gDiagTestTable[diagInput].pass++;
									}
								}
								else
								{
									C55x_msgWrite("\n\nInvalid Test Handle!!\n\r");
								}
							}
							else
							{
								inputErr = 1;
							}

							if(inputErr)
							{
								inputErr = 0;
								/* Invalid input. Get the desired option again and continue */
								C55x_msgWrite("\n\n\rInvalid Input!\n\r");
								C55x_msgWrite("Please Enter Valid Input\n\r");
								continue;
							}

							C55x_msgWrite("\n\r");
							break;

					}
				}
			}
			else
			{
				C55x_msgWrite("Platform Initialization Failed!\n");
			}
		}
	}

	return(CSL_SOK);

}
