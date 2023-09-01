/* ============================================================================
 * Copyright (c) 2017 Texas Instruments Incorporated.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/
/** @file audio_data_collection.c
 *
 *  @brief Initializes, collects and processes audio through the keyword recognition engine.
 *
 * Revision History
 * ================
 * 15th-Dec-2016 created
  * ============================================================================
 */

#include <std.h>
#include <string.h>
#include <tsk.h>
#include <swi.h>
#include <hwi.h>
#include <sem.h>
#include <clk.h>
// RTS includes
#include <stdio.h>
#include <stdlib.h>

#include "sample_rate.h"
#include "codec_pcm186x.h"

// CSL includes
#include "csl_types.h"
#include "csl_error.h"
#include "soc.h"
#include "csl_intc.h"
// Sensory includes
#include "SensoryLib.h"
#include "BF_rt_bios_cfg.h"

#ifdef PROFILE_CYCLES
#include "csl_gpt.h"
#endif

#ifdef PROFILE_CYCLES
typedef struct cycles_s {
  long     max; /**< running maximum of cycles */
  long     avg; /**< running average of cycles (using exponential averaging)*/
} cycles_t;

extern CSL_Handle  hGpt;
extern CSL_Config  hwConfig;
CSL_Status 	       statusInt = 0;
Uint32 		       timeCntInt1= 0;
Uint32 		       timeCntInt2 = 0;
cycles_t		   cyclesDmaInt = {0, 0};
static inline void process_cycles(long cycles, cycles_t * cyclesPtr)
{
  if (cycles > cyclesPtr->max) {
    cyclesPtr->max = cycles;
  }

  cyclesPtr->avg = ((cyclesPtr->avg) * 255 + cycles) >> 8;
}
#endif

/* Global instance of the appStructure */
appStruct_T appStruct;
appStruct_T *ap;
t2siStruct *t;

/* Initializes Voice Recognition*/
void SensoryRecogInit(Void);
void 	waitLoop()   ;

Int16 g_initcount=0;
Int16 g_recogcount=0;
Bool gEndRecog = TRUE;

// Rx circular buffer
#define NUM_SAMP_PER_MS             ( SAMPS_PER_MSEC_16KHZ )  // samples per msec
#define NUM_MS_PER_FRAME            ( 10 )  // msec per frame
#define RX_FRAME_LEN                ( NUM_MS_PER_FRAME * NUM_SAMP_PER_MS )  // frame length in samples


#define NUM_MS_PER_FRAME            ( 10 )  // msec per frame
#define RX_CIRCBUF_NUM_FRAMES       ( 10 )  // 10 frames in Rx circular buffer
#define RX_CIRCBUF_LEN              ( RX_FRAME_LEN * RX_CIRCBUF_NUM_FRAMES)  // Rx circular buffer length

Int16 gRxCircBuf[RX_CIRCBUF_LEN];

Int16 gRxCircBuf[RX_CIRCBUF_LEN];
Uint16 gRxCircBufInFrame;   // circular buffer input frame
Uint16 gRxCircBufOutFrame;  // circular buffer output frame
Uint32 gRxCircBufOvrCnt;    // circular buffer overflow count
Uint32 gRxCircBufUndCnt;    // circular buffer underflow count
Int16 gRxPingPongBufSel;


#define TRIGGER_PHRASE          ( "Hello Blue Genie" )
extern Int SEG0;

#include "deepApphbg_snrINF10_dur_v40.h"
#define CURRENT_GRAMMAR gs_hbg_snrINF10_dur_v40
#define CURRENT_NET     nn_en_us_adults

SAMPLE audioBuffer[AUDIO_BUFFER_LEN] = {0};  // Note: this could be malloc'd

/* Resets Rx circular buffer */
void resetRxCircBuf(
    Int16 *rxCircBuf,
    Uint16 rxCircBufLen,
    Uint16 *pRxCircBufInFrame,
    Uint16 *pRxCircBufOutFrame,
    Uint32 *pRxCircBufOvrCnt,
    Uint32 *pRxCircBufUndCnt
)
{
    memset(rxCircBuf, 0, rxCircBufLen);
    *pRxCircBufOutFrame = 0;
    *pRxCircBufInFrame = *pRxCircBufOutFrame + 1;
    *pRxCircBufOvrCnt = 0;
    *pRxCircBufUndCnt = 0;
}

