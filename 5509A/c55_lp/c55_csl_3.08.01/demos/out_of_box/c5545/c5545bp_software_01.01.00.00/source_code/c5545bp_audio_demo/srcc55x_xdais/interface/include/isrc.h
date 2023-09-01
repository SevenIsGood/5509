/*
 *  Copyright 1999 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */
/* $Id: isrc.h,v 1.1.4.2.8.1 2000/10/13 20:20:26 rowlands Exp $ */
/*
 *  ======== src.h ========
 *  This header defines all types, constants, and functions shared by all
 *  implementations of the abstract interface for the ISRC module.
 */
#ifndef ISRC_
#define ISRC_

#include <std.h>
#include <ialg.h>

/*
 *  ======== ISRC_Handle ========
 *  This handle is used to reference all ISRC instance objects.
 */
typedef struct ISRC_Obj *ISRC_Handle, SRC_Handle;

/*
 *  ======== ISRC_Params ========
 *  This structure defines the creation parameters for an ISRC instance object.
 */
typedef struct ISRC_Params {
	Int	size;			/* Size of this structure */

	Int	inputBufferSize;	/* Input frame length in number of samples */
	Int	outputBufferSize;	/* Output frame length in number of samples */

	LgInt	inputRate;		/* Input sample rate in Hz */
	LgInt	outputRate;		/* Output sample rate in Hz */

    Int realtimeInput;                   /* flag to specify streaming input */
} ISRC_Params, SRC_Params;

extern const ISRC_Params ISRC_PARAMS;	/* default params */

/*
 *  ======== ISRC_Status ========
 *  This structure defines the parameters that can be changed at runtime
 *  (read/write), and the instance status parameters (read-only).
 */
typedef struct ISRC_Status {
	Int	size;			/* Size of this structure */

	Bool	inputBufferEmpty;	/* Flag for empty input Buffer */
	Bool	outputBufferFull;	/* Flag for full output Buffer */

	Int	inputBufferSize;	/* Input frame length in number of samples */
	Int	outputBufferSize;	/* Output frame length in number of samples */

	LgInt	inputRate;		/* Input sample rate in Hz */
	LgInt	outputRate;		/* Output sample rate in Hz */
} ISRC_Status, SRC_Status;

/*
 *  ======== ISRC_Cmd ========
 */
typedef IALG_Cmd ISRC_Cmd;

/*
 *  ======== ISRC_Obj ========
 *  State variables for the sample rate converter.
 */
typedef struct ISRC_Obj {
	struct ISRC_Fxns const *fxns;
} ISRC_Obj, SRC_Obj;

/*
 * Declarations for operations on ISRC objects
 */
 
/* 
typedef	Void	ISRC_setInput_Fxn		(ISRC_Handle handle, Int *in);
typedef	Void	ISRC_setOutput_Fxn		(ISRC_Handle handle, Int *out);
typedef	Void	ISRC_apply_Fxn			(ISRC_Handle handle);
typedef	Void	ISRC_getStatus_Fxn		(ISRC_Handle, ISRC_Status);
typedef	Void	ISRC_setStatus_Fxn		(ISRC_Handle, const ISRC_Status);
typedef	Bool	ISRC_isInputBufferEmpty_Fxn	(ISRC_Handle handle);
typedef	Bool	ISRC_isOutputBufferFull_Fxn	(ISRC_Handle handle);
typedef	LgInt	ISRC_getInputRate_Fxn		(ISRC_Handle handle);
typedef	LgInt	ISRC_getOutputRate_Fxn		(ISRC_Handle handle);
typedef	Void	ISRC_setInputRate_Fxn		(ISRC_Handle handle, LgInt inputRate);
typedef	Void	ISRC_setOutputRate_Fxn		(ISRC_Handle handle, LgInt outputRate);
*/

typedef struct ISRC_Fxns {
	IALG_Fxns ialg;
	Void	(*setInput)		(ISRC_Handle handle, Int *in);
	Void	(*setOutput)		(ISRC_Handle handle, Int *out);
	Void	(*apply)			(ISRC_Handle handle);
	Void	(*getStatus)		(ISRC_Handle, ISRC_Status *);
	Void	(*setStatus)		(ISRC_Handle, const ISRC_Status*);
	Bool	(*isInputBufferEmpty)	(ISRC_Handle handle);
	Bool	(*isOutputBufferFull)	(ISRC_Handle handle);
	LgInt	(*getInputRate)		(ISRC_Handle handle);
	LgInt	(*getOutputRate)		(ISRC_Handle handle);
	Void	(*setInputRate)		(ISRC_Handle handle, LgInt inputRate);
	Void	(*setOutputRate)		(ISRC_Handle handle, LgInt outputRate);
	
	
/*
	ISRC_setInput_Fxn *		setInput;
	ISRC_setOutput_Fxn *		setOutput;
	ISRC_apply_Fxn *		apply;
	ISRC_getStatus_Fxn *		getStatus;
	ISRC_setStatus_Fxn *		setStatus;
	ISRC_isInputBufferEmpty_Fxn *	isInputBufferEmpty;
	ISRC_isOutputBufferFull_Fxn *	isOutputBufferFull;
	ISRC_getInputRate_Fxn *		getInputRate;
	ISRC_getOutputRate_Fxn *	getOutputRate;
	ISRC_setInputRate_Fxn *		setInputRate;
	ISRC_setOutputRate_Fxn *	setOutputRate;
*/	
	
} ISRC_Fxns, SRC_Fxns;


/*
 * Operations on ISRC objects
 */
/* IALG */
Void    SRC_algActivate		(ISRC_Handle);
Int     SRC_algControl		(ISRC_Handle, ISRC_Cmd, ISRC_Status *);
Void    SRC_algDeactivate	(ISRC_Handle);
Int     SRC_algFree		(ISRC_Handle, IALG_MemRec *);
Int     SRC_algInit		(ISRC_Handle, const IALG_MemRec *, ISRC_Handle, const ISRC_Params *);
Void    SRC_algMoved		(ISRC_Handle, const IALG_MemRec *, ISRC_Handle, const ISRC_Params *);

/* ISRC */
Void	SRC_setInput		(ISRC_Handle handle, Int *in);
Void	SRC_setOutput		(ISRC_Handle handle, Int *out);
Void	SRC_apply		(ISRC_Handle handle);
Void	SRC_getStatus		(ISRC_Handle, ISRC_Status *);
Void	SRC_setStatus		(ISRC_Handle, const ISRC_Status *);
Bool	SRC_isInputBufferEmpty	(ISRC_Handle handle);
Bool	SRC_isOutputBufferFull	(ISRC_Handle handle);
LgInt	SRC_getInputRate	(ISRC_Handle handle);
LgInt	SRC_getOutputRate	(ISRC_Handle handle);
Void	SRC_setInputRate	(ISRC_Handle handle, LgInt inputRate);
Void	SRC_setOutputRate	(ISRC_Handle handle, LgInt outputRate);


#endif  /* ISRC_ */
