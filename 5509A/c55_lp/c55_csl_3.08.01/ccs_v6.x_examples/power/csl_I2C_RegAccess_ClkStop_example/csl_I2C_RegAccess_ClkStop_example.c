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

/** @file csl_I2C_RegAccess_ClkStop_example.c
 *
 *  @brief I2C functional layer interrupt mode example source file
 *
 * \page    page23 POWER EXAMPLE DOCUMENTATION
 *
 * \section POWER1   POWER EXAMPLE1 - I2C Register Access & clock stop TEST
 * \subsection POWER1x   TEST DESCRIPTION:
 *  I2C REG R/W RECOVERY AFTER CLOCK GATING
 *
 *         This test starts reading default after-reset values in the
 * I2C register space and validates them to be as expected by the
 * datasheet. In between the clock to the I2C peripheral is shut off
 * and restarted. The I2C register space r/w check is resumed. The
 * r/w shouldn't be affected by the shutdown-restart activity on the
 * clock to the I2C module.
 *
 * \subsection POWER1y   TEST PROCEDURE:
 *
 * 1. Configure the clock to the I2C module.
 * 2. Start reading the default after-reset values of the I2C registers.
 * 3. Compare them with the expected values as mentioned in the datasheet.
 * 4. Shut down the clock to I2C peripheral by setting the corresponding bit in PCGCR1.
 * 5. After a small delay, restart the clock by clearing the same bit.
 * 6. Resume reg r/w and ensure there are no mismatches.
 *
 * \subsection POWER1z   TEST RESULT:
 *   All the CSL APIs should return success.
 *   Reg r/w shouldn't throw any mismatch.
*/

#include <csl_i2c.h>
#include <stdio.h>
#include <csl_general.h>

#define CSL_I2C_TEST_PASSED      (0)
#define CSL_I2C_TEST_FAILED      (1)

#define CSL_I2C_DATA_SIZE        (64)
#define CSL_EEPROM_ADDR_SIZE     (2)
#define CSL_I2C_OWN_ADDR         (0x2F)
#define CSL_I2C_SYS_CLK          (100)
#define CSL_I2C_BUS_FREQ         (10)
#define CSL_I2C_EEPROM_ADDR		 (0x50)
#define CSL_I2C_CODEC_ADDR		 (0x18)

/*Values to be written to I2C registers*/
#define ICOAR_WRITEVAL  	0xFFFF
#define ICIMR_WRITEVAL 		0xFFFF
#define ICSTR_WRITEVAL 		0xFFFF  
#define ICCLKL_WRITEVAL 	0xFFFF
#define ICCLKH_WRITEVAL 	0xFFFF
#define ICCNT_WRITEVAL 		0xFFFF
#define ICDRR_WRITEVAL 		0xFFFF
#define ICSAR_WRITEVAL 		0xFFFF
#define ICDXR_WRITEVAL  	0xFFFF
#define ICMDR_WRITEVAL 		0xFFFF
#define ICIVR_WRITEVAL 		0xFFFF
#define ICEMDR_WRITEVAL 	0xFFFF
#define ICPSC_WRITEVAL 		0xFFFF
#define ICPID1_WRITEVAL 	0xFFFF
#define ICPID2_WRITEVAL 	0xFFFF

/*Values to be compared with the read ones from I2C registers*/
#define ICOAR_READVAL  		0x03FF
#define ICIMR_READVAL 		0x007F
#define ICSTR_READVAL 		0x0410
#define ICCLKL_READVAL 		0xFFFF
#define ICCLKH_READVAL 		0xFFFF
#define ICCNT_READVAL 		0xFFFF
#define ICDRR_READVAL 		0x0000
#define ICSAR_READVAL 		0x03FF
#define ICDXR_READVAL  		0x00FF
#define ICMDR_READVAL 		0xEFFF
#define ICIVR_READVAL 		0x0000
#define ICEMDR_READVAL 		0x0003
#define ICPSC_READVAL 		0x00FF
#define ICPID1_READVAL 		0x0106
#define ICPID2_READVAL 		0x0005




CSL_Status CSL_i2cRegAccessTest(void);

/////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
/////  Define PaSs_StAtE variable for catching errors as program executes.
/////  Define PaSs flag for holding final pass/fail result at program completion.
    volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
    volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
/////                                  program flow reaches expected exit point(s).
/////

void main(void)
{
	CSL_Status    result;

	printf("I2C REGISTER ACCESS TEST!\n\n");

	result =  CSL_i2cRegAccessTest();

	if(result == CSL_I2C_TEST_PASSED)
	{
		printf("\nI2C REGISTER ACCESS TEST PASSED!!\n");
	}
	else
	{
		printf("\nI2C REGISTER ACCESS TEST FAILED!!\n");
		PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
	}
    /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
    /////  At program exit, copy "PaSs_StAtE" into "PaSs".
        PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
    /////                   // pass/fail value determined during program execution.
    /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
    /////   control of a host PC script, will read and record the PaSs' value.
    /////
}

