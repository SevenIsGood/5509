 /*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2008
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *  ============================================================================
 */

/** @file csl_mcbsp_tgt.h
 *
 *
 *  Path: \(CSLPATH)\ inc
 */

/* ============================================================================
 * Revision History
 * ================
 * 14-Oct-2010 Created
 * ============================================================================
 */

#include "cslr.h"
#include "csl_error.h"
#include "csl_types.h"
#include "cslr_mcbsp.h"
#include "soc.h"
#include "csl_intc.h"
#include "csl_general.h"


/****************************************\
* MCBSP Targets global macro declarations
\****************************************/

                                     // GPIO DATA in 16 bit format
                                     // gpio[9:6] is connected to Mcbsp targer
                                     // ---- --++ ++-- ----
#define MCBSP_TGT_NO_LB       0x0000 // 0000 0000 0000 0000
#define MCBSP_TGT_LB_MODE0    0x0040 // 0000 0000 0100 0000
#define MCBSP_TGT_LB_MODE1    0x00C0 // 0000 0000 1100 0000
#define MCBSP_TGT_LB_MODE2    0x0140 // 0000 0001 0100 0000
#define MCBSP_TGT_LB_MODE3    0x01C0 // 0000 0001 1100 0000
#define MCBSP_TGT_LB_MODE4    0x0240 // 0000 0010 0100 0000
#define MCBSP_TGT_LB_MODE5    0x02C0 // 0000 0010 1100 0000
#define MCBSP_TGT_LB_MODE6    0x0340 // 0000 0011 0100 0000
#define MCBSP_TGT_LB_MODE7    0x03C0 // 0000 0011 1100 0000



