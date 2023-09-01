
1. Enable clocks to Analog Domain registers and SAR ADC.
2. Enable GPO functionality in SARGPOCTRL register.
3. Set breakpoints at lines 66, 71, 76, 80.
(i.e. at points where you drive the GPO pins low and high)
NOTE: Don't drive GPO_0 high. 
It cant can't be driven high by design. Can be driven only low.
4. Measure with a multimeter, voltages at pins 2,4,6,8 on JP15 exposed on-board.
Voltage must be equal to VDD_ANA, i.e. 1.3V when driven high and 0V when driven low.