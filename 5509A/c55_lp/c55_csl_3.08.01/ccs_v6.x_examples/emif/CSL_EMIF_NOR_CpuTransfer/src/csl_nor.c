/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2008
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *  ============================================================================
 */

/** @file csl_nor.c
 *
 *  @brief NOR functional layer API source file
 *
 *  Path: \(CSLPATH)\src\nor\src
 */

/* ============================================================================
 * Revision History
 * ================
 * 15-Oct-2010 Created
 * ============================================================================
 */

#include "..\inc\csl_nor.h"

CSL_Status NOR_setup(CSL_NorObj *norObj, Uint16 instId)
{
	CSL_Status status;

	status = CSL_SOK;

	norObj->emifRegs 	= CSL_EMIF_REGS;
	norObj->sysCtrlRegs = CSL_SYSCTRL_REGS;
	norObj->instId		= instId;

	return status;
}

CSL_Status NOR_emifResetAndClock(CSL_NorHandle hNor)
{
	CSL_Status 	status;
	Uint16		i;

	status = CSL_SOK;

	/* Configure EMIF */
	/* 1c26h ECDR */
	CSL_FINS(hNor->sysCtrlRegs->ECDR, SYS_ECDR_EDIV, 0x1);

	/* 1c04h PSRCR - Set Reset Count */
	CSL_FINS(hNor->sysCtrlRegs->PSRCR, SYS_PSRCR_COUNT, 0x20);

	/* 1c05h PRCR - enable emif self-clearing*/
	CSL_FINS(hNor->sysCtrlRegs->PRCR, SYS_PRCR_PG4_RST, 0x0);

	/* 1c05h PRCR - Reset EMIF */
	CSL_FINS(hNor->sysCtrlRegs->PRCR, SYS_PRCR_PG1_RST, 0x1);

	/* Give some delay for the device to reset */
	for(i = 0; i < 100; i++){;}

	/* Enable EMIF module in Idle PCGCR */
	CSL_FINST(hNor->sysCtrlRegs->PCGCR1, SYS_PCGCR1_EMIFCG, ACTIVE);

	return status;
}

