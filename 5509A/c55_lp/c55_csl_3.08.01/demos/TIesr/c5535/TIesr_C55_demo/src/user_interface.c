/*
* user_interface.c
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
#include <string.h>
// DSP/BIOS includes
#include <std.h>
#include "TIesrDemoC55cfg.h"
// CSL includes
#include "csl_intc.h"
#include "csl_sar.h"
#include "csl_gpt.h"
// application includes
#include "app_globals.h"
#include "pll_control.h"
#include "audio_data_collection.h"
#include "user_interface.h"

#define SAR_MAX_CLK_FREQ        ( 2000000.0 ) /* SAR max. clock freq. = 2 MHz  */
#define SAR_MEAS_CH             ( CSL_SAR_CHAN_3 ) /* SAR measurement channel */

#ifdef PF_C5515_EVM
/* SAR data range to push-button network mapping */
const SarDataRng2PbnStateObj gSarData2PbnState[UI_NUM_PBN_STATES] = 
{
    {SAR_DATA_RNG_EP00, UI_PUSH_BUTTON_NONE},
    {SAR_DATA_RNG_EP01, UI_PUSH_BUTTON_SW06},
    {SAR_DATA_RNG_EP02, UI_PUSH_BUTTON_SW08},
    {SAR_DATA_RNG_EP03, UI_PUSH_BUTTON_SW14},
    {SAR_DATA_RNG_EP04, UI_PUSH_BUTTON_SW07},
    {SAR_DATA_RNG_EP05, UI_PUSH_BUTTON_SW09},
    {SAR_DATA_RNG_EP06, UI_PUSH_BUTTON_SW10},
    {SAR_DATA_RNG_EP07, UI_PUSH_BUTTON_SW11},
    {SAR_DATA_RNG_EP08, UI_PUSH_BUTTON_SW12},
    {SAR_DATA_RNG_EP09, UI_RECOGNIZER_TOGGLE},
    {SAR_DATA_RNG_EP10, UI_PUSH_BUTTON_SW13}
};

#elif defined(PF_C5535_EZDSP)
/* SAR data range to push-button network mapping */
const SarDataRng2PbnStateObj gSarData2PbnState[UI_NUM_PBN_STATES] = 
{
    {SAR_DATA_RNG_EP00, UI_PUSH_BUTTON_NONE},
    {SAR_DATA_RNG_EP01, UI_PUSH_BUTTON_SW01},
    {SAR_DATA_RNG_EP02, UI_RECOGNIZER_TOGGLE}
};

#else
#warn "Unsupported platform"

#endif

/* GPT object/handle for SAR sampling for user interface */
static CSL_GptObj gsGptObj;
static CSL_Handle gsGptHandle;
/* SAR object/handle for SAR sampling for user interface */
static CSL_SarHandleObj gsSarObj;
static SAR_Handle gsSarHandle;

/* Momentary control states */
UI_McStatesObj gMcStates;

/* Call control states */
UI_UcStatesObj gUcStates;

static UI_PBNState gPrevPbnState;  /* previous push-button network state */

/* Allowed push-button network states */
/* Note multiple button presses not allowed */
const UI_PBNState gAllowedPbnStates[UI_NUM_ALLOWED_PBN_STATES] = 
{
    UI_PUSH_BUTTON_NONE, 
    UI_RECOGNIZER_TOGGLE
};

