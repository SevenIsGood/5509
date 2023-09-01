;
;  Copyright 2003 by Texas Instruments Incorporated.
;  All rights reserved. Property of Texas Instruments Incorporated.
;  Restricted rights to use, duplicate or disclose this code are
;  granted through contract.
;  
;
; "@(#) DSP/BIOS 4.90.270 06-11-03 (barracuda-m10)"
        .sect ".vectors"
        .global _VECSTART
        .ref _c_int00
      .def nmi, int0, int1, int2, int3, int4, int5, int6
      .def int7, int8, int9, int10, int11, int12, int13
      .def int14, int15, int16, int17, int18, int19, int20
      .def int21, int22, int23, int24, int25, int26, int27
      .def int28, int29

_VECSTART:
        .ivec _c_int00,c54x_stk
nmi     .ivec no_isr
        nop_16
int0    .ivec no_isr
        nop_16
int1    .ivec no_isr
        nop_16
int2    .ivec no_isr
        nop_16
int3    .ivec no_isr
        nop_16
int4    .ivec no_isr
        nop_16
int5    .ivec no_isr
        nop_16
int6    .ivec no_isr
        nop_16
int7    .ivec no_isr
        nop_16
int8    .ivec no_isr
        nop_16
int9    .ivec no_isr
        nop_16
int10   .ivec no_isr
        nop_16
int11   .ivec no_isr
        nop_16
int12   .ivec no_isr
        nop_16
int13 .ivec no_isr
        nop_16
int14   .ivec no_isr
        nop_16
int15   .ivec no_isr
        nop_16
int16   .ivec no_isr
        nop_16
int17   .ivec no_isr
        nop_16
int18   .ivec no_isr
        nop_16
int19   .ivec no_isr
        nop_16
int20   .ivec no_isr
        nop_16
int21   .ivec no_isr
        nop_16
int22   .ivec no_isr
        nop_16
int23   .ivec no_isr
        nop_16
int24   .ivec no_isr
        nop_16
int25   .ivec no_isr
        nop_16
int26   .ivec no_isr
        nop_16
int27   .ivec no_isr
        nop_16
int28   .ivec no_isr
        nop_16
int29   .ivec no_isr
        nop_16


        .text
        .def no_isr
no_isr:
        b #no_isr
