*
* Copyright (C) 2003 Texas Instruments Incorporated
* All Rights Reserved
*
*
*---------vectors_gpt1.s55---------
*
* Assembly file to set up interrupt vector table
*
		.sect ".vectors"
 
*------------------------------------------------------------------------------
* Global symbols defined here and exported out of this file
*------------------------------------------------------------------------------
		.global _vecs
*------------------------------------------------------------------------------
* Global symbols referenced in this file but defined somewhere else. 
* Remember that your interrupt service routines need to be referenced here.
*------------------------------------------------------------------------------
		.ref _c_int00
		
_vecs:	.ivec	_c_int00,use_reta

NMI:    .ivec  1
	
INT0:   .ivec  2

INT2:   .ivec  3   		;fir ISR

TINT0:  .ivec  4

RINT0:  .ivec  5

RINT1:  .ivec  6

XINT1:  .ivec  7

LCKINT: .ivec  8

DMAC1:  .ivec  9

DSPINT: .ivec 10

INT3:   .ivec 11

UART:   .ivec 12 ;_UART_ISRHandler

XINT2:  .ivec 13

DMAC4:  .ivec 14

DMAC5:  .ivec 15

INT1:   .ivec 16

XINT0:  .ivec 17

DMAC0:  .ivec 18

INT4:   .ivec 19

DMAC2:  .ivec 20

DMAC3:  .ivec 21

TINT1:  .ivec 22

INT5:   .ivec 23

*------------------------------------------------------------------------------
