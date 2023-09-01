
MEMORY
{
 PAGE 0:
    PROG0(RWX)    : origin = 04000h   length = 08000h
    P_VECT(RIX)   : origin = 0FF80h   length = 00080h
 PAGE 1:
    DATA0(RW)     : origin = 0C000h   length = 04000h
} 

SECTIONS
{

  .vectors              : { } > P_VECT      PAGE 0
  .text                 : { } > PROG0       PAGE 0
  .cinit                : { } > PROG0       PAGE 0   
  .csldata              : { } > DATA0       PAGE 1
  .stack                : { } > DATA0       PAGE 1  
  .sysstack             : { } > DATA0       PAGE 1
  .bss                  : { } > DATA0       PAGE 1   
  .cio                  : { } > DATA0       PAGE 1
  .const                : { } > DATA0       PAGE 1
  .sysmem               : { } > DATA0       PAGE 1  
}       

