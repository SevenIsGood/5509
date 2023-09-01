-stack 		0x2000   /* PRIMARY STACK SIZE    */
-sysstack	0x2000   /* SECONDARY STACK SIZE  */
-heap       0x3F80   /* HEAP AREA SIZE        */  

MEMORY
{
    MMR     (RW) : origin = 0000000h length = 0000C0h /* MMRs */
    VEC     (RX) : origin = 00000C0h length = 000300h /* on-chip ROM vectors */
    DARAM   (RW) : origin = 0000400h length = 00FBFFh /* on-chip DARAM  */
    SARAM   (RW) : origin = 0010000h length = 03DFFFh /* on-chip SARAM -- ignoring SARAM31 for UHPI BFM data transfer  */
}
 
SECTIONS
{
     vectors(NOLOAD)
     vector     : > VEC    ALIGN = 256
    .text       : > SARAM  ALIGN = 4
    .stack      : > SARAM  ALIGN = 4
    .sysstack   : > SARAM  ALIGN = 4
    .data       : > SARAM
    .bss        : > SARAM, fill = 0
	.cinit 		: > SARAM
	.const 		: > SARAM
	.switch		: > SARAM
	.sysmem 	: > SARAM
	.cio    	: > SARAM
	.buffer1   	: > SARAM
	.buffer2   	: > SARAM
}