CSL_Status NOR_setupEmifChipSelect(CSL_NorHandle hNor, CSL_NorConfig *norCfg)
{
	CSL_Status 					status;
	volatile ioport Uint16    	*asyncCSnCR1Addr;
    volatile ioport Uint16    	*asyncCSnCR2Addr;

	status = CSL_SOK;

	hNor->commandSet 		= norCfg->commandSet;
	hNor->norWidth			= norCfg->norWidth;
	if(CSL_NOR_16_BIT == norCfg->norWidth) {
		/*hNor->blockSize is in bytes. In 16bit device, one address points to 16bit or 2byte*/
		hNor->blockLength = norCfg->blockSize/2;
	} else if(CSL_NOR_8_BIT == norCfg->norWidth) {
		hNor->blockLength = norCfg->blockSize;
	}

	hNor->numOfBlocks		= norCfg->numOfBlocks;
	hNor->writeBufferSize	= norCfg->writeBufferSize;
	hNor->norOpMode			= norCfg->norOpMode;
	hNor->enableUnlockSeq	= TRUE;
/*	hNor->intrNumNor		= 0;
	hNor->addrCycles		= 0;*/

	/* 1c00h EBSR - Configure EM_A[20:15] for EMIF */
	CSL_FINS(hNor->sysCtrlRegs->EBSR, SYS_EBSR_A20_MODE, 0x0);
	CSL_FINS(hNor->sysCtrlRegs->EBSR, SYS_EBSR_A19_MODE, 0x0);
	CSL_FINS(hNor->sysCtrlRegs->EBSR, SYS_EBSR_A18_MODE, 0x0);
	CSL_FINS(hNor->sysCtrlRegs->EBSR, SYS_EBSR_A17_MODE, 0x0);
	CSL_FINS(hNor->sysCtrlRegs->EBSR, SYS_EBSR_A16_MODE, 0x0);
	CSL_FINS(hNor->sysCtrlRegs->EBSR, SYS_EBSR_A15_MODE, 0x0);
	
	/* 1c33h ESCR - EMIF generates 16bit access to External Memory for every word access */
	CSL_FINS(hNor->sysCtrlRegs->ESCR, SYS_ESCR_BYTEMODE, 0x0);

	/* 1004h AWCCR1 MEWC - 0xFF (max) */
	CSL_FINS(hNor->emifRegs->AWCCR1, EMIF_AWCCR1_MEWC, norCfg->asyncWaitCfg->waitCycles);

	switch(norCfg->norChipSelect)
	{
	case CSL_NOR_CS2:
		/* 1005h AWCCR2 - EM_WAIT0 pin for EM_CS2 */
		CSL_FINS(hNor->emifRegs->AWCCR2, EMIF_AWCCR2_CS2_WAIT, norCfg->asyncWaitCfg->waitPin);
		hNor->CSnBASE = (Uint32)CSL_EMIF_CS2_DATA_BASE_ADDR;
		hNor->DMABYTEADDR = (Uint32)CSL_NAND_CE0_ADDR;
		asyncCSnCR1Addr = &hNor->emifRegs->ACS2CR1;
		asyncCSnCR2Addr = &hNor->emifRegs->ACS2CR2;
		break;
	case CSL_NOR_CS3:
		/* 1005h AWCCR2 - EM_WAIT0 pin for EM_CS3 */
		CSL_FINS(hNor->emifRegs->AWCCR2, EMIF_AWCCR2_CS3_WAIT, norCfg->asyncWaitCfg->waitPin);
		hNor->CSnBASE = (Uint32)CSL_EMIF_CS3_DATA_BASE_ADDR;
		hNor->DMABYTEADDR = (Uint32)CSL_NAND_CE1_ADDR;
		asyncCSnCR1Addr = &hNor->emifRegs->ACS3CR1;
		asyncCSnCR2Addr = &hNor->emifRegs->ACS3CR2;
		break;
	case CSL_NOR_CS4:
		/* 1005h AWCCR2 - EM_WAIT0 pin for EM_CS4 */
		CSL_FINS(hNor->emifRegs->AWCCR2, EMIF_AWCCR2_CS4_WAIT, norCfg->asyncWaitCfg->waitPin);
		hNor->CSnBASE = (Uint32)CSL_EMIF_CS4_DATA_BASE_ADDR;
		hNor->DMABYTEADDR = (Uint32)CSL_ASYNC_CE0_ADDR;
		asyncCSnCR1Addr = &hNor->emifRegs->ACS4CR1;
		asyncCSnCR2Addr = &hNor->emifRegs->ACS4CR2;
		break;
	case CSL_NOR_CS5:
		/* 1005h AWCCR2 - EM_WAIT0 pin for EM_CS5 */
		CSL_FINS(hNor->emifRegs->AWCCR2, EMIF_AWCCR2_CS5_WAIT, norCfg->asyncWaitCfg->waitPin);
		hNor->CSnBASE = (Uint32)CSL_EMIF_CS5_DATA_BASE_ADDR;
		hNor->DMABYTEADDR = (Uint32)CSL_ASYNC_CE1_ADDR;
		asyncCSnCR1Addr = &hNor->emifRegs->ACS5CR1;
		asyncCSnCR2Addr = &hNor->emifRegs->ACS5CR2;
		break;
	default:
		break;
	}

	switch(norCfg->asyncWaitCfg->waitPin)
	{
	case CSL_NOR_WAIT0:
		/* 1005h AWCCR2 - Insert wait cycles if EM_WAIT0 is high/low */
		CSL_FINS(hNor->emifRegs->AWCCR2, EMIF_AWCCR2_WP0, norCfg->asyncWaitCfg->waitPol);
		break;
	case CSL_NOR_WAIT1:
		/* 1005h AWCCR2 - Insert wait cycles if EM_WAIT1 is high/low */
		CSL_FINS(hNor->emifRegs->AWCCR2, EMIF_AWCCR2_WP1, norCfg->asyncWaitCfg->waitPol);
		break;
	case CSL_NOR_WAIT2:
		/* 1005h AWCCR2 - Insert wait cycles if EM_WAIT2 is high/low */
		CSL_FINS(hNor->emifRegs->AWCCR2, EMIF_AWCCR2_WP2, norCfg->asyncWaitCfg->waitPol);
		break;
	case CSL_NOR_WAIT3:
		/* 1005h AWCCR2 - Insert wait cycles if EM_WAIT3 is high/low */
		CSL_FINS(hNor->emifRegs->AWCCR2, EMIF_AWCCR2_WP3, norCfg->asyncWaitCfg->waitPol);
		break;
	default:
		break;
	}

	/* 1010h ACS2CR1 */
	CSL_FINS(*asyncCSnCR1Addr, EMIF_ACS2CR1_RSETUPLSB, (Uint16)(norCfg->asyncCfg->r_setup & 0x7));
	/* Set read strobe duration cycles */
	CSL_FINS(*asyncCSnCR1Addr, EMIF_ACS2CR1_RSTROBE, norCfg->asyncCfg->r_strobe);
	/* Set read strobe hold cycles */
	CSL_FINS(*asyncCSnCR1Addr, EMIF_ACS2CR1_RHOLD, norCfg->asyncCfg->r_hold);
	/* Set turn around cycles */
	CSL_FINS(*asyncCSnCR1Addr, EMIF_ACS2CR1_TA, norCfg->asyncCfg->turnAround);
	/* Set asynchronous memory size */
	CSL_FINS(*asyncCSnCR1Addr, EMIF_ACS2CR1_ASIZE, norCfg->asyncCfg->aSize);

	/* 1011h ACS2CR2 */
	CSL_FINS(*asyncCSnCR2Addr, EMIF_ACS2CR2_SS, norCfg->asyncCfg->selectStrobe);
	/* Set extended wait mode bit */
	CSL_FINS(*asyncCSnCR2Addr, EMIF_ACS2CR2_EW, norCfg->asyncCfg->ewMode);
	/* Set write srobe setup cycles */
	CSL_FINS(*asyncCSnCR2Addr, EMIF_ACS2CR2_WSETUP, norCfg->asyncCfg->w_setup);
	/* Set write duration cycles */
	CSL_FINS(*asyncCSnCR2Addr, EMIF_ACS2CR2_WSTROBE, norCfg->asyncCfg->w_strobe);
	/* Set write hold cycles */
	CSL_FINS(*asyncCSnCR2Addr, EMIF_ACS2CR2_WHOLD, norCfg->asyncCfg->w_hold);
	/* Set read strobe setup cycles */
	CSL_FINS(*asyncCSnCR2Addr, EMIF_ACS2CR2_RSETUPMSB, (norCfg->asyncCfg->r_setup >> 0x3));
#if 0
	/* 1068h PGMODECTRL1 & 1069h PGMODECTRL2*/
	/*For PHOENIX_QT*/
	CSL_FINS(hNor->emifRegs->PAGEMODCTRL1, EMIF_PAGEMODCTRL1_CS2_PAGE_DELAY, 0x2);
	CSL_FINS(hNor->emifRegs->PAGEMODCTRL1, EMIF_PAGEMODCTRL1_CS2_PAGE_SIZE, 0x1);
	CSL_FINS(hNor->emifRegs->PAGEMODCTRL1, EMIF_PAGEMODCTRL1_CS2_PAGEMOD_EN, 0x1);

	/*For EVM5515*/
	CSL_FINS(hNor->emifRegs->PAGEMODCTRL1, EMIF_PAGEMODCTRL1_CS2_PAGE_DELAY, 0x2);
	CSL_FINS(hNor->emifRegs->PAGEMODCTRL1, EMIF_PAGEMODCTRL1_CS2_PAGE_SIZE, 0x0);/*don't know*/
	CSL_FINS(hNor->emifRegs->PAGEMODCTRL1, EMIF_PAGEMODCTRL1_CS2_PAGEMOD_EN, 0x0);/*don't know*/
#endif
	return status;
}

