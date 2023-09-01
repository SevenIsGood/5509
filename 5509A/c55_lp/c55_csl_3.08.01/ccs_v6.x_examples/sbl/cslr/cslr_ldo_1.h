/********************************************************************
* Copyright (C) 2003-2008 Texas Instruments Incorporated.
* All Rights Reserved
*********************************************************************
* file: cslr_ldo_1.h
*
* Brief: This file contains the Register Description for ldo
*
*********************************************************************/
#ifndef _CSLR_LDO_1_H_
#define _CSLR_LDO_1_H_

#include <cslr.h>

#include <tistdtypes.h>


/* Minimum unit = 2 bytes */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint16 RSVD0[4];
    volatile Uint16 LDOCTRL;
} CSL_LdoRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* LDOCTRL */


#define CSL_LDO_LDOCTRL_DSPLDOCNTL_MASK  (0x0002u)
#define CSL_LDO_LDOCTRL_DSPLDOCNTL_SHIFT (0x0001u)
#define CSL_LDO_LDOCTRL_DSPLDOCNTL_RESETVAL (0x0000u)
/*----DSPLDOCNTL Tokens----*/
#define CSL_LDO_LDOCTRL_DSPLDOCNTL_1_3V  (0x0000u)
#define CSL_LDO_LDOCTRL_DSPLDOCNTL_1_05V (0x0001u)

#define CSL_LDO_LDOCTRL_USBLDOEN_MASK    (0x0001u)
#define CSL_LDO_LDOCTRL_USBLDOEN_SHIFT   (0x0000u)
#define CSL_LDO_LDOCTRL_USBLDOEN_RESETVAL (0x0000u)
/*----USBLDOEN Tokens----*/
#define CSL_LDO_LDOCTRL_USBLDOEN_DISABLED (0x0000u)
#define CSL_LDO_LDOCTRL_USBLDOEN_ENABLED (0x0001u)

#define CSL_LDO_LDOCTRL_RESETVAL         (0x0000u)

#endif
