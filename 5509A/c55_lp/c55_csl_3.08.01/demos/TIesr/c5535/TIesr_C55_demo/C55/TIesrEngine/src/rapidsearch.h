/*=======================================================================

 *
 * rapidsearch.h
 *
 * Header for implementation of rapid likelihood calculation.
 *
 * Copyright (C) 2010-2013 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation version 2.1 of the License.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any kind,
 * whether express or implied; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 
 This header contains parameters used with the rapid search likelihood
 calculation based on Gaussian clustering.

======================================================================*/

#ifndef _RAPID_SEARCH_H
#define _RAPID_SEARCH_H

#include "tiesr_config.h"
#include "noise_sub.h"


/* -------------------------------------------------
   for Gaussian selection
   ------------------------------------------------- */
const ushort u_rj_perc_core = 16384; /* 50% core clusters */
const ushort u_rj_perc_inte = 0; /* 0% intermedium clusters */

/* --------------------------------------------------
   parameters for on-line reference modeling
   ------------------------------------------------- */
#define GBG_NUM_IDS 10  /* the maximum number of begining frames for a garbage model */
const ushort GBG_num_ids = 9; /* number of frames to construct a garbage model */

/* --------------------------------------------------
   parameters for GMM-based VAD
   ------------------------------------------------*/
const ushort GBG_HNOISE = 12000; /* Q9, threshold of noise level in log2, larger than the
				  number is considered as high noise level. */
const ushort GBG_VAD_VERIFY_TH_HNOISE = 80;
/* Threshold of the log-likelihood ratio for high noise level */
const ushort GBG_VAD_VERIFY_TH_LNOISE = 150;
/* Threshold of the log-likelihood ratio for low noise level */

const ushort GBG_NUM_SIL = 20;
/* minimum number of frames to declare long pause */
const ushort u_low_is_speech = 6;
/* threshold of log-likelihood ratio to declare long pause */

const short GBG_NUM_AFTER_PEAK_TO_TRIGGER_END_SPEECH = 150;
/* the maximum number of ending_cnt, which is increased after peak of LLR,
   that is allowed for VAD.
   above the number will trigger EOS */

/* parameters for end-of-speech detection */
const short GBG_END_SPEECH_START = 30; /* the minimum number of frame counter in decoder */
const short GBG_END_SPEECH_CNT_TH = 80;
/* the number of frames that are continuously below the threshold */
const ushort GBG_END_SPEECH_TH_PERC = 3277; /* threshold of LLR to declare EOS,
					       the threshold is computed as a certain
					       percentage of the maximum LLR.
					       10% in Q15 */

/* parameters for updating reference model */
const short GBG_post_threshold = 6; /* 10% in Q6 */

const short  GS_LOW_SCORE = -8104; /* the default lowest score */

#ifdef DEV_CLS2PHONE
#define PHONEPERCLS 20
#endif


#endif
