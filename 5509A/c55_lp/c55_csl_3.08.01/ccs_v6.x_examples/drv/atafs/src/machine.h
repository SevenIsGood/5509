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
 * (c)2001, Texas Instruments Inc.                                  *
 * Digital Audio Group                                              *
 *                                                                  *
 * Author: Jason Lin                                                *
 * File: machine.h                                                  *
 * Description:                                                     *
 *  This file should NOT be documented by Doxygen.  It is solely a  *
 *  bunch of machine specific definitions.  It basically makes it   *
 *  simple to port between PC, DSPs, and other micro.               *
 ********************************************************************/
#ifndef _MACHINE_H_
#define _MACHINE_H_

#ifdef __TMS320C55X__

#ifndef _Uint16_
#define _Uint16_
typedef unsigned short  Uint16;
#endif

#ifndef _Uint32_
#define _Uint32_
typedef unsigned long   Uint32;
#endif

#ifndef _Uchar_
#define _Uchar_
typedef unsigned char   Uchar;
#endif
#endif

#ifndef NULL
#define NULL 0
#endif

#endif /* _MACHINE_H_ */

