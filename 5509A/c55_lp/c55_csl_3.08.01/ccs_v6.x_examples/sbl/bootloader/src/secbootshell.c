/********************************************************************* 
*    Property of Texas Instruments Incorporated, Copyright 2008
*    All rights reserved
**********************************************************************
*    secbootshell.c
**********************************************************************
* Bootloader shell for C5505.  This shell determines the boot mode
* based on available media devices and then proceeds with the secure or
* insecure boot from that device.
**********************************************************************/
#include "machine.h"
#include "soc.h"
#include "sysctrl.h"
#include "secboot.h"
#include "mcspi_eeprom.h"
#include "user_option.h"

#define SECBOOT_ID_SEED_AND_DIEID   0x09A4 /* secure boot image bound to the device */
#define SECBOOT_ID_SEED_ONLY        0x09A5 /* secure boot image NOT bound to the device */
#define SECBOOT_ID_REAUTHOR         0x09A6 /* secure boot image to be REAUTHORED and bound to the device */
#define SECBOOT_ID_INSECURE         0x09AA /* insecure boot image */

// Chip Selelction (C5515 or Phoenix)
//#define C5515
#ifndef C5515
#define PHOENIX
#endif

// define the SARAM start address
#define SARAMSTARTADDRESS	0x10000
// define and assign the bootloader ID to 0xFFFEFC
#ifdef PHOENIX
#ifdef QUICKTURN
#define BOOT_LOADER_ID				0x000300DB // bootloader ID 03.00 for QT debug
#else
#define BOOT_LOADER_ID				0x00030017 // bootloader ID 03.00 for Phoenix (C5517)
#endif // QUICKTURN
#else
#define BOOT_LOADER_ID				0x00010015 // bootloader ID 01.00 for C5515
#endif // PHOENIX
#pragma DATA_SECTION(boot_loader_id, ".bootid")
const Uint32 boot_loader_id = BOOT_LOADER_ID;

// define the EMIF A[20:15] and GPIO[26:21] switch flag
// On reset it is GPIO mode (1). It will need to set to EMIF address mode (0)
// if the EMIF read/write has address over 32KW boundary
Uint16	emifAddrGPIOFlag = 1;
Uint16	Saved_1C17 = 0;
Uint16	Saved_1C18 = 0;
Uint16	Saved_1C19 = 0;
Uint16	Saved_1C4C = 0;
Uint16	Saved_1C4D = 0;
Uint16	Saved_1C4F = 0;
Uint16	Saved_1C50 = 0;

Uint32 retryCount = 0;

static void bootFromMedia(int BootSource);
static int getBootId(int BootSource, UInt16 *BootId);
static void getAppData(int BootSource, UInt16 BootFlags);
static void null_func(void);

void boot_from_mcspi(int div);
void clkx1(void);
void clkx3(void);
void toggle_XF(int flag);

// enable MCSPI related pull ups/downs
void pull_enable_mcspi(void);
// disable MCSPI related pull ups/downs
void pull_disable_mcspi(void);

#define BOOT_SELECT_NAND_FLASH    0
#define BOOT_SELECT_NOR_FLASH     1
#define BOOT_SELECT_SPI_EEPROM    2
#define BOOT_SELECT_IIC_EEPROM    3
#define BOOT_SELECT_MMC_SD        4
#define BOOT_SELECT_UART          5
#define BOOT_SELECT_USB           6
#define BOOT_SELECT_SPI_EEPROM24  7
#define BOOT_SELECT_MCSPI_EEPROM24	8
#define BOOT_SELECT_HPI			  	9
#define BOOT_SELECT_EMMC_BP       	10

// Global Varibles
// the value of General Purpose eFuse Register 0
// bit 7: efuse_polling_bootmode 
// bit 6: Use_BootM_Efuse 
// bit [5:0]: BootM_Efuse[5:0]
Uint16 gpefr0Val = 0;
// the value of Boot Mode Register
Uint16 bmrVal = 0;
// Boot Modes from GPIO pins or EFUSE
Uint16 bootMode = 0;
// input clock
// 0: 11.2896Mhz
// 1: 12Mhz or 12.288Mhz
// 2: 16.8Mhz
// 3: 19.2Mhz
Uint16 clkInput = 0xFFFF;
// clock selction
// 0: use USB OSC
// 1: use external clock
Uint16 clkSelect = 0;

static const BootSourceFunctions_t bootSourceFunctions[] =
{
	{mcspi_eeprom_read_n_words, null_func, null_func}
};


