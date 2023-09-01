;****************************************************************************************
;
;    File Name   : lpva200.h
;    Author      : Pedro Gelabert
;
;    Version     : 0.1
;    Date        : 23 June 2004
;    Update      : Initial Version
;
;    Version     : 0.2
;    Date        : 26 Oct. 2010
;    Update      : Modified for Phoenix 1.0

;    Description : This file defines LPVA200 memory mapped registers
;
;****************************************************************************************

; *******************************************************************************************
; *******************************************************************************************
; *****  SYSREG Registers:               (IO Space:  0x1C00 - 0x1CFF)
; *******************************************************************************************
; *******************************************************************************************
PERIPHSEL0              .set    0x1C00
    ; PERIPHSEL0   Address:  0x1C00 (IO Space)           PERIPHSEL0 (Peripheral Pin Mapping Register 0)
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |rw:rw:rw:rw|rw:rw:rw:rw|rw:rw:rw:rw|rw:rw:rw:rw|  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 0: 0: 0: 1| 0: 0: 0: 0| 0: 0: 1: 1| 1: 1: 1: 1|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  SEL_GPIO21_nA15
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  SEL_GPIO22_nA16
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  SEL_GPIO23_nA17
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  SEL_GPIO24_nA18
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  SEL_GPIO25_nA19
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  SEL_GPIO26_nA20
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  reserved
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  reserved
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  SEL_S0[0]
    ;   |  |  |  |  |  |  +----------------------------- [9]  SEL_S0[1]
    ;   |  |  |  |  |  +-------------------------------- [10] SEL_S1[0]
    ;   |  |  |  |  +----------------------------------- [11] SEL_S1[1]
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] SEL_P[0]
    ;   |  |  +----------------------------------------- [13] SEL_P[1]
    ;   |  +-------------------------------------------- [14] SEL_P[2]
    ;   +----------------------------------------------- [15] MCBSP_CLK_SEL

    ; PERIPHSEL1   Address:  0x1C01 (IO Space)           PERIPHSEL1 (Peripheral Pin Mapping Register 1)
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |r :r :r :r |r :r :r :r |r :r :r :r |r :r :r :r |  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  reserved
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  reserved
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  reserved
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  reserved
    ;   |  |  |  |  |  |  +----------------------------- [9]  reserved
    ;   |  |  |  |  |  +-------------------------------- [10] reserved
    ;   |  |  |  |  +----------------------------------- [11] reserved
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] reserved
    ;   |  |  +----------------------------------------- [13] reserved
    ;   |  +-------------------------------------------- [14] reserved
    ;   +----------------------------------------------- [15] reserved
;----------------------------------------------------------------------------------------
;        Peripheral (External) Bus Selection Bit Fields
;----------------------------------------------------------------------------------------
A15_GPIO           .set    1b<<0        ; Enable Address pin 15 as GPIO 
A16_GPIO           .set    1b<<1        ; Enable Address pin 16 as GPIO 
A17_GPIO           .set    1b<<2        ; Enable Address pin 17 as GPIO 
A18_GPIO           .set    1b<<3        ; Enable Address pin 18 as GPIO 
A19_GPIO           .set    1b<<4        ; Enable Address pin 19 as GPIO 
A20_GPIO           .set    1b<<5        ; Enable Address pin 20 as GPIO 
S0_MMCSD0          .set    00b<<8       ; MMC/SD0 module routed to Serial Port 0 pins
S0_I2S0            .set    01b<<8       ; I2S0 routed to Serial Port 0 pins
S0_GPIO            .set    10b<<8       ; Serial Port 0 pins configured as GPIO
S1_MMCSD1          .set    00b<<10      ; MMC/SD1 module routed to Serial Port 1 pins
S1_MCSPI           .set    01b<<10      ; MCSPI routed to Serial Port 1 pins
S1_GPIO            .set    10b<<10      ; Serial Port 1 pins configured as GPIO

PP_UHPI             .set    000b<<12     ; 21 Parallel port pins configured as 16-bit LCD
PP_GPIO_SPI0_UART_I2S2 .set 001b<<12     ; 6:GPIO + 7:SPI0(CS-0,1,2,3) + 4:UART + 4:I2S2
PP_UHPI_GPI0        .set    010b<<12     ; 10:8-bit UHPI + 8:GPIO
PP_UHPI_SPI0_I2S3    .set    011b<<12     ; 10:8-bit UHPI + 4:SPI0(CS-0) + 4:I2S2
PP_UHPI_UART_I2S2   .set    100b<<12     ; 10:8-bit UHPI + 4:UART + 4:I2S2
PP_UHPI_UART_SPI0    .set    101b<<12     ; 10:8-bit UHPI + 4:UART + 4:SPI0(CS-0)
PP_GPIO_SPI0_I2S2_3 .set    110b<<12     ; 6:GPIO + 7:SPI0(CS-0,1,2,3) + 4:I2S2 + 4:I2S3
;----------------------------------------------------------------------------------------

IDLE_PCGCR              .set    0x1C02
    ; IDLE_PCGCR   Address:  0x1C02 (IO Space)           Peripheral Clock Gating Control Register LSW
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |rw:rw:rw:rw|rw:rw:rw:rw|rw:rw:rw:rw|rw:rw:rw:rw|  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  IIS3_IDLE         (0:clk active, 1:clk disabled)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  SPI_IDLE          (0:clk active, 1:clk disabled)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  UART_IDLE         (0:clk active, 1:clk disabled)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  DMA0_IDLE         (0:clk active, 1:clk disabled)
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  MMCSD0_IDLE       (0:clk active, 1:clk disabled)
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  MCBSP_IDLE		(0:clk active, 1:clk disabled)
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  I2C_IDLE          (0:clk active, 1:clk disabled)
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  MMCSD1_IDLE       (0:clk active, 1:clk disabled)
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  IIS0_IDLE         (0:clk active, 1:clk disabled)
    ;   |  |  |  |  |  |  +----------------------------- [9]  MCSPI_IDLE        (0:clk active, 1:clk disabled)         
    ;   |  |  |  |  |  +-------------------------------- [10] TIMER0_IDLE       (0:clk active, 1:clk disabled)
    ;   |  |  |  |  +----------------------------------- [11] EMIF_IDLE         (0:clk active, 1:clk disabled)
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] TIMER1_IDLE       (0:clk active, 1:clk disabled)
    ;   |  |  +----------------------------------------- [13] TIMER2_IDLE       (0:clk active, 1:clk disabled)
    ;   |  +-------------------------------------------- [14] IIS2_IDLE         (0:clk active, 1:clk disabled)
    ;   +----------------------------------------------- [15] MASTER_CLK_DIS    (0:Master clock active, 1:all clks disabled)

;----------------------------------------------------------------------------------------
;        IDLE_PCGCR         Bit Fields
;----------------------------------------------------------------------------------------
IIS3_IDLE           .set    1b<<0       ; IIS3 Idle Control Bit
SPI_IDLE            .set    1b<<1       ; SPI Idle Control Bit
UART_IDLE           .set    1b<<2       ; UART Control Bit
DMA0_IDLE           .set    1b<<3       ; DMA0 Idle Control Bit
MMCSD0_IDLE         .set    1b<<4       ; MMCSD0 Idle Control Bit
MCBSP_IDLE          .set    1b<<5       ; MCBSP Idle Control Bit
I2C_IDLE            .set    1b<<6       ; I2C Idle Control Bit
MMCSD1_IDLE         .set    1b<<7       ; MMCSD1 Idle Control Bit
IIS0_IDLE           .set    1b<<8       ; I2S0 Idle Control Bit
MCSPI_IDLE          .set    1b<<9       ; MCSPI Idle Control Bit
TIMER0_IDLE         .set    1b<<10      ; Timer 0 Idle Control Bit
EMIF_IDLE           .set    1b<<11      ; EMIF Idle Control Bit
TIMER1_IDLE         .set    1b<<12      ; Timer 1 Module Clock off
TIMER2_IDLE         .set    1b<<13      ; Timer 2 Module Clock off
IIS2_IDLE           .set    1b<<14      ; IIS2 Idle Control Bit
OSC_DISABLE         .set    1b<<15      ; Oscillator disable      (cory: use of this name should be discontinued... use 'MASTER_CLK_DIS')
MASTER_CLK_DIS      .set    1b<<15      ; Disables clock to ALL domains (can only wake via: RTC alarms, WAKEUP, INT0, INT1, & RESET)
RESERVED_PCGCR      .set    0000000000000000b       ; Reserved bits (MSB --> LSB)

IDLE_PCGCR_MSW          .set    0x1C03
    ; IDLE_PCGCR_MSW   Address:  0x1C03 (IO Space)           Peripheral Clock Gating Control Register MSW
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |r :r :r :r |r :r :r :r |r :r :rw:rw|rw:rw:rw:rw|  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  UHPI_CG        (0:active, 1:disabled)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  SAR_CG        (0:active, 1:disabled)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  USB_CG        (0:active, 1:disabled)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  DMA1_CG       (0:active, 1:disabled)
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  DMA2_CG       (0:active, 1:disabled)
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  DMA3_CG       (0:active, 1:disabled)
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  ANAREG_CG     (0:active, 1:disabled) (note: SAR_CG==0 then this reg is ignored and the ANAREG clocks are forced ON).
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  reserved
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  reserved
    ;   |  |  |  |  |  |  +----------------------------- [9]  reserved
    ;   |  |  |  |  |  +-------------------------------- [10] reserved
    ;   |  |  |  |  +----------------------------------- [11] reserved
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] reserved
    ;   |  |  +----------------------------------------- [13] reserved
    ;   |  +-------------------------------------------- [14] reserved
    ;   +----------------------------------------------- [15] reserved

;----------------------------------------------------------------------------------------
;        IDLE_PCGCR_MSW         Bit Fields
;----------------------------------------------------------------------------------------
UHPI_IDLE           .set    1b<<0       ; UHPI Idle Control Bit
SARADC_IDLE         .set    1b<<1       ; SAR A/D Idle Control Bit
USB_IDLE            .set    1b<<2       ; USB Idle Control Bit
DMA1_IDLE           .set    1b<<3       ; DMA1 Idle Control Bit
DMA2_IDLE           .set    1b<<4       ; DMA2 Idle Control Bit
DMA3_IDLE           .set    1b<<5       ; DMA3 Idle Control Bit
ANAREG_IDLE         .set    1b<<6       ; ANAREG Idle Control Bit
RESERVED_PCGCR_MSW  .set    1111111110000000b       ; Reserved bits


PER_RSTCOUNT            .set    0x1C04
    ; PER_RSTCOUNT   Address:  0x1C04 (IO space)          Peripheral Software Reset Counter Register
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |rw:rw:rw:rw|rw:rw:rw:rw|rw:rw:rw:rw|rw:rw:rw:rw|  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  PeripheralsResetCount[0]
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  PeripheralsResetCount[1]
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  PeripheralsResetCount[2]
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  PeripheralsResetCount[3]
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  PeripheralsResetCount[4]
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  PeripheralsResetCount[5]
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  PeripheralsResetCount[6]
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  PeripheralsResetCount[7]
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  PeripheralsResetCount[8]
    ;   |  |  |  |  |  |  +----------------------------- [9]  PeripheralsResetCount[9]
    ;   |  |  |  |  |  +-------------------------------- [10] PeripheralsResetCount[10]
    ;   |  |  |  |  +----------------------------------- [11] PeripheralsResetCount[11]
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] PeripheralsResetCount[12]
    ;   |  |  +----------------------------------------- [13] PeripheralsResetCount[13]
    ;   |  +-------------------------------------------- [14] PeripheralsResetCount[14]
    ;   +----------------------------------------------- [15] PeripheralsResetCount[15]

PER_RESET               .set    0x1C05
    ; PER_RESET   Address:  0x1C05 (IO space)             Peripheral Reset Register
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |rw:rw:rw:rw|rw:rw:rw:rw|rw:rw:rw:rw|rw:rw:rw:rw|  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+       Note:  Order matters.  Write to PeripheralsResetCount[15:0] first, then this register.
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |               ******* WRITES TO THIS REGISTER ********
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  ResetEn_I2C                         (0:reset left not asserted, 1:reset asserted for count cycles (HoldInReset==0) or indefinitely (HoldInReset==1))
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  ResetEn_EMIF_Timers_RTC             (0:reset left not asserted, 1:reset asserted for count cycles (HoldInReset==0) or indefinitely (HoldInReset==1))
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  ResetEn_Analog_SAR  (& TRIM prior to PG2.0!!!)     (0:reset left not asserted, 1:reset asserted for count cycles (HoldInReset==0) or indefinitely (HoldInReset==1)) (Customers should never reset this domain because of its effect on trim!!)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  ResetEn_USB                         (0:reset left not asserted, 1:reset asserted for count cycles (HoldInReset==0) or indefinitely (HoldInReset==1))
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  ResetEn_DMA                         (0:reset left not asserted, 1:reset asserted for count cycles (HoldInReset==0) or indefinitely (HoldInReset==1))
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  ResetEn_MMCSD0_MMCSD1_IIS0_MCSPI    (0:reset left not asserted, 1:reset asserted for count cycles (HoldInReset==0) or indefinitely (HoldInReset==1))
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  ResetEn_MCBSP                       (0:reset left not asserted, 1:reset asserted for count cycles (HoldInReset==0) or indefinitely (HoldInReset==1))
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  ResetEn_HPI_IIS2_IIS3_UART_SPI      (0:reset left not asserted, 1:reset asserted for count cycles (HoldInReset==0) or indefinitely (HoldInReset==1))
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  HoldInReset_I2C                     (0:hold in reset for PeripheralsResetCount[15:0] cycles, 1:hold in reset indefinitely)  (note ResetEn must also be written with a 1 to hold indefinitely)
    ;   |  |  |  |  |  |  +----------------------------- [9]  HoldInReset_EMIF_TIMERS_RTC         (0:hold in reset for PeripheralsResetCount[15:0] cycles, 1:hold in reset indefinitely)  (note ResetEn must also be written with a 1 to hold indefinitely)
    ;   |  |  |  |  |  +-------------------------------- [10] HoldInReset_Analog_SAR  (& TRIM prior to PG2.0!!!) (0:hold in reset for PeripheralsResetCount[15:0] cycles, 1:hold in reset indefinitely)  (note ResetEn must also be written with a 1 to hold indefinitely)
    ;   |  |  |  |  +----------------------------------- [11] HoldInReset_USB                     (0:hold in reset for PeripheralsResetCount[15:0] cycles, 1:hold in reset indefinitely)  (note ResetEn must also be written with a 1 to hold indefinitely)
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] HoldInReset_DMA                     (0:hold in reset for PeripheralsResetCount[15:0] cycles, 1:hold in reset indefinitely)  (note ResetEn must also be written with a 1 to hold indefinitely)
    ;   |  |  +----------------------------------------- [13] HoldInReset_MMCSD0_MMCSD1_IIS0_MCSPI (0:hold in reset for PeripheralsResetCount[15:0] cycles, 1:hold in reset indefinitely)  (note ResetEn must also be written with a 1 to hold indefinitely)
    ;   |  +-------------------------------------------- [14] HoldInReset_MCBSP                  (0:hold in reset for PeripheralsResetCount[15:0] cycles, 1:hold in reset indefinitely)  (note ResetEn must also be written with a 1 to hold indefinitely)
    ;   +----------------------------------------------- [15] HoldInReset_UHPI_IIS2_IIS3_UART_SPI  (0:hold in reset for PeripheralsResetCount[15:0] cycles, 1:hold in reset indefinitely)  (note ResetEn must also be written with a 1 to hold indefinitely)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |               ******* READS FROM THIS REGISTER ********
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  ResetState_I2C                      (read returns current state of peripheral's RESET REQUEST (0: not in reset, 1:peripheral is in reset))
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  ResetState_EMIF_Timers_RTC          (read returns current state of peripheral's RESET REQUEST (0: not in reset, 1:peripheral is in reset))
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  ResetState_Analog_SAR               (read returns current state of peripheral's RESET REQUEST (0: not in reset, 1:peripheral is in reset))
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  ResetState_USB                      (read returns current state of peripheral's RESET REQUEST (0: not in reset, 1:peripheral is in reset))
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  ResetState_DMA                      (read returns current state of peripheral's RESET REQUEST (0: not in reset, 1:peripheral is in reset))
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  ResetState_MMCSD0_MMCSD1_IIS0_MCSPI (read returns current state of peripheral's RESET REQUEST (0: not in reset, 1:peripheral is in reset))
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  ResetState_MCBSP                    (read returns current state of peripheral's RESET REQUEST (0: not in reset, 1:peripheral is in reset))
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  ResetState_LCD_IIS2_IIS3_UART_SPI   (read returns current state of peripheral's RESET REQUEST (0: not in reset, 1:peripheral is in reset))
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  HoldInReset_I2C                     (read returns the value last written)
    ;   |  |  |  |  |  |  +----------------------------- [9]  HoldInReset_EMIF_Timers_RTC         (read returns the value last written)
    ;   |  |  |  |  |  +-------------------------------- [10] HoldInReset_Analog_SAR              (read returns the value last written)
    ;   |  |  |  |  +----------------------------------- [11] HoldInReset_USB                     
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] HoldInReset_DMA                     (read returns the value last written)
    ;   |  |  +----------------------------------------- [13] HoldInReset_MMCSD0_MMCSD1_IIS0_MCSPI (read returns the value last written)
    ;   |  +-------------------------------------------- [14] HoldInReset_MCBSP                  (read returns the value last written)
    ;   +----------------------------------------------- [15] HoldInReset_UHPI_IIS2_IIS3_UART_SPI  (read returns the value last written)

;----------------------------------------------------------------------------------------
;        PER_RESET      Bit Fields
;----------------------------------------------------------------------------------------
I2C_RST                         .set    1b<<0       ; I2C Reset Bit
EMIF_RST                        .set    1b<<1       ; EMIF Reset Bit
SAR_RST                         .set    1b<<2       ; SAR Reset Bit
USB_RST                         .set    1b<<3       ; USB Reset Bit
DMA_RST                         .set    1b<<4       ; DMA Reset Bit
MMCSD0_MMCSD1_IIS0_MCSPI_RST    .set    1b<<5       ; MMCSD0_MMCSD1_IIS0_MCSPI Reset Bit
MCBSP_RST                       .set    1b<<6       ; MCBSP CoProcessor Reset Bit
UHPI_IIS2_IIS3_UART_SPI0_RST     .set    1b<<7       ; UHPI_IIS2_IIS3_UART_SPI Reset Bit

SLEW_RATE          .set    0x1C16
    ; SLEW_RATE   Address:  0x1C16 (IO space)       Output Pin Slew Rate Control Register
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |r :r :r :r |r :r :r :r |r :r :r :r |r :rw:r :rw|  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0| 0: 1: 0: 1|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  EMIF_SR[0]          (0:low freq, 1:high freq operation)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  CLKOUT_SR[0]        (0:low freq, 1:high freq operation)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  reserved
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  reserved
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  reserved
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  reserved
    ;   |  |  |  |  |  |  +----------------------------- [9]  reserved
    ;   |  |  |  |  |  +-------------------------------- [10] reserved
    ;   |  |  |  |  +----------------------------------- [11] reserved
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] reserved
    ;   |  |  +----------------------------------------- [13] reserved
    ;   |  +-------------------------------------------- [14] reserved
    ;   +----------------------------------------------- [15] reserved

CLKSTOP0         .set    0x1C3A
    ; CLKSTOP0   Address:  0x1C3A (IO Space)           Clock Stop Request/Acknowledge Register 0
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |r :r :r :r |r :r :r :r |r :r :r :rw|r :rw:r :rw|  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: x: 0| x: 0: x: 0|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  EMIF_CLKSTOP_REQ    (0:normal, 1:request permission to stop the clock)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  EMIF_CLKSTOP_ACK    (status register: 0:not ok to stop the clock, 1:ok to stop the clock)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  USB_CLKSTOP_REQ     (0:normal, 1:request permission to stop the clock)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  USB_CLKSTOP_ACK     (status register: 0:not ok to stop the clock, 1:ok to stop the clock)
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  UART_CLKSTOP_REQ    (0:normal, 1:request permission to stop the clock)
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  UART_CLKSTOP_ACK    (status register: 0:not ok to stop the clock, 1:ok to stop the clock)
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  MCBSP_CLKSTOP_REQ   (0:normal, 1:request permission to stop the clock)
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  MCBSP_CLKSTOP_ACK    (status register: 0:not ok to stop the clock, 1:ok to stop the clock)
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  reserved
    ;   |  |  |  |  |  |  +----------------------------- [9]  reserved
    ;   |  |  |  |  |  +-------------------------------- [10] UHPI_CLKSTOP_REQ   (0:normal, 1:request permission to stop the clock)
    ;   |  |  |  |  +----------------------------------- [11] UHPI_CLKSTOP_ACK    (status register: 0:not ok to stop the clock, 1:ok to stop the clock)
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] reserved
    ;   |  |  +----------------------------------------- [13] reserved
    ;   |  +-------------------------------------------- [14] reserved
    ;   +----------------------------------------------- [15] reserved

;----------------------------------------------------------------------------------------
;        CLK STOP Register0 Bit Fields 
;----------------------------------------------------------------------------------------
EMIF_CLKSTOP0_REQ                .set    1b<<0     ; Request permission to stop EMIF internal clock
EMIF_CLKSTOP0_ACK                .set    1b<<1     ; EMIF clock stop request status
USB_CLKSTOP0_REQ                 .set    1b<<2     ; Request permission to stop USB internal clock
USB_CLKSTOP0_ACK                 .set    1b<<3     ; USB clock stop request status
UART_CLKSTOP0_REQ                .set    1b<<4     ; Request permission to stop UART internal clock
UART_CLKSTOP0_ACK                .set    1b<<5     ; UART clock stop request status
MCBSP_CLKSTOP0_REQ               .set    1b<<6     ; Request permission to stop MCBSP internal clock
MCBSP_CLKSTOP0_ACK               .set    1b<<7     ; MCBSP clock stop request status
UHPI_CLKSTOP0_REQ               .set    1b<<10     ; Request permission to stop UHPI internal clock
UHPI_CLKSTOP0_ACK               .set    1b<<11     ; UHPI clock stop request status

CLKSTOP1         .set    0x1C3B
    ; CLKSTOP1   Address:  0x1C3B (IO Space)           Clock Stop Request/Acknowledge Register 1
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |r :r :r :r |r :r :r :r |r :r :r :rw|r :rw:r :rw|  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: x: 0| x: 0: x: 0|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  MCSPI_CLKSTOP_REQ     (0:normal, 1:request permission to stop the clock)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  MCSPI_CLKSTOP_ACK     (status register: 0:not ok to stop the clock, 1:ok to stop the clock)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  MCSPI_BRIDGE_CLKSTOP_REQ     (0:normal, 1:request permission to stop the clock)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  MCSPI_BRIDGE_CLKSTOP_ACK     (status register: 0:not ok to stop the clock, 1:ok to stop the clock)
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  reserved
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  reserved
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  reserved
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  reserved
    ;   |  |  |  |  |  |  +----------------------------- [9]  reserved
    ;   |  |  |  |  |  +-------------------------------- [10] reserved
    ;   |  |  |  |  +----------------------------------- [11] reserved
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] reserved
    ;   |  |  +----------------------------------------- [13] reserved
    ;   |  +-------------------------------------------- [14] reserved
    ;   +----------------------------------------------- [15] reserved

;----------------------------------------------------------------------------------------
;        CLK STOP Register1 Bit Fields 
;----------------------------------------------------------------------------------------
MCSPI_CLKSTOP_REQ               .set    1b<<0     ; Request permission to stop MCSPI internal clock
MCSPI_CLKSTOP_ACK               .set    1b<<1     ; MCSPI clock stop request status
MCSPI_BRIDGE_CLKSTOP_REQ        .set    1b<<2     ; Request permission to stop MCSPI Bridge internal clock
MCSPI_BRIDGE_CLKSTOP_ACK        .set    1b<<3     ; MCSPI Bridge clock stop request status

;****************************************************************************************
PIN_CONFIG              .set    0x1C1E
CONFIG_LSW              .set    0x1C1E
    ; PIN_CONFIG/CONFIG_LSW   Address:  0x1C1E (IO space)      Configuration LSW Register
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |r :r :r :r |r :r :r :r |r :r :r :r |r :r :r :rw|  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  ExtOscResEn       (0:XI & XO pins have internal 1Mohm resistor between them, 1:XI & XO have no internal resistor between them (external resistor required))
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  reserved
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  reserved
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  reserved
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  reserved
    ;   |  |  |  |  |  |  +----------------------------- [9]  reserved
    ;   |  |  |  |  |  +-------------------------------- [10] reserved
    ;   |  |  |  |  +----------------------------------- [11] reserved
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] reserved
    ;   |  |  +----------------------------------------- [13] reserved
    ;   |  +-------------------------------------------- [14] reserved
    ;   +----------------------------------------------- [15] reserved

CONFIG_MSW              .set    0x1C1F
    ; CONFIG_MSW   Address:  0x1C1F (IO space)           Clock Configuration MSW Register
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |r :r :r :r |r :r :r :r |r :r :r :r |rw:r :r :rw|  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 0: 0: 0: 0| 0: 0: 0: 0| 0: 0:??: 0| 0:??: 0: 0|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  SysClkSel        (0:PLL Bypassed (sysclk=osc), 1:PLL used (sysclk=pllout))
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  CLK_SEL_PIN      (read only - reads the state of the CLK_SEL pin; writes have no affect)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  Timer0ClkSel     (0:normal mode/same as all other peripherals (sysclk), 1:PLL used (Timer0clk <= pllout) (testmode for pll))
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  SysClkSelStatus[0]  (Read Only Status of Glitch-Free-Clock Mux's selected clk source:
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  SysClkSelStatus[1]   00:RTCXI, 01:PLL out, 10:CLKIN pin, 11:PLL out)   (note: bit1 is identical to CLK_SEL_PIN, bit0 is the pll used vs bypassed status)
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  reserved
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  reserved
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  reserved
    ;   |  |  |  |  |  |  +----------------------------- [9]  reserved
    ;   |  |  |  |  |  +-------------------------------- [10] reserved
    ;   |  |  |  |  +----------------------------------- [11] reserved
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] reserved
    ;   |  |  +----------------------------------------- [13] reserved
    ;   |  +-------------------------------------------- [14] reserved
    ;   +----------------------------------------------- [15] reserved

;****************************************************************************************
;    Clock Config Control Register Bit Fields
;****************************************************************************************
SYS_PLL_BYPASS      .set    0b << 0     ; System clock has PLL Bypassed
SYS_PLL_SEL         .set    1b << 0     ; System clock has PLL Selected
SAR_PLL_BYPASS      .set    0b << 1     ; SAR clock has PLL Bypassed
;(cory) removed in REL_CORAZON_80    SAR_PLL_SEL         .set    1b << 1     ; SAR clock PLL Selected
CLK_SEL_STAT        .set    2           ; To test the status fo the CLK_SEL_PIN
OSC_INT_RES_EN      .set    0b << 0     ; Osciallator internal 1MOhm reistance enabled
OSC_INT_RES_DIS     .set    1b << 0     ; Osciallator internal 1MOhm reistance disabled
TIMER0_SYSCLK       .set    0b << 3     ; Timer0 clock is the same as the system clock
TIMER0_OSCCLK       .set    1b << 3     ; Timer0 clock derived before PLL (Oscillator or CLKIN) (Incorrect description, see below)
TIMER0_PLLCLK       .set    1b << 3     ; Timer0 clock derived from PLL
;****************************************************************************************

BOOTMODE                .set    0x1C34
    ; BOOTMODE   Address:  0x1C34 (IO space)       Test Module's Bootmode Read Only Register
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |r :r :r :r |r :r :r :r |r :r :r :r |r :r :r :r |  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0|  <<== Reset Values (trstn)
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  BootMode[0]   Test bootmode register value from jtag
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  BootMode[1]   (used for testmodes in rom)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  BootMode[2]
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  BootMode[3]
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  BootMode[4]
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  BootM_A15[0]
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  BootM_A16[1]
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  BootM_A17[2]
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  BootM_A18[3]
    ;   |  |  |  |  |  |  +----------------------------- [9]  BootM_A19[4]
    ;   |  |  |  |  |  +-------------------------------- [10] BootM_A20[5]
    ;   |  |  |  |  +----------------------------------- [11] reserved
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] reserved
    ;   |  |  +----------------------------------------- [13] reserved
    ;   |  +-------------------------------------------- [14] reserved
    ;   +----------------------------------------------- [15] reserved
    ;
    ; ????   Address:  0x1C35 (IO space)
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |r :r :r :r |r :r :r :r |r :r :r :r |r :r :r :r |  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | ?: ?: ?: ?| ?: ?: ?: ?| ?: ?: ?: ?| ?: ?: ?: ?|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  reserved
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  reserved
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  reserved
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  reserved
    ;   |  |  |  |  |  |  +----------------------------- [9]  reserved
    ;   |  |  |  |  |  +-------------------------------- [10] reserved
    ;   |  |  |  |  +----------------------------------- [11] reserved
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] reserved
    ;   |  |  +----------------------------------------- [13] reserved
    ;   |  +-------------------------------------------- [14] reserved
    ;   +----------------------------------------------- [15] reserved

EFUSE_CTRL   .set 0x1C54
    ; EFUSE_CTRL   Address:  0x1C54 (IO space)          EFUSE CTRL Register
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |r :r :r :r |r :r :r :r |r :r :r :r |r :r :r :rw|  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |        *** This register must be unlocked with the TEST_LOCK register before it can be written ***
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  sys_initz       (0: holds the efuse controller in reset, 1: efuse controller released from reset)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  efc_ready       (Status of efuse controller:  0: not finished shifting the repair chain, 1: finished shifting the repair chain)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  efc_error[0]    (Error Status of the efuse controller.  Non zero indicates an error occured while
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  efc_error[1]     shifting the repair data.  Register is updated at the same time as efc_ready.
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  efc_error[2]     See the C027 fuse_farm controller documentation for error codes.)
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  efc_error[3]
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  efc_error[4]
    ;   |  |  |  |  |  |  +----------------------------- [9]  reserved
    ;   |  |  |  |  |  +-------------------------------- [10] reserved
    ;   |  |  |  |  +----------------------------------- [11] reserved
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] reserved
    ;   |  |  +----------------------------------------- [13] reserved
    ;   |  +-------------------------------------------- [14] reserved
    ;   +----------------------------------------------- [15] reserved

TEST_LOCK   .set 0x1C55
    ; TEST_LOCK   Address:  0x1C55 (IO space)          **Undocumented** Test Lock Register
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |rw:rw:rw:rw|rw:rw:rw:rw|rw:rw:rw:rw|rw:rw:rw:rw|  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  test_lock[0]    (Certain values in test_lock[15:0] unlock other test registers for writes:
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  test_lock[1]        EFUSE_CTRL    : cannot be written unless unlocked first with code 0x569A
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  test_lock[2]        SET_REF_TRIM  : cannot be written unless unlocked first with code 0xC001
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  test_lock[3]    )
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  test_lock[4]    (note: the TEST_LOCK register is not self-clearing.  Software should write
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  test_lock[5]           0x0000 to re-lock the test registers after they're initialized.)
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  test_lock[6]
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  test_lock[7]
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  test_lock[8]
    ;   |  |  |  |  |  |  +----------------------------- [9]  test_lock[9]
    ;   |  |  |  |  |  +-------------------------------- [10] test_lock[10]
    ;   |  |  |  |  +----------------------------------- [11] test_lock[11]
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] test_lock[12]
    ;   |  |  +----------------------------------------- [13] test_lock[13]
    ;   |  +-------------------------------------------- [14] test_lock[14]
    ;   +----------------------------------------------- [15] test_lock[15]
EFUSE_TRIM_CHNG_KEY     .set    0x569A
SET_REF_TRIM_CHNG_KEY   .set    0xC001

;****************************************************************************************
;        Clock Domain Registers
;****************************************************************************************
IDLE_ICR        .set    0x0001        ; IDLE Control Register
    ; IDLE_ICR   Address:  0x0001 (IO Space)             IDLE Control Register
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |rw:rw:rw:rw|rw:rw:rw:rw|rw:rw:rw:rw|rw:rw:rw:rw|  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 1: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  CPU_IDLE          (0:active, 1:idled)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  reserved, writes should set to 1
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  reserved, writes should set to 1
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  reserved, writes should set to 1
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  CLK_IDLE  *** reserved, writes should set to 0 ***
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  DPORT_IDLE        (0:active, 1:idled)
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  XPORT_IDLE        (0:active, 1:idled - CPUI must be idled too)
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  MPORT_IDLE        (0:active, 1:idled - DMA must be idled too)
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  IPORT_IDLE        (0:active, 1:idled)
    ;   |  |  |  |  |  |  +----------------------------- [9]  HWA_IDLE          (0:active, 1:idled)
    ;   |  |  |  |  |  +-------------------------------- [10] reserved
    ;   |  |  |  |  +----------------------------------- [11] reserved
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] reserved
    ;   |  |  +----------------------------------------- [13] reserved
    ;   |  +-------------------------------------------- [14] reserved
    ;   +----------------------------------------------- [15] reserved
IDLE_ST         .set    0x0002        ; Idle Status Register
    ; IDLE_ST   Address:  0x0002 (IO Space)              Idle Status Register
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |r :r :r :r |r :r :r :r |r :r :r :r |r :r :r :r |  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 1: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  CPUIS     (0:active, 1:idled)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  DPORTIS       (0:active, 1:idled)
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  XPORTIS       (0:active, 1:idled)
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  MPORTIS       (0:active, 1:idled)
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  IPORTIS       (0:active, 1:idled)
    ;   |  |  |  |  |  |  +----------------------------- [9]  HWAIS         (0:active, 1:idled)
    ;   |  |  |  |  |  +-------------------------------- [10] reserved
    ;   |  |  |  |  +----------------------------------- [11] reserved
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] reserved
    ;   |  |  +----------------------------------------- [13] reserved
    ;   |  +-------------------------------------------- [14] reserved
    ;   +----------------------------------------------- [15] reserved

;----------------------------------------------------------------------------------------
;        ICR Bit Fields
;----------------------------------------------------------------------------------------
CPU_IDLE        .set    1b          ; CPU Idle Control Bit
CLK_IDLE        .set    1b<<4       ; Clock Generator Idle Control Bit
DPORT_IDLE      .set    1b<<5       ; DPORT Idle Control Bit
XPORT_IDLE      .set    1b<<6       ; XPORT Idle Control Bit
MPORT_IDLE      .set    1b<<7       ; MPORT Idle Control Bit
IPORT_IDLE      .set    1b<<8       ; IPORT Idle Control Bit
HWA_IDLE        .set    1b<<9       ; HWA Idle Control Bit
RESERVED_ICR    .set    1111110000001110b       ; Reserved bits (UMA peripheral, DMA, cache)

;****************************************************************************************
;        ANA_REG Registers          (0x7002 - 0x70FF)
;****************************************************************************************
TRIM_FROM_EFUSE         .set    0x7002
    ; TRIM_FROM_EFUSE   Address:  0x7002 (IO space)       Read-Only Trim Values from Efuse
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |r :r :r :r |r :r :r :r |r :r :r :r |r :r :r :r |  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | ?: ?: ?: ?| ?: ?: ?: ?| ?: ?: ?: ?| ?: ?: ?: ?|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  efuse_BG_CoarseTrim[0]       (read-only value from the efuse bits.  Will always read 0000 prior
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  efuse_BG_CoarseTrim[1]        to the fuses being blown.)
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  efuse_BG_CoarseTrim[2]
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  efuse_BG_CoarseTrim[3]
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  efuse_CurrentRef_Trim[0]
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  efuse_CurrentRef_Trim[1]
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  efuse_CurrentRef_Trim[2]
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  efuse_CurrentRef_Trim[3]
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  reserved
    ;   |  |  |  |  |  |  +----------------------------- [9]  reserved
    ;   |  |  |  |  |  +-------------------------------- [10] reserved
    ;   |  |  |  |  +----------------------------------- [11] reserved
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] reserved
    ;   |  |  +----------------------------------------- [13] reserved
    ;   |  +-------------------------------------------- [14] reserved
    ;   +----------------------------------------------- [15] reserved

SET_REF_TRIM        .set    0x7003
    ; SET_REF_TRIM   Address:  0x7003 (IO space)             Analog Reference Trim Register
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |rw:rw:r :r |r :rw:rw:rw|rw:rw:rw:rw|rw:rw:rw:rw|  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+       **** This register must be unlocked via the TEST_LOCK register to enable writes. ****
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |        In production test, the Bandgap voltage reference should be trimmed 
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |        to 0.7v +- 44mV.  Current Reference should be trimmed to 4uA +- 70nA.
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  ana_BG_CoarseTrim[0]         [0000, 0001, 0010, ... 0111 = (0, 1, 2, ... 7) * BG_Coarse_StepSize,
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  ana_BG_CoarseTrim[1]          1000, 1001, 1010, ... 1111 = (-8, -7, -6, ... -1) * BG_Coarse_StepSize]
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  ana_BG_CoarseTrim[2]
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  ana_BG_CoarseTrim[3]         (BG_Coarse_StepSize =~ 40mV)
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  ana_CurrentRef_Trim[0]      [0000, 0001, 0010, ... 0111 = (0, 1, 2, ... 7) * CR_StepSize,
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  ana_CurrentRef_Trim[1]       1000, 1001, 1010, ... 1111 = (-8, -7, -6, ... -1) * CR_StepSize]
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  ana_CurrentRef_Trim[2]
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  ana_CurrentRef_Trim[3]      (CR_StepSize =~ 70nA)
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  analog_testmode_sel[0]  [Enables transmission gates from various analog testpoints to the GPAIN3 pin.  In many cases it is also necessary to enable the testpoint in another register.
    ;   |  |  |  |  |  |  +----------------------------- [9]  analog_testmode_sel[1]   000:reserved, 001:reserved, 010:reserved, 011:pll_opamp_out_test,
    ;   |  |  |  |  |  +-------------------------------- [10] analog_testmode_sel[2]   100:sar ibias test, 101:Current Reference test, 110:reserved, 111:reserved]
    ;   |  |  |  |  +----------------------------------- [11] reserved
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] reserved
    ;   |  |  +----------------------------------------- [13] reserved
    ;   |  +-------------------------------------------- [14] currentref_testmode     [0:normal, 1:Enables the current reference to be measured on the GPAIN3 pin.  Must also set analog_testmode_sel[2:0] == 101]
    ;   +----------------------------------------------- [15] por_lv_det_dis          [POR low voltage detection - 0:enabled, 1:disabled.  Should be disabled (set high) before the trim bits are adjusted, else the POR might think a low-voltage condition exists & reset the DSP.]

USB_ANA_CNTL            .set    0x7004
LDO_CNTL            .set    0x7004
    ; LDO_CNTL/USB_ANA_CNTL   Address:  0x7004 (IO space)              LDO Control
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |r :r :r :r |r :r :r :r |r :r :r :r |r :r :rw:rw|  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  usb_ldo_en      [0:disabled (w/output tristated), 1:enabled]
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  dsp_ldo_v       [0:DSP_LDO set to 1.3V, 1:DSP_LDO set to 1.05V]
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  reserved
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  reserved
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  reserved
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  reserved
    ;   |  |  |  |  |  |  +----------------------------- [9]  reserved
    ;   |  |  |  |  |  +-------------------------------- [10] reserved
    ;   |  |  |  |  +----------------------------------- [11] reserved
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] reserved
    ;   |  |  +----------------------------------------- [13] reserved
    ;   |  +-------------------------------------------- [14] reserved
    ;   +----------------------------------------------- [15] reserved

TRIM_FROM_EFUSE2         .set    0x7006
    ; TRIM_FROM_EFUSE2   Address:  0x7006 (IO space)       Read-Only Trim Values from Efuse
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |r :r :r :r |r :r :r :r |r :r :r :r |r :r :r :r |  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | ?: ?: ?: ?| ?: ?: ?: ?| ?: ?: ?: ?| ?: ?: ?: ?|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  efuse_Vth_CoarseTrim[0]
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  efuse_Vth_CoarseTrim[1]
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  efuse_Vth_CoarseTrim[2]
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  efuse_Vth_CoarseTrim[3]
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  efuse_BG_FineTrim[0]
    ;   |  |  |  |  |  |  +----------------------------- [9]  efuse_BG_FineTrim[1]
    ;   |  |  |  |  |  +-------------------------------- [10] efuse_BG_FineTrim[2]
    ;   |  |  |  |  +----------------------------------- [11] reserved
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] efuse_Vth_FineTrim[0]
    ;   |  |  +----------------------------------------- [13] efuse_Vth_FineTrim[1]
    ;   |  +-------------------------------------------- [14] efuse_Vth_FineTrim[2]
    ;   +----------------------------------------------- [15] reserved

SET_REF_TRIM2        .set    0x7007
    ; SET_REF_TRIM2   Address:  0x7007 (IO space)             Analog Reference Trim Register
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    ; |15:14:13:12|11:10: 9: 8| 7: 6: 5: 4| 3: 2: 1: 0|  <<== Bit Position
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; |r :rw:rw:rw|r :rw:rw:rw|rw:rw:rw:rw|r :rw:rw:rw|  <<== Read / Write Access
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--:--:--+
    ; | 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0| 0: 0: 0: 0|  <<== Reset Values
    ; +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+       **** This register must be unlocked via the TEST_LOCK register to enable writes. ****
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |        In production test, the Bandgap voltage reference should be trimmed 
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |        to 0.7v +- 10mV.  Current Reference should be trimmed to 4uA +- 70nA.
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-- [0]  Disable_CP_DSP_LDO
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----- [1]  Disable_CP_ANA_LDO
    ;   |  |  |  |  |  |  |  |  |  |  |  |  |  +-------- [2]  Disable_CP_USB_LDO
    ;   |  |  |  |  |  |  |  |  |  |  |  |  +----------- [3]  reserved
    ;   |  |  |  |  |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  |  |  |  |  +-------------- [4]  ana_Vth_CoarseTrim[0]         [0000, 0001, 0010, ... 0111 = (0, 1, 2, ... 7) * Vth_Coarse_StepSize,
    ;   |  |  |  |  |  |  |  |  |  |  +----------------- [5]  ana_Vth_CoarseTrim[1]          1000, 1001, 1010, ... 1111 = (-8, -7, -6, ... -1) * Vth_Coarse_StepSize]
    ;   |  |  |  |  |  |  |  |  |  +-------------------- [6]  ana_Vth_CoarseTrim[2]
    ;   |  |  |  |  |  |  |  |  +----------------------- [7]  ana_Vth_CoarseTrim[3]         (Vth_Coarse_StepSize =~ 40mV)
    ;   |  |  |  |  |  |  |  |
    ;   |  |  |  |  |  |  |  +-------------------------- [8]  ana_BG_FineTrim[0]        [000, 001, 010, 011 = (0, 1, 2, 3) * BG_Fine_StepSize,
    ;   |  |  |  |  |  |  +----------------------------- [9]  ana_BG_FineTrim[1]         100, 101, 110, 111 = (-4, -3, -2, ... -1) * BG_Fine_StepSize]
    ;   |  |  |  |  |  +-------------------------------- [10] ana_BG_FineTrim[2]        (BG_Fine_StepSize =~ 5-8mV)
    ;   |  |  |  |  +----------------------------------- [11] reserved
    ;   |  |  |  |
    ;   |  |  |  +-------------------------------------- [12] ana_Vth_FineTrim[0]       [000, 001, 010, 011 = (0, 1, 2, 3) * Vth_Fine_StepSize,
    ;   |  |  +----------------------------------------- [13] ana_Vth_FineTrim[1]        100, 101, 110, 111 = (-4, -3, -2, ... -1) * Vth_Fine_StepSize]
    ;   |  +-------------------------------------------- [14] ana_Vth_FineTrim[2]       (Vth_Fine_StepSize =~ 5-8mV)
    ;   +----------------------------------------------- [15] reserved

