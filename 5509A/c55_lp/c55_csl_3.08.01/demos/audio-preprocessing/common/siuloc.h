#ifndef _SIULOC_H
#define _SIULOC_H 
/******************************************************************************
 * FILE PURPOSE: Local definitions and API for SIU.
 ******************************************************************************
 * FILE NAME:   siuloc.h
 *
 * DESCRIPTION: Contains local SIU definitions and API.
 *
 *        Copyright (c) 2007 – 2013 Texas Instruments Incorporated                
 *                     ALL RIGHTS RESERVED
 *
 *****************************************************************************/

/* System definitions and utilities */
#include <ti/mas/types/types.h>                       /* DSP types            */

#include <ti/mas/aer/test/siusim/siu.h>        /* required for siuTxSendOut_t */
#include <ti/mas/aer/test/siusim/siuagc.h>
#include <ti/mas/aer/test/aersim/aersimcfg.h>

#define AER_HES_ID 0   /* for handset/headset instance */
#define AER_HF0_ID 1   /* for handsfree instance 0, and add x for instance x  */
#define AER_HFMAX_ID 15 /* maximum ID for 4 bits */

/* Useful macros */
#define siuMakeID(mid,chnum)    ((tuint)(mid)<<8|(tuint)(chnum))
#define siuExtrModID(id)        ((tuint)(((id)>>8) & 0xf0))
#define siuExtrMicID(id)        ((tuint)(((id)>>8) & 0xf))
#define siuExtrChID(id)         ((id) & 0xff)

/*----------------------------*/
/* Exception and Debug Macros */
/*----------------------------*/

#define _SIUSTR(x)   __SIUSTR(x)
#define __SIUSTR(x)  #x
/* Assert macro for the exceptions */
#define siu_exc_assert(expr,inst) {                     \
  ((expr) ? ((void)0) :                             \
            (*(inst)->exception)((inst)->ID,        \
              __FILE__ "(" _SIUSTR(__LINE__) ")")); \
}

/* Debug macro  */
#define siu_dbg_out(inst,s) {  (*(inst)->debug)((inst)->ID,(s));  }

/******************************************************************************
 * DATA DEFINITION: SIU Global Context.
 ******************************************************************************
 * DESCRIPTION: Contains state information for SIU module.
 *
 *****************************************************************************/

typedef struct {
  tuint   ID;                                 /* SIU Module ID (initialized in
                                               *  siuInit() */
  void    (*exception)(tuint id, char *str);  /* master exception function
                                               *  (initialized in siuInit() */
  void    (*debug)(tuint id, char *str);      /* master debug function
                                               *  (initialized in siuInit() */
  tuint    cheap_used;   /* # words of core heap used outside siuInst_t */
  void    *cheap;       /* core heap handle */
  tuint    vheap_used;   /* # words of voice heap used outside siuInst_t */
  void    *vheap;       /* voice heap handle */
} siuContext_t;

/******************************************************************************
 * DATA DEFINITION: SIU Tx Task Instance.
 ******************************************************************************
 * DESCRIPTION: Transmit task instance.
 *
 *****************************************************************************/

typedef struct {
  siuTxSendOut_t  TxSendOut;            /* send-out object                   */
  linSample       *send_frame_ptr;      /* a pointer to send frame           */
  linSample       *recv_frame_ptr;      /* a pointer to receive frame        */
  tint            send_out_frame_length;/* # samples in send out frame       */
  tint            send_in_frame_length; /* # samples in send in frame        */
  linSample       send_frm_del[AER_MAX_FRAME_LENGTH];
  tbool           enabled;              /* TRUE: task works, FALSE: doesn't  */
} siuTxInst_t;

typedef struct {
  tint            rin_frm_len;    /* # samples in receive frame        */
  tint            rout_frm_len;    /* # samples in receive frame        */
} siuRxInst_t;

typedef struct {
  tuint           phone_mode;
  tint            mic_proc_order[AER_SIM_NUM_HF_MIC];
  tint            mic_delay[AER_SIM_NUM_HF_MIC];
  tint            mic_scale[AER_SIM_NUM_HF_MIC];
  tint            num_HF_mic;
  tint            active_mic;
  tint            mic_active_time;  /* simulate switching HF microphones */
  tint            round_robin_cntr;
  tint            inactive_highload_mic;
}siuPhoneSys_t;

/******************************************************************************
 * DATA DEFINITION: SIU Instance.
 ******************************************************************************
 * DESCRIPTION: Contains state information for one SIU channel.
 *
 *****************************************************************************/

