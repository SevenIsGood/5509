-heap  0x2000
-stack 0x2000
-c
MEMORY
{
    IRAM:       o = 000000c0h   l = 00040000h
}

SECTIONS
{
    .text       >       IRAM      fill = 0
    .stack      >       IRAM      fill = 0
    .bss        >       IRAM      fill = 0
    .cinit      >       IRAM      fill = 0
    .const      >       IRAM      fill = 0
    .data       >       IRAM      fill = 0
    .far        >       IRAM      fill = 0
    .switch     >       IRAM      fill = 0
    .sysmem     >       IRAM      fill = 0
    .tables     >       IRAM      fill = 0
    .cio        >       IRAM      fill = 0
}


