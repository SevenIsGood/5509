=============
INTRODUCTION
=============
This document gives brief details of the out of box demo on C5545 BoosterPack.

C5545 BoosterPack out of box demo contains audio demo running on C5545 DSP and C5545 Boost Android App
for the devices running Lollipop and Marshmallow versions of Android OS. Android App GUI is targeted for
devices with display size of 5 inch or more. 

Audio demo comprises streaming of audio samples to headphone port of C5545 BoosterPack. 
Audio source will be either wave files stored on SD card or input from LINE IN. 
Demo supports voice commands through on-board MIC for playback control while playing the 
wave files from SD card 

C5545 Boost Android App is a companion for the demo which acts as utility to control equalizer 
settings of the playback. Audio demo running on C5545 DSP and Android App will be communicating 
through CC2650 BLE controller on the BoosterPack.

Out of Box demo features
--------------------------
Below are the features supported by out of box demo on C5545 BoosterPack 

- Wave file playback from SD card
  -- Stereo & mono wave files with standard sampling rates (8, 11.025, 16, 22.05, 24, 32, 44.1, 48 KHz)
- Audio loopback from LINE IN to HEADPHONE port
- Voice commands (play/pause/stop) to control the playback (While playing files from SD card)
- Push buttons to control the playback
- OLED display for play status indication
- C5545 Boost Android App
  -- Playback Equalizer control from the App
  -- Play status (playing/paused/stopped) indication on the App



Software Setup
=====================
This section describes the steps to program and run the Out of Box demo on C5545 BoosterPack.
C5545 BoosterPack comes with Out of box demo binaries programmed to DSP and BLE by default. 
Follow the steps in this section to reprogram the out of box demo binaries provided at 
'software\bin\oob_demo'. 
Install the Android App on the device being used for testing as desribed in this section.

BLE Firmware
-------------
Follow below steps to flash BLE firmware to BoosterPack

- Connect emulator to JTAG-CC2650 port on BoosterPack
  Pin converter may be needed while using standard emulators with 14/20 pin headers
  Please check the references at below link
   http://www.spectrumdigital.com/14-pin-to-20-pin-cti-jtag-adapter
   http://www.spectrumdigital.com/cti20-pin-to-arm10-pin-jtag-adapter
- Connect micro USB cable to DEBUG micro-USB port of BoosterPack and host PC
- Open CCS IDE and launch the target configuration for CC2650
  Follow below steps to create target configuration file if needed
  -- Select the menu 'File->New->Target Configuration File'
  -- Give a name to the target configuration file and click 'Finish'
  -- In the new target configuration file created
     Select 'Connection' based on emulator being used
     Select 'Board or Device' as CC2650F128 and save the file
  -- Select 'View->Target Configurations' to open Target Configurations window
  -- Right click on the CC2650 Target Configuration file created and select 'Launch Selected Configuration'
- After target is launched, select 'Run->Connect Target' to connect to the target
- After target is connected, select 'Run->Load->Load Program' to load the program
- Browse and load the program 'c5545BoostEqualizerStack.out' 
  Target will not halt after running above program, ignore and go to next step
- Browse and load the program 'c5545BoostEqualizer.out'
- Disconnect the JTAG and reboot the board 



DSP Firmware
-------------
- Format the SD card using SD formatter tool which can be downloaded form below link
  https://www.sdcard.org/downloads/formatter_4
- Copy the audio demo boot image (software\bin\oob_demo\bootimg.bin) to SD card root directory

Note: SD card delivered with BoosterPack is having correct formatting and out of box demo
binaries copied to SD card. 
Skip the this step while using BoostePack SD card as is.
 
 
Android App
-------------
 - Copy/Download the software\bin\oob_demo\C5545_BOOST.apk to Android device
 - Enable settings of Android device to install external Apps if needed
   -- Go to Menu > Settings > Security > and check Unknown Sources
 - Click on the C5545_BOOST.apk downloaded to Android device and select install
 - After successful installation, C5545 Boost Android App appears in App list 



Running the BoosterPack Out of Box Demo
=======================================

Wave file playback from SD card
-------------------------------
 - Insert the SD card with audio demo boot image and few wave files to microSD slot of the BoosterPack 
 - Connect headset to HEADPHONE port of BoosterPack
 - Power ON the BoosterPack
 - Select Audio source as SD (SW3) 
 - Wait till system initialization is complete 
 - Launch Equalizer App on Android device.
 - Allow the App to enable Bluetooth, if not already enabled
 - Press on 'scan' button to start scanning for the BoosterPack
 - Press on 'connect' button once the 'BoosterPack' device is detected by the App
 - Start playback by pressing SW2 or by speaking 'play' near BoosterPack on-board MIC
 - Slide the equalizer bars on the App to change the equalizer values of playback 
 - Observe the change in audio at headphone output while changing the equalizer values from App
 - Pause the playback by pressing SW2 or by speaking 'pause' near BoosterPack on-board MIC
 - Resume playback by pressing SW2 or by speaking 'play' near BoosterPack on-board MIC
 - Stop playback by pressing SW3 or by speaking 'stop' near BoosterPack on-board MIC
 - Observe the change in C5545 Boost App Playback status when playback is stopped/paused and playback is running


Audio Input from LINE IN
------------------------
 - Insert the SD card with audio demo boot image to microSD slot of BoosterPack
 - Connect LINE IN cable between an audio device (PC/Mobile/audio player) and LINE IN port of BoosterPack
 - Connect headset to HEADPHONE port of BoosterPack
 - Power ON the BoosterPack
 - Select Audio source as LINE IN (SW4) 
 - Wait till system initialization is complete 
 - Launch Equalizer App on Android device.
 - Allow the App to enable Bluetooth, if not already enabled
 - Press on 'scan' button to start scanning for the BoosterPack
 - Press on 'connect' button once the 'BoosterPack' device is detected by the App
 - Play an audio file on the audio device connected to LINE IN
 - Press push button 'SW2' on BoosterPack to start playback
 - Slide the equalizer bars on the App to change the equalizer values of playback 
 - Observe the change in audio at headphone output while changing the equalizer values from App
 - Press 'SW2' to pause and 'SW3' to stop playback on BoosterPack. Press 'SW2' to resume the playback
 - Observe the change in C5545 Boost App Playback status when playback is stopped/paused and playback is running

Note: Voice command support is not available while running the demo with audio source as LINE IN

