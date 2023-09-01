Example code to verify McBSP 32 channel mode and companding

MCBSP 32-CH COMPANDING


TEST DESCRIPTION:
This test is to verify the McBSP in 32 channel mode, and companding using ALAW and MULAW.
  

TEST PROCEDURE:
This test configures PLL to be 100MHz

This is an External Loop-back Test
The following connections are required on J14 of EVM5517

1. McBSP_CLKX(pin#12) to  pin 20 on J15 (or) McBSP_CLKR(Test point -TP#15)
2. McBSP_FSX(pin#8) to McBSP_FSR(pin#1)
3. McBSP_DX(pin#22) to McBSP_DR(pin#21)


USE_DMA macro
=============
Uncomment this to run in DMA mode.
Commenting this makes the test in POLL mode.


USE_ALAW_COMPANDING macro
=========================
Uncomment this macro to enable ALAW companding
The test case transmits data with ALAW compression.
The test case receiuves data with ALAW expanding.
The data is externally looped back.
NOTE:Please comment the macro USE_MULAW_COMPANDING

USE_MULAW_COMPANDING macro
==========================
Uncomment this macro to enable MULAW companding
The test case transmits data with MULAW compression.
The test case receiuves data with MULAW expanding.
The data is externally looped back.
NOTE:Please comment the macro USE_ALAW_COMPANDING


TEST RESULT:

Verification of xmt & rcv buffers
=================================
As both ALAW and MULAW compression are lossy, we cant do a byte matching to verify the results.
Hence we use the Graph Tool of CCS to do a visual comparison.

The data transmitted is 16-bit sine wave data.
The graph plotted with this will be a sine wave.

***The verification involves plotting graphs from xmt & rcv buffer & doing a VISUAL comparison***

How to plot the graph using CCS Graph Tool
==========================================
1. Open the Graph Tool with View->Graph->Time/Frequency
2. Use the below configuration on the Graph Property Dialog
3. Give 'xmt' as the 'Start Address'. Click OK
4. Right click and select 'Auto Scale'.
NOTE:If you do not see Auto Scale, it means that you are already on 'Auto Scale'
5. Open the Graph Tool again as in Step-1 & Use the same configuration
6. Give 'rcv' as the 'Start Address'. Click OK
7. Right click and select 'Auto Scale' as in Step-4
8. Compare both the graphs side-by-side


Graph Property Dialog Configuration
===================================

Display Type 				- Single Time
Graph Title 				- Graphical Display
Start Address 				- 
Page						- DATA
Acquisition Buffer Size		- 1024
Index Increment 			- 1
Display Data Size 			- 1024
DSP Data Type				- 16-bit signed integer
Q-value						- 0
Sampling Rate (Hz)			- 1
Plot Data From				- Left To Right
Left-shifted Data Display	- Yes
Autoscale					- On
DC Value					- 0
Axes Display				- On
Time Display Unit			- s
Status Bar Display			- On
Magnitude Display Scale		- Linear
Data Plot Style				- Line
Grid Style					- Zero Line
Cursor Mode					- No Cursor


     
