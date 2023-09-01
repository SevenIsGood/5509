/*
 *  Copyright 1999 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */
/* $Id: smplrate.h,v 1.1.4.1 1999/12/07 22:45:26 rowlands Exp $ */
/*
 *  ======== smplrate.h ========
 *  This header defines types and constants used in the TI Internet Audio interfaces.
 */

#ifndef SMPLRATE_
#define SMPLRATE_

#include <std.h>

typedef LgInt	SMPLRATE_Hz;

typedef enum {
	SMPLRATE_Invalid = -1,
	SMPLRATE_8kHz = 0,
	SMPLRATE_11kHz,
	SMPLRATE_12kHz,
	SMPLRATE_16kHz,
	SMPLRATE_22kHz,
	SMPLRATE_24kHz,
	SMPLRATE_32kHz,
	SMPLRATE_44kHz,
	SMPLRATE_48kHz,
	SMPLRATE_8at48kHz,
	SMPLRATE_11at16kHz,
	SMPLRATE_16at48kHz,
	SMPLRATE_11at48kHz,
	SMPLRATE_22at48kHz,
	SMPLRATE_32at48kHz,
	SMPLRATE_12at16kHz,
	SMPLRATE_11at32kHz,
	SMPLRATE_12at32kHz,
	SMPLRATE_NumRates
} SMPLRATE_Idx;





SMPLRATE_Idx	SRC_TI_SMPLRATE_hzToIdx	(SMPLRATE_Hz hz);
SMPLRATE_Hz	SRC_TI_SMPLRATE_idxToHz	(SMPLRATE_Idx idx);

#endif /* SMPLRATE_ */
