/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
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
 * --/COPYRIGHT--*/

/** \file audio_collect.c Contains functions for setting up a ping-pong
 * buffer for audio data, starting and stopping audio data collection, and
 * the interrupt handler for the DMA interrupt that declares a buffer of
 * data is available for processing.\n (C) Copyright 2017, Texas Instruments,
 * Inc.
 */

/*----------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "appConfig.h"
#if !USE_I2S_AUDIO_IN
#include <ti/drivers/ADC.h>
#else
#include <ti/drivers/I2S.h>
#include <ti/drivers/i2s/I2SCC32XXDMA.h>
#include <ti/drivers/I2C.h>
#include "ti_codec.h"
#endif
#include <ti/drivers/Timer.h>
#include <ti/drivers/GPIO.h>
#include "Board.h"
#include "audio_collect_private.h"
#include "uart_term.h"

#if USE_CONTINUOUS_UART_RECORDING
#include "ti/devices/cc32xx/driverlib/utils.h"
#endif





/*----------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/
//Global Audio struct
Audio_Object AudioGlobal;

//Buffer to store ADC samples
uint16_t ADCScratchBuffer[320];
uint16_t *ADCScratchBufferPointer = ADCScratchBuffer;

//Used to keep track of how many samples taken from the ADC so far
int currentSampleNumber = 0;

//Used in UART continuous mode to determine whether to enable timeout
int currentFrameNumber = 0;

extern int g_intSleepAllowed;
//Semaphore for the Audio module to signal to the main task that an event has occurred
extern Semaphore_Struct mainSemStruct;
extern Semaphore_Handle mainSemHandle;

#if USE_ANALOG_AUDIO_IN
//For analog audio, an 8KHz timer, a 16KHz timer, and the ADC are required peripherals
Timer_Handle    adcSamplingTimerHandle;
Timer_Handle    adcSamplingSlowTimerHandle;
ADC_Handle adcHandle;
void adc_sampling_timer_isr_callback(Timer_Handle handle);

#elif USE_I2S_AUDIO_IN
//For I2S audio, the I2S peripheral is required
I2S_BufDesc bufReadDesc[2];
I2S_Handle i2sHandle;
I2C_Handle i2cHandle;
void i2s_isr_callback(I2S_Handle handle, I2S_BufDesc *desc);

#elif USE_UART_AUDIO_IN

#if USE_UART0
//If using UART0, get the handle from uart_term.c
extern UART_Handle uartHandle;
#else
UART_Handle uartHandle1;
#endif

#if USE_CONTINUOUS_UART_RECORDING
Timer_Handle    uartTimeoutTimerHandle;
int             timeoutTicks = 0;
extern int toggleRecordingSend;
//Callback invoked after the UART timer hits 0 and times out
void uart_timeout_timer_isr_callback(Timer_Handle handle);
#endif
//Callback invoked after UART_read() completes
void uart_isr_callback (UART_Handle handle, void *buf, size_t count);
#endif
/*----------------------------------------------------------------------------
 * Functions
 * --------------------------------------------------------------------------*/
