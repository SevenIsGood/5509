/*
* user_interface.h
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

#ifndef _USER_INTERFACE_H_
#define _USER_INTERFACE_H_

#include <std.h>

#define UI_SOK      ( 0 )   /* indicates successful return status */
#define UI_INV_PRM  ( 1 )   /* indicates invalid parameter to function */
#define UI_CSL_FAIL ( 2 )   /* indicates CSL function failure */

#define UI_DEF_SAMP_FREQ            ( 15.625 )  /* default user interface sampling frequency in Hz */

#ifdef PF_C5515_EVM
#define UI_NUM_PUSH_BUTTONS         ( 10 ) /* number of push-buttons */

/* SAR data range endpoints */
#define SAR_DATA_RNG_EP00           ( 0x03BA )  /* range 00 endpoint -- NONE */
#define SAR_DATA_RNG_EP01           ( 0x0346 )  /* range 01 endpoint -- SW06 */
#define SAR_DATA_RNG_EP02           ( 0x02EF )  /* range 03 endpoint -- SW08 */
#define SAR_DATA_RNG_EP03           ( 0x0290 )  /* range 04 endpoint -- SW14 */
#define SAR_DATA_RNG_EP04           ( 0x0247 )  /* range 05 endpoint -- SW07 */
#define SAR_DATA_RNG_EP05           ( 0x0227 )  /* range 06 endpoint -- SW09 */
#define SAR_DATA_RNG_EP06           ( 0x01F8 )  /* range 07 endpoint -- SW10 */
#define SAR_DATA_RNG_EP07           ( 0x01A0 )  /* range 08 endpoint -- SW11 */
#define SAR_DATA_RNG_EP08           ( 0x0120 )  /* range 09 endpoint -- SW12 */
#define SAR_DATA_RNG_EP09           ( 0x006C )  /* range 10 endpoint -- SW15 */
#define SAR_DATA_RNG_EP10           ( 0x0000 )  /* range 11 endpoint -- SW13 */

/* SAR data ranges */
typedef enum
{
    SAR_DATA_RNG00,
    SAR_DATA_RNG01,
    SAR_DATA_RNG02,
    SAR_DATA_RNG03,
    SAR_DATA_RNG04,
    SAR_DATA_RNG05,
    SAR_DATA_RNG06,
    SAR_DATA_RNG07,
    SAR_DATA_RNG08,
    SAR_DATA_RNG09,
    SAR_DATA_RNG10
} UI_SarDataRngs;

/* Push-button network states */
typedef enum
{
    UI_PUSH_BUTTON_UNKNOWN,
    UI_PUSH_BUTTON_NONE,
    UI_PUSH_BUTTON_SW06,
    UI_PUSH_BUTTON_SW07,
    UI_PUSH_BUTTON_SW08,
    UI_PUSH_BUTTON_SW09,
    UI_PUSH_BUTTON_SW10,
    UI_PUSH_BUTTON_SW11,
    UI_PUSH_BUTTON_SW12,
    UI_PUSH_BUTTON_SW13,
    UI_PUSH_BUTTON_SW14,
    UI_PUSH_BUTTON_SW15
} UI_PBNState;

#define UI_RECOGNIZER_TOGGLE        ( UI_PUSH_BUTTON_SW15 )

#elif defined(PF_C5535_EZDSP)
#define UI_NUM_PUSH_BUTTONS         ( 2 ) /* number of push-buttons */

/* SAR data range endpoints */
#define SAR_DATA_RNG_EP00           ( 0x0354 )  /* range 00 endpoint -- NONE */
#define SAR_DATA_RNG_EP01           ( 0x0254 )  /* range 01 endpoint -- SW01 */
#define SAR_DATA_RNG_EP02           ( 0x0000 )  /* range 11 endpoint -- SW02 */

/* SAR data ranges */
typedef enum
{
    SAR_DATA_RNG00,
    SAR_DATA_RNG01,
    SAR_DATA_RNG02
} UI_SarDataRngs;

/* Push-button network states */
typedef enum
{
    UI_PUSH_BUTTON_UNKNOWN,
    UI_PUSH_BUTTON_NONE,
    UI_PUSH_BUTTON_SW01,
    UI_PUSH_BUTTON_SW02
} UI_PBNState;

#define UI_RECOGNIZER_TOGGLE        ( UI_PUSH_BUTTON_SW02 )

#else
#warn "Unsupported platform"

#endif

#define UI_NUM_PBN_STATES           ( UI_NUM_PUSH_BUTTONS + 2 ) /* number of push-button network states; +1 for NONE, +1 for UNKNOWN */
#define UI_NUM_ALLOWED_PBN_STATES   ( 2 ) /* number of allowed push-button network states */

/* Number of momentary controls  */
#define UI_NUM_MC                   ( 1 )
/* Momentary control indices */
#define MC_RECOGNIZER_TOGGLE_IDX    ( 0 )

/* SAR data range to push-button network state object */
typedef struct {
    Uint16 rngEndPt; /* SAR data range endpoint */
    UI_PBNState pbnState; /* PBN state for SAR data range */
} SarDataRng2PbnStateObj;

/* Momentary control states object */
typedef struct {
    Uint16 curMcStates[UI_NUM_MC];
    Uint16 prevMcStates[UI_NUM_MC];
} UI_McStatesObj;

/* User control states object */
typedef struct
{
    Uint16 recognizerActive;    /* recognizer active state */
    Bool recognizerActiveUpd;   /* flag indicates whether recognizer active state updated */
} UI_UcStatesObj;

/* SAR data range to push-button network mapping */
extern const SarDataRng2PbnStateObj gSarData2PbnState[UI_NUM_PBN_STATES];

/* Allowed push-button network states */
extern const UI_PBNState gAllowedPbnStates[UI_NUM_ALLOWED_PBN_STATES];

/* Momentary control states */
extern UI_McStatesObj gMcStates;
/* Call control states */
extern UI_UcStatesObj gUcStates;

/* Initializes user interface */
Int16 userInterfaceInit(
     Float pbnSampFreq,
     CSL_Status *pCslStatus
);

/* Starts user interface */
Int16 userInterfaceStart(
     CSL_Status *pCslStatus
);

/* Initializes momentary and user control states */
void initUcStates(
    UI_McStatesObj *mcStates,
    UI_UcStatesObj *ucStates
);

/* Updates momentary control states */
Int16 updateMcStates(
    UI_PBNState pbnState,
    UI_McStatesObj *mcStates
);

/* Updates user control states */
void updateUcStates(
    UI_McStatesObj *mcStates,
    UI_UcStatesObj *ucStates
);

Void swi_UserInputFxn(Void);

#endif
