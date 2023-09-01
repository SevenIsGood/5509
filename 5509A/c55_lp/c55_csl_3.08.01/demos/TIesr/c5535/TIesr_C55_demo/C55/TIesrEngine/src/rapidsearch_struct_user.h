/*=======================================================================

 *
 * rapidsearch_struct_user.h
 *
 * Structures for rapid likelihood calculation using clustering.
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
#ifndef _RAPID_SEARCH_STRUCT_H
#define _RAPID_SEARCH_STRUCT_H


#include "tiesr_config.h"
#include "tiesrcommonmacros.h"

/* --------------------------------------------------
   data structure for offline VQ-based HMM processing
   saves VQ information of mean vectors
   -------------------------------------------------- */

#define VQHMM_SIZE 10000

typedef struct _OFFLINE_VQ_HMM{
  short  base_mem[VQHMM_SIZE];
  ushort  mem_count ;

  ushort n_cs; /* number of clusters */
  ushort nbr_dim; /* dimension of the vectors */

  short sz_RamMean; /* number of RAM mean vectors */
  unsigned char * pRAM2Cls; /* mapping of RAM mean to cluster index */

  short sz_ROMMean; /* number of ROM mean vectors */

  short gconst;
  short *inv_ave_var; /* inverse of the average covarianc of the acoustic model */

  short *vq_centroid; /* centroid of clusters */
  short *vq_bias; /* JAC compensation bias */
  Boolean bCompensated;

  /* ------------ Gaussian selection ---------------------------------------*/
  short low_score;
  short max_dist; /* maximum score of Gaussian clusters */
  short * dist ; /* distance of clusters to observation feature */
  char * category; /* cluster category, 0 - core, 1 - intermediate, 2 - outmost */

  ushort sVQ_CLS_TO_EVAL ; /* number of core clusters */
  ushort sVQ_NUM_INTER; /* number of intermedidate clusters */
  /* -----------------------------------------------------------------------*/

#ifdef  USE_ONLINE_REF
  /* --------------- On-line reference modeling -----------------------------*/
  short *imax_10; /* indices of clusters for the reference model */
  short inum_diff_cls; /* number of different clusters in the first GBG_NUM_IDS frames */
  short inum_frms_gbg; /* number of frames to construct a garbage model */
  short i_low_is_speech; /* number of frames with is_speech continuously below a threshold */
  short is_speech; /* averaged log-likelihood ratio of the best matched cluster relative
		      to a reference model */
  short ga_score; /* garbage model score */

#ifdef USE_ORM_PU
  short *iprior; /* prior list for reference model */
  short *pprior; /* prior prabobility of clusters in the reference model */
  short inum_prior;
#endif

  /* ---------------------------------------------------------------------- */

#if defined(DEV_CLS2PHONE)
  /* cluster to phone mapping */
  char cls_phone[128 * PHONEPERCLS];
  char cls_nphone[128];

  Boolean bRefCloseToSilence; /* TRUE if the reference model has silence model */
#endif

  short fd_frms; /* number of frames input for ORM */

#ifdef USE_ORM_VAD
  /* ---------------------------- GMM-based VAD ----------------------------- */
  Boolean bVADVerified; /* if the VAD has been verified by the VQ method */
  short voice_idx; /* processed frames */
  Boolean bSynced; /* if the following mfcc_buf has been dumped */
  short iPushed; /* number of frames that have been pushed */
  short *mfcc_buf ; //[ MFCC_BUF_SZ ][ MAX_DIM ]; /* MFCC buffer */
  Boolean b_orig_done; /* if the current MFCC has been used */

  /* some threshold for begining of speech detection and frame dropping */
  short th_noise_lvl; /* the noise level threshold to select an LLR threshold */
  short th_llr_in_low_noise; /* the LLR threshold in low noise level */
  short th_llr_in_high_noise;  /* the LLR threshold in high noise level */

  /* ------------------------- end-of-speech detection ----------------------- */
  /* for end of speech detection */
  /* parameters */
  short max_llr; /* the maximum value of LLR */
  short ending_cnt; /* the number of frames after the peak of LLR */

  short ending_cnt_stt; /* don't do EOS untill ending_cnt_stt of frames have passed
			   peak of LLR */
  short ending_llr_perc; /* the percentage to compute ending_llr_th */
  short ending_llr_th; /* the threshold of LLR to declare end of speech */
  short ending_cnt_th; /* the threshold for $ending_cnt, above it, a different threshold
			  of LLR is used, as compared to a tighter threshold for
			  near frames */

  short ending_low_llr_cnt;
  /* the number of frames that continuoulsy with LLR below $ending_llr_th */
  short ending_cnt_after_peak; /* the threshold to stop decoder if
				  more than the number of frames have been processed
				  fter peak of LLR has been reached */

  /* ------------------------------------------------------------------------ */
#endif

#endif

} OFFLINE_VQ_HMM_type;

/* ---------------------------------------------------------------
   Phone-level pruning
   --------------------------------------------------------------- */
/* the maximum number of monophones */
#ifdef MODEL_LEVEL_PRUNE

#define MLP_NUM_MONOPHONE 64
#define MLP_SIZE (MLP_NUM_MONOPHONE << 1)
const ushort MLP_uPercentage = 22938; /* 70% of active phones are removed */

/* data structure of the phone-level pruning */
typedef struct _MODEL_PRUNE{
  short  base_mem[MLP_SIZE];
  ushort  mem_count ;

  ushort uPercentage; /* percentage of phone removed active phones */
  short *p_s_active; /* indicator of active status of monophones */
  short *p_s_cur_mono; /* score of each monophone at the current frame */

  short prevBeam; /* previous beamwidth */
}MODEL_PRUNE_type;

#endif

#ifdef USE_AVL_TREE

#define TREE_SPACE_SIZE 20000
typedef struct _TREE_SPACE{
  short  base_mem[TREE_SPACE_SIZE];
  ushort  mem_count ;

} TreeSpaceType;

struct AvlNode;
typedef short ElementType;
typedef struct AvlNode *Position;
typedef struct AvlNode *AvlTree;

struct AvlNode
{
  ElementType Element; /* the index */

  ElementType Value;   /* the value */

  /* the indices in the TreeSpaceType */
  short  Left;
  short  Right;

  short      Height;
};

#endif

#endif
