/*
* recognizer.c
*
*
* Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/ 
* 
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

// RTS includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// CSL includes
#include "csl_gpio.h"
#include "csl_intc.h"
// DSP/BIOS includes
#include <std.h>
#include "TIesrDemoC55cfg.h"
// TIesr includes
#include "TIesr_engine_api_sireco.h"
// application includes
#include "app_globals.h"
#include "gpio_control.h"
#include "lcd_osd.h"
#include "audio_data_collection.h"
#include "user_interface.h"
#include "recognizer.h"

TIesrEngineSIRECOType tiesr;
TIesr_t tiesrInstance;

Bool gEndRecog;

extern pI2cHandle    i2cHandle;

//Uint32 swiRecognizerFxnCnt=0; // debug

// Debug
//#define _DBG_TIESR_ERR_LOGS

#ifdef _DBG_TIESR_ERR_LOGS
#define NUM_TIESR_ERR ( 100 )

typedef enum
{
    OpenSearchEngineErr,
    CallSearchEngineErr, 
    CloseSearchEngineErr,
    JacUpdateErr
} TIesrErrType;

TIesrErrType gTIesrErrType[NUM_TIESR_ERR];
Uint16 gTIesrStat[NUM_TIESR_ERR] = {eTIesrEngineSuccess};
Uint16 gTIesrStatIdx=0;
#endif /* _DBG_TIESR_ERR_LOGS */


/*
 *  ======== swi_RecognizerFxn ========
 */