/******************************************************************************
* main
******************************************************************************/
void main()
{
	volatile Uint32 i; 

#ifdef PHOENIX
	/****************************************************************
	* Get the input clock frequency and boot modes.
	****************************************************************/
	// read the GPEFR0
	gpefr0Val = CSL_SYSCTRL_REGS->GPEFR0;
	// read the BMR
	bmrVal = CSL_SYSCTRL_REGS->BMR;
	// get clock select
	clkSelect = CSL_FEXT(CSL_SYSCTRL_REGS->CCR2, SYS_CCR2_CLKSELSTAT);
	// determine the bootmode
	// polling or fixed boot?
	if (CSL_FEXT(gpefr0Val, SYS_GPEFR0_EPBM)==CSL_SYS_GPEFR0_EPBM_PIN)
	{
		// boot from assigned peripheral
		// EFUSE or GPIO pins?
		if (CSL_FEXT(gpefr0Val, SYS_GPEFR0_UBME)==CSL_SYS_GPEFR0_UBME_GPIO)
		{
			// use GPIO pins
			// get the input clock frequency from BMR
			clkInput = CSL_FEXT(bmrVal, SYS_BMR_BMA)>>4;
			// get the boot mode from BMR
			bootMode = CSL_FEXT(bmrVal, SYS_BMR_BMA)&0x000F;
		} else
		{
			// use EFUSE
			// get the input clock frequency from GPEFR0
			clkInput = CSL_FEXT(gpefr0Val, SYS_GPEFR0_BMEF)>>4;
			// get the boot mode from GPEFR0
			bootMode = CSL_FEXT(gpefr0Val, SYS_GPEFR0_BMEF)&0x000F;
		}
	} else
	{
		// boot by polling all the peripherals in order
		// get input clcok frequency from GPEFR0
		clkInput = CSL_FEXT(gpefr0Val, SYS_GPEFR0_BMEF)>>4;
		// set the boot mode to 16 - polling mode 2 (no SD/MMC1, but MCSPI)
		bootMode = 16;
	}

	// adjust the clkInout according to the clkSelect
	if (clkSelect==0)
		clkInput = 1;	// USB OSC is always 12Mhz

#endif

	/****************************************************************
	* Check for MCSPI 24-bit address, 10Mhz boot mode
	****************************************************************/
	// boost the clock to input clock*3
	clkx3();
	for (;;)
	{
		if (clkInput>1)
		{
			// set MCSPI clock to 26Mhz*3/8 (2^3) = 9.75 Mhz
			boot_from_mcspi(3);
		} else
		{
			// set MCSPI clock to 12.288Mhz*3/4 (2^2) = 9.2Mhz
			boot_from_mcspi(2);
		}
		retryCount++;
		if ((retryCount%100)==0)
			toggle_XF(3);
	}
}

/******************************************************************************
* bootFromMedia
******************************************************************************/
static void bootFromMedia(int BootSource)
{
	UInt16 flags;

	/* Attempt to read boot ID */
	if (getBootId(BootSource, (UInt16 *)&flags) != 0)
	{
		/* If read fails, return no entry point */
		return;
	}

	/* Attempt to load and execute the application */
	getAppData(BootSource, flags); // If this function returns, then the load/execute failed

	// if the EBSR have been changed, then restore them
	if (emifAddrGPIOFlag==0)
	{
		// set the EBSR[5:0] to 1 (set those pins for GPIO)
		CSL_SYSCTRL_REGS->EBSR |= 0x003F;
		// set emifAddrGPIOFlag to 1
		emifAddrGPIOFlag = 1;
	}
}

/******************************************************************************
* getBootId
******************************************************************************/
static int getBootId(int BootSource, UInt16 *BootFlags)
{
	volatile UInt16 bootID = 0;

	/* use standard read function, but modify starting address for 24-bit SPI EEPROM */
	if (bootSourceFunctions[0].readNWords(BOOT_BASE_ADDR, (UInt16 *)&bootID, 1) != 0)
	{
		/* Read failed */
		bootSourceFunctions[0].done();
		return 1; // failure
	}

	/* Test for valid boot ID */
	if (bootID != SECBOOT_ID_INSECURE)
	{
		return 1; // Invalid Boot-ID
	}

    if (bootID == SECBOOT_ID_INSECURE)
	{
		*BootFlags = SECBOOT_FLAGS_INSECURE;
	}
   
	return 0; // success
}

/******************************************************************************
* getAppData
******************************************************************************/
static void getAppData(int BootSource, UInt16 BootFlags)
{
	UInt32 entry_point = 0;
	UInt32 index = 0;
	UInt16 rom_seed[4];
	fpReadNWords read_n_words;
	fpWriteNWords write_n_words;
	fpDone done;

	/* Initialize function pointers */
	read_n_words = bootSourceFunctions[0].readNWords;
	done = bootSourceFunctions[0].done;
	write_n_words = memory_write_n_words;

	index = BOOT_BASE_ADDR+1;

	/* Read the boot image */
	entry_point = secBoot(read_n_words, write_n_words, index, rom_seed, BootFlags);

	/* Clear up the conditions on the boot device */
	done();

	/* If code was successfully loaded, disable the SROM and begin code execution */
	if (entry_point)
	{
		// set XF to low for successful bootloading
		toggle_XF(0);

		/* Run the loaded application (this is the end of the bootloader -- there is no return) */
		branch_to(entry_point);
	}
}