/* Initializes user interface */
Int16 userInterfaceInit(
     Float pbnSampFreq,
     CSL_Status *pCslStatus
)
{
    Float cpuFreq;
    CSL_SarChSetup SarParam;
    CSL_Config hwConfig;
    Uint32 tmrPrd;
    CSL_Status status;

    /* Initialize SAR CSL */
    SAR_init();

    /* Populate SAR object information */
    status = SAR_chanOpen(&gsSarObj, SAR_MEAS_CH);
    if (status != CSL_SOK)
    {
        *pCslStatus = status;
        return UI_CSL_FAIL;
    }
    gsSarHandle = &gsSarObj;

    cpuFreq = (Float)appGetSysClk();

    /* Initialize SAR registers */
    SarParam.SysClkDiv = (Uint16)((Float)cpuFreq/SAR_MAX_CLK_FREQ + 0.5)-1;
    SarParam.OpMode = CSL_SAR_POLLING;
    SarParam.MultiCh = CSL_SAR_NO_DISCHARGE;
    SarParam.RefVoltage = CSL_SAR_REF_VIN;
    status = SAR_chanSetup(gsSarHandle, &SarParam);
    if (status != CSL_SOK)
    {
        *pCslStatus = status;
        return UI_CSL_FAIL;
    }

    /* Initialize A/D conversion type */
    status = SAR_chanCycSet(gsSarHandle, CSL_SAR_SINGLE_CONVERSION);
    if (status != CSL_SOK)
    {
        *pCslStatus = status;
        return UI_CSL_FAIL;
    }

    /* Initialize UI control variables */
    gPrevPbnState = UI_PUSH_BUTTON_NONE;
    /* Initialize momentary and user control states */
    initUcStates(&gMcStates, &gUcStates);

    /* Open GPT1 -- used as timer for SAR sampling for HID */
    gsGptHandle = GPT_open(GPT_1, &gsGptObj, &status);
    if (status != CSL_SOK)
    {
        *pCslStatus = status;
        return UI_CSL_FAIL;
    }

    /* Reset GPT1 */
    status = GPT_reset(gsGptHandle);
    if(status != CSL_SOK)
    {
        *pCslStatus = status;
        return UI_CSL_FAIL;
    }

    /* Configure GPT1 */
    tmrPrd = (Uint32)((Float)cpuFreq/2.0/pbnSampFreq + 0.5)-1;
    hwConfig.autoLoad = GPT_AUTO_ENABLE;
    hwConfig.ctrlTim = GPT_TIMER_ENABLE;
    hwConfig.preScaleDiv = GPT_PRE_SC_DIV_0;
    hwConfig.prdLow = (Uint16)tmrPrd;
    hwConfig.prdHigh = (Uint16)(tmrPrd>>16);
    status = GPT_config(gsGptHandle, &hwConfig);
    if (status != CSL_SOK)
    {
        *pCslStatus = status;
        return UI_CSL_FAIL;
    }

    /* Clear pending interrupts */
    CSL_SYSCTRL_REGS->TIAFR = 0x7;

    /* Enable timer interrupt */
    IRQ_enable(TINT_EVENT);

    return UI_SOK;
}

/* Starts user interface */
Int16 userInterfaceStart(
     CSL_Status *pCslStatus
)
{
    CSL_Status status;

    /* Start 1st conversion -- subsequent conversions are initiated in response to Timer ISR */
    status = SAR_startConversion(gsSarHandle);
    if (status != CSL_SOK)
    {
        *pCslStatus = status;
        return UI_CSL_FAIL;
    }

    /* Start GPT1 -- initiate SAR sampling */
    status = GPT_start(gsGptHandle);
    if (status != CSL_SOK)
    {
        *pCslStatus = status;
        return UI_CSL_FAIL;
    }

    return UI_SOK;
}

/* Gets current state of push-button network */
Int16 getCurrentPBNState(
    Uint16 sarReadData, 
    UI_PBNState *pPbnState
)
{
    Uint16 curSarDataRng;
    Bool found;
    UI_PBNState pbnState;

    if (pPbnState == NULL)
    {
        return UI_INV_PRM;
    }

    curSarDataRng = SAR_DATA_RNG00;
    found = FALSE;
    while (found == FALSE)
    {
        if (sarReadData >= gSarData2PbnState[curSarDataRng].rngEndPt)
        {
            found = TRUE;
        }
        else
        {
            curSarDataRng++;
        }
    }

    pbnState = gSarData2PbnState[curSarDataRng].pbnState;

    *pPbnState = pbnState;

    return UI_SOK;
}

/* Checks if current push-button network state is in allowed set of states */
Int16 chkAllowedPBNState(
    UI_PBNState pbnState, 
    UI_PBNState *allowedPbnStates, 
    Uint16 numAllowedPbnStates, 
    Int16 *pPbnStateAllowed
)
{
    Uint16 curAllowedPbnState;
    Int16 allowed;

    if ((allowedPbnStates == NULL) || (pPbnStateAllowed == NULL))
    {
        return UI_INV_PRM;
    }

    curAllowedPbnState = 0;
    allowed = 0;
    while (!allowed && (curAllowedPbnState < numAllowedPbnStates))
    {
        if (pbnState == allowedPbnStates[curAllowedPbnState])
        {
            allowed = 1;
        }
        else
        {
            curAllowedPbnState++;
        }
    }

    *pPbnStateAllowed = allowed;

    return UI_SOK;
}

/* Initializes momentary and user control states */
void initUcStates(
    UI_McStatesObj *mcStates, 
    UI_UcStatesObj *ucStates
)
{
    memset(mcStates, 0, sizeof(UI_McStatesObj));       /* all momentary controls inactive */
    memset(ucStates, 0, sizeof(UI_UcStatesObj));       /* recognizer inactive */
}

/* Updates momentary control states */
Int16 updateMcStates(
    UI_PBNState pbnState, 
    UI_McStatesObj *mcStates
)
{
    Uint16 *curMcStates;
    Uint16 *prevMcStates;
    Int16 status = UI_SOK;

    curMcStates = mcStates->curMcStates;
    prevMcStates = mcStates->prevMcStates;

    /* Update previous momentary control states */
    memcpy(prevMcStates, curMcStates, UI_NUM_MC);

    if (pbnState == UI_PUSH_BUTTON_NONE)
    {
        memset(&curMcStates[0], 0, UI_NUM_MC);
    }
    else
    {
        // note: PBN states with multiple button presses can be accomodated
        switch(pbnState)
        {
        case UI_RECOGNIZER_TOGGLE:
            curMcStates[MC_RECOGNIZER_TOGGLE_IDX] = 1;
            break;
        default:
            status = UI_INV_PRM;
            break;
        }
    }

    return status;
}

