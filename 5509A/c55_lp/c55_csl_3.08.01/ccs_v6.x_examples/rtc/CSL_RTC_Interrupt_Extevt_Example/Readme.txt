Example to exhibit RTC external event interrupt

RTC EXTERNAL EVENT

TEST DESCRIPTION:
	This test configures RTC for an external event after it's 
started. 

TEST PROCEDURE AND EXPECTED RESULT: 
 1. Run the test. RTC would be configured for Ext event and RTC started. 
 2. The test will wait for the external event interrupt.
 3. Move the jumper JP6 (on C5517 EVM)/ JP5 (on C5515 evm) from 1-2 position to 2-3 position. WAKEUP toggles HIGH->LOW. 
 4. Observe if the external event interrupt got generated.
