/* --COPYRIGHT--,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/**************************************************************************************************
  @headerfile:    snp_rpc_synchro.h
  Revised:        $Date: 2015-06-22 20:01:12 -0700 (Mon, 22 Jun 2015) $
  Revision:       $Revision: 44190 $

  Description:    This file contains required synchronization and memory
                  functions used by the SNP RPC functions

**************************************************************************************************/

#ifndef SNP_SYNCHRO_H
#define SNP_SYNCHRO_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * HELPER FUNCTIONS
 */

/*********************************************************************
 * FUNCTIONS
 */

// Call before using any synchronization.
extern void SNP_open(void);

// Call when closing connection with SNP
extern void SNP_close(void);

// enter critical section.
extern void SNP_enterCS(void);

// exist critical section.
extern void SNP_exitCS(void);

// Block an application until a response is received.
extern void SNP_waitForResponse(void);

// Unblock application from running.
extern void SNP_responseReceived(void);

// Pass in size get a pointer to dynamically allocated buffer.
extern void *SNP_malloc(Uint16 size);

// Pass in the pointer to a buffer allocated by SNP_malloc to free the buffer.
extern void SNP_free(void* pointer);

/*********************************************************************
*********************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* SNP_SYNCHRO_H */
