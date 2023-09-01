/* ============================================================================
 * Copyright (c) 2008-2012 Texas Instruments Incorporated.
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


#ifndef _CSLR__SPI_1_H_
#define _CSLR__SPI_1_H_

#include <cslr.h>

/* Minimum unit = 2 bytes */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint16 SPICC1;
    volatile Uint16 SPICC2;
    volatile Uint16 SPIDC1;
    volatile Uint16 SPIDC2;
    volatile Uint16 SPICR1;
    volatile Uint16 SPICR2;
    volatile Uint16 SPISR1;
    volatile Uint16 SPISR2;
    volatile Uint16 SPIDR1;
    volatile Uint16 SPIDR2;
} CSL_SpiRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* SPICC1 */

#define CSL_SPI_SPICC1_DCLK_DIV_MASK (0xFFFFu)
#define CSL_SPI_SPICC1_DCLK_DIV_SHIFT (0x0000u)
#define CSL_SPI_SPICC1_DCLK_DIV_RESETVAL (0x0000u)

#define CSL_SPI_SPICC1_RESETVAL (0x0000u)

/* SPICC2 */

#define CSL_SPI_SPICC2_CLKEN_MASK (0x8000u)
#define CSL_SPI_SPICC2_CLKEN_SHIFT (0x000Fu)
#define CSL_SPI_SPICC2_CLKEN_RESETVAL (0x0000u)


#define CSL_SPI_SPICC2_RESETVAL (0x0000u)

/* SPIDC1 */


#define CSL_SPI_SPIDC1_DD1_MASK (0x1800u)
#define CSL_SPI_SPIDC1_DD1_SHIFT (0x000Bu)
#define CSL_SPI_SPIDC1_DD1_RESETVAL (0x0000u)

#define CSL_SPI_SPIDC1_CKPH1_MASK (0x0400u)
#define CSL_SPI_SPIDC1_CKPH1_SHIFT (0x000Au)
#define CSL_SPI_SPIDC1_CKPH1_RESETVAL (0x0000u)

#define CSL_SPI_SPIDC1_CSP1_MASK (0x0200u)
#define CSL_SPI_SPIDC1_CSP1_SHIFT (0x0009u)
#define CSL_SPI_SPIDC1_CSP1_RESETVAL (0x0000u)

#define CSL_SPI_SPIDC1_CKP1_MASK (0x0100u)
#define CSL_SPI_SPIDC1_CKP1_SHIFT (0x0008u)
#define CSL_SPI_SPIDC1_CKP1_RESETVAL (0x0000u)


#define CSL_SPI_SPIDC1_DD0_MASK (0x0018u)
#define CSL_SPI_SPIDC1_DD0_SHIFT (0x0003u)
#define CSL_SPI_SPIDC1_DD0_RESETVAL (0x0000u)

#define CSL_SPI_SPIDC1_CKPH0_MASK (0x0004u)
#define CSL_SPI_SPIDC1_CKPH0_SHIFT (0x0002u)
#define CSL_SPI_SPIDC1_CKPH0_RESETVAL (0x0000u)

#define CSL_SPI_SPIDC1_CSP0_MASK (0x0002u)
#define CSL_SPI_SPIDC1_CSP0_SHIFT (0x0001u)
#define CSL_SPI_SPIDC1_CSP0_RESETVAL (0x0000u)

#define CSL_SPI_SPIDC1_CKP0_MASK (0x0001u)
#define CSL_SPI_SPIDC1_CKP0_SHIFT (0x0000u)
#define CSL_SPI_SPIDC1_CKP0_RESETVAL (0x0000u)

#define CSL_SPI_SPIDC1_RESETVAL (0x0000u)

/* SPIDC2 */

#define CSL_SPI_SPIDC2_LPBK_MASK (0x8000u)
#define CSL_SPI_SPIDC2_LPBK_SHIFT (0x000Fu)
#define CSL_SPI_SPIDC2_LPBK_RESETVAL (0x0000u)


#define CSL_SPI_SPIDC2_DD3_MASK (0x1800u)
#define CSL_SPI_SPIDC2_DD3_SHIFT (0x000Bu)
#define CSL_SPI_SPIDC2_DD3_RESETVAL (0x0000u)

#define CSL_SPI_SPIDC2_CKPH3_MASK (0x0400u)
#define CSL_SPI_SPIDC2_CKPH3_SHIFT (0x000Au)
#define CSL_SPI_SPIDC2_CKPH3_RESETVAL (0x0000u)

#define CSL_SPI_SPIDC2_CSP3_MASK (0x0200u)
#define CSL_SPI_SPIDC2_CSP3_SHIFT (0x0009u)
#define CSL_SPI_SPIDC2_CSP3_RESETVAL (0x0000u)

#define CSL_SPI_SPIDC2_CKP3_MASK (0x0100u)
#define CSL_SPI_SPIDC2_CKP3_SHIFT (0x0008u)
#define CSL_SPI_SPIDC2_CKP3_RESETVAL (0x0000u)


