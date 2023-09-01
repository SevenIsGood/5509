
CSL_EMIF_NOR_CpuTransfer Example
NOTE:	NOR Flash on-board C5517 EVM is a 128Mbit Spansion flash S29Gl128S11DHIV20.
		
In CSL_NOR_Test.h, comment #define EVM5515 1 and Uncomment #define EVM5517 1 if running on C5517 EVM.
In CSL_NOR_Test.h, comment #define EVM5517 1 and Uncomment #define EVM5515 1 if running on C5515 EVM.
  
1. Configure EMIF, start the clock and bring it out of reset.
2. Configure the NOR flash parameters in EMIF registers such as AWCCR1, AWCCR2, ACS2CR1, ACS2CR2, etc.
3. Read the manufacturer and device IDs.
4. Check if the test-block is blocked for r/w or not.
5. Erase the block and prepare it for r/w.
6. Write a block of data to the selected block in the NOR Flash.
7. Read a block of data from the test block.
8. Perform a comparison of read and written data. There should be no mismatch.
 