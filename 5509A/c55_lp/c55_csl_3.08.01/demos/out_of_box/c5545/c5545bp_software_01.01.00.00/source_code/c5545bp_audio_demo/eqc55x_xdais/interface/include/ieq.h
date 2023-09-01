/*
 *  Copyright 1999 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */
/* $Id: ieq.h,v 1.1.4.1 1999/12/07 22:45:25 rowlands Exp $ */
/*
 *  ======== eq.h ========
 *  This header defines all types, constants, and functions shared by all
 *  implementations of the abstract interface for the IEQ module.
 */
#ifndef IEQ_
#define IEQ_

#include "ialg.h"
#include "xdas.h"

typedef struct IEQ_Coeff{
	LgInt	Beta;
	LgInt	BGamma;
} IEQ_Coeff,EQ_Coeff;




/*
 *  ======== IEQ_Handle ========
 *  This handle is used to reference all IEQ instance objects.
 */
typedef struct IEQ_Obj *IEQ_Handle, EQ_Handle;

/*
 *  ======== IEQ_Params ========
 *  This structure defines the creation parameters for a IEQ instance object. 
 */
typedef struct IEQ_Params {
	/* IALG */
	Int	size;		/* Size of this structure */

	/* IEQ */
	Int	bufferSize;	/* Frame length in number of samples */

	Int	numBands;	/* Number of equalizer bands */
	Int const *dBGains;	/* Initial gain settings */

	/*EQ coefficients..User can change it through this pointers*/
	IEQ_Coeff *coeffs;
    IEQ_Coeff *deltaCoeffs;

} IEQ_Params, EQ_Params;

/*
 *  ======== IEQ_Status ========
 *  This structure defines the parameters that can be changed at runtime
 *  (read/write), and the instance status parameters (read-only).
 */
typedef struct IEQ_Status {
	/* IALG */
	Int	size;		/* Size of this structure */

		/* IEQ */
	Int	bufferSize;	/* Frame length in number of samples */

	Int	numBands;	/* Number of equalizer bands */
	
	/*EQ coefficients..User can change it through this pointers*/
	IEQ_Coeff *coeffs;
    IEQ_Coeff *deltaCoeffs;

} IEQ_Status, EQ_Status;

/*
 *  ======== IEQ_Cmd ========
 */
typedef IALG_Cmd IEQ_Cmd;

/*
 *  ======== IEQ_Obj ========
 *  This structure must be the first field of all IEQ instance objects.
 */
typedef struct IEQ_Obj {
	/* IALG */
	struct IEQ_Fxns const *	fxns;
} IEQ_Obj, EQ_Obj;

/*
 * Declarations for operations on IEQ objects
 */
 
 /*
typedef	Void	IEQ_apply_Fxn	(IEQ_Handle handle, Int *in, Int *out);
typedef	Void	IEQ_getStatus_Fxn	(IEQ_Handle handle, IEQ_Status *status);
typedef	Void	IEQ_setStatus_Fxn	(IEQ_Handle handle, const IEQ_Status *status);

typedef	Void	IEQ_getGains_Fxn	(IEQ_Handle handle, Int *gains);
typedef	Void	IEQ_setGains_Fxn	(IEQ_Handle handle, Int *gains);
typedef	Int	    IEQ_getGain_Fxn		(IEQ_Handle handle, Int band);
typedef	Void	IEQ_setGain_Fxn		(IEQ_Handle handle, Int band, Int gain);

*/

typedef struct IEQ_Fxns {
	IALG_Fxns ialg;
	
	Void ( *apply)(IEQ_Handle handle, Int *in, Int *out);
	Void (*getStatus)(IEQ_Handle handle, IEQ_Status *status);
	Void (*setStatus)(IEQ_Handle handle, const IEQ_Status *status);
	Void (*getGains)(IEQ_Handle handle, Int *gains);
	Void (*setGains)(IEQ_Handle handle, Int const *gains);
	Int (*getGain)(IEQ_Handle handle, Int band);
	Void (*setGain)(IEQ_Handle handle, Int band, Int gain);
/*	IEQ_apply_Fxn *	apply;
	IEQ_getStatus_Fxn *	getStatus;
	IEQ_setStatus_Fxn *	setStatus;
	IEQ_getGains_Fxn *	getGains;
	IEQ_setGains_Fxn *	setGains;
	IEQ_getGain_Fxn *	getGain;
	IEQ_setGain_Fxn *	setGain;
*/

		
} IEQ_Fxns, EQ_Fxns;

/*
 * Operations on IEQ objects
 */
/* IALG */

Void    EQ_algActivate		(IEQ_Handle);
Int     EQ_algControl		(IEQ_Handle, IEQ_Cmd, IEQ_Status *);
Void    EQ_algDeactivate	(IEQ_Handle);
Int     EQ_algFree		(IEQ_Handle, IALG_MemRec *);
Int     EQ_algInit		(IEQ_Handle, const IALG_MemRec *, IEQ_Handle, const IEQ_Params *);
Void    EQ_algMoved		(IEQ_Handle, const IALG_MemRec *, IEQ_Handle, const IEQ_Params *);

/* IEQ */
Void	EQ_apply	(IEQ_Handle handle, Int *in, Int *out);
Void	EQ_getStatus	(IEQ_Handle handle, IEQ_Status *status);
Void	EQ_setStatus	(IEQ_Handle handle, const IEQ_Status *status);

/* IEQ */
Void	EQ_getGains	(IEQ_Handle handle, Int *gains);
Void	EQ_setGains	(IEQ_Handle handle, Int *gains);
Int		EQ_getGain	(IEQ_Handle handle, Int band);
Void	EQ_setGain	(IEQ_Handle handle, Int band, Int gain);

#endif  /* IEQ_ */