/* Set up the device to collect audio samples in ping-pong buffers */
Audio_Handle Audio_setupCollect( int_least16_t *audioBuffer1, int_least16_t *audioBuffer2,
    uint_least32_t bufferSize, uint_least32_t sampleRate)
{
	/* Setting preamp power alternate to output low = off */
    /* Mapped to pin 63 (GPIO 8) */
#if !ENABLE_C5545_PIN_SETTINGS
    GPIO_write(CC3220SF_LAUNCHXL_GPIO_MIC_PREAMP,0);
#endif
    Audio_Handle audio = &AudioGlobal;

    /* Initial audio object initialization*/
    audio->audioBuffer1 = audioBuffer1;
    audio->audioBuffer2 = audioBuffer2;
    audio->bufferSize = bufferSize;
    audio->sampleRate = sampleRate;
    audio->samplingActive = false;
    audio->uartResetRequired = false;
    audio->uartIgnoreData = false;

#if USE_ANALOG_AUDIO_IN //Using analog audio in on Pin 60
    /* Set up a timer to generate 8kHz and 16kHz sample hold signal for ADC. */
    /* CC3220 - Can't configure ADC to use timer resource, but we can poll the ADC on a 8kHz/16kHz timer */
    /* TIRTOS - just need to set up timer params and then call Timer_open() */
    /* TIRTOS - Setup the ADC sampling timer with the TIMERA1A hardware timer */
    Timer_Params    params;
    Timer_Params_init(&params);

    params.periodUnits = Timer_PERIOD_HZ;
    params.period = 16000;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = adc_sampling_timer_isr_callback;
    adcSamplingTimerHandle = Timer_open(CC3220SF_LAUNCHXL_TIMER0, &params);
    params.period = 8000;
    adcSamplingSlowTimerHandle = Timer_open(CC3220SF_LAUNCHXL_TIMER4, &params);

    /* CC3220 - Setup ADC to use ADC channel 3, pin 60 as input (Need to jumper p60 on CC3220 to boosterpack mic out)*/
    /* TIRTOS - All config handled by ADC_open() */
    ADC_Params    adcParams;
    ADC_Params_init(&adcParams);
    adcHandle = ADC_open(CC3220SF_LAUNCHXL_ADC1,&adcParams);

    /* Power up the preamp */
#if !ENABLE_C5545_PIN_SETTINGS
    GPIO_write(CC3220SF_LAUNCHXL_GPIO_MIC_PREAMP,1);
#endif
#elif USE_I2S_AUDIO_IN //Using I2S0 for audio input
    //Don't need to do anything, I2S handling starts at Audio_startCollect
    bufReadDesc[0].bufPtr = audioBuffer1;
    bufReadDesc[0].bufSize = bufferSize*2;
    bufReadDesc[1].bufPtr = audioBuffer2;
    bufReadDesc[1].bufSize = bufferSize*2;
#if CONFIG_AUDIO_BOOSTERPACK
    I2C_Params i2cParams;
    /* Create I2C for usage */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2cHandle = I2C_open(Board_I2C0, &i2cParams);

    if (i2cHandle == NULL) {
        /* Error Initializing I2C */
        while (1);
    }
    ConfigureAudioCodec(0x20);
#endif
#elif USE_UART_AUDIO_IN
    //Don't need to do anything on startup for UART
#if USE_UART0

#else

#endif
#if USE_CONTINUOUS_UART_RECORDING
    //If we want to use continuous recording, setup the timeout timer
    Timer_Params params5;
    Timer_Params_init(&params5);

    params5.periodUnits = Timer_PERIOD_HZ;
    params5.period = 10;
    params5.timerMode = Timer_CONTINUOUS_CALLBACK;
    params5.timerCallback = uart_timeout_timer_isr_callback;
    uartTimeoutTimerHandle = Timer_open(CC3220SF_LAUNCHXL_TIMER5, &params5);

    //Furthermore, setup the UART interface here, so that it will be always-on
    UART_Params uartParams;
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = UART_BAUDRATE;
    uartParams.readCallback = uart_isr_callback;
    uartParams.readMode = UART_MODE_CALLBACK;
    uartHandle1 = UART_open(Board_UART1, &uartParams);
#endif
#endif
    return audio;
}

