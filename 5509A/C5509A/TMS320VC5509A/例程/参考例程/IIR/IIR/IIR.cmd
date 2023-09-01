-w
-stack 500
-sysstack 500
-l rts55x.lib

MEMORY 
{
    DARAM:	o=0x100,	l=0x7f00
    VECT :  o=0x8000,	l=0x100
    DARAM2: o=0x8100,   l=0x200
    DARAM3: o=0x8300,	l=0x7d00
    SARAM:  o=0x10000,	l=0x30000
    SDRAM:	o=0x40000,	l=0x3e0000
}

SECTIONS
{
    .text:    {} > DARAM 
    .vectors: {} > VECT 
    .trcinit: {} > DARAM 
    .gblinit: {} > DARAM 
     frt:     {} > DARAM 
    
    .cinit:   {} > DARAM 
    .pinit:   {} > DARAM 
    .sysinit: {} > DARAM 
    .bss:     {} > DARAM3 
    .far:     {} > DARAM3 
    .const:   {} > DARAM3 
    .switch:  {} > DARAM3 
    .sysmem:  {} > DARAM3 
    .cio:     {} > DARAM3 
    .MEM$obj: {} > DARAM3 
    .sysheap: {} > DARAM3 
    .sysstack {} > DARAM3 
    .stack:   {} > DARAM3 
    
}


