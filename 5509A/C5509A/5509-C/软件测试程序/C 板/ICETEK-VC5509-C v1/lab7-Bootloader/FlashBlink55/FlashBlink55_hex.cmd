FlashBlink55.out
-o FlashBlink.hex
-map FlashBlink.mxp
-m2
-v5510:2
-boot
-parallel16
ROMS
{
	PAGE 0 : ROM : o=0x400000, l=0x100000
}

