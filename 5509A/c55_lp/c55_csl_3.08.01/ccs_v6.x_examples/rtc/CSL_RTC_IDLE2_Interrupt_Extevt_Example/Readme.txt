Example to exhibit IDLE 2 state and recovery using RTC external wakeup interrupt

IDLE2 RECOVERY USING RTC EXTERNAL WAKEUP EVENT

TEST DESCRIPTION:
	This test configures RTC for generating a wakeup interrupt based on an 
external event after it's started, during which period the system is put into 
IDLE2 state. The subsequent RTC external interrupt should bring the CPU out of the
IDLE2 state and resume execution from where it was before it idled out.

TEST PROCEDURE AND EXPECTED RESULT: 

 1. Ensure JP10 CLKSEL is shorted so that usb osc is not used. 
 2. Run the test.
 3. It will stop once usb clk is shut off.
 4. Pause and Resume the test.
 5. The test will execute the idle instsrn that will idle the cpu and wait.
 6. Move the jumper JP6 (on C5517 EVM)/ JP5 (on C5515 evm) from 1-2 position to 2-3 position. WAKEUP toggles HIGH->LOW. 
 7. Observe if the external event interrupt got the CPU out of idle state.