/*--------------------------------------------------------------------------*/
/* Start collecting audio samples in ping-pong buffers */
void Audio_startCollect(Audio_Handle audio)
{
    if(!(audio->samplingActive)){
        /* Start with primary (first) audio buffer transfer */
        audio->fillingBuffer1 = true;
        audio->overflow = false;
        audio->bufferActive = false;
        audio->samplingActive = true;
        audio->uartIgnoreData = false;
        g_intSleepAllowed = 0;
        currentSampleNumber = 0;
        //GPIO_write(CC3220_LAUNCHXL_GPIO_DEBUG,1);

#if USE_ANALOG_AUDIO_IN
        //For analog audio, starting the timer will allow the timer ISR to run and record ADC data at 8KHz
        Timer_start(adcSamplingSlowTimerHandle);
#elif USE_I2S_AUDIO_IN
        //For I2S audio, setup I2S peripheral and issue two reads (for ping-pong buffering)
        I2SCC32XXDMA_SerialPinParams I2SCC32XXDMA_serialParams;
        I2S_Params i2sParams;
        I2S_Params_init(&i2sParams);
        I2SCC32XXDMA_Params_init(&I2SCC32XXDMA_serialParams);
        currentFrameNumber = 0;
        i2sParams.customParams = (void *) &I2SCC32XXDMA_serialParams;
        i2sParams.readMode = I2S_MODE_CALLBACK;
        i2sParams.samplingFrequency =16000;
        i2sParams.readCallback = i2s_isr_callback;
        //i2sParams.writeMode = I2S_MODE_CALLBACK;
        //i2sParams.writeCallback = i2s_isr_callback;
        i2sHandle = I2S_open(Board_I2S0, &i2sParams);
        I2S_read(i2sHandle, &bufReadDesc[0]);
        I2S_read(i2sHandle, &bufReadDesc[1]);
#elif USE_UART_AUDIO_IN
#if USE_UART0
        //Close UART interface first, since it would have been opened by the uart_term.c
        //Then reopen the UART interface with target baudrate defined in appConfig.h
        //Finally, issue one read to start ping-pong cycle
        UART_close(uartHandle);
        UART_Params         uartParams;
        UART_Params_init(&uartParams);
        uartParams.writeDataMode    = UART_DATA_BINARY;
        uartParams.readDataMode     = UART_DATA_BINARY;
        uartParams.readReturnMode   = UART_RETURN_FULL;
        uartParams.readEcho         = UART_ECHO_OFF;
        uartParams.baudRate         = UART_BAUDRATE;
        uartParams.readCallback = uart_isr_callback;
        uartParams.readMode = UART_MODE_CALLBACK;
        uartHandle = UART_open(Board_UART0, &uartParams);
        UART_read(uartHandle,AudioGlobal.audioBuffer1,AudioGlobal.bufferSize*2);

#else
#if !USE_CONTINUOUS_UART_RECORDING
        //Open UART interface 1 with target baudrate defined in appConfig.h
        //Then issue one read to start ping-pong cycle.
        UART_Params         uartParams;
        UART_Params_init(&uartParams);
        uartParams.writeDataMode    = UART_DATA_BINARY;
        uartParams.readDataMode     = UART_DATA_BINARY;
        uartParams.readReturnMode   = UART_RETURN_FULL;
        uartParams.readEcho         = UART_ECHO_OFF;
        uartParams.baudRate         = UART_BAUDRATE;
        uartParams.readCallback = uart_isr_callback;
        uartParams.readMode = UART_MODE_CALLBACK;
        uartHandle1 = UART_open(Board_UART1, &uartParams);
        UART_read(uartHandle1,AudioGlobal.audioBuffer1,AudioGlobal.bufferSize*2);
#endif
#endif
#if USE_CONTINUOUS_UART_RECORDING
        //If using continuous mode, UART interface is always on, so no need to open UART
        //Start the UART timeout timer, and issue one read to start ping-pong cycle
        timeoutTicks = 0;
        Timer_start(uartTimeoutTimerHandle);
        UART_read(uartHandle1,AudioGlobal.audioBuffer1,AudioGlobal.bufferSize*2);
#endif
#endif
    }
}
/*--------------------------------------------------------------------------*/
/* Start collecting 16Khz audio samples in ping-pong buffers */
void Audio_startCollect16KHz(Audio_Handle audio)
{
    if(!(audio->samplingActive)){
        /* Start with primary (first) audio buffer transfer */
        audio->fillingBuffer1 = true;
        audio->overflow = false;
        audio->bufferActive = false;
        audio->samplingActive = true;
        audio->uartIgnoreData = false;
        g_intSleepAllowed = 0;
        currentSampleNumber = 0;

#if USE_ANALOG_AUDIO_IN
        /* TIRTOS - enable the hardware TIMERA1A so that it will collect a sample every 0.125ms */
        //For analog audio, starting the timer will allow the timer ISR to run and record ADC data at 8KHz
        Timer_start(adcSamplingTimerHandle);
#elif USE_I2S_AUDIO_IN
        //For I2S audio, setup I2S peripheral and issue two reads (for ping-pong buffering)
        I2SCC32XXDMA_SerialPinParams I2SCC32XXDMA_serialParams;
        I2S_Params i2sParams;
        I2S_Params_init(&i2sParams);
        I2SCC32XXDMA_Params_init(&I2SCC32XXDMA_serialParams);
        currentFrameNumber = 0;
        i2sParams.customParams = (void *) &I2SCC32XXDMA_serialParams;
        i2sParams.readMode = I2S_MODE_CALLBACK;
        i2sParams.samplingFrequency =16000;
        i2sParams.readCallback = i2s_isr_callback;
        //i2sParams.writeMode = I2S_MODE_CALLBACK;
        //i2sParams.writeCallback = i2s_isr_callback;
        i2sHandle = I2S_open(Board_I2S0, &i2sParams);
        I2S_read(i2sHandle, &bufReadDesc[0]);
        I2S_read(i2sHandle, &bufReadDesc[1]);
#elif USE_UART_AUDIO_IN
#if USE_UART0
        //Close UART interface first, since it would have been opened by the uart_term.c
        //Then reopen the UART interface with target baudrate defined in appConfig.h
        //Finally, issue one read to start ping-pong cycle
        UART_close(uartHandle);
        UART_Params         uartParams;
        UART_Params_init(&uartParams);
        uartParams.writeDataMode    = UART_DATA_BINARY;
        uartParams.readDataMode     = UART_DATA_BINARY;
        uartParams.readReturnMode   = UART_RETURN_FULL;
        uartParams.readEcho         = UART_ECHO_OFF;
        uartParams.baudRate         = UART_BAUDRATE;
        uartParams.readCallback = uart_isr_callback;
        uartParams.readMode = UART_MODE_CALLBACK;
        uartHandle = UART_open(Board_UART0, &uartParams);
        UART_read(uartHandle,AudioGlobal.audioBuffer1,AudioGlobal.bufferSize*2);
#else
#if !USE_CONTINUOUS_UART_RECORDING
        //Open UART interface 1 with target baudrate defined in appConfig.h
        //Then issue one read to start ping-pong cycle.
        UART_Params         uartParams;
        UART_Params_init(&uartParams);
        uartParams.writeDataMode    = UART_DATA_BINARY;
        uartParams.readDataMode     = UART_DATA_BINARY;
        uartParams.readReturnMode   = UART_RETURN_FULL;
        uartParams.readEcho         = UART_ECHO_OFF;
        uartParams.baudRate         = UART_BAUDRATE;
        uartParams.readCallback = uart_isr_callback;
        uartParams.readMode = UART_MODE_CALLBACK;
        uartHandle1 = UART_open(Board_UART1, &uartParams);
        UART_read(uartHandle1,AudioGlobal.audioBuffer1,AudioGlobal.bufferSize*2);
#endif
#endif
#if USE_CONTINUOUS_UART_RECORDING
        //If using continuous mode, UART interface is always on, so no need to open UART
        //Start the UART timeout timer, and issue one read to start ping-pong cycle
        timeoutTicks = 0;
        Timer_start(uartTimeoutTimerHandle);
        UART_read(uartHandle1,AudioGlobal.audioBuffer1,AudioGlobal.bufferSize*2);
#endif
#endif
    }
}

