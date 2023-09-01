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

        .sect ".vectors"

        .ref _c_int00           ; C entry point

        .if (.MNEMONIC)

rsv:                            ; reset vector	;SINT0
        B _c_int00                              ; branch to C entry point
        NOP
        .align 8
nmi:									;SINT1
    b $
    nop
    .align 8
int0:									;SINT2
    b $
    nop
    .align 8
int2:									;SINT3
    b $
    nop
	.align 8
	.ref _isrTimer0
TINT0:									;SINT4
    b _isrTimer0
    nop
	.align 8
	.ref _mcbspRcv0
RINT0:									;SINT5
    b _mcbspRcv0
    nop
	.align 8
RINT1:									;SINT6
    b $
    nop
    .align 8
XINT1_MMCSD1:							;SINT7
	b $
    nop
    .align 8
USB:									;SINT8
    b $
    nop
    .align 8
DMAC1:									;SINT9
    b $
    nop
    .align 8
DSPINT:									;SINT10
    b $
    nop
    .align 8
INT3_WDTINT:							;SINT11
    b $
    nop
    .align 8
RINT2:									;SINT12
    b $
    nop
    .align 8
XINT2_MMCSD2:							;SINT13
    b $
    nop
    .align 8
DMAC4:									;SINT14
    b $
    nop
    .align 8
DMAC5:									;SINT15
    b $
    nop
    .align 8
INT1:									;SINT16
    b $
    nop
	.align 8
	.ref _mcbspXimt0
XINT0:									;SINT17
     b _mcbspXimt0
    nop
	.align 8
	.ref _isrDma0
DMAC0:									;SINT18
    b _isrDma0
    nop
    .align 8
INT4_RTC:								;SINT19
    b $
    nop
    .align 8
DMAC2:									;SINT20
    b $
    nop
    .align 8
DMAC3:									;SINT21
    b $
    nop
    .align 8
TINT1:									;SINT22
    b $
    nop
    .align 8
IIC:									;SINT23
    b $
    nop
    .align 8
BERR:									;SINT24
    b $
    nop
    .align 8
DLOG:									;SINT25
    b $
    nop
    .align 8
RTOS:									;SINT26
    b $
    nop
    .align 8
Soft_int0:								;SINT27
    b $
    nop
    .align 8
Soft_int1:								;SINT28
    b $
    nop
    .align 8
Soft_int2:								;SINT29
    b $
    nop
    .align 8		
	
    	
        .else

rsv:                            ; reset vector
        goto _c_int00                           ; branch to C entry point
        NOP
        .align 8


        .endif
