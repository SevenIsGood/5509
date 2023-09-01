/******************************************************************************
 * FILE PURPOSE: To include the header file specific to platforms for AEC.
 ******************************************************************************
 * FILE NAME:   siuport.h
 *
 * DESCRIPTION: Based on compiler switch, include platform specific header file.
 *
 *        Copyright (c) 2007 – 2013 Texas Instruments Incorporated                
 *                     ALL RIGHTS RESERVED
 *
 *****************************************************************************/

#ifndef _SIUSIMPORT_H
#define _SIUSIMPORT_H

/* System definitions and utilities */
#include <ti/mas/types/types.h>                      /* DSP types            */
#include <ti/mas/types/const.h>

#define SIU_MAX_CHANNELS      1         /* Maximum number of SIU channels */

/* Maximum PCM/linear sample frame duration and length */

#define SIU_MAX_FRAME_DURATION  10      /* 10ms @8 kHz or 20 ms at 16kHz, ie */
                                        /*   const.h const_VOX_SAMPLING_RATE */
										/*   fixed, so lie about duration    */
/* Maximum frame length in samples */
#define SIU_MAX_FRAME_LENGTH    (1UL*SIU_MAX_FRAME_DURATION*              \
                                 const_MAX_VOX_SAMPLING_RATE/1000)
#if (SIU_MAX_FRAME_LENGTH*1000UL != 1UL*SIU_MAX_FRAME_DURATION*           \
                                    const_MAX_VOX_SAMPLING_RATE)
# error invalid SIU maximum frame duration/length
#endif

/* Maximum AER tail duration and length */
#define SIU_MAX_AER_FILTER_DURATION   200      /* max tail length = 200msec */

/* AER core sampling rate */
#define SIU_aer_SAMP_RATE_CORE        8000     /* AER Core runs at 8kHz      */

/* Maximum tail length in samples */
#define SIU_MAX_AER_FILTER_LENGTH     \
       (1UL*SIU_MAX_AER_FILTER_DURATION*SIU_aer_SAMP_RATE_CORE/1000)
#if (SIU_MAX_AER_FILTER_LENGTH*1000UL != 1UL*SIU_MAX_AER_FILTER_DURATION*  \
                                         SIU_aer_SAMP_RATE_CORE)
# error invalid SIU maximum AER tail duration/length
#endif

/* programmable gain initialization - normalization hardware codec dependent */
/* for +36 dB maximum gain, linear gain = 10^((gain in dB - 36 dB)/20)       */
#define SIU_GAINR_INIT 0x207  /* Q15 receive path mechanical speaker volume  */
                              /* 0 dB gain for [-12,36] dB hardware codec    */
                              /* with max programmable gain scaled to unity  */
#define SIU_GAINS_INIT 0x207  /* Q15 send path microphone gain               */
                              /* 0 dB gain for [-12,36] dB hardware codec    */
							  /* with max programmable gain scaled to unity  */

/* Module ID's */
#define SIU_MID_SIU     0x01      /* SIU module */
#define SIU_MID_VCU     0x02      /* VCU module */
#define SIU_MID_VCA     0x03      /* VCA modules */
#define SIU_MID_PIU     0x04      /* PIU module */
#define SIU_MID_AER     0x10      /* AER module */
#define SIU_MID_AGC     0x20      /* AGC module */
#define SIU_MID_DRC     0x40      /* DRC module */
#define SIU_MID_MSS     0x80      /* MSS module */
#define SIU_MID_BF      0x100     /* BF  module */

#define SIU_DBGMSG_SIZE   256     /* Size of debug message buffer */

#endif /* _SIUSIMPORT_H */

/* Nothing past this point */


