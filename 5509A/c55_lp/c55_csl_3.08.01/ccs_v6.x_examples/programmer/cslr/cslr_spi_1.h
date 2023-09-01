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


/********************************************************************
* file: cslr_spi_1.h
*
* Brief: This file contains the Register Description for spi
*
*********************************************************************/
#ifndef _CSLR_SPI_1_H_
#define _CSLR_SPI_1_H_

#include <cslr.h>

#include <tistdtypes.h>


/* Minimum unit = 2 bytes */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint16 SPICDR;
    volatile Uint16 SPICCR;
    volatile Uint16 SPIDCR1;
    volatile Uint16 SPIDCR2;
    volatile Uint16 SPICMD1;
    volatile Uint16 SPICMD2;
    volatile Uint16 SPISTAT1;
    volatile Uint16 SPISTAT2;
    volatile Uint16 SPIDR1;
    volatile Uint16 SPIDR2;
} CSL_SpiRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* SPICDR */

#define CSL_SPI_SPICDR_CLKDV_MASK        (0xFFFFu)
#define CSL_SPI_SPICDR_CLKDV_SHIFT       (0x0000u)
#define CSL_SPI_SPICDR_CLKDV_RESETVAL    (0x0000u)

#define CSL_SPI_SPICDR_RESETVAL          (0x0000u)

/* SPICCR */

#define CSL_SPI_SPICCR_CLKEN_MASK        (0x8000u)
#define CSL_SPI_SPICCR_CLKEN_SHIFT       (0x000Fu)
#define CSL_SPI_SPICCR_CLKEN_RESETVAL    (0x0000u)
/*----CLKEN Tokens----*/
#define CSL_SPI_SPICCR_CLKEN_DISABLED    (0x0000u)
#define CSL_SPI_SPICCR_CLKEN_ENABLED     (0x0001u)

#define CSL_SPI_SPICCR_RST_MASK          (0x4000u)
#define CSL_SPI_SPICCR_RST_SHIFT         (0x000Eu)
#define CSL_SPI_SPICCR_RST_RESETVAL      (0x0000u)
/*----RST Tokens----*/
#define CSL_SPI_SPICCR_RST_RELEASE       (0x0000u)
#define CSL_SPI_SPICCR_RST_ASSERT        (0x0001u)


#define CSL_SPI_SPICCR_RESETVAL          (0x0000u)

/* SPIDCR1 */


#define CSL_SPI_SPIDCR1_DD1_MASK         (0x1800u)
#define CSL_SPI_SPIDCR1_DD1_SHIFT        (0x000Bu)
#define CSL_SPI_SPIDCR1_DD1_RESETVAL     (0x0000u)
/*----DD1 Tokens----*/
#define CSL_SPI_SPIDCR1_DD1_DELAY0       (0x0000u)
#define CSL_SPI_SPIDCR1_DD1_DELAY1       (0x0001u)
#define CSL_SPI_SPIDCR1_DD1_DELAY2       (0x0002u)
#define CSL_SPI_SPIDCR1_DD1_DELAY3       (0x0003u)

#define CSL_SPI_SPIDCR1_CKPH1_MASK       (0x0400u)
#define CSL_SPI_SPIDCR1_CKPH1_SHIFT      (0x000Au)
#define CSL_SPI_SPIDCR1_CKPH1_RESETVAL   (0x0000u)
/*----CKPH1 Tokens----*/
#define CSL_SPI_SPIDCR1_CKPH1_LOW        (0x0000u)
#define CSL_SPI_SPIDCR1_CKPH1_HIGH       (0x0001u)

#define CSL_SPI_SPIDCR1_CSP1_MASK        (0x0200u)
#define CSL_SPI_SPIDCR1_CSP1_SHIFT       (0x0009u)
#define CSL_SPI_SPIDCR1_CSP1_RESETVAL    (0x0000u)
/*----CSP1 Tokens----*/
#define CSL_SPI_SPIDCR1_CSP1_LOW         (0x0000u)
#define CSL_SPI_SPIDCR1_CSP1_HIGH        (0x0001u)

#define CSL_SPI_SPIDCR1_CKP1_MASK        (0x0100u)
#define CSL_SPI_SPIDCR1_CKP1_SHIFT       (0x0008u)
#define CSL_SPI_SPIDCR1_CKP1_RESETVAL    (0x0000u)
/*----CKP1 Tokens----*/
#define CSL_SPI_SPIDCR1_CKP1_LOW         (0x0000u)
#define CSL_SPI_SPIDCR1_CKP1_HIGH        (0x0001u)


