Example code to test out the GPIO functionality of UHPI

UHPI GPIO

TEST DESCRIPTION:
	This test makes use of the GPIO functionality of the UHPI. The UHPI lines are
enabled as GPIO using GPIO_ENL register, direction set as output or input depending 
upon if the test is being used for testing the output gpio functionality or the 
input using DIR1L and DIR2L and driven using DAT1L and DAT2L.

TEST PROCEDURE and EXPECTED RESULT:
/* @filename : UHPI_GPIO
 * **
 *
 * **********************************************************************************
 * NOTE: HPI_ON switch of SW4 should be in OFF position (HPI_ON signal should be high) 
 *       HPI_SEL_IF switch should be in ON position (HPI_SEL_IF signal should be low) 
 *
 * **********************************************************************************     
 *
 * Note: ----------------  Testing GPIO OUTPUT Functionality ------------------------
 *
 * The Address/Data Lines direction between MSP430 to Phoenix is controlled by HR/W pin in u64 
 * ************************      Run C5517 first       ************************
 *
 * ********************************  C5517  ***********************************
 * STEP 1: Open and Config UHPI peripheral controller
 * STEP 2: Set all pins in UHPI controller to GPIO mode using UHPI GPIO_EN Register
 * STEP 3: Set all pins to output and logic HIGH.
 * STEP 4: Set pins one bye one from HIGH to LOW
 * STEP 5: Close UHPI PORT
 * ****************************************************************************
 * ********************************  MSP430 ***********************************
 * STEP 1: Set up the clock and wait till CLOCK is stabilized.
 * STEP 2: Set all pins to input.
 * STEP 3: Check if Pin state is changed from HIGH to LOW in any of the pins.
 * ============================================================================
 *
 * ===============================  Validating ===================================
 * STEP 1: Load C5517 and put breakpoints on line 96,line 126.
 * STEP 2: Uncomment C5517_UHPI_OUTPUT_TEST and comment C5517_UHPI_INPUT_TEST macro in MSP430 code, HPI_Host.c, C5517 code.
 * STEP 3: Build and load MSP430 code and put breakpoint on line 113.
 * STEP 4: Run code in C5517 till line 96, run code in MSP till line 113 and check if Status_Buffer values 
 *         are 0xFF,0xFF,0x33 which indicates all lines are logic HIGH. 
 * STEP 5: Run code in C5517 till line 126, run code again in MSP till line 113 and check if Status_Buffer values 
 *         are 0x00,0x00,0x00 which indicates all lines are logic LOW. 
 *
 * ============================================================================= 
 *
 * Note: ----------------  Testing GPIO INPUT Functionality ------------------------
 *
 * The Address/Data Lines direction between MSP430 to Phoenix is controlled by HR/W pin in u64 
 * ************************      Run C5517 first       ************************
 *
 * ********************************  C5517  ***********************************
 * STEP 1: Open and Config UHPI peripheral controller
 * STEP 2: Set all pins in UHPI controller to GPIO mode using UHPI GPIO_EN Register
 * STEP 3: Set all pins to inout.
 * STEP 4: Read all pins status and display it.
 * STEP 5: Close UHPI PORT
 * ****************************************************************************
 * ********************************  MSP430 ***********************************
 * STEP 1: Set up the clock and wait till CLOCK is stabilized.
 * STEP 2: Set all pins to output.
 * STEP 3: Set pins state to HIGH snd change from HIGH to LOW.
 * ============================================================================
 *
 * ===============================  Validating ===================================
 * STEP 1: Load C5517 and run code
 * STEP 2: Uncomment C5517_UHPI_INPUT_TEST and comment C5517_UHPI_OUTPUT_TEST macro in MSP430 code, HPI_Host.c and C5517 code.
 * STEP 3: Build and load MSP430 code and put breakpoints on lines 85,89 and 94.
 * STEP 4: Run code in MSP till line 118 and check value displayed in output console.
 * STEP 5: Run code in MSP till line 122 and check value displayed in output console.
 * STEP 6: Run code in MSP till line 127 and check value displayed in output console.
 * STEP 7: The output pin state of MSP pins should reflect in displayed values.        
 *
 */
