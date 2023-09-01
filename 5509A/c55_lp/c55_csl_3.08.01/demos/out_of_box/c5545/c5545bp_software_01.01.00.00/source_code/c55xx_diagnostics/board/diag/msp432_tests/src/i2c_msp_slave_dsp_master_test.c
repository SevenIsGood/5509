/*
 * -------------------------------------------
 *    MSP432 DriverLib - v3_10_00_09 
 * -------------------------------------------
 *
 * --COPYRIGHT--,BSD,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
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
 * --/COPYRIGHT--*/
/*******************************************************************************
 *  MSP432 I2C - EUSCI_B0_BASE I2C Slave RX multiple bytes from MSP432 Master
 *
 *  Description: This demo connects two MSP432 's via the I2C bus. The master
 *  transmits to the slave. This is the slave code. The interrupt driven
 *  data reception is demonstrated using the USCI_B0 RX interrupt. Data is
 *  stored in the RXData array.
 *
 *                                /|\  /|\
 *                MSP432P401      10k  10k      MSP432P401
 *                   slave         |    |         master
 *             -----------------   |    |   -----------------
 *            |     P1.6/UCB0SDA|<-|----+->|P1.6/UCB0SDA     |
 *            |                 |  |       |                 |
 *            |                 |  |       |                 |
 *            |     P1.7/UCB0SCL|<-+------>|P1.7/UCB0SCL     |
 *            |                 |          |                 |
 *
 * Author: Timothy Logan
 ******************************************************************************/

#include "i2c_msp_slave_dsp_master_test.h"
/* Application Defines */
#define SLAVE_ADDRESS       0x38
#define NUM_OF_RX_BYTES     16
#define RX_TOTAL_BYTES      16

/* Statics */
static volatile uint8_t RXData[RX_TOTAL_BYTES];
volatile uint32_t xferIndex = 0;
volatile bool runLoop = 1;

//#define I2C_PORT_NUM   (EUSCI_B0_BASE)
#define I2C_PORT_NUM   (EUSCI_B1_BASE)
int byteCnt = 0;
int totalBytes = 0;

void testExit(void)
{
    platform_write("\n\rData Received from Master:\n\r");
    for (xferIndex = 0; xferIndex < RX_TOTAL_BYTES;)
    {
    	platform_write("0x%x\t", RXData[xferIndex++]);
		if(!(xferIndex%8))
		{
			platform_write("\n\r");
		}
    }

	/* Compare the data received */
	/* For ease of verification it is assumed that master sends 16 bytes of data
	 * from 0 to 15. Same data should be sent by master test implementation for
	 * successful execution of the test */
    for (xferIndex = 0; xferIndex < RX_TOTAL_BYTES; xferIndex++)
    {
		if(RXData[xferIndex] != xferIndex)
		{
			platform_write("\n\rData Received from Master Mismatched with Expected Data at Offset %d\n\r", xferIndex);
			platform_write("\n'MSP Slave - DSP Master' I2C Test Failed!\n\r");
			exit(1);
		}
    }

    platform_write("\n\rData Received from Master Matched with Expected Data!\n\r");

    platform_write("\n'MSP Slave - DSP Master' I2C Test Passed!\n\r");
    platform_write("\n'MSP Slave - DSP Master' I2C Test Completed!!\n\r");

    exit(0);
}

