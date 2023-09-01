
hextst.out

-map hextst.mxp
-o hextst.hex

-m2

-v5510:1

-boot
-emif32

ROMS
{
	PAGE 0 : ROM : o=0x400000, l=0x100000
}

/*
SECTIONS
{
   .vectors   boot
   .stack     boot
   .text      boot
   .cinit     boot
   .cio       boot
   .switch    boot
   .bss       boot
   .const     boot
   .sysmem    boot
}
*/