/*--------------------------------------------------------------------------*/
/* Stop collecting 8Khz audio samples in buffers */
void Audio_stopCollect(Audio_Handle audio)
{
#if USE_ANALOG_AUDIO_IN
    /* TIRTOS - just need to stop the hardware TIMERA1A timer */
    Timer_stop(adcSamplingSlowTimerHandle);
#elif USE_I2S_AUDIO_IN
    I2S_close(i2sHandle);
#elif USE_UART_AUDIO_IN
#if USE_UART0
    //Close UART interface first, then execute InitTerm to reopen UART interface to PC console
    UART_close(uartHandle);
    InitTerm();
#else
#if !USE_CONTINUOUS_UART_RECORDING
    UART_close(uartHandle1);
#endif
#endif
#if USE_CONTINUOUS_UART_RECORDING
    //Stop the timeout timer
    Timer_stop(uartTimeoutTimerHandle);
#endif
#endif
    audio->samplingActive = false;
    audio->uartResetRequired = false;
    audio->uartIgnoreData = true;
    g_intSleepAllowed = 1;
    //GPIO_write(CC3220_LAUNCHXL_GPIO_DEBUG,0);
}
/*--------------------------------------------------------------------------*/
/* Stop collecting 16Khz audio samples in buffers */
void Audio_stopCollect16KHz(Audio_Handle audio)
{

#if USE_ANALOG_AUDIO_IN
    /* TIRTOS - just need to stop the hardware TIMERA1A timer */
    Timer_stop(adcSamplingTimerHandle);
#elif USE_I2S_AUDIO_IN
    I2S_close(i2sHandle);
#elif USE_UART_AUDIO_IN
#if USE_UART0
    //Close UART interface first, then execute InitTerm
    UART_close(uartHandle);
    InitTerm();
#else
#if !USE_CONTINUOUS_UART_RECORDING
    UART_close(uartHandle1);
#endif
#endif
#if USE_CONTINUOUS_UART_RECORDING
    Timer_stop(uartTimeoutTimerHandle);
#endif
#endif
    audio->samplingActive = false;
    audio->uartResetRequired = false;
    audio->uartIgnoreData = true;
    g_intSleepAllowed = 1;
}

