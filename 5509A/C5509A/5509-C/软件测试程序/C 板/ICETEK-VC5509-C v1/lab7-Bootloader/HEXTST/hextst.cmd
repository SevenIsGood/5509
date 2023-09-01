-w
MEMORY {
 
    DATA(RWI):  origin = 0x2200,        len = 0x1000
    PROG:       origin = 0x200,         len = 0x2000
    VECT:       origin = 0xd000,        len = 0x100
}

SECTIONS
{
    .text:    {} > PROG 
    .vectors: {} > VECT 
    .trcinit: {} > PROG 
    .gblinit: {} > PROG 
     frt:     {} > PROG 
    
    .cinit:   {} > PROG 
    .pinit:   {} > PROG 
    .sysinit: {} > PROG 
    .bss:     {} > DATA 
    .far:     {} > DATA 
    .const:   {} > DATA 
    .switch:  {} > DATA 
    .sysmem:  {} > DATA 
    .cio:     {} > DATA 
    .MEM$obj: {} > DATA 
    .sysheap: {} > DATA 
    .sysstack {} > DATA 
    .stack:   {} > DATA 
    
}