static TEST_STATUS run_i2c_mspslv_dspmst_test(void *testArgs)
{
	uint8_t ch;
	uint32_t index;
	volatile uint32_t delay;

    /* Disabling the Watchdog */
    MAP_WDT_A_holdTimer();
    xferIndex = 0;

    for (index = 0; index < RX_TOTAL_BYTES; index++)
    {
    	RXData[index] = 0;
    }

    platform_write("MSP432 I2C Slave Mode Test\n\r");
    platform_write("MSP432 is Configured in Slave Mode with Address 0x%x\n\r", SLAVE_ADDRESS);

    // Set P4.1 to output direction
    GPIO_setAsOutputPin(
        GPIO_PORT_P4,
        GPIO_PIN1
        );

    GPIO_setOutputLowOnPin(GPIO_PORT_P4,
            GPIO_PIN1);

    /* Wait some time before releasing DSP reset */
    for (delay = 0; delay < 0x200; delay++);

    GPIO_setOutputHighOnPin(GPIO_PORT_P4,
            GPIO_PIN1);

    platform_write("Press Any Key to Continue After DSP is Out of Reset\n\r");
    platform_read(&ch);

    /* Select Port 1 for I2C - Set Pin 6, 7 to input Primary Module Function,
     *   (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
     */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
            GPIO_PIN4 + GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

    /* eUSCI I2C Slave Configuration */
    MAP_I2C_initSlave(I2C_PORT_NUM, SLAVE_ADDRESS, EUSCI_B_I2C_OWN_ADDRESS_OFFSET0,
            EUSCI_B_I2C_OWN_ADDRESS_ENABLE);

    /* Set in receive mode */
    MAP_I2C_setMode(I2C_PORT_NUM, EUSCI_B_I2C_RECEIVE_MODE);

    /* Enable the module and enable interrupts */
    MAP_I2C_enableModule(I2C_PORT_NUM);
    MAP_I2C_clearInterruptFlag(I2C_PORT_NUM, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
    MAP_I2C_enableInterrupt(I2C_PORT_NUM, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableInterrupt(INT_EUSCIB1);
    MAP_Interrupt_enableMaster();

    platform_write("\nWaiting for Data from Master...\n\r");

    while (1);

    /* Wait for Data from Matesr */
//    while (MAP_Interrupt_isEnabled(INT_EUSCIB1))
//    {
//    	for (delay = 0; delay < 0x200; delay++);
//    }

    return (TEST_PASS);
}

#ifdef MSP_INTEGRARTION
/******************************************************************************
 * The USCI_B0 data ISR RX vector is used to move received data from the I2C
 * master to the MSP432 memory.
 ******************************************************************************/
void EUSCIB0_IRQHandler(void)
{
    uint_fast16_t status;

    status = MAP_I2C_getEnabledInterruptStatus(I2C_PORT_NUM);
    MAP_I2C_clearInterruptFlag(I2C_PORT_NUM, status);

    /* RXIFG */
    if (status & EUSCI_B_I2C_RECEIVE_INTERRUPT0)
    {
        RXData[xferIndex++] = MAP_I2C_slaveGetData(I2C_PORT_NUM);

        /* Resetting the index if we are at the end */
        if (xferIndex == NUM_OF_RX_BYTES)
        {
            xferIndex = 0;
        }

    }
}
#else
/******************************************************************************
 * The USCI_B0 data ISR RX vector is used to move received data from the I2C
 * master to the MSP432 memory.
 ******************************************************************************/
void EUSCIB0_IRQHandle(void)
{
    uint_fast16_t status;
    volatile uint8_t data;

    status = MAP_I2C_getEnabledInterruptStatus(I2C_PORT_NUM);
    MAP_I2C_clearInterruptFlag(I2C_PORT_NUM, status);

    /* RXIFG */
    if (status & EUSCI_B_I2C_RECEIVE_INTERRUPT0)
    {
    	data = MAP_I2C_slaveGetData(I2C_PORT_NUM);
        RXData[xferIndex++] = data;

        if((xferIndex >= RX_TOTAL_BYTES) || (data == 0xf))
        {
            MAP_Interrupt_disableInterrupt(INT_EUSCIB1);
            MAP_Interrupt_disableMaster();
            testExit();
        }
    }
}
#endif


/**
 * \brief This function performs oled display test
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS i2cMspSlaveDspMasterTest(void *testArgs)
{
    int16_t retVal;

	platform_write("\n*************************************************\n\r");
	platform_write(  "        'MSP Slave - DSP Master' I2C Test        \n\r");
	platform_write(  "*************************************************\n\r");

	retVal = run_i2c_mspslv_dspmst_test(testArgs);
	if(retVal != TEST_PASS)
    {
	    platform_write("\n'MSP Slave - DSP Master' I2C Test Failed!\n\r");
	    return (TEST_FAIL);
	}
	else
	{
		platform_write("\n'MSP Slave - DSP Master' I2C Test Passed!\n\r");
	}

	platform_write("\n'MSP Slave - DSP Master' I2C Test Completed!!\n\r");

    return (TEST_PASS);

}
