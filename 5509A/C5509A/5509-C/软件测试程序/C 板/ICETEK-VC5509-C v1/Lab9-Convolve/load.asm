;
;  Copyright 2003 by Texas Instruments Incorporated.
;  All rights reserved. Property of Texas Instruments Incorporated.
;  Restricted rights to use, duplicate or disclose this code are
;  granted through contract.
;  
;
; "@(#) DSP/BIOS 4.90.270 06-11-03 (barracuda-m10)"
;
;  ======== load.asm ========
;
;  C-callable interface to assembly language utility functions for the
;  volume example.

    .mmregs

    .global _load

    .text

N   .set        1000          ; multiple of four.

;
;  ======== _load ========
;  This function simulates a load on the DSP by executing N * loadValue
;  instructions, where loadValue is the input parameter to load().
;
;  true execution time: (loadValue)*((N/4 + 25) + 13
;
;  Preconditions:        t0 = loadValue
;
;  Postconditions:       none.
;
;  Modifies:             t0
;
;  void load(unsigned int loadValue)
;

    .if (.MNEMONIC)
    
_load:
    BCC done, T0 == #0          ; done if t0 = 0
    MOV T0, T1                  ; t1 = loopCount
    MOV #(N/4-2), T0            ; t0 = (N / 4) - 2
loop:
    MOV T0, BRC0                ; initialize count register for block rpt
    NOP
    NOP
    RPTB $1
    NOP
    NOP
    NOP
$1  NOP

    SUB #1, T1
    BCC loop, T1 != 0

done:
    RET                         ;/* end load() */

    .else

_load:
    if(t0 == #0) goto done      ; done if t0 = 0
    t1 = t0                     ; t1 = loopCount
    t0 = #(N/4-2)               ; t0 = (N / 4) - 2
loop:   
    brc0 = t0                   ; initialize count register for block rpt
    nop
    nop
    blockrepeat {
    nop
    nop
    nop
    nop
    }
    t1 = t1 - #1
    if (t1 != #0) goto loop

done:
        return                  ;/* end load() */
        
    .endif
