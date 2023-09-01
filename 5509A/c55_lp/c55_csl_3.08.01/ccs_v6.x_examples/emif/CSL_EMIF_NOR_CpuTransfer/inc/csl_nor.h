/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2008
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *  ============================================================================
 */

/** @file csl_nor.h
 *
 *  @brief NOR functional layer API header file
 *
 *  Path: \(CSLPATH)\ inc
 */

/* ============================================================================
 * Revision History
 * ================
 * 15-Oct-2010 Created
 * ============================================================================
 */

#ifndef _CSL_NOR_H_
#define _CSL_NOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cslr.h>
#include <csl_error.h>
#include <csl_types.h>
#include <cslr_emif.h>
#include <soc.h>
#include <csl_general.h>
#include <csl_dma.h>

/*Default asyncwait configurations from 2.01 c5515 csl*/
#define CSL_NOR_ASYNCWAITCFG_WAITPOL_DEFAULT       	(0x00)
#define CSL_NOR_ASYNCWAITCFG_WAITPIN_DEFAULT		(0x00)
#define CSL_NOR_ASYNCWAITCFG_WAITCYCLE_DEFAULT     	(0x80)

/*Default async configurations from 2.01 c5515 csl*/
#define CSL_NOR_ASYNCCFG_SELECTSTROBE_DEFAULT      	(0x00)
#define CSL_NOR_ASYNCCFG_WEMODE_DEFAULT            	(0x00)
#define CSL_NOR_ASYNCCFG_WSETUP_DEFAULT            	(0x0F)
#define CSL_NOR_ASYNCCFG_WSTROBE_DEFAULT           	(0x3F)
#define CSL_NOR_ASYNCCFG_WHOLD_DEFAULT             	(0x07)
#define CSL_NOR_ASYNCCFG_RSETUP_DEFAULT            	(0x0F)
#define CSL_NOR_ASYNCCFG_RSTROBE_DEFAULT           	(0x3F)
#define CSL_NOR_ASYNCCFG_RHOLD_DEFAULT             	(0x07)
#define CSL_NOR_ASYNCCFG_TAROUND_DEFAULT           	(0x03)
#define CSL_NOR_ASYNCCFG_ASIZE_DEFAULT             	(0x00)

/*Different command sets used by NOR device*/
#define CSL_NOR_INTEL_STD_CMDSET 0x1
#define CSL_NOR_INTEL_ADV_CMDSET 0x2
#define CSL_NOR_AMD_STD_CMDSET   0x4
#define CSL_NOR_AMD_ADV_CMDSET   0x8

/*Status check defines for amd cmdset based NOR device*/
#define CSL_NOR_DQ7_STATUS 0x1
#define CSL_NOR_DQ6_STATUS 0x2
#define CSL_NOR_BYPASS_UNLOCK_SEQUENCE 0x4

/*Intel Std command set for reference*/
#define CSL_NOR_INTEL_READ_ARRAY 			0xFFu
#define CSL_NOR_INTEL_READ_STATUS	 		0x70u
#define CSL_NOR_INTEL_READ_CONFIG	 		0x90u
#define CSL_NOR_INTEL_READ_ID 				0x90u
#define CSL_NOR_INTEL_READ_CFI				0x98u
#define CSL_NOR_INTEL_CLEAR_STATUS			0x50u
#define CSL_NOR_INTEL_WORD_PRGM_SETUP		0x40u
#define CSL_NOR_INTEL_WORD_PRGM_SETUP_ALT 	0x10u
#define CSL_NOR_INTEL_BUFFERED_PRGM_SETUP	0xE8u
#define CSL_NOR_INTEL_BUFFERED_PRGM_CONFIRM	0xD0u
#define CSL_NOR_INTEL_BEFP_SETUP 			0x80u
#define CSL_NOR_INTEL_BEFP_CONFIRM 			0xD0u
#define CSL_NOR_INTEL_BLOCK_ERASE_SETUP 	0x20u
#define CSL_NOR_INTEL_BLOCK_ERASE_CONFIRM 	0xD0u
#define CSL_NOR_INTEL_PRGM_ERASE_SUSPEND 	0xB0u
#define CSL_NOR_INTEL_PRGM_ERASE_RESUME 	0xD0u
#define CSL_NOR_INTEL_BLOCK_LOCK_SETUP 		0x60u
#define CSL_NOR_INTEL_BLOCK_LOCK 			0x01u
#define CSL_NOR_INTEL_BLOCK_UNLOCK 			0xD0u
#define CSL_NOR_INTEL_BLOCK_LOCK_DOWN 		0x2Fu
#define CSL_NOR_INTEL_PRGM_PROTECTION_SETUP 0xC0u
#define CSL_NOR_INTEL_WRITE_CONFIG_SETUP 	0x60u
#define CSL_NOR_INTEL_WRITE_CONFIG 			0x03u