CSL_Status NOR_readId(CSL_NorHandle hNor)
{
	CSL_Status 	status;

	status = CSL_SOK;

	/* Read NOR ID */
	if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
	{
		*(CSL_VUint16*)hNor->CSnBASE = 0x90;
	}
	else if (CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
		if(CSL_NOR_8_BIT == hNor->norWidth)
		{
			*(CSL_VUint16*)(hNor->CSnBASE+0xAAA) = 0xAA;
			*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0x55;
			*(CSL_VUint16*)(hNor->CSnBASE+0xAAA) = 0x90;
		}
		else if(CSL_NOR_16_BIT == hNor->norWidth)
		{
			*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0xAA;
			*(CSL_VUint16*)(hNor->CSnBASE+0x2AA) = 0x55;
			*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0x90;
		}
	}
	__asm("	NOP");/*For "A write followed by A read" pre-fetch reordering*/
	__asm("	NOP");
	__asm("	NOP");
	if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
	{
		hNor->deviceID[0] = (*(CSL_VUint16*)(hNor->CSnBASE+0x00));/*man id*/
		hNor->deviceID[1] = (*(CSL_VUint16*)(hNor->CSnBASE+0x01));/*dev id*/
		hNor->deviceID[2] = (*(CSL_VUint16*)(hNor->CSnBASE+0x05));/*rcr*/
	}
	else if (CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
		if(CSL_NOR_8_BIT == hNor->norWidth)
		{
			hNor->deviceID[0] = (*(CSL_VUint16*)(hNor->CSnBASE+0x00));/*man id*/
			hNor->deviceID[1] = (*(CSL_VUint16*)(hNor->CSnBASE+0x02));/*dev id-1*/
			hNor->deviceID[2] = (*(CSL_VUint16*)(hNor->CSnBASE+0x1C));/*dev id-2*/
			hNor->deviceID[3] = (*(CSL_VUint16*)(hNor->CSnBASE+0x1E));/*dev id-3*/
		}
		else if(CSL_NOR_16_BIT == hNor->norWidth)
		{
			hNor->deviceID[0] = (*(CSL_VUint16*)(hNor->CSnBASE+0x00));/*man id*/
			hNor->deviceID[1] = (*(CSL_VUint16*)(hNor->CSnBASE+0x01));/*dev id-1*/
			hNor->deviceID[2] = (*(CSL_VUint16*)(hNor->CSnBASE+0x0E));/*dev id-2*/
			hNor->deviceID[3] = (*(CSL_VUint16*)(hNor->CSnBASE+0x0F));/*dev id-3*/
		}

		/*Reset to exit auto-select mode for AMD*/
		*(CSL_VUint16*)(hNor->CSnBASE+0x0) = 0xF0;
	}

	return status;
}

