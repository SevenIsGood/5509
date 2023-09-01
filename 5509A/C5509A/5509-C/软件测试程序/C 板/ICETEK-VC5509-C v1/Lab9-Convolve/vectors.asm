;
;  Copyright 2003 by Texas Instruments Incorporated.
;  All rights reserved. Property of Texas Instruments Incorporated.
;  Restricted rights to use, duplicate or disclose this code are
;  granted through contract.
;  
;
; "@(#) DSP/BIOS 4.90.270 06-11-03 (barracuda-m10)"
;
;  ======== vectors.asm ========
;  Plug in the entry point at RESET in the interrupt vector table
;
;
;  ======== unused ========
;  plug inifinite loop -- with nested branches to
;  disable interrupts -- for all undefined vectors
;

        .sect ".vectors"

        .ref _c_int00           ; C entry point

        .if (.MNEMONIC)

rsv:                            ; reset vector
        B _c_int00                              ; branch to C entry point
        NOP
        .align 8

        .else

rsv:                            ; reset vector
        goto _c_int00                           ; branch to C entry point
        NOP
        .align 8


        .endif