/*Intel Adv command set for reference*/
/*Amd Std command set for reference*/
/*Amd Adv command set for reference*/

/**
 * \brief NOR Width enumeration
 */
typedef enum {
	/** External Device Bus Width 8 bit */
    CSL_NOR_8_BIT  = (0u),
    /** External Device Bus Width 16 bit */
    CSL_NOR_16_BIT = (1u)
} CSL_NorWidth;

/**
 * \brief   NOR Chip selection
 */
typedef enum {
	/** Chip selsect 2 */
    CSL_NOR_CS2 = (0u),
	/** Chip selsect 3 */
    CSL_NOR_CS3 = (1u),
    /** Chip selsect 4 */
    CSL_NOR_CS4 = (2u),
    /** Chip selsect 5 */
    CSL_NOR_CS5 = (3u)
} CSL_NorChipSelect;

/**
 * \brief   NOR Operating mode
 */
typedef enum {
	/** polled mode of operation */
    CSL_NOR_OPMODE_POLLED = (0u),
	/** DMA mode of operation */
    CSL_NOR_OPMODE_DMA = (1u),
    /** Interrupt mode operation */
    CSL_NOR_OPMODE_INTRRUPT = (2u)
} CSL_NorOpMode;

/**
 * \brief   NOR Wait Pins
 */
typedef enum {
	/** EM_WAIT0 */
    CSL_NOR_WAIT0 = (0u),
	/** EM_WAIT1 */
    CSL_NOR_WAIT1 = (1u),
    /** EM_WAIT2 */
    CSL_NOR_WAIT2 = (2u),
    /** EM_WAIT3 */
    CSL_NOR_WAIT3 = (3u)
} CSL_NorWaitPin;

/**
 * \brief   NOR wait polarity
 */
typedef enum {
	/** wait polarity low                                               */
    CSL_NOR_WP_LOW = (0u),
	/** wait polarity high                                              */
    CSL_NOR_WP_HIGH = (1u)
} CSL_NorWaitPol;

/**
 * \brief Current Asynchronous NOR Bank configuration.
 */
typedef struct CSL_NorAsyncBank {
	/** Current NOR Base */
    volatile Uint32     CExBASE;
} CSL_NorAsyncBank;

/**
 * \brief NOR Async wait config structure
 */
typedef struct CSL_NorAsyncWaitCfg {
    /** Wait polarity for nand ports                                         */
	CSL_NorWaitPol       waitPol;
	/** Nand port Id                                                         */
	CSL_NorWaitPin       waitPin;
	/** EMIF maximum extended wait cycles                                    */
    Uint16              waitCycles;
} CSL_NorAsyncWaitCfg;

/**
 * \brief Nor Async config structure
 */
typedef struct CSL_NorAsyncCfg {
	/** Strobe mode selection */
    Uint16    selectStrobe;
    /** Enable extended wait mode */
    Uint16    ewMode;
    /** Write strobe setup cycles */
    Uint16    w_setup;
    /** Write strobe duration cycles */
    Uint16    w_strobe;
    /** Write strobe hold cycle */
    Uint16    w_hold;
    /** Read strobe setup cycles */
    Uint16    r_setup;
    /** Read strobe duration cycles */
	Uint16    r_strobe;
	/** Read strobe hold cycle */
    Uint16    r_hold;
    /** Turn around cycles */
    Uint16    turnAround;
	/** Asynchronous memory size */
    Uint16    aSize;
} CSL_NorAsyncCfg;