/* This is available only for Intel Command Based*/
/*The second arg is long because it is the block address not the block number*/
static CSL_Status NOR_readAndClearStatus(CSL_NorHandle hNor, Uint32 dBlockOffset, Uint16 *norStatus)
{
	CSL_Status status;

	status = CSL_SOK;

	if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
	{/* Read status register */
		*(CSL_VUint16*)(hNor->CSnBASE+dBlockOffset) = 0x70;

		__asm("	NOP");
		__asm("	NOP");
		__asm("	NOP");
		*norStatus = (*(CSL_VUint16*)(hNor->CSnBASE+dBlockOffset));

		/* Clear status register */
		*(CSL_VUint16*)(hNor->CSnBASE+dBlockOffset) = 0x50;
		__asm("	NOP");
		__asm("	NOP");
		__asm("	NOP");
	}
	return status;
}

CSL_Status NOR_readCFI(CSL_NorHandle hNor)
{
	CSL_Status status;

	status = CSL_SOK;

	/* Read CFI */
	if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
	{
		*(CSL_VUint16*)(hNor->CSnBASE+0x10) = 0x98;
	}
	else if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
		if(CSL_NOR_8_BIT == hNor->norWidth)
		{
			*(CSL_VUint16*)(hNor->CSnBASE+0xAA) = 0x98;
		}
		else if(CSL_NOR_16_BIT == hNor->norWidth)
		{
			*(CSL_VUint16*)(hNor->CSnBASE+0x55) = 0x98;
		}
	}

	__asm("	NOP");
	__asm("	NOP");
	__asm("	NOP");
	hNor->cfiData[0] = (*(CSL_VUint16*)(hNor->CSnBASE+0x26));
	hNor->cfiData[1] = (*(CSL_VUint16*)(hNor->CSnBASE+0x27));
	hNor->cfiData[2] = (*(CSL_VUint16*)(hNor->CSnBASE+0x28));
	hNor->cfiData[3] = (*(CSL_VUint16*)(hNor->CSnBASE+0x29));
	hNor->cfiData[4] = (*(CSL_VUint16*)(hNor->CSnBASE+0x2a));
	hNor->cfiData[5] = (*(CSL_VUint16*)(hNor->CSnBASE+0x2b));
	hNor->cfiData[6] = (*(CSL_VUint16*)(hNor->CSnBASE+0x2c));
	hNor->cfiData[7] = (*(CSL_VUint16*)(hNor->CSnBASE+0x2d));
	hNor->cfiData[8] = (*(CSL_VUint16*)(hNor->CSnBASE+0x2e));

	if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
		/*Reset to exit Read-CFI mode in AMD*/
		*(CSL_VUint16*)(hNor->CSnBASE+0x0) = 0xF0;
	}

	return status;
}

CSL_Status NOR_readBlockLockStatus(CSL_NorHandle hNor, Uint16 *blockLockStatus)
{
	CSL_Status 	status;
	Uint16		blockLockConf;
	Uint32		i;

	status = CSL_SOK;

	if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
	{
		*(CSL_VUint16*)hNor->CSnBASE = 0x90;
	}
	else if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
		if(CSL_NOR_8_BIT == hNor->norWidth)
		{
			*(CSL_VUint16*)(hNor->CSnBASE+0xAAA) = 0xAA;
			*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0x55;
			*(CSL_VUint16*)(hNor->CSnBASE+0xAAA) = 0x90;
		}
		else if(CSL_NOR_16_BIT == hNor->norWidth)
		{
			*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0xAA;
			*(CSL_VUint16*)(hNor->CSnBASE+0x2AA) = 0x55;
			*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0x90;
		}
	}
	__asm("	NOP");
	__asm("	NOP");
	__asm("	NOP");
	for(i=0; i<=130/*hNor->numOfBlocks*/; i++)//taking only first 130 blocks
	{
		if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
		{
			blockLockConf = (*(CSL_VUint16*)(hNor->CSnBASE+(i*hNor->blockLength)+0x2));
		}
		else if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
		{
			/*This is the same for 8-bit and 16-bit*/
			blockLockConf = (*(CSL_VUint16*)(hNor->CSnBASE+((i*hNor->blockLength)&0xFFFFFF00)+0x02));
		}

		if(blockLockConf & 0x1)
		{
			/*Block is LOCKED*/
			blockLockStatus[i/16] |= (0x1 << (i%16));
			/*If bit[1] is set then Block is LOCKED-DOWN*/
		}
		else
		{
			/*Block is Unlocked*/
			blockLockStatus[i/16] &= (0x1 << (i%16));
		}
	}

	return status;
}

CSL_Status NOR_unlockBlock(CSL_NorHandle hNor, Uint16 blockNum, Uint16 *blockLockStatus)
{
	CSL_Status 	status;
	Uint32		dBlockAddress;

	status = CSL_SOK;

	dBlockAddress = (Uint32)blockNum * hNor->blockLength;
	/* Lock Block Setup Command */
	if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
	{
		*(CSL_VUint16*)(hNor->CSnBASE+dBlockAddress) = 0x60;
	}
	else if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
	}

	/* Unlock Block Command */
	if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
	{
		*(CSL_VUint16*)(hNor->CSnBASE+dBlockAddress) = 0xD0;
	}
	else if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
	}

	/* Read ID Command */
	if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
	{
		*(CSL_VUint16*)hNor->CSnBASE = 0x90;
	}
	else if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
	}
	__asm("	NOP");
	__asm("	NOP");
	__asm("	NOP");


	*blockLockStatus = (*(CSL_VUint16*)(hNor->CSnBASE+dBlockAddress+0x2));

	return status;
}

CSL_Status NOR_readArray(CSL_NorHandle hNor, Uint16 blockNum, Uint16 wordOffset)
{
	CSL_Status 	status;
	Uint32		dWordAddress;

	status = CSL_SOK;

	dWordAddress = ((Uint32)blockNum * hNor->blockLength) + (Uint32)wordOffset;
	/* Read Array Command */
	if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
	{
		*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress) = 0xFF;
	}
	else if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
		/*No unlock or command cycle required when bank is reading array data
		Issuing a reset command, as it may be required in some cases and
		it doesn't matter in others*/
		/* This is the same for 8 and 16 bit devices*/
		*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress) = 0xF0;
	}
	__asm("	NOP");
	__asm("	NOP");
	__asm("	NOP");

	return status;
}


CSL_Status NOR_enterUnlockBypassedMode(CSL_NorHandle hNor)
{
	CSL_Status status;

	status = CSL_SOK;

	/*This is only for amd cmdset based nor device*/
	/*Unlock bypass is possible only for wordWrite, blockErase and chipErase*/
	if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
		if(CSL_NOR_8_BIT == hNor->norWidth)
		{
			*(CSL_VUint16*)(hNor->CSnBASE+0xAAA) = 0xAA;
			*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0x55;
			*(CSL_VUint16*)(hNor->CSnBASE+0xAAA) = 0x20;
		}
		else if(CSL_NOR_16_BIT == hNor->norWidth)
		{
			*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0xAA;
			*(CSL_VUint16*)(hNor->CSnBASE+0x2AA) = 0x55;
			*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0x20;
		}
		hNor->enableUnlockSeq = FALSE;
	}

	return status;
}


CSL_Status NOR_exitUnlockBypassedMode(CSL_NorHandle hNor)
{
	CSL_Status status;

	status = CSL_SOK;

	/*This is only for amd cmdset based nor device*/
	/*Unlock bypass is possible only for wordWrite, blockErase and chipErase*/
	if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
		/*This is the same for 8-bit and 16-bit devices*/
		*(CSL_VUint16*)(hNor->CSnBASE+0x0) = 0x90;
		*(CSL_VUint16*)(hNor->CSnBASE+0x0) = 0x00;
	}
	hNor->enableUnlockSeq = TRUE;

	return status;
}


CSL_Status NOR_eraseBlock(CSL_NorHandle hNor, Uint16 blockNum,
							Uint16 statusMode, Uint16 *eraseStatus)
{
	CSL_Status 		status;
	volatile Uint16 pollStatus;
	volatile Uint16 dqNValue;
	Uint32			dBlockAddress;

	status = CSL_SOK;

	dBlockAddress = (Uint32)blockNum*hNor->blockLength;

	/* Block erase setup */
	if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
	{
		*(CSL_VUint16*)(hNor->CSnBASE+dBlockAddress) = 0x20;
	}
	else if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
		if(CSL_NOR_8_BIT == hNor->norWidth)
		{
			if(hNor->enableUnlockSeq)
			{
				*(CSL_VUint16*)(hNor->CSnBASE+0xAAA) = 0xAA;
				*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0x55;
			}
			*(CSL_VUint16*)(hNor->CSnBASE+0xAAA) = 0x80;
		}
		else if(CSL_NOR_16_BIT == hNor->norWidth)
		{
			if(hNor->enableUnlockSeq)
			{
				*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0xAA;
				*(CSL_VUint16*)(hNor->CSnBASE+0x2AA) = 0x55;
			}
			*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0x80;
		}
	}

	/* Block erase confirm */
	if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
	{
		*(CSL_VUint16*)(hNor->CSnBASE+dBlockAddress) = 0xD0;
	}
	else if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
		if((CSL_NOR_8_BIT == hNor->norWidth) && hNor->enableUnlockSeq)
		{
			*(CSL_VUint16*)(hNor->CSnBASE+0xAAA) = 0xAA;
			*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0x55;
		}
		else if((CSL_NOR_16_BIT == hNor->norWidth) && hNor->enableUnlockSeq)
		{
			*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0xAA;
			*(CSL_VUint16*)(hNor->CSnBASE+0x2AA) = 0x55;
		}
		*(CSL_VUint16*)(hNor->CSnBASE+dBlockAddress) = 0x30;
	}

	if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
	{
		do {
			status = NOR_readAndClearStatus(hNor, 0, eraseStatus);
		} while(!(*eraseStatus & 0x0080));
	}
	else if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
	#if 0
		/*Check DQ3 for Erase Timer timedout condition*/
		do {
			dqNValue = (*(CSL_VUint16*)(hNor->CSnBASE+dBlockAddress));
		} while(0x0008 != (dqNValue & 0x0008));
	#endif
		/*After erase timer times out. erase algorithm begins*/
		if(CSL_NOR_DQ7_STATUS == statusMode)
		{
			do {
				pollStatus = (*(CSL_VUint16*)(hNor->CSnBASE+dBlockAddress));
				if(0x0080 == (pollStatus&0x0080))
				{/*If DQ7=1 then erase complete or erase suspended state*/
					break;
				}
			} while(0x0020 != (pollStatus&0x0020));/*Program/Erase/Write-to-Buffer timedout*/

			if(0x0020 == (pollStatus&0x0020))/*A timeout happened*/
			{
				pollStatus = (*(CSL_VUint16*)(hNor->CSnBASE+dBlockAddress));
				if(0x0080 == (pollStatus&0x0080))
				{
					status = CSL_SOK;
				}
				else
				{
					status = CSL_EMMCSD_TIMEOUT;
					/*Issue a reset to change to read array mode*/
					*(CSL_VUint16*)(hNor->CSnBASE+0x0) = 0xF0;
				}
			}
		}/*DQ7 status*/
		else if (CSL_NOR_DQ6_STATUS == statusMode)
		{
			do {
				dqNValue = (*(CSL_VUint16*)(hNor->CSnBASE+dBlockAddress));
				pollStatus = (*(CSL_VUint16*)(hNor->CSnBASE+dBlockAddress));
				#if 0
				if((dqNValue&0x0004) == (pollStatus&0x0004))
				{/*DQ2 not toggling. This sector is not for erase*/
				/*As we are checking the status soon after the erase command,
				this condition shall always be in the toggle state*/
					break;
				}
				#endif
				if((dqNValue&0x0040) == (pollStatus&0x0040))
				{/*DQ6 not toggling. Erase suspend or Erase complete*/
					break;
				}
			} while(0x0020 != (pollStatus&0x0020));/*Program/Erase/Write-to-Buffer timedout*/

			if(0x0020 == (pollStatus&0x0020))/*A timeout happened*/
			{
				dqNValue = (*(CSL_VUint16*)(hNor->CSnBASE+dBlockAddress));
				pollStatus = (*(CSL_VUint16*)(hNor->CSnBASE+dBlockAddress));
				if((dqNValue&0x0040) == (pollStatus&0x0040))
				{
					status = CSL_SOK;
				}
				else
				{
					status = CSL_EMMCSD_TIMEOUT;
					/*Issue a reset to change to read array mode*/
					*(CSL_VUint16*)(hNor->CSnBASE+0x0) = 0xF0;
				}
			}
		}/*DQ6 status*/

		*eraseStatus = pollStatus;
	}/*amd cmdset*/

	return status;
}

/*Uint16 is ok for wordOffset as we deal with 64KWord NOR device. Range is 0x0-0xFFFF*/
CSL_Status NOR_writeWord(CSL_NorHandle hNor, Uint16 blockNum,
						Uint16 wordOffset, Uint16 wordData,
						Uint16 statusMode, Uint16 *writeStatus)
{
	CSL_Status		status;
	Uint32			dWordAddress;
	volatile Uint16 pollStatus;
	volatile Uint16 dqNValue;

	status = CSL_SOK;

	dWordAddress = ((Uint32)blockNum * hNor->blockLength) + (Uint32)wordOffset;

	if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
	{
		*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress) = 0x40;
	}
	else if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
		if(CSL_NOR_8_BIT == hNor->norWidth)
		{
			if(hNor->enableUnlockSeq)
			{
				*(CSL_VUint16*)(hNor->CSnBASE+0xAAA) = 0xAA;
				*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0x55;
			}
			*(CSL_VUint16*)(hNor->CSnBASE+0xAAA) = 0xA0;
		}
		else if(CSL_NOR_16_BIT == hNor->norWidth)
		{*(CSL_VUint16*)(hNor->CSnBASE+0x0) = 0xF0;
			if(hNor->enableUnlockSeq)
			{
				*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0xAA;
				*(CSL_VUint16*)(hNor->CSnBASE+0x2AA) = 0x55;
			}
			*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0xA0;
		}
	}

	*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress) = wordData;

	if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
	{
		do {
			status = NOR_readAndClearStatus(hNor, 0, writeStatus);
		} while(!((*writeStatus) & 0x0080));
	}
	else if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
		if(CSL_NOR_DQ7_STATUS == statusMode)
		{
			dqNValue = wordData&0x0080;
			do {
				pollStatus = (*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress));
				if(dqNValue == (pollStatus&0x0080))
				{/*If DQ7 read, is equal to DQ7 written then write is complete*/
					break;
				}
			} while(0x0020 != (pollStatus&0x0020));/*Program/Erase/Write-to-Buffer timedout*/

			if(0x0020 == (pollStatus&0x0020))/*A timeout happened*/
			{
				pollStatus = (*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress));
				if(dqNValue == (pollStatus&0x0080))
				{
					status = CSL_SOK;
				}
				else
				{
					status = CSL_EMMCSD_TIMEOUT;
					/*Issue a reset to change to read array mode*/
					*(CSL_VUint16*)(hNor->CSnBASE+0x0) = 0xF0;
				}
			}
		}/*DQ7 status*/
		else if (CSL_NOR_DQ6_STATUS == statusMode)
		{
			do {
				dqNValue = (*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress));
				pollStatus = (*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress));
				if((dqNValue&0x0040) == (pollStatus&0x0040))
				{/*If DQ6 stopped its toggle then write is complete*/
					break;
				}
			} while(0x0020 != (pollStatus&0x0020));/*Program/Erase/Write-to-Buffer timedout*/

			if(0x0020 == (pollStatus&0x0020))/*A timeout happened*/
			{
				/*read twice. will the compiler optimize??*/
				dqNValue = (*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress));
				pollStatus = (*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress));
				if((dqNValue&0x0040) == (pollStatus&0x0040))
				{/*If DQ6 stopped its toggle then write is complete*/
					status = CSL_SOK;
				}
				else
				{
					status = CSL_EMMCSD_TIMEOUT;
					/*Issue a reset to change to read array mode*/
					*(CSL_VUint16*)(hNor->CSnBASE+0x0) = 0xF0;
				}
			}
		}/*DQ6 status*/

		*writeStatus = pollStatus;
	}/*amd cmdset*/

	return status;
}


CSL_Status NOR_writeBufferSetup(CSL_NorHandle hNor, Uint16 blockNum, 
								Uint16 wordOffset, Uint16 numOfWords)
{
	CSL_Status 		status;
	Uint32			dWordAddress;
	Uint32			dBlockAddress;
	Uint16			writeStatus;

	status = CSL_SOK;

	if(hNor->writeBufferSize < numOfWords)
		status = CSL_ESYS_FAIL;

	dBlockAddress = (Uint32)blockNum * hNor->blockLength;
	dWordAddress = ((Uint32)blockNum * hNor->blockLength) + (Uint32)wordOffset;

	/* Buffered Program Setup command */
	if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
	{
		*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress) = 0xE8;

		do {/* Wait for Buffer Ready on SR[7]*/
			#if 0
			status = NOR_readAndClearStatus(hNor, dWordAddress, &writeStatus);
			#endif
			writeStatus = *(CSL_VUint16*)(hNor->CSnBASE+dWordAddress);
		} while(!(writeStatus & 0x80));

		/* Write the number of Words to be written */
		*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress) = numOfWords-1;
	}
	else if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
		if(CSL_NOR_8_BIT == hNor->norWidth)
		{
			*(CSL_VUint16*)(hNor->CSnBASE+0xAAA) = 0xAA;
			*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0x55;
		}
		else if(CSL_NOR_16_BIT == hNor->norWidth)
		{
			*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0xAA;
			*(CSL_VUint16*)(hNor->CSnBASE+0x2AA) = 0x55;
		}
		*(CSL_VUint16*)(hNor->CSnBASE+dBlockAddress) = 0x25;
		*(CSL_VUint16*)(hNor->CSnBASE+dBlockAddress) = numOfWords-1;
	}/*amd cmdset*/

	return status;
}

