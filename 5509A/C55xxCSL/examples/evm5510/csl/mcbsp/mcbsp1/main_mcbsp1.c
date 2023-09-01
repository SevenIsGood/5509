/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_mcbsp1.c---------
 * In this example, the MCBSP is configured in digital loopback
 * mode, with 32 bit data transfer, in multi-frame mode, using                                                   
 * sample rate generator to sync frames                        
 */
#include <stdio.h>

#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>

//---------Global constants---------
#define N 20                 

//---------Global data definition---------

/* Create a MCBSP configuration structure */
  MCBSP_Config ConfigLoopBack32= {
  MCBSP_SPCR1_RMK(
    MCBSP_SPCR1_DLB_ON,                    /* DLB    = 1 */
    MCBSP_SPCR1_RJUST_RZF,                 /* RJUST  = 0 */
    MCBSP_SPCR1_CLKSTP_DISABLE,            /* CLKSTP = 0 */
    MCBSP_SPCR1_DXENA_NA,                  /* DXENA  = 0 */
    MCBSP_SPCR1_ABIS_DISABLE,              /* ABIS   = 0 */
    MCBSP_SPCR1_RINTM_RRDY,                /* RINTM  = 0 */
    0,                                     /* RSYNCER = 0 */
    MCBSP_SPCR1_RRST_DISABLE               /* RRST   = 0 */
   ),
    MCBSP_SPCR2_RMK(
    MCBSP_SPCR2_FREE_NO,                   /* FREE   = 0 */
    MCBSP_SPCR2_SOFT_NO,                   /* SOFT   = 0 */
    MCBSP_SPCR2_FRST_FSG,                  /* FRST   = 0 */
    MCBSP_SPCR2_GRST_CLKG,                 /* GRST   = 0 */
    MCBSP_SPCR2_XINTM_XRDY,                /* XINTM  = 0 */
    0,                                     /* XSYNCER = N/A */
    MCBSP_SPCR2_XRST_DISABLE               /* XRST   = 0 */
   ),
  MCBSP_RCR1_RMK( 
  MCBSP_RCR1_RFRLEN1_OF(0),                /* RFRLEN1 = 0 */
  MCBSP_RCR1_RWDLEN1_32BIT                 /* RWDLEN1 = 5 */
  ),
 MCBSP_RCR2_RMK(    
    MCBSP_RCR2_RPHASE_SINGLE,              /* RPHASE  = 0 */
    MCBSP_RCR2_RFRLEN2_OF(0),              /* RFRLEN2 = 0 */
    MCBSP_RCR2_RWDLEN2_8BIT,               /* RWDLEN2 = 0 */
    MCBSP_RCR2_RCOMPAND_MSB,               /* RCOMPAND = 0 */
    MCBSP_RCR2_RFIG_YES,                   /* RFIG    = 0 */
    MCBSP_RCR2_RDATDLY_2BIT                /* RDATDLY = 0 */
    ),  
   MCBSP_XCR1_RMK(    
    MCBSP_XCR1_XFRLEN1_OF(0),              /* XFRLEN1 = 0 */ 
    MCBSP_XCR1_XWDLEN1_32BIT               /* XWDLEN1 = 5 */
    
 ),   
 MCBSP_XCR2_RMK(   
    MCBSP_XCR2_XPHASE_SINGLE,              /* XPHASE  = 0 */
    MCBSP_XCR2_XFRLEN2_OF(0),              /* XFRLEN2 = 0 */
    MCBSP_XCR2_XWDLEN2_8BIT,               /* XWDLEN2 = 0 */
    MCBSP_XCR2_XCOMPAND_MSB,               /* XCOMPAND = 0 */
    MCBSP_XCR2_XFIG_YES,                   /* XFIG    = 0 */
    MCBSP_XCR2_XDATDLY_2BIT                /* XDATDLY = 0 */
  ),            
 MCBSP_SRGR1_RMK( 
   MCBSP_SRGR1_FWID_OF(1),                /* FWID    = 1 */
   MCBSP_SRGR1_CLKGDV_OF(1)               /* CLKGDV  = 1 */
 ),   
 MCBSP_SRGR2_RMK(  
    MCBSP_SRGR2_GSYNC_FREE,                /* FREE    = 0 */
    MCBSP_SRGR2_CLKSP_RISING,              /* CLKSP   = 0 */
    MCBSP_SRGR2_CLKSM_INTERNAL,            /* CLKSM   = 1 */
    MCBSP_SRGR2_FSGM_DXR2XSR,              /* FSGM    = 0 */
    MCBSP_SRGR2_FPER_OF(15)                /* FPER    = 0 */
 ),  
 MCBSP_MCR1_DEFAULT,
 MCBSP_MCR2_DEFAULT, 
 MCBSP_PCR_RMK(
   MCBSP_PCR_IDLEEN_RESET,                 /* IDLEEN   = 0   */
   MCBSP_PCR_XIOEN_SP,                     /* XIOEN    = 0   */
   MCBSP_PCR_RIOEN_SP,                     /* RIOEN    = 0   */
   MCBSP_PCR_FSXM_INTERNAL,                /* FSXM     = 1   */
   MCBSP_PCR_FSRM_EXTERNAL,                /* FSRM     = 0   */
   MCBSP_PCR_CLKXM_OUTPUT,                 /* CLKXM    = 1   */
   MCBSP_PCR_CLKRM_INPUT,                  /* CLKRM    = 0   */
   MCBSP_PCR_SCLKME_NO,                    /* SCLKME   = 0   */
   0,                                      /* DXSTAT = N/A   */
   MCBSP_PCR_FSXP_ACTIVEHIGH,              /* FSXP     = 0   */
   MCBSP_PCR_FSRP_ACTIVEHIGH,              /* FSRP     = 0   */
   MCBSP_PCR_CLKXP_RISING,                 /* CLKXP    = 0   */
   MCBSP_PCR_CLKRP_FALLING                 /* CLKRP    = 0   */
 ),
 MCBSP_RCERA_DEFAULT, 
 MCBSP_RCERB_DEFAULT, 
 MCBSP_RCERC_DEFAULT, 
 MCBSP_RCERD_DEFAULT, 
 MCBSP_RCERE_DEFAULT, 
 MCBSP_RCERF_DEFAULT, 
 MCBSP_RCERG_DEFAULT, 
 MCBSP_RCERH_DEFAULT, 
 MCBSP_XCERA_DEFAULT,
 MCBSP_XCERB_DEFAULT,
 MCBSP_XCERC_DEFAULT,
 MCBSP_XCERD_DEFAULT,  
 MCBSP_XCERE_DEFAULT,
 MCBSP_XCERF_DEFAULT,  
 MCBSP_XCERG_DEFAULT,
 MCBSP_XCERH_DEFAULT
};     

/* Create data buffers for transfer */
Uint32 xmt[N], rcv[N]; 

//---------Function prototypes---------
void taskFunc(void);

//---------main routine---------
void main() {
  Uint16 i;
  
  /* Initialize CSL library, this step is required */
  CSL_init();

  /* Initilize data buffers. xmt will be 32 bit value      */
  /* with element number in lower 16 bits and 2 * element  */
  /* in upper 16 bits.  i.e.  0x00020001, 0x00040002, etc. */
  for(i=0;i<=N-1;i++) {
    xmt[i] = (((Uint32)(i+1)<<17) + (i+1));
    rcv[i] = 0;
  }

  /* Call example function to perform task */
  taskFunc();
}

void taskFunc(void) {

  MCBSP_Handle mhMcbsp;
  Uint16 i;
  Uint16 err = 0;
  
  /* Open MCBSP Port 0, this will return a MCBSP handle that will */
  /* be used in calls to other CSl functions.                     */ 
  mhMcbsp = MCBSP_open(MCBSP_PORT0, MCBSP_OPEN_RESET);

  /* Write configuration structure values to MCBSP control */
  /* registers                                             */
  MCBSP_config(mhMcbsp, &ConfigLoopBack32); 

  /* Start Sample Rate Generator and Frame Sync */
  MCBSP_start(mhMcbsp, 
    MCBSP_SRGR_START | MCBSP_SRGR_FRAMESYNC, 
    0x300
  );
 
  /* Enable MCBSP transmit and receive */
  MCBSP_start(mhMcbsp, 
    MCBSP_RCV_START | MCBSP_XMIT_START, 
    0
  );
    
  /* Begin data transfer loop. We will loop thru to transmit */
  /* and receive the data.                                   */  
  for (i=0; i<=N-1;i++) {
 
    /* Wait for XRDY signal before writing data to DXR */
    while (!MCBSP_xrdy(mhMcbsp));       
                 
    /* Write 32 bit data value to DXR */    
    MCBSP_write32(mhMcbsp,xmt[i]);       
 
    /* Wait for RRDY signal to read data from DRR */
    while (!MCBSP_rrdy(mhMcbsp));

    /* Read 32 bit value from DRR */
    rcv[i] = MCBSP_read32(mhMcbsp);               
  }    

  /* Check data to make sure transfer was successful */
  for(i=0; i<= N-1;i++) {
    if (xmt[i] != rcv[i]) {
       ++err;
       break;
    }
  }
 
  /* We are done with MCBSP, so close it */ 
  MCBSP_close(mhMcbsp);
   
  printf ("%s\n",err?"TEST FAILED" : "TEST PASSED");
}
