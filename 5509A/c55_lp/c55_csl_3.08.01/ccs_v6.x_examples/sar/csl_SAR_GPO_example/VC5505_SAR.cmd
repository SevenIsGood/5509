-stack    0x2000                /* PRIMARY STACK SIZE    */
-sysstack 0x1000                /* SECONDARY STACK SIZE  */
-heap     0x2000                /* HEAP AREA SIZE        */  

MEMORY
{
    MMR     (RW) : origin = 0000000h length = 0000C0h /* MMRs */
    VEC     (RX) : origin = 00000C0h length = 000300h /* on-chip ROM vectors */
    DARAM   (RW) : origin = 0000400h length = 00FBFFh /* on-chip DARAM  */
    SARAM   (RW) : origin = 0010000h length = 03FFFFh /* on-chip SARAM  */
}

SECTIONS
{
   vectors(NOLOAD)
   vector     : > VEC    ALIGN = 256
  .text      : > SARAM 
  .data      : > SARAM  
  .cinit     : > SARAM 
  .switch    : > SARAM
  .stack     : > SARAM 
  .sysstack  : > SARAM 
  .bss       : > SARAM
  .sysmem    : > SARAM
  .const     : > SARAM
  .cio	     : > SARAM
}