/******************************************************************************
* memory_write_n_words
******************************************************************************/
UInt16 memory_write_n_words(UInt32 address, UInt16 *buffer, UInt16 count)
{
	int ii;

	/***************************************************************************
	* Prevent writing to reserved memory (MMR and Bootloader-RAM).
	* The following are all 16-bit word addresses...
	* <Reserved> MMR = 0x00 - 0x5F
	* <Reserved> Bootloader-RAM (SARAM31) = 0x27000 - 0x27FFF
	* <Reserved> ROM = 0x7F0000 - 0x7FFFFF
	* <Available> DARAM/SARAM = 0x60 - 0x26FFF
	* <Available> External-Memory = 0x28000 - 0x7EFFFF
	***************************************************************************/
	if ((address < 0x60) || (address >= 0x7F0000))
	{
		return 1;
	}

	for (ii=0; ii<count; ii++,address++)
	{
		if ((address & 0xFF000) != 0x27000)
		{
			*(UInt16 *)address = buffer[ii];
		}
	}
	return 0;
}

/******************************************************************************
* null_func
******************************************************************************/
static void null_func(void)
{
}

void boot_from_mcspi(int div)
{
	// disable the MCSPI related pull-ups
	pull_disable_mcspi();
	/* Setup pin-mapping (MCSPI on serial bus 1 in mode 1) */
	PeripheralPinMapping(CSL_FMKT(SYS_EBSR_SP0MODE, MODE0), CSL_FMKT(SYS_EBSR_SP1MODE, MODE1), CSL_FMKT(SYS_EBSR_PPMODE, MODE0));
	/* Enable MCSPI Clock */
	PeripheralClkEnableOnly(CSL_FMK(SYS_PCGCR1_SYSCLKDIS, 1L) | CSL_FMK(SYS_PCGCR1_TMR0CG, 1L) | CSL_FMK(SYS_PCGCR1_MCSPICG, 1L));
	/* Reset MCSPI */
	PeripheralReset(CSL_FMK(SYS_PRCR_PG1_RST, 1L), 0x20, 500);

	if (mcspi_eeprom_init(div) == 0) // successful init
	{
		bootFromMedia(BOOT_SELECT_MCSPI_EEPROM24);  // check for 24-bit MCSPI device second
	}

	// enable the MCSPI related pull-ups
	pull_enable_mcspi();
	
	return;
}

void clkx1(void)
{
	CSL_SYSCTRL_REGS->CCR2 = 0; /* force to bypass PLL */

	/****************************************************************
	* Fix Timer-0 setup for new SysClk (to track elapsed time).
	* Must wait 200ms after TRIM setup before re-enabling the
	* low-voltage detection circuit.
	****************************************************************/
	CSL_TIM_0_REGS->TCR = 0;
	if (clkInput>1)
	{
		/* Start Timer-0 with counter starting at 0x1D4C00 (200ms) for 19.2Mhz */
		CSL_TIM_0_REGS->TIMPRD1 = 0x4C00;
		CSL_TIM_0_REGS->TIMPRD2 = 0x001D;
	} else
	{
		/* Start Timer-0 with counter starting at 0x124F80 (200ms) for 12Mhz */
		CSL_TIM_0_REGS->TIMPRD1 = 0x4F80;
		CSL_TIM_0_REGS->TIMPRD2 = 0x0012;
	}
	CSL_TIM_0_REGS->TCR = (CSL_FMKT(TIM_TCR_TIMEN, ENABLE) | CSL_FMK(TIM_TCR_PSCDIV, 0) |
						CSL_FMKT(TIM_TCR_AUTORELOAD, DISABLE) | CSL_FMK(TIM_TCR_START, 1));

	return;
}

