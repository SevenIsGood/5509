/*
 *  Copyright (C) 2001, Spectrum Digital, Inc.  All Rights Reserved.
 */
 
#define DSP_CLKIN        12
#define NULLLOOP_CLK     12
/*Hardware Interrupts */
#define IRQ_EVT_RESET        (0)
#define IRQ_EVT_NMI          (1)

#define IRQ_EVT_INT0         (2)
#define IRQ_EVT_INT2         (3)
#define IRQ_EVT_TINT0        (4)
#define IRQ_EVT_RINT0        (5)
#define IRQ_EVT_RINT1        (6)

#define IRQ_EVT_XINT1        (7)
#define IRQ_EVT_MMC0         (7)
#define IRQ_EVT_MST0         (7)

#define IRQ_EVT_USB          (8)
#define IRQ_EVT_LCKINT       (8)

#define IRQ_EVT_DMAC1        (9)
#define IRQ_EVT_DSPINT       (10)

#define IRQ_EVT_INT3         (11)
#define IRQ_EVT_WDTIM        (11)
#define IRQ_EVT_TINT2        (11)

#define IRQ_EVT_RINT2        (12)
#define IRQ_EVT_UART         (12)

#define IRQ_EVT_XINT2        (13)
#define IRQ_EVT_MMC1         (13)
#define IRQ_EVT_MST1         (13)

#define IRQ_EVT_DMAC4        (14)
#define IRQ_EVT_DMAC5        (15)
#define IRQ_EVT_INT1         (16)
#define IRQ_EVT_XINT0        (17)
#define IRQ_EVT_DMAC0        (18)

#define IRQ_EVT_INT4         (19)
#define IRQ_EVT_RTC          (19)

#define IRQ_EVT_DMAC2        (20)
#define IRQ_EVT_DMAC3        (21)
#define IRQ_EVT_TINT1        (22)

#define IRQ_EVT_INT5         (23)
#define IRQ_EVT_I2C          (23)

typedef struct {
    int freq;        // DSP operating clock
    int clkin;       // DSP input clock
    int pllmult;     // PLL multiplier
    int plldiv;      // PLL divisor
    int clksperusec; // DSP clocks per usec
    int nullloopclk; // DSP clocks per null loop
} DSPCLK;

extern DSPCLK dspclk;

//  Read and write from an address
#define Read(addr) addr
#define Write(addr,data) addr = data

//  Set or clear all bits in the mask
#define ClearMask(addr,mask) addr = (addr & ~(mask))
#define SetMask(addr,mask) addr = (addr | (mask))  

//  Read and write data at addr, only bits in mask are affected
#define ReadMask(addr,mask) (addr & (mask))
#define WriteMask(addr,data,mask) addr = (addr & ~(mask)) | (data)

//  Read and write data at addr, shift data so that bit 0 of data is aligned
//  with lowest set bit in mask.  Only bits in mask are modified.  Example
//  with address 0x1000 initial contents = 0x4007:
//  WriteField(0x1000, 0x56, 0x0ff0) will set address 0x1000 to 0x4567.
#define ReadField(addr,mask) ((addr & (mask)) >> firstbit(mask)) 
#define WriteField(addr,data,mask) addr = (addr & ~(mask)) | (data << firstbit(mask))
unsigned short intEnableGlobal();
unsigned short intDisableGlobal();
void irqRestore(int intm);

extern int firstbit(unsigned short mask);