#define CSL_SPI_SPIDCR1_DD0_MASK         (0x0018u)
#define CSL_SPI_SPIDCR1_DD0_SHIFT        (0x0003u)
#define CSL_SPI_SPIDCR1_DD0_RESETVAL     (0x0000u)
/*----DD0 Tokens----*/
#define CSL_SPI_SPIDCR1_DD0_DELAY0       (0x0000u)
#define CSL_SPI_SPIDCR1_DD0_DELAY1       (0x0001u)
#define CSL_SPI_SPIDCR1_DD0_DELAY2       (0x0002u)
#define CSL_SPI_SPIDCR1_DD0_DELAY3       (0x0003u)

#define CSL_SPI_SPIDCR1_CKPH0_MASK       (0x0004u)
#define CSL_SPI_SPIDCR1_CKPH0_SHIFT      (0x0002u)
#define CSL_SPI_SPIDCR1_CKPH0_RESETVAL   (0x0000u)
/*----CKPH0 Tokens----*/
#define CSL_SPI_SPIDCR1_CKPH0_LOW        (0x0000u)
#define CSL_SPI_SPIDCR1_CKPH0_HIGH       (0x0001u)

#define CSL_SPI_SPIDCR1_CSP0_MASK        (0x0002u)
#define CSL_SPI_SPIDCR1_CSP0_SHIFT       (0x0001u)
#define CSL_SPI_SPIDCR1_CSP0_RESETVAL    (0x0000u)
/*----CSP0 Tokens----*/
#define CSL_SPI_SPIDCR1_CSP0_LOW         (0x0000u)
#define CSL_SPI_SPIDCR1_CSP0_HIGH        (0x0001u)

#define CSL_SPI_SPIDCR1_CKP0_MASK        (0x0001u)
#define CSL_SPI_SPIDCR1_CKP0_SHIFT       (0x0000u)
#define CSL_SPI_SPIDCR1_CKP0_RESETVAL    (0x0000u)
/*----CKP0 Tokens----*/
#define CSL_SPI_SPIDCR1_CKP0_LOW         (0x0000u)
#define CSL_SPI_SPIDCR1_CKP0_HIGH        (0x0001u)

#define CSL_SPI_SPIDCR1_RESETVAL         (0x0000u)

/* SPIDCR2 */

#define CSL_SPI_SPIDCR2_LPBK_MASK        (0x8000u)
#define CSL_SPI_SPIDCR2_LPBK_SHIFT       (0x000Fu)
#define CSL_SPI_SPIDCR2_LPBK_RESETVAL    (0x0000u)
/*----LPBK Tokens----*/
#define CSL_SPI_SPIDCR2_LPBK_DISABLE     (0x0000u)
#define CSL_SPI_SPIDCR2_LPBK_ENABLE      (0x0001u)


#define CSL_SPI_SPIDCR2_DD3_MASK         (0x1800u)
#define CSL_SPI_SPIDCR2_DD3_SHIFT        (0x000Bu)
#define CSL_SPI_SPIDCR2_DD3_RESETVAL     (0x0000u)
/*----DD3 Tokens----*/
#define CSL_SPI_SPIDCR2_DD3_DELAY0       (0x0000u)
#define CSL_SPI_SPIDCR2_DD3_DELAY1       (0x0001u)
#define CSL_SPI_SPIDCR2_DD3_DELAY2       (0x0002u)
#define CSL_SPI_SPIDCR2_DD3_DELAY3       (0x0003u)

#define CSL_SPI_SPIDCR2_CKPH3_MASK       (0x0400u)
#define CSL_SPI_SPIDCR2_CKPH3_SHIFT      (0x000Au)
#define CSL_SPI_SPIDCR2_CKPH3_RESETVAL   (0x0000u)
/*----CKPH3 Tokens----*/
#define CSL_SPI_SPIDCR2_CKPH3_LOW        (0x0000u)
#define CSL_SPI_SPIDCR2_CKPH3_HIGH       (0x0001u)

#define CSL_SPI_SPIDCR2_CSP3_MASK        (0x0200u)
#define CSL_SPI_SPIDCR2_CSP3_SHIFT       (0x0009u)
#define CSL_SPI_SPIDCR2_CSP3_RESETVAL    (0x0000u)
/*----CSP3 Tokens----*/
#define CSL_SPI_SPIDCR2_CSP3_LOW         (0x0000u)
#define CSL_SPI_SPIDCR2_CSP3_HIGH        (0x0001u)

