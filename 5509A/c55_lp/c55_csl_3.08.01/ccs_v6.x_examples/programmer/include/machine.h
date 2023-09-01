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


/*********************************************************************
*    machine.h
**********************************************************************/
#ifndef MACHINE_H
#define MACHINE_H

#include <tistdtypes.h>

/* This should be #define'd if true. */
#if defined(WIN32)
#define LITTLE_ENDIAN
#define BYTES_PER_CHAR  1
typedef unsigned short UInt16;
typedef unsigned long UInt32;
#elif defined(_TMS320C5XX)
#undef LITTLE_ENDIAN
#define BYTES_PER_CHAR  2
typedef unsigned short UInt16;
typedef unsigned long UInt32;
#elif defined(__TMS320C55X__)
#undef LITTLE_ENDIAN
#define BYTES_PER_CHAR 2
typedef unsigned short UInt16;
typedef unsigned long UInt32;
typedef unsigned char byte;
#endif


/* -----------------11/1/2000 1:57PM-----------------
Where should this really go?
 --------------------------------------------------*/
#ifndef NULL
#define NULL 0
#endif

#endif /* MACHINE_H */
