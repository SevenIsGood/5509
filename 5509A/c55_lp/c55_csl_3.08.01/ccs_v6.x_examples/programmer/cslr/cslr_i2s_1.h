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
* file: cslr_i2s_1.h
*
* Brief: This file contains the Register Description for i2s
*
*********************************************************************/
#ifndef _CSLR_I2S_1_H_
#define _CSLR_I2S_1_H_

#include <cslr.h>

#include <tistdtypes.h>


/* Minimum unit = 2 bytes */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint16 I2SSCTRL;
    volatile Uint16 RSVD0[3];
    volatile Uint16 I2SSRATE;
    volatile Uint16 RSVD1[3];
    volatile Uint16 I2STXLT0;
    volatile Uint16 I2STXLT1;
    volatile Uint16 RSVD2[2];
    volatile Uint16 I2STXRT0;
    volatile Uint16 I2STXRT1;
    volatile Uint16 RSVD3[2];
    volatile Uint16 I2SINTFL;
    volatile Uint16 RSVD4[3];
    volatile Uint16 I2SINTMASK;
    volatile Uint16 RSVD5[19];
    volatile Uint16 I2SRXLT0;
    volatile Uint16 I2SRXLT1;
    volatile Uint16 RSVD6[2];
    volatile Uint16 I2SRXRT0;
    volatile Uint16 I2SRXRT1;
} CSL_I2sRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* I2SSCTRL */

#define CSL_I2S_I2SSCTRL_ENABLE_MASK     (0x8000u)
#define CSL_I2S_I2SSCTRL_ENABLE_SHIFT    (0x000Fu)
#define CSL_I2S_I2SSCTRL_ENABLE_RESETVAL (0x0000u)
/*----ENABLE Tokens----*/
#define CSL_I2S_I2SSCTRL_ENABLE_CLEAR    (0x0000u)
#define CSL_I2S_I2SSCTRL_ENABLE_SET      (0x0001u)


#define CSL_I2S_I2SSCTRL_MONO_MASK       (0x1000u)
#define CSL_I2S_I2SSCTRL_MONO_SHIFT      (0x000Cu)
#define CSL_I2S_I2SSCTRL_MONO_RESETVAL   (0x0000u)
/*----MONO Tokens----*/
#define CSL_I2S_I2SSCTRL_MONO_CLEAR      (0x0000u)
#define CSL_I2S_I2SSCTRL_MONO_SET        (0x0001u)

#define CSL_I2S_I2SSCTRL_LOOPBACK_MASK   (0x0800u)
#define CSL_I2S_I2SSCTRL_LOOPBACK_SHIFT  (0x000Bu)
#define CSL_I2S_I2SSCTRL_LOOPBACK_RESETVAL (0x0000u)
/*----LOOPBACK Tokens----*/
#define CSL_I2S_I2SSCTRL_LOOPBACK_CLEAR  (0x0000u)
#define CSL_I2S_I2SSCTRL_LOOPBACK_SET    (0x0001u)

#define CSL_I2S_I2SSCTRL_FSPOL_MASK      (0x0400u)
#define CSL_I2S_I2SSCTRL_FSPOL_SHIFT     (0x000Au)
#define CSL_I2S_I2SSCTRL_FSPOL_RESETVAL  (0x0000u)
/*----FSPOL Tokens----*/
#define CSL_I2S_I2SSCTRL_FSPOL_CLEAR     (0x0000u)
#define CSL_I2S_I2SSCTRL_FSPOL_SET       (0x0001u)

#define CSL_I2S_I2SSCTRL_CLKPOL_MASK     (0x0200u)
#define CSL_I2S_I2SSCTRL_CLKPOL_SHIFT    (0x0009u)
#define CSL_I2S_I2SSCTRL_CLKPOL_RESETVAL (0x0000u)
/*----CLKPOL Tokens----*/
#define CSL_I2S_I2SSCTRL_CLKPOL_CLEAR    (0x0000u)
#define CSL_I2S_I2SSCTRL_CLKPOL_SET      (0x0001u)

#define CSL_I2S_I2SSCTRL_DATADLY_MASK    (0x0100u)
#define CSL_I2S_I2SSCTRL_DATADLY_SHIFT   (0x0008u)
#define CSL_I2S_I2SSCTRL_DATADLY_RESETVAL (0x0000u)
/*----DATADLY Tokens----*/
#define CSL_I2S_I2SSCTRL_DATADLY_CLEAR   (0x0000u)
#define CSL_I2S_I2SSCTRL_DATADLY_SET     (0x0001u)