/**
 *  \brief  Tests I2C register access 
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status    CSL_i2cRegAccessTest(void)
{
	volatile Uint16    looper,i, status=0;

	/* Initialize I2C module */
	status = I2C_init(CSL_I2C0);
	if(status != CSL_SOK)
	{
		printf("I2C Init Failed!!\n");
		return(CSL_I2C_TEST_FAILED);
	}

	if(CSL_I2C_ICOAR_RESETVAL 		!= CSL_I2C_0_REGS->ICOAR)     
	{
	 printf("\r\nReset value is deviating for the register ICOAR\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	if(CSL_I2C_ICIMR_RESETVAL		!= CSL_I2C_0_REGS->ICIMR)       
	{
	 printf("\r\nReset value is deviating for the register ICIMR\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	if(CSL_I2C_ICSTR_RESETVAL      != CSL_I2C_0_REGS->ICSTR)           
	{
	 printf("\r\nReset value is deviating for the register ICSTR\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	if(CSL_I2C_ICCLKL_RESETVAL	    != CSL_I2C_0_REGS->ICCLKL)   		
	{
 	 printf("\r\nReset value is deviating for the register ICCLKL\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	if(CSL_I2C_ICCLKH_RESETVAL		!= CSL_I2C_0_REGS->ICCLKH)   		
	{
 	 printf("\r\nReset value is deviating for the register ICCLKH\r\n");	 
	 return(CSL_I2C_TEST_FAILED);
	}
	if(CSL_I2C_ICCNT_RESETVAL		!= CSL_I2C_0_REGS->ICCNT)   	
	{
 	 printf("\r\nReset value is deviating for the register ICCNT\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}

	CSL_SYSCTRL_REGS->PCGCR1 |= ~0xFFBF;//just i2c


		for(looper=0;looper<1000;looper++);

		CSL_SYSCTRL_REGS->PCGCR1 &= 0xFFBF;//restart i2c clk


	if(CSL_I2C_ICDRR_RESETVAL		!= CSL_I2C_0_REGS->ICDRR)   		
	{
 	 printf("\r\nReset value is deviating for the register ICDRR\r\n");	 
	 return(CSL_I2C_TEST_FAILED);
	}
	if(CSL_I2C_ICSAR_RESETVAL		!= CSL_I2C_0_REGS->ICSAR)  			
	{
 	 printf("\r\nReset value is deviating for the register ICSAR\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	if(CSL_I2C_ICDXR_RESETVAL		!= CSL_I2C_0_REGS->ICDXR)			
	{
 	 printf("\r\nReset value is deviating for the register ICDXR\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	if(CSL_I2C_ICMDR_RESETVAL		!= CSL_I2C_0_REGS->ICMDR)			
	{
 	 printf("\r\nReset value is deviating for the register ICMDR\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	if(CSL_I2C_ICIVR_RESETVAL		!=CSL_I2C_0_REGS->ICIVR)		
	{
 	 printf("\r\nReset value is deviating for the register ICIVR\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	if(CSL_I2C_ICEMDR_RESETVAL		!= CSL_I2C_0_REGS->ICEMDR)			
	{
  	 printf("\r\nReset value is deviating for the register ICEMDR\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	if(CSL_I2C_ICPSC_RESETVAL		!= CSL_I2C_0_REGS->ICPSC)			
	{
 	 printf("\r\nReset value is deviating for the register ICPSC\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	if(CSL_I2C_ICPID1_RESETVAL		!= CSL_I2C_0_REGS->ICPID1)			
	{
 	 printf("\r\nReset value is deviating for the register ICPID1\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	if(CSL_I2C_ICPID2_RESETVAL		!= CSL_I2C_0_REGS->ICPID2)
	{
 	 printf("\r\nReset value is deviating for the register ICPID2\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}

	/*Register access */
	CSL_I2C_0_REGS->ICOAR = ICOAR_WRITEVAL;
	if(CSL_I2C_0_REGS->ICOAR	    != ICOAR_READVAL)   	
	{
	 printf("\r\nRegister access failed for the register ICOAR\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	CSL_I2C_0_REGS->ICOAR = CSL_I2C_ICOAR_RESETVAL;

	CSL_I2C_0_REGS->ICIMR = ICIMR_WRITEVAL;
	if(CSL_I2C_0_REGS->ICIMR		!= ICIMR_READVAL )      
	{
	 printf("\r\nRegister access failed for the register ICIMR\r\n");	 
	 return(CSL_I2C_TEST_FAILED);
	}
	CSL_I2C_0_REGS->ICIMR = CSL_I2C_ICIMR_RESETVAL;
	
	CSL_I2C_0_REGS->ICSTR = ICSTR_WRITEVAL;
	if(CSL_I2C_0_REGS->ICSTR       != ICSTR_READVAL)       
	{
	 printf("\r\nRegister access failed for the register ICSTR\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	CSL_I2C_0_REGS->ICSTR = CSL_I2C_ICSTR_RESETVAL;

	CSL_I2C_0_REGS->ICCLKL= ICCLKL_WRITEVAL;
	if(CSL_I2C_0_REGS->ICCLKL	    != ICCLKL_READVAL)   
	{
	 printf("\r\nRegister access failed for the register ICCLKL\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	CSL_I2C_0_REGS->ICCLKL= CSL_I2C_ICCLKL_RESETVAL;

	CSL_I2C_0_REGS->ICCLKH= ICCLKH_WRITEVAL;
	if(CSL_I2C_0_REGS->ICCLKH		!= ICCLKH_READVAL)   
	{
	 printf("\r\nRegister access failed for the register ICCLKH\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	CSL_I2C_0_REGS->ICCLKH= CSL_I2C_ICCLKH_RESETVAL;

	CSL_I2C_0_REGS->ICCNT= ICCNT_WRITEVAL;
	if(CSL_I2C_0_REGS->ICCNT		!= ICCNT_READVAL)   
	{
	 printf("\r\nRegister access failed for the register ICCNT\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	CSL_I2C_0_REGS->ICCNT= CSL_I2C_ICCNT_RESETVAL;

	CSL_I2C_0_REGS->ICDRR= ICDRR_WRITEVAL;
	if(CSL_I2C_0_REGS->ICDRR		!= ICDRR_READVAL )   
	{
 	 printf("\r\nRegister access failed for the register ICDRR\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	CSL_I2C_0_REGS->ICDRR= CSL_I2C_ICDRR_RESETVAL;

	CSL_I2C_0_REGS->ICSAR= ICSAR_WRITEVAL;
	if(CSL_I2C_0_REGS->ICSAR		!= ICSAR_READVAL)  	
	{
	 printf("\r\nRegister access failed for the register ICSAR\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	CSL_I2C_0_REGS->ICSAR= CSL_I2C_ICSAR_RESETVAL;

	CSL_I2C_0_REGS->ICDXR= ICDXR_WRITEVAL;
	if(CSL_I2C_0_REGS->ICDXR		!= ICDXR_READVAL)		
	{
	 printf("\r\nRegister access failed for the register ICDXR\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	CSL_I2C_0_REGS->ICDXR= CSL_I2C_ICDXR_RESETVAL;

	CSL_I2C_0_REGS->ICMDR= ICMDR_WRITEVAL;
	if(CSL_I2C_0_REGS->ICMDR		!= ICMDR_READVAL)		
	{
 	 printf("\r\nRegister access failed for the register ICMDR\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	CSL_I2C_0_REGS->ICMDR= CSL_I2C_ICMDR_RESETVAL;

	CSL_I2C_0_REGS->ICIVR= ICIVR_WRITEVAL;
	if(CSL_I2C_0_REGS->ICIVR		!= ICIVR_READVAL)	
	{
	 printf("\r\nRegister access failed for the register ICIVR\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	CSL_I2C_0_REGS->ICIVR= CSL_I2C_ICIVR_RESETVAL;

	CSL_I2C_0_REGS->ICEMDR= ICEMDR_WRITEVAL;
	if(CSL_I2C_0_REGS->ICEMDR		!= ICEMDR_READVAL)	
	{
	 printf("\r\nRegister access failed for the register ICEMDR\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	CSL_I2C_0_REGS->ICEMDR= CSL_I2C_ICEMDR_RESETVAL;

	CSL_I2C_0_REGS->ICPSC= ICPSC_WRITEVAL;
	if(CSL_I2C_0_REGS->ICPSC		!= ICPSC_READVAL)		
	{
 	 printf("\r\nRegister access failed for the register ICPSC\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	CSL_I2C_0_REGS->ICPSC= CSL_I2C_ICPSC_RESETVAL;

	CSL_I2C_0_REGS->ICPID1= ICPID1_WRITEVAL;  //Read only register
	if(CSL_I2C_0_REGS->ICPID1		!= ICPID1_READVAL)		
	{
 	 printf("\r\nRegister access failed for the register ICPID1\r\n");
	 return(CSL_I2C_TEST_FAILED);
	}
	CSL_I2C_0_REGS->ICPID1= CSL_I2C_ICPID1_RESETVAL;

	CSL_I2C_0_REGS->ICPID2= ICPID2_WRITEVAL;  //Read Only register
	if(CSL_I2C_0_REGS->ICPID2		!= ICPID2_READVAL)
	{
	 printf("\r\nRegister access failed for the register ICPID2\r\n");	 
	 return(CSL_I2C_TEST_FAILED);
	}
	CSL_I2C_0_REGS->ICPID2= CSL_I2C_ICPID2_RESETVAL;

	return(CSL_I2C_TEST_PASSED);

}