CSL_Status NOR_writeBufferComplete(CSL_NorHandle hNor, Uint16 blockNum, 
									Uint16 wordOffset, Uint16 numOfWords,
									Uint16 *writeBuffer, Uint16 statusMode,
									Uint16 *writeStatus)
{
	CSL_Status 		status;
	Uint32			dWordAddress;
	Uint32			dBlockAddress;
	volatile Uint16 pollStatus=0;
	volatile Uint16 dqNValue=0;

	status = CSL_SOK;

	if(hNor->writeBufferSize < numOfWords)
		status = CSL_ESYS_FAIL;

	dBlockAddress = (Uint32)blockNum * hNor->blockLength;
	dWordAddress = ((Uint32)blockNum * hNor->blockLength) + (Uint32)wordOffset;

	/* Buffered Program Confirm command */
	if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
	{
		*(CSL_VUint16*)(hNor->CSnBASE+dBlockAddress) = 0xD0;
	}
	else if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
		*(CSL_VUint16*)(hNor->CSnBASE+dBlockAddress) = 0x29;
	}

	if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
	{
		do { /*Wait for Buffer Ready on SR[7]*/
			status = NOR_readAndClearStatus(hNor, 0, writeStatus);
		} while(!((*writeStatus) & 0x80));
	}
	else if(CSL_NOR_AMD_STD_CMDSET & hNor->commandSet)
	{
		if(CSL_NOR_DQ7_STATUS == statusMode)
		{
//			dqNValue = writeBuffer[numOfWords-1] & 0x0080;/*This is not working properly, with s/m test on QT - Pramod*/
			dqNValue = *(writeBuffer+numOfWords-1);
			dqNValue &= 0x0080;
			do {
				pollStatus = (*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress+numOfWords-1));
				if(dqNValue == (pollStatus&0x0080))
				{/*If DQ7 read, is equal to DQ7 written then write is complete*/
					break;
				}
				if(0x0002 == (pollStatus&0x0002))
				{/*When DQ1=1, it means that the Write-to-buffer programming has aborted*/
					/*Abort write-to-buffer*/
					if(CSL_NOR_8_BIT == hNor->norWidth)
					{
						*(CSL_VUint16*)(hNor->CSnBASE+0xAAA) 		= 0xAA;
						*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress) = 0x55;
						*(CSL_VUint16*)(hNor->CSnBASE+0x555) 		= 0xF0;
					}
					else if(CSL_NOR_16_BIT == hNor->norWidth)
					{
						*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0xAA;
						*(CSL_VUint16*)(hNor->CSnBASE+0x2AA) = 0x55;
						*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0xF0;
					}
					status = CSL_ESYS_FAIL;
					break;
				}
			} while(0x0020 != (pollStatus&0x0020));/*Program/Erase/Write-to-Buffer timedout*/

			if(0x0020 == (pollStatus&0x0020))/*A timeout happened*/
			{
				pollStatus = (*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress+numOfWords-1));
				if(dqNValue == (pollStatus&0x0080))
				{
					status = CSL_SOK;
				}
				else
				{
					status = CSL_EMMCSD_TIMEOUT;
					/*Issue a reset to change to read array mode*/
					*(CSL_VUint16*)(hNor->CSnBASE+0x0) = 0xF0;
				}
			}
			/*DQ1=1, is handled below*/
		}/*DQ7 status*/
		else if (CSL_NOR_DQ6_STATUS == statusMode)
		{
			do {
				dqNValue = (*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress+(Uint32)(numOfWords-1)));
				pollStatus = (*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress+(Uint32)(numOfWords-1)));
				if((dqNValue&0x0040) == (pollStatus&0x0040))
				{/*If DQ6 stopped its toggle then write is complete*/
					break;
				}
				if(0x0002 == (pollStatus&0x0002))
				{/*When DQ1=1, it means that the Write-to-buffer programming has aborted*/
					/*Abort write-to-buffer*/
					if(CSL_NOR_8_BIT == hNor->norWidth)
					{
						*(CSL_VUint16*)(hNor->CSnBASE+0xAAA) 		= 0xAA;
						*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress) = 0x55;
						*(CSL_VUint16*)(hNor->CSnBASE+0x555) 		= 0xF0;
					}
					else if(CSL_NOR_16_BIT == hNor->norWidth)
					{
						*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0xAA;
						*(CSL_VUint16*)(hNor->CSnBASE+0x2AA) = 0x55;
						*(CSL_VUint16*)(hNor->CSnBASE+0x555) = 0xF0;
					}
					status = CSL_ESYS_FAIL;
					break;
				}
			} while(0x0020 != (pollStatus&0x0020));/*Program/Erase/Write-to-Buffer timedout*/

			if(0x0020 == (pollStatus&0x0020))/*A timeout happened*/
			{
				/*read twice. will the compiler optimize??*/
				dqNValue = (*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress+(Uint32)(numOfWords-1)));
				pollStatus = (*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress+(Uint32)(numOfWords-1)));
				if((dqNValue&0x0040) == (pollStatus&0x0040))
				{/*If DQ6 stopped its toggle then write is complete*/
					status = CSL_SOK;
				}
				else
				{
					status = CSL_EMMCSD_TIMEOUT;
					/*Issue a reset to change to read array mode*/
					*(CSL_VUint16*)(hNor->CSnBASE+0x0) = 0xF0;
				}
			}
			/*DQ1=1, is handled below*/
		}/*DQ6 status*/

		*writeStatus = pollStatus;
	}/*amd cmdset*/

	return status;
}



