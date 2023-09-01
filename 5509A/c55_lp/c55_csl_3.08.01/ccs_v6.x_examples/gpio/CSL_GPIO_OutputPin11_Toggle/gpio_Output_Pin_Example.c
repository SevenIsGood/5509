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


/** @file gpio_Output_Pin_Example.c
 *
 *  @brief Test code to measure the bootloading performance
 *
 * \page    page3      GPIO EXAMPLE DOCUMENTATION
 *
 * \section GPIO3   GPIO EXAMPLE3 GPIO PIN11 TOGGLE (BOOTLOADING PERFORMANCE TEST)
 *
 * \subsection GPIO3x    TEST DESCRIPTION:
 *		This example code tests the GPIO(General Purpose Input Output) pin
 * functionality as output pin. This example also verifies configuring the
 * CSL GPIO module. The intention of this example is to test boot performance
 * by toggling GPIO.
 * GPIO 11 is chosen based on debug facility available in most of the C55xx
 * hardware platform.
 * To vary the bin file size chose the appropriate pre-directives "file size".
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
 * C5545 AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED
 * IN THE FILE c55xx_csl\inc\csl_general.h.
 *
 * \subsection GPIO3y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM)
 *  @li Open the project "CSL_GPIO_OutputPin11_Toggle.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *
 * \subsection GPIO3z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Value written to the GPIO pin configured as output pin(Pin11) should be
 *      read successfully
 *  @li GPIO11 toggles.
 *
 *  The following command is used for generating the bin file.
 *
 *  C:<>hex55 -boot -v5505 -serial8 -reg_config 0x3000,0x0003 -b -o
 *           gpio_Output_Pin_Example.bin gpio_Output_Pin_Example.out
 *
 *  Translating to Binary format...
 *
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ================
 * 26-Oct-2015 - Created
 * 09-Mar-2016 Update the header
 * ============================================================================
 */

#include "csl_gpio.h"
#include <stdio.h>
#include <csl_general.h>
#include "csl_sysctrl.h"

#define EN_FILESZ_26KB 0x1
//#define EN_FILESZ_80KB 0x1
//#define EN_FILESZ_100KB 0x1
//#define EN_FILESZ_150KB 0x1
//#define EN_FILESZ_170KB 0x1

#ifdef EN_FILESZ_26KB
#include "dummy_table_1.h"	// Change the dummy table to increase the size of the .out file (check size in .map file)
#elif EN_FILESZ_80KB
#include "dummy_table_1.h"
#include "dummy_table_2.h"
#elif EN_FILESZ_100KB
#include "dummy_table_1.h"
#include "dummy_table_2.h"
#include "dummy_table_3.h"		// Change the dummy table to increase the size of the .out file (check size in .map file)
#elif EN_FILESZ_150KB
#include "dummy_table_1.h"
#include "dummy_table_2.h"
#include "dummy_table_3.h"
#include "dummy_table_4.h"		// Change the dummy table to increase the size of the .out file (check size in .map file)
#elif EN_FILESZ_170KB
#include "dummy_table_1.h"
#include "dummy_table_2.h"
#include "dummy_table_3.h"
#include "dummy_table_4.h"
#include "dummy_table_5.h"		// Change the dummy table to increase the size of the .out file (check size in .map file)
#endif

#define CSL_TEST_FAILED  (-1)
#define CSL_TEST_PASSED  (0)

CSL_GpioObj    gpioObj;
CSL_GpioObj    *hGpio;

/* ---------Function prototypes--------- */
/**
 *  \brief  Function to test the functionality of GPIO as output pin
 *
 *  This function configures GPIO pin 0 as output pin and writes a value.
 *  Written value is verified and result is returned to the main function
 *
 *  \param  none
 *
 *  \return Test result
 */
int  gpio_output_pin_test(void);

/**
 *  \brief  Tests the configuration of the CSL GPIO module
 *
 *  This function configures the CSL GPIO module using GPIO_config API.
 *  Configured values are verified by reading back using the CSL function
 *  GPIO_getConfig.
 *
 *  \param  none
 *
 *  \return Test result
 */
//int gpio_pin_config_test(void);


/**
 *  \brief  main function
 *
 *   This function calls GPIO test functions and displays the test
 *   result.
 *
 *  \param  none
 *
 *  \return none
 */

void main(void)
{

	int result;
	#ifdef EN_FILESZ_26KB
		int a;
		a = dummy_table1[0];	        // Need this line to increase .out size by increasing dummy_table.h size
	#elif EN_FILESZ_80KB
		int a,b;
		a = dummy_table1[0];	        // Need this line to increase .out size by increasing dummy_table.h size
		b = dummy_table2[0];
	#elif EN_FILESZ_100KB
		int a,b,c;
		a = dummy_table1[0];	        // Need this line to increase .out size by increasing dummy_table.h size
		b = dummy_table2[0];
		c = dummy_table3[0];
	#elif EN_FILESZ_150KB
		int a,b,c,d;
		a = dummy_table1[0];	        // Need this line to increase .out size by increasing dummy_table.h size
		b = dummy_table2[0];
		c = dummy_table3[0];
		d = dummy_table4[0];
	#elif EN_FILESZ_170KB
		int a,b,c,d,e;
		a = dummy_table1[0];	        // Need this line to increase .out size by increasing dummy_table.h size
		b = dummy_table2[0];
		c = dummy_table3[0];
		d = dummy_table4[0];
		e = dummy_table5[0];
	#endif

    //To test GPIO channel (pin) as an output channel
    result = gpio_output_pin_test();
    if(CSL_TEST_PASSED == result)
    {
        printf("GPIO Output Pin Test Passed!!\n");
    }
    else
	{
		printf("GPIO Output Pin Test Failed!!\n");
	}

}

/**
 *  \brief  Function to test the functionality of GPIO as output pin
 *
 *  This function configures GPIO pin 11 as output pin and writes a value
 *  to Toggle GPIO 11
 *
 *  \return Test result
 */
int  gpio_output_pin_test(void)
{
    CSL_Status           status;
    CSL_GpioPinConfig    config;
    Uint16               delay;
    Uint16               i;

    delay = 10;

     /* Pin Muxing gor GPIO Pins */
    status = SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,
                         CSL_EBSR_SP1MODE_2);
    // commented this part of the code as bin file size will
    // be not less than 20KB
    //if(CSL_SOK != status)
    //{
    //   printf("SYS_setEBSR failed\n");
    //   return(CSL_TEST_FAILED);
    //}

    /* Open GPIO module */
    hGpio = GPIO_open(&gpioObj, &status);

    /* Reset all the pins */
    GPIO_reset(hGpio);

    /* Configure GPIO pin 11 as output pin */
    config.pinNum    = CSL_GPIO_PIN11;
    config.direction = CSL_GPIO_DIR_OUTPUT;
    config.trigger   = CSL_GPIO_TRIG_RISING_EDGE;

    status = GPIO_configBit(hGpio, &config);

    while(1)
    {
    	/* Write 1 to output pin */
    	for (i=0;i<delay;i++)
    	GPIO_write(hGpio, CSL_GPIO_PIN11,0);
    	for (i=0;i<delay;i++)
    	GPIO_write(hGpio, CSL_GPIO_PIN11,1);

    }
 }
