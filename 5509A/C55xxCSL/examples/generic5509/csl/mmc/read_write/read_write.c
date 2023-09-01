/*
 * Copyright (C) 2004 Texas Instruments Incorporated
 * All Rights Reserved
 *
 *
 *---------read_write.c---------
 * This example does a write to the MMC/SD card and reads the
 * data written to verify the transfer.
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

/* Read and write buffers */
Uint16 datasend[512];
Uint16 datareceive[512];
  
void main()
{
  CSL_init();
  
  printf ("\nMMC Single block read-write test\n");
  
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
  
  if (cardtype == MMC_CARD){
     cid = &cardid;
     MMC_sendAllCID(mmc0,cid); // get the CID structure for all cards. 
     rca = 10;
     card = &cardalloc;
     retVal = MMC_setRca(mmc0,card,rca);
  } else {
     cid = &cardid;
     SD_sendAllCID(mmc0,cid); // get the CID structure for all cards.     
     card = &cardalloc;
     rca = SD_sendRca(mmc0,card);
     printf ("RCA sent by the SD card is 0x%x\n", rca);
  }
  
  /* Select the card to transfer data to/from. This step puts the card into the *
   * transfer state where it is ready to read/write data                        */
  
  retVal = MMC_selectCard(mmc0,card);

  /* MMC_write(Handle, start address, buffer pointer, length in bytes);         *
   * This does a single block transfer to the MMC card. Max length in bytes is  *
   * 512 bytes. datasend buffer has 256 elements of 16 bits each, which total   *
   * upto 512 bytes. By default data bus width is 1 bit. An SD card can support *
   * data bus width of 4 bits. The sd_set_width example shows how this is done  */
  
  printf ("Transferring data to card...\n");
  retVal = MMC_write(mmc0,0x00000000,datasend,512);
  
  for (count = 0; count <= 25000; ++count)
      asm("		NOP");
  
  for (count = 0; count <= 25000; ++count)
      asm("		NOP");
  
  for (count = 0; count <= 25000; ++count)
      asm("		NOP");

  for (count = 0; count <= 25000; ++count)
      asm("		NOP");
            
  /* MMC_read(Handle, start address, buffer pointer, length in bytes);          *
   * This does a single block read from the MMC card. Max length in bytes for a *
   * block is 512 bytes. datareceive buffer has 256 elements of 16 bits each,   *
   * which total upto 512 bytes. By default data bus width is 1 bit. An SD card *
   * can support a data bus width of 4 bits. The sd_set_width eg. explains this */
  
  printf ("Reading data from card...\n");
  retVal = MMC_read(mmc0,0x00000000,datareceive,512);
  
  /* Verify that data read corresponds to data transferred                      */
  
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

}
