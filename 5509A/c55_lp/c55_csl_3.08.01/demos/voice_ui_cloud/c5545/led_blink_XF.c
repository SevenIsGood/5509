/* ============================================================================
 * Copyright (c) 2017 Texas Instruments Incorporated.
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
/** @file led_blink_XF.c
 *
 *  @brief OLED operation on C5545BP
 *

 * Revision History
 * ================
 * 03rd-Mar-2017 Created
  * ============================================================================
 */

#include <csl_intc.h>
#include <std.h>
#include <sem.h>
#include "oled.h"
#include "BF_rt_bios_cfg.h"

void 	waitLoop()   ;
Int16 r, s, v ;
Int32 counter = 0;
int start = 0;

void blinkXF()
{
       while (1)
        {
            SEM_pend(&SEM_BlinkXF, SYS_FOREVER);

			start = 1;
			oledInit();
			clear();
			setline(0);
			setOrientation(1);
			printstr("Blue Genie");
        }
}

// set XF according to flag
void setXF(int flag)
{
       Uint16 temp;

       // set ST1_55
    temp = CSL_CPU_REGS->ST1_55;
    // clear XF bit (bit 13)
    temp &=0xDFFF;
    // set bit 13 according to the value of flag
    if (flag!=0)
    temp |= 0x2000;
    // write it back to ST1_55
    CSL_CPU_REGS->ST1_55 = temp;
}

void waitLoop(int N)
{


	int i,j,k   ;
	for (k=0; k<20*N;k++)
	{
		for (j=0; j<1000; j++)
		{
			for (i=0; i<1000; i++)
			{
				asm ( "    NOP ; " )   ;
				asm ( "    NOP ; " )   ;
				asm ( "    NOP ; " )   ;
				asm ( "    NOP ; " )   ;
			}
		}
	}
}
