#include <csl.h>ESP8266
#include <csl_pll.h>
#include <csl_mcbsp.h>
#include <csl_gpio.h>

#include "spi.h"

MCBSP_Config Mcbsptest;

MCBSP_Config Mcbsp0Config = {
	MCBSP_SPCR1_RMK(
		MCBSP_SPCR1_DLB_OFF,				/* DLB	= 0,禁止自闭环方式 */
		MCBSP_SPCR1_RJUST_RZF,				/* RJUST  = 2 */
		//MCBSP_SPCR1_CLKSTP_DISABLE,		/* CLKSTP = 0 */
		MCBSP_SPCR1_CLKSTP_NODELAY,
		MCBSP_SPCR1_DXENA_NA,				/* DXENA  = 1 */
		0,									/* ABIS   = 0 */
		MCBSP_SPCR1_RINTM_RRDY,				/* RINTM  = 0 */
		0,									/* RSYNCER = 0 */
		MCBSP_SPCR1_RRST_DISABLE			/* RRST   = 0 */
	),
	MCBSP_SPCR2_RMK(
		MCBSP_SPCR2_FREE_NO,				/* FREE   = 0 */
		MCBSP_SPCR2_SOFT_NO,				/* SOFT   = 0 */
		MCBSP_SPCR2_FRST_RESET,  			 //帧同步关闭
		//MCBSP_SPCR2_FRST_FSG,				/* FRST   = 0 */
		MCBSP_SPCR2_GRST_RESET,				/* GRST   = 0 */
		MCBSP_SPCR2_XINTM_XRDY,				/* XINTM  = 0 */
		0,									/* XSYNCER = N/A */
		MCBSP_SPCR2_XRST_DISABLE			/* XRST   = 0 */
	),
	/*单数据相，接受数据长度为16位,每相2个数据*/
	MCBSP_RCR1_RMK(
		MCBSP_RCR1_RFRLEN1_OF(0),			/* RFRLEN1 = 1 */
		MCBSP_RCR1_RWDLEN1_16BIT			/* RWDLEN1 = 2 */
	),
	MCBSP_RCR2_RMK(
		MCBSP_RCR2_RPHASE_SINGLE,			/* RPHASE  = 0 */
		MCBSP_RCR2_RFRLEN2_OF(0),			/* RFRLEN2 = 0 */
		MCBSP_RCR2_RWDLEN2_8BIT,			/* RWDLEN2 = 0 */
		MCBSP_RCR2_RCOMPAND_MSB,			/* RCOMPAND = 0 */
		MCBSP_RCR2_RFIG_YES,				/* RFIG	= 0 */
		MCBSP_RCR2_RDATDLY_1BIT				/* RDATDLY = 1 */
		),
	MCBSP_XCR1_RMK(
		MCBSP_XCR1_XFRLEN1_OF(0),			/* XFRLEN1 = 1 */
		MCBSP_XCR1_XWDLEN1_16BIT			/* XWDLEN1 = 2 */

	),
	MCBSP_XCR2_RMK(
		MCBSP_XCR2_XPHASE_SINGLE,			/* XPHASE  = 0 */
		MCBSP_XCR2_XFRLEN2_OF(0),			/* XFRLEN2 = 0 */
		MCBSP_XCR2_XWDLEN2_8BIT,			/* XWDLEN2 = 0 */
		MCBSP_XCR2_XCOMPAND_MSB,			/* XCOMPAND = 0 */
		MCBSP_XCR2_XFIG_YES,				/* XFIG	= 0 */
		MCBSP_XCR2_XDATDLY_1BIT				/* XDATDLY = 1 */
	),
	MCBSP_SRGR1_RMK(
		MCBSP_SRGR1_FWID_OF(0),				/* 停止模式无效**重要FWID	= 0 帧同步信号的脉宽周期数*/
		MCBSP_SRGR1_CLKGDV_OF(11)			/* 停止模式无效**重要CLKGDV = 9 CLKG时钟频率*/
	),
	MCBSP_SRGR2_RMK(
		0,//MCBSP_SRGR2_GSYNC_FREE,			/* FREE	= 0 内部时钟帧同步 */
		MCBSP_SRGR2_CLKSP_RISING,			/* 重要CLKSP   = 0 CLKS 引脚极性*/
		MCBSP_SRGR2_CLKSM_INTERNAL,			/* 重要CLKSM   = 1 cpu时钟*/
		MCBSP_SRGR2_FSGM_DXR2XSR,			/* FSGM	= 0 发送帧同步模式，如果FXSM=1,当DXR->XSR时,McBSP产生帧同步信号*/
		MCBSP_SRGR2_FPER_OF(15)				/* 重要 FPER	= 0 FSG信号帧同步时钟周期数，应该是7*/
	),
	MCBSP_MCR1_DEFAULT,
	MCBSP_MCR2_DEFAULT,
	MCBSP_PCR_RMK(
		MCBSP_PCR_IDLEEN_RESET,					/* IDLEEN   = 0   */
		MCBSP_PCR_XIOEN_SP,						/* XIOEN	= 0   */
		MCBSP_PCR_RIOEN_SP,						/* RIOEN	= 0   */
		MCBSP_PCR_FSXM_INTERNAL,				/* FSXM	= 0   */
		MCBSP_PCR_FSXM_INTERNAL,				/* FSRM	= 0   */
												/* DXSTAT = N/A   */
		MCBSP_PCR_CLKXM_OUTPUT,					/* CLKXM	= 0   */
		MCBSP_PCR_CLKRM_INPUT,					/* CLKRM	= 0   */
		MCBSP_PCR_SCLKME_NO,					/* SCLKME   = 0   */
		0,
		MCBSP_PCR_FSXP_ACTIVELOW,				/* FSXP	= 0   */
		MCBSP_PCR_FSXP_ACTIVELOW,				/* FSRP	= 1   */
		MCBSP_PCR_CLKXP_RISING,					/* CLKXP	= 1   */
		MCBSP_PCR_CLKRP_RISING					/* CLKRP	= 1   */
	),
	MCBSP_RCERA_DEFAULT,
	MCBSP_RCERB_DEFAULT,
	MCBSP_RCERC_DEFAULT,
	MCBSP_RCERD_DEFAULT,
	MCBSP_RCERE_DEFAULT,
	MCBSP_RCERF_DEFAULT,
	MCBSP_RCERG_DEFAULT,
	MCBSP_RCERH_DEFAULT,
	MCBSP_XCERA_DEFAULT,
	MCBSP_XCERB_DEFAULT,
	MCBSP_XCERC_DEFAULT,
	MCBSP_XCERD_DEFAULT,
	MCBSP_XCERE_DEFAULT,
	MCBSP_XCERF_DEFAULT,
	MCBSP_XCERG_DEFAULT,
	MCBSP_XCERH_DEFAULT
};

