#include <stdio.h>
#include "5509.h"
#include "dma.h"
unsigned int openDma(unsigned short num)
{
	if(num>5)
	{
		return NULL;
	}
	else
	{
		return C55XX_DMA0_ADDR+num*0x20;
	}	
}
void configDma(PC55XX_DMA dmaHandle,C55XX_DMA* dmaConfig)
{
	dmaHandle->csdp=dmaConfig->csdp;
	dmaHandle->ccr=dmaConfig->ccr;
	dmaHandle->cicr=dmaConfig->cicr;
	dmaHandle->cssa_l=dmaConfig->cssa_l;
	dmaHandle->cssa_u=dmaConfig->cssa_u;
	dmaHandle->cdsa_l=dmaConfig->cdsa_l;
	dmaHandle->cdsa_u=dmaConfig->cdsa_u;
	dmaHandle->cen=dmaConfig->cen;
	dmaHandle->cfn=dmaConfig->cfn;
	dmaHandle->cfi=dmaConfig->cfi;
}
void getconfigDma(PC55XX_DMA dmaHandle,C55XX_DMA* dmaConfig)
{
	dmaConfig->csdp=dmaHandle->csdp;
	dmaConfig->ccr=dmaHandle->ccr;
	dmaConfig->cicr=dmaHandle->cicr;
	dmaConfig->csr=dmaHandle->csr;
	dmaConfig->cssa_l=dmaHandle->cssa_l;
	dmaConfig->cssa_u=dmaHandle->cssa_u;
	dmaConfig->cdsa_l=dmaHandle->cdsa_l;
	dmaConfig->cdsa_u=dmaHandle->cdsa_u;
	dmaConfig->cen=dmaHandle->cen;
	dmaConfig->cfn=dmaHandle->cfn;
	dmaConfig->cfi=dmaHandle->cfi;
}
void startDma(PC55XX_DMA dmaHandle)
{
	dmaHandle->ccr|=0x0800;
	dmaHandle->ccr|=0x0080;
}
void stopDma(PC55XX_DMA dmaHandle)
{
	dmaHandle->ccr&=~0x0080;
}
void closeDma(PC55XX_DMA dmaHandle)
{
	stopDma(dmaHandle);
	dmaHandle->csdp		=0x0000;
	dmaHandle->ccr		=0x0000;
	dmaHandle->cicr		=0x0000;
	dmaHandle->cssa_l	=0x0000;
	dmaHandle->cssa_u	=0x0000;
	dmaHandle->cdsa_l	=0x0000;
	dmaHandle->cdsa_u	=0x0000;
	dmaHandle->cen		=0x0000;
	dmaHandle->cfn		=0x0000;
	dmaHandle->cfi		=0x0000;
	dmaHandle=NULL;
}
interrupt void isrDma0(void)
{
	isrDma0Hook();
}