typedef struct {
  tuint ID;                                 /* instance ID (initialized in
                                             *  siuInit() */
  void  (*exception)(tuint id, char *str);  /* exception function (initialized
                                             *  in siuInit() */
  void  (*debug)(tuint id, char *str);      /* debug function (initialized
                                             *  in siuInit() */
  /* Channel information */
  siuTxInst_t TxInst;      /* Tx Task Instance */
  siuRxInst_t RxInst;      /* Rx task instance */
  siuPhoneSys_t phone_sys; /* system information of the phone */
  tuint  cheap_used;       /* # words allocated from core heap on this channel */
  tuint  vheap_used;       /* # words allocated from voice heap on this channel */
  tint  companding_law;   /* Provides information on which type of companding
                           *  is used before linear samples are obtained. Valid
                           *  values are const_COMP_ALAW_8, const_COMP_MULAW_8,
                           *  const_COMP_LINEAR (no companding, the number of
                           *  bits in a linear sample has to be provided
                           *  somewhere.) Refers to physical I/O used. */
  tint  pcm_bits;         /* # of bits in a PCM sample. (8 for u-law, A-law,
                           *  and transparent mode, [1,16] range for linear) */
  linSample *aer_recv_in_buf;   /* Receive-in buffer for AER (receive-out for
                                 * PIU. Should be part of PIU, not SIU!!! */
  tint  tx_ag_chg_delay_frm;
  tint  rx_ag_chg_delay_frm;
  tint  tx_ag_chg_delay_cnt;
  tint  rx_ag_chg_delay_cnt;
  Fract spklin_gain;
  Fract analog_micgainlin;            /* dB in Q4 */
  Fract digital_micgainlin;           /* dB in Q4 */
  tbool pending_gain_request[AER_SIM_NUM_HF_MIC];
  siuGainControl_t gain_ctl;
  void  *piuInst;         /* a pointer to PIU instance */
  void  *aerHESInst;      /* a pointer to AER HES/HS instance */
  void  *agcHESInst;      /* a pointer to AGC HES/HS instance */
  void  *aerHFInst[AER_SIM_NUM_HF_MIC]; /* AER HF instances */
  void  *agcHFInst[AER_SIM_NUM_HF_MIC];   /* a pointer to AGC instance */
  void  *aerHEShandle;
  void  *aerHFhandles[AER_SIM_NUM_HF_MIC];
  void  *agcHEShandle;
  void  *agcHFhandles[AER_SIM_NUM_HF_MIC];
  void  *drcInst;
  void  *veuInst;
  void  *mssInst;
  void  *bfInst; 
} siuInst_t;


/* External references local to SIU */
extern siuContext_t siuContext;         /* SIU Global Context (siu.c) */
extern siuInst_t    siuInst[];          /* SIU instances (siu.c) */

/* External functions local to SIU (siu.c) */
void  siuDebug      (tuint id, char *s);
void  siuException  (tuint id, char *s);
void siuDbgAer(void *handle, tuint msg, tuint code, tuint unused1, tuint *unused2);
void siuDbgAgc(void *handle, tuint msg, tuint code, tuint unused1, tuint *unused2);
void siuDbgDrc(void *handle, tuint msg, tuint code, tuint unused1, tuint *unused2);
void siu_drc_process (void *drcInst, void *sin, void *sout);

/* siumgr.c */
void siu_new_piu  (tint chnum, tint max_frame_length);
void siu_open_piu (tint chnum, tint segment_size, tint frame_size,
                   tint recv_out_length, tint samp_rate);
void siu_open_tx  (tint chnum, tint num_HF_mic, tint frame_in_size, tint frame_out_size);
void siu_open_rx (tint chnum, tint frame_size_in, tint frame_size_out);
void siuRxOpen    (siuInst_t *siuInst, siuRxConfig_t *cfg);
void siuTxOpen    (siuInst_t *siuInst, siuTxConfig_t *cfg);
void siuTxSendIn  (siuInst_t *siuInst, linSample *send_frame, linSample *recv_frame);
void siuReceiveIn(siuInst_t *inst, linSample *rxin, linSample *rxout, 
                  tuint *trace, tint num_mic);
void siuSendIn    (siuInst_t *siuInst, linSample *rx_sync, 
                   linSample txin[][AER_SIM_MAX_FRAME_LENGTH], linSample *txout, 
				   tuint *trace, tint num_mic, tint mic_type, tint frm_len);
void siu_new_agc    (siuInst_t *inst, tint num_HF_mic); 
void siu_open_agc   (siuInst_t *inst, tint sampling_rates, tint num_mic);
void siu_close_agc  (siuInst_t *inst);
void siu_new_mms(siuInst_t *inst, tint num_mic, tint samp_rates);
void siu_mmic_control(siuInst_t *inst);
void siu_mmic_init(siuInst_t *inst);
void siu_print_buff_usage(const ecomemBuffer_t *bufs, tint num_bufs);
void siu_mmic_proc_load_control(siuInst_t *inst, tint num_mic);
void siu_put_aer_low_load(void *aer_inst);
void siu_put_aer_high_load(void *aer_inst);


#endif  /* _SIULOC_H */
/* nothing past this point */
