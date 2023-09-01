/*
 *  Copyright 1999 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */
/* $Id: eq_ti.h,v 1.1.4.2 1999/12/15 16:55:15 rowlands Exp $ */
/*
 *  ======== eq.h ========
 *  This header defines all types, constants, and functions shared by all
 *  implementations of the abstract interface for the EQ module.
 */
#ifndef EQ_TI_PRVT_
#define EQ_TI_PRVT_

#include "ieq.h"

/*
 * EQ_Coeff
 */
 
typedef struct EQ_TI_Coeff{
	LgInt	Beta;
	LgInt	BGamma;
} EQ_TI_Coeff;

/*
 *  ======== EQ_Params ========
 *  This structure defines the creation parameters for a EQ instance object. 
 */
typedef struct EQ_TI_Params {
	/* IALG */
	Int	size;		/* Size of this structure */

		/* IEQ */
	Int	bufferSize;	/* Frame length in number of samples */

	Int	numBands;	/* Number of equalizer bands */
	Int const *dBGains;	/* Initial gain settings */

	/* EQ_TI */
	EQ_TI_Coeff const *coeffs;	/* array of band coeffs - 0 sets defaults */
	EQ_TI_Coeff const *deltaCoeffs;	/* array of band coeffs - 0 sets defaults */
} EQ_TI_Params;


/*
 *  ======== EQ_Handle ========
 *  This handle is used to reference all EQ instance objects.
 */
typedef struct EQ_TI_Obj *EQ_TI_Handle;

/*
 *  ======== EQ_TI_Band ========
 *  Object must be aligned on 4-word address boundary
 */
typedef struct EQ_TI_Band {
	Int	XState[2];	/* align on 4-word address boundary */
/*	Int	_align2;	*//* align Beta on 2-word address boundary *//*SUSMIT*/
    Int padd;
	Int	Gain;		/* Gain, BGamma, and Beta must be contiguous */
	LgInt	Beta;
	LgInt	BGamma;

	LgInt	WState[2];	/* align on 4-word address boundary */
	Int	dBGain;
	Int const * pGain;

	EQ_TI_Coeff const *pCoeff;
	EQ_TI_Coeff const *pDeltaCoeff;
} EQ_TI_Band;

/*
 *  ======== EQ_Obj ========
 *  This structure must be the first field of all EQ instance objects.
 */
typedef struct EQ_TI_Obj {
	/* IALG */
	const IEQ_Fxns *fxns;		/* Function table */

	/* EQ */
	Int	bufferSize;	/* Length of input and output data blocks */
	Int	numBands;	/* Number of equalizer bands */
	EQ_TI_Band *pBands;
//	LgInt  tempbuf[3];
	Uns	EQChangeFlag;
	LgInt  *tempbuf; /*;;LgInt*/
	
/*	EQ_TI_Coeff const *coeffs;
	EQ_TI_Coeff const *deltaCoeffs;*/
} EQ_TI_Obj;

/*
 *  ======== EQ_Status ========
 *  This structure defines the parameters that can be changed at runtime
 *  (read/write), and the instance status parameters (read-only).
 */
typedef IEQ_Status  EQ_TI_Status;

/*
 * V-table for EQ_TI
 */
extern const IEQ_Fxns EQ_TI_IEQ;

/*
 * Operations on EQ_TI objects
 */
 
Int  EQ_TI_algNumAlloc(void);
Int	 EQ_TI_algAlloc	(const IALG_Params * Params, IALG_Fxns **fxns, IALG_MemRec *memTab); 
Int  EQ_TI_algFree 	(IALG_Handle Handle, IALG_MemRec * mem);

Int	EQ_TI_algInit	(IALG_Handle Handle, const IALG_MemRec *mem, IALG_Handle p, const IALG_Params *Params);

Void	EQ_TI_apply	(IEQ_Handle handle, Int *in, Int *out);
Void	EQ_TI_getStatus	(IEQ_Handle, IEQ_Status *status);
Void	EQ_TI_setStatus	(IEQ_Handle, const IEQ_Status *status);

Void	EQ_TI_getGains	(IEQ_Handle handle, Int *gains);
Void	EQ_TI_setGains	(IEQ_Handle handle, Int const *gains);

Int	EQ_TI_getGain	(IEQ_Handle handle, Int band);
Void	EQ_TI_setGain	(IEQ_Handle handle, Int band, Int gain);

#endif  /* EQ_TI_ */
