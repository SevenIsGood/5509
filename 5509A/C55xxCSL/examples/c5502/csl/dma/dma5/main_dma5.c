/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_dma5.c---------
 * This example demostrates the usage of different source and dstination
 * frame and element indices as well as the usage of negative index in DMA.
 * It performs matrix transpose operation for square matrix 'src1' into 
 * 'dst1'. The element size is 16 bits. Note that frame here is equivalent to
 * row of the matrix. Thus FRM_CNT is no. of rows in the matrix and ELE_CNT
 * is no. of columns in the matrix.
 */
#include <stdio.h>
   
#include <csl.h>
#include <csl_irq.h>
#include <csl_dma.h>

//---------Global constants---------

// Constants that define transfer length
// 2D matrix row and column count
#define FRM_CNT			25	//Row count
#define ELE_CNT			25	//Column count

//Source matrix traversing is contiguous
#define SRC_FRM_IDX		1		
#define SRC_ELE_IDX		1
	
//Destination matrix is transpose of source matrix

//NOTE: Multiplication by 2 is because DMA needs byte addressing
//      and element size is 16 bit

//Number of elements the address pointer must travel
//  in reverse at the end of one frame transfer
#define REVERSE_ELE_CNT	((FRM_CNT-1)*ELE_CNT - 1)
#define DST_FRM_IDX		-(REVERSE_ELE_CNT*2 + 1)
#define DST_ELE_IDX		(2*ELE_CNT - 1)

//---------Global data definition---------

#pragma DATA_ALIGN(src1, 16)
#pragma DATA_SECTION(src1,".matrix")
Uint16 src1[FRM_CNT][ELE_CNT];

#pragma DATA_ALIGN(dst1, 16)    
#pragma DATA_SECTION(dst1, ".matrix")
Uint16 dst1[FRM_CNT][ELE_CNT];

DMA_Config  myconfig = { 
  DMA_DMACSDP_RMK(
    DMA_DMACSDP_DSTBEN_NOBURST,
    DMA_DMACSDP_DSTPACK_OFF,
    DMA_DMACSDP_DST_DARAMPORT0,
    DMA_DMACSDP_SRCBEN_NOBURST,
    DMA_DMACSDP_SRCPACK_OFF,
    DMA_DMACSDP_SRC_DARAMPORT0,
    DMA_DMACSDP_DATATYPE_16BIT
  ),                                       /* DMACSDP  */
  DMA_DMACCR_RMK(
    DMA_DMACCR_DSTAMODE_DBLINDX,
    DMA_DMACCR_SRCAMODE_DBLINDX,
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
    SRC_FRM_IDX,                           /* DMACSFI  */
    SRC_ELE_IDX,                           /* DMACSEI  */
    DST_FRM_IDX,                           /* DMACDFI  */
    DST_ELE_IDX                            /* DMACDEI  */
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
    for (i = 0; i < FRM_CNT; i++)
    {
    	for (j = 0; j < ELE_CNT; j++)
    	{
    		dst1[i][j] = 0;
    		src1[i][j] = i*ELE_CNT + j;
    	}
    }

    /* Call function to effect transfer */
    taskFxn();
}

void taskFxn(void)
{
    Uint16 src1AddrHi, src1AddrLo;
    Uint16 dst1AddrHi, dst1AddrLo;

    /* Open DMA Channel 1 setting registers to their power on defualts */
    myhDma = DMA_open(DMA_CHA1, DMA_OPEN_RESET);    

    /* By default, the TMS320C55xx compiler assigns all data symbols word */
    /* addresses. The DMA however, expects all addresses to be byte       */
    /* addresses. Therefore, we must shift the address by 2 in order to   */
    /* change the word address to a byte address for the DMA transfer.    */ 
    src1AddrHi = (Uint16)(((Uint32)(&src1)) >> 15) & 0xFFFFu;
    src1AddrLo = (Uint16)(((Uint32)(&src1)) << 1) & 0xFFFFu;
    dst1AddrHi = (Uint16)(((Uint32)(&dst1)) >> 15) & 0xFFFFu;
    dst1AddrLo = (Uint16)(((Uint32)(&dst1)) << 1) & 0xFFFFu;

    myconfig.dmacssal = (DMA_AdrPtr)src1AddrLo;
    myconfig.dmacssau = src1AddrHi;
    myconfig.dmacdsal = (DMA_AdrPtr)dst1AddrLo;
    myconfig.dmacdsau = dst1AddrHi;

    /* Write configuration structure values to DMA control registers */
    DMA_config(myhDma, &myconfig);    

    /* Enable DMA channel to begin transfer */
    DMA_start(myhDma);


    while (!DMA_FGETH(myhDma,DMACSR,BLOCK))
    {
        ;   
    }

    /* Check data values to make sure transfer happened correctly */
    for (i = 0; i < FRM_CNT; i++)
    {
    	for (j = 0; j < ELE_CNT; j++)
    	{
    		if (dst1[i][j] != src1[j][i])
    			++err;    		
    	}
    }
            
    printf ("%s\n",err?"TEST FAILED" : "TEST PASSED");
    
    /* We are through with DMA, so close it */
    DMA_close(myhDma); 
}