/*--------------------------------------------------------------------------*/
/* Shut down the audio collection peripherals*/
void Audio_shutdownCollect(Audio_Handle audio)
{
    /* Turn off preamp power */
#if !ENABLE_C5545_PIN_SETTINGS
    GPIO_write(CC3220SF_LAUNCHXL_GPIO_MIC_PREAMP,0);
#endif

#if USE_ANALOG_AUDIO_IN
    /* Disable the ADC */
    ADC_close(adcHandle);
#elif USE_I2S_AUDIO_IN

#elif USE_UART_AUDIO_IN
#if USE_UART0

#else
#if USE_CONTINUOUS_UART_RECORDING
    UART_close(uartHandle1);
#endif
#endif
#endif
}

/*--------------------------------------------------------------------------*/
/* Get pointer to active buffer with valid data ready for processing */
int_least16_t *Audio_getActiveBuffer(Audio_Handle audio)
{
    if(audio->fillingBuffer1)
    {
        return audio->audioBuffer2;
    }
    else
    {
        return audio->audioBuffer1;
    }
}

/*--------------------------------------------------------------------------*/
/* Indicate if an active buffer exists to be processed */
bool Audio_getActive(Audio_Handle audio)
{
    return(audio->bufferActive);
}

/*--------------------------------------------------------------------------*/
/* Indicate done processing active buffer holding valid data */
void Audio_resetActive(Audio_Handle audio)
{
    audio->bufferActive = false;

    /* DEBUG ONLY */
    ///MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN7);
}

/*--------------------------------------------------------------------------*/
/* Get overflow status */
bool Audio_getOverflow(Audio_Handle audio)
{
    return audio->overflow;
}

/*--------------------------------------------------------------------------*/
/* Reset overflow status */
void Audio_resetOverflow(Audio_Handle audio)
{
    audio->overflow = false;
}

