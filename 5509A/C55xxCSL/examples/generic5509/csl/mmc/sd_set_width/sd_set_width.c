/*
 * Copyright (C) 2004 Texas Instruments Incorporated
 * All Rights Reserved
 *
 *
 *---------sd_set_width.c---------
 * This example demonstrates how the bus width for a transfer to an SD card 
 * can be changed. 
 * This example is written for c5509. To run on c5509a, link in the
 * c5509a CSL library and define CHIP_5509A in the build options
 */

#include <csl_mmc.h>
#include <stdio.h>

#pragma DATA_SECTION(mmc0,"cslmem")
MMC_Handle mmc0;
MMC_CardObj *card, cardalloc;
MMC_CardIdObj *cid, cardid;

int count, retVal;
Uint16 rca;
Uint16 cardtype;

/* Because of different operating clock frequencies, the Init structure for c5509    *
 * and c5509a have different values for memory and function clock divide down values */
#if CHIP_5509A
  MMC_SetupNative Init = {
    0,   /* Disable DMA for data read/write                 */
    0,   /* Set level of edge detection for DAT3 pin        */
    0,   /* Determines if MMC goes IDLE during IDLE instr   */
    1,   /* Memory clk reflected on CLK Pin                 */
    7,   /* CPU CLK to MMC function clk divide down         */
    5,   /* MMC function clk to memory clk divide down      */
    0,   /* No. memory clks to wait before response timeout */
    0,   /* No. memory clks to wait before data timeout     */
    512, /* Block Length must be same as CSD                */
  };
#else
  MMC_SetupNative Init = {
    0,   /* Disable DMA for data read/write                 */
    0,   /* Set level of edge detection for DAT3 pin        */
    0,   /* Determines if MMC goes IDLE during IDLE instr   */
    1,   /* Memory clk reflected on CLK Pin                 */
    3,   /* CPU CLK to MMC function clk divide down         */
    3,   /* MMC function clk to memory clk divide down      */
    0,   /* No. memory clks to wait before response timeout */
    0,   /* No. memory clks to wait before data timeout     */
    512, /* Block Length must be same as CSD                */
  };
#endif

Uint16 datasend[512];
Uint16 datareceive[512];

void main()
{

  CSL_init();

//  IRQ_globalDisable();
                                
//  IRQ_setVecs(0x10000);
  printf ("Test data transfer to SD card with 4-bit bus width\n");
  
  /* Initialize the source and destination buffers */
  for (count=0;count<=256;count++){
      datasend[count]    = count;
      datareceive[count] = 0xFFFF;
  }

  /* A detailed explanation of MMC initialization is provided in mmc_setup example */
  mmc0 = MMC_open(MMC_DEV1);
  MMC_setupNative(mmc0,&Init);
  MMC_sendGoIdle(mmc0);
  
  for (count=0;count<4016;count++)
  	asm("	NOP");
  
  cardtype = MMC_sendOpCond(mmc0,0x00100000); 
  if (cardtype == 0xFFFF){
     printf ("Card not recognized. Insert SD card\n");
     exit(0);
  }
  
  if (cardtype == MMC_CARD){
     /* Bus width is configurable only for SD cards */
     printf ("The bus width cannot be changed for an MMC card. Insert an SD card and restart\n");
     exit(0);
  } else {
     cid = &cardid;
     SD_sendAllCID(mmc0,cid); // get the CID structure for all cards.     
     card = &cardalloc;
     rca = SD_sendRca(mmc0,card);
     printf ("RCA sent by the SD card is 0x%x\n", rca);

  }
  
  IRQ_globalEnable();  

  /* Select the card using it's RCA */
  retVal = MMC_selectCard(mmc0,card);

  /* NOTE: The bus width for SD data transfer can be changed only   *
   * after the card is selected using it's RCA. This is so because  *
   * this command is valid only in the transfer state               */
  
  /* Default bus width is 1 bit wide. Change bus width to 4 bits    *
   * To change bus width back to 1 bit, use SD_setWidth(mmc0, 0x1); */

  printf ("Setting the SD bus width to 4 bits...\n");  
  retVal = SD_setWidth(mmc0, 0x4);
  
  printf ("Writing data to the SD card...\n"); 
  /* Start data transfer to card */
  retVal = MMC_write(mmc0,0x00000000,datasend,512);
                           
  for (count = 0; count <= 25000; ++count)
      asm("	NOP");
  for (count = 0; count <= 25000; ++count)
      asm("	NOP");
  for (count = 0; count <= 25000; ++count)
      asm("	NOP");
  for (count = 0; count <= 25000; ++count)
      asm("	NOP");
  /* Read data written earlier to the card */
  printf ("Reading data from the SD card...\n");
  retVal = MMC_read(mmc0,0x00000000,datareceive,512);
  
  for (count = 0; count < 256; ++count)
     if (datasend[count] != datareceive[count])
        break;
  
  if (count < 256){
     printf ("\nNot all data was transferred/read successfully!\n");
     printf ("TEST FAILED\n");
  } else {
     printf ("\nData transfer successful\n");
     printf ("TEST PASSED\n");
  }
  /* Done */
}

