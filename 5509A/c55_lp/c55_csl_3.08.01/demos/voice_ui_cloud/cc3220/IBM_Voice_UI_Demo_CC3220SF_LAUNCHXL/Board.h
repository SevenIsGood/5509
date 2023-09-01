/*
 * Copyright (c) 2016-2017, Texas Instruments Incorporated
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

#ifndef __BOARD_H
#define __BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/drivers/ADC.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/I2S.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/SDSPI.h>
#include <ti/drivers/SD.h>
#include <ti/drivers/SDFatFS.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/Watchdog.h>
#include <ti/drivers/Timer.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/drivers/dpl/HwiP.h>

#include "CC3220SF_LAUNCHXL.h"

#define Board_initGeneral            CC3220SF_LAUNCHXL_initGeneral

#define Board_ADC0                   CC3220SF_LAUNCHXL_ADC0
#define Board_ADC1                   CC3220SF_LAUNCHXL_ADC1

#define Board_CRYPTO0                CC3220SF_LAUNCHXL_CRYPTO0

#define Board_GPIO_LED_ON            CC3220SF_LAUNCHXL_GPIO_LED_ON
#define Board_GPIO_LED_OFF           CC3220SF_LAUNCHXL_GPIO_LED_OFF
#define Board_GPIO_LED0              CC3220SF_LAUNCHXL_GPIO_LED_D7
/*
 *  CC3220SF_LAUNCHXL_GPIO_LED_D5 and CC3220SF_LAUNCHXL_GPIO_LED_D6 are shared with the I2C
 *  and PWM peripherals. In order for those examples to work, these LEDs are
 *  taken out of gpioPinCOnfig[]
 */
#define Board_GPIO_LED1              CC3220SF_LAUNCHXL_GPIO_LED_D7
#define Board_GPIO_LED2              CC3220SF_LAUNCHXL_GPIO_LED_D7

#define Board_GPIO_BUTTON0           CC3220SF_LAUNCHXL_GPIO_SW2
#define Board_GPIO_BUTTON1           CC3220SF_LAUNCHXL_GPIO_SW3

#define Board_I2C0                   CC3220SF_LAUNCHXL_I2C0
#define Board_I2C_TMP                CC3220SF_LAUNCHXL_I2C0

#define Board_I2S0                   CC3220SF_LAUNCHXL_I2S0

#define Board_PWM0                   CC3220SF_LAUNCHXL_PWM6
#define Board_PWM1                   CC3220SF_LAUNCHXL_PWM7

#define Board_SDSPI0                 CC3220SF_LAUNCHXL_SDSPI0

#define Board_SD0                    CC3220SF_LAUNCHXL_SD0

#define Board_SDFatFS0               CC3220SF_LAUNCHXL_SD0

/* CC3220SF_LAUNCHXL_SPI0 is reserved for the NWP */
#define Board_SPI0                   CC3220SF_LAUNCHXL_SPI1

#define Board_TIMER0                 CC3220SF_LAUNCHXL_TIMER0
#define Board_TIMER1                 CC3220SF_LAUNCHXL_TIMER1
#define Board_TIMER2                 CC3220SF_LAUNCHXL_TIMER2

#define Board_UART0                  CC3220SF_LAUNCHXL_UART0
#define Board_UART1                  CC3220SF_LAUNCHXL_UART1

#define Board_WATCHDOG0              CC3220SF_LAUNCHXL_WATCHDOG0

/* Board specific I2C addresses */
#define Board_TMP_ADDR               (0x41)
#define Board_SENSORS_BP_TMP_ADDR    (0x40)
/* Macros for accessing peripherals */
#define CC3220_LED_GREEN_GPIO_BASE GPIOA1_BASE
#define CC3220_LED_GREEN_GPIO_MASK 8
#define CC3220_LED_GREEN_GPIO_NUMBER 11
#define CC3220_LED_GREEN_PACKAGE_PIN PIN_02

#define CC3220_LED_YELLOW_GPIO_BASE GPIOA1_BASE
#define CC3220_LED_YELLOW_GPIO_MASK 4
#define CC3220_LED_YELLOW_GPIO_NUMBER 10
#define CC3220_LED_YELLOW_PACKAGE_PIN PIN_01

#define CC3220_LED_RED_GPIO_BASE GPIOA1_BASE
#define CC3220_LED_RED_GPIO_MASK 2
#define CC3220_LED_RED_GPIO_NUMBER 9
#define CC3220_LED_RED_PACKAGE_PIN PIN_64

#define CC3220_SWITCH_3_GPIO_BASE GPIOA1_BASE
#define CC3220_SWITCH_3_GPIO_MASK 32
#define CC3220_SWITCH_3_GPIO_NUMBER 13
#define CC3220_SWITCH_3_PACKAGE_PIN PIN_04
#define CC3220_SWITCH_3_GPIO_INT_NUM INT_GPIOA1

