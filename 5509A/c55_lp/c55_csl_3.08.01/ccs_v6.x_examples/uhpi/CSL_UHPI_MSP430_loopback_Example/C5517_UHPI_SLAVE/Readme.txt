Example code to verify UHPI slave and MSP430 master host external loopback.

UHPI MSP430 HOST LOOPBACK

TEST DESCRIPTION:
	This test configures C5517 UHPI as slave and MSP430 host as the master. 
The host sends data to the UHPI interface of C5517 and the same is collected 
and looped back to the host by the slave UHPI interface.
Data integrity is confirmed by checking the SARAM locations being accessed by
the host and the read buffer back at the host end.

TEST PROCEDURE AND EXPECTED RESULT:
/* @filename : MSP_Communication
 * 
 * ************************      Run C5517 first       ************************
 *
 * ********************************  C5517  ***********************************
 * STEP 1: Open and Config UHPI peripheral controller
 * STEP 2: Inititalize SARAM with data 0xAAAA 0x5555 from 0x8000 location of SARAM.
 * STEP 3: Wait for DSP_INT(Host to DSP interrupt) to occur
 * STEP 4: If interrupt is received, exit from loop
 * STEP 5: Close UHPI PORT
 * ****************************************************************************
 * ********************************  MSP430 ***********************************
 * STEP 1: Set up the clock and wait till CLOCK is stabilized.
 * STEP 2: Acess HPIA Register in write mode and Set some adrress value.
 * STEP 3: Acess HPIA Register in Read mode.
 * STEP 4: Check if Written and READ values are correct.
 * ============================================================================
 */