void clkx3(void)
{
	volatile Uint16 Count;

	CSL_SYSCTRL_REGS->CCR2 = 0; /* force to bypass PLL */
	// set up the PLL for Phoenix here
	/* use external clock (expected to be 19.2 Mhz, 12.288MHz, 12.0MHz, or 11.2896MHz) */
	/* setup for external-clock * 3 */
	/* 19.2MHz * 3 == 57.6MHz */
	/* 16.800MHz * 3 == 50.400MHz */
	/* 12.288MHz * 3 == 36.864MHz or 12.000MHz * 3 == 36.000MHz */
	/* 11.2896MHz * 3 == 33.8688MHz */
	/* (((input clk/4) * 96) / 8) == input clk * 3 */

	// PLL power up sequence:
	// Assert PLL reset (PLL Reset = 1, done at the hardware reset)
	// Deaasert Power Down (PLL Power Down = 0)
	// Program PLLM, divders
	// Wait for 5us
	// Deaasert PLL reset (PLL reset = 0)
	// Wait for lock (at least 2ms - 54000 cycles for 27Mhz)
	// Clear Bypass
	
	CSL_SYSCTRL_REGS->PCR &= ~(0x2000);		// 0x1c22, PLL power up

	CSL_SYSCTRL_REGS->PMR = 0x5F00;		// 0x1c20,  PLLM[15:0] = 24320 (X96)
	CSL_SYSCTRL_REGS->PICR =0x0003;		// 0x1c21, PLLM[16] = 0 | RD = 3 (/4)
	CSL_SYSCTRL_REGS->PODCR = 0x0027;	// 0x1c23, OD2 Enable|OD=7 (/8) | OD2 bypass

	// wait for 5us at 27Mhz for PLL power up
	asm("  repeat(#135)\n  nop");

	CSL_SYSCTRL_REGS->PCR = 0x0010;		// 0x1c22, PLL Enable | PLL Power on | Fast Enable

	// we need to wait at least 2ms for the PLL to stabilize 
	// wait for 2ms for 27Mhz, 4.7ms for 11.2896Mhz
	asm("  repeat(#54000)\n  nop");	

	CSL_SYSCTRL_REGS->CCR2 = 0x0001; /* enable the PLL (use PLL as SysClk) */

	/****************************************************************
	* Fix Timer-0 setup for new SysClk (to track elapsed time).
	* Must wait 200ms after TRIM setup before re-enabling the
	* low-voltage detection circuit.
	****************************************************************/
	CSL_TIM_0_REGS->TCR = 0;
	if (clkInput>1)
	{
		/* Start Timer-0 with counter starting at 0x57E400 (200ms) for 57.6Mhz */
		CSL_TIM_0_REGS->TIMPRD1 = 0xE400;
		CSL_TIM_0_REGS->TIMPRD2 = 0x0057;
	} else
	{
		/* Start Timer-0 with counter starting at 0x36EE80 (200ms) for 36Mhz */
		CSL_TIM_0_REGS->TIMPRD1 = 0xEE80;
		CSL_TIM_0_REGS->TIMPRD2 = 0x0036;
	}
	CSL_TIM_0_REGS->TCR = (CSL_FMKT(TIM_TCR_TIMEN, ENABLE) | CSL_FMK(TIM_TCR_PSCDIV, 0) |
						CSL_FMKT(TIM_TCR_AUTORELOAD, DISABLE) | CSL_FMK(TIM_TCR_START, 1));

	return;
}

// toggle the XF according to the flag
// 0: set the XF to low
// 1: set the XF to high
// 2: toggle the XF forever
// 3: toggle the XF once
void toggle_XF(int flag)
{
	volatile Uint32	looper;
	int st1_55_val;

	if (flag==0)
	{
		// set ST1_55 to low
		asm("	bit(ST1, #13) = 0");
	} else if (flag==1)
	{
		// set ST1_55 to high
			asm("	bit(ST1, #13) = 1");
	} else if (flag==2)
	{
		// loop forever and toggle the XF
		for (;;)
		{
			// set XF to high
			asm("	bit(ST1, #13) = 1");
			// wait at least 0.5 sec for 27Mhz clock
			for (looper=0; looper<1350000; looper++);
			asm("	bit(ST1, #13) = 0");
			// wait at least 0.5 sec for 27Mhz clock
			for (looper=0; looper<1350000; looper++);
			
		}
	} else
	{
		// get current ST1_55
		st1_55_val = (*((volatile unsigned int *)0x0003))&0x2000;
		// toggle it
		if (st1_55_val)
			asm("	bit(ST1, #13) = 0");
		else
			asm("	bit(ST1, #13) = 1");
	}
}

// enable MCSPI related pull ups/downs
void pull_enable_mcspi(void)
{
	// restore the PI register settings
	*((ioport volatile unsigned int *)0x1C17) = Saved_1C17;   /* S1[5:0] pins */
}

// disable MCSPI related pull ups/downs
void pull_disable_mcspi(void)
{
	// save the PI register settings
	Saved_1C17 = *((ioport volatile unsigned int *)0x1C17);
	// disable the pull-up/downs as required
	*((ioport volatile unsigned int *)0x1C17) |= 0x3F00;   /* S1[5:0] pins */
}
