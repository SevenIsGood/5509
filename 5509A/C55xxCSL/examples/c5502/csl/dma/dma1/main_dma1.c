/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_dma1.c---------
 * This example demostrates simple DMA transfer of single frame of 
 * 128 elements, 16 bit each.
 */
#include <stdio.h>
#include <stdlib.h>

#include <csl.h>
#include <csl_irq.h>
#include <csl_dma.h>

//---------Global constants---------
//* Constant defines transfer length */
#define N       128

//---------Global data definition---------
/* Place src and dst of DMA transfer in seperate memory section */
/* to better control placement in user specified memory range   */
#pragma DATA_SECTION(src,"dmaMem")
Uint16 src[N];
    
#pragma DATA_SECTION(dst, "dmaMem")
Uint16 dst[N];

DMA_Config  myconfig = { 
  DMA_DMACSDP_RMK(
    DMA_DMACSDP_DSTBEN_NOBURST,
    DMA_DMACSDP_DSTPACK_OFF,
    DMA_DMACSDP_DST_DARAMPORT1,
    DMA_DMACSDP_SRCBEN_NOBURST,
    DMA_DMACSDP_SRCPACK_OFF,
    DMA_DMACSDP_SRC_DARAMPORT1,
    DMA_DMACSDP_DATATYPE_16BIT
  ),                                       /* DMACSDP  */
  DMA_DMACCR_RMK(
    DMA_DMACCR_DSTAMODE_POSTINC,
    DMA_DMACCR_SRCAMODE_POSTINC,
    DMA_DMACCR_ENDPROG_OFF,
    DMA_DMACCR_WP_DEFAULT,
    DMA_DMACCR_REPEAT_OFF,
    DMA_DMACCR_AUTOINIT_OFF,
    DMA_DMACCR_EN_STOP,
    DMA_DMACCR_PRIO_HI,
    DMA_DMACCR_FS_ENABLE,
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
    (DMA_AdrPtr) &src,                     /* DMACSSAL */
    0,                                     /* DMACSSAU */
    (DMA_AdrPtr)&dst,                      /* DMACDSAL */
    0,                                     /* DMACDSAU */
    N,                                     /* DMACEN   */
    1,                                     /* DMACFN   */
    0,                                     /* DMACSFI  */
    0,                                     /* DMACSEI  */
    0,                                     /* DMACDFI  */
    0                                      /* DMACDEI  */
};

/* Define a DMA_Handle object */
DMA_Handle myhDma;

int i, j;   
Uint16 err = 0;
volatile Uint16 WaitForTransfer = TRUE;

//---------Function prototypes---------
void taskFxn(void);

//---------main routine---------
void main(void)
{
    /* Initialize CSL library */
    CSL_init();
    
    /* Initialise randome generator with a random seed from dst[0]*/
    srand((unsigned int)dst[0]);
    
    /* Initialize source and destination buffers */
    for (i = 0; i <= (N - 1); i++) {
        dst[i] = 0;
        src[i] = (rand() & 0xffff) + i + 1;
    }

    /* Call Function For DMA Transfer */
    taskFxn();
}

void taskFxn(void)
{
	/* Open DMA Channel 0 */
    myhDma = DMA_open(DMA_CHA0, 0);    

    /* By default, the TMS320C55xx compiler assigns all data symbols word */
    /* addresses. The DMA however, expects all addresses to be byte       */
    /* addresses. Therefore, we must shift the address by 2 in order to   */
    /* change the word address to a byte address for the DMA transfer.     */
    
    /* Configure lower address registers */   
    myconfig.dmacssal = (DMA_AdrPtr)(((Uint32)(myconfig.dmacssal)<<1)&0xFFFF);
    myconfig.dmacdsal = (DMA_AdrPtr)(((Uint32)(myconfig.dmacdsal)<<1)&0xFFFF);
    
    /* Configure upper address registers */       
	myconfig.dmacssau = (((Uint32) &src) >> 15) & 0xFFFF;
	myconfig.dmacdsau = (((Uint32) &dst) >> 15) & 0xFFFF;

    /* Write configuration structure values to DMA control registers */
    DMA_config(myhDma, &myconfig);                   

    /* Enable DMA channel to begin transfer */
    DMA_start(myhDma);

    /* Wait for FRAME status bit in DMA status register to signal */
    /* transfer is complete.                                      */
    while (!DMA_FGETH(myhDma,DMACSR,FRAME)) {
        ;   
    }

    /* Check data values to make sure transfer happened correctly */
    for (i = 0; i <= (N - 1); i++) {
        if (dst[i] != src[i]) {
            ++err;
            break;
        }
    }
            
    printf("%s", err?"TEST FAILED":"TEST PASSED");
 
    /* We are through with DMA, so close it */
    DMA_close(myhDma); 
}
