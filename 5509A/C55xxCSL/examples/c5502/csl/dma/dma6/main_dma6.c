/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_dma6.c---------
 * This example demostrates the usage of single indexing mode to implement
 * byte-swapping in a buffer. DMA transfer element size is 8 bits.
 * Destination buffer has byte-swapped as well as reverse order elements
 * of the source buffer.
 * e.g. src: [0] ->0x0123
 *			 [1] ->0x4567
 *			 [2] ->0x89AB
 *			 [3] ->0xCDEF
 *
 *		dst: [0] ->0xEFCD
 *		     [1] ->0xAB89
 *    		 [2] ->0x6745
 *     		 [3] ->0x2301
 */
#include <stdio.h>
   
#include <csl.h>
#include <csl_irq.h>
#include <csl_dma.h>

//---------Global constants---------

//Using single index feature - only 1 frame
#define FRM_CNT			1

//A frame consists of ELE_CNT no of elements (in bytes)
#define ELE_CNT			100

//Buffer size in 16-bit words
#define N				(FRM_CNT * ELE_CNT / 2)

//Source element index to use for byte-swapping
#define SRC_ELE_IDX		-1

//PAD for dst buffer when ELE_CNT is odd
#define PAD				(ELE_CNT % 2)

//---------Global data definition---------

#pragma DATA_ALIGN(src1, 16)
#pragma DATA_SECTION(src1,".buffer")
Uint16 src1[N];

#pragma DATA_ALIGN(dst1, 16)    
#pragma DATA_SECTION(dst1, ".buffer")
Uint16 dst1[N + PAD];

DMA_Config  myconfig = { 
  DMA_DMACSDP_RMK(
    DMA_DMACSDP_DSTBEN_NOBURST,
    DMA_DMACSDP_DSTPACK_OFF,
    DMA_DMACSDP_DST_DARAMPORT0,
    DMA_DMACSDP_SRCBEN_NOBURST,
    DMA_DMACSDP_SRCPACK_OFF,
    DMA_DMACSDP_SRC_DARAMPORT0,
    DMA_DMACSDP_DATATYPE_8BIT
  ),                                       /* DMACSDP  */
  DMA_DMACCR_RMK(
    DMA_DMACCR_DSTAMODE_POSTINC,
    DMA_DMACCR_SRCAMODE_SGLINDX,
    DMA_DMACCR_ENDPROG_DEFAULT,
    DMA_DMACCR_WP_DEFAULT,
    DMA_DMACCR_REPEAT_DEFAULT,
    DMA_DMACCR_AUTOINIT_OFF,
    DMA_DMACCR_EN_STOP,
    DMA_DMACCR_PRIO_HI,
    DMA_DMACCR_FS_DISABLE,
    DMA_DMACCR_SYNC_NONE
  ),                                       /* DMACCR   */
  DMA_DMACICR_RMK(
    DMA_DMACICR_AERRIE_ON,
    DMA_DMACICR_BLOCKIE_OFF,
    DMA_DMACICR_LASTIE_OFF,
    DMA_DMACICR_FRAMEIE_ON,
    DMA_DMACICR_FIRSTHALFIE_OFF,
    DMA_DMACICR_DROPIE_OFF,
    DMA_DMACICR_TIMEOUTIE_OFF
  ),                                       /* DMACICR  */
    (DMA_AdrPtr)&src1,                     /* DMACSSAL */
    0,                                     /* DMACSSAU */
    (DMA_AdrPtr)&dst1,                     /* DMACDSAL */
    0,                                     /* DMACDSAU */
    ELE_CNT,                               /* DMACEN   */
    FRM_CNT,                               /* DMACFN   */
    0,                           		   /* DMACSFI  */
    SRC_ELE_IDX,                           /* DMACSEI  */
    0,                           		   /* DMACDFI  */
    0                            		   /* DMACDEI  */
};

DMA_Handle myhDma;
int i, j;   
Uint16 err = 0;
Uint16 numTransfers = 0;

//---------Function prototypes---------
void taskFxn(void);

//---------main routine---------
void main(void)
{
    /* Initialize CSL library */
    CSL_init();

    /* Initialize source and destination buffers */
    for (i = 1; i <= N; i++)
    {
    	dst1[i-1] = 0;
    	src1[i-1] = ((2 * i) << 8) | (2 * i - 1);
    }
	dst1[N + PAD - 1] = 0xDEAD;
	
    /* Call function to effect transfer */
    taskFxn();
}

void taskFxn(void)
{
    Uint16 src1AddrHi, src1AddrLo;
    Uint16 dst1AddrHi, dst1AddrLo;
	Uint16 swapDst;
	
    /* Open DMA Channel 1 setting registers to their power on defualts */
    myhDma = DMA_open(DMA_CHA1, DMA_OPEN_RESET);    

    /* By default, the TMS320C55xx compiler assigns all data symbols word */
    /* addresses. The DMA however, expects all addresses to be byte       */
    /* addresses. Therefore, we must shift the address by 2 in order to   */
    /* change the word address to a byte address for the DMA transfer.    */ 
    src1AddrHi = (Uint16)(((Uint32)(&src1[N])) >> 15) & 0xFFFFu;
    src1AddrLo = ((Uint16)(((Uint32)(&src1[N])) << 1) & 0xFFFFu) - 0x1;
    dst1AddrHi = (Uint16)(((Uint32)(&dst1)) >> 15) & 0xFFFFu;
    dst1AddrLo = ((Uint16)(((Uint32)(&dst1)) << 1) & 0xFFFFu);

    myconfig.dmacssal = (DMA_AdrPtr)src1AddrLo;
    myconfig.dmacssau = src1AddrHi;
    myconfig.dmacdsal = (DMA_AdrPtr)dst1AddrLo;
    myconfig.dmacdsau = dst1AddrHi;

    /* Write configuration structure values to DMA control registers */
    DMA_config(myhDma, &myconfig);    

    /* Enable DMA channel to begin transfer */
    DMA_start(myhDma);

	/* Wait while complete frame is being transferred */
    while (!DMA_FGETH(myhDma,DMACSR,FRAME)) ;

    /* Check data values to make sure byte-swapping happened correctly */
    for (i = 0; i < N; i++)
    {
    	swapDst = (dst1[i] << 8) & 0xFF00 | 
    				((dst1[i] >> 8) & 0x00FF);
    	if (src1[N-i-1] != swapDst)
			++err;    		
    }

    printf ("%s\n",err?"TEST FAILED" : "TEST PASSED");
    
    /* We are through with DMA, so close it */
    DMA_close(myhDma); 
}
