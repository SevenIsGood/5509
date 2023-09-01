#ifndef _MSS_H
#define _MSS_H

/**
 *  @file   mss.h
 *  @brief  Contains external API for Multi-Source Selection (MSS) module.
 *
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <ti/mas/types/types.h>
#include <ti/mas/util/ecomem.h>               /* memory descriptor structure */

/* Define MSS Module as a master group in Doxygen format and add all MSS API 
   definitions to this group. */
/** @defgroup mss_module MSS Module API
 *  @{
 */
/** @} */

/** @defgroup mss_api_functions MSS Functions
 *  @ingroup mss_module
 */

/** @defgroup mss_api_structures MSS Data Structures
 *  @ingroup mss_module
 */

/** @defgroup mss_api_constants MSS Constants (enum's and define's)
 *  @ingroup mss_module
 */

/** @defgroup mss_err_code MSS Error Codes
 *  @ingroup mss_api_constants
 *  @{
 *
 *  @name MSS error codes
 *
 *  Error codes returned by MSS API functions.
 */
/*@{*/
enum {
  mss_NOERR              = 0,  /**< success, no error */
  mss_ERR_NOMEMORY       = 1,  /**< supplied memory are not enough            */
  mss_ERR_NOTOPENED      = 2,  /**< MSS instance not in opened state          */
  mss_ERR_NOTCLOSED      = 3,  /**< MSS instance not in closed state          */
  mss_ERR_INVALIDPAR     = 4,  /**< configuration parameter is invalid        */
  mss_ERR_INVALIDSRC     = 5   /**< no valid source is provided to process    */
};
/*@}*/
/** @} */

/** @defgroup mss_samp_rate MSS Sampling Rates Definitions
 *  @ingroup mss_api_constants
 *  @{
 *
 *  @name MSS sampling rates
 *
 *  Sampling rates that are supported by MSS. 
*/
/*@{*/
enum {
  mss_SAMP_RATE_8K  = 1,                   /**< 8kHz  */
  mss_SAMP_RATE_16K = 2                    /**< 16kHz */
};
typedef tint mssSampRate_t;

/*@}*/
/** @} */

/** @defgroup mss_src_type MSS Source Type Definitions
 *  @ingroup mss_api_constants
 *  @{
 *
 *  @name MSS source types
 *
 *  MSS supports multiple types of sources which can be microphones, beams, etc.
 *  Microphones can be fixed (mounted on the device) or remote, and can have 
 *  their own built-in acoustic echo canceller. 
*/
/*@{*/
enum {
  mss_SRC_INVALID     = -1,       /**< Invalid source type                    */
  mss_SRC_MIC_FIXED   =  0,       /**< Fixed microphones                      */
  mss_SRC_MIC_REMOTE  =  1,       /**< Remote microphones                     */
  mss_SRC_MIC_CLEAN   =  2,       /**< Clean microphones (with built-in AEC)  */
  mss_SRC_BEAM        =  3,       /**< Beams from microphone arrays    */
  mss_SRC_MAX         =  mss_SRC_BEAM
};

typedef tint mssSrcType_t;
#define mss_MAX_NUM_SRC_TYPES (mss_SRC_MAX+1) /**< Max # of supported source types */
/*@}*/
/** @} */

/** 
 *  @ingroup mss_api_structures
 *
 *  @brief Source structure used to manually select a source. 
 */
typedef struct mssSrc_s {
  mssSrcType_t group;    /**< source group to be manually selected:
                              valid values: mss_SRC_MIC_FIXED, etc.  */
  tint         index;    /**< group index of the source to be manually selected:
                              range: 0 - number of sources minus 1 */
} mssSrc_t;

/** 
 *  @ingroup mss_api_structures
 *
 *  @brief Configuration structure for mssGetSizes()
 *  \brief This structure is used for getting memory requirements of MSS through 
 *         function mssGetSizes().
 */
typedef struct mssSizeConfig_s {
  mssSampRate_t max_sampling_rate;      /**< Maximum sampling rate: 
                                             8kHz:  set to mss_SAMP_RATE_8K. 
                                             16kHz: set to mss_SAMP_RATE_16K. */  
  tint max_num_mic_fixed;  /**< Maximum number of fixed microphones           */
  tint max_num_mic_remote; /**< Maximum number of remote microphones          */
  tint max_num_mic_clean;  /**< Maximum number of microphones with built-in AEC */
  tint max_num_beam;       /**< Maximum number of beams from microphone array   */
} mssSizeConfig_t;

/** 
 *  @ingroup mss_api_structures
 *
 *  @brief Configuration structure for mssNew()
 *  \brief This structure is used for creating a new MSS instance through 
 *         function mssNew().
 */