/*定义McBSP的句柄*/

MCBSP_Handle hMcbsp[3];


/*两次操作之间的延时*/
static void SPIEND_Delay()
{
	Uint32 j =0;
	for(j= 0;j<0x20;j++)
	{}
}

int  SPI_init(int *handle, int clk, SPI_BIT_LEN bitLen, SPI_CLK_MODE mode)
{
	MCBSP_Handle hMcbsp = (MCBSP_Handle)handle;
	int cfgClk = 144000000/clk;
	/****************************************************************/
	/* Place port in reset - setting XRST & RRST to 0               */
	/****************************************************************/
	/*进行发送复位*/
	MCBSP_FSETH(hMcbsp, SPCR2, XRST,0);
	/*进行接收复位*/
	MCBSP_FSETH(hMcbsp, SPCR1, RRST,0);
	/*进行分频器复位*/
	MCBSP_FSETH(hMcbsp, SPCR2, GRST,0);
	/****************************************************************/
	/* Set values of all control registers                          */
	/****************************************************************/

	/*设置XCR1寄存器，单数据相，每次接受发送数据长度为1*/
	MCBSP_FSETH(hMcbsp, XCR1, XFRLEN1, 0x0);
	MCBSP_FSETH(hMcbsp, RCR1, RFRLEN1, 0x0);
	MCBSP_FSETH(hMcbsp, XCR1, XWDLEN1, bitLen);
	MCBSP_FSETH(hMcbsp, RCR1, RWDLEN1, bitLen);
	/*设置XCR2寄存器，发送数据延时一个位*/
	MCBSP_FSETH(hMcbsp, XCR2, XDATDLY,0x1);
	MCBSP_FSETH(hMcbsp, RCR2, RDATDLY,0x1);
	/*设置PCR寄存器，设置BLCK为输出，即MCBSP为Master；
					设置BFSX为输出，并且其极性为低有效
					设置CLKXP为高*/
	if(mode == SPI_MODE_0)
	{
		/*设置SPCR1寄存器,SPI时钟停止模式*/
		MCBSP_FSETH(hMcbsp, SPCR1,CLKSTP,0x2);
		MCBSP_FSETH(hMcbsp, PCR,CLKXP, 0x0);
		MCBSP_FSETH(hMcbsp, PCR,CLKRP, 0x0);

	}
	else if(mode == SPI_MODE_1)
	{
		/*设置SPCR1寄存器,SPI时钟停止模式*/
		MCBSP_FSETH(hMcbsp, SPCR1,CLKSTP,0x3);
		MCBSP_FSETH(hMcbsp, PCR,CLKXP, 0x0);
		MCBSP_FSETH(hMcbsp, PCR,CLKRP, 0x1);
	}
	else if(mode == SPI_MODE_2)
	{
		/*设置SPCR1寄存器,SPI时钟停止模式*/
		MCBSP_FSETH(hMcbsp, SPCR1,CLKSTP,0x2);
		MCBSP_FSETH(hMcbsp, PCR,CLKXP, 0x1);
		MCBSP_FSETH(hMcbsp, PCR,CLKRP, 0x0);
	}ddddddddddddddddddd
	else if(mode == SPI_MODE_3)
	{
		/*设置SPCR1寄存器,SPI时钟停止模式*/
		MCBSP_FSETH(hMcbsp, SPCR1,CLKSTP,0x3);
		MCBSP_FSETH(hMcbsp, PCR,CLKXP, 0x1);
		MCBSP_FSETH(hMcbsp, PCR,CLKRP, 0x1);													-
	}
	MCBSP_FSETH(hMcbsp, PCR,CLKXM, 0x1);
	MCBSP_FSETH(hMcbsp, PCR,FSXM, 0x1);
	MCBSP_FSETH(hMcbsp, PCR,FSXP, 0x1);
	/*设置SRGR2，确定时钟来源为内部的CPU?
				，确定帧同步为低有效。*/
	MCBSP_FSETH(hMcbsp, SRGR2,CLKSM,1);
	/*使能波特率寄存器*/
	MCBSP_FSETH(hMcbsp, SPCR2, GRST,1);
	/*等待两个CLKG时钟周期*/
	asm(" nop ");
	asm(" nop ");
	/*设置SRGR1，确定分频数为0x20，MCBSP2的波待率为100KHz*/
	//   MCBSP_FSETH(hMcbsp, SRGR1, FWID,0x20);
	//   MCBSP_FSETH(hMcbsp, SRGR2,FPER,0x20);
	MCBSP_FSETH(hMcbsp, SRGR1, CLKGDV, (cfgClk - 1));
	/*设置FRST位*/
	MCBSP_FSETH(hMcbsp, SPCR2,FRST,1);
	/*使能MCBSP0发送*/
	MCBSP_FSETH(hMcbsp, SPCR2, XRST,1);
	/*使能MCBSP0接收*/
	MCBSP_FSETH(hMcbsp, SPCR1, RRST,1);
	/*等待两个CLKG时钟周期*/
	asm(" nop ");
	asm(" nop ");
}

