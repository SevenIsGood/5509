TIesr for C5545BP
-----------------

In order to run the TIesr demo on the C5545BP, please ensure the correct macro #define C5545_BSTPCK is defined in C55XXCSL_LP at C:\ti\c55_lp\c55_csl_3.07\inc\csl_general.h.

The memory models will have to be set to HUGE in C55XXCSL_LP and atafs_bios_drv_lib.

The onboard electret microphone is used for the demo. Not the headphone microphone.

When running the demo on the C5545BP, there is no need to push any of the buttons to activate the recognizer. The demo activates as soon as it starts running awaiting the keyword "T I Voice Trigger". The OLED will display this phrase if successfully recognized.

In order to clear the OLED, some sort of background sound such as a clap or a snap of fingers will clear the screen.