#include "5509.h"
#include "util.h"

// AIC23 Control Register addresses
#define AIC23_LT_LINE_CTL         0x00  // 0
#define AIC23_RT_LINE_CTL         0x02  // 1
#define AIC23_LT_HP_CTL           0x04  // 2
#define AIC23_RT_HP_CTL           0x06  // 3
#define AIC23_ANALOG_AUDIO_CTL    0x08  // 4
#define AIC23_DIGITAL_AUDIO_CTL   0x0A  // 5
#define AIC23_POWER_DOWN_CTL      0x0C  // 6
#define AIC23_DIGITAL_IF_FORMAT   0x0E  // 7
#define AIC23_SAMPLE_RATE_CTL     0x10  // 8
#define AIC23_DIG_IF_ACTIVATE     0x12  // 9
#define AIC23_RESET_REG           0x1E  // F - Writing 0 to this reg triggers reset

// AIC23 Control Register settings
#define lt_ch_vol_ctrl       0x0017   /* 0  */
#define rt_ch_vol_ctrl       0x0017   /* 1  */
#define lt_ch_headph_ctrl    0x0079   /* 2  */
#define rt_ch_headph_ctrl    0x0079   /* 3  */
#define alog_au_path_ctrl    0x0000   /* 4  */
#define digi_au_path_ctrl    0x0000   /* 5  */
#define pow_mgt_ctrl_ctrl    0x0002   /* 6  */
#define digi_au_intf_ctrl    0x000D   /* 7  */
#define au_FS_TIM_ctrl       0x0000   /* 8 MCLK=12MHz, Sample Rate setting */
#define digi_intf1_ctrl      0x0001   /* 9  */
#define digi_intf2_ctrl      0x00FF   /* 10 */

#define DIGIF_FMT_MS              0x40
#define DIGIF_FMT_LRSWAP          0x20
#define DIGIF_FMT_LRP             0x10
#define DIGIF_FMT_IWL             0x0c
#define DIGIF_FMT_FOR             0x03

#define DIGIF_FMT_IWL_16          0x00
#define DIGIF_FMT_IWL_20          0x04
#define DIGIF_FMT_IWL_24          0x08
#define DIGIF_FMT_IWL_32          0xc0

#define DIGIF_FMT_FOR_MSBRIGHT    0x00
#define DIGIF_FMT_FOR_MSLEFT      0x01
#define DIGIF_FMT_FOR_I2S         0x02
#define DIGIF_FMT_FOR_DSP         0x03

#define POWER_DEV                 0x80
#define POWER_CLK                 0x40
#define POWER_OSC                 0x20
#define POWER_OUT                 0x10
#define POWER_DAC                 0x08
#define POWER_ADC                 0x04
#define POWER_MIC                 0x02
#define POWER_LINE                0x01

#define SRC_CLKOUT                0x80
#define SRC_CLKIN                 0x40
#define SRC_SR                    0x3c
#define SRC_BOSR                  0x02
#define SRC_MO                    0x01

#define SRC_SR_44                 0x20
#define SRC_SR_32                 0x18

#define ANAPCTL_STA               0xc0
#define ANAPCTL_STE               0x20
#define ANAPCTL_DAC               0x10
#define ANAPCTL_BYP               0x08
#define ANAPCTL_INSEL             0x04
#define ANAPCTL_MICM              0x02
#define ANAPCTL_MICB              0x01    

#define DIGPCTL_DACM              0x08
#define DIGPCTL_DEEMP             0x06
#define DIGPCTL_ADCHP             0x01
#define DIGPCTL_DEEMP_DIS         0x00
#define DIGPCTL_DEEMP_32          0x02
#define DIGPCTL_DEEMP_44          0x04
#define DIGPCRL_DEEMP_48          0x06

#define DIGIFACT_ACT              0x01

#define LT_HP_CTL_LZC             0x80
#define RT_HP_CTL_RZC             0x80

void AIC23_Write(unsigned short regaddr, unsigned short data)
{
    unsigned char buf[2];
    buf[0] = regaddr;
    buf[1] = data;
    I2C_Write(I2C_AIC23, 2, buf);
}

void McBSP0_InitSlave()
{
    PC55XX_MCSP pMCBSP0 = (PC55XX_MCSP)C55XX_MSP0_ADDR;
    
    // Put the MCBSP in reset
    Write(pMCBSP0 -> spcr1, 0);
    Write(pMCBSP0 -> spcr2, 0);
       
    // Config frame parameters (32 bit, single phase, no delay)
    Write(pMCBSP0 -> xcr1, XWDLEN1_32);
    Write(pMCBSP0 -> xcr2, XPHASE_SINGLE | XDATDLY_0);
    Write(pMCBSP0 -> rcr1, RWDLEN1_32);
    Write(pMCBSP0 -> rcr2, RPHASE_SINGLE | RDATDLY_0);

    // Disable int frame generation and enable slave w/ext frame signals on FSX
    // Frame sync is active high, data clocked on rising edge of clkx
    Write(pMCBSP0 -> pcr, PCR_CLKXP);

    // Bring transmitter and receiver out of reset
    SetMask(pMCBSP0 -> spcr2, SPCR2_XRST);
    SetMask(pMCBSP0 -> spcr1, SPCR1_RRST);    
}
void AIC23_Mixer()
{
    PC55XX_MCSP pMCBSP0 = (PC55XX_MCSP)C55XX_MSP0_ADDR;
    int left, right;
    
    while(1)
    {
        // Wait for transmitter to become free
        while (!ReadMask(pMCBSP0 -> spcr2, SPCR2_XRDY));
            
        // Write the data (left then right)
        left = Read(pMCBSP0 -> ddr2);
        right = Read(pMCBSP0 -> ddr1);
        left = left /2;
        right = right /2;
        Write(pMCBSP0 -> dxr2, left);
        Write(pMCBSP0 -> dxr1, right);           
    }
}

void AIC23_Init()
{
    I2C_Init();
   
    // Reset the AIC23 and turn on all power
    AIC23_Write(AIC23_RESET_REG, 0);
    AIC23_Write(AIC23_POWER_DOWN_CTL, 0);    
    AIC23_Write(AIC23_ANALOG_AUDIO_CTL, ANAPCTL_DAC | ANAPCTL_INSEL);
    AIC23_Write(AIC23_DIGITAL_AUDIO_CTL, 0);
    
    // Turn on volume for line inputs
    AIC23_Write(AIC23_LT_LINE_CTL, 0);
    AIC23_Write(AIC23_RT_LINE_CTL,0);

    // Configure the AIC23 for master mode, 44.1KHz stereo, 16 bit samples
    // Use 12MHz USB clock
    AIC23_Write(AIC23_DIGITAL_IF_FORMAT, DIGIF_FMT_MS | DIGIF_FMT_IWL_16 | DIGIF_FMT_FOR_DSP);
    AIC23_Write(AIC23_SAMPLE_RATE_CTL, SRC_SR_44 | SRC_BOSR | SRC_MO);

    // Turn on headphone volume and digital interface
    AIC23_Write(AIC23_LT_HP_CTL, 0x79);  // 0x79 for speakers
    AIC23_Write(AIC23_RT_HP_CTL, 0x79);
    AIC23_Write(AIC23_DIG_IF_ACTIVATE, DIGIFACT_ACT);

    // Set McBSP0 to be transmit slave
    McBSP0_InitSlave();
}

void AIC23_Disable()
{
    PC55XX_MCSP pMCBSP0 = (PC55XX_MCSP)C55XX_MSP0_ADDR;
    I2C_Disable();
    
    // Put the MCBSP in reset
    Write(pMCBSP0 -> spcr1, 0);
    Write(pMCBSP0 -> spcr2, 0);
}
    

    
  

