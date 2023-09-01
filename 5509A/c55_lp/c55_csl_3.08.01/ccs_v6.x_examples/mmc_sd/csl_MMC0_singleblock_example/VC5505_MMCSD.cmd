-stack 		0x1000   /* PRIMARY STACK SIZE    */
-sysstack	0x1000   /* SECONDARY STACK SIZE  */
-heap       0x3F80  /* HEAP AREA SIZE        */  

MEMORY
{
    MMR     (RW) : origin = 0000000h length = 0000C0h /* MMRs */
    VEC     (RX) : origin = 00000C0h length = 000300h /* on-chip ROM vectors */
    DARAM   (RW) : origin = 0000400h length = 00FBFFh /* on-chip DARAM  */

    SARAM_0 (RW)  : origin = 0010000h length = 0011000h  	/* 8 blocks for all */
/*    SARAM_1 (RW)  : origin = 0012000h length = 002000h*/  /* on-chip SARAM 1 */
/*    SARAM_2 (RW)  : origin = 0014000h length = 002000h */ /* on-chip SARAM 2 */
/*    SARAM_3 (RW)  : origin = 0016000h length = 002000h*/  /* on-chip SARAM 3 */

/*    SARAM_4 (RW)  : origin = 0018000h length = 002000h */ /* on-chip SARAM 4 */
/*    SARAM_5 (RW)  : origin = 001A000h length = 002000h */ /* on-chip SARAM 5 */
/*    SARAM_6 (RW)  : origin = 001C000h length = 002000h */ /* on-chip SARAM 6 */
/*    SARAM_7 (RW)  : origin = 001e000h length = 002000h */ /* on-chip SARAM 7 */

      SARAM_8 (RW)  : origin = 0021000h length = 008000h  /* 4 blocks for sysmem */
/*    SARAM_9 (RW)  : origin = 0022000h length = 002000h  */
/*    SARAM_10 (RW) : origin = 0024000h length = 002000h  */
/*    SARAM_11 (RW) : origin = 0026000h length = 002000h  */
                                                                           
      SARAM_12 (RW) : origin = 0029000h length = 028000h
/*     SARAM_13 (RW) : origin = 002A000h length = 002000h   */
/*     SARAM_14 (RW) : origin = 002C000h length = 002000h   */
/*     SARAM_15 (RW) : origin = 002e000h length = 002000h   */
/*                                                          */
/*     SARAM_16 (RW) : origin = 0030000h length = 002000h   */
/*     SARAM_17 (RW) : origin = 0032000h length = 002000h   */
/*     SARAM_18 (RW) : origin = 0034000h length = 002000h   */
/*     SARAM_19 (RW) : origin = 0036000h length = 002000h   */
/*                                                          */
/*     SARAM_20 (RW) : origin = 0038000h length = 002000h   */
/*     SARAM_21 (RW) : origin = 003A000h length = 002000h   */

/*	   SARAM_22 (RW) : origin = 003C000h length = 002000h   */
/*     SARAM_23 (RW) : origin = 003e000h length = 002000h   */
/*     SARAM_24 (RW) : origin = 0040000h length = 002000h   */
/*     SARAM_25 (RW) : origin = 0042000h length = 002000h   */
/*     SARAM_26 (RW) : origin = 0044000h length = 002000h   */
/*     SARAM_27 (RW) : origin = 0046000h length = 002000h   */
/*     SARAM_28 (RW) : origin = 0048000h length = 002000h   */
/*     SARAM_29 (RW) : origin = 004A000h length = 002000h   */
/*     SARAM_30 (RW) : origin = 004C000h length = 002000h   */
/*     SARAM_31 (RW) : origin = 004E000h length = 002000h   */

	
}
 
SECTIONS
{
     vectors(NOLOAD)
     vector     : > VEC    ALIGN = 256
    .text       : > SARAM_0  ALIGN = 4
    .stack      : > SARAM_0  ALIGN = 4
    .sysstack   : > SARAM_0  ALIGN = 4
    .data       : > SARAM_0
    .bss        : > SARAM_0, fill = 0
    .cinit 		: > SARAM_0
    .sysmem 	: > SARAM_8
	.const 		: > SARAM_0
    .switch		: > SARAM_0
    .cio    	: > SARAM_0
    .buffer1   	: > SARAM_12
	.buffer2   	: > SARAM_12
}