#define CSL_I2S_I2SSCTRL_PACK_MASK       (0x0080u)
#define CSL_I2S_I2SSCTRL_PACK_SHIFT      (0x0007u)
#define CSL_I2S_I2SSCTRL_PACK_RESETVAL   (0x0000u)
/*----PACK Tokens----*/
#define CSL_I2S_I2SSCTRL_PACK_CLEAR      (0x0000u)
#define CSL_I2S_I2SSCTRL_PACK_SET        (0x0001u)

#define CSL_I2S_I2SSCTRL_SIGN_EXT_MASK   (0x0040u)
#define CSL_I2S_I2SSCTRL_SIGN_EXT_SHIFT  (0x0006u)
#define CSL_I2S_I2SSCTRL_SIGN_EXT_RESETVAL (0x0000u)
/*----SIGN_EXT Tokens----*/
#define CSL_I2S_I2SSCTRL_SIGN_EXT_CLEAR  (0x0000u)
#define CSL_I2S_I2SSCTRL_SIGN_EXT_SET    (0x0001u)

#define CSL_I2S_I2SSCTRL_WDLNGTH_MASK    (0x003Cu)
#define CSL_I2S_I2SSCTRL_WDLNGTH_SHIFT   (0x0002u)
#define CSL_I2S_I2SSCTRL_WDLNGTH_RESETVAL (0x0000u)

#define CSL_I2S_I2SSCTRL_MODE_MASK       (0x0002u)
#define CSL_I2S_I2SSCTRL_MODE_SHIFT      (0x0001u)
#define CSL_I2S_I2SSCTRL_MODE_RESETVAL   (0x0000u)
/*----MODE Tokens----*/
#define CSL_I2S_I2SSCTRL_MODE_CLEAR      (0x0000u)
#define CSL_I2S_I2SSCTRL_MODE_SET        (0x0001u)

#define CSL_I2S_I2SSCTRL_FRMT_MASK       (0x0001u)
#define CSL_I2S_I2SSCTRL_FRMT_SHIFT      (0x0000u)
#define CSL_I2S_I2SSCTRL_FRMT_RESETVAL   (0x0000u)
/*----FRMT Tokens----*/
#define CSL_I2S_I2SSCTRL_FRMT_CLEAR      (0x0000u)
#define CSL_I2S_I2SSCTRL_FRMT_SET        (0x0001u)

#define CSL_I2S_I2SSCTRL_RESETVAL        (0x0000u)

/* I2SSRATE */


#define CSL_I2S_I2SSRATE_FSDIV_MASK      (0x0038u)
#define CSL_I2S_I2SSRATE_FSDIV_SHIFT     (0x0003u)
#define CSL_I2S_I2SSRATE_FSDIV_RESETVAL  (0x0000u)
/*----FSDIV Tokens----*/
#define CSL_I2S_I2SSRATE_FSDIV_DIV8      (0x0000u)
#define CSL_I2S_I2SSRATE_FSDIV_DIV16     (0x0001u)
#define CSL_I2S_I2SSRATE_FSDIV_DIV32     (0x0002u)
#define CSL_I2S_I2SSRATE_FSDIV_DIV64     (0x0003u)
#define CSL_I2S_I2SSRATE_FSDIV_DIV128    (0x0004u)
#define CSL_I2S_I2SSRATE_FSDIV_DIV256    (0x0005u)
#define CSL_I2S_I2SSRATE_FSDIV_RES1      (0x0006u)
#define CSL_I2S_I2SSRATE_FSDIV_RES2      (0x0007u)

#define CSL_I2S_I2SSRATE_CLKDIV_MASK     (0x0007u)
#define CSL_I2S_I2SSRATE_CLKDIV_SHIFT    (0x0000u)
#define CSL_I2S_I2SSRATE_CLKDIV_RESETVAL (0x0000u)
/*----CLKDIV Tokens----*/
#define CSL_I2S_I2SSRATE_CLKDIV_DIV2     (0x0000u)
#define CSL_I2S_I2SSRATE_CLKDIV_DIV4     (0x0001u)
#define CSL_I2S_I2SSRATE_CLKDIV_DIV8     (0x0002u)
#define CSL_I2S_I2SSRATE_CLKDIV_DIV16    (0x0003u)
#define CSL_I2S_I2SSRATE_CLKDIV_DIV32    (0x0004u)
#define CSL_I2S_I2SSRATE_CLKDIV_DIV64    (0x0005u)
#define CSL_I2S_I2SSRATE_CLKDIV_DIV128   (0x0006u)
#define CSL_I2S_I2SSRATE_CLKDIV_DIV256   (0x0007u)

#define CSL_I2S_I2SSRATE_RESETVAL        (0x0000u)

/* I2STXLT0 */

