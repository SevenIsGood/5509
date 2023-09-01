/********************************************************************* 
*    Property of Texas Instruments Incorporated, Copyright 2007
*    All rights reserved
**********************************************************************
*    Bootloader.cmd
**********************************************************************
* Bootloader linker cmd file for memory configuration
**********************************************************************/

MEMORY
{
  PAGE 0:
		/* SARAM[0..31] = Byte Addresses 0x10000 -> 0x4FFFF */
        //SAROM_BOOT     (RW) : origin = 004c000h length = 002000h fill=0 /* on-chip SARAM[30] = Byte Addresses 0x4C000 -> 0x4DFFF  */
        SARAM_BOOT    (RW) : origin = 004e000h length = 001ffch /* on-chip SARAM[31] = Byte Addresses 0x4E000 -> 0x4FFFB (reserved for Bootloader) */
        SARAM_API_PTR (RW) : origin = 004fffch length = 000004h /* on-chip SARAM[31] = Byte Addresses 0x4FFFC -> 0x4FFFF (reserved for API-Pointer) */

        SAROM_API        (RX) : origin = 004C000h length = 0003e8h fill=0  /* API */
        SAROM_ID        (RX) : origin =  004C400h length = 000004h fill=0  /* Bootloader ID */
        SAROM_VEC        (RX) : origin = 004C500h length = 000100h fill=0  /* Bootloader interrupt-vector-table */
        SAROM_BOOT       (RX) : origin = 004C600h length = 001a00h  /* Bootloader code */
}


SECTIONS
{
    api_ptr  : {}  > SARAM_API_PTR

	api_vect:      > SAROM_API align(4)
	api_lut:       > SAROM_API align(4)
	
    .iic:          > SAROM_BOOT align(2)
	.text:         > SAROM_BOOT
	.switch:       > SAROM_BOOT
	.cinit:        > SAROM_BOOT
	.const:        > SAROM_BOOT
	vectors:       > SAROM_VEC
	.bootid:	   > SAROM_ID
	
	.stack :
	    {
	        * (.stack)
	    }          > SARAM_BOOT     ALIGN = 4
	.sysstack :    > SARAM_BOOT     ALIGN = 4
	.bss :
	    {
	        * (.bss)
	    }          > SARAM_BOOT     ALIGN = 4
}