/**
 * \brief Nand Hw configuration structure
 */
typedef struct CSL_NorConfig {
	/** External Device Width */
	CSL_NorWidth           norWidth;
    /** NOR Operating mode */
    CSL_NorOpMode         norOpMode;
    /** NOR chip select Id */
	CSL_NorChipSelect     norChipSelect;
	/** Supported command sets */ 
	Uint16				commandSet;
	/**Block size*/
	Uint32				blockSize;
	/**Number of blocks*/
	Uint32				numOfBlocks;
	/**Size of the internal write buffer in bytes*/
	Uint16				writeBufferSize;
	/** NOR Async wait config structure */
	CSL_NorAsyncWaitCfg   *asyncWaitCfg;
	/** NOR Async config structure */
	CSL_NorAsyncCfg       *asyncCfg;
} CSL_NorConfig;

/**
 * \brief This object contains the reference to the instance of NOR device
 *
 *  This object is initialized during NOR initialization and passed as
 *  the parameter to all NOR CSL APIs
 */
typedef struct CSL_NorObj {
	/** EMIF register overlay structure */
    CSL_EmifRegsOvly    emifRegs;
    /** CPU system control register overlay structure */
    CSL_SysRegsOvly     sysCtrlRegs;
	/** CS space base address */
	volatile Uint32		CSnBASE;
	/**DMA byte address*/
	Uint32				DMABYTEADDR;
	/** Instance Id of NOR on the SoC */
	Uint16              instId;
	/** device ID*/
	Uint16				deviceID[4];
	/** CFI data*/
	Uint16				cfiData[10];
    /** Width of NOR (8 or 16 bit) */
    CSL_NorWidth       norWidth;
	/** Supported command sets */ 
	Uint16				commandSet;
	/**Length of a block*/
	Uint32				blockLength;
	/**Number of blocks*/
	Uint32				numOfBlocks;
	/**Size of the internal write buffer in bytes*/
	Uint16				writeBufferSize;
	/**Unlock Sequence is enabled or bypassed*/
	Bool				enableUnlockSeq;
	/** Interrupt number of NOR */
    Int                 intrNumNor;
    /** NOR Address cycles for setting page/block address */
    Uint16              addrCycles;
    /** NOR Operating mode */
    CSL_NorOpMode      	norOpMode;
} CSL_NorObj;

/**  \brief Nand object structure pointer */
typedef CSL_NorObj *CSL_NorHandle;

CSL_Status NOR_setup(CSL_NorObj *norObj, Uint16 instId);
CSL_Status NOR_emifResetAndClock(CSL_NorHandle hNor);
CSL_Status NOR_setupEmifChipSelect(CSL_NorHandle hNor, CSL_NorConfig *norCfg);
CSL_Status NOR_readId(CSL_NorHandle hNor);
CSL_Status NOR_readCFI(CSL_NorHandle hNor);
CSL_Status NOR_readBlockLockStatus(CSL_NorHandle hNor, Uint16 *blockLockStatus);
CSL_Status NOR_unlockBlock(CSL_NorHandle hNor, Uint16 blockNum, Uint16 *blockLockStatus);
CSL_Status NOR_readArray(CSL_NorHandle hNor, Uint16 blockNum, Uint16 wordOffset);
CSL_Status NOR_writeWord(CSL_NorHandle hNor, Uint16 blockNum,
							Uint16 wordOffset, Uint16 wordData,
							Uint16 statusMode, Uint16 *writeStatus);
CSL_Status NOR_writeBufferSetup(CSL_NorHandle hNor, Uint16 blockNum, 
								Uint16 wordOffset, Uint16 numOfWords);
CSL_Status NOR_writeBufferComplete(CSL_NorHandle hNor, Uint16 blockNum, 
									Uint16 wordOffset, Uint16 numOfWords,
									Uint16 *writeBuffer, Uint16 statusMode,
									Uint16 *writeStatus);
CSL_Status NOR_eraseBlock(CSL_NorHandle hNor, Uint16 blockNum,
							Uint16 statusMode, Uint16 *eraseStatus);

#ifdef __cplusplus
}
#endif

#endif /*_CSL_NOR_H_*/