#define CSL_SPI_SPIDCR2_CKP3_MASK        (0x0100u)
#define CSL_SPI_SPIDCR2_CKP3_SHIFT       (0x0008u)
#define CSL_SPI_SPIDCR2_CKP3_RESETVAL    (0x0000u)
/*----CKP3 Tokens----*/
#define CSL_SPI_SPIDCR2_CKP3_LOW         (0x0000u)
#define CSL_SPI_SPIDCR2_CKP3_HIGH        (0x0001u)


#define CSL_SPI_SPIDCR2_DD2_MASK         (0x0018u)
#define CSL_SPI_SPIDCR2_DD2_SHIFT        (0x0003u)
#define CSL_SPI_SPIDCR2_DD2_RESETVAL     (0x0000u)
/*----DD2 Tokens----*/
#define CSL_SPI_SPIDCR2_DD2_DELAY0       (0x0000u)
#define CSL_SPI_SPIDCR2_DD2_DELAY1       (0x0001u)
#define CSL_SPI_SPIDCR2_DD2_DELAY2       (0x0002u)
#define CSL_SPI_SPIDCR2_DD2_DELAY3       (0x0003u)

#define CSL_SPI_SPIDCR2_CKPH2_MASK       (0x0004u)
#define CSL_SPI_SPIDCR2_CKPH2_SHIFT      (0x0002u)
#define CSL_SPI_SPIDCR2_CKPH2_RESETVAL   (0x0000u)
/*----CKPH2 Tokens----*/
#define CSL_SPI_SPIDCR2_CKPH2_LOW        (0x0000u)
#define CSL_SPI_SPIDCR2_CKPH2_HIGH       (0x0001u)

#define CSL_SPI_SPIDCR2_CSP2_MASK        (0x0002u)
#define CSL_SPI_SPIDCR2_CSP2_SHIFT       (0x0001u)
#define CSL_SPI_SPIDCR2_CSP2_RESETVAL    (0x0000u)
/*----CSP2 Tokens----*/
#define CSL_SPI_SPIDCR2_CSP2_LOW         (0x0000u)
#define CSL_SPI_SPIDCR2_CSP2_HIGH        (0x0001u)

#define CSL_SPI_SPIDCR2_CKP2_MASK        (0x0001u)
#define CSL_SPI_SPIDCR2_CKP2_SHIFT       (0x0000u)
#define CSL_SPI_SPIDCR2_CKP2_RESETVAL    (0x0000u)
/*----CKP2 Tokens----*/
#define CSL_SPI_SPIDCR2_CKP2_LOW         (0x0000u)
#define CSL_SPI_SPIDCR2_CKP2_HIGH        (0x0001u)

#define CSL_SPI_SPIDCR2_RESETVAL         (0x0000u)

/* SPICMD1 */

#define CSL_SPI_SPICMD1_FIRQ_MASK        (0x8000u)
#define CSL_SPI_SPICMD1_FIRQ_SHIFT       (0x000Fu)
#define CSL_SPI_SPICMD1_FIRQ_RESETVAL    (0x0000u)
/*----FIRQ Tokens----*/
#define CSL_SPI_SPICMD1_FIRQ_DISABLE     (0x0000u)
#define CSL_SPI_SPICMD1_FIRQ_ENABLE      (0x0001u)

#define CSL_SPI_SPICMD1_CIRQ_MASK        (0x4000u)
#define CSL_SPI_SPICMD1_CIRQ_SHIFT       (0x000Eu)
#define CSL_SPI_SPICMD1_CIRQ_RESETVAL    (0x0000u)
/*----CIRQ Tokens----*/
#define CSL_SPI_SPICMD1_CIRQ_DISABLE     (0x0000u)
#define CSL_SPI_SPICMD1_CIRQ_ENABLE      (0x0001u)


#define CSL_SPI_SPICMD1_FLEN_MASK        (0x0FFFu)
#define CSL_SPI_SPICMD1_FLEN_SHIFT       (0x0000u)
#define CSL_SPI_SPICMD1_FLEN_RESETVAL    (0x0000u)

#define CSL_SPI_SPICMD1_RESETVAL         (0x0000u)

/* SPICMD2 */


