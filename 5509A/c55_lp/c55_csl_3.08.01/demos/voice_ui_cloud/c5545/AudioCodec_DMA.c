/* ============================================================================
* Copyright (c) 2017 Texas Instruments Incorporated.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/
/** @file AudioCodec_DMA.c
*
*  @brief Test code to run the C5545BP + CC3220SF-LAUNCHXL TI design demo TIDEP-0083
*
* NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSION C5545_BSTPCK.
* ENSURE THAT THE PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE PRIOR TO COMPILATION.
* c55xx_csl\inc\csl_general.h.
*/
/* ============================================================================
* Revision History
* ================
* 07th-Aug-2014 Created
* 09th-Mar-2016 Header update
* 01st-Sep-2017 Voiceuicloud demo updates
* ============================================================================
*/

#include <log.h>
#include <std.h>
#include <mbx.h>
#include <sem.h>
#include <tsk.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/* CSLR include files */
#include "soc.h"
#include "cslr.h"
#include "cslr_sysctrl.h"
/* CSL include files */
#include "csl_gpio.h"
#include "csl_i2s.h"
#include "csl_dma.h"
#include "csl_intc.h"
#include "csl_sysctrl.h"

#include "pll_control.h"
#include "codec_aic3254.h"
#include "codec_pcm186x.h"
#include "IdleLoop.h"
#include "BF_rt_bios_cfg.h"

#include "csl_uart.h"
#include "csl_uartAux.h"
#include "csl_general.h"

#include "oled.h"

/* Global constants */
/* String length to be received and transmitted */
#define WR_STR_LEN    (80u)
#define RD_STR_LEN    (10u)

#define CSL_TEST_FAILED        (-1)
#define NO_OF_CHAR_TO_READ     (256u)
#define NO_OF_CHAR_TO_WRITE    (28u)
#define CSL_UART_WRBUF_SIZE    (NO_OF_CHAR_TO_WRITE*4)
#define CSL_UART_RDBUF_SIZE    (NO_OF_CHAR_TO_READ*4)

#define CSL_PLL_DIV_000    (0)
#define CSL_PLL_DIV_001    (1u)
#define CSL_PLL_DIV_002    (2u)
#define CSL_PLL_DIV_003    (3u)
#define CSL_PLL_DIV_004    (4u)
#define CSL_PLL_DIV_005    (5u)
#define CSL_PLL_DIV_006    (6u)
#define CSL_PLL_DIV_007    (7u)

#define CSL_PLL_CLOCKIN    (32768u)

#define PLL_CNTL1        *(ioport volatile unsigned *)0x1C20
#define PLL_CNTL2        *(ioport volatile unsigned *)0x1C21
#define PLL_CNTL3        *(ioport volatile unsigned *)0x1C22
#define PLL_CNTL4        *(ioport volatile unsigned *)0x1C23

//#define UART_OUTPUT_DEBUG

//buffer for UART output debug
#ifdef UART_OUTPUT_DEBUG
#define UART_DUMP_BUF_SIZE 32000
Uint16 uartdumpbuf [UART_DUMP_BUF_SIZE];
Uint16 uartidx =0;
#endif

#define C5545_BSTPCK

/* Global data definition */
/* UART setup structure */
CSL_UartSetup uartSetup =
{
      /* Input clock freq in MHz */
    200000000,
      /* Baud rate */
    1228800,
      /* Word length of 8 */
    CSL_UART_WORD8,
      /* To generate 1 stop bit */
    0,
      /* Disable the parity */
    CSL_UART_DISABLE_PARITY,
      /* Disable fifo */
      /* Enable trigger 01 fifo */
    CSL_UART_FIFO_DMA1_ENABLE_TRIG01,
      /* Loop Back disable */
    CSL_UART_NO_LOOPBACK,
      /* No auto flow control*/
      CSL_UART_NO_AFE ,
      /* No RTS */
      CSL_UART_NO_RTS ,
};

CSL_UartObj uartObj;

Uint32 getSysClk(void);
Uint32 getUartSysClk(void);
CSL_UartHandle hUart;
CSL_Status CSL_uartInit(void);
void SensoryRecogInit(Void);
void AudioDataCollection(Int16 *aerSendInBuf);

/* CSL UART object */
CSL_UartObj uartObj;

/* CSL DMA data structures used for UART data transfers */
CSL_DMA_Handle dmaWrHandle;
CSL_DMA_Handle dmaRdHandle;
CSL_DMA_Config dmaConfig;
CSL_DMA_ChannelObj    dmaWrChanObj;
CSL_DMA_ChannelObj    dmaRdChanObj;

/**Dma write is happening in 32 bits,if use character array*/
//#define UART_RX_BUF_SIZE 80
Uint32 guartDmaWriteBuf[(I2S_DMA_BUF_LEN +2)*2] = {'S','Y','N','C'};
Uint32 guartDmaReadBuf[UART_RX_BUF_SIZE];
int guartDmaReadBufIdx = 0;
/**
*  \brief  Configures Dma
*
*  \param  chanNum - Dma channel number
*
*  \return Dma handle
*/
CSL_DMA_Handle CSL_configDmaForUart(CSL_DMA_ChannelObj    *dmaChanObj,
                                    CSL_DMAChanNum        chanNum);
//////////////////////////////////////////////////////////////////


#ifdef PROFILE_CYCLES
#include "csl_gpt.h"
#endif

//UART DMA CHANNEL
#define DMA_CHAN_TX_L      ( CSL_DMA_CHAN4 )
#define DMA_CHAN_TX_R      ( CSL_DMA_CHAN5 )

//#ifndef C5545BP_2MIC
#define DMA_CHAN_RX_L      ( CSL_DMA_CHAN6 )
#define DMA_CHAN_RX_R      ( CSL_DMA_CHAN7 )
//#endif

#ifdef INSTANCE1_I2S
#define I2S_INSTANCE_2      ( I2S_INSTANCE1 )
#define DMA_CHAN_RX_L2      ( CSL_DMA_CHAN12 )
#define DMA_CHAN_RX_R2      ( CSL_DMA_CHAN13 )
#endif

#ifdef PROFILE_CYCLES
typedef struct cycles_s {
  long     max; /**< running maximum of cycles */
  long     avg; /**< running average of cycles (using exponential averaging) */
} cycles_t;

extern CSL_Handle  hGpt;
extern CSL_Config  hwConfig;
CSL_Status         statusInt = 0;
Uint32             timeCntInt1= 0;
Uint32             timeCntInt2 = 0;
cycles_t           cyclesDmaInt = {0, 0};
static inline void process_cycles(long cycles, cycles_t * cyclesPtr)
{
  if (cycles > cyclesPtr->max) {
    cyclesPtr->max = cycles;
  }

  cyclesPtr->avg = ((cyclesPtr->avg) * 255 + cycles) >> 8;
}
#endif

extern void VECSTART(void); // defined in vector table
CSL_IRQ_Dispatch     dispatchTable;

CSL_GpioObj gGpioObj;
GPIO_Handle hGpio;

CSL_I2sHandle hI2s;

CSL_DMA_Handle dmaLeftTxHandle;
CSL_DMA_Handle dmaRightTxHandle;
CSL_DMA_ChannelObj dmaObj0, dmaObj1, dmaObj2, dmaObj3;
volatile Uint16 dmaTxFrameCount = 0, dmaRxFrameCount=0;

CSL_DMA_Handle dmaLeftRxHandle;
CSL_DMA_Handle dmaRightRxHandle;

/* Ping/pong buffers for serilizer 1 (I2S2) */
#ifdef ENABLE_AUDIO_CONVERSION
#pragma DATA_ALIGN(i2sDmaWriteBufLeftOut, 8);
Uint32 i2sDmaWriteBufLeftOut[I2S_DMA_BUF_LEN*SYS_FS_RATIO];  // left Tx write ping/pong buffer output
#pragma DATA_ALIGN(i2sDmaWriteBufRightOut, 8);
Uint32 i2sDmaWriteBufRightOut[I2S_DMA_BUF_LEN*SYS_FS_RATIO]; // right Tx write ping/pong buffe output
#endif
#pragma DATA_ALIGN(i2sDmaWriteBufLeft, 8);
Uint32 i2sDmaWriteBufLeft[I2S_DMA_BUF_LEN];  // left Tx write ping/pong buffer
#pragma DATA_ALIGN(i2sDmaWriteBufRight, 8);
Uint32 i2sDmaWriteBufRight[I2S_DMA_BUF_LEN]; // right Tx write ping/pong buffe
#ifdef ENABLE_AUDIO_CONVERSION
#pragma DATA_ALIGN(i2sDmaReadBufLeftIn, 8);
Uint32 i2sDmaReadBufLeftIn[I2S_DMA_BUF_LEN*SYS_FS_RATIO];  // left Rx read ping/pong buffer input
#pragma DATA_ALIGN(i2sDmaReadBufRightIn, 8);
Uint32 i2sDmaReadBufRightIn[I2S_DMA_BUF_LEN*SYS_FS_RATIO]; // right Rx read ping/pong buffer input
#endif
#pragma DATA_ALIGN(i2sDmaReadBufLeft, 8);
Uint32 i2sDmaReadBufLeft[I2S_DMA_BUF_LEN];  // left Rx read ping/pong buffer
#pragma DATA_ALIGN(i2sDmaReadBufRight, 8);
Uint32 i2sDmaReadBufRight[I2S_DMA_BUF_LEN]; // right Rx read ping/pong buffer

#ifdef INSTANCE1_I2S
CSL_I2sHandle hI2s2;

CSL_DMA_Handle dmaLeftRxHandle2;
CSL_DMA_Handle dmaRightRxHandle2;
CSL_DMA_ChannelObj dmaObj4, dmaObj5;
#endif

// Initializes device
CSL_Status DeviceInit(void);
#ifndef CHIP_C5517
// USB LDO Switch
void UsbLdoSwitch(Uint16 mode);
#endif
// Initializes DMA
CSL_Status DmaInit(void);

// GPIO initialization
CSL_Status GpioInit(Uint32 ioDir, Uint32 intEn, Uint32 intEdg);

// I2S and DMA initialization
CSL_Status I2sDmaInit(void);

// Send OLED data
void Oledsend(void);

// User defined algorithm
void UserAlgorithm(void);

// Put CPU in idle
void UserIdle(void);

// DMA ISR
void DmaIsr(void);

CSL_Status ioExpander_Setup(void);
CSL_Status ioExpander_Read(Uint16 port, Uint16 pin, Uint16 *rValue);
CSL_Status ioExpander_Write(Uint16 port, Uint16 pin, Uint16 value);

extern int *inFramePtr[];
extern int *buf_Outptr;
extern int *buf_Outptr2;
void bf_main (void);
int bf_process(void);

extern int      start   ;
extern void     setXF()   ;
extern void     waitLoop()   ;

void main(void)
{
    CSL_Status status;

    // Enable HWA, CPU, DPORT, MPORT, XPORT, and IPORT in ICR */
    *(volatile ioport Uint16 *)(0x0001) = 0x000E;
    /* Execute idle instruction */
    asm("    idle");

    // enable all the peripherals 400
    CSL_SYSCTRL_REGS->PCGCR1 = 0x0000;
    CSL_SYSCTRL_REGS->PCGCR2 = 0x0000;

    printf("Voice UI Cloud Demo for C5545 Booster Pack with LMB (2 mics) is now running!\n");
    printf("Say the trigger phrase Hello Blue Genie followed by an English Phrase up to 4.5 seconds in length\n");
    printf("The audio clip would be put through the algorithms BF+ASNR+MSS+DRC, then sent\n");
    printf("to the CC3220SF-LAUNCHXL via UART and onwards to the IBM Watson servers.\n");
    printf("IBM will transcribe the audio and send back to the C5545BP via UART from the CC3220SF-LAUNCHXL\n");
    printf("The transcription will be seen on the C5545BP OLED\n");
    printf("The system would then be standing by again for the trigger phrase\n\n");

    // Initialize the audio DMA TX buffers
#ifdef ENABLE_AUDIO_CONVERSION
    memset(i2sDmaWriteBufLeftOut, 0, I2S_DMA_BUF_LEN*SYS_FS_RATIO*sizeof(Uint32));
    memset(i2sDmaWriteBufRightOut, 0, I2S_DMA_BUF_LEN*SYS_FS_RATIO*sizeof(Uint32));
#else
    memset(i2sDmaWriteBufLeft, 0, I2S_DMA_BUF_LEN*sizeof(Uint32));
    memset(i2sDmaWriteBufRight, 0, I2S_DMA_BUF_LEN*sizeof(Uint32));
#endif

    // Initialize DSP
    status = DeviceInit();
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize DSP\n");
        exit(EXIT_FAILURE);
    }

    //OLED init
    /* Initialize I2C */
    oled_display_I2C_init();

    // Initialize DMA
    status = DmaInit();
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize DMA\n");
        exit(EXIT_FAILURE);
    }

    /* GPIO10 for AIC3204 reset */
    Uint32 gpioIoDir = (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN10);
    status = GpioInit(gpioIoDir, 0x00000000, 0x00000000);
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize GPIO\n");
        exit(1);
    }

    // Initialize I2S with DMA
    status = I2sDmaInit();
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize I2S and DMA\n");
        exit(1);
    }

    /* Intialize AIC3204 codec */
    status = AIC3254_init(16000, 16000, PLL_MHZ);
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize AIC3204\n");
        exit(1);
    }

    /* Intialize PCM1864 codec */
    status = PCM186x_init(16000, PLL_MHZ);
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize PCM186x\n");
        exit(1);
    }

    /* Initialize Sensory Process */
     SensoryRecogInit() ;

    /* Set CPUI bit in ICR */
    //*(volatile ioport Uint16 *)(0x0001) |= 0x000F;
    /* Set CPUI, DPORTI, XPORTI, and IPORTI in ICR */
    *(volatile ioport Uint16 *)(0x0001) |= 0x016F;

    /* Enable global interrupt */
    IRQ_globalEnable();

    if(!dmaRxFrameCount)
    {
        // For reading Mic 1 and Mic 2 using Boosterpack headers
        /* Start left Rx DMA for I2S1 */
        status = DMA_start(dmaLeftRxHandle2);
        if (status != CSL_SOK)
        {
            printf("I2S Dma Left Rx for I2S1 Failed!!\n");
            exit(EXIT_FAILURE);
        }


        /* Start right Rx DMA for I2S1 */
        status = DMA_start(dmaRightRxHandle2);
        if (status != CSL_SOK)
        {
            printf("I2S Dma Right Rx for I2S1 Failed!!\n");
            exit(EXIT_FAILURE);
        }

    }

    /* Enable I2S */
    CSL_I2S1_REGS->I2SSCTRL |= 0x8000;
#ifndef LOOPBACK_ONLY

    // setup BF, ASNR, MSS, DRC
    bf_main();
#endif

    //UART init
    CSL_uartInit();

    //Display "Ready" at start on BP OLED
    oledInit();
    clear();
    setline(0);
    setOrientation(1);
    printstr("Ready");

   //   printf("Ready. Say 'Hello Blue Genie' to trigger.....\n");
    printf("Pumping PC lineout to the HP Output Started!!\n");
}

void audioLoopTask()
{
    while (1)
    {
        // wait for the I2S DMA is done
        SEM_pend(&SEM_i2s_dmaTransferDone, SYS_FOREVER);


        /* Perform your algorithm here */
        UserAlgorithm();

        /* Set CPU to Idle */
        //UserIdle();
    }
}
//*****************************************************************************
// DeviceInit
//*****************************************************************************
//  Description:
//      Sets up interrupt vector table,
//      Disables and clears interrupts,
//      Resets all peripherals,
//      Sets PLL output to desired MHz,
//      Disables USB LDO
//
//  Arguments:
//      none
//
//  Return:
//      CSL_Status: CSL_SOK - Device initialization successful
//                  other   - Device initialization unsuccessful
//
//*****************************************************************************
CSL_Status DeviceInit(void)
{
    Uint16 prcr;
    CSL_Status status;

    // Set up dispatch table
    status = IRQ_init(&dispatchTable, 0);
    if (status != CSL_SOK)
    {
        //printf("IRQ_init() fail, status=%d\n", status);
        return status;
    }

    // Disable global interrupts
    IRQ_globalDisable();

    // Disable all the interrupts
    IRQ_disableAll();

    // Clear any pending interrupts
    IRQ_clearAll();

    // Set the interrupt vector table address
    status = IRQ_setVecs((Uint32)&VECSTART);
    if (status != CSL_SOK)
    {
        //printf("IRQ_setVecs() fail, status=%d\n", status);
        return status;
    }

    // Reset all peripherals
    CSL_SYSCTRL_REGS->PSRCR = 0x0020;
    CSL_SYSCTRL_REGS->PRCR = 0x00BF;
    do
    {
        prcr = CSL_SYSCTRL_REGS->PRCR;
    } while (prcr != 0);

    // Set PLL output frequency
    // Note: desired frequency must be allowed for core voltage setting
    status = pll_sample(PLL_MHZ);
    if (status != CSL_SOK)
    {
        printf("pll_sample() fail, status=%d\n", status);
        return status;
    }

    // Turn off USB LDO
    UsbLdoSwitch(0);
    return CSL_SOK;
}

//*****************************************************************************
// UsbLdoSwitch
//*****************************************************************************
//  Description: Enables/disables USB LDO
//
//  Arguments:
//      mode:   0 - disable USB LDO
//              1 - enable USB LDO
//              other - no change
//
//  Return:
//      none
//
//*****************************************************************************
void UsbLdoSwitch(Uint16 mode)
{
    Uint16 pcgcr2;
    pcgcr2 = CSL_SYSCTRL_REGS->PCGCR2; // save PCGCR2

    /* Enable the Analog Register only */
    CSL_SYSCTRL_REGS->PCGCR2 &= ~(0x0040);

    if (mode == 0)
    {
        /* Disable USB LDO (clear bit 0) */
        CSL_SAR_REGS->USBLDOCNTL &= 0xFFFE;
    }

    if (mode == 1)
    {
        /* Enable USB LDO (set bit 0) */
        CSL_SAR_REGS->USBLDOCNTL |= 0x0001;
    }

    CSL_SYSCTRL_REGS->PCGCR2 = pcgcr2; // restore PCGCR2
}

//*****************************************************************************
// DmaInit
//*****************************************************************************
//  Description: Initializes DMA, enables DMA interrupts
//
//  Arguments:
//      none
//
//  Return:
//      CSL_Status: CSL_SOK - DMA initialization successful
//                  other   - DMA initialization unsuccessful
//
//*****************************************************************************
CSL_Status DmaInit(void)
{
    Uint16 ifrValue;
    CSL_Status status;

    // Disable DMA interrupt
    IRQ_disable(DMA_EVENT);

    // Clear DMA interrupt
    IRQ_clear(DMA_EVENT);

    // Disable DMA interrupts
    CSL_SYSCTRL_REGS->DMAIER = 0x0000;

    // Clear pending DMA interrupts
    ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
    CSL_SYSCTRL_REGS->DMAIFR |= ifrValue;

    // Plug in DMA ISR
    IRQ_plug(DMA_EVENT, &DmaIsr);

    // DMA initialization
    status = DMA_init();
    if (status != CSL_SOK)
    {
        //printf("DMA_init() fail, status=%d\n", status);
        return status;
    }

    // Enable DMA interrupt
    IRQ_enable(DMA_EVENT);

    return CSL_SOK;
}

//*****************************************************************************
// GpioInit
//*****************************************************************************
//  Description: Initializes GPIO module
//
//  Arguments:
//      hGpio       - handle to GPIO (global)
//      gGpioObj    - GPIO parameter structure (global)
//      ioDir       - GPIO direction
//      intEn       - GPIO interrupt enables
//      intEdg      - GPIO edge
//
//  Return:
//      CSL_Status: CSL_SOK - GPIO module initialization successful
//                  other   - GPIO module initialization unsuccessful
//
//*****************************************************************************
CSL_Status GpioInit(
    Uint32 ioDir,
    Uint32 intEn,
    Uint32 intEdg
)
{
    CSL_Status status;
    CSL_GpioConfig config;

    /* Open GPIO module */
    hGpio = GPIO_open(&gGpioObj, &status);
    if ((hGpio == NULL) || (status != CSL_SOK))
    {
        return status;
    }

    /* Reset GPIO module */
    status = GPIO_reset(hGpio);
    if (status != CSL_SOK)
    {
        return status;
    }

    /* Configure GPIO module */
    config.GPIODIRL = ioDir & 0xFFFF;
    config.GPIODIRH = ioDir >> 16;
    config.GPIOINTENAL = intEn & 0xFFFF;
    config.GPIOINTENAH = intEn >> 16;
    config.GPIOINTTRIGL = intEdg & 0xFFFF;
    config.GPIOINTTRIGH = intEdg >> 16;
    status = GPIO_config(hGpio, &config);
    if (status != CSL_SOK)
    {
        return status;
    }

    return CSL_SOK;
}

//*****************************************************************************
// I2sDmaInit
//*****************************************************************************
//  Description:
//      Configures and starts I2S with DMA
//
//  Parameters:
//      hI2s                - I2S handle (global)
//      dmaLeftTxHandle     - handle for I2S left DMA (global)
//      dmaRightTxHandle    - handle for I2S right DMA (global)
//
//      CSL_Status: CSL_SOK - I2S & DMA initialization successful
//                  other   - I2S & DMA initialization unsuccessful
//
//*****************************************************************************
CSL_Status I2sDmaInit(void)
{
    I2S_Config hwConfig;
    CSL_DMA_Config dmaConfig;
    CSL_Status status;

    // Set Pinmux for I2S1
    status = SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,
                         CSL_EBSR_SP1MODE_1);

    /* DMA engine initialization */
    /* Open the device with instance 1 */
    hI2s2 = I2S_open(I2S_INSTANCE_2, DMA_POLLED, I2S_CHAN_STEREO);
    if(NULL == hI2s2)
    {
        status = CSL_ESYS_FAIL;
        return (status);
    }
    else
    {
        printf ("I2S1 Module Instance opened successfully\n");
    }

    /* Set the value for the configure structure */
    hwConfig.dataFormat     = I2S_DATAFORMAT_LJUST;
    hwConfig.dataType       = I2S_STEREO_ENABLE;
    hwConfig.loopBackMode   = I2S_LOOPBACK_DISABLE;
    hwConfig.fsPol          = I2S_FSPOL_HIGH; //I2S_FSPOL_LOW;
    hwConfig.clkPol         = I2S_RISING_EDGE;//I2S_FALLING_EDGE;
    hwConfig.datadelay      = I2S_DATADELAY_ONEBIT;
    hwConfig.datapack       = I2S_DATAPACK_ENABLE; //I2S_DATAPACK_DISABLE;
    hwConfig.signext        = I2S_SIGNEXT_DISABLE;
    hwConfig.wordLen        = I2S_WORDLEN_16;
    hwConfig.i2sMode        = I2S_SLAVE;
    hwConfig.clkDiv         = I2S_CLKDIV2; // don't care for slave mode
    hwConfig.fsDiv          = I2S_FSDIV32; // don't care for slave mode
    hwConfig.FError         = I2S_FSERROR_DISABLE;
    hwConfig.OuError        = I2S_OUERROR_DISABLE;

    /* Configure hardware registers */
    status = I2S_setup(hI2s2, &hwConfig);
    if(status != CSL_SOK)
    {
        return (status);
    }
    else
    {
        printf ("I2S1 Module Configured successfully\n");
    }

    // Set Pinmux for I2S2 and UART
    status = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                                      CSL_EBSR_PPMODE_1);

#ifdef INSTANCE1_I2S
    /* Configure DMA channel 12 for I2S1 left channel read */
    dmaConfig.pingPongMode  = CSL_DMA_PING_PONG_ENABLE;
    dmaConfig.autoMode      = CSL_DMA_AUTORELOAD_ENABLE;
    dmaConfig.burstLen      = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger       = CSL_DMA_EVENT_TRIGGER;
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S1_RX;
    dmaConfig.dmaInt        = CSL_DMA_INTERRUPT_ENABLE;
    dmaConfig.chanDir       = CSL_DMA_READ;
    dmaConfig.trfType       = CSL_DMA_TRANSFER_IO_MEMORY;

#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.dataLen       = I2S_DMA_BUF_LEN*4*SYS_FS_RATIO; // frame buffer size in bytes
#else
    dmaConfig.dataLen       = I2S_DMA_BUF_LEN*4; // frame buffer size in bytes
#endif

    dmaConfig.srcAddr      = (Uint32)&CSL_I2S1_REGS->I2SRXLT0;
#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.destAddr       = (Uint32)i2sDmaReadBufLeftIn;
#else
    dmaConfig.destAddr       = (Uint32)i2sDmaReadBufLeft;
#endif
    /* Open DMA ch12 for I2S1 left channel read */
    dmaLeftRxHandle2 = DMA_open(DMA_CHAN_RX_L2, &dmaObj4, &status);
    if (dmaLeftRxHandle2 == NULL)
    {
        printf("DMA_open CH12 Failed \n");
        dmaLeftRxHandle2 = NULL;
    }

    /* Configure DMA channel12 */
    status = DMA_config(dmaLeftRxHandle2, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config CH12 Failed \n");
        dmaLeftRxHandle = NULL;
    }

    /* Configure DMA ch13 for I2S1 right channel read */
    dmaConfig.pingPongMode = CSL_DMA_PING_PONG_ENABLE;
    dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_ENABLE;
    dmaConfig.burstLen     = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S1_RX;
    dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_ENABLE;
    dmaConfig.chanDir      = CSL_DMA_READ;
    dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.dataLen      = I2S_DMA_BUF_LEN*4*SYS_FS_RATIO; // frame buffer size in bytes
#else
    dmaConfig.dataLen      = I2S_DMA_BUF_LEN*4; // frame buffer size in bytes
#endif
    dmaConfig.srcAddr     = (Uint32)&CSL_I2S1_REGS->I2SRXRT0;
#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.destAddr      = (Uint32)i2sDmaReadBufRightIn;
#else
    dmaConfig.destAddr      = (Uint32)i2sDmaReadBufRight;
#endif
    /* Open DMA ch13 for I2S1 right channel read */
    dmaRightRxHandle2 = DMA_open(DMA_CHAN_RX_R2, &dmaObj5, &status);
    if (dmaRightRxHandle2 == NULL)
    {
        printf("DMA_open CH13 Failed \n");
        dmaRightRxHandle2 = NULL;
    }

    /* Configure DMA channel13 */
    status = DMA_config(dmaRightRxHandle2, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config CH13 Failed \n");
        dmaRightRxHandle2 = NULL;
    }
#endif


    return CSL_SOK;
}


// last DMA transfer type
Bool lastDmaType;

/* User defined algorithm */
void UserAlgorithm(void)
{
    Uint16 i, offset;

    CSL_Status status = 0;
    short *tempPtrSrcL, *tempPtrDstL, *tempPtrSrcR, *tempPtrDstR;

    if (dmaRxFrameCount >= NUM_OF_MICS)
    {
        dmaRxFrameCount=0;

        // get last DMA transfer type
        lastDmaType = DMA_getLastTransferType(dmaLeftRxHandle, &status);
        if (status != CSL_SOK)
        {
            printf("I2S Dma Left Rx Get Type Failed!!\n");
            exit(EXIT_FAILURE);
        }

        // set offset for data copy
        if (lastDmaType==0)
            offset = 0;     // data is in Ping buffer
        else
#ifdef ENABLE_AUDIO_CONVERSION
            offset = I2S_DMA_BUF_LEN*SYS_FS_RATIO/2;   // data is in Pong buffer
#else
            offset = I2S_DMA_BUF_LEN/2;   // data is in Pong buffer
#endif

        /* --- Insert algorithm here --- */
#ifdef LOOPBACK_ONLY
#ifdef ENABLE_AUDIO_CONVERSION
        for (i=0; i<I2S_DMA_BUF_LEN*SYS_FS_RATIO/2; i++)
#else
        for (i=0; i<I2S_DMA_BUF_LEN/2; i++)
#endif //ENABLE_AUDIO_CONVERSION
        {
            // i2sDmaReadBufLeft (I2S2 left) - mic1 */
            // i2sDmaReadBufRight (I2S2 right) - mic2 */
            // i2sDmaReadBufLeft2 (I2S3 left) - mic4 */
            // i2sDmaReadBufRight2 (I2S3 right) - mic3 */
            // i2sDmaReadBufLeft3 (I2S0 left) - mic5 */
            // i2sDmaReadBufRight3 (I2S0 right) - mic6 */
#ifdef INSTANCE0_I2S
            // loopback mic1 and mic2
           ///i2sDmaWriteBufLeft[offset+i]  = i2sDmaReadBufLeft[offset+i];
           ///i2sDmaWriteBufRight[offset+i] = i2sDmaReadBufRight[offset+i];

            // loopback mic3 and mic4
           ///i2sDmaWriteBufLeft[offset+i]  = i2sDmaReadBufRight2[offset+i];
           ///i2sDmaWriteBufRight[offset+i] = i2sDmaReadBufLeft2[offset+i];

           // loopback mic5 and mic6
#ifdef ENABLE_AUDIO_CONVERSION
            i2sDmaWriteBufLeftOut[offset+i]  = i2sDmaReadBufLeft3In[offset+i];
            i2sDmaWriteBufRightOut[offset+i] = i2sDmaReadBufRight3In[offset+i];
#else
            i2sDmaWriteBufLeft[offset+i]  = i2sDmaReadBufLeft3[offset+i];
            i2sDmaWriteBufRight[offset+i] = i2sDmaReadBufRight3[offset+i];
#endif //ENABLE_AUDIO_CONVERSION
#else
#ifdef INSTANCE3_I2S
             // loopback mic1 and mic2
            ///i2sDmaWriteBufLeft[offset+i]  = i2sDmaReadBufLeft[offset+i];
            ///i2sDmaWriteBufRight[offset+i] = i2sDmaReadBufRight[offset+i];

             // loopback mic3 and mic4
#ifdef ENABLE_AUDIO_CONVERSION
            i2sDmaWriteBufLeftOut[offset+i]  = i2sDmaReadBufLeft2In[offset+i];
            i2sDmaWriteBufRightOut[offset+i] = i2sDmaReadBufRight2In[offset+i];
#else
            i2sDmaWriteBufLeft[offset+i]  = i2sDmaReadBufLeft2[offset+i];
            i2sDmaWriteBufRight[offset+i] = i2sDmaReadBufRight2[offset+i];
#endif //ENABLE_AUDIO_CONVERSION
#else
             // loopback mic1 and mic2
#ifdef ENABLE_AUDIO_CONVERSION
            i2sDmaWriteBufLeftOut[offset+i]  = i2sDmaReadBufLeftIn[offset+i];
            i2sDmaWriteBufRightOut[offset+i] = i2sDmaReadBufRightIn[offset+i];
#else

            i2sDmaWriteBufLeft[offset+i]  = i2sDmaReadBufLeft[offset+i];
            i2sDmaWriteBufRight[offset+i] = i2sDmaReadBufRight[offset+i];
#endif //ENABLE_AUDIO_CONVERSION
#endif

#endif
        }
#else   // BF+ASNR+MSS signal processing
        // i2sDmaReadBufLeft (I2S2 left) - mic1 */
        // i2sDmaReadBufRight (I2S2 right) - mic2 */
        // i2sDmaReadBufLeft2 (I2S3 left) - mic4 */
        // i2sDmaReadBufRight2 (I2S3 right) - mic3 */
        // i2sDmaReadBufLeft3 (I2S0 left) - mic5 */
        // i2sDmaReadBufRight3 (I2S0 right) - mic6 */

#ifdef ENABLE_AUDIO_CONVERSION
        // downsample from 48Khz to 16 Khz by taking 1 sample out of 3
        tempPtrSrcL = (short *)&i2sDmaReadBufLeftIn[offset];
        tempPtrDstL = (short *)&i2sDmaReadBufLeft[offset];
        tempPtrSrcR = (short *)&i2sDmaReadBufRightIn[offset];
        tempPtrDstR = (short *)&i2sDmaReadBufRight[offset];
        for (i=0; i<I2S_DMA_BUF_LEN; i++)
        {
            *tempPtrDstL = *tempPtrSrcL;
            *tempPtrDstR = *tempPtrSrcR;
            tempPtrDstL++;
            tempPtrDstR++;
            tempPtrSrcL += SYS_FS_RATIO;
            tempPtrSrcR += SYS_FS_RATIO;
        }
#endif //ENABLE_AUDIO_CONVERSION
        // set up the input frame pointers
        // set up the input frame pointers
#ifdef C5545BP_2MIC
        inFramePtr[0] = (int *)(&i2sDmaReadBufLeft[offset]);
        inFramePtr[1] = (int *)(&i2sDmaReadBufRight[offset]);
#else
        inFramePtr[0] = (int *)(&i2sDmaReadBufLeft[offset]);
        inFramePtr[1] = (int *)(&i2sDmaReadBufRight[offset]);
#endif


#ifdef INSTANCE3_I2S

#ifndef C5545BP_2MIC
        inFramePtr[2] = (int *)(&i2sDmaReadBufRight2[offset]);
        inFramePtr[3] = (int *)(&i2sDmaReadBufLeft2[offset]);
#endif

#endif

#ifdef INSTANCE0_I2S
#ifdef ENABLE_AUDIO_CONVERSION
        // downsample from 48Khz to 16 Khz by taking 1 sample out of 3
        tempPtrSrcL = (short *)&i2sDmaReadBufLeft3In[offset];
        tempPtrDstL = (short *)&i2sDmaReadBufLeft3[offset];
        tempPtrSrcR = (short *)&i2sDmaReadBufRight3In[offset];
        tempPtrDstR = (short *)&i2sDmaReadBufRight3[offset];
        for (i=0; i<I2S_DMA_BUF_LEN; i++)
        {
            *tempPtrDstL = *tempPtrSrcL;
            *tempPtrDstR = *tempPtrSrcR;
            tempPtrDstL++;
            tempPtrDstR++;
            tempPtrSrcL += SYS_FS_RATIO;
            tempPtrSrcR += SYS_FS_RATIO;
        }
#endif //ENABLE_AUDIO_CONVERSION
        inFramePtr[2] = (int *)(&i2sDmaReadBufLeft3[offset]);
        inFramePtr[3] = (int *)(&i2sDmaReadBufRight3[offset]);
#endif
        // set up the processed audio output to left ear
        buf_Outptr = (int *)(&i2sDmaWriteBufLeft[offset]);
        // set up the processed audio output to right ear
        buf_Outptr2 = (int *)(&i2sDmaWriteBufRight[offset]);

        // process next frame
        bf_process();

#if 0   // upsample from 16Khz to 48 Khz by repeating 1 sample 3 times. This is only used if the audio needs to be output to a codec for playback (e.g headphones).
        //Since this demo sends audio to the cloud, this step isn't necessary.
#ifdef ENABLE_AUDIO_CONVERSION
        tempPtrSrcL = (short *)&i2sDmaReadBufLeft[offset];
        tempPtrDstL = (short *)&i2sDmaWriteBufLeftOut[offset];
        tempPtrSrcR = (short *)&i2sDmaReadBufRight[offset];
        tempPtrDstR = (short *)&i2sDmaWriteBufRightOut[offset];
        for (i=0; i<I2S_DMA_BUF_LEN; i++)
        {
            for (j=0; j<SYS_FS_RATIO; j++)
            {
                *tempPtrDstL = *tempPtrSrcL;
                *tempPtrDstR = *tempPtrSrcR;
                tempPtrDstL++;
                tempPtrDstR++;
            }
            tempPtrSrcL++;
            tempPtrSrcR++;
        }
#endif //ENABLE_AUDIO_CONVERSION
#endif
        //sensory recognition
        AudioDataCollection((Int16 *) buf_Outptr); //Send to keyword recognition. Clean audio.

        //Send SEM for UART
        SEM_post(&SEM_UARTdataTransfer);

#endif
    }


    /* Clear DMA frame count */
    if(dmaTxFrameCount>=NUM_OF_OUTPUT_CH)
    {
        dmaTxFrameCount = 0;
    }

}

// Put CPU in idle
void UserIdle(void)
{
    /* Set CPUI bit in ICR */
    //*(volatile ioport Uint16 *)(0x0001) |= 0x000F;
    /* Set CPUI, DPORTI, XPORTI, and IPORTI in ICR */
    //*(volatile ioport Uint16 *)(0x0001) |= 0x016F;

    /* Execute idle instruction */
    asm("    idle");
}

volatile Uint32 dmaIntCount = 0;
volatile Uint32 dmaSemCount = 0;

// DMA ISR
long dmatxLcount =0;
long dmatxRcount =0;

long dmarxLcount =0;
long dmarxRcount =0;

interrupt void DmaIsr(void)
{
    volatile Uint16 ifrValue;


#ifdef PROFILE_CYCLES
      statusInt = GPT_getCnt(hGpt, &timeCntInt1);;
#endif
    /* Clear the DMA interrupt */
    ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
    CSL_SYSCTRL_REGS->DMAIFR |= ifrValue;

#ifdef INSTANCE1_I2S
    /* Check for DMA DMA_CHAN_RX_L2 transfer completion */
    if (ifrValue & (1<<DMA_CHAN_RX_L2))
    {
        dmaRxFrameCount++;
    }

    /* Check for DMA DMA_CHAN_RX_R2 transfer completion */
    if (ifrValue & (1<<DMA_CHAN_RX_R2))
    {
        dmaRxFrameCount++;
    }
#endif

    /* Check for UART RX DMA transfer completion */
        if (ifrValue & (1<<CSL_DMA_CHAN5))
        {
          SEM_post(&SEM_uartrx_transferDone);
          dmaSemCount++;
        }

    // wait up the audio task when both RX and TX DMAs are done
    if (dmaRxFrameCount>=NUM_OF_MICS)
    {
        SEM_post(&SEM_i2s_dmaTransferDone);
        dmaSemCount++;
    }

    dmaIntCount++;
#ifdef PROFILE_CYCLES
      statusInt = GPT_getCnt(hGpt, &timeCntInt2);
      process_cycles((timeCntInt1-timeCntInt2)<<(hwConfig.preScaleDiv+1), &cyclesDmaInt);
#endif
}

Uint32             dmatimeCnt1;
Uint32             dmatimeCnt2;
Uint32             dmatimeCnt;

CSL_Status CSL_uartInit(void)
{

      CSL_Status        status;
      //Uint16            stringSize;
       Uint32            sysClk;

      //Uint32            delay;
      volatile Uint32   i;

      // reset the guartDmaWriteBuf
      for (i=4; i<(I2S_DMA_BUF_LEN +2)*2; i++)
          guartDmaWriteBuf[i] = (Uint32)(0);

      // get system clock in HZ
      sysClk = getUartSysClk();

      uartSetup.clkInput = sysClk;

      /* Loop counter and error flag */
      status = UART_init(&uartObj,CSL_UART_INST_0,UART_POLLED);
      if(CSL_SOK != status)
      {
             printf("UART_init failed error code %d\n",status);
             return(status);
      }
      else
      {
             //printf("UART_init Successful\n");
      }


      /* Handle created */
      hUart = (CSL_UartHandle)(&uartObj);

    /* Configure the DMA channel for UART transmit */
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5517) || defined(CHIP_C5535) || defined(CHIP_C5545))
      dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
      dmaConfig.autoMode = CSL_DMA_AUTORELOAD_DISABLE;
      dmaConfig.burstLen = CSL_DMA_TXBURST_4WORD;
      dmaConfig.trigger  = CSL_DMA_EVENT_TRIGGER;
      dmaConfig.dmaEvt   = CSL_DMA_EVT_UART_TX;
      dmaConfig.dmaInt   = CSL_DMA_INTERRUPT_ENABLE;
      dmaConfig.chanDir  = CSL_DMA_WRITE;
      dmaConfig.trfType  = CSL_DMA_TRANSFER_IO_MEMORY;
      dmaConfig.dataLen  = (I2S_DMA_BUF_LEN +2)*4*2;
      dmaConfig.srcAddr  = (Uint32)guartDmaWriteBuf;
      dmaConfig.destAddr = (Uint32)&(hUart->uartRegs->THR);

    dmaWrHandle = CSL_configDmaForUart(&dmaWrChanObj, CSL_DMA_CHAN4);
      if(dmaWrHandle == NULL)
      {
             printf("DMA Config for Uart Write Failed!\n!");
             return(CSL_TEST_FAILED);
      }

    /* Start Dma transfer */
      status = DMA_start(dmaWrHandle);
      if(status != CSL_SOK)
      {
             printf("Uart Dma Write Failed!!\n");
             return(status);
      }

      /* Configure UART registers using setup structure */
      status = UART_setup(hUart,&uartSetup);
      if(CSL_SOK != status)
      {
             printf("UART_setup failed error code %d\n",status);
             return(status);
      }
      else
      {
             printf("UART_setup Successful\n");
      }

#ifdef PROFILE_CYCLES
      status = GPT_getCnt(hGpt, &dmatimeCnt1);;
#endif
      /* Check transfer complete status */
      while(DMA_getStatus(dmaWrHandle));

#ifdef PROFILE_CYCLES
      status = GPT_getCnt(hGpt, &dmatimeCnt2);
      dmatimeCnt = dmatimeCnt1-dmatimeCnt2;
#endif

#if 1
    /* Configure the DMA channel for UART receive */
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5517) || defined(CHIP_C5535) || defined(CHIP_C5545))
      dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
      dmaConfig.autoMode = CSL_DMA_AUTORELOAD_DISABLE;
      dmaConfig.burstLen = CSL_DMA_TXBURST_1WORD;
      dmaConfig.trigger  = CSL_DMA_EVENT_TRIGGER;
      dmaConfig.dmaEvt   = CSL_DMA_EVT_UART_RX;
      dmaConfig.dmaInt   = CSL_DMA_INTERRUPT_ENABLE;
      dmaConfig.chanDir  = CSL_DMA_READ;
      dmaConfig.trfType  = CSL_DMA_TRANSFER_IO_MEMORY;
      dmaConfig.dataLen  = UART_RX_BUF_SIZE*4;
      dmaConfig.srcAddr  = (Uint32)&(hUart->uartRegs->THR);
      dmaConfig.destAddr = (Uint32)guartDmaReadBuf;

      dmaRdHandle = CSL_configDmaForUart(&dmaRdChanObj,CSL_DMA_CHAN5);
      if(dmaRdHandle == NULL)
      {
             printf("DMA Config for DMA Read Failed!\n!");
             return(CSL_TEST_FAILED);
      }

    /* Start Dma transfer */
    status = DMA_start(dmaRdHandle);
      if(status != CSL_SOK)
      {
        printf("Uart Dma Write Failed!!\n");
        return(status);
      }
      // Reset UART Rx
      CSL_FINST(hUart->uartRegs->PWREMU_MGMT,UART_PWREMU_MGMT_URRST,RESET);
      asm(" NOP");
      CSL_FINST(hUart->uartRegs->PWREMU_MGMT,UART_PWREMU_MGMT_URRST,ENABLE);

      /* Check transfer complete status */
      //while(DMA_getStatus(dmaRdHandle));
#endif

      return(CSL_SOK);
}

/**
*  \brief  Function to calculate the clock at which system is running
*
*  \param    none
*
*  \return   System clock value in Hz
*/
Uint32 getUartSysClk(void)
{
       Bool      pllRDBypass;
       Bool      pllOutDiv;
       Uint32    sysClk;
       Uint16    pllM;
       Uint16    pllRD;
       Uint16    pllOD;

       pllM = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR1, SYS_CGCR1_M);

       pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDRATIO);
       pllOD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_ODRATIO);

       pllRDBypass = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDBYPASS);
       pllOutDiv   = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_OUTDIVEN);

       sysClk = CSL_PLL_CLOCKIN;

       if (0 == pllRDBypass)
       {
              sysClk = sysClk/(pllRD + 4);
       }

       sysClk = (sysClk * (pllM + 4));

       if (1 == pllOutDiv)
       {
              sysClk = sysClk/(pllOD + 1);
       }

       /* Return the value of system clock in Hz */
       return(sysClk);
}

long uartDmaCount = 0;
long uartDmaErrCount = 0;
void audioLoopUartsend()
{
    char c;
    int i, j;
    unsigned int samp;
    CSL_Status        status;

    while (1)
    {
      // wait for the I2S DMA is done
      SEM_pend(&SEM_UARTdataTransfer, SYS_FOREVER);

      uartDmaCount++;

      // Check the status of the UART TX DMA
      // The previous DMA transaction should have been completed by now, if not report error
      if (DMA_getStatus(dmaWrHandle))
      {
          uartDmaErrCount++;
         // skip this audio frame transfer
          continue;
      }
#ifdef UART_OUTPUT_DEBUG
      Uint16 tempWord;
#endif
      if(start == 1)
      {
      // convert the audio data into 32 bit word per byte and fill in the UART TX buffer
          for (i=2, j=0; i<(I2S_DMA_BUF_LEN +2); i++, j++)
          {
              // get a sample
              samp = buf_Outptr[j];
              // get the high 8 bit of the sample
              c = (samp>>8)&0xFF;
#ifdef UART_OUTPUT_DEBUG
              tempWord = (c<<8);
#endif
              guartDmaWriteBuf[i*2] = (Uint32)c;
              // get the low 8 bit of the sample
              c = samp&0xFF;
#ifdef UART_OUTPUT_DEBUG
              tempWord |= c;
#endif
              guartDmaWriteBuf[i*2+1] = (Uint32)c;
#ifdef UART_OUTPUT_DEBUG
              uartdumpbuf[uartidx++] = tempWord;
              if (uartidx>=UART_DUMP_BUF_SIZE)
                  uartidx = 0;
#endif
          }


                /* Configure the DMA channel for UART transmit */
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5517) || defined(CHIP_C5535) || defined(CHIP_C5545))
      dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
      dmaConfig.autoMode = CSL_DMA_AUTORELOAD_DISABLE;
      dmaConfig.burstLen = CSL_DMA_TXBURST_4WORD;
      dmaConfig.trigger  = CSL_DMA_EVENT_TRIGGER;
      dmaConfig.dmaEvt   = CSL_DMA_EVT_UART_TX;
      dmaConfig.dmaInt   = CSL_DMA_INTERRUPT_ENABLE;
      dmaConfig.chanDir  = CSL_DMA_WRITE;
      dmaConfig.trfType  = CSL_DMA_TRANSFER_IO_MEMORY;
      dmaConfig.dataLen  = (I2S_DMA_BUF_LEN+2)*4*2;
      dmaConfig.srcAddr  = (Uint32)guartDmaWriteBuf;
      dmaConfig.destAddr = (Uint32)&(hUart->uartRegs->THR);

      /* Configure a Dma channel */
      status = DMA_config(dmaWrHandle, &dmaConfig);
      if(status != CSL_SOK)
      {
        printf("DMA_config Failed!\n");
      }
      // start the UART TX DMA
      status = DMA_start(dmaWrHandle);
      if(status != CSL_SOK)
      {
         printf("Uart Dma Write Failed!!\n");
      }

      // Reset UART Tx to create TX event to start DMA
      CSL_FINST(hUart->uartRegs->PWREMU_MGMT,UART_PWREMU_MGMT_UTRST,RESET);
      asm(" NOP");
      CSL_FINST(hUart->uartRegs->PWREMU_MGMT,UART_PWREMU_MGMT_UTRST,ENABLE);

      }// end if start=1
    }
}

///////////////////////////////////////////////////////

/**
*  \brief  Configures Dma
*
*  \param  chanNum - Dma channel number
*
*  \return Dma handle
*/
CSL_DMA_Handle CSL_configDmaForUart(CSL_DMA_ChannelObj    *dmaChanObj,
                                    CSL_DMAChanNum        chanNum)
{
      CSL_DMA_Handle    dmaHandle;
      CSL_Status        status;

      dmaHandle = NULL;

      /* Open A Dma channel */
      dmaHandle = DMA_open(chanNum, dmaChanObj, &status);
    if(dmaHandle == NULL)
    {
        printf("DMA_open Failed!\n");
    }

      /* Configure a Dma channel */
      status = DMA_config(dmaHandle, &dmaConfig);
    if(status != CSL_SOK)
    {
        printf("DMA_config Failed!\n");
        dmaHandle = NULL;
    }

    return(dmaHandle);
}
char uartRxBufChar[UART_RX_BUF_SIZE];
int uartRxBufCharLen = 0;
Uint32 uartRxCount = 0;
void uartRxTask(void)
{
    int i;
    CSL_Status        status;
    char tmpChar;

    while (1)
    {
      // wait for the UART RX is done
      SEM_pend(&SEM_uartrx_transferDone, SYS_FOREVER);
      // UART RX count increase
      uartRxCount++;

      // copy/convert the UART RX string from UART DMA RX buffer to char buffer
      uartRxBufCharLen = 0;
      for (i=0; i<UART_RX_BUF_SIZE; i++)
      {
          tmpChar = guartDmaReadBuf[i]&0xFF;
          if ((tmpChar==0x00)||(tmpChar==0x0D)||(tmpChar==0x0A))
          {
              // terminate the string by '\0'
              uartRxBufChar[i] = 0;
              break;
          }
          uartRxBufChar[i] = tmpChar;
      }
      uartRxBufCharLen = i+1;


      printf("Transcript returned from cloud transcription engine:\n");
      printf(">>>>>>%s\n", uartRxBufChar);

      //reset start flag
      start = 0;
      // display string on OLED screen
      SEM_post(&SEM_OLEDTransfer);

      // start the next UART DMA RX transfer
    /* Configure the DMA channel for UART receive */
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5517) || defined(CHIP_C5535) || defined(CHIP_C5545))
      dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
      dmaConfig.autoMode = CSL_DMA_AUTORELOAD_DISABLE;
      dmaConfig.burstLen = CSL_DMA_TXBURST_1WORD;
      dmaConfig.trigger  = CSL_DMA_EVENT_TRIGGER;
      dmaConfig.dmaEvt   = CSL_DMA_EVT_UART_RX;
      dmaConfig.dmaInt   = CSL_DMA_INTERRUPT_ENABLE;
      dmaConfig.chanDir  = CSL_DMA_READ;
      dmaConfig.trfType  = CSL_DMA_TRANSFER_IO_MEMORY;
      dmaConfig.dataLen  = UART_RX_BUF_SIZE*4;
      dmaConfig.srcAddr  = (Uint32)&(hUart->uartRegs->THR);
      dmaConfig.destAddr = (Uint32)guartDmaReadBuf;

      /* Configure UART RX Dma channel */
      status = DMA_config(dmaRdHandle, &dmaConfig);
      if(status != CSL_SOK)
      {
        printf("DMA_config Failed!\n");
        dmaRdHandle = NULL;
      }

      /* Start Dma transfer */
      status = DMA_start(dmaRdHandle);
      if(status != CSL_SOK)
      {
        printf("Uart Dma Read Failed!!\n");
        return;
      }
      // Reset UART Rx
      CSL_FINST(hUart->uartRegs->PWREMU_MGMT,UART_PWREMU_MGMT_URRST,RESET);
      asm(" NOP");
      CSL_FINST(hUart->uartRegs->PWREMU_MGMT,UART_PWREMU_MGMT_URRST,ENABLE);
    }
}

//Sends the string recieved from UART and displays it on the OLED screen
void Oledsend()
{
    //Wait until uart data is received, then proceed to display string
    while(1){
        SEM_pend(&SEM_OLEDTransfer, SYS_FOREVER);
        oledInit();
        clear();
        setline(0);
        setOrientation(1);
        printstr(uartRxBufChar);
        C55x_delay_msec(1000);
        scrollDisplayLeft();
    }
}