void SensoryRecogInit(Void)
{
    unsigned int size=0;
    infoStruct_T isp;
    appStruct_T *ap = &appStruct;
    t2siStruct *t = &ap->_t;

    memset(t, 0, sizeof(t2siStruct));

    /* Reset Rx circular buffer */
    resetRxCircBuf(gRxCircBuf, RX_CIRCBUF_LEN, &gRxCircBufInFrame, &gRxCircBufOutFrame, &gRxCircBufOvrCnt, &gRxCircBufUndCnt);


    /* Set up recognition control parameters in the t2siStruct.
     * NOTE: t->net, gram, maxResults and maxTokens affect the amount of memory needed
     * (which will be calculated by the call to SensoryAlloc). */

    t->net  = (u32)&CURRENT_NET;
    t->gram = (u32)&CURRENT_GRAMMAR;
    t->sdet_type = SDET_NONE;
    t->knob = T2SI_DEFAULT_KNOB;
    t->paramAOffset = 0;
    t->noVerify = 0;
    t->maxResults = MAX_RESULTS;
    t->maxTokens  = MAX_TOKENS;
    t->keepGoing=0;     // except for testing, set to 0
    t->timeout = 0;     // use default

    /* Initialize audio buffer items */
    ap->audioBufferLen = AUDIO_BUFFER_LEN;
    ap->audioBufferStart = &audioBuffer[0];
    ap->audioPutIndex = ap->audioGetIndex = ap->lwsdGetIndex = 0;
    ap->audioGetFrameCounter = 0;
    ap->lwsdGetFrameCounter = 0;
    ap->audioFilledCount = 0;

    /* Initialize Sound Detector (OK to do this even for SDET_NONE) */
    SensoryLWSDInit(ap);

    /* Optionally get the version (or other info) */
    SensoryInfo(&isp);

    /* Find out how much persistent memory we need */
    SensoryAlloc(&appStruct, &size);

    // debug -- check used heap
    //status = MEM_stat(SEG0, &memStatBuf);

    /* Open an instance of the engine, loading the grammar */
    t->spp = (void *)MEM_alloc(SEG0, size * sizeof(short), 0);
    if (t->spp == MEM_ILLEGAL)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: Unable to allocate Sensory memory");
#endif
        exit(EXIT_FAILURE);
    }

    // debug -- check used heap
    //status = MEM_stat(SEG0, &memStatBuf);

    gEndRecog = TRUE;

}

