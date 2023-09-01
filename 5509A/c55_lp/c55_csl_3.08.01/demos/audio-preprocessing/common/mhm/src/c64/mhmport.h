#ifndef _MHMPORT_H
#define _MHMPORT_H
/******************************************************************************
 * FILE PURPOSE: Definitions that are used on a C6x platform.
 ******************************************************************************
 * FILE NAME:   MHMPORT.H
 *
 * DESCRIPTION: Contains definitions that are used on a C6x platform.
 *
 * TABS: NONE
 *
 * $Id: mhmport.h 1.2 1998/08/06 16:50:04 jdowdal Exp $
 *
 * REVISION HISTORY:
 *
 * $Log: mhmport.h $
 * Revision 1.2  1998/08/06 16:50:04  jdowdal
 *      Fixed size of mhmAddress_t.
 * Revision 1.1  1998/07/22 14:12:15  jdowdal
 * Initial revision
 * Initial revision taken from c54x\mhmport.h
 *
 * Copyright (c) 2016, Texas Instruments Incorporated
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
 *
 */

/* System definitions and utilities */
#include <ti/mas/types/types.h>               /* DSP types                   */

/* The address type is an unsigned integer type that has the same number of
 *  bits as the longest possible (data) address on this platform. */
typedef tulong  mhmAddress_t;

#endif  /* _MHMPORT_H */
/* nothing past this point */