/*--------------------------------------------------------------------------*/
/* Get UART reset required status */
bool Audio_getUARTResetRequired(Audio_Handle audio)
{
    return audio->uartResetRequired;
}
#if USE_ANALOG_AUDIO_IN
/*--------------------------------------------------------------------------*/
/* Interrupt handler callback for both 8KHz and 16KHz Timer */
void adc_sampling_timer_isr_callback(Timer_Handle Handle){
    ////unsigned long ulSample;
    uint16_t adcSample;
    /* TIRTOS - all of the interrupt status checking and clearing is taken care of by the timer driver */
    /*Read the current value from the ADC */
    ////ulSample = MAP_ADCFIFORead(ADC_BASE,ADC_CH_3);
    ////ulSample = (ulSample & 0x3ffc)>>2;
    ADC_convert(adcHandle,&adcSample);
    //Lorin's suggestion, make 12bit unsigned into 16 bit signed
    ////ulSample = ulSample<<4;
    ////ulSample^=(1<<15);
    adcSample = adcSample<<4;
    adcSample^=(1<<15);
    //*(ADCScratchBufferPointer+currentSampleNumber) = ulSample;
    *(AudioGlobal.audioBuffer1+currentSampleNumber+((!AudioGlobal.fillingBuffer1)*AudioGlobal.bufferSize)) = adcSample;
    currentSampleNumber++;
    /*If we have 160ADC samples, then set the proper flags to indicate that data is ready for processing */
    if(currentSampleNumber==160){
        if(AudioGlobal.bufferActive){
            AudioGlobal.overflow = true;
        }
        AudioGlobal.fillingBuffer1 = !AudioGlobal.fillingBuffer1;
        AudioGlobal.bufferActive = true;
        currentSampleNumber = 0;
        /* TIRTOS - post the event processing semaphore to main */
        ////IntDisableSleepOnISRExit();
        Semaphore_post(mainSemHandle);
    }
}
#elif USE_I2S_AUDIO_IN
/*--------------------------------------------------------------------------*/
/* Callback to handle I2S read complete events */
void i2s_isr_callback(I2S_Handle handle, I2S_BufDesc *desc){
    //I2S_BufDesc *pReadDesc;
//Needed if using SDK ver. earlier than v. 1.40
#if BUGGED_I2S_CALLBACK
    if(currentFrameNumber==0){
        currentFrameNumber++;
        return;
    }
    if(currentFrameNumber%2==1){//If Buffer1 just completed
        //ptr = bufReadDesc[0].bufPtr;
        //int temp = *ptr;
        //pointerToSoundStorage = SoundStorageBuffer+(160*(counter-1));
        //memcpy((void *)pointerToSoundStorage,(void *)ptr,320);
        I2S_read(i2sHandle,&bufReadDesc[0]);
    }
    else{            //If Buffer2 just completed
        //ptr = bufReadDesc[1].bufPtr;
        //int temp = *ptr;
        //pointerToSoundStorage = SoundStorageBuffer+(160*(counter-1));
        //memcpy((void *)pointerToSoundStorage,(void *)ptr,320);
        I2S_read(i2sHandle,&bufReadDesc[1]);
    }
#else
    I2S_read(i2sHandle,desc);
#endif
    currentFrameNumber++;
    if(AudioGlobal.bufferActive){
        AudioGlobal.overflow = true;
    }
    AudioGlobal.fillingBuffer1 = !AudioGlobal.fillingBuffer1;
    AudioGlobal.bufferActive = true;
    Semaphore_post(mainSemHandle);
}
#elif USE_UART_AUDIO_IN
void uart_isr_callback (UART_Handle handle, void *buf, size_t count){
#if UART_SYNC_IN_USE
    if (!AudioGlobal.uartIgnoreData)
    {
        /* Check the incoming frame to see if "SYNC" is the first 4 bytes */
        uint32_t syncWord = *(uint32_t*) buf;
        if (syncWord == 0x434E5953)
        {
            if (buf == AudioGlobal.audioBuffer1)
            {
                UART_read(handle, AudioGlobal.audioBuffer2,
                          AudioGlobal.bufferSize * 2);
            }
            else
            {
                UART_read(handle, AudioGlobal.audioBuffer1,
                          AudioGlobal.bufferSize * 2);
            }
            currentFrameNumber++;
            if (AudioGlobal.bufferActive)
            {
                AudioGlobal.overflow = true;
            }
            AudioGlobal.fillingBuffer1 = !AudioGlobal.fillingBuffer1;
            AudioGlobal.bufferActive = true;
            Semaphore_post(mainSemHandle);
            currentSampleNumber++;
        }
        /* If "SYNC" was not present correctly at the beginning of the frame, then request UART reset */
        else
        {
            AudioGlobal.bufferActive = true;
            AudioGlobal.uartResetRequired = true;
            Semaphore_post(mainSemHandle);
        }
        //UtilsDelay(5000);
    }
    else
    {
        //Ignoring all incoming UART data. By not alerting the main task of data it is effectively discarded.
    }
#else
    if(buf==AudioGlobal.audioBuffer1)
    {
        UART_read(handle,AudioGlobal.audioBuffer2,AudioGlobal.bufferSize*2);
    }
    else
    {
        UART_read(handle,AudioGlobal.audioBuffer1,AudioGlobal.bufferSize*2);
    }
    currentFrameNumber++;
    if(AudioGlobal.bufferActive)
    {
        AudioGlobal.overflow = true;
    }
    AudioGlobal.fillingBuffer1 = !AudioGlobal.fillingBuffer1;
    AudioGlobal.bufferActive = true;
    Semaphore_post(mainSemHandle);
#endif
}
#if USE_CONTINUOUS_UART_RECORDING
/* Interrupt handler callback for the timeout timer for UART recording w/C55x devices */
void uart_timeout_timer_isr_callback(Timer_Handle Handle){
    /* Only start ticking timer if we have started receiving data */
    if(currentSampleNumber!=0){
        timeoutTicks++;
    }
    /*If timer has ticked up to max value, then signal timeout*/
    if(timeoutTicks==TIMEOUT_TICK_COUNTS){
        toggleRecordingSend++;
        Semaphore_post(mainSemHandle);
    }
}
#endif
#endif
/* Set the UART interface to ignore data and not trigger callback */
void Audio_setUARTIgnoreData(Audio_Handle audio, bool val){
    audio->uartIgnoreData = val;
}

/* Get whether the UART data is currently being ignored */
bool Audio_getUARTIgnoreData(Audio_Handle audio){
    return audio->uartIgnoreData;
}

/* Flushes the UART FIFO in case of data corruption */
void Audio_flushFIFO(Audio_Handle audio){
    //Read out all 128 bytes of the UART FIFO
    UART_read(uartHandle1, audio->audioBuffer1, 16);
    //Wait 0.5ms to ensure any bytes in FIFO are read
    UtilsDelay(6730);
    //Cancel the UART read, in case there are less than 16 bytes in the FIFO
    UART_readCancel(uartHandle1);
}