Void RecognizerFxnLocalTsk(Void)
{
    Uint16 numFrames;
    Int16 rxCbNumFrames;
    Uint16 frameCnt;
    errors_t error;

   while (1)
    {
        SEM_pend(&SEM_DoneAudioCollection, SYS_FOREVER);
    //swiRecognizerFxnCnt++; // debug
#ifdef PROFILE_CYCLES
      statusInt = GPT_getCnt(hGpt, &timeCntInt1);;
#endif

    numFrames = SEM_count(&SEM_DoneAudioCollection)+1;  /* get number of audio frames collected since last recognizer execution */

	TSK_disable();

    rxCbNumFrames = gRxCircBufInFrame - gRxCircBufOutFrame;
    TSK_enable();

    if (rxCbNumFrames < 0)
    {
        rxCbNumFrames += RX_CIRCBUF_NUM_FRAMES;
    }
    //LOG_printf(&trace, "REC-TSK: %d calls, %d cfrms", numFrames, rxCbNumFrames);

    if (rxCbNumFrames < numFrames)
    {
        gRxCircBufUndCnt++;
        //LOG_printf(&trace, "ERROR: Rx circular buffer underflow");
    }
    //LOG_printf(&trace, "0x%04x 0x%04x", gRxCircBufInFrame, gRxCircBufOutFrame);
    //LOG_printf(&trace, "%d %d", rxCbNumFrames, numFrames);
    //if (numFrames > 1) LOG_printf(&trace, "%d %d", rxCbNumFrames, numFrames);

    frameCnt = 0;
    while ((frameCnt < numFrames))
    {
        if (gEndRecog == TRUE)
        {
            gEndRecog = FALSE;

            error = SensoryProcessInit(&appStruct);

             if (error) {
               printf("SensoryProcessInit failed with 0x%lx\n", error);
               exit(-1);
            }

            if (error != ERR_OK)
            {
//#ifdef DEBUG_LOG_PRINT
               // LOG_printf(&trace, "ERROR: SensoryProcessInit()=%d", (u16)error);
//#endif
            }
        }

        while ((frameCnt < numFrames) && (gEndRecog == FALSE))
        {

          error = (u16)SensoryProcessData(&gRxCircBuf[gRxCircBufOutFrame*RX_FRAME_LEN], &appStruct);  // keep sending these bricks


#ifdef _DBG_ERR_LOGS // debug
            if (gErrIdx < NUM_ERR)
            {
                gErrType[gErrIdx++] = error;
            }
#endif

            if (t->outOfMemory)
            {
            	//LOG_printf(&trace, "out of memory !! \n");
              	t->outOfMemory = 0;
            }

            if (error == ERR_OK)
            {
                gEndRecog = TRUE;

                if (appStruct._t.wordID)
                {
                  //  LOG_printf(&trace, "Word recognized !! \n");
                  // LOG_printf(&trace, "Recognition: wordID = %d ", appStruct._t.wordID);
                    g_recogcount++;


                    SEM_post(&SEM_BlinkXF); //EVM5517 XF LED blink or C5545BP OLED operation that prints Hello Blue Genie

                }
                else
                {

                 //   LOG_printf(&trace, "ERROR: SensoryProcessData()=%d, not recognized", (u16)error);

                }
            }
            else if (error == ERR_RECOG_SV_FAILED)
            {
                gEndRecog = TRUE;

               // LOG_printf(&trace, "ERROR: SensoryProcessData()=%d, WS pass, SV fail, wordID=%d", (u16)error, t->wordID);

            }
            else if (error != ERR_NOT_FINISHED)
            {
                gEndRecog = TRUE;

               // LOG_printf(&trace, "ERROR: SensoryProcessData()=%d", (u16)error);

            }

            gRxCircBufOutFrame++;
            if (gRxCircBufOutFrame >= RX_CIRCBUF_NUM_FRAMES)
            {
                gRxCircBufOutFrame = 0;
            }

            frameCnt++;
          }
       }

#ifdef PROFILE_CYCLES
	  statusInt = GPT_getCnt(hGpt, &timeCntInt2);
      process_cycles((timeCntInt1-timeCntInt2)<<(hwConfig.preScaleDiv+1), &cyclesDmaInt);
#endif
   }
}



void AudioDataCollection(Int16 *aerSendInBuf)
{
    Int16 rxCbNumFrames;
    Uint16 offset, i;

    //LOG_printf(&trace, "AU-GET, %lu" , CLK_gethtime());
   // LOG_printf(&trace, "AU-GET");


        /* Check Rx circular buffer overflow */
        rxCbNumFrames = gRxCircBufInFrame - gRxCircBufOutFrame;
        if (rxCbNumFrames < 0)
        {
            rxCbNumFrames += RX_CIRCBUF_NUM_FRAMES;
        }
        if (rxCbNumFrames > (RX_CIRCBUF_NUM_FRAMES-1))
        {
            gRxCircBufOvrCnt++;
        }

        /* Determine which frame to use ping or pong */
        offset = gRxPingPongBufSel * RX_FRAME_LEN;
        gRxPingPongBufSel ^= 1; /* toggle ping/pong flag */

        /* Copy current frame from ping or pong buffer into Rx circular buffer SWI_Recognizer*/
        for (i=0; i<RX_FRAME_LEN; i++)
        {

            gRxCircBuf[gRxCircBufInFrame*RX_FRAME_LEN + i] = *aerSendInBuf++;
        }

        gRxCircBufInFrame++;
        if (gRxCircBufInFrame >= RX_CIRCBUF_NUM_FRAMES)
        {
            gRxCircBufInFrame = 0;
        }

        SEM_post(&SEM_DoneAudioCollection);
}
