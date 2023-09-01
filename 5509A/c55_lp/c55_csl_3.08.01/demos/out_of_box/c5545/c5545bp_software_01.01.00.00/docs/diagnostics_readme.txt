================
1. Introduction
================
Diagnostic software package provides the test software for verifying the hardware 
interfaces on C5545 BoosterPack. Diagnostic tests include three different types of 
interface tests as described below.

1.1	DSP Interface tests
----------------------------
DSP interface tests provide the test cases for verifying on-board interfaces connected to C5545 DSP. 

DSP interface tests include
•	SPI Flash Test
•	LED Test
•	OLED Display Test
•	Audio Tests
•	Current Monitor Test
•	RTC Test
•	Push Button Test
•	USB Test 

1.2	LaunchPad interface tests
----------------------------------
C5545 BoosterPack provides hardware connectivity to interface with MSP432 LaunchPad.
LaunchPad interface tests provide the test cases to verify the interfaces between 
C5545 DSP and MSP432/CC3200 LaunchPad.

LaunchPad interface tests include
•	C5545 BP to MSP432 LP GPIO Test
•	C5545 BP to CC3200 LP GPIO Test
•	MSP-DSP I2C Tests

Refer to C5545 BoosterPack HW technical reference manual for details on interfacing
C5545 BoosterPack with MSP432 LaunchPad.

1.3	BLE Interface tests
----------------------------
BLE interface tests include the test cases to verify the interfaces connected to 
CC2650 BLE module on the BoosterPack. 

BLE interface tests include
•	BLE to DSP GPIO Test
•	BLE to MSP432 LP GPIO Test
•	BLE UART Test


========================
2. Diagnostic Test Setup
========================
This section describes the steps to program and execute diagnostic tests on C5545 BoosterPack.
Diagnostic test binaries referred in this document are available on the SD card at below path
software\bin\diagnostics. 

2.1	Initial Steps
---------------------
 - Copy the diagnostic boot image (software\bin\diagnostics\bootimg.bin) to SD card 
   root directory and insert the card into SD slot of BoosterPack.
 - Short the pins 1 and 3 of JP2 and JP3 ports on the board. 
 - Short the pins on J2 port
   Use this jumper configuration as default unless a different setup is mentioned
   in the test procedure
 - Connect the micro USB between BoosterPack DEBUG microUSB port and the host PC. 
 - Set SW6 to ON.
 - Open serial console (Ex: Teraterm) on host PC, connect to COM port on which BoosterPack 
   Debug port is connected and setup for following configurations
    -- Baud rate    -  115200
    -- Data length  -  8 bit
    -- Parity       -  None
    -- Stop bits    -  1
    -- Flow control -  None
 - Press the RESET switch on the BoosterPack or press Enter key on the serial console 
   to get the diagnostic test menu display on the serial console as shown below.


*******************************************************************
               C5545 BoosterPack HW Diagnostic Tests
*******************************************************************


Diagnostic Tests                         Pass            Fail
-----------------------                 ------          ------
  0 - Auto Run All Tests                  0               0
  1 - SPI Flash Test                      0               0
  2 - LED Test                            0               0
  3 - OLED Display Test                   0               0
  4 - Audio Playback Test                 0               0
  5 - Audio LINE IN Loopback Test         0               0
  6 - Audio On-board MIC Loopback Test    0               0
  7 - Audio Headset Loopback Test         0               0
  8 - Audio External MIC Loopback Test    0               0
  9 - Current Monitor Test                0               0
 10 - RTC Test                            0               0
 11 - Push Button Test                    0               0
 12 - USB Test                            0               0
 13 - I2C DSP Master - MSP Slave          0               0
 14 - I2C DSP Slave - MSP Master          0               0
 15 - DSP-BLE Interface Test              0               0

  q - Quit

Enter Desired Option:


2.2	Test Accessories
-------------------------
 - BoosterPack
 - Windows PC with CCS & TeraTerm installed
 - Micro SD card
 - Two Micro USB cables
 - Mini USB cable
 - Headset with MIC
 - LINE IN Cable
 - Micro Phone or headset with separate MIC port
 - MSP432 LaunchPad
 - CC3200 LaunchPad
 - XDS110/XDS200 emulator 
 - CTI 20-pin/14-pin to ARM10-pin adapter for BLE programming
   http://www.spectrumdigital.com/14-pin-to-20-pin-cti-jtag-adapter
   http://www.spectrumdigital.com/cti20-pin-to-arm10-pin-jtag-adapter

=============================
3. Diagnostic Test Procedure
=============================
This section describes the setup and procedure to run diagnostic tests 
on C5545 BoosterPack. Most of the tests are part of boot image copied 
to SD card with few exceptions. Some of the tests may need
CCS IDE and connecting to different HW platforms from CCS.
Refer to 'Appendix' section for more details about connecting to
different targets from CCS.


----------------------------
3.1	DSP Interface Tests
----------------------------

3.1.1	SPI Test
-----------------
3.1.1.1	Test Accessories
No additional accessories are required for this test.

