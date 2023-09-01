/*=======================================================================

 *
 * confidence_struct_user.h
 *
 * Header for confidence functionality interface.
 *
 * Copyright (C) 2012,2013 Texas Instruments Incorporated - http://www.ti.com/
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

======================================================================*/

#ifndef CONFIDENCE_STRUCT_USER_H
#define CONFIDENCE_STRUCT_USER_H

#include "tiesr_config.h"
#include "tiesrcommonmacros.h"
 
/* Include the confidence structures */

typedef struct{
  short mean; /* Q11 */
  short inv ; /* Q9 */
  short gconst; /* Q6 */
}CM_hypo;

#define CM_SIZE 1300
typedef struct{
  short  base_mem[CM_SIZE];
  ushort  mem_count ;

  ushort *answers; //[CM_MAX_WORD]; /* recognized words */
  ushort *uStp; //[CM_MAX_WORD]; /* word-ending frame index */
  ushort nWords; /* number of words */

  short nS; /* number of segments */

  short *cm_scr; /* the confidence measure
		    [0] : raw log-likelihood distance of the best path versus the
		          accumulated score of the maximum likelihood score along speech frames,
		    [1] : non-stationary SNR, refer to AT&T paper
		    [2] : likelihood ratio of the two hypothesis log(P(X|H_0)) - log(P(X|H_1))
		    [3] : logistic regression score, P(H_0|X) ,
		    the final confidence score is cm_scr[2]*cm_scr[3], which is the product
		        of the scores = p(H_0|X)*[ log(P(X|H_0)) - log(P(X|H_1)) ] */

  Boolean bNeedUpdate; /* TRUE if the anti score needs to be updated */
  short *cm_anti_scr; //[CM_MAX_FRM];

  long cm_fg_scr;
  long cm_bk_scr; /* accumulated max score of speech frames */

  short th_nbest; /* the threshold of number of N-bests to declare low
		     confidence of an uttearnce */

  short nSpeech; /* number of speech frames */
  short nNoise;  /* number of noise frames */
  long speech_power; /* Q6 */
  long noise_power;  /* Q6 */
  long noise_power_sqr; /* Q12 */
  short NSNR; /* non-stationary SNR */
  CM_hypo H0;
  CM_hypo H1;

  short *lgBeta; //[CM_NBR_BETA];
  long *acc_dif2_beta; //[CM_NBR_BETA];

  Boolean bCalced; /* calculated confidence score */

#ifdef USE_CONF_DRV_ADP
  /* for confidence driven unsupervised adaptation */
  short *sCH; /* channel distortion estimate backup */
  long * lAccum;
  short adp_th; /* the threshold for adaptation */
#ifdef USE_SVA
  short * sLogVarRho;
  long * lVarFisherIM;
#endif

#endif

}ConfType;

#endif