#define CSL_SPI_SPIDC2_DD2_MASK (0x0018u)
#define CSL_SPI_SPIDC2_DD2_SHIFT (0x0003u)
#define CSL_SPI_SPIDC2_DD2_RESETVAL (0x0000u)

#define CSL_SPI_SPIDC2_CKPH2_MASK (0x0004u)
#define CSL_SPI_SPIDC2_CKPH2_SHIFT (0x0002u)
#define CSL_SPI_SPIDC2_CKPH2_RESETVAL (0x0000u)

#define CSL_SPI_SPIDC2_CSP2_MASK (0x0002u)
#define CSL_SPI_SPIDC2_CSP2_SHIFT (0x0001u)
#define CSL_SPI_SPIDC2_CSP2_RESETVAL (0x0000u)

#define CSL_SPI_SPIDC2_CKP2_MASK (0x0001u)
#define CSL_SPI_SPIDC2_CKP2_SHIFT (0x0000u)
#define CSL_SPI_SPIDC2_CKP2_RESETVAL (0x0000u)

#define CSL_SPI_SPIDC2_RESETVAL (0x0000u)

/* SPICR1 */


#define CSL_SPI_SPICR1_WIRQ_MASK (0x4000u)
#define CSL_SPI_SPICR1_WIRQ_SHIFT (0x000Eu)
#define CSL_SPI_SPICR1_WIRQ_RESETVAL (0x0000u)


#define CSL_SPI_SPICR1_FLEN_MASK (0x0FFFu)
#define CSL_SPI_SPICR1_FLEN_SHIFT (0x0000u)
#define CSL_SPI_SPICR1_FLEN_RESETVAL (0x0000u)

#define CSL_SPI_SPICR1_RESETVAL (0x0000u)

/* SPICR2 */


#define CSL_SPI_SPICR2_CSNUM_MASK (0x3000u)
#define CSL_SPI_SPICR2_CSNUM_SHIFT (0x000Cu)
#define CSL_SPI_SPICR2_CSNUM_RESETVAL (0x0000u)


#define CSL_SPI_SPICR2_WLEN_MASK (0x00F8u)
#define CSL_SPI_SPICR2_WLEN_SHIFT (0x0003u)
#define CSL_SPI_SPICR2_WLEN_RESETVAL (0x0000u)


#define CSL_SPI_SPICR2_CMD_MASK (0x0003u)
#define CSL_SPI_SPICR2_CMD_SHIFT (0x0000u)
#define CSL_SPI_SPICR2_CMD_RESETVAL (0x0000u)

#define CSL_SPI_SPICR2_RESETVAL (0x0000u)

/* SPISR1 */


#define CSL_SPI_SPISR1_AE_MASK (0x0008u)
#define CSL_SPI_SPISR1_AE_SHIFT (0x0003u)
#define CSL_SPI_SPISR1_AE_RESETVAL (0x0000u)

#define CSL_SPI_SPISR1_FC_MASK (0x0004u)
#define CSL_SPI_SPISR1_FC_SHIFT (0x0002u)
#define CSL_SPI_SPISR1_FC_RESETVAL (0x0000u)

#define CSL_SPI_SPISR1_WC_MASK (0x0002u)
#define CSL_SPI_SPISR1_WC_SHIFT (0x0001u)
#define CSL_SPI_SPISR1_WC_RESETVAL (0x0000u)

#define CSL_SPI_SPISR1_BSY_MASK (0x0001u)
#define CSL_SPI_SPISR1_BSY_SHIFT (0x0000u)
#define CSL_SPI_SPISR1_BSY_RESETVAL (0x0000u)

#define CSL_SPI_SPISR1_RESETVAL (0x0000u)

/* SPISR2 */


#define CSL_SPI_SPISR2_WDCNT_MASK (0x1FFFu)
#define CSL_SPI_SPISR2_WDCNT_SHIFT (0x0000u)
#define CSL_SPI_SPISR2_WDCNT_RESETVAL (0x0000u)

#define CSL_SPI_SPISR2_RESETVAL (0x0000u)

/* SPIDR1 */

#define CSL_SPI_SPIDR1_DATA_MASK (0xFFFFu)
#define CSL_SPI_SPIDR1_DATA_SHIFT (0x0000u)
#define CSL_SPI_SPIDR1_DATA_RESETVAL (0x0000u)

#define CSL_SPI_SPIDR1_RESETVAL (0x0000u)

/* SPIDR2 */

#define CSL_SPI_SPIDR2_DATA_MASK (0xFFFFu)
#define CSL_SPI_SPIDR2_DATA_SHIFT (0x0000u)
#define CSL_SPI_SPIDR2_DATA_RESETVAL (0x0000u)

#define CSL_SPI_SPIDR2_RESETVAL (0x0000u)

#endif

