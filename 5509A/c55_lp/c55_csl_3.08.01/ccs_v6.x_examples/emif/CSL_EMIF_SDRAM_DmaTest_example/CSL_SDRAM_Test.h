/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2008
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *  ============================================================================
 */

/** @file CSL_SDRAM_Test.h
 *
 *  @brief SDRAM test header file
 *
 *  Path: 
 */

/* ============================================================================
 * Revision History
 * ================
 * 17-Dec-2010 Created
 * ============================================================================
 */

#ifndef _CSL_SDRAM_TEST_H_
#define _CSL_SDRAM_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CSL_SDRAM_DATA_TEST_VALUE1 0x5555
#define CSL_SDRAM_DATA_TEST_VALUE2 0xAAAA
#define CSL_SDRAM_DATA_TEST_VALUE3 0x0000
#define CSL_SDRAM_DATA_TEST_VALUE4 0xFFFF
#define CSL_SDRAM_DATA_TEST_VALUE5 0x55555555 
#define CSL_SDRAM_DATA_TEST_VALUE6 0xAAAAAAAA
#define CSL_SDRAM_DATA_TEST_VALUE7 0x00000000
#define CSL_SDRAM_DATA_TEST_VALUE8 0xFFFFFFFF
/*7.6875MB is available at CS. Device has 8MB*/
#define CSL_SDRAM_TOTAL_LEN_IN_BYTE 0x7B0000
/*This value is for CS. Device is 4M x 16-bit*/
#define CSL_SDRAM_TOTAL_LEN_IN_WORD 0x3D8000
/*This value is for CS. Device can be considered as a 2M x 32-bit*/
#define CSL_SDRAM_TOTAL_LEN_IN_DWORD 0x1EC000
#define CSL_SDRAM_TEST_BUFFER_WORD_COUNT 256
#define CSL_SDRAM_USE_ALIGNED_BUFFER TRUE
#define CSL_SDRAM_BUFFER_ALIGN_BOUNDARY 32
#define CSL_SDRAM_TEST_START_OFFSET 0
#define CSL_SDRAM_DATA_WORD_ADDR 0x28000
#define CSL_SDRAM_DMA3_BYTE_ADDR 0x1000000
#define CSL_SDRAM_DMA_CHAN CSL_DMA_CHAN15

#ifdef __cplusplus
}
#endif

#endif /*_CSL_SDRAM_TEST_H_*/

