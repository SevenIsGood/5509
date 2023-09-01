#include "5509.h"
#include "main.h"
#include "mcbsp.h"
void enableXimtInt()
{
	IER1|=0x02;
	DBIER1 |=0x02;
}
void enableRcvInt()
{
	IER0|=0x20;
	DBIER0 |=0x20;
}
void McBSP0_InitSlave()
{
   PC55XX_MCSP pMCBSP0 = (PC55XX_MCSP)C55XX_MSP0_ADDR;
    
     // Put the MCBSP in reset
    pMCBSP0 -> spcr1=0;
    pMCBSP0 -> spcr2=0;

    pMCBSP0 -> spcr1=0x4080;
    pMCBSP0 -> spcr2=0x00c0;   
    // Config frame parameters (32 bit, single phase, no delay)
    /*pMCBSP0 -> xcr1= XWDLEN1_32;
    pMCBSP0 -> xcr2= XPHASE_SINGLE | XDATDLY_0;
    pMCBSP0 -> rcr1= RWDLEN1_32;
    pMCBSP0 -> rcr2= RPHASE_SINGLE | RDATDLY_0;*/
	pMCBSP0 -> xcr1= 0x0140;
    pMCBSP0 -> xcr2= 0x0041;
    pMCBSP0 -> rcr1= 0x0140;
    pMCBSP0 -> rcr2= 0x0041;
	pMCBSP0 ->srgr1=0x0001;
	pMCBSP0 ->srgr2=0x2000;
    // Disable int frame generation and enable slave w/ext frame signals on FSX
    // Frame sync is active high, data clocked on rising edge of clkx
    pMCBSP0 -> pcr= PCR_CLKXP|PCR_CLKRP;

    // Bring transmitter and receiver out of reset
	enableXimtInt();
	enableRcvInt();
    SetMask(pMCBSP0 -> spcr2, SPCR2_XRST);
    SetMask(pMCBSP0 -> spcr1, SPCR1_RRST); 
}
void configMbsp(PC55XX_MCSP mcbspHadle,C55XX_MCSP* mcbspConfig)
{
	mcbspHadle->spcr1=mcbspConfig->spcr1;
	mcbspHadle->spcr2=mcbspConfig->spcr2;
	mcbspHadle->xcr1=mcbspConfig->xcr1;
	mcbspHadle->xcr2=mcbspConfig->xcr2;
	mcbspHadle->rcr1=mcbspConfig->rcr1;
	mcbspHadle->rcr2=mcbspConfig->rcr2;
	mcbspHadle->srgr1=mcbspConfig->srgr1;
	mcbspHadle->srgr2=mcbspConfig->srgr2;
}
unsigned int openMcbsp(unsigned short num)
{
	if(num>1)
	{
		return NULL;
	}
	else
	{
		return C55XX_MSP0_ADDR+num*0x400;
	}	
}
void startMcbspRcv(PC55XX_MCSP mcbspHadle)
{
    SetMask(mcbspHadle -> spcr1, SPCR1_RRST); 
}
void startMcbspXmit(PC55XX_MCSP mcbspHadle)
{
	SetMask(mcbspHadle -> spcr2, SPCR2_XRST);
}
void stopMcbspRcv(PC55XX_MCSP mcbspHadle)
{
    ClearMask(mcbspHadle -> spcr1, SPCR1_RRST); 
}
void stopMcbspXmit(PC55XX_MCSP mcbspHadle)
{
	ClearMask(mcbspHadle -> spcr2, SPCR2_XRST);
}
void closeMcbsp(PC55XX_MCSP mcbspHadle)
{
	mcbspHadle->spcr1=0x0000;
	mcbspHadle->spcr2=0x0000;
	mcbspHadle->xcr1=0x0000;
	mcbspHadle->xcr2=0x0000;
	mcbspHadle->rcr1=0x0000;
	mcbspHadle->rcr2=0x0000;
	mcbspHadle->srgr1=0x0000;
	mcbspHadle->srgr2=0x0000;
	mcbspHadle=NULL;
}
unsigned short readMcbsp16(PC55XX_MCSP mcbspHadle)
{
	 unsigned short data;
	 while (!ReadMask(mcbspHadle -> spcr1, SPCR1_RRDY));	// 等待数据传输完成
	 data = mcbspHadle -> ddr1;		// 读入左声道数据
	 //data |= (pMCBSP0 -> ddr2)<<8;		// 读入左声道数据
	 return data;
}
unsigned long readMcbsp32(PC55XX_MCSP mcbspHadle)
{
	 unsigned long data;
	 while (!ReadMask(mcbspHadle -> spcr1, SPCR1_RRDY));	// 等待数据传输完成
	 data = mcbspHadle -> ddr1;		// 读入左声道数据
	 data |= (mcbspHadle -> ddr2)<<16;		// 读入左声道数据
	 return data;
}
void writeMcbsp16(unsigned short data,PC55XX_MCSP mcbspHadle)
{
	 while (!ReadMask(mcbspHadle -> spcr2, SPCR2_XRDY));	// 等待数据传输完成
	 mcbspHadle -> dxr1=data;	//  将原始数据送左声道输出
     mcbspHadle -> dxr2=data;	// 将经过滤波后的数据送右声道输出
}
void interrupt mcbspXimt0()
{
/*    readp++;
    if(readp>=READSIZE)
 		readp=0;
    writeMcbsp(readbuf[readp]);*/
	mcbspXmitHook();
}
void interrupt mcbspRcv0()
{
    /*readp++;
    if(readp>=READSIZE)
 		readp=0;*/
	//readMcbsp(data);
    //writeMcbsp(data);

	mcbspRcvHook();
}
