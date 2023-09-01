/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_dma2.c---------
 * This example effects a two single-frame transfers of 128 
 * elements from DARAM to DARAM, via DMA, using DMA auto-  
 * initialization mode to effect the second transfer       
 * The macro invocation reflect the settings required in   
 * DMA control registers to make this happen.              
 */
#include <stdio.h>
   
#include <csl.h>
#include <csl_irq.h>
#include <csl_dma.h>

//---------Global constants---------
/* Constant defines transfer length */
#define N   128

//---------Global data definition---------
/* Place src and dst of DMA transfer in seperate memory section */
/* to better control placement in user specified memory range   */
#pragma DATA_SECTION(src1,"dmaMem")
Uint16 src1[N];
    
#pragma DATA_SECTION(src2,"dmaMem")
Uint16 src2[N];

#pragma DATA_SECTION(dst1, "dmaMem")
Uint16 dst1[N];

#pragma DATA_SECTION(dst2, "dmaMem")
Uint16 dst2[N];

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
    DMA_DMACCR_ENDPROG_ON,
    DMA_DMACCR_WP_DEFAULT,
    DMA_DMACCR_REPEAT_OFF,
    DMA_DMACCR_AUTOINIT_ON,
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
    N,                                     /* DMACEN   */
    1,                                     /* DMACFN   */
    0,                                     /* DMACSFI  */
    0,                                     /* DMACSEI  */
    0,                                     /* DMACDFI  */
    0                                      /* DMACDEI  */
};

//---------Function prototypes---------
/* Reference start of interrupt vector table  */
/* This symbol is defined in file vectors.s55 */
extern void VECSTART(void);

/* Define a DMA_Handle object */
interrupt void dmaIsr(void);

#pragma CODE_SECTION(taskFxn,".text1");
void taskFxn(void);

DMA_Handle myhDma;
int i, j;   
Uint16 err = 0;
Uint16 numTransfers = 0;
volatile Uint16 transferComplete = FALSE;

//---------main routine---------
void main(void)
{
    /* Initializa CSL library - This is REQUIRED !!! */
    CSL_init();

    /* Set IVPH/IVPD to start of interrupt vector table */
    IRQ_setVecs((Uint32)(&VECSTART));

    /* Initialize source and destination buffers */
    for (i = 0; i <= (N - 1); i++) {
        dst1[i] = 0;
        dst2[i] = 0;
        src1[i] = i + 1;
        src2[i] = (i + 1) * 2;
    }

    /* Call function to effect transfer */
    taskFxn();
}

void taskFxn(void)
{
    Uint16 src1AddrHi, src1AddrLo;
    Uint16 src2AddrHi, src2AddrLo;
    Uint16 dst1AddrHi, dst1AddrLo;
    Uint16 dst2AddrHi, dst2AddrLo;
    Uint32 eventId;
    Uint16 old_intm;

    /* Open DMA Channel 1 setting registers to their power on defualts */
    myhDma = DMA_open(DMA_CHA1, DMA_OPEN_RESET);    

    /* Get Interrupt Event Id associated with this DMA */
    eventId = DMA_getEventId(myhDma);

    /* By default, the TMS320C55xx compiler assigns all data symbols word */
    /* addresses. The DMA however, expects all addresses to be byte       */
    /* addresses. Therefore, we must shift the address by 2 in order to   */
    /* change the word address to a byte address for the DMA transfer.    */ 
    src1AddrHi = (Uint16)(((Uint32)(&src1)) >> 15) & 0xFFFFu;
    src1AddrLo = (Uint16)(((Uint32)(&src1)) << 1) & 0xFFFFu;
    dst1AddrHi = (Uint16)(((Uint32)(&dst1)) >> 15) & 0xFFFFu;
    dst1AddrLo = (Uint16)(((Uint32)(&dst1)) << 1) & 0xFFFFu;

    src2AddrHi = (Uint16)(((Uint32)(&src2)) >> 15) & 0xFFFFu;
    src2AddrLo = (Uint16)(((Uint32)(&src2)) << 1) & 0xFFFFu;
    dst2AddrHi = (Uint16)(((Uint32)(&dst2)) >> 15) & 0xFFFFu;
    dst2AddrLo = (Uint16)(((Uint32)(&dst2)) << 1) & 0xFFFFu;

    myconfig.dmacssal = (DMA_AdrPtr)src1AddrLo;
    myconfig.dmacssau = src1AddrHi;
    myconfig.dmacdsal = (DMA_AdrPtr)dst1AddrLo;
    myconfig.dmacdsau = dst1AddrHi;

    /* Write configuration structure values to DMA control registers */
    DMA_config(myhDma, &myconfig);    

    /* Temporarily Disable All Interrupts */
    old_intm = IRQ_globalDisable();

    /* Clear any pending interrupts for DMA in IFR */
    IRQ_clear(eventId);

    /* Enable the DMA interrupt in IER register */
    IRQ_enable(eventId);
    
    /* Place ISR address in associated vector location */
    IRQ_plug(eventId, &dmaIsr);

    /* Enable all maskable interrupts */   
    IRQ_globalEnable();

    /* Enable DMA channel to begin transfer */
    DMA_start(myhDma);


    /* Wait for programmation bit, ENDPROG == 0, to make sure that device's   */
    /* configuration register set has already been copied to working set */
    while (DMA_FGETH(myhDma,DMACCR,ENDPROG)) {
        ;   
    }

    /* Write next set of configuration values to the DMA control regs */
    /* for next transfer */

    DMA_RSETH(myhDma,DMACSSAU,src2AddrHi);
    DMA_RSETH(myhDma,DMACSSAL,src2AddrLo);
    DMA_RSETH(myhDma,DMACDSAU,dst2AddrHi);
    DMA_RSETH(myhDma,DMACDSAL,dst2AddrLo);

    DMA_RSETH(myhDma,DMACEN, N);
    DMA_RSETH(myhDma,DMACFN, 1);
 
    /* Set programmation bit to 1, ENDPROG = 1) */

    DMA_FSETH(myhDma,DMACCR,ENDPROG,1);

    /* Wait for transfers to complete */
    while (!transferComplete) {
        ;   
    }

    /* Check data values to make sure transfer happened correctly */
    for (i = 0; i <= (N - 1); i++) {
        if ((dst1[i] != src1[i]) || (dst2[i] != src2[i])) {
            ++err;
        }
    }
            
    printf ("%s\n",err?"TEST FAILED" : "TEST PASSED");

    /* Restore GLobal Interrupt Enable to Previous Setting */
    IRQ_globalRestore(old_intm);
    
    /* We are through with DMA, so close it */
    DMA_close(myhDma); 
}

/* DMA Interrupt Service Routine */
interrupt void dmaIsr(void) {
  ++numTransfers;
  DMA_FSETH(myhDma,DMACSR,FRAME,0);
  if (numTransfers == 2) {
    transferComplete = TRUE;
    DMA_stop(myhDma);
  }
}
