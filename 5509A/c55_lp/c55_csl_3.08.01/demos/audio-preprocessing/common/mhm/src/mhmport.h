/******************************************************************************
 * FILE PURPOSE: Target specific definitions.
 ******************************************************************************
 * FILE NAME:   MHMPORT.H
 *
 * DESCRIPTION: Contains definitions that are specific to different targets.
 *
 * Copyright (c) 2016, Texas Instruments Incorporated
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
 *
 */

#ifdef ti_targets_C54
#define _MHMPORT_C54 1
#else
#define _MHMPORT_C54 0
#endif

#ifdef ti_targets_C54_far
#define _MHMPORT_C54F 1
#else
#define _MHMPORT_C54F 0
#endif

#ifdef ti_targets_C55
#define _MHMPORT_C55 1
#else
#define _MHMPORT_C55 0
#endif

#ifdef ti_targets_C55_large
#define _MHMPORT_C55L 1
#else
#define _MHMPORT_C55L 0
#endif

#ifdef ti_targets_C64
#define _MHMPORT_C64 1
#else
#define _MHMPORT_C64 0
#endif

#ifdef ti_targets_C64_big_endian
#define _MHMPORT_C64_BIG_ENDIAN 1
#else
#define _MHMPORT_C64_BIG_ENDIAN 0
#endif

#ifdef ti_targets_C64P
#define _MHMPORT_C64P 1
#else
#define _MHMPORT_C64P 0
#endif

#ifdef ti_targets_elf_C64P 
#define _MHMPORT_ELF_C64P 1
#else
#define _MHMPORT_ELF_C64P 0
#endif

#ifdef ti_targets_C64P_big_endian
#define _MHMPORT_C64P_BIG_ENDIAN 1
#else
#define _MHMPORT_C64P_BIG_ENDIAN 0
#endif

#ifdef ti_targets_elf_C64P_big_endian
#define _MHMPORT_ELF_C64P_BIG_ENDIAN 1
#else
#define _MHMPORT_ELF_C64P_BIG_ENDIAN 0
#endif

#ifdef ti_targets_C66
#define _MHMPORT_C66 1
#else
#define _MHMPORT_C66 0
#endif

#ifdef ti_targets_elf_C66
#define _MHMPORT_ELF_C66 1
#else
#define _MHMPORT_ELF_C66 0
#endif

#ifdef ti_targets_C66_big_endian
#define _MHMPORT_C66_BIG_ENDIAN 1
#else
#define _MHMPORT_C66_BIG_ENDIAN 0
#endif

#ifdef ti_targets_elf_C66_big_endian
#define _MHMPORT_ELF_C66_BIG_ENDIAN 1
#else
#define _MHMPORT_ELF_C66_BIG_ENDIAN 0
#endif

#ifdef ti_targets_C674
#define _MHMPORT_C674 1
#else
#define _MHMPORT_C674 0
#endif

#ifdef ti_targets_C674_big_endian
#define _MHMPORT_C674_BIG_ENDIAN 1
#else
#define _MHMPORT_C674_BIG_ENDIAN 0
#endif

#ifdef gnu_targets_arm_GCArmv7A
#define _MHMPORT_ARM_GCARMV7A 1
#else
#define _MHMPORT_ARM_GCARMV7A 0
#endif

#if   (_MHMPORT_C54 || _MHMPORT_C54F)
#include <ti/mas/mhm/src/c54/mhmport.h>
#elif (_MHMPORT_C55 || _MHMPORT_C55L)
#include <ti/mas/mhm/src/c55/mhmport.h>
#elif _MHMPORT_ARM_GCARMV7A
#include <ti/mas/mhm/src/arm/mhmport.h>
#elif (_MHMPORT_C64       || _MHMPORT_C64_BIG_ENDIAN      || \
       _MHMPORT_C64P      || _MHMPORT_C64P_BIG_ENDIAN     || \
       _MHMPORT_ELF_C64P  || _MHMPORT_ELF_C64P_BIG_ENDIAN || \
       _MHMPORT_C66       || _MHMPORT_C66_BIG_ENDIAN      || \
       _MHMPORT_ELF_C66   || _MHMPORT_ELF_C66_BIG_ENDIAN  || \
       _MHMPORT_C674      || _MHMPORT_C674_BIG_ENDIAN)
#include <ti/mas/mhm/src/c64/mhmport.h>
#else
#error invalid target
#endif

#define MHM_ALIGN_BITS    TYP_STRUCT_LOG2ALIGN
#define MHM_ALIGN_MASK    (~((~0U)<<MHM_ALIGN_BITS))   

/* Align gap is the maximum number of words that may be used to fill a gap
 *  to the next memory block due to alignment requirements. */
#define MHM_MAX_ALIGNGAP  MHM_ALIGN_MASK

/* The maximum value of a buffer size and the maximum buffer size. They are
 *  not the same due to alignment gap. */
#define MHM_MAX_SIZEVAL   (~0U)
#define MHM_MAX_SIZE      (MHM_MAX_SIZEVAL - MHM_MAX_ALIGNGAP)

/* Nothing past this point */
