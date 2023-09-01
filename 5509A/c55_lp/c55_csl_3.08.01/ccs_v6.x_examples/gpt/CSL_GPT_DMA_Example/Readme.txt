Example code to perform read from DARAM and write to SARAM using DMA and sync 
the same to GPT events

GPT DMA

TEST DESCRIPTION:

	This test configures all 3 GPT's (0,1, and 2) and synchronizes a DMA 
transfer of 8 contiguous 16-bit data words from DARAM location 0x400 to SARAM
location 0x20000, with the three GPT events.

TEST PROCEDURE and EXPECTED RESULT:
(Put breakpoints at all for() loop locations in the code. There are only 3 
 such locations.)

1. Configure DMA0 channels 0,1,2 for DMA events w.r.t TIMER0,1,2: 4Word Burst,
   Write mode, 16byte datalength, Mem-mem transfer, SRC addr in DARAM and Dest
   Addr in SARAM.
2. Configure TIMER0,1,2: Auto mode disabled, prescale i/p clk div-by-4.
3. Write 8 contiguous 16-byte locations @ SRC addr in DARAM to be 0xFFFF.
4. Start DMA0 channel0.
5. Start TIMER0.
6. Wait for DMA event w.r.t Timer0 count expiry.
7. Stop DMA0 Channel0.
8. Check if 8 contiguous 16-bit locations @ DST addr in SARAM have been 
   written with 0xFFFF. (In the CCS Memory Browser Window)
9. Write 8 contiguous 16-byte locations @ SRC addr in DARAM to be 0xAAAA.
10. Start DMA0 channel1.
11. Start TIMER1.
12. Wait for DMA event w.r.t Timer1 count expiry.
13. Stop DMA0 Channel1.
14. Check if 8 contiguous 16-bit locations @ DST addr in SARAM have been 
    written with 0xFFFF. (In the CCS Memory Browser Window)
15. Write 8 contiguous 16-byte locations @ SRC addr in DARAM to be 0xBBBB.
16. Start DMA0 channel2.
17. Start TIMER2.
18. Wait for DMA event w.r.t Timer2 count expiry.
19. Stop DMA0 Channel2.
20. Check if 8 contiguous 16-bit locations @ DST addr in SARAM have been 
    written with 0xFFFF. (In the CCS Memory Browser Window)