#define CSL_I2S_I2STXLT0_DATA_MASK       (0xFFFFu)
#define CSL_I2S_I2STXLT0_DATA_SHIFT      (0x0000u)
#define CSL_I2S_I2STXLT0_DATA_RESETVAL   (0x0000u)

#define CSL_I2S_I2STXLT0_RESETVAL        (0x0000u)

/* I2STXLT1 */

#define CSL_I2S_I2STXLT1_DATA_MASK       (0xFFFFu)
#define CSL_I2S_I2STXLT1_DATA_SHIFT      (0x0000u)
#define CSL_I2S_I2STXLT1_DATA_RESETVAL   (0x0000u)

#define CSL_I2S_I2STXLT1_RESETVAL        (0x0000u)

/* I2STXRT0 */

#define CSL_I2S_I2STXRT0_DATA_MASK       (0xFFFFu)
#define CSL_I2S_I2STXRT0_DATA_SHIFT      (0x0000u)
#define CSL_I2S_I2STXRT0_DATA_RESETVAL   (0x0000u)

#define CSL_I2S_I2STXRT0_RESETVAL        (0x0000u)

/* I2STXRT1 */

#define CSL_I2S_I2STXRT1_DATA_MASK       (0xFFFFu)
#define CSL_I2S_I2STXRT1_DATA_SHIFT      (0x0000u)
#define CSL_I2S_I2STXRT1_DATA_RESETVAL   (0x0000u)

#define CSL_I2S_I2STXRT1_RESETVAL        (0x0000u)

/* I2SINTFL */


#define CSL_I2S_I2SINTFL_XMITSTFL_MASK   (0x0020u)
#define CSL_I2S_I2SINTFL_XMITSTFL_SHIFT  (0x0005u)
#define CSL_I2S_I2SINTFL_XMITSTFL_RESETVAL (0x0000u)
/*----XMITSTFL Tokens----*/
#define CSL_I2S_I2SINTFL_XMITSTFL_CLEAR  (0x0000u)
#define CSL_I2S_I2SINTFL_XMITSTFL_SET    (0x0001u)

#define CSL_I2S_I2SINTFL_XMITMONFL_MASK  (0x0010u)
#define CSL_I2S_I2SINTFL_XMITMONFL_SHIFT (0x0004u)
#define CSL_I2S_I2SINTFL_XMITMONFL_RESETVAL (0x0000u)
/*----XMITMONFL Tokens----*/
#define CSL_I2S_I2SINTFL_XMITMONFL_CLEAR (0x0000u)
#define CSL_I2S_I2SINTFL_XMITMONFL_SET   (0x0001u)

#define CSL_I2S_I2SINTFL_RCVSTFL_MASK    (0x0008u)
#define CSL_I2S_I2SINTFL_RCVSTFL_SHIFT   (0x0003u)
#define CSL_I2S_I2SINTFL_RCVSTFL_RESETVAL (0x0000u)
/*----RCVSTFL Tokens----*/
#define CSL_I2S_I2SINTFL_RCVSTFL_CLEAR   (0x0000u)
#define CSL_I2S_I2SINTFL_RCVSTFL_SET     (0x0001u)

#define CSL_I2S_I2SINTFL_RCVMONFL_MASK   (0x0004u)
#define CSL_I2S_I2SINTFL_RCVMONFL_SHIFT  (0x0002u)
#define CSL_I2S_I2SINTFL_RCVMONFL_RESETVAL (0x0000u)
/*----RCVMONFL Tokens----*/
#define CSL_I2S_I2SINTFL_RCVMONFL_CLEAR  (0x0000u)
#define CSL_I2S_I2SINTFL_RCVMONFL_SET    (0x0001u)

#define CSL_I2S_I2SINTFL_FERRFL_MASK     (0x0002u)
#define CSL_I2S_I2SINTFL_FERRFL_SHIFT    (0x0001u)
#define CSL_I2S_I2SINTFL_FERRFL_RESETVAL (0x0000u)
/*----FERRFL Tokens----*/
#define CSL_I2S_I2SINTFL_FERRFL_CLEAR    (0x0000u)
#define CSL_I2S_I2SINTFL_FERRFL_SET      (0x0001u)

#define CSL_I2S_I2SINTFL_OUERRFL_MASK    (0x0001u)
#define CSL_I2S_I2SINTFL_OUERRFL_SHIFT   (0x0000u)
#define CSL_I2S_I2SINTFL_OUERRFL_RESETVAL (0x0000u)
/*----OUERRFL Tokens----*/
#define CSL_I2S_I2SINTFL_OUERRFL_CLEAR   (0x0000u)
#define CSL_I2S_I2SINTFL_OUERRFL_SET     (0x0001u)

#define CSL_I2S_I2SINTFL_RESETVAL        (0x0000u)

/* I2SINTMASK */


#define CSL_I2S_I2SINTMASK_XMITST_MASK   (0x0020u)
#define CSL_I2S_I2SINTMASK_XMITST_SHIFT  (0x0005u)
#define CSL_I2S_I2SINTMASK_XMITST_RESETVAL (0x0000u)
/*----XMITST Tokens----*/
#define CSL_I2S_I2SINTMASK_XMITST_DISABLE (0x0000u)
#define CSL_I2S_I2SINTMASK_XMITST_ENABLE (0x0001u)

#define CSL_I2S_I2SINTMASK_XMITMON_MASK  (0x0010u)
#define CSL_I2S_I2SINTMASK_XMITMON_SHIFT (0x0004u)
#define CSL_I2S_I2SINTMASK_XMITMON_RESETVAL (0x0000u)
/*----XMITMON Tokens----*/
#define CSL_I2S_I2SINTMASK_XMITMON_DISABLE (0x0000u)
#define CSL_I2S_I2SINTMASK_XMITMON_ENABLE (0x0001u)

#define CSL_I2S_I2SINTMASK_RCVST_MASK    (0x0008u)
#define CSL_I2S_I2SINTMASK_RCVST_SHIFT   (0x0003u)
#define CSL_I2S_I2SINTMASK_RCVST_RESETVAL (0x0000u)
/*----RCVST Tokens----*/
#define CSL_I2S_I2SINTMASK_RCVST_DISABLE (0x0000u)
#define CSL_I2S_I2SINTMASK_RCVST_ENABLE  (0x0001u)

#define CSL_I2S_I2SINTMASK_RCVMON_MASK   (0x0004u)
#define CSL_I2S_I2SINTMASK_RCVMON_SHIFT  (0x0002u)
#define CSL_I2S_I2SINTMASK_RCVMON_RESETVAL (0x0000u)
/*----RCVMON Tokens----*/
#define CSL_I2S_I2SINTMASK_RCVMON_DISABLE (0x0000u)
#define CSL_I2S_I2SINTMASK_RCVMON_ENABLE (0x0001u)

#define CSL_I2S_I2SINTMASK_FERR_MASK     (0x0002u)
#define CSL_I2S_I2SINTMASK_FERR_SHIFT    (0x0001u)
#define CSL_I2S_I2SINTMASK_FERR_RESETVAL (0x0000u)
/*----FERR Tokens----*/
#define CSL_I2S_I2SINTMASK_FERR_DISABLE  (0x0000u)
#define CSL_I2S_I2SINTMASK_FERR_ENABLE   (0x0001u)

#define CSL_I2S_I2SINTMASK_OUERR_MASK    (0x0001u)
#define CSL_I2S_I2SINTMASK_OUERR_SHIFT   (0x0000u)
#define CSL_I2S_I2SINTMASK_OUERR_RESETVAL (0x0000u)
/*----OUERR Tokens----*/
#define CSL_I2S_I2SINTMASK_OUERR_DISABLE (0x0000u)

#define CSL_I2S_I2SINTMASK_RESETVAL      (0x0000u)

/* I2SRXLT0 */

#define CSL_I2S_I2SRXLT0_DATA_MASK       (0xFFFFu)
#define CSL_I2S_I2SRXLT0_DATA_SHIFT      (0x0000u)
#define CSL_I2S_I2SRXLT0_DATA_RESETVAL   (0x0000u)

#define CSL_I2S_I2SRXLT0_RESETVAL        (0x0000u)

/* I2SRXLT1 */

#define CSL_I2S_I2SRXLT1_DATA_MASK       (0xFFFFu)
#define CSL_I2S_I2SRXLT1_DATA_SHIFT      (0x0000u)
#define CSL_I2S_I2SRXLT1_DATA_RESETVAL   (0x0000u)

#define CSL_I2S_I2SRXLT1_RESETVAL        (0x0000u)

/* I2SRXRT0 */

#define CSL_I2S_I2SRXRT0_DATA_MASK       (0xFFFFu)
#define CSL_I2S_I2SRXRT0_DATA_SHIFT      (0x0000u)
#define CSL_I2S_I2SRXRT0_DATA_RESETVAL   (0x0000u)

#define CSL_I2S_I2SRXRT0_RESETVAL        (0x0000u)

/* I2SRXRT1 */

#define CSL_I2S_I2SRXRT1_DATA_MASK       (0xFFFFu)
#define CSL_I2S_I2SRXRT1_DATA_SHIFT      (0x0000u)
#define CSL_I2S_I2SRXRT1_DATA_RESETVAL   (0x0000u)

#define CSL_I2S_I2SRXRT1_RESETVAL        (0x0000u)

#endif
