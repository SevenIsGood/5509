********************************************************************* 
*    Property of Texas Instruments Incorporated, Copyright 2007
*    All rights reserved
*********************************************************************

*********************************************************************
* This file includes the API Call Vector table and the API Look Up
* Table which are combined to implement a relocate-able call table
* for the secure bootloader security subroutines and Media drivers.
* This relocateable table allows user applications to call these
* routines out of ROM or out of RAM by relocating the API Vector
* Table to RAM and replacing any desired functions with new ones
* while still being able to use ROM functions.  The initial API
* Vector Table contained in this file is the ROM version which will
* be used as the default table until or if a user application moves
* the table to another location in ROM or more likely RAM.
* 
* SARAM Memory (0x27FFE..0x27FFF) is reserved to point to the
* API Vector Table.  This is a 32-bit value (24-bit address).
*********************************************************************

*********************************************************************
*                                                                   *
* _:base:   ->   r:base:    ->   _:target:                          *
*                                                                   *
* Example:                                                          *
*                                                                   *
* _GetDieID ->   rGetDieID  ->   _vGetDieID                         *
* ^              ^               ^                                  *
* |              |               |                                  *
* |              |               +--- Defined in utils.s55          *
* |              |                                                  *
* |              +-- Defined in api_vec.s55                         *
* |                                                                 *
* +------- Defined in api_lut.s55                                   *
*                                                                   *
*********************************************************************

        ; General functions
        VecMacro GetROMVersion                        , vGetROMVersion
        VecMacro GetDieID                             , vGetDieID
        VecMacro SetAPIVectorAddress                  , vSetAPIVectorAddress
        VecMacro secBoot                              , vsecBoot
        
        ; MCSPI functions (used by bootloader)
        VecMacro MCSPI_RESET                            , vMCSPI_RESET
        VecMacro MCSPI_INIT                             , vMCSPI_INIT
        VecMacro MCSPI_RDSR                             , mcspi_eeprom_rdsr
