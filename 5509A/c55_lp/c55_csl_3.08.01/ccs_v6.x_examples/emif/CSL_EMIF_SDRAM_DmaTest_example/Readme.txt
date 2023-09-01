/* @testname :SDRAM_DmaTest====================================================
 * ============================================================================
 */

This test assumes the func_clk as 100MHz

The test requires

1. DMA and EMIF peripheral interfaces clocked & brought out of reset. 
2. HPI_ON signal should be LOW. For this "HPI_ON in SW4"should towards ON, away from DOT.
3. DMA to be configured to transfer data from one SDRAM location to another.
4. DMA burst-size to be configured to 1-WORD first(32-bit transfer).
5. DMA tx and rx channels started.
6. Transmitted and received data compared to ensure there's no mismatch.
7. Test repeated with 2/4/8/16-WORD burst-sizes.


