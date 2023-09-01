/*
 *  Copyright 2001 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */
/* "@(#) XDAS 2.12 04-24-01 (__imports)" */
/*
 *  ======== _alg.h ========
 */
#ifndef _ALG_
#define _ALG_

/*
 *  ======== _ALG_allocMemory ========
 */
extern Bool _ALG_allocMemory(IALG_MemRec *memTab, Int n);

/*
 *  ======== _ALG_freeMemory ========
 */
extern Void _ALG_freeMemory(IALG_MemRec *memTab, Int n);

#endif
