Example code to verify the McBSP in interrupt mode.

MCBSP INTERRUPT

TEST DESCRIPTION:
This test verifies TX-RX McBSP external loopback transfers using
MCBSP TX and RX interrupts.


TEST PROCEDURE AND EXPECTED RESULT:
This test configures PLL to be 100MHz

This is an External Loop-back Test
The following connections are required on J14 of EVM5517

1. McBSP_CLKX(pin#12) to McBSP_CLKR(TP#15) or pin 20 on J15
2. McBSP_FSX(pin#8) to McBSP_FSR(pin#1)
3. McBSP_DX(pin#22) to McBSP_DR(pin#21)


Brief description of the procedure and the expected result to be checked:
------------------------------------------------------------------------

0. Configure and enable interrupts PROG0 and PROG1 doe McBSP Tx and Rx ISR's.
1. Configure McBSP: 32 Word wordlenth, framelenth 1, single phase, 0-bit data delay, r-justified msb first, clksrc sysclk, frame sync selection, sample rate.
2. Enable and start McBSP Tx, Rx and Frame Sync Generator.
3. Wait for McBSP Tx interrupt.
4. Send data in xmt[] out of McBSP Tx via DXRU and DXRL.
5. Wait McBSP Rx interrupt.
6. Collect the data rfom RXRU and RXRL into rcv[].
7. Check Data in rcv array for integrity vis-a-vis the xmt array.
