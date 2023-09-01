-stack    0x1000      /* Primary stack size   */
-sysstack 0x1000      /* Secondary stack size */
-heap     0x2000      /* Heap area size       */

-c                    /* Use C linking conventions: auto-init vars at runtime */
-u _Reset             /* Force load of reset interrupt handler                */

MEMORY
{
    MMR     (RW) : origin = 0000000h length = 0000c0h /* MMRs */
    VEC     (RX) : origin = 00000c0h length = 000300h  /*on-chip ROM vectors */
    DARAM_0 (RW) : origin = 0000400h length = 001A00h /* on-chip DARAM 0 */
    DARAM_1 (RW) : origin = 0002000h length = 008000h /* on-chip DARAM 1 */
    DARAM_2 (RW) : origin = 000A000h length = 008000h /* on-chip DARAM 2 */
    DARAM_3 (RW) : origin = 0012000h length = 008000h /* on-chip DARAM 3 */
    SARAM_0 (RW) : origin = 001A000h length = 008000h /* on-chip SARAM 5-8 */
    SARAM_1 (RW) : origin = 0022000h length = 008000h /* on-chip SARAM 9-12 */
    SARAM_2 (RW) : origin = 002A000h length = 008000h /* on-chip SARAM 13-16 */
    SARAM_3 (RW) : origin = 0032000h length = 007000h /* on-chip SARAM 17-20 */

 /*   SAROM_0 (RX) : origin = 0ff0000h length = 008000h */ /* on-chip ROM 0 */
/*    SAROM_1 (RX) : origin = 0ff8000h length = 007f00h  on-chip ROM 1 */
    /*VECS    (RX) : origin = 0ffff00h length = 000300h *//* on-chip ROM vectors */
	
}
 
SECTIONS
{
	vectors(NOLOAD)
    /*intvec	: > VECS  ALIGN = 256*/
	vector      : > VEC    ALIGN = 256
    .text       : > SARAM_2 ALIGN = 4
    .stack      : > SARAM_3 
    .sysstack   : > SARAM_3 
    
    .data       : > SARAM_1 
    .bss        : > SARAM_1 fill=0
	.const 		: > SARAM_1 
	.sysmem 	: > SARAM_1 
	.cio    	: > SARAM_1 
	/*sysmem  	: > SARAM_1 */


    .cinit 		: > SARAM_2 
	.pinit 		: > SARAM_2 

	.buffer1   	: > SARAM_0 
	.buffer2   	: > SARAM_0 
}
