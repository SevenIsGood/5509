/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2008, 2016
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *  ============================================================================
 */

/** @file CSL_NOR_Test.h
 *
 *  @brief NOR test header file
 *
 *  Path:
 */

/* ============================================================================
 * Revision History
 * ================
 * 03-Nov-2010 Created
 * 10-Mar-2016 Updates to Header
 * ============================================================================
 */

#ifndef _CSL_NOR_TEST_H_
#define _CSL_NOR_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif

//#include <csl_nor.h>

#if  (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5535) || defined(CHIP_C5545))
#define EVM5515 1
#else
#define EVM5517 1
#endif

/*Conf definitions for the test*/
#define CSL_NOR_DATA_TEST_VALUE1 0x5555
#define CSL_NOR_DATA_TEST_VALUE2 0xAAAA
#define CSL_NOR_DATA_TEST_VALUE3 0x0000
#define CSL_NOR_TEST_BUFFER_WORD_COUNT 256
#define CSL_NOR_INT_WR_BUFFER_WORD_SIZE 10
#define CSL_NOR_TEST_BLOCK_NUM 7
#define CSL_NOR_TEST_WORD_OFFSET 0
#define CSL_NOR_USE_ALIGNED_BUFFER TRUE
#define CSL_NOR_BUFFER_ALIGN_BOUNDARY 512//32
#define CSL_NOR_BLOCKLOCK_STATUS_ARRAY 10
#define CSL_NOR_DMA_CHAN CSL_DMA_CHAN13

/*Definitions for PC28F128P30T85 NOR*/
#define PC28F128P30T85_MAX_ERASE_BLOCKS 130
#define PC28F128P30T85_BYTES_PER_BLOCK 0x20000 /* 128KB */
#define PC28F128P30T85_WORDS_PER_BLOCK (PC28F128P30T85_BYTES_PER_BLOCK/2)
#define PC28F128P30T85_WORDS_PER_WRITE_BUFFER 32

#define PC28F128P30T85_ASYNCWAITCFG_DEFAULT {\
		CSL_NOR_WP_LOW, \
		CSL_NOR_WAIT0, \
		CSL_NOR_ASYNCWAITCFG_WAITCYCLE_DEFAULT}

#define PC28F128P30T85_ASYNCCFG_DEFAULT {\
		CSL_NOR_ASYNCCFG_SELECTSTROBE_DEFAULT, \
		CSL_NOR_ASYNCCFG_WEMODE_DEFAULT, \
		CSL_NOR_ASYNCCFG_WSETUP_DEFAULT, \
		CSL_NOR_ASYNCCFG_WSTROBE_DEFAULT, \
		CSL_NOR_ASYNCCFG_WHOLD_DEFAULT, \
		CSL_NOR_ASYNCCFG_RSETUP_DEFAULT, \
		CSL_NOR_ASYNCCFG_RSTROBE_DEFAULT, \
		CSL_NOR_ASYNCCFG_RHOLD_DEFAULT, \
		CSL_NOR_ASYNCCFG_TAROUND_DEFAULT, \
		0x1}

#define PC28F128P30T85_NORCFG_POLLED {\
		CSL_NOR_16_BIT, \
		CSL_NOR_OPMODE_POLLED, \
		CSL_NOR_CS2, \
		CSL_NOR_INTEL_STD_CMDSET, \
		PC28F128P30T85_BYTES_PER_BLOCK, \
		PC28F128P30T85_MAX_ERASE_BLOCKS, \
		PC28F128P30T85_WORDS_PER_WRITE_BUFFER, \
		NULL, \
		NULL }

#define PC28F128P30T85_NORCFG_DMA {\
		CSL_NOR_16_BIT, \
		CSL_NOR_OPMODE_DMA, \
		CSL_NOR_CS2, \
		CSL_NOR_INTEL_STD_CMDSET, \
		PC28F128P30T85_BYTES_PER_BLOCK, \
		PC28F128P30T85_MAX_ERASE_BLOCKS, \
		PC28F128P30T85_WORDS_PER_WRITE_BUFFER, \
		NULL, \
		NULL }

/*Definitions for S29GL128S11DHIV20 NOR*/
#define S29GL128S11DHIV20_MAX_ERASE_BLOCKS 130 /*Test value for testing*/
#define S29GL128S11DHIV20_BYTES_PER_BLOCK 0x20000 /* 128KB */
#define S29GL128S11DHIV20_WORDS_PER_BLOCK (S29GL128S11DHIV20_BYTES_PER_BLOCK/2)
//#define S29GL128S11DHIV20_WORDS_PER_WRITE_BUFFER 16
#define S29GL128S11DHIV20_WORDS_PER_WRITE_BUFFER 256

#define S29GL128S11DHIV20_ASYNCWAITCFG_DEFAULT {\
		CSL_NOR_WP_LOW, \
		CSL_NOR_WAIT2, \
		CSL_NOR_ASYNCWAITCFG_WAITCYCLE_DEFAULT}

#define S29GL128S11DHIV20_ASYNCCFG_DEFAULT {\
		CSL_NOR_ASYNCCFG_SELECTSTROBE_DEFAULT, \
		CSL_NOR_ASYNCCFG_WEMODE_DEFAULT, \
		CSL_NOR_ASYNCCFG_WSETUP_DEFAULT, \
		CSL_NOR_ASYNCCFG_WSTROBE_DEFAULT, \
		CSL_NOR_ASYNCCFG_WHOLD_DEFAULT, \
		CSL_NOR_ASYNCCFG_RSETUP_DEFAULT, \
		CSL_NOR_ASYNCCFG_RSTROBE_DEFAULT, \
		CSL_NOR_ASYNCCFG_RHOLD_DEFAULT, \
		CSL_NOR_ASYNCCFG_TAROUND_DEFAULT, \
		0x1}

#define S29GL128S11DHIV20_NORCFG_POLLED {\
		CSL_NOR_16_BIT, \
		CSL_NOR_OPMODE_POLLED, \
		CSL_NOR_CS2, \
		CSL_NOR_AMD_STD_CMDSET, \
		S29GL128S11DHIV20_BYTES_PER_BLOCK, \
		S29GL128S11DHIV20_MAX_ERASE_BLOCKS, \
		S29GL128S11DHIV20_WORDS_PER_WRITE_BUFFER, \
		NULL, \
		NULL }

#define S29GL128S11DHIV20_NORCFG_DMA {\
		CSL_NOR_16_BIT, \
		CSL_NOR_OPMODE_DMA, \
		CSL_NOR_CS2, \
		CSL_NOR_AMD_STD_CMDSET, \
		S29GL128S11DHIV20_BYTES_PER_BLOCK, \
		S29GL128S11DHIV20_MAX_ERASE_BLOCKS, \
		S29GL128S11DHIV20_WORDS_PER_WRITE_BUFFER, \
		NULL, \
		NULL }


#ifdef __cplusplus
}
#endif

#endif /*_CSL_NOR_TEST_H_*/