typedef struct mssNewConfig_s {
  void  *handle;                                     /**< handle              */
  mssSizeConfig_t  sizeCfg;                          /**< size configration   */
} mssNewConfig_t;

/** 
 *  @ingroup mss_api_structures
 *
 *  @brief Configuration structure for mssOpen()
 *  \brief This structure contains system parameters to configure an MSS 
 *         instance through mssOpen(). MSS doesn't have default value for these
 *         parameters, and they must be set explicitly when mssOpen() is called
 *         the first time after mssNew(). 
 *  \remarks This structure doesn't contain any configurable parameters that are
 *           controlled through mssControl_s to avoid confusion.
 */
typedef struct mssConfig_s {
  mssSampRate_t sampling_rate;  /**< Sampling rate: 
                                     8kHz:  set to mss_SAMP_RATE_8K. 
                                     16kHz: set to mss_SAMP_RATE_16K.         */  
  tint  num_mic_fixed;          /**< Number of fixed microphones              */
  tint  num_mic_remote;         /**< Number of remote microphones             */
  tint  num_mic_clean;          /**< Number of microphones with built-in AEC  */
  tint  num_mic_array;          /**< Number of microphones in microphone-array*/
  tint  num_beam;               /**< Number of beams from microphone array    */
} mssConfig_t;

/** 
 *  @ingroup mss_api_structures
 *
 *  @brief MSS control bitfield structure 
 *
 *  Bitfield masks mss_CTL_MODES_XYZ may be used to change MSS operation modes
 *  through mssControl(), by constructing elements "mask" and "value" of mssModes_s.
 *
 *  @verbatim
    MSS Modes Control Bitfield: 
    ---------------------------------------
    |15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
    ---------------------------------------
       bits      values       meaning
    (from LSB) 
        0          1       MSS bypass operation - MSS always selects the 
                           first available source passed to mssProcess().               
                   0       MSS normal operation. 
        1          1       Freeze the update of MSS states - MSS always selects 
                           whatever source that was selcted before update was 
                           frozen. 
                   0       Enable the update of MSS states - normal operation.                   
 *
 */
typedef struct mssCtlBf_s {
  tuint mask;            /**< Indicates the bits to be changed.               */
  tuint value;           /**< Gives new value of each bit indicated by mask.  */
/*@{*/
#define mss_CTL_MODES_BYPASS   0x0001  /**< Bypass MSS */
#define mss_CTL_MODES_FREEZE   0x0002  /**< Freeze MSS internal states update */
#define mss_CTL_MODES_DEFAULT  0x0000  /**< Default (normal) operation modes  */
/*@}*/
} mssCtlBf_t;

/** 
 *  @ingroup mss_api_structures
 *
 *  @brief Control structure for mssControl()
 *
 *  \brief This structure is used to change MSS configurable parameters. 
 *         Element mssControl_s.valid_bitfield indicates which parameter to be
 *         changed, and other elements give the parameter values. 
 */
typedef struct mssControl_s {
    tuint      valid_bitfield;
/*@{*/
#define mss_CTL_MODES           0x0001 /**< change mssControl_s.modes         */
#define mss_CTL_SELECT_SRC      0x0002 /**< change mssControl_s.source        */
#define mss_CTL_SWITCH_THRESH   0x0004 /**< change mssControl_s.switch_thresh */
#define mss_CTL_SWITCH_DURATION 0x0008 /**< change mssControl_s.switch_duration */
#define mss_CTL_SWITCH_HNAGOVER 0x0010 /**< change mssControl_s.switch_hangover */
#define mss_CTL_ALL              (  mss_CTL_MODES \
                                  | mss_CTL_SELECT_SRC \
                                  | mss_CTL_SWITCH_THRESH \
                                  | mss_CTL_SWITCH_DURATION \
                                  | mss_CTL_SWITCH_HNAGOVER ) /**< change all */
/*@}*/   
    mssCtlBf_t modes;             /**< controls MSS operation modes */
    mssSrc_t   source;            /**< the source selected by external user   */
    tint       switch_thresh;     /**< switch threshold in msec: a source needs
                                       to meet the selection criteria for this
                                       amount of time before switch to this 
                                       source starts. Default is 250msec      */
    tint       switch_duration;   /**< switch duration in msec: it takes this
                                       amount of time to switch from one source 
                                       to another. Default is 500msec.        */
    tint       switch_hangover;   /**< switch hangover in msec: after a new 
                                       source is selected, it has to wait this 
                                       time before evaluating next possible 
                                       source switch. Default is 1000msec.    */
} mssControl_t;

/** 
 *  @ingroup mss_api_structures
 *
 *  @brief MSS debug structure 
 *
 *  \brief This structure is used to provide MSS debugging information.
 */
