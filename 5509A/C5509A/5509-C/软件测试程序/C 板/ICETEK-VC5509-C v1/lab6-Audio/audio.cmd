-w
-stack 500
-sysstack 500
-lrts55x.lib
MEMORY                   
{
    RAM:  origin = 07000h    length = 06000h
    ROM:  origin = 00100h    length = 06F00h
    SND:  origin = 0D000h    length = 12000h
}

SECTIONS                 
{
    .text:    >   ROM
    .data:    >   RAM 
    .cinit    >   ROM 
    .bss:     >   RAM
    .stack    >   RAM
    .sysstack >   RAM
    sounds    >   SND
}