#define CSL_SPI_SPICMD2_CSNUM_MASK       (0x3000u)
#define CSL_SPI_SPICMD2_CSNUM_SHIFT      (0x000Cu)
#define CSL_SPI_SPICMD2_CSNUM_RESETVAL   (0x0000u)
/*----CSNUM Tokens----*/
#define CSL_SPI_SPICMD2_CSNUM_CS0        (0x0000u)
#define CSL_SPI_SPICMD2_CSNUM_CS1        (0x0001u)
#define CSL_SPI_SPICMD2_CSNUM_CS2        (0x0002u)
#define CSL_SPI_SPICMD2_CSNUM_CS3        (0x0003u)


#define CSL_SPI_SPICMD2_CLEN_MASK        (0x00F8u)
#define CSL_SPI_SPICMD2_CLEN_SHIFT       (0x0003u)
#define CSL_SPI_SPICMD2_CLEN_RESETVAL    (0x0000u)


#define CSL_SPI_SPICMD2_CMD_MASK         (0x0003u)
#define CSL_SPI_SPICMD2_CMD_SHIFT        (0x0000u)
#define CSL_SPI_SPICMD2_CMD_RESETVAL     (0x0000u)
/*----CMD Tokens----*/
#define CSL_SPI_SPICMD2_CMD_RSV1         (0x0000u)
#define CSL_SPI_SPICMD2_CMD_READ         (0x0001u)
#define CSL_SPI_SPICMD2_CMD_RSV2         (0x0002u)
#define CSL_SPI_SPICMD2_CMD_WRITE        (0x0003u)

#define CSL_SPI_SPICMD2_RESETVAL         (0x0000u)

/* SPISTAT1 */


#define CSL_SPI_SPISTAT1_FC_MASK         (0x0004u)
#define CSL_SPI_SPISTAT1_FC_SHIFT        (0x0002u)
#define CSL_SPI_SPISTAT1_FC_RESETVAL     (0x0000u)
/*----FC Tokens----*/
#define CSL_SPI_SPISTAT1_FC_NOTCOMPLETE  (0x0000u)
#define CSL_SPI_SPISTAT1_FC_COMPLETE     (0x0001u)

#define CSL_SPI_SPISTAT1_CC_MASK         (0x0002u)
#define CSL_SPI_SPISTAT1_CC_SHIFT        (0x0001u)
#define CSL_SPI_SPISTAT1_CC_RESETVAL     (0x0000u)
/*----CC Tokens----*/
#define CSL_SPI_SPISTAT1_CC_NOTCOMPLETE  (0x0000u)
#define CSL_SPI_SPISTAT1_CC_COMPLETE     (0x0001u)

#define CSL_SPI_SPISTAT1_BSY_MASK        (0x0001u)
#define CSL_SPI_SPISTAT1_BSY_SHIFT       (0x0000u)
#define CSL_SPI_SPISTAT1_BSY_RESETVAL    (0x0000u)
/*----BSY Tokens----*/
#define CSL_SPI_SPISTAT1_BSY_NOTBUSY     (0x0000u)
#define CSL_SPI_SPISTAT1_BSY_BUSY        (0x0001u)

#define CSL_SPI_SPISTAT1_RESETVAL        (0x0000u)

/* SPISTAT2 */


#define CSL_SPI_SPISTAT2_CCNT_MASK       (0x1FFFu)
#define CSL_SPI_SPISTAT2_CCNT_SHIFT      (0x0000u)
#define CSL_SPI_SPISTAT2_CCNT_RESETVAL   (0x0000u)

#define CSL_SPI_SPISTAT2_RESETVAL        (0x0000u)

/* SPIDR1 */

#define CSL_SPI_SPIDR1_DATA_MASK         (0xFFFFu)
#define CSL_SPI_SPIDR1_DATA_SHIFT        (0x0000u)
#define CSL_SPI_SPIDR1_DATA_RESETVAL     (0x0000u)

#define CSL_SPI_SPIDR1_RESETVAL          (0x0000u)

/* SPIDR2 */

#define CSL_SPI_SPIDR2_DATA_MASK         (0xFFFFu)
#define CSL_SPI_SPIDR2_DATA_SHIFT        (0x0000u)
#define CSL_SPI_SPIDR2_DATA_RESETVAL     (0x0000u)

#define CSL_SPI_SPIDR2_RESETVAL          (0x0000u)

#endif
