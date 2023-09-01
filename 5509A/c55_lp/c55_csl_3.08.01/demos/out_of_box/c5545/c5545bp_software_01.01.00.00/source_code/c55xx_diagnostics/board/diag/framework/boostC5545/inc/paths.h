/*
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

#include "sd_test/inc/sd_test.h"
#include "uart_test/inc/uart_test.h"
#include "led_test/inc/led_test.h"
#include "audio_test/audio_headset_loopback_test/inc/audio_headset_loopback_test.h"
#include "audio_test/audio_line_in_loopback_test/inc/audio_line_in_loopback_test.h"
#include "audio_test/audio_mic_in_loopback_test/inc/audio_mic_in_loopback_test.h"
#include "audio_test/audio_playback_test/inc/audio_playback_test.h"
#include "audio_test/external_mic_in_loopback_test/inc/external_mic_in_loopback_test.h"
#include "dsp_ble_interface_test/inc/dsp_ble_interface_test.h"
#include "gpio_dsp_to_lp_cc3200_test/inc/gpio_dsp_to_lp_cc3200_test.h"
#include "current_monitor_test/inc/current_monitor_test.h"
#include "oled_display_test/inc/oled_display_test.h"
#include "push_button_test/inc/push_button_test.h"
#include "rtc_test/inc/rtc_test.h"
#include "usb_test/inc/usb_test.h"
#include "spi_flash_test/inc/spi_flash_test.h"
#include "gpio_dsp_to_lp_msp432_test/inc/gpio_dsp_to_lp_msp432_test.h"
#include "i2c_dsp_msp_test/dsp_slave_msp_master/inc/i2c_dsp_slave_msp_master.h"
#include "i2c_dsp_msp_test/dsp_master_msp_slave/inc/i2c_dsp_master_msp_slave.h"