#define mss_DEBUG_STATE_NUM_TUINT 16 
typedef struct mssDebugStat_s {
  mssSrc_t cur_src;                                 /**< Current source */
  mssSrc_t new_src;                                 /**< Temporary new source */
  tuint    states[mss_DEBUG_STATE_NUM_TUINT];       /**< MSS internal states */
} mssDebugStat_t;

 
/**
 *  @ingroup mss_api_functions
 *
 *  @brief Function mssGetSizes() obtains from MSS the memory requirements of  
 *         an instance, which depend on provided configuration parameters.   
 *
 *  @param[in]   cfg     Pointer to a size configuration structure.
 *  @param[out]  nbufs   Memory location to store the returned number of buffers
 *                       required by the instance.
 *  @param[out]  bufs    Memory location to store the returned address of the 
 *                       vector of memory buffer descriptions required by MSS. 
 *
 *  @remark Type ecomemBuffer_t is defined in ecomem.h of util package.
 *
 *  @retval              MSS error code, always mss_NOERR.
 */
tint mssGetSizes(tint *nbufs, const ecomemBuffer_t **bufs, mssSizeConfig_t *cfg);

/**
 *  @ingroup mss_api_functions
 *
 *  @brief Function mssNew() creates a new instance of MSS.  
 *  \remark Function mssNew() must be called before mssOpen()is called.
 *
 *  @param[in]     nbufs     Number of memory buffers allocated by the user.
 *  @param[in]     bufs      Pointer to memory buffer descriptors defined by
 *                           user.
 *  \remark Buffer alignment property of each buffer passed to mssNew() must be 
 *          equal to or better than what is returned by mssGetSizes(), and must
 *          be in consistency with the base address of the buffer.
 *
 *  @param[in]     cfg       Pointer to new instance configuration structure.
 *  @param[in,out] mssInst   Memory location that will receive a pointer to 
 *                           the created MSS instance. This pointer will be  
 *                           returned by mssNew().
 *  @retval                  MSS error code. See \ref mss_err_code.
 *  @verbatim
     error code              description
     mss_NOERR               success
     mss_ERR_INVALIDPAR      *mssInst is not NULL or nbufs is not correct 
     mss_ERR_NOMEMORY        properties of certain buffer are bad:
                             - size is not zero but base address is NULL,
                             - alignment and base address are not consistent,
                             - volatility does not meet requirement.
    \endverbatim
 *
 *  @pre  The pointer at the location pointed to by mssInst must be set to NULL 
 *        before this function is called.
 *  @post A pointer to the MSS instance buffer will be returned to the location
 *        pointed to by mssInst. Instance state will be set to closed.
 */
tint mssNew (void **mssInst, tint nbufs, ecomemBuffer_t *bufs, mssNewConfig_t *cfg);

/**
 *  @ingroup mss_api_functions 
 *
 *  @brief Function mssOpen() initializes and configures an MSS instance.
 *
 *  @remark This function must be called after mssNew() to initialize a new MSS
 *          instance. It may also be called to reconfigure an instance that 
 *          has been closed by mssClose() but not deleted by mssDelete().
 *
 *  @param[in]      cfg      Pointer to MSS configuration parameter.
 *  @param[in]      mssInst  Pointer to MSS instance.
 *  @retval                  MSS error code. See \ref mss_err_code.
 *  @verbatim
     error code              description
     mss_NOERR               success
     mss_ERR_NOTCLOSED       MSS instance state has not been set to closed, i.e,
                             mssNew() or mssClose() not called for the instance.
     mss_ERR_INVALIDPAR      bad configuration parameters:
                             - cfg is NULL
                             - total number of sources is less than or equal to 0.
    \endverbatim
 *
 *  @pre  Function mssNew() must be called before mssOpen() is called the first
 *        time to open a new instance. For subsequent calls to open an existing
 *        instance, mssClose() must be called before mssOpen() to close the
 *        instance.
 *  @post All the parameters that are not part of mssConfig_t are set to 
 *        default values by mssOpen. 
 *  @post After MSS instance is opened, mssControl() and mssProcess() may 
 *        be called for control or processing.  
 *
 */
tint mssOpen (void *mssInst, mssConfig_t *cfg);

/**
 *  @ingroup mss_api_functions
 *
 *  @brief Function mssControl() changes MSS operation modes or parameters. 
 *
 *  @param[in]      mssInst   Pointer to MSS instance.
 *  @param[in,out]  ctl       Pointer to MSS control structure. 
 *
 *  @retval                   MSS error code. See \ref mss_err_code.
 *  @verbatim
     error code              description
     mss_NOERR               success
     mss_ERR_NOTOPENED       MSS instance has not been opened
     mss_ERR_INVALIDSRC      mssControl_s.source is an invalid source
    \endverbatim
 *
 */
tint mssControl (void *mssInst, mssControl_t *ctl);

/**
 *  @ingroup mss_api_functions
 *
 *  @brief Function mssProcess() examines all input sources to MSS and selects 
 *         the best one.
 *
 *  @param[in]  mssInst         Pointer to MSS instance.
 *  @param[in]  rx_out_sync     Synchronized Rx path output signal, NOT to be
 *                              selected, but to assist source
 *                              selection. (Not used)
 *  @param[in]  mic_fixed       Signals from fixed mics.
 *  @param[in]  mic_remote      Signals from remote mics.
 *  @param[in]  mic_clean       Signals from mics with own AEC.
 *  @param[in]  mic_array       Signals from mics in the mic-array.
 *  @param[in]  beam            Signals from beams formed from mic-array.
 *
 *  @param[out] out             Buffer to store the selected source signal
 *
 *  @remark All input signals and output signal are 16-bit linear samples 
 *          of 10 msec. 
 *
 *  @retval                   MSS error code. See \ref mss_err_code.
 *  @verbatim
     error code              description
     mss_NOERR               success
     mss_ERR_NOTOPENED       MSS instance has not been opened
     mss_ERR_INVALIDSRC      mssControl_s.source is an invalid source
    \endverbatim
 *
 */
tint mssProcess(void *mssInst, void *out, void *rx_out_sync, void *mic_fixed[],
         void *mic_remote[], void *mic_clean[], void *mic_array[], void *beam[]);

/**
 *  @ingroup mss_api_functions
 *
 *  @brief Function mssDebugStat() returns debug information.
 *
 *  @param[in]  mssInst         Pointer to MSS instance.
 *  @param[out] mssDbg          Debug information.
 *
 *  @retval                   MSS error code. See \ref mss_err_code.
 *  @verbatim
     error code              description
     mss_NOERR               success
     mss_ERR_NOTOPENED       MSS instance has not been opened
    \endverbatim
 *
 */         
tint mssDebugStat(void *mssInst, mssDebugStat_t *mssDbg);

/**
 *  @ingroup mss_api_functions
 *
 *  @brief Function mssClose() closes an MSS instance. 
 *  \remark Function mssClose() must be called before mssDelete() is called.
 *
 *  @param[in]  mssInst      pointer to the instance to be closed
 *  @retval                  MSS error code. See \ref mss_err_code.
 *  @verbatim
     error code              description
     mss_NOERR               success
     mss_ERR_NOTOPENED       MSS instance is not in open state
    \endverbatim
 *
 */
tint mssClose(void *mssInst);

/**
 *  @ingroup mss_api_functions
 *
 *  @brief Function mssDelete() deletes the MSS instance identified by mssInst
 *         and returns the addresses of those buffers used by this instance.  
 *
 *  @param[in]      nbufs    Number of buffers used by this instance.
 *  @param[in]      bufs     Pointer to buffer descriptors to store returned
 *                           addresses of the buffers used by this instance.
 *  @param[in,out]  mssInst  Memory location where the pointer to MSS instance
 *                           is stored.
 *  @retval                  MSS error code. See \ref mss_err_code.
 *  @verbatim
     error code              description
     mss_NOERR               success
     mss_ERR_NOTCLOSED       MSS instance has not been closed.
     mss_ERR_NOMEMORY        Not enough buffer descriptors to store all returned 
                             buffer addresses.
    \endverbatim
 *
 *  @pre  MSS instance must be closed by mssClose() before mssDelete() is called. 
 *  @post After mssDelete() is called, MSS instance pointer stored at mssInst 
          will be set to NULL, and the addresses of the buffers used by this 
          instance will be returned to the location pointed to by bufs.
 */
tint mssDelete(void **mssInst, tint nbufs, ecomemBuffer_t *bufs);


/* -------------------------- MSS Call Table -------------------------------- */
/**
 *  @ingroup mss_api_structures
 *
 *  @brief MSS call table
 *
 */
typedef struct {
  tint  (*mssClose)     (void *mssInst);
  tint  (*mssControl)   (void *mssInst, mssControl_t *ctl);
  tint  (*mssDelete)    (void **mssInst, tint nbufs, ecomemBuffer_t *bufs);
  tint  (*mssGetSizes)  (tint *nbufs, const ecomemBuffer_t **bufs,
                         mssSizeConfig_t *cfg);
  tint  (*mssNew)       (void **mssInst, tint nbufs, ecomemBuffer_t *bufs,
                         mssNewConfig_t *cfg);
  tint  (*mssOpen)      (void *mssInst, mssConfig_t *cfg);
  tint  (*mssProcess)   (void *mssInst, void *out, void *rx_out_sync, 
                         void *mic_fixed[], void *mic_remote[], void *mic_clean[], 
                         void *mic_array[], void *beam[]);                               
} mssCallTable_t;

#endif
/* Nothing past this point */

