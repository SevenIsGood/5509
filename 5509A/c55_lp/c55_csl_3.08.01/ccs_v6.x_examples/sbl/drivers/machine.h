/********************************************************************* 
*    Property of Texas Instruments Incorporated, Copyright 2007
*    All rights reserved
**********************************************************************
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
