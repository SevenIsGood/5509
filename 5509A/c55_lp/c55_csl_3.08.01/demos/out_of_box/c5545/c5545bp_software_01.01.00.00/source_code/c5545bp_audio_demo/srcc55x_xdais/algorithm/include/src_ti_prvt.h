/*
 *  Copyright 1999 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */
/* $Id: src_ti.h,v 1.1.4.3 2000/07/27 20:27:18 rowlands Exp $ */
/*
 *  ======== src.h ========
 *  This header defines all types, constants, and functions shared by all
 *  implementations of the abstract interface for the SRC module.
 */
#ifndef SRC_TI_PRVT_
#define SRC_TI_PRVT_

#include "isrc.h"

#define UPSAMPLE_WRAP 1764*2
#define DOWNSAMPLE_WRAP 1760*2

/*
 *  ======== SRC_TI_Handle ========
 *  This handle is used to reference all SRC instance objects.
 */
typedef struct SRC_TI_Obj *SRC_TI_Handle;

/*
 *  ======== SRC_TI_Params ========
 *  This structure defines the creation parameters for a SRC instance object.
 */
typedef ISRC_Params SRC_TI_Params;

/*
 *  ======== SRC_TI_Status ========
 *  This structure defines the parameters that can be changed at runtime
 *  (read/write), and the instance status parameters (read-only).
 */
typedef ISRC_Status SRC_TI_Status;

/*
 *  ======== SRC_TI_Order ========
 *  Order of each interpolation filter phase
 */
#define SRC_TI_Order	(16)

/*
 *  ======== SRC_TI_Obj ========
 *  State variables for the sample rate converter.
 *  NOTE: The structure must be aligned on a 32-word address boundary.
 */
typedef struct SRC_TI_Obj {
	/* IALG */
	struct ISRC_Fxns const *fxns;

	/* SRC_TI */
	LgInt	inputRate;		/* Input sample rate in Hz */
	LgInt	outputRate;		/* Output sample rate in Hz */

	Int *	inputBuffer;		/* Input Buffer */
	Int	inputBufferSize;	/* Input Buffer Size */
	Int	inputBufferCounter;	/* Input Buffer Counter */
	Bool	inputBufferEmpty;	/* Flag for empty input Buffer */

	Int *	outputBuffer;		/* Output Buffer */
	Int	outputBufferSize;	/* OutputBuffer Size */
	Int	outputBufferCounter;	/* OutputBuffer Counter */
	Bool	outputBufferFull;	/* Flag for full output Buffer */

	Int	TimeDelta;		/* Ratio between 2 sample rates */
	Int	DistanceCounter;	/* Stores the offset New Sample is from Original Sample */
	Int	SampleRateIdx;		/* SampleRate index from smplrate.h */

	Int *	pLPFDataX;		/* Stores the Low Pass Filter coefficients x values */
	Int *	pInterpolData;		/* Stores Samples to be used in interpolation */
    Int Skipped_Last;    /*determines whether skipped last sample in 48kHz to 44kHz SRC */

    Int *pOverflowBuf;       /* Downsampling Overflow buffer                                  */
    Int overflowSize;        /* Downsampling Overflow size                                    */
    Int upSample;
    Int realtimeInput;                   /* If we are streaming the input, then we can't afford to lose any input so we need the overflow */
/*-----------------11/28/00 6:42PM------------------
 * This next variable should no longer be needed since 4 new variables were added when the SRC's were combined - SRH
 * --------------------------------------------------*/
  	Int alignintepoldata;	/*SUSMIT*/	/*For keeping offset through different XDAIS calls*/
    Int LPFData[4];       /* y[n-1], x[n-1], y[n-2], x[n-2] */

    Int skipNum;          /* number of input samples to skip for each new output sample */
    Int temp;
	Int alignlpfdata;	/*SUSMIT*/	/*For keeping offset through different XDAIS calls*/
    Int pad[25]; 	/*SUSMIT*/	/*Padding to align Interpoldata to a 32word boundary*/
	Int	InterpolData[SRC_TI_Order+1];
    Int overflowData[72];

    Int needToInterpolate;   /* Flag to indicate when input/output buffer empties/fills before interpolate is done.*/
                             /* If flag is set, need to interpolate next time we enter src. */
    Int CounterWrapValue;    /* Value at which counter wraps around. */
   
} SRC_TI_Obj;

/*
 * Operations on SRC_TI objects
 */
Int	SRC_TI_algInit			(IALG_Handle Handle, const IALG_MemRec *mem, IALG_Handle  p, IALG_Params const *params);

Void	SRC_TI_setInput			(ISRC_Handle handle, Int *in);
Void	SRC_TI_setOutput		(ISRC_Handle handle, Int *out);
Void	SRC_TI_apply			(ISRC_Handle handle);
Bool	SRC_TI_isInputBufferEmpty	(ISRC_Handle handle);
Bool	SRC_TI_isOutputBufferFull	(ISRC_Handle handle);

Void	SRC_TI_getStatus		(ISRC_Handle handle, ISRC_Status *status);
Void	SRC_TI_setStatus		(ISRC_Handle handle, const ISRC_Status *status);
LgInt	SRC_TI_getInputRate		(ISRC_Handle handle);
LgInt	SRC_TI_getOutputRate		(ISRC_Handle handle);
Void	SRC_TI_setInputRate		(ISRC_Handle handle, LgInt inputRate);
Void	SRC_TI_setOutputRate		(ISRC_Handle handle, LgInt outputRate);



/* private to SRC_TI */
Void	SRC_TI_interp			(ISRC_Handle handle);

//extern Int  SRC_TI_algInit	(IALG_Handle Handle, const IALG_MemRec * mem, IALG_Handle p, const IALG_Params *params);
Int	 SRC_TI_algNumAlloc (void);
Int	 SRC_TI_algAlloc	(const IALG_Params * Params, IALG_Fxns ** fxns, IALG_MemRec * memTab);
Int  SRC_TI_algFree 	(IALG_Handle Handle, IALG_MemRec * mem);


#endif  /* SRC_TI_ */
