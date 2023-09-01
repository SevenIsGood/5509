
MEMORY
{
    PAGE 0:

        MMR     : origin = 0000000h, length = 00000c0h 
        SPRAM   : origin = 00000c0h, length = 0000040h
        VECS    : origin = 0000100h, length = 0000100h
        DARAM0  : origin = 0000200h, length = 0003E00h
        DARAM1  : origin = 0004000h, length = 0004000h
        DARAM2  : origin = 0008000h, length = 0004000h
        DARAM3  : origin = 000c000h, length = 0004000h
        SARAM0  : origin = 0010000h, length = 0004000h
        SARAM1  : origin = 0014000h, length = 0005000h
        SARAM2  : origin = 0019000h, length = 0003000h
        SARAM3  : origin = 001c000h, length = 0004000h
        SARAM4  : origin = 0020000h, length = 0004000h
        SARAM5  : origin = 0024000h, length = 0004000h
        SARAM6  : origin = 0028000h, length = 0004000h
        SARAM7  : origin = 002c000h, length = 0004000h
        SARAM8  : origin = 0030000h, length = 0004000h
        SARAM9  : origin = 0034000h, length = 0004000h
        SARAM10 : origin = 0038000h, length = 0004000h
        SARAM11 : origin = 003c000h, length = 0004000h  
} 
SECTIONS
{
        .vectors  : {} > VECS   PAGE 0         /* interrupt vector table */
        .cinit   : {} > SARAM0 PAGE 0
        .text    : {} > SARAM1 PAGE 0
        isrs     : {} > SARAM2 PAGE 0

        .stack   : {} > DARAM0 PAGE 0
        .sysstack: {} > DARAM0 PAGE 0
        .sysmem  : {} > DARAM0 PAGE 0
        .data    : {} > DARAM1 PAGE 0
        .bss     : {} > DARAM1 PAGE 0
        .const   : {} > DARAM1 PAGE 0

        .cio     : {} > DARAM2 PAGE 0
        .csldata:  {} > DARAM0 PAGE 0
}       