Void swi_RecognizerFxn(Void)
{
    Uint16 numFrames;
    Int16 rxCbNumFrames;
    Uint16 frameCnt;
    Uint16 numWords;
    Uint16 wordCnt;
    const Uint16 *wordIdx;
    const char *word;
    TIesrEngineStatusType tiesrStatus;
    TIesrEngineJACStatusType jacStatus;

    //swiRecognizerFxnCnt++; // debug

    numFrames = SWI_getmbox();  /* get number of audio frames collected since last recognizer execution */
    //LOG_printf(&trace, "%d", numFrames);

    // Check Rx circular buffer underflow
    SWI_disable();
    rxCbNumFrames = gRxCircBufInFrame - gRxCircBufOutFrame;
    SWI_enable();
    if (rxCbNumFrames < 0)
    {
        rxCbNumFrames += RX_CIRCBUF_NUM_FRAMES;
    }
    if (rxCbNumFrames < numFrames)
    {
        gRxCircBufUndCnt++;
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: Rx circular buffer underflow");
#endif
    }
    //LOG_printf(&trace, "0x%04x 0x%04x", gRxCircBufInFrame, gRxCircBufOutFrame);
    //LOG_printf(&trace, "%d %d", rxCbNumFrames, numFrames);
    //if (numFrames > 1) LOG_printf(&trace, "%d %d", rxCbNumFrames, numFrames);

    frameCnt = 0;
#ifdef C5535_EZDSP
    while ((frameCnt < numFrames) && (gUcStates.recognizerActive == 1))
#else
    	while ((frameCnt < numFrames))
#endif
    {
        if (gEndRecog == TRUE)
        {
            gEndRecog = FALSE;

            tiesrStatus = tiesr.OpenSearchEngine(tiesrInstance);
            if(tiesrStatus != eTIesrEngineSuccess)
            {
#ifdef DEBUG_LOG_PRINT
                LOG_printf(&trace, "ERROR: Unable to open TIesr engine instance %d", tiesrStatus);
#endif

#ifdef _DBG_TIESR_ERR_LOGS
                if (gTIesrStatIdx < NUM_TIESR_ERR)
                {
#ifdef PF_C5535_EZDSP
                    GPIO_write(hGpio, CSL_GPIO_PIN15, 0); // light yellow LED
#endif
                    gTIesrStat[gTIesrStatIdx] = tiesrStatus;
                    gTIesrErrType[gTIesrStatIdx++] = OpenSearchEngineErr;
                }
                else
                {
#ifdef PF_C5535_EZDSP
                    GPIO_write(hGpio, CSL_GPIO_PIN16, 0); // light red LED
#endif
                    //I2S_transEnable(hI2s, FALSE);   // disable I2S
                    //DMA_stop(hDmaRx);               // stop left channel DMA
                    //IRQ_clear(DMA_EVENT);           // clear pending DMA events
                    //while(1);
                }
#endif /* _DBG_TIESR_ERR_LOGS */

            }
        }

#ifdef C5535_EZDSP
        while ((frameCnt < numFrames) && (gEndRecog == FALSE) && (gUcStates.recognizerActive == 1))
#else
        while ((frameCnt < numFrames) && (gEndRecog == FALSE))
#endif
        {
            //CSL_CPU_REGS->ST1_55 ^= (1<<CSL_CPU_ST1_55_XF_SHIFT); // toggle XF -- debug

            tiesrStatus = tiesr.CallSearchEngine(&gRxCircBuf[gRxCircBufOutFrame*RX_FRAME_LEN], tiesrInstance, 0, TRUE);
            if (tiesrStatus != eTIesrEngineSuccess)
            {
#ifdef DEBUG_LOG_PRINT
                LOG_printf(&trace, "ERROR: TIesr search engine failure %d", tiesrStatus);
#endif

#ifdef _DBG_TIESR_ERR_LOGS
                if (gTIesrStatIdx < NUM_TIESR_ERR)
                {
#ifdef PF_C5535_EZDSP
                    GPIO_write(hGpio, CSL_GPIO_PIN15, 0); // light yellow LED
#endif
                    gTIesrStat[gTIesrStatIdx] = tiesrStatus;
                    gTIesrErrType[gTIesrStatIdx++] = CallSearchEngineErr;
                }
                else
                {
#ifdef PF_C5535_EZDSP
                    GPIO_write(hGpio, CSL_GPIO_PIN16, 0); // light red LED
#endif
                    //I2S_transEnable(hI2s, FALSE);   // disable I2S
                    //DMA_stop(hDmaRx);               // stop left channel DMA
                    //IRQ_clear(DMA_EVENT);           // clear pending DMA events
                    //while(1);
                }
#endif /* _DBG_TIESR_ERR_LOGS */

            }

            gEndRecog = tiesr.SpeechEnded(tiesrInstance);

            gRxCircBufOutFrame++;
            if (gRxCircBufOutFrame >= RX_CIRCBUF_NUM_FRAMES)
            {
                gRxCircBufOutFrame = 0;
            }

            frameCnt++;
        }

#ifdef C5535_EZDSP
        if ((gEndRecog == TRUE) && (gUcStates.recognizerActive == 1))
#else
        if ((gEndRecog == TRUE))
#endif
        {
            tiesrStatus = tiesr.CloseSearchEngine(tiesrStatus, tiesrInstance);

            if (tiesrStatus == eTIesrEngineSuccess)
            {
                jacStatus = tiesr.JAC_update(tiesrInstance);
                if (jacStatus != eTIesrEngineJACSuccess)
                {
#ifdef DEBUG_LOG_PRINT
                    LOG_printf(&trace, "ERROR: Unable to update JAC %d", jacStatus);
#endif

#ifdef _DBG_TIESR_ERR_LOGS
                    if (gTIesrStatIdx < NUM_TIESR_ERR)
                    {
#ifdef PF_C5535_EZDSP
                        GPIO_write(hGpio, CSL_GPIO_PIN15, 0); // light yellow LED
#endif
                        gTIesrStat[gTIesrStatIdx] = jacStatus;
                        gTIesrErrType[gTIesrStatIdx++] = JacUpdateErr;
                    }
                    else
                    {
#ifdef PF_C5535_EZDSP
                        GPIO_write(hGpio, CSL_GPIO_PIN16, 0); // light red LED
#endif
                        //I2S_transEnable(hI2s, FALSE);   // disable I2S
                        //DMA_stop(hDmaRx);               // stop left channel DMA
                        //IRQ_clear(DMA_EVENT);           // clear pending DMA events
                        //while(1);
                    }
#endif /* _DBG_TIESR_ERR_LOGS */

                }

#ifdef PF_C5535_EZDSP
                /* Clear OLED LCD */
                clear_lcd_page(0, i2cHandle);
#endif

                /* Get TIesr answer count */
                numWords = tiesr.GetAnswerCount(tiesrInstance, 0);
                //LOG_printf(&trace, "nw=%u", numWords);

                /* Get TIesr answer indices */
                wordIdx = tiesr.GetAnswerIndex(tiesrInstance, 0);

                for (wordCnt = numWords; wordCnt >= 1; wordCnt--)
                {
                    //LOG_printf(&trace, "%u", wordIdx[wordCnt-1]);
                    if ((wordIdx[wordCnt-1] != _SIL_IDX) && (wordIdx[wordCnt-1] != _FILL_IDX))
                    {
                        /* Get TIesr answer word */
                        //word = tiesr.GetAnswerWord(numWords-1-wordCnt+1, tiesrInstance, 0);
                        word = tiesr.GetAnswerWord(numWords-wordCnt, tiesrInstance, 0);

#ifdef PF_C5535_EZDSP
                        /* Output answer to OLED LCD */
                        print_string((char *)word, i2cHandle);
                        print_string(" ", i2cHandle);
#endif
                    }
                }
            }
#ifdef _DBG_TIESR_ERR_LOGS
            else
            {
                if (gTIesrStatIdx < NUM_TIESR_ERR)
                {
#ifdef PF_C5535_EZDSP
                    GPIO_write(hGpio, CSL_GPIO_PIN15, 0); // light yellow LED
#endif
                    gTIesrStat[gTIesrStatIdx] = tiesrStatus;
                    gTIesrErrType[gTIesrStatIdx++] = CloseSearchEngineErr;
                }
                else
                {
#ifdef PF_C5535_EZDSP
                    GPIO_write(hGpio, CSL_GPIO_PIN16, 0); // light red LED
#endif
                    //I2S_transEnable(hI2s, FALSE);   // disable I2S
                    //DMA_stop(hDmaRx);               // stop left channel DMA
                    //IRQ_clear(DMA_EVENT);           // clear pending DMA events
                    //while(1);
                }
            }
#endif /* _DBG_TIESR_ERR_LOGS */

        }
    }
}
