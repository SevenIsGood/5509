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
 *  MSP432 I2C - EUSCI_B1_BASE I2C Master TX multiple bytes to MSP432 Slave
 *
 *  Description: This demo connects two MSP432 's via the I2C bus. The master
 *  transmits to the slave. This is the MASTER CODE. It continuously
 *  transmits an array of data and demonstrates how to implement an I2C
 *  master transmitter sending multiple bytes using the USCI_B0 TX interrupt.
 *  ACLK = n/a, MCLK = HSMCLK = SMCLK = BRCLK = default DCO = ~3.0MHz
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

#include "i2c_msp_master_dsp_slave_test.h"
/* Slave Address for I2C Slave */
#define SLAVE_ADDRESS 0x38

/* Statics */
static uint8_t TXData;
volatile uint16_t TXByteCtr;
static uint16_t total_bytes_transmitting = 16;
#define I2C_MST_DATA_BLOCK_SIZE    (8u)
#define I2C_MST_DATA_BLOCK_COUNT   (32u)

#define I2C_MST_DATA_SIZE          ((I2C_MST_DATA_BLOCK_SIZE) * (I2C_MST_DATA_BLOCK_COUNT))

/* I2C Master Configuration Parameter */
const eUSCI_I2C_MasterConfig i2cConfig =
{
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        3000000,                                // SMCLK = 3MHz
        EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 400khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

/**
 * \brief This function transfers 256 bytes of the data from MSP432 to DSP
 *        through i2c
 *
 * \param testArgs  - Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
static TEST_STATUS run_i2c_msp432_master_dsp_slave_test(void *testArgs)
{
    volatile uint32_t ii;
    uint8_t ch;
    volatile uint32_t delay;

    /* Disabling the Watchdog */
    MAP_WDT_A_holdTimer();

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

    platform_write("Press Any Key to Continue After Running DSP Program\n\r");
    platform_read(&ch);

    /* Select Port 1 for I2C - Set Pin 6, 7 to input Primary Module Function,
     *   (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
     */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
            GPIO_PIN4 + GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Initializing I2C Master to SMCLK at 400kbs with no autostop */
    MAP_I2C_initMaster(EUSCI_B1_BASE, &i2cConfig);

    /* Specify slave address */
    MAP_I2C_setSlaveAddress(EUSCI_B1_BASE, SLAVE_ADDRESS);

    /* Set Master in receive mode */
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    /* Enable I2C Module to start operations */
    MAP_I2C_enableModule(EUSCI_B1_BASE);

    /* Enable and clear the interrupt flag */
    MAP_I2C_clearInterruptFlag(EUSCI_B1_BASE,
            EUSCI_B_I2C_TRANSMIT_INTERRUPT0 + EUSCI_B_I2C_NAK_INTERRUPT);
    //Enable master Receive interrupt
    MAP_I2C_enableInterrupt(EUSCI_B1_BASE,
            EUSCI_B_I2C_TRANSMIT_INTERRUPT0 + EUSCI_B_I2C_NAK_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIB1);

    /* Load Byte Counter */
    TXByteCtr = total_bytes_transmitting;
    TXData = 0;

    platform_write("Sending %d Bytes to Slave\n\r", total_bytes_transmitting);
    /* Output the bytes that will be sent to slave on the console */
    while(total_bytes_transmitting)
    {
    	platform_write("0x%x\t", TXData);
    	TXData++;

    	total_bytes_transmitting--;

    	if((total_bytes_transmitting%8) == 0)
    	{
    		platform_write("\n\r");
    	}
    }

    TXData = 0;

    /* Making sure the last transaction has been completely sent out */
    while (MAP_I2C_masterIsStopSent(EUSCI_B1_BASE) == EUSCI_B_I2C_SENDING_STOP);

    /* Sending the initial start condition */
    MAP_I2C_masterSendMultiByteStart(EUSCI_B1_BASE, TXData++);

    /* Wait till all the bytes are transferred */
    while (TXByteCtr);

    /* Disable the I2C interrupts before exiting the test */
    MAP_I2C_disableInterrupt(EUSCI_B1_BASE,
            EUSCI_B_I2C_TRANSMIT_INTERRUPT0 + EUSCI_B_I2C_NAK_INTERRUPT);
    MAP_Interrupt_disableInterrupt(INT_EUSCIB1);

    return (TEST_PASS);

}

#ifndef MSP_GPIO_INDIVIDUAL_TEST
#ifdef MSP_INTEGRARTION
/*******************************************************************************
 * The USCIAB0TX_ISR is structured such that it can be used to transmit any
 * number of bytes by pre-loading TXByteCtr with the byte count. Also, TXData
 * points to the next byte to transmit.
 ******************************************************************************/
void EUSCIB0_IRQHandle(void)
{
    uint_fast16_t status;

    status = MAP_I2C_getEnabledInterruptStatus(EUSCI_B1_BASE);
    MAP_I2C_clearInterruptFlag(EUSCI_B1_BASE, status);

    if (status & EUSCI_B_I2C_NAK_INTERRUPT)
    {
        MAP_I2C_masterSendStart(EUSCI_B1_BASE);
    }

    if (status & EUSCI_B_I2C_TRANSMIT_INTERRUPT0)
    {
        /* Check the byte counter */
        if (TXByteCtr)
        {
        	/* Send the next data and decrement the byte counter */
            MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, TXData++);
            TXByteCtr--;
        }
        else
        {
            MAP_I2C_masterSendMultiByteStop(EUSCI_B1_BASE);
            MAP_Interrupt_disableSleepOnIsrExit();
        }
    }
}
#else
/*******************************************************************************
 * The USCIAB0TX_ISR is structured such that it can be used to transmit any
 * number of bytes by pre-loading TXByteCtr with the byte count. Also, TXData
 * points to the next byte to transmit.
 ******************************************************************************/
void EUSCIB0_IRQHandle(void)
{
    uint_fast16_t status;

    status = MAP_I2C_getEnabledInterruptStatus(EUSCI_B1_BASE);
    MAP_I2C_clearInterruptFlag(EUSCI_B1_BASE, status);

    if (status & EUSCI_B_I2C_NAK_INTERRUPT)
    {
        MAP_I2C_masterSendStart(EUSCI_B1_BASE);
    }

    if (status & EUSCI_B_I2C_TRANSMIT_INTERRUPT0)
    {
        /* Check the byte counter */
        if (TXByteCtr)
        {
        	/* Send the next data and decrement the byte counter */
            MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, TXData++);
            TXByteCtr--;
        }
        else
        {
            MAP_I2C_masterSendMultiByteStop(EUSCI_B1_BASE);
            MAP_Interrupt_disableSleepOnIsrExit();
        }
    }
}
#endif
#endif

/**
 * \brief This function performs i2c Msp432 master to dsp slave test
 *
 * \param testArgs  - Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS i2cMsp432MasterDspSlaveTest(void *testArgs)
{
    TEST_STATUS testStatus;

	platform_write("\n*******************************************************\n\r");
	platform_write(  "         'MSP Master - DSP Slave' I2C Test       \n\r");
	platform_write(  "*******************************************************\n\r");

    testStatus = run_i2c_msp432_master_dsp_slave_test(testArgs);
    if(testStatus != TEST_PASS)
    {
    	platform_write("\n\r'MSP Master - DSP Slave' I2C Test Failed!\n\r");
    }
    else
    {
    	//platform_write("\n\n\r'MSP Master - DSP Slave' I2C Test Passed!\n\r");
    }

    platform_write("\n\n\r'MSP Master - DSP Slave' I2C Test Completed!!\n\r");

    return testStatus;
}

