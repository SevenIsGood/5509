/********************************************************************* 
*    Property of Texas Instruments Incorporated, Copyright 2008
*    All rights reserved
**********************************************************************
*    secbootutils.c
**********************************************************************/
#include "machine.h"
#include "soc.h"
#include "secboot.h"
#include "romver.h"
#include "sysctrl.h"

void ROM_API_TABLE();

extern unsigned long API_PTR;

/********************************************************************* 
* vSetAPIVectorAddress
********************************************************************/
void vSetAPIVectorAddress()
{
   unsigned long *ptr;

   ptr = &API_PTR;
   *ptr = ((unsigned long)ROM_API_TABLE);
}

/********************************************************************* 
* vGetROMVersion
********************************************************************/
UInt32 vGetROMVersion()
{
   return (ROM_VERSION);
}

/********************************************************************* 
* vGetDieID
********************************************************************/
void vGetDieID(UInt16 *buffer)
{
   ioport UInt16 *ioptr = (UInt16 *)&(CSL_SYSCTRL_REGS->DIEIDR0);

   /* copy the 4 16-bit words */
   *buffer++ = *ioptr++;
   *buffer++ = *ioptr++;
   *buffer++ = *ioptr++;
   *buffer = *ioptr;
}

