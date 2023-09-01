/*
 *  Copyright (C) 2001, Spectrum Digital, Inc.  All Rights Reserved.
 */

#include "5509.h"
#include "util.h"
DSPCLK dspclk;
void pllInit(int freq)
{
    int i;
    ioport unsigned int *clkmd;
    ioport unsigned int *sysr;
    clkmd=(unsigned int *)0x1c00;
    
    sysr=(unsigned int *)0x07fd;
    
    // Calculate PLL multiplier values (only integral multiples now)
    dspclk.clkin = DSP_CLKIN;
    dspclk.pllmult = (freq *2)/ dspclk.clkin;
    
    if(dspclk.pllmult>= 32)dspclk.pllmult=31; 
   // dspclk.nullloopclk = NULLLOOP_CLK;

    // Turn the PLL off
    *clkmd &= ~0x10; //pll enable = 0;
    for(i=*clkmd&1; i!= 0 ;i=*clkmd&1);

    // Initialize PLL flags
    *clkmd &= ~0x4000; // iai=0;
    *clkmd |= 0x2000;  // iob=1;
      // Set the multiplier/divisor
  //  WriteMask(pCMOD -> clkmd,
  //      CLKMD_PLLDIV_1 | CLKMD_BYPASSDIV_1,
  //      CLKMD_PLLDIV_MASK | CLKMD_BYPASSDIV_MASK);
    *clkmd &= ~0xc;
    *clkmd |= 4;
    *clkmd &= ~0x60;
    *clkmd |= 0x20;
    //WriteField(pCMOD -> clkmd, dspclk.pllmult, CLKMD_PLLMULT_MASK);
    *clkmd &= ~0x0f80;
    *clkmd |= dspclk.pllmult<<7;
    // Enable the PLL and wait for lock
    *clkmd|=0x10;
    for(i=0;i<10;i++);
    for(i=*clkmd&1; i!= 1 ;i=*clkmd&1);
    
    *sysr=2;

    
}

void intEnable(unsigned short EventId)
{
   	unsigned short bit,reg,prev,value,mask;
	int old_intm;
   	bit= EventId & 0xfu; 
   	reg=((EventId & 0x10) >> 4);
   	mask= 0x1ul<<bit;
   	old_intm = intDisableGlobal();
	prev = (reg) ? IER1 : IER0;
   	value= (prev | mask);
   	if (reg) 
     	IER1 = value;
   	else 
    	IER0 = value; 
	irqRestore(old_intm);
}

void intDisable(unsigned short EventId)
{
  	unsigned short bit, reg, mask;
  	int old_intm;
  	bit= EventId & 0xfu; 
  	reg=((EventId & 0x10) >> 4);
  	mask= 0x1ul<<bit;
  	old_intm = intDisableGlobal();
  	if (reg)
    	IFR1 = mask;
  	else
    	IFR0 = mask;
	irqRestore(old_intm);
}

void irqRestore(int intm)
{
	asm(" BCC l1,T0 != #0");
	asm(" NOP");
	asm(" BCLR ST1_INTM");
	asm(" B l2");
	asm("l1: NOP");
	asm(" BSET ST1_INTM");
	asm("l2: NOP");
}


unsigned short intEnableGlobal()
{
	IVPD=0xd0;
	IVPH=0xd0;
	IFR0=0xffff;
	asm(" MOV #3, AR3");
	asm(" MOV *AR3,AC0");
	asm(" BFXTR #2048,AC0,T0");
	asm(" NOP");
    asm(" BCLR INTM");
}

unsigned short intDisableGlobal()
{
	asm(" MOV #3,AR3");
	asm(" MOV *AR3,AC0");
	asm(" BFXTR #2048,AC0,T0");
	asm(" NOP");
    asm(" BSET INTM");
}

void intSetVec(unsigned long addr)
{
    IVPD = (unsigned short)(addr >> 8);
    IVPH = IVPD;
}

void intInstallHandler(int vecno, void(*handler)())
{
    unsigned long handler_addr, vec_addr;
    unsigned short *pdata;

    // Calculate address of vector
    vec_addr = IVPD;
    vec_addr = vec_addr << 7;
    vec_addr += vecno * 4;

    // Insert branch to handler in vector location
    handler_addr = (unsigned long)handler;
    pdata = (unsigned short *)vec_addr;
    *pdata++ = ((handler_addr >> 16) & 0x00ff) | 0xea00;
    *pdata++ = handler_addr & 0xffff;
    *pdata++ = 0x5e80;
    *pdata++ = 0x5f80;
}

void SWDelayUsec(unsigned int usec)
{
    unsigned int i, j, loopsperusec;

    loopsperusec = dspclk.freq / dspclk.nullloopclk;
    for (i=0;i<usec;i++)
        for (j = 0; j < loopsperusec; j++);
}

void SWDelayMsec(unsigned int msec)
{
    unsigned int i;
    for (i=0;i<msec;i++)
        SWDelayUsec(1000);
}
void delay(int k)
{
   while(k--);
}
void ddelay(unsigned int kt)
{
	unsigned int jj,kk;
	for(jj=0;jj<65535;jj++)
	{
		kk = kt;
		while(kk>0)
		{
			kk--;
		}
	}
}
int firstbit(unsigned short mask)
{
    int shiftamt;
    unsigned short bit;

    // Find offset of first bit in mask
    bit = 1;
    for (shiftamt = 0; shiftamt < 16; shiftamt++) {
        if (bit & mask)
            break;
        bit = bit << 1;
    }

    return shiftamt;
}
