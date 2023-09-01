Tx: I2S0		Rx: I2S2     
STEP1: Enable clock, get reg space pointer handlers, and set EBSR for the corresp. I2S channels
STEP2: Setup channel configuration: mono, fs and clk pol, datadelay: 1bit, packing, sign extension, fs/clk div: 16/32, dsp data format, master for Tx/slave for Rx
 STEP3: Write into the Tx channel I2S0
 STEP4: Read from the Rx channel I2S2
 STEP5: Verify the Buffer data matching to ensure data integrity
 STEP6: Close the channel handlers
HPI_ON in switch DIP Switch bank SW4 is shifted right, away from the DOT, towards ON.
Connect pins on J14
1.	 I2S0_CLK(pin#12) to I2S2_CLK(pin#5) 
2.	 I2S0_FS(pin#8)   to I2S2_FS(pin#9) 
3.	 I2S0_RX(pin#21)  to I2S2_DX(pin#19) 
4.	 I2S0_DX(pin#22)  to I2S2_RX(pin#14)

On C5515,
Connect pins on J13
1.	 I2S0_CLK(pin#12) to I2S2_CLK(pin#3) 
2.	 I2S0_FS(pin#8)   to I2S2_FS(pin#7) 
3.	 I2S0_RX(pin#21)  to I2S2_DX(pin#11) 
4.	 I2S0_DX(pin#22)  to I2S2_RX(pin#13)
