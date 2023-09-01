/*
 *  Copyright 1999 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */
/* $Id: ialgmods.h,v 1.1.4.1 1999/12/07 22:45:24 rowlands Exp $ */
/*
 *  ======== ialgmods.h ========
 *  This header adds some useful definitions to the IALG module.
 */
#ifndef IALGMODS_
#define IALGMODS_

#include <xdas.h>
#include <ialg.h>

typedef	Void    IALG_algActivate_Fxn	(IALG_Handle);
typedef	Int	IALG_algAlloc_Fxn	(const IALG_Params *, struct IALG_Fxns **, IALG_MemRec *);
typedef	Int     IALG_algControl_Fxn	(IALG_Handle, IALG_Cmd, IALG_Status *);
typedef	Void    IALG_algDeactivate_Fxn	(IALG_Handle);
typedef	Int     IALG_algFree_Fxn	(IALG_Handle, IALG_MemRec *);
typedef	Int     IALG_algInit_Fxn	(IALG_Handle, const IALG_MemRec *, IALG_Handle, const IALG_Params *);
typedef	Void    IALG_algMoved_Fxn	(IALG_Handle, const IALG_MemRec *, IALG_Handle, const IALG_Params *);
typedef	Int	IALG_algNumAlloc_Fxn	(Void);

#endif  /* IALGMODS_ */
