AudioCodec_DMA

On C5517 EVM, for codec-1 (U19):
0. Uncomment #define INSTANCE0_I2S and Comment #define INSTANCE2_I2S
   in codec_aic3254.h
	- to use aic3204-1 (U19 on-board) on C5517 EVM
	- Use port P8
	- Stereo-In - Blue jack, HP-Out - Lime-green jack				
1. Clear SEL_I2C_S0 and SEL_I2C_S1 (IO Expander lines P02,P03) to direct I2C data and scl lines to codec-1.
2. Set AIC_MCBSP_MODE (IO Expander line P01) to choose AIC mode.
3. Set SEL_MMC0_I2S to choose I2S mode. (IO Expander line P10).
4. Send a reset to AIC0 by clearing AIC_RST (IO Expander line P00).
5. Enable Clk to DMA and I2S0 and bring them out of reset.
6. Configure I2S0 to be slave, l-justified stereo, loopback-datapack-sign extension disabled, 
   wordlength 16-bit, Tx on rising edge, L-channel txn when FS low.
7. Configure DMA channels 0,1,2,3 for L-R I2S audio tx and rx, 1 32-bit burst word, interrupts
   enabled, pingpong disabled, auto-reload enabled.
8. Configure the Audio codec to collect line in over IN2_L and IN2_R, send it over to C5517 over I2S0 and on the way back, collect data from 
   C5517 over I2S0, finally send it out of the HPL and HPR.
9. Start the Rx-dma, and wait for two Rx-DMA transaction complete interrupts.
10. The received audio data is stored in i2sDmaReadBufLeft and i2sDmaReadBufRight.
11. Transfer this collected data into i2sDmaWriteBufLeft and i2sDmaWriteBufRight.
12. Start the Tx-dma channels that will transfer this received data out of C5517 to the headphone.
13. The process is repeated in a while(1).

On C5517 EVM, for codec-2 (U72):
1. Uncomment #define INSTANCE2_I2S and Comment #define INSTANCE0_I2S 
   in codec_aic3254.h  
	- to use aic3204-2 (U72 on-board) on C5517 EVM
	- Use port P9, 
	- Stereo-In - Blue jack, HP-Out - Lime-green jack			
2. Set SEL_I2C_S0 and Clear SEL_I2C_S1 (IO Expander lines P02,P03) to direct I2C data and scl lines to codec-2.
3. Set SPI_I2S2_S0 and SPI_I2S2_S1 (IO Expander lines P05,P06) to direct choose I2S2 over SPI.
4. Send a reset to AIC0 by clearing AIC_RST (IO Expander line P00).
5. Enable Clk to DMA and I2S2 and bring them out of reset.
6. Configure I2S2 to be slave, l-justified stereo, loopback-datapack-sign extension disabled, 
   wordlength 16-bit, Tx on rising edge, L-channel txn when FS low.
7. Configure DMA channels 4,5,6,7 for L-R I2S audio tx and rx, 1 32-bit burst word, interrupts
   enabled, pingpong disabled, auto-reload enabled.
8. Configure the Audio codec to collect line in over IN2_L and IN2_R, send it over to C5517 over I2S0 and on the way back, collect data from 
   C5517 over I2S2, finally send it out of the HPL and HPR.
9. Start the Rx-dma, and wait for two Rx-DMA transaction complete interrupts.
10. The received audio data is stored in i2sDmaReadBufLeft and i2sDmaReadBufRight.
11. Transfer this collected data into i2sDmaWriteBufLeft and i2sDmaWriteBufRight.
12. Start the Tx-dma channels that will transfer this received data out of C5517 to the headphone.
13. The process is repeated in a while(1).

On C5515 EVM:
1. Uncomment #define INSTANCE2_I2S and Comment #define INSTANCE0_I2S
   in codec_aic3254.h
	- Stereo-In - J9, HP-Out - J4
2. Set up interrupt vector table
   Disable and clear interrupts
3. Reset all peripherals
4. Set PLL output to desired MHz
   Disable USB LDO
5. Enable Clk to DMA and I2S2 and bring them out of reset.
6. Configure I2S2 to be slave, l-justified stereo, loopback-datapack-sign extension disabled, 
   wordlength 16-bit, Tx on rising edge, L-channel txn when FS low.
7. Configure DMA channels 4,5,6,7 for L-R I2S audio tx and rx, 1 32-bit burst word, interrupts
   enabled, pingpong disabled, auto-reload enabled.
8. Configure the Audio codec to collect line in over IN2_L and IN2_R, send it over to C5517 over I2S0 and on the way back, collect data from 
   C5517 over I2S2, finally send it out of the HPL and HPR.
9. Start the Rx-dma, and wait for two Rx-DMA transaction complete interrupts.
10. The received audio data is stored in i2sDmaReadBufLeft and i2sDmaReadBufRight.
11. Transfer this collected data into i2sDmaWriteBufLeft and i2sDmaWriteBufRight.
12. Start the Tx-dma channels that will transfer this received data out of C5517 to the headphone.
13. The process is repeated in a while(1).
