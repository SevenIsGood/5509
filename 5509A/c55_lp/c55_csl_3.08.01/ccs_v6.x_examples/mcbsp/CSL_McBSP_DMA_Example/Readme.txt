Example code to verify McBSP transfers using DMA 

MCBSP DMA

TEST DESCRIPTION:
This test is to verify the McBSP external loopback TX-RX transfers using DMA.

TEST PROCEDURE AND EXPECTED RESULT:
Uncomment "#define EVM5517 1" & Comment "#define USE_MCBSP_TARGET 1"
This test configures PLL to be 100MHz

This is an External Loop-back Test
The following connections are required on J14 of EVM5517 revD

1. McBSP_CLKX(pin#12) to  pin 20 on J15 (or) McBSP_CLKR(Test point -TP#15)
2. McBSP_FSX(pin#8) to McBSP_FSR(pin#1)
3. McBSP_DX(pin#22) to McBSP_DR(pin#21)


Brief description of the procedure and the expected result to be checked:
------------------------------------------------------------------------
1. Configure DMA3 Channel0: 1Word Burst, McBSP Tx Dma event, write mode, global tx array addr as src and McBSP TXRL as dst, IO-mem transfer and suitable datalength, dma interrupt enabled.
2. Configure DMA3 Channel1: 1Word Burst, McBSP Rx Dma event, read mode, McBSP RXRL as src and global rx array addr as dst, IO-mem transfer and suitable datalength, dma interrupt enabled.
3. Configure McBSP: 32 Word wordlenth, framelenth 1, single phase, 0-bit data delay, r-justified msb first, clksrc sysclk, frame sync selection, sample rate
4. Start DMA3 channels 0&1
5. Enable and start McBSP Tx, Rx and Frame Sync Generator
6. Wait for Rx and Tx DMA event completion in the DMA interrupt flag register
7. Check Data in rx array for integrity vis-a-vis the tx array