3.1.1.2	Test Setup
No additional test setup is required.

3.1.1.3	Test Execution
SPI flash device data write, data read and block erase operations 
are verified during SPI flash test.


Sample SPI test log is shown below

********************************
         SPI FLASH Test
********************************

Running SPI flash erase test...

SPI flash erase test passed!!


Running SPI flash data read/write test...

SPI Write & Read buffer matching

SPI flash read write test Passed!!

SPI FLASH Test Completed!!


3.1.2	LED Test
-----------------

3.1.2.1	Test Accessories
No additional accessories are required for this test

3.1.2.2	Test Setup
No additional test setup is required.

3.1.2.3	Test Execution
LED test toggles 3 user LEDs on the BoosterPack.

Sample test log for LED test is shown below

**********************
       LED Test
**********************

Check if all three LED's on the BoosterPack are toggling
Press Y/y if three LED's are blinking properly, Any other key for failure:
y

LED Test Passed!

LED Test Completed!!


3.1.3	OLED Test
------------------

3.1.3.1	Test Accessories
No additional accessories are required for this test

3.1.3.2	Test Setup
No additional test setup is required.

3.1.3.3	Test Execution
Test verifies device detection and configuration of OLED module on the BoosterPack 
by displaying test messages on to line1 and line2 of the OLED. Messages displayed 
on the screen shall be scrolled at the end of test. Test will prompt for user input 
to confirm the OLED display.

Sample test log for OLED test is shown below
  
*************************************
              OLED TEST
*************************************
Running OLED Device Detect Test...
OLED detection Successful

Running OLED Display Test...

Displaying 'TI C5545 BP' on first row...

Displaying 'TI C5545 BP' on second row...

Displaying 'Texas Instruments' on first row and
'C5545 BoosterPack' on the second row

Scrolling OLED Display from Right to Left
OLED Display Test Completed

Press Y/y if the test messages are displayed properly on OLED,
any other key for failure:
y

 OLED Test Passed!

OLED Test Completed!!



3.1.4	Audio Playback Test
----------------------------

3.1.4.1	Test Accessories
Headset

3.1.4.2	Test Setup
Connect headset to HEADPHONE port of BoosterPack.

3.1.4.3	Test Execution
Audio playback test verifies AIC3206 audio codec interfaces by sending an audio stream 
to HEADPHONE port of the BoosterPack through AIC3206. This test verifies Tx path of 
on-board audio interface. Press push button SW3 to stop running the test. 
Press ‘y’ in case audio output is proper or any other key for failure.

Sample Audio Playback test log is shown below
	
***************************************
          AUDIO PLAYBACK TEST
***************************************
Test outputs a sine tone on HEADPHONE port of BoosterPack

Connect the headset to the HEADPHONE port of the BoosterPack
Observe the Audio Tone at HEADPHONE port

Starting the Audio Tone Play...

Press SW3 on the BoosterPack for exiting from the test

Press Y/y if Audio output from the HEADPHONE port is proper,
any other key for failure:
y

 Audio Playback Test Passed!

Audio Playback Test Completed!



3.1.5	Audio LINE IN Loopback Test
------------------------------------

3.1.5.1	Test Accessories
 - 3.5mm male to male audio (LINE IN) cable
 - Headset
 - Test PC / audio device

3.1.5.2	Test Setup
 - Connect 3.5mm male to male audio (LINE IN) cable between audio OUT 
   of PC / audio device and LINE IN port of the BoosterPack.
 - Connect headset to HEADPHONE port of BoosterPack.

3.1.5.3	Test Execution
Audio loopback test verifies AIC3206 audio codec interfaces by receiving an audio 
stream on LINE IN and sending it back to HEADPHONE port of the BoosterPack through AIC3206. 
This test verifies both Tx and Rx paths of on-board audio interfaces. 
Play a media file on audio player device or PC, run the audio loopback test and observe that
audio is played at HEADPHONE port of the BoosterPack. Press SW3 to stop the test. 
Press ‘y’ in case audio output is proper or any other key for failure.

Sample Audio LINE IN loopback test log is shown below

*******************************************
        AUDIO LINE IN LOOPBACK TEST
*******************************************
Test Receives audio samples from LINE IN and output the same
on HEADPHONE port

Connect headset to the HEADPHONE port of the BoosterPack

Connect a LINE-IN cable between the audio port of the
Test PC and LINE IN of the BoosterPack

Play any audio file from the Test PC and Check
Audio from the headset connected to BoosterPack

Press SW3 on the BoosterPack for exiting from the test

Press Y/y if the Audio stream from LINE IN is
observed at the headset connected to HEADPHONE port,
any other key for failure:
y
 Audio LINE IN Loopback Test Passed!

Audio LINE IN Loopback Test Completed!!


3.1.6	Audio MIC IN Loopback Test
----------------------------------

3.1.6.1	Test Accessories
Headset

3.1.6.2	Test Setup
Connect headset to HEADPHONE port of BoosterPack.

3.1.6.3	Test Execution
Audio MIC IN Loopback test verifies the interface between on board MIC and 
AIC3206 by receiving an audio stream from the on board MIC and sending it back 
to the HEADPHONE port of the BoosterPack. Speak near BoosterPack MIC and confirm
that same audio is observed at headset connected to HEADPHONE port of BoosterPack.
Press SW3 to stop the test. 
Press ‘y’ in case audio output is proper or any other key for failure.

Sample Audio MIC IN Loopback test log is shown below

********************************************
         AUDIO MIC IN LOOPBACK TEST
********************************************
Connect headset to the HEADPHONE port of the BoosterPack

This test loops back the audio input from the on-board MIC
of the BoosterPack to the headset connected to HEADPHONE port


Press SW3 on the BoosterPack for exiting from the test

Press Y/y if the Audio received from on-board MIC of the BoosterPack is
observed at the headset connected at the HEADPHONE port properly,
any other key for failure:
y

 Audio MIC IN Loopback Test Passed!

Audio MIC IN Loopback Test Completed!


3.1.7	Audio Headset Loopback Test
-----------------------------------

3.1.7.1	Test Accessories
Headset with MIC.

3.1.7.2	Test Setup
Connect the headset to the HEADPHONE port of the BoosterPack.

3.1.7.3	Test Execution
Audio Headset Loopback test verifies the interface between the headset’s MIC 
and AIC3206 by receiving an audio stream from the headset with MIC and sending 
it back to the HEADPHONE port of the BoosterPack. Speak near headset MIC and confirm
that same audio is observed at headset output connected to HEADPHONE port of BoosterPack.
Press SW3 to stop the test. 
Press ‘y’ in case audio output is proper or any other key for failure.

Sample log for Audio headset loopback test is shown below

*****************************************
       AUDIO HEADSET LOOPBACK TEST
*****************************************
Test receives audio samples from HP MIC IN and outputs the
same on HEADPHONE port

Connect headset to the HEADPHONE port of the BoosterPack

Speak near the headset MIC and check the audio from headset output

Press SW3 on the BoosterPack for exiting from the test

Press Y/y if the Audio received from headphone MIC is
observed at the headset output properly
any other key for failure:
y

 Audio Headset Loopback Test Passed!

Audio Headset Loopback Test Completed!



3.1.8	External MIC IN Loopback Test
--------------------------------------

3.1.8.1	Test Accessories
PC Microphone with one speaker jack and one audio jack.

3.1.8.2	Test Setup
Connect the MIC jack of the PC microphone to the LINE IN and 
audio output jack to the HEADPHONE port of the BoosterPack.

3.1.8.3	Test Execution
External MIC IN Loopback test verifies AIC3206 audio codec interfaces by receiving 
an audio stream on LINE IN and sending it back to HEADPHONE port of the BoosterPack 
through AIC3206. This test verifies both Tx and Rx paths of on-board audio interfaces.

Speak something from the PC microphone’s mic and check whether you can hear the same 
on the HEADPHONE port of the BoosterPack. Press SW3 to stop the test. 
Press ‘y’ in case audio output is proper or any other key for failure. 

Sample log for External MIC IN Loopback test is shown below

***************************************************
         AUDIO EXTERNAL MIC IN LOOPBACK TEST
***************************************************
Test Receives audio samples from PC microphone's MIC
and outputs the same on HEADPHONE port

Connect a PC microphone audio input jack to the LINE IN and
Speaker jack to the HEADPHONE port of the BoosterPack

Speak near the MIC of the PC microphone and
check Audio from the HEADPHONE port

Press SW3 on the BoosterPack for exiting from the test

Press Y/y if the Audio stream from the PC microphone
is properly Loopback to the HEADPHONE port of the BoosterPack,
any other key for failure:
y

 Audio External MIC In Loopback Test Passed!

Audio External MIC In Loopback Test Completed!


3.1.9	Current Monitor Test
----------------------------

3.1.9.1	Test Accessories
No additional accessories are required for this test

3.1.9.2	Test Setup
No additional test setup is required.

3.1.9.3	Test Execution
This test verifies the four INA219 devices on the BoosterPack by reading 
the current, power, bus and shunt voltage values.
 
Sample log for current monitor test is shown below.

************************************
        Current Monitor Test
************************************
Reading values from CVDD port
Shunt voltage - 34.250000mV
Bus voltage - 1.300000V
Power - 44.516346mW
Current - 35.407639mA

Reading values from LDOI port
Shunt voltage - 35.169998mV
Bus voltage - 1.756000V
Power - 57.752338mW
Current - 33.886391mA

Reading values from DSP_DVDDIO port
Shunt voltage - 3.490000mV
Bus voltage - 1.792000V
Power - 6.249617mW
Current - 3.469874mA

Reading values from VCC3V3_USB port
Shunt voltage - 327.639984mV
Bus voltage - 0.020000V
Power - 0.000000mW
Current - 35.430191mA


Current Monitor Test Completed!