/* Updates user control states */
void updateUcStates(
    UI_McStatesObj *mcStates, 
    UI_UcStatesObj *ucStates
)
{
    Uint16 *curMcStates;
    Uint16 *prevMcStates;

    curMcStates = mcStates->curMcStates;
    prevMcStates = mcStates->prevMcStates;

    /* Update recognizer active state */
    if (curMcStates[MC_RECOGNIZER_TOGGLE_IDX] &&
        !prevMcStates[MC_RECOGNIZER_TOGGLE_IDX])
    {
        ucStates->recognizerActive ^= 0x1;
        ucStates->recognizerActiveUpd = TRUE;
    }
}

void gpt1Isr(void)
{
    /* Clear TIAFR flag */
    CSL_SYSCTRL_REGS->TIAFR = 0x2;

    SWI_post(&SWI_UserInput);
}

//Uint32 swiUserInputCnt=0; // debug
/*
 *  ======== swi_AudioDataCollectionFxn ========
 */
Void swi_UserInputFxn(Void)
{
    Bool sarDataReady;
    Uint16 sarReadData;
    UI_PBNState pbnState;
    Int16 pbnStateAllowed;
    CSL_Status cslStatus;
    Int16 status;

    //swiUserInputCnt++;

    //CSL_CPU_REGS->ST1_55 ^= (1<<CSL_CPU_ST1_55_XF_SHIFT); // toggle XF -- debug

    /* Check SAR status */
    sarDataReady = SAR_getStatus(gsSarHandle, &cslStatus); /* SAR data should always be ready after application startup since prd frequency is very slow compared with SAR sampling frequency */
    if ((sarDataReady != CSL_SAR_DATA_AVAILABLE) || (cslStatus != CSL_SOK))
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: SAR_getStatus()");
#endif
    	return;
    }

    /* Read SAR data */
    cslStatus = SAR_readData(gsSarHandle, &sarReadData);
    if (cslStatus != CSL_SOK)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: SAR_readData()");
#endif
    }
    //LOG_printf(&trace, "%04x", sarReadData);

    /* Start next conversion */
    cslStatus = SAR_startConversion(gsSarHandle);
    if (cslStatus != CSL_SOK)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: SAR_startConversion()");
#endif
    }

    /* Get current push-button network state */
    status = getCurrentPBNState(sarReadData, &pbnState);
    if (status != UI_SOK)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: getCurrentPBNState()");
#endif
    }
    //LOG_printf(&trace, "pbnState = %d", pbnState);
    //if (pbnState == UI_PUSH_BUTTON_UNKNOWN) LOG_printf(&trace, "pbnState = 0x%04X", pbnState);

    /* Check push-button network state is allowed state for HID */
    status = chkAllowedPBNState(pbnState, (UI_PBNState *)gAllowedPbnStates, UI_NUM_ALLOWED_PBN_STATES, &pbnStateAllowed);
    if (status != UI_SOK)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: chkAllowedPBNState()");
#endif
    }

    /* Filter disallowed push-button network states */
    /* Check change in push-button network state */
    if ((pbnStateAllowed) && (pbnState != gPrevPbnState))
    {
        gPrevPbnState = pbnState;

        /* Update momentary control states */
        updateMcStates(pbnState, &gMcStates);
        //LOG_printf(&trace, "1 %d %d", gMcStates.prevMcStates[0], gMcStates.curMcStates[0]);

        SWI_disable();
        /* Update user control states */
        updateUcStates(&gMcStates, &gUcStates);
        //LOG_printf(&trace, "2 %d %d", gUcStates.recognizerActive, gUcStates.recognizerActiveUpd);

        if (gUcStates.recognizerActiveUpd == TRUE)
        {
            gUcStates.recognizerActiveUpd = FALSE;

            if (gUcStates.recognizerActive == 1)
            {
                /* Reset Rx circular buffer */
                resetRxCircBuf(gRxCircBuf, RX_CIRCBUF_LEN, &gRxCircBufInFrame, &gRxCircBufOutFrame, &gRxCircBufOvrCnt, &gRxCircBufUndCnt);
                //LOG_printf(&trace, "3");
            }
            else
            {
                CSL_CPU_REGS->ST1_55 |= (1<<CSL_CPU_ST1_55_XF_SHIFT); // set XF -- indicate recognizer inactive
            }
        }
        SWI_enable();
    }
}
