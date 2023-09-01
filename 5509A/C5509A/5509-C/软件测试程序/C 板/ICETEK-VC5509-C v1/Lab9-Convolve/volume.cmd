/*
 *  Copyright 2001 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 */
/*
 *  ======== volume.cmd ========
 *
 */


MEMORY {
   DATA:       origin = 0x6000,        len = 0x4000
   PROG:       origin = 0x200,         len = 0x5e00
   VECT:       origin = 0xd000,        len = 0x100
}

SECTIONS
{
    .vectors: {} > VECT
    .trcinit: {} > PROG
    .gblinit: {} > PROG
     frt:     {} > PROG
    .text:    {} > PROG
    .cinit:   {} > PROG
    .pinit:   {} > PROG
    .sysinit: {} > PROG
    .bss:     {} > DATA
    .far:     {} > DATA
    .const:   {} > DATA
    .switch:  {} > DATA
    .sysmem:  {} > DATA
    .cio:     {} > DATA
    .MEM$obj: {} > DATA
    .sysheap: {} > DATA
    .stack:   {} > DATA
    .sysstack {} > DATA
}
