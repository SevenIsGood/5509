Example code to verify the IDLE functionality of McBSP module

MCBSP IDLE


TEST DESCRIPTION:
This test verifies McBSP TX-RX external loopback in McBSP-IDLE_EN bit set in PCRL


TEST PROCEDURE AND EXPECTED RESULT:
This test configures PLL to be 100MHz

This is an External Loop-back Test
The following connections are required on J14 of EVM5517

1. McBSP_CLKX(pin#12) to McBSP_CLKR(TP#15)
2. McBSP_FSX(pin#8) to McBSP_FSR(pin#1)
3. McBSP_DX(pin#22) to McBSP_DR(pin#21)

Brief description of the procedure and the expected result to be checked:
------------------------------------------------------------------------
1. Configure McBSP: 32 Word wordlenth, framelenth 1, single phase, 0-bit 
   data delay, r-justified msb first, clksrc sysclk, frame sync selection,
   sample rate.
2. Enable and start McBSP Tx, Rx and Frame Sync Generator.
3. Set IDLE_EN in PCRL.
4. Wait for XRDY bit in SPCRU to be set.
5. Send data in xmt[] out of McBSP Tx via DXRU and DXRL.
6. Wait for RRDY bit in SPCRL to be set.
7. Collect the data rfom RXRU and RXRL into rcv[].
8. Check Data in rcv array for integrity vis-a-vis the xmt array.
