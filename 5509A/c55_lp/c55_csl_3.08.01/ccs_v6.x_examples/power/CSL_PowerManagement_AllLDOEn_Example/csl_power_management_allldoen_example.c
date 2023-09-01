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


/** @file CSL_PowerManagement_AllLDOEn_Example.c
 *
 *  @brief Example to enable power measurement when all ldo's are enabled.
 *
 * \page    page23 POWER EXAMPLE DOCUMENTATION
 *
 * \section POWER2   POWER EXAMPLE2 - ALL LDO ENABLE MODE TEST
 *
 * \subsection POWER2x    TEST DESCRIPTION:
 *        This test runs a dummy while loop while power can be measured
 *        with all LDO's enabled.
 *
 * \subsection POWER2y    TEST PROCEDURE:
 *  1. Ensure CLK_SEL C7 pin is low
 *  2. Ensure nDSP_LDO_EN pin D12 is low
 *  3. ANA_LDO is always enabled on Power ON
 *  4. Open the CCS and connect the target (C5517 EVM-Master board)
 *  5. Open the project "CSL_PowerManagement_AllLDOEn_Example.pjt" and build it
 *  6. Load the program on to the target
 *
*/


#include <stdio.h>
#include <csl_rtc.h>
#include <csl_intc.h>
#include <csl_general.h>




void main()
{
	printf("This test will be used for Power measurement"
			" with all 3 ldo's: DSP, ANA, USB enabled\r\n");

#if (defined(CHIP_C5517))
	printf("\nEnsure CLK_SEL JP10 is open");
	printf("\nEnsure nDSP_LDO_EN JP24 is shorted");
#else
	printf("\nEnsure CLK_SEL JP9 is open");
	printf("\nEnsure nDSP_LDO_EN JP6 is shorted");
#endif
	printf("\nANA_LDO is always enabled on Power ON");
	
        while(1);
}


