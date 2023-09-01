/********************************************************************* 
*    Property of Texas Instruments Incorporated, Copyright 2008
*    All rights reserved
**********************************************************************
*    secboot.h
**********************************************************************/
#ifndef SECBOOT_H
#define SECBOOT_H

#include "machine.h"

#define SECBOOT_FLAGS_USE_ID    0x0001
#define SECBOOT_FLAGS_REAUTHOR  0x0002
#define SECBOOT_FLAGS_INSECURE  0x0010

#define IOPORT_REGISTER(reg)     (*(ioport volatile unsigned short *)(reg))
#define IOPORT_REGISTER_PTR(reg)	((ioport volatile unsigned short *)(reg))

#define IOPORT_REG_PTR_SECUREROM_ADDR   (IOPORT_REGISTER_PTR(0x2400))
#define IOPORT_REG_SECUREROM_CNTL      (IOPORT_REGISTER(0x1C25))
#define IOPORT_REG_TEST_LOCK           (IOPORT_REGISTER(0x1C55))
#define IOPORT_REG_SET_REF_TRIM        (IOPORT_REGISTER(0x7003))
#define TEST_LOCK_EFUSE_CTRL_CHNG_KEY   0x569A
#define TEST_LOCK_SET_REF_TRIM_CHNG_KEY 0xC001
#define TEST_LOCK_RELOCK                0x0000

typedef void (*fpDone)();
typedef UInt16 (*fpReadNWords)(UInt32 address, UInt16 *buffer, UInt16 count);
typedef UInt16 (*fpWriteNWords)(UInt32 address, UInt16 *buffer, UInt16 count);

UInt16 memory_write_n_words(UInt32 address, UInt16 *buffer, UInt16 count);

void sec_boot_fail();
void branch_to(UInt32 address);

typedef struct
{
    fpReadNWords readNWords;
    fpWriteNWords writeNWords;
    fpDone done;
} BootSourceFunctions_t;

UInt32 secBoot
(
    fpReadNWords read_n_words,
    fpWriteNWords write_n_words,
    UInt32 start_address,
    UInt16 *seed,
    UInt16 flags
);
void SetAPIVectorAddress();
UInt32 GetROMVersion();
void GetDieID(UInt16 *buffer);
void GetSeed(int SeedOffset, UInt16 *Seed);
void EnableLowVoltageDetection(void);

UInt32 vsecBoot
(
    fpReadNWords read_n_words,
    fpWriteNWords write_n_words,
    UInt32 start_address,
    UInt16 *seed,
    UInt16 flags
);
void vSetAPIVectorAddress();
UInt32 vGetROMVersion();
void vGetDieID(UInt16 *buffer);

#endif /* SECBOOT_H */
