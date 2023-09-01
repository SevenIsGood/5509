TIesr for C5535eZdsp
--------------------

In order to run the TIesr demo on the C5535eZdsp, please ensure the correct macro #define C5535_EZDSP is defined in C55XXCSL_LP at C:\ti\c55_lp\c55_csl_3.07\inc\csl_general.h.

The memory models will have to be set to HUGE in C55XXCSL_LP and atafs_bios_drv_lib.

When running the demo on the C5535 eZdsp, SW2 will need to be pressed to activate the recognizer. The LED, DS2 will flash rapidly indicating that the recognizer is awaiting the keyword "T I Voice Trigger". The OLED will display this phrase if successfully recognized.

In order to clear the OLED, some sort of background sound such as a clap or a snap of fingers will clear the screen.