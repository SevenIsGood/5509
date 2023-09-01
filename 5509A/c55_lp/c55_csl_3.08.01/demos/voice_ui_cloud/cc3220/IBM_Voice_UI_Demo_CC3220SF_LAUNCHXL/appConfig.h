/*
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
 */

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef ASSERT_ON_ERROR
#define LOOP_FOREVER() \
            {\
                while(1); \
            }

// check the error code and handle it
#define ASSERT_ON_ERROR(error_code)\
            {\
                 if(error_code < 0) \
                   {\
                        ERR_PRINT(error_code);\
                        /*return error_code;*/\
                 }\
            }
#endif

#define APPLICATION_NAME        "CC3220 Voice Recognition Demo"
#define APPLICATION_VERSION     "0.03.08.01"

//Ping parameters
//Only used for the pingGoogleTest() function, which isn't called by default but left as a connectivity test tool
#define PING_INTERVAL   (1000)  /* In msecs */
#define PING_TIMEOUT    (3000)  /* In msecs */
#define PING_PKT_SIZE   (20)    /* In bytes */
#define NO_OF_ATTEMPTS  (3)

//Wifi options
#define USE_PROVISIONING 0  //Set to 1 to use app-based AP provisioning. Set to 0 to connect to AP defined in wifi options above
#define SSID_NAME           "simplelinktest"    /* AP SSID */
#define SECURITY_TYPE       SL_WLAN_SEC_TYPE_WPA_WPA2/* Security type (OPEN or WEP or WPA*/
#define SECURITY_KEY        "wifitest"              /* Password of the secured AP */
#define SSID_LEN_MAX        32
#define BSSID_LEN_MAX       6

#define ASYNC_EVT_TIMEOUT   (5000) //Timeout (ms) for wifi init and provisioning
#define SL_STOP_TIMEOUT         200 //Timeout (ms) for sl_stop()

//Audio input options. Must enable only one audio source at a time.
#define USE_ANALOG_AUDIO_IN 0   //Set this option to take in analog audio from CC3220SF_LAUNCHXL_ADC1 (default pin60). Tested to work with analog audio boosterpack
#define USE_I2S_AUDIO_IN 0      //Set this option to take in digital, 16bit 2channel PCM audio over the I2S interface (default pins: CLK p53, FS p63, data p64)
#define USE_UART_AUDIO_IN 1     //Set this option to take in digital, 16bit 1channel PCM audio over the UART interface (default pin: rx p2)
#define USE_UART0 0             //Set this option to use the UART0 (normally reserved for PC console) instead of UART1. Only useful for debugging.


//UART-specific options
#define UART_BAUDRATE 1228800   //MUST set correct baudrate for UART interface if in use
//#define UART_BAUDRATE 921600
//#define UART_BAUDRATE 614400
//#define UART_BAUDRATE 460800
#define UART_SYNC_IN_USE 1              //Setting this option will modify audio collection module to account for 'SYNC' word preceding each frame
                                        //MUST set to 0 if not using UART, or not using the SYNC feature.
#define USE_CONTINUOUS_UART_RECORDING 1 //Setting this option will make the UART read data and record to memory continuously, without waiting for a GPIO trigger
                                        //This prevents the UART interface from ever being closed once initialized, thus avoiding the UART TX line from being pulled low
#define TIMEOUT_TICK_COUNTS         50 //Set this define to desired timeout length for continuous recording (each tick is 0.1s). Should be of sufficient time to allow MAX_FRAMES to be recorded

//I2S-specific defines. Leave all as 0 if not using I2S
#define CONFIG_AUDIO_BOOSTERPACK 0   //Set this define to configure the codec used on the CC3200 Audio Boosterpack during init. Do not set if using other I2S audio source
#define USE_2CH_AUDIO 0 //If using I2S (which uses 2 channel audio), must set to 1 to inform HTTP handler that audio data has two channels
#define BUGGED_I2S_CALLBACK 0 //Set to 1 if using version of SDK with bugged I2S drivers (version <1.40)

//General software options
#define ENABLE_DEBUG_OUTPUT 1 //Set to 1 to enable verbose console output to PC.
#define ENABLE_C5545_PIN_SETTINGS 0 //Setting this to 1 will mux out the pins to a C5545-friendly config, and also disable the display
                                    //MUST also have USE_UART_AUDIO_IN, USE_CONTINUOUS_UART_RECORDING = 1
                                    //MUST NOT have USE_ANALOG_AUDIO_IN,USE_I2S_AUDIO_IN,USE_UART0 enabled
#define MAX_TRANSCRIPT_LENGTH 100 //How many characters that the HTTP handler will save from the IBM transcript output
#define MAX_FRAMES 450 //How many 160-sample frames that the app will be able to store. 4.5s at 16KHz mono audio settings
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __APP_CONFIG_H__ */