int * SPI_Open(int DevNum, int clk, SPI_BIT_LEN bitLen, SPI_CLK_MODE mode)
{
	int port[2] = {MCBSP_PORT0, MCBSP_PORT1, MCBSP_PORT2};


	hMcbsp[DevNum] = MCBSP_open(port[DevNum],MCBSP_OPEN_RESET);
	if(hMcbsp[DevNum])
	{
		return -1;
	}

	/*
	MCBSP_FSETH(hMcbsp[DevNum], SRGR1, CLKGDV, (cfgClk - 1));
	MCBSP_start(hMcbsp[DevNum],
		MCBSP_SRGR_START | MCBSP_XMIT_START | MCBSP_RCV_START,
			0);
	*/
	SPI_init(hMcbsp[DevNum], clk, bitLen, mode);

	return hMcbsp[DevNum];
}


int  SPI_Write(int *handle, char *buff, int len)
{
	int i = 0;

	if(handle < 0)
	{
		return -1;
	}

	for(i = 0; i < len; i++)
	{
		while(!MCBSP_xrdy((MCBSP_Handle)handle));

		MCBSP_write16(hMcbsp, buff[i]);
	}

	while(!MCBSP_xrdy((MCBSP_Handle)handle));

	return 0;
}

int SPI_Read(int *handle, char *inBuff, char *outBuff,int len)
{
	int i = 0;
	short datRead = 0;
	if(handle < 0)
	{
		return -1;
	}

	for(i = 0; i < len; i++)
	{
		while(!MCBSP_xrdy((MCBSP_Handle)handle));

		MCBSP_write16(hMcbsp, inBuff[i]);
		SPIEND_Delay();
		datRead = MCBSP_read16(hMcbsp);
		outBuff[i] = datRead&0xFF;
	}

	while(!MCBSP_xrdy((MCBSP_Handle)handle));

	return 0;
}