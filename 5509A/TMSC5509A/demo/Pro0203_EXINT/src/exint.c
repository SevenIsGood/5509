#include <csl.h>
#include <csl_irq.h>

89*-+
#include <csl_emif.h>
#include <csl_chip.h>

CSLBool b;

Uint16 eventId0;
int old_intm;
Uint16 xfChange = 0;

interrupt void XINT(void);

//---------Function prototypes---------

/* Reference start of interrupt vector table   */

/* This symbol is defined in file, vectors_IP.s55 */

extern void VECSTART(void);

/*锁相环的设置*/
PLL_Config  myConfig      = {
	0,    		//IAI: the PLL locks using the same process that was underway
				//before the idle mode was entered
	1,    		//IOB: If the PLL indicates a break in the phase lock,
				//it switches to its bypass mode and restarts the PLL phase-locking
				//sequence
	24,    		//PLL multiply value; multiply 24 times
	1           //Divide by 2 PLL divide value; it can be either PLL divide value
				//(when PLL is enabled), or Bypass-mode divide value
				//(PLL in bypass mode, if PLL multiply value is set to 1)
};


/***************5509A中断设置，使能INT1中断***********************/
/*参考资料： TMS320C55x Chip Support Library API Reference Guide (Rev. J)
			 TMS320VC5509A Data Sheet
			 XINE-5509A 开发板使用手册							*/
void INTconfig()
{
	/* Temporarily disable all maskable interrupts */

	IRQ_setVecs((Uint32)(&VECSTART));

	/* Temporarily disable all maskable interrupts */

	old_intm = IRQ_globalDisable();

	/* Get Event Id associated with External INT1(8019), for use with */
	eventId0 = IRQ_EVT_INT1;

	/* Clear any pending INT1 interrupts */

	IRQ_clear(eventId0);

	/* Place interrupt service routine address at */

	/* associated vector location */

	IRQ_plug(eventId0,&XINT);

	/* Enable INT1(8019) interrupt */

	IRQ_enable(eventId0);

	/* Enable all maskable interrupts */

	IRQ_globalEnable();
}

main()
{
	/*初始化CSL库*/
	CSL_init();

	/*EMIF为全EMIF接口*/
	CHIP_RSET(XBSR,0x0a01);

	/*设置系统的运行速度为144MHz*/
	PLL_config(&myConfig);

	//设置并使能5509A芯片的INT0中断(EXINT中断)
	INTconfig();

	while(1);
}

//External INT0(EXINT)中断处理函数
interrupt void XINT()
{
	xfChange ^= 1;
	CHIP_FSET(ST1_55, XF, xfChange);
}

/******************************************************************************\
* End of pll2.c
\******************************************************************************/