#define CC3220_SWITCH_2_GPIO_BASE GPIOA2_BASE
#define CC3220_SWITCH_2_GPIO_MASK 64
#define CC3220_SWITCH_2_GPIO_NUMBER 22
#define CC3220_SWITCH_2_PACKAGE_PIN PIN_15
#define CC3220_SWITCH_2_GPIO_INT_NUM INT_GPIOA2

#define CC3220_GPIO6_BASE GPIOA0_BASE
#define CC3220_GPIO6_MASK 64
#define CC3220_GPIO6_NUMBER 6
#define CC3220_GPIO6_PACKAGE_PIN PIN_61
#define CC3220_GPIO6_INT_NUM INT_GPIOA0

#define CC3220_GPIO3_BASE GPIOA0_BASE
#define CC3220_GPIO3_MASK 8
#define CC3220_GPIO3_NUMBER 3
#define CC3220_GPIO3_PACKAGE_PIN PIN_58
#define CC3220_GPIO3_INT_NUM INT_GPIOA0

#define CC3220_GPIO4_BASE GPIOA0_BASE
#define CC3220_GPIO4_MASK 16
#define CC3220_GPIO4_NUMBER 4
#define CC3220_GPIO4_PACKAGE_PIN PIN_59
#define CC3220_GPIO4_INT_NUM INT_GPIOA0

#define CC3220_GPIO24_BASE GPIOA3_BASE
#define CC3220_GPIO24_MASK 1
#define CC3220_GPIO24_NUMBER 24
#define CC3220_GPIO24_PACKAGE_PIN PIN_17
#define CC3220_GPIO24_INT_NUM INT_GPIOA3
/* Pin to jumper to p15, replacement for p3.7 Mic Preamp */
#define CC3220_GPIO31_BASE GPIOA3_BASE
#define CC3220_GPIO31_MASK 128
#define CC3220_GPIO31_NUMBER 31
#define CC3220_GPIO31_PACKAGE_PIN PIN_45
#define CC3220_GPIO31_INT_NUM INT_GPIOA3
/* New pin to jumper to p15, replacement for p3.7 Mic Preamp */
#define CC3220_GPIO1_BASE GPIOA0_BASE
#define CC3220_GPIO1_MASK 2
#define CC3220_GPIO1_NUMBER 1
#define CC3220_GPIO1_PACKAGE_PIN PIN_55
#define CC3220_GPIO1_INT_NUM INT_GPIOA0
/* Pin to jumper to p59, replacement for CS
#define CC3220_GPIO23_BASE GPIOA2_BASE
#define CC3220_GPIO23_MASK 128
#define CC3220_GPIO23_NUMBER 23
#define CC3220_GPIO23_PACKAGE_PIN PIN_16
#define CC3220_GPIO23_INT_NUM INT_GPIOA2 */
/* Test pin to jumper to p15, replacement for p3.7 Mic Preamp */
#define CC3220_GPIO8_BASE GPIOA1_BASE
#define CC3220_GPIO8_MASK 1
#define CC3220_GPIO8_NUMBER 8
#define CC3220_GPIO8_PACKAGE_PIN PIN_63
#define CC3220_GPIO8_INT_NUM INT_GPIOA0

/* Pin to jumper to p59, replacement for CS */
#define CC3220_GPIO0_BASE GPIOA0_BASE
#define CC3220_GPIO0_MASK 1
#define CC3220_GPIO0_NUMBER 0
#define CC3220_GPIO0_PACKAGE_PIN PIN_50
#define CC3220_GPIO0_INT_NUM INT_GPIOA0
/*
 * These macros are provided for backwards compatibility.
 * Please use the <Driver>_init functions directly rather
 * than Board_init<Driver>.
 */
#define Board_initADC                ADC_init
#define Board_initGPIO               GPIO_init
#define Board_initI2C                I2C_init
#define Board_initI2S                I2S_init
#define Board_initPWM                PWM_init
#define Board_initSDSPI              SDSPI_init
#define Board_initSD                 SD_init
#define Board_initSDFatFS            SDFatFS_init
#define Board_initSPI                SPI_init
#define Board_initUART               UART_init
#define Board_initWatchdog           Watchdog_init

/*
 * These macros are provided for backwards compatibility.
 * Please use the corresponding 'Board_GPIO_xxx' macros as the macros
 * below are deprecated.
 */
#define Board_LED_ON                 Board_GPIO_LED_ON
#define Board_LED_OFF                Board_GPIO_LED_OFF
#define Board_LED0                   Board_GPIO_LED0
#define Board_LED1                   Board_GPIO_LED1
#define Board_LED2                   Board_GPIO_LED2
#define Board_BUTTON0                Board_GPIO_BUTTON0
#define Board_BUTTON1                Board_GPIO_BUTTON1
#define Board_TMP006_ADDR            Board_TMP_ADDR

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */
