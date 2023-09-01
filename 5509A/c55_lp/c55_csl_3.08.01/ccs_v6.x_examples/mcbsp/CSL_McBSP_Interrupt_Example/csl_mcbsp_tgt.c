#define GPIO_DIR_REG0 0x1C06
#define GPIO_DOUT_REG0 0x1C0A

#include "csl_mcbsp_tgt.h"

 // Below function added by Chandra for configuring the MCBSP target
 // MCBSP target's config port is connected to Serial Port 1 (S1).
 // Steps involved here are
 //     1. Configure Serial port S1 as GPIO (gpio[9:6]; lower 4 pins of S1)
 //     2. Configure MCBSP Target with the below options by writing into the GPIO
 //   ==========================================================================================================
 //   MCBSP Target Mode     | GPIO Data | Data          |  Frame Sync |   Clock       | External Clock         |
 //                         | [9:6]     | Loop back     | Loop back   |  Loop back    | Connection             |
 //   ==========================================================================================================
 //   MCBSP_TGT_NO_LB       |  0000     |  No loop back |  --         |  --           |  --                    |
 //   MCBSP_TGT_LB_MODE0    |  0001     |  dx -> dr     |  fsr -> fsx |  clkr -> clkx |  --                    |
 //   MCBSP_TGT_LB_MODE1    |  0011     |  dx -> dr     |  fsx -> fsr |  clkr -> clkx |  --                    |
 //   MCBSP_TGT_LB_MODE2    |  0101     |  dx -> dr     |  fsr -> fsx |  clkx -> clkr |  clkx -> clks          |
 //   MCBSP_TGT_LB_MODE3    |  0111     |  dx -> dr     |  fsx -> fsr |  clkx -> clkr |  clkx -> clks          |
 //   MCBSP_TGT_LB_MODE4    |  1001     |  dx -> dr     |  fsr -> fsx |  clkx -> clkr |  --                    |
 //   MCBSP_TGT_LB_MODE5    |  1011     |  dx -> dr     |  fsx -> fsr |  clkx -> clkr |  --                    |
 //   MCBSP_TGT_LB_MODE6    |  1101     |  dx -> dr     |  fsr -> fsx |  --           |  External Clock -> clks|
 //   MCBSP_TGT_LB_MODE7    |  1111     |  dx -> dr     |  fsx -> fsr |  --           |  External Clock -> clks|
 //   ==========================================================================================================

void MCBSP_target_config(Uint16 TgtMode)
{
   Uint16 k=0;
 ioport volatile CSL_SysRegs *sysRegs;

 sysRegs = (CSL_SysRegs *)CSL_SYSCTRL_REGS;
 CSL_FINS(sysRegs->EBSR,SYS_EBSR_SP1MODE,CSL_SYS_EBSR_SP1MODE_MODE2);
 (*(volatile ioport unsigned *)GPIO_DIR_REG0) = 0x0FC0; 
  for(k=0;k<5;k++) { __asm("    NOP"); }
 (*(volatile ioport unsigned *)GPIO_DOUT_REG0) = TgtMode;
  for(k=0;k<5;k++) { __asm("    NOP"); }
}
