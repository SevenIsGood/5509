CSL_EMIF_NOR_DmaTransfer

NOTE:	Do power-cycle the board each time you run the test
		NOR Flash on-board C5517 EVM is a 128Mbit Spansion flash S29Gl128S11DHIV20.

In CSL_NOR_Test.h, comment #define EVM5515 1 and Uncomment #define EVM5517 1 if running on C5517 EVM.
In CSL_NOR_Test.h, comment #define EVM5517 1 and Uncomment #define EVM5515 1 if running on C5515 EVM.
		
 1. Configure EMIF, start the clock and bring it out of reset.
 2. Configure the NOR flash parameters in EMIF registers such as AWCCR1, AWCCR2, ACS2CR1, ACS2CR2, etc.
 3. Read the manufacturer and device IDs.
 4. Check if the test-block is blocked for r/w or not.
 5. Erase the block and prepare it for r/w.
 6. Configure the DMA channel 13 to write a "Write-Buffer" size number of words to the NOR flash in the test-block.
 7. Send Write-Buffer command followed by number of words to the test-block address in the NOR Flash.
 8. Start the DMA so that it transfers the Write-Buffer size number of words to the flash.
 9. Issue a reset to change to read array mode.
10. Configure the DMA channel 13 again for a reading the Write-Buffer size number of words.
11. Start the DMA so that it reads Write-Buffer size of data into a local buffer.
12. Perform a comparison of read and written data. There should be no mismatch.
 