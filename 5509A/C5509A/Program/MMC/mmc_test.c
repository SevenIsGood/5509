/*
 *  Copyright 2004 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 */
/*
 *---------mmc_sd_setup.c---------
 * This example initializes the MMC controller and reads data from the CSD
 * depending on whether an MMC or SD card is chosen 
 * This example is written for c5509. To run on c5509a, link in the
 * c5509a CSL library and define CHIP_5509A in the build options
 */

/* What we do in this example.
 *
 * The MMC Controller supports both an MMC and an SD card. During the initialization, 
 * we first check if the card is an SD card and initialize accordingly. Then onwards,
 * a sequence of instructions is to be passed to traverse it's state machine and bring
 * it to a data transfer state. The MMC and SD card initialization sequence varies when
 * it comes to the Relative Card Address, which is used by the host to communicate with
 * a particular card. The MMC card has to be sent an RCA from the host, while the SD card
 * publishes it's own RCA and sends it to the host. 
 *
 * NOTE: The EVM5509 MMC Controller can be operated only in the Native mode. Support for
 * SPI mode is now obsolete and the MMC CSL source has been modified accordingly.
 *
 * Data Structures used-
 *   - MMC_Handle : 
 *                  Structure that contains information about the device chosen. 
 *                  Important fields are eventId (CPU interrupt line for MMC device),
 *                  MMC ISR Dispatcher address to be plugged as ISR for the MMC,
 *                  pointer to MMC registers and a pointer to the call back table 
 *                  holding functions, one of which is called when the ISR is hit 
 *                  (the interrupt example offers more explanation on MMC interrupts)
 *
 *   - MMC_CardObj : 
 *                  Structure that holds information about the card. Important fields
 *                  are rca (Relative Card Address), manufacturer's card ID and pointers
 *                  to an MMC CSD structure and an SD CSD structure, one of which will
 *                  be used based on which card is being accessed.
 *
 *   - MMC_CardIdObj :
 *                  Structure that holds the manufacturer's Card Id present on the card.
 *                  This information is read-only.
 *
 *   _ MMC_CardCsdObj :
 *                  Structure that holds the card specific data provided on an MMC Card.
 *                  This information is read-only.
 *
 *   - SD_CardCsdObj :
 *                  Structure that holds the card specific data provided on an SD Card.
 *                  This information is read-only.
 *                  
 */

#include <csl_pll.h>
#include <csl_mmc.h>
#include <stdio.h>

//#pragma DATA_SECTION(mmc0,"cslmem")
MMC_Handle mmc0;
MMC_CardObj *card, cardalloc;
MMC_CardIdObj *cid, cardid;
MMC_CardCsdObj *csd, cardcsd;
SD_CardCsdObj *sdcsd, sdcardcsd;
MMC_Config test;

int retVal,count;
int mmcstate;
Uint16 mmcargh;
Uint16 mmcargl;
Uint16 currentwlen;
Uint16 currentrlen;
Uint16 rca;
Uint16 cardtype;
Uint16 taac, naac, tranSpeed, readBlLen, readBlPartial;
Uint16 writeBlkMisalign, readBlkMisalign, writeBlLen, writeBlPartial;
Uint16 myrecbuf[512];
Uint16 mysendbuf[512];

#define MMC_dxr (*((volatile  unsigned  int *)0x4c0c))
#define MMC_drr (*((volatile  unsigned  int *)0x4c0b))

PLL_Config  myConfig      = {
  0,    //IAI: the PLL locks using the same process that was underway 
                //before the idle mode was entered
  1,    //IOB: If the PLL indicates a break in the phase lock, 
                //it switches to its bypass mode and restarts the PLL phase-locking 
                //sequence
  12,    //PLL multiply value; multiply 24 times
  1             //Divide by 2 PLL divide value; it can be either PLL divide value 
                //(when PLL is enabled), or Bypass-mode divide value
                //(PLL in bypass mode, if PLL multiply value is set to 1)
};


/* Because of different operating clock frequencies, the Init structure for c5509    *
 * and c5509a have different values for memory and function clock divide down values */
#if CHIP_5509A
  MMC_SetupNative Init = {
    0,   /* disable DMA for data read/write                    */
    0,   /* Set level of edge detection for DAT3 pin           */
    0,   /* Determines if MMC goes IDLE during IDLE instr      */
    1,   /* Memory clk reflected on CLK Pin                    */
    7,   /* CPU CLK to MMC function clk divide down            */
    5,   /* MMC function clk to memory clk divide down         */
    0,   /* No. memory clks to wait before response timeout    */
    0,   /* No. memory clks to wait before data timeout        */
    128, /* Block Length must be same as CSD                   */
  };
#else
  MMC_SetupNative Init = {
    0,   /* disable DMA for data read/write                    */
    0,   /* Set level of edge detection for DAT3 pin           */
    0,   /* Determines if MMC goes IDLE during IDLE instr      */
    1,   /* Memory clk reflected on CLK Pin                    */
    3,   /* CPU CLK to MMC function clk divide down            */
    3,   /* MMC function clk to memory clk divide down         */
    0,   /* No. memory clks to wait before response timeout    */
    0,   /* No. memory clks to wait before data timeout        */
    128, /* Block Length must be same as CSD                   */
  };
#endif

void main()
{
  CSL_init();
  //PLL_config(&myConfig);

  for (count=0;count<=256;count++){
      mysendbuf[count]    = count;
      myrecbuf[count] = 0xFFFF;
  }

  printf ("MMC Controller setup test...\n");
  /* The MMC shares a event line with McBSP. In this function, we  *
   *  - reserve the system event line for MMC                      *
   *  - initialize the handle with data about register locations,  *
   *    event id and other device details                          *
   *  - initialize the dispatcher for the device to point to the   *
   *    dispatcher for the MMC device instance                     */

  /* Prototype: MMC_open(<device instance>);                       *
   * We choose device 1 on the board                               */
  mmc0 = MMC_open(MMC_DEV1);

  /* In this function, we initialize the MMC registers with the    *
   * parameters passed in the Init structure                       *
   * Prototype: MMC_setupNative(Handle, MMC_setupNative*);         */
  MMC_setupNative(mmc0,&Init); //initialize the MMC controller

  /* Ask all cards to go to idle state. This is equivalent to a    *
   * s/w reset.                                                    *
   * Prototype: MMC_sendGoIdle(Handle);                            */

  MMC_getConfig(mmc0,&test);

  MMC_sendGoIdle(mmc0);
  for (count=0;count<4016;count++)
  	asm("	NOP");
  
  /* Set the voltage window for the card. If the card does not      *
   * support this voltage range, it will return -1.                 *
   *                                                                *
   * Working:                                                       *
   * We first send an APP_CMD. An SD card responds to this command  *
   * while an MMC does not. If the card responds, we now send the   *
   * ACMD41 to initialize the SD card with the desiered voltage     *
   * range. If there is no response, we send the CMD1 to initialize *
   * the MMC card with the desired voltage range.                   *
   *                                                                *
   * Return Value:                                                  *
   * If an MMC card is identified, it returns 0.                    *
   * If an SD card is identified, it returns 1.                     *
   * If the voltage range is not supported or the card does not     *
   * initialize correctly, it returns -1.                           */
  cardtype = MMC_sendOpCond(mmc0,0x00100000); 
  
  if (cardtype == 0xFFFF){
     printf ("Card not recognized\n");
     exit(0);
  }
  
  /* Store the card type in the relevant field of the MMC_CardObj    */
  //MMC_setCardType(card, cardtype);

  if (cardtype == MMC_CARD){
     printf ("MMC card found\n");
     printf ("Setting 10 as RCA value\n");
     cid = &cardid;
     /* Instruct all cards to send their Card Identification Data    */
     MMC_sendAllCID(mmc0,cid); // get the CID structure for all cards.
     printf ("The values in the CID of the MMC card are:\n");
     printf ("mgfId         = %d\n",cid->mfgId);
     printf ("hwRew         = %d\n",cid->hwRev);
     printf ("fwRev         = %d\n",cid->fwRev);
     printf ("serialNo      = %d\n",cid->serialNumber);
     printf ("month         = %d\n",cid->monthCode);
     printf ("year          = %d\n",cid->yearCode);
     /* NOTE: Year 0 = 1997 for MMC */
        
     /* Once the host gets the CID for the card, it has to set a      *
      * Relative Card Address that will be used to address the card   *
      * for all future transactions                                   *
      *                                                               *
      * After this command, the card will be in standby state         */
     
     card = &cardalloc;
     
     //retVal = MMC_setRca(mmc0,card,0x8DAB);
     retVal = MMC_setRca(mmc0,card,2);

     /* The host can request for the CSD in the standby state. This   *
      * structure contains the data outlined below                    */
     printf ("Reading CSD structure for MMC card\n");
     csd = &cardcsd;
     retVal = MMC_sendCSD(mmc0);
     MMC_getCardCsd(mmc0, csd);
     printf ("The values in the CSD of the MMC card are:\n");
     printf ("csdStructure     = %d\n", csd->csdStructure);
     printf ("mmcProt          = %d\n", csd->mmcProt);
     printf ("taac             = %d\n", csd->taac);
     printf ("nsac             = %d\n", csd->nsac);
     printf ("tranSpeed        = %d\n", csd->tranSpeed);
     printf ("ccc              = %d\n", csd->ccc);
     printf ("readBlLen        = %d\n", csd->readBlLen);
     printf ("readBlPartial    = %d\n", csd->readBlPartial);
     printf ("writeBlkMisalign = %d\n", csd->writeBlkMisalign);
     printf ("readBlkMisalign  = %d\n", csd->readBlkMisalign);
     printf ("dsrImp           = %d\n", csd->dsrImp);
     printf ("cSize            = %d\n", csd->cSize);
     printf ("vddRCurrMin      = %d\n", csd->vddRCurrMin);
     printf ("vddRCurrMax      = %d\n", csd->vddRCurrMax);
     printf ("vddWCurrMin      = %d\n", csd->vddWCurrMin);
     printf ("vddWCurrMax      = %d\n", csd->vddWCurrMax);
     printf ("cSizeMult        = %d\n", csd->cSizeMult);
     printf ("eraseGrpSize     = %d\n", csd->eraseGrpSize);
     printf ("eraseGrpMult     = %d\n", csd->eraseGrpMult);
     printf ("wpGrpSize        = %d\n", csd->wpGrpSize);
     printf ("wpGrpEnable      = %d\n", csd->wpGrpEnable);
     printf ("defaultEcc       = %d\n", csd->defaultEcc);
     printf ("r2wFactor        = %d\n", csd->r2wFactor);
     printf ("writeBlLen       = %d\n", csd->writeBlLen);
     printf ("writeBlPartial   = %d\n", csd->writeBlPartial);
     printf ("fileFmtGrp       = %d\n", csd->fileFmtGrp);
     printf ("copy             = %d\n", csd->copy);
     printf ("permWriteProtect = %d\n", csd->permWriteProtect);
     printf ("tmpWriteProtect  = %d\n", csd->tmpWriteProtect);
     printf ("fileFmt          = %d\n", csd->fileFmt);
     printf ("ecc              = %d\n", csd->ecc);
     printf ("crc              = 0x%x\n", csd->crc);
     
  } else {
     printf ("SD card found\n");
     cid = &cardid;
     /* Instruct all cards to send their Card Identification Data    */
     SD_sendAllCID(mmc0,cid);
     printf ("The values in the CID of the SD card are:\n");
     printf ("mgfId         = %d\n",cid->mfgId);
     printf ("hwRew         = %d\n",cid->hwRev);
     printf ("fwRev         = %d\n",cid->fwRev);
     printf ("serialNo      = %d\n",cid->serialNumber);
     printf ("month         = %d\n",cid->monthCode);
     printf ("year          = %d\n",cid->yearCode);
     /* NOTE: Year 0 = 2000 for MMC */
 
     /* Once the host gets the CID for the card, it has to request a  *
      * Relative Card Address that will be used to address the card   *
      * for all future transactions                                   *
      *                                                               *
      * NOTE: In case of SD cards, the hosts requests the card to     *
      * send an RCA instead of sending one to the card                *
      * After this command, the card will be in standby state         */
    
     printf ("Asking SD card for its RCA...\n");
     card = &cardalloc;
     rca = SD_sendRca(mmc0,card);
     printf ("RCA sent is 0x%x\n", rca);

     /* The host can request for the CSD in the standby state. This   *
      * structure contains the data outlined below                    */

     retVal = MMC_sendCSD(mmc0);
     sdcsd = &sdcardcsd;
     SD_getCardCsd(mmc0, sdcsd);
     printf ("The values in the CSD of the SD card are:\n");
     printf ("csdStructure     = %d\n", sdcsd->csdStructure);
     printf ("taac             = %d\n", sdcsd->taac);
     printf ("nsac             = %d\n", sdcsd->nsac);
     printf ("tranSpeed        = %d\n", sdcsd->tranSpeed);
     printf ("ccc              = %d\n", sdcsd->ccc);
     printf ("readBlLen        = %d\n", sdcsd->readBlLen);
     printf ("readBlPartial    = %d\n", sdcsd->readBlPartial);
     printf ("writeBlkMisalign = %d\n", sdcsd->writeBlkMisalign);
     printf ("readBlkMisalign  = %d\n", sdcsd->readBlkMisalign);
     printf ("dsrImp           = %d\n", sdcsd->dsrImp);
     printf ("cSize            = %d\n", sdcsd->cSize);
     printf ("vddRCurrMin      = %d\n", sdcsd->vddRCurrMin);
     printf ("vddRCurrMax      = %d\n", sdcsd->vddRCurrMax);
     printf ("vddWCurrMin      = %d\n", sdcsd->vddWCurrMin);
     printf ("vddWCurrMax      = %d\n", sdcsd->vddWCurrMax);
     printf ("cSizeMult        = %d\n", sdcsd->cSizeMult);
     printf ("eraseBlkEn       = %d\n", sdcsd->eraseBlkEn);
     printf ("sectorSize       = %d\n", sdcsd->sectorSize);
     printf ("wpGrpSize        = %d\n", sdcsd->wpGrpSize);
     printf ("wpGrpEnable      = %d\n", sdcsd->wpGrpEnable);
     printf ("r2wFactor        = %d\n", sdcsd->r2wFactor);
     printf ("writeBlLen       = %d\n", sdcsd->writeBlLen);
     printf ("writeBlPartial   = %d\n", sdcsd->writeBlPartial);
     printf ("fileFmtGrp       = %d\n", sdcsd->fileFmtGrp);
     printf ("copy             = %d\n", sdcsd->copy);
     printf ("permWriteProtect = %d\n", sdcsd->permWriteProtect);
     printf ("tmpWriteProtect  = %d\n", sdcsd->tmpWriteProtect);
     printf ("fileFmt          = %d\n", sdcsd->fileFmt);
     printf ("crc              = 0x%x\n", sdcsd->crc);
  
  }        
  /* Initialization is done at this stage. The MMC/SD card is in the   *
   * standby state. At this point, a MMC_selectCard command will       *
   * use the RCA and drive the card to the transfer state. Data        *
   * can now be sent/received from the card. The read_write example    *
   * deals with data transfer in more detail                           */

  printf ("Initialized card successfully\n");
  printf ("TEST PASSED\n");

  retVal = MMC_selectCard(mmc0,card);

  /* MMC_write(Handle, start address, buffer pointer, length in bytes);         *
   * This does a single block transfer to the MMC card. Max length in bytes is  *
   * 512 bytes. datasend buffer has 256 elements of 16 bits each, which total   *
   * upto 512 bytes. By default data bus width is 1 bit. An SD card can support *
   * data bus width of 4 bits. The sd_set_width example shows how this is done  */
  
  printf ("Transferring data to card...\n");
  retVal = MMC_write(mmc0,0x00000000,mysendbuf,512);
  
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
  retVal = MMC_read(mmc0,0x00000000,myrecbuf,512);
  
  /* Verify that data read corresponds to data transferred                      */
  
  for (count = 0; count < 256; ++count)
     if (mysendbuf[count] != myrecbuf[count])
        break;
        
  if (count < 256){
     printf ("\nNot all data was transferred/read successfully!\n");
     printf ("TEST FAILED\n");
  } else {
     printf ("\nData transfer successful\n");
     printf ("TEST PASSED\n");
  }
}


