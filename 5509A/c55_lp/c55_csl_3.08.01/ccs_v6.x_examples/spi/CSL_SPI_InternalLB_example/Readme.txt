
1. Enable clocks to SPI and configure PPMODE in EBSR for SPI.
2. Configure the SPI interface: clk, wordlength, framelength, chip sel, 
   data delay, clk and chip sel polarities.
3. Enable Loopback in SPIDCRU.
4. Fill in CPU Write data buffer with alternate 0x11's and 0xAB's and 
   Read Data buffer with alternate 0x00's and 0xCD's.
4. Write Data into SPIDRU and collect it from SPIDRL.
5. Compare the write and read data. There shouldn't be any data mismatch.
   Read Data Buffer should now have alternate 0x11's and 0xAB's. 