3.1.10	RTC Test
-------------------------

3.1.10.1	Test Accessories
No additional accessories are required for this test

3.1.10.2	Test Setup
No additional test setup is required.

3.1.10.3	Test Execution
This test verifies the RTC module by setting the Date, Time and reading them back.

Sample log for RTC test is shown below

**************************
         RTC Test
**************************

RTC Set Time Test...


This test demonstrates RTC Time functionality
RTC Time will be set, read and displayed 10 times.
RTC interrupt will be generated for each Second

RTC_setCallback Successful
Setting RTC Time Successful
Setting RTC Date Successful
Setting RTC Events Successful

Starting the RTC

Iteration 1: Time and Date is : 12:12:12:0021, 16-10-08
Iteration 2: Time and Date is : 12:12:12:0026, 16-10-08
Iteration 3: Time and Date is : 12:12:12:0031, 16-10-08
Iteration 4: Time and Date is : 12:12:12:0036, 16-10-08
Iteration 5: Time and Date is : 12:12:12:0041, 16-10-08
Iteration 6: Time and Date is : 12:12:12:0046, 16-10-08
Iteration 7: Time and Date is : 12:12:12:0051, 16-10-08
Iteration 8: Time and Date is : 12:12:12:0056, 16-10-08
Iteration 9: Time and Date is : 12:12:12:0061, 16-10-08
Iteration 10: Time and Date is : 12:12:12:0067, 16-10-08

RTC Set Time Test Passed!!

RTC Test Completed!


3.1.11	Push Button Test
---------------------------------

3.1.11.1	Test Accessories
No additional accessories are required for this test

3.1.11.2	Test Setup
No additional test setup is required.

3.1.11.3	Test Execution
This test verifies the working of three user switches on the BoosterPack 
by generating an interrupt to display the name of the switch when a push button 
is pressed.

For every press on the push button switch it will display two option either to 
continue or to exit, Enter ‘y’ or ‘Y’ to continue (or) Enter ‘x’  or ‘X’  
to exit on the TeraTerm.
 

Sample test log for push button test is shown below

********************************
        Push Button Test
********************************
Press any push button switch on the BoosterPack and check
if the corresponding switch name is displayed on the console

SW2 is pressed
Press X/x for exiting the test
Press Y/y to continue
y
continue
SW3 is pressed
Press X/x for exiting the test
Press Y/y to continue
y
continue
SW4 is pressed
Press X/x for exiting the test
Press Y/y to continue
x
Exiting from the push button test

Push Button Test Completed!



3.1.12	USB Test
-----------------

3.1.12.1	Test Accessories
Micro USB cable.

3.1.12.2	Test Setup
Connect the micro USB cable between the Test PC and CLIENT Port of the BoosterPack.

3.1.12.3	Test Execution
This test verifies the USB interface by moving the mouse cursor left and right by 
pressing SW2 and SW4 respectively. Press SW2 and SW4 during the test and observe the 
corresponding mouse movement on host PC. SW3 is used to exit the test. Test will prompt for 
user input to confirm if the corresponding events are sent properly through USB. 

Sample log for USB test on BoosterPack is shown below

****************************
          USB Test
****************************
Press SW2 to move the mouse cursor to LEFT
Press SW4 to move the mouse cursor to RIGHT
Press SW3 to exit from the test

Press Y/y if the corresponding events are sent properly
through USB, any other key for failure
y
 USB Test Passed!

USB Test Completed!!


---------------------------------
3.2	LaunchPad Interface Tests
---------------------------------
This section describes the procedure for running the diagnostic tests between 
C5545 BoosterPack and MSP432, CC3200 LaunchPads. Binaries for MSP432 LaunchPad 
are available in ‘msp432_lp’ folder and binaries for CC3200 LaunchPad are available 
in ‘cc3200_lp’ folder in diagnostic binary package (software\bin\diagnostics).


3.2.1	C5545 BP to MSP432 LP GPIO Test
---------------------------------------
3.2.1.1	Test Accessories
 - Two micro USB cables.
 - MSP432 LaunchPad.
 
3.2.1.2	Test Setup
 - Connect the MSP432 to the BoosterPack by using the Expansion connectors.
 - Configure the UART jumpers on BoosterPack to enable the communication 
   between DSP and LaunchPad
	JP2 – Short pin 3 & 4
	JP3 – Short pin 1 & 2
 - Connect one micro USB cable to the BoosterPack’s Debug port and the 
   Test PC and other micro USB cable to MSP432 LaunchPad and the Test PC.
 - Power ON the BoosterPack board
 
3.2.1.3	Test Execution
This test verifies the GPIO lines between the DSP and MSP432 LaunchPad.

MSP432 LP (From CCS)
- Open CCS IDE and launch target configuration file for MSP432 LP
- Connect to the target, load and run the program gpio_LaunchPad_msp432_to_dsp_test.out
- Press any key to continue the test after starting DSP program as described below

BoosterPack (from CCS)
- Open CCS IDE and launch target configuration file for C5545 DSP
- Connect to the target, load and run the program gpio_dsp_to_lp_msp432_test.out
- Observe the logs on CCS consoles of both MSP432 LP and C5545 BoosterPack

Sample log for Test on BoosterPack is shown below:

***********************************************
       C5545 BP TO MSP432 LaunchPad TEST       
***********************************************

Writing HIGH to GPIO PINS 8 & 31
Waiting for LaunchPad to Write HIGH on GPIO PINS 6,7,9 & 30...

LaunchPad write completed DSP started reading...

DSP is able to read the GPIO PINS 6,7 & 9 as HIGH
Writing LOW to GPIO PINS 8 & 31

Waiting for LaunchPad to Write LOW on GPIO PIN 6,7,9 & 30...
LaunchPad write completed DSP started reading...

DSP is able to read the GPIO PINS 6,7 & 9 as LOW

GPIO C5545 BP TO LaunchPad TEST Passed!


Sample log at LaunchPad is shown below:

***********************************************
        MSP432 LP to C5545 BP GPIO Test        
***********************************************

Configured Uart_rx and I2s_data as input pins
Configured I2s_clk, Uart_tx, i2s_rx, i2s_fs data as output pins
Clearing Uart_tx, I2s_fx, I2s_clk and I2s_rx pins

Enter any character once DSP side code started running
1

Waiting for DSP to write HIGH for Uart_tx and I2s_data pins...
I2s_data is read HIGH
Uart_tx pin also read HIGH

Writing HIGH to the Uart_tx, I2s_fx, I2s_clk and I2s_rx

Waiting for DSP to write LOW for Uart_tx and I2s_data pins...
I2s_data pin is read LOW
Writing LOW to the Uart_tx[30], I2s_fx[7], I2s_clk[6] and I2s_rx[9]

MSP432 LP to C5545 BP GPIO Test Passed!

MSP432 LP to C5545 BP GPIO Test Completed!!



3.2.2	C5545 BP to CC3200 LP GPIO Test
----------------------------------------

3.2.2.1	Test Accessories
 - Two micro USB cables.
 - CC3200 LaunchPad.
 - Energia IDE

3.2.2.2	Test Setup
 - Connect the CC3200 LaunchPad to the BoosterPack by using the Expansion connectors.
 - Configure the UART jumpers on BoosterPack to enable the communication between 
   DSP and LaunchPad
	JP2 – Short pin 3 & 4
	JP3 – Short pin 1 & 2
 - Connect one micro USB cable to the BoosterPack’s Debug port and the Test PC 
   and other micro USB cable to the CC3200 LaunchPad and the Test PC.
 - Power ON the BoosterPack board
 
3.2.2.3	Test Execution
This test verifies the GPIO lines between the DSP and CC3200 LaunchPad.

CC3200 LP (From Energia)
- Remove the jumper at J8 (TCK) and connect it to SOP2 for programming the LaunchPad 
- Open Energia IDE, configure board and serial port for CC3200 LP that is connected. 
- Open the sketch gpio_lp_cc3200_bp_test.ino (software\bin\diagnostics\cc3200_lp\gpio_lp_cc3200_bp_test), 
  compile and upload it to CC3200 LP.
- Put the jumper back from SOP2 to J8 for running the program
- Open Energia serial monitor and press restart button on LaunchPad to start running the program
- Press any key to continue the test after starting DSP program as described below

BoosterPack (from CCS)
- Open CCS IDE and launch target configuration file for C5545 DSP
- Connect to the target, load and run the program gpio_dsp_to_lp_cc3200_test.out
- Observe the logs on CCS and Energia consoles

Sample log for Test on BoosterPack is shown below:

***********************************************
       C5545 BP TO CC3200 LaunchPad TEST       
***********************************************

Writing HIGH to GPIO PINS 8 & 31
Waiting for LaunchPad to Write HIGH on GPIO PINS 6,7,9 & 30...

LaunchPad write completed DSP started reading...
DSP is able to read the GPIO PINS 6,7 & 9 as HIGH

Writing LOW to GPIO PINS 8 & 31

Waiting for LaunchPad to Write LOW on GPIO PIN 6,7,9 & 30...
LaunchPad write completed DSP started reading...

DSP is able to read the GPIO PINS 6,7 & 9 as LOW

GPIO C5545 BP TO LaunchPad TEST Passed!


Sample log at LaunchPad is shown below:

CC3200 LP to C5545 BP GPIO Test

Enter Any Character to Proceed further:
Waiting for DSP to write HIGH
Values are HIGH from DSP
i2s1dx	uartrx	
1	1	

LaunchPad as Output, writing HIGH
uarttx	i2s1fs	i2s1clk	i2s1rx	
1	1	1	1

Waiting for DSP to write LOW
Values are read LOW from DSP
i2s1dx	uartrx	
0	0	

LaunchPad as Output, writing LOW
uarttx	i2s1fs	i2s1clk	i2s1rx	
0	0	0	0

CC3200 LP to C5545 BP GPIO Test Passed!



3.2.3	MSP432 Slave DSP Master I2C Test
-----------------------------------------

3.2.3.1	Test Accessories
MSP432 LaunchPad

3.2.3.2	Test Setup
 - Connect the MSP432 LP to the BoosterPack by using the Expansion connectors.
 - Configure the UART jumpers on BoosterPack to enable the communication between DSP and serial port
	JP2 – Short pin 1 & 3
	JP3 – Short pin 1 & 3
 - Connect one micro USB cable to the BoosterPack’s Debug port and the Test PC and 
   other micro USB cable to MSP432 LaunchPad and the Test PC.
 - Power ON the BoosterPack 

3.2.3.3	Test Execution
This test verifies the i2c interface between the MSP and DSP while the MSP 
is acting as slave and DSP as master. During the test, master sends 16 bytes 
of data which should be received by slave properly.

MSP432 LP (From CCS)
- Open CCS and connect to msp432 LaunchPad using the relevant target configuration file.
- Load and run the program msp432_i2c_msp_slave_dsp_master_test.out on msp432 
- Press any key to continue the test after DSP comes out of reset and 
  DSP diagnostic menu is accessible

BoosterPack (from Diagnostic serial console menu)
- After starting the program on MSP432, confirm that BoosterPack diagnostic menu 
  is accessible on serial console
- Run the ‘I2C DSP Master - MSP Slave‘ test from diagnostic menu
- Observe the logs on CCS for MSP432 and serial console for BoosterPack
- Verify that data sent from BoosterPack is received properly by MSP432


Sample log for Test on BoosterPack is shown below:

*************************************************
        'DSP Master - MSP Slave' I2C Test
*************************************************

Sending 16 Bytes to Slave in blocks of 8
Data Sent to Slave:
0x0     0x1     0x2     0x3     0x4     0x5     0x6     0x7
0x8     0x9     0xa     0xb     0xc     0xd     0xe     0xf

'MSP Slave - DSP Master' I2C Test Completed!!



Sample log for Test on MSP432 is shown below:

*************************************************
        'MSP Slave - DSP Master' I2C Test        
*************************************************
MSP432 I2C Slave Mode Test
MSP432 is Configured in Slave Mode with Address 0x38

Press Any Key to Continue After DSP is Out of Reset
1

Waiting for Data from Master...

Data Received from Master:
0x0	0x1	0x2	0x3	0x4	0x5	0x6	0x7	
0x8	0x9	0xa	0xb	0xc	0xd	0xe	0xf	

Data Received from Master Matched with Expected Data!

'MSP Slave - DSP Master' I2C Test Passed!
'MSP Slave - DSP Master' I2C Test Completed!!



3.2.4	MSP432 Master DSP Slave I2C Test
----------------------------------------

3.2.4.1	Test Accessories
MSP432 LaunchPad

3.2.4.2	Test Setup
 - Connect the MSP432 LP to the BoosterPack by using the Expansion connectors.
 - Configure the UART jumpers on BoosterPack to enable the communication between 
   DSP and serial port
	JP2 – Short pin 1 & 3
	JP3 – Short pin 1 & 3
 - Connect one micro USB cable with the BoosterPack’s Debug port and the Test PC 
   and other micro USB cable with MSP432 LaunchPad and the Test PC.
 - Power ON the BoosterPack 

3.2.4.3	Test Execution
This test verifies the i2c interface between the MSP and DSP while the MSP is acting 
as master and DSP as slave. During the test, master sends 16 bytes of data which should 
be received by slave properly.

MSP432 LP (From CCS)
- Open CCS and connect to msp432 LaunchPad using the relevant target configuration file.
- Load and run the program msp432_i2c_msp_master_dsp_slave_test.out on msp432 
- Press any key to continue the test after running DSP side program

BoosterPack (from Diagnostic serial console menu)
- After starting the program on MSP432, confirm that BoosterPack diagnostic menu is 
  accessible on serial console
- Run the ‘I2C DSP Slave - MSP Master‘ test from diagnostic menu
- Observe the logs on CCS for MSP432 and serial console for BoosterPack
- Verify that data sent from MSP432 is received properly by BoosterPack


Sample log for Test on BoosterPack is shown below:

*******************************************************
         'DSP Slave - MSP Master' I2C Test
*******************************************************
Waiting for the MSP to transfer data...

Data Received from Master:
0x0     0x1     0x2     0x3     0x4     0x5     0x6     0x7
0x8     0x9     0xa     0xb     0xc     0xd     0xe     0xf

Data Received from Master Matched with Expected Data!

'MSP Master- DSP Slave ' I2C Test Passed!!

'MSP Master - DSP Slave ' I2C Test Completed!!



Sample log for Test on MSP432 is shown below:

*******************************************************
         'MSP Master - DSP Slave' I2C Test       
*******************************************************
Press Any Key to Continue After Running DSP Program
1

Sending 16 Bytes to Slave
0x0	0x1	0x2	0x3	0x4	0x5	0x6	0x7	
0x8	0x9	0xa	0xb	0xc	0xd	0xe	0xf	

'MSP Master - DSP Slave' I2C Test Completed!!



3.2.5	MSP432 INA Device Test
-------------------------------

3.2.5.1	Test Accessories
MSP432 LaunchPad

3.2.5.2	Test Setup
 - Connect the MSP432 LP to the BoosterPack by using the Expansion connectors.
 - Connect one micro USB cable to the BoosterPack’s Debug port and the 
   Test PC and other micro USB cable to MSP432 LaunchPad and the Test PC.
 
3.2.5.3	Test Execution
This test verifies accessing the INA device on BoosterPack from MSP432 LaunchPad.

MSP432 LP (From Energia)
 - Open Energia IDE, configure board and serial port for MSP432 LP that is connected. 
 - Open the sketch msp_ina_access.ino, compile and upload it to MSP432 LP.
 - Open Energia serial monitor and press restart button on LaunchPad to start running the program


Sample test log is shown below:

MSP432 INA Device Test


Reading values from CVDD port

shunt Voltage -34.02mv
Bus Voltage - 1.31V
Power - 44.52mW
Current - 34.05mA

Reading values from LDOI port

shunt Voltage -34.81mv
Bus Voltage - 1.76V
Power - 50.46mW
Current - 28.74mA

Reading values from DSP_DVDDIO port

shunt Voltage -3.11mv
Bus Voltage - 1.79V
Power - 9.87mW
Current - 5.59mA

Reading values from VCC3V3_USB port

shunt Voltage -327.63mv
Bus Voltage - 0.02V
Power - 0.00mW
Current - 48.39mA

MSP432 INA Device Test Completed



-----------------------------------
3.3	CC2650 BLE Interface Tests
-----------------------------------

3.3.1	DSP to CC2650 Interface Test
-------------------------------------

3.3.1.1	Test Accessories
- Mini USB Cable
- Micro USB cables
- XDS110/200 emulator
- 14/20 to ARM 10 pin JTAG converter

3.3.1.2	Test Setup
- Connect one micro USB cable to the BoosterPack’s Debug port and the Test PC. 
- Connect emulator to JTAG-CC2650 port on BoosterPack
  Pin converter may be needed while using standard emulators with 14/20 pin headers
- Power ON the BoosterPack

3.3.1.3	Test Execution
This test verifies GPIO & SPI lines between CC2650 and C5545 DSP on BoosterPack.

CC2650 (From CCS)
- Open CCS, launch target configuration file for CC2650
- Connect to target, load & run the program ble_dsp_interface_test.out

C5545 DSP (from Diagnostic serial console menu)
- After the BLE firmware is flashed, select ‘DSP-BLE Interface Test’ 
  from DSP diagnostic test menu.
- Check the logs CCS for CC2650 and on serial console for DSP

Sample log for Test on CC2650 is shown below:

**********************************
           BLE SPI Test       
**********************************

Waiting to Read data from BP to BLE.....

read data 0x1 
read data 0x2 
read data 0x3 
read data 0x4 
 read data match rxbuffer
Write start:4 bytes

write data 0x5
write data 0x6
write data 0x7
write data 0x8

BLE Data Read and Transfer Test Passed!

BLE SPI Passed!



Sample log for Test on DSP is shown below:


************************************
       DSP-BLE Interface Test
************************************

Test verifies SPI & GPIO interfaces between BLE and DSP


Writing Four Bytes of Data to BLE
0x1     0x2     0x3     0x4

Write Successful

Waiting for Data from BLE

Data Read from BLE
0x5     0x6     0x7     0x8

Read data match with expected data

DSP-BLE Interface Test Passed!



3.3.2	MSP432 to CC2650 GPIO Test
-----------------------------------

3.3.2.1	Test Accessories
 - MSP432 LaunchPad
 - Mini USB Cable
 - Two micro USB cables
 - XDS110/200 emulator
 - 14/20 to ARM 10 pin JTAG converter

3.3.2.2	Test Setup
 - Connect the MSP432 LP to the BoosterPack by using the Expansion connectors.
 - Configure the UART jumpers on BoosterPack to enable the communication between 
   CC2650 and LaunchPad
	JP2 – Short pin 2 & 4
	JP3 – Short pin 2 & 4
 - Connect one micro USB cable to the BoosterPack’s Debug port and the Test PC 
   and other micro USB cable to the MSP432 LaunchPad and the Test PC.
 - Connect emulator to JTAG-CC2650 port on BoosterPack
   Pin converter may be needed while using standard emulators with 14/20 pin headers
 - Power ON the BoosterPack


3.3.2.3	Test Execution
This test verifies GPIO lines between CC2650 on BoosterPack and MSP432 LaunchPad.

MSP432 LP (From CCS)
- Open CCS and connect to msp432 LaunchPad using the relevant target configuration file.
- Load and run the program gpio_LaunchPad_msp432_to_ble_test.out on msp432 
- Press any key to continue the test after running program on CC2650

BoosterPack (from CCS)
- Open CCS and connect to CC2650 using the relevant target configuration file.
- Load and run the program gpio_ble_to_msp432_lp_test.out
- Observe the logs on both the CCS consoles


Sample log for Test on CC2650 is shown below:

**********************************************************
         CC2650 BLE TO MSP432 LaunchPad GPIO Test         
**********************************************************
Writing HIGH to the UART_TX pin
Waiting for the Launchpad to write HIGH to the UART_RX pin...
UART_RX has read HIGH

Writing LOW to the UART_TX pin
Waiting for the Launchpad to write LOW to UART_RX pin...
UART_RX has read LOW

CC2650 BLE TO MSP432 LaunchPad GPIO Test Passed!



Sample log for Test on MSP432 is shown below:

*******************************************************
        MSP432 LaunchPad to CC2650 BLE GPIO Test       
*******************************************************

This Test Verifies UART interface lines between MSP432 LP and CC2650 BLE as GPIOs
Enter any character once BLE side code starting running
1

Waiting for BLE to write HIGH for UART_RX pin...
UART_RX pin is read as HIGH
Writing HIGH to the UART_TX Pin

Waiting for BLE to write LOW to UART_RX pin...
UART_RX pin read as LOW
Writing LOW to the UART_TX

MSP432 LaunchPad to CC2650 BLE GPIO Test!

MSP432 LaunchPad to CC2650 BLE GPIO Test Completed!!



3.3.3	CC2650 UART Test
-------------------------
3.3.3.1	Test Accessories
 - Mini USB Cable
 - Micro USB cables
 - XDS110/200 emulator
 - 14/20 to 10 pin JTAG converter

3.3.3.2	Test Setup
- Configure the UART jumpers on BoosterPack to enable the communication between 
  CC2650 and serial port
	JP2 – Short pin 1 & 2
	JP3 – Short pin 3 & 4
- Connect micro USB cable to the BoosterPack’s Debug port and the Test PC.
- Connect emulator to JTAG-CC2650 port on BoosterPack
   Pin converter may be needed while using standard emulators with 14/20 pin headers
- Power ON the BoosterPack
- Open serial console (Ex: Teraterm) on host PC, connect to COM port on which BoosterPack 
  Debug port is connected and setup for following configurations
   -- Baud rate     -  115200
   -- Data length   -  8 bit
   -- Parity        -  None
   -- Stop bits     -  1
   -- Flow control  -  None 
- Enable local echo on serial console

3.3.3.3	Test Execution
This test verifies CC26540 UART communication with host PC.

- Open CCS and connect to CC2650 using the relevant target configuration file.
- Load and run the program ble_uart_test.out
- Check the message ‘CC2650 UART Test’ is displayed on serial console of host PC
- Enter 10 characters on serial console and check that same is displayed on CCS console


Sample test log is shown below 

******************************
       CC2650 UART Test       
******************************
Writing Data to Serial Console
Write Successful

Enter 10 Characters on the Serial Console
Waiting for Data from Serial Console...

Characters Received:1234567890

UART Test Passed!




============
4. Appendix
============

Connecting to C5545 DSP from CCS
---------------------------------
- Connect micro USB cable to DEBUG micro-USB port of BoosterPack and host PC
- Open CCS IDE and launch target configuration for C5545
  Follow below steps to create target configuration file if needed
  -- Select CCS menu 'File->New->Target Configuration File'
  -- Give a name to the target configuration file and click 'Finish'
  -- In the new target configuration file created
     Select 'Connection' as 'Texas Instruments XDS100v2 USB Debug Probe'
     Select 'Board or Device' as TMS320C5545 and save the file
  -- Select 'View->Target Configurations' to open Target Configurations window
  -- Right click on the C5545 Target Configuration file created and select 'Launch Selected Configuration'
- After target is launched, select 'Run->Connect Target' to connect to the target
- After target is connected, select 'Run->Load->Load Program' to load the program
- After program is loaded, select 'Run->Resume' to start running the program

    

Connecting to CC2650 BLE from CCS
----------------------------------
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
- After program is loaded, select 'Run->Resume' to start running the program



Connecting to MSP432 LP from CCS
--------------------------------
- Connect micro USB cable to USB port of MSP432 LaunchPad and host PC
- Open CCS IDE and launch target configuration for MSP432
  Follow below steps to create target configuration file if needed
  -- Select CCS menu 'File->New->Target Configuration File'
  -- Give a name to the target configuration file and click 'Finish'
  -- In the new target configuration file created
     Select 'Connection' as 'Texas Instruments XDS110 USB Debug Probe'
     Select 'Board or Device' as MSP432P401R and save the file
  -- Select 'View->Target Configurations' to open Target Configurations window
  -- Right click on the MSP432 Target Configuration file created and select 'Launch Selected Configuration'
- After target is launched, select 'Run->Connect Target' to connect to the target
- After target is connected, select 'Run->Load->Load Program' to load the program
- After program is loaded, select 'Run->Resume' to start running the program
