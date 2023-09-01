#include <csl.h>ESP8266
#include <csl_pll.h>
#include <csl_mcbsp.h>
#include <csl_gpio.h>

#include "spi.h"

MCBSP_Config Mcbsptest;

MCBSP_Config Mcbsp0Config = {
	MCBSP_SPCR1_RMK(
		MCBSP_SPCR1_DLB_OFF,				/* DLB	= 0,��ֹ�Աջ���ʽ */
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
		MCBSP_SPCR2_FRST_RESET,  			 //֡ͬ���ر�
		//MCBSP_SPCR2_FRST_FSG,				/* FRST   = 0 */
		MCBSP_SPCR2_GRST_RESET,				/* GRST   = 0 */
		MCBSP_SPCR2_XINTM_XRDY,				/* XINTM  = 0 */
		0,									/* XSYNCER = N/A */
		MCBSP_SPCR2_XRST_DISABLE			/* XRST   = 0 */
	),
	/*�������࣬�������ݳ���Ϊ16λ,ÿ��2������*/
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
		MCBSP_SRGR1_FWID_OF(0),				/* ֹͣģʽ��Ч**��ҪFWID	= 0 ֡ͬ���źŵ�����������*/
		MCBSP_SRGR1_CLKGDV_OF(11)			/* ֹͣģʽ��Ч**��ҪCLKGDV = 9 CLKGʱ��Ƶ��*/
	),
	MCBSP_SRGR2_RMK(
		0,//MCBSP_SRGR2_GSYNC_FREE,			/* FREE	= 0 �ڲ�ʱ��֡ͬ�� */
		MCBSP_SRGR2_CLKSP_RISING,			/* ��ҪCLKSP   = 0 CLKS ���ż���*/
		MCBSP_SRGR2_CLKSM_INTERNAL,			/* ��ҪCLKSM   = 1 cpuʱ��*/
		MCBSP_SRGR2_FSGM_DXR2XSR,			/* FSGM	= 0 ����֡ͬ��ģʽ�����FXSM=1,��DXR->XSRʱ,McBSP����֡ͬ���ź�*/
		MCBSP_SRGR2_FPER_OF(15)				/* ��Ҫ FPER	= 0 FSG�ź�֡ͬ��ʱ����������Ӧ����7*/
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

/*����McBSP�ľ��*/

MCBSP_Handle hMcbsp[3];


/*���β���֮�����ʱ*/
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
	/*���з��͸�λ*/
	MCBSP_FSETH(hMcbsp, SPCR2, XRST,0);
	/*���н��ո�λ*/
	MCBSP_FSETH(hMcbsp, SPCR1, RRST,0);
	/*���з�Ƶ����λ*/
	MCBSP_FSETH(hMcbsp, SPCR2, GRST,0);
	/****************************************************************/
	/* Set values of all control registers                          */
	/****************************************************************/

	/*����XCR1�Ĵ������������࣬ÿ�ν��ܷ������ݳ���Ϊ1*/
	MCBSP_FSETH(hMcbsp, XCR1, XFRLEN1, 0x0);
	MCBSP_FSETH(hMcbsp, RCR1, RFRLEN1, 0x0);
	MCBSP_FSETH(hMcbsp, XCR1, XWDLEN1, bitLen);
	MCBSP_FSETH(hMcbsp, RCR1, RWDLEN1, bitLen);
	/*����XCR2�Ĵ���������������ʱһ��λ*/
	MCBSP_FSETH(hMcbsp, XCR2, XDATDLY,0x1);
	MCBSP_FSETH(hMcbsp, RCR2, RDATDLY,0x1);
	/*����PCR�Ĵ���������BLCKΪ�������MCBSPΪMaster��
					����BFSXΪ����������伫��Ϊ����Ч
					����CLKXPΪ��*/
	if(mode == SPI_MODE_0)
	{
		/*����SPCR1�Ĵ���,SPIʱ��ֹͣģʽ*/
		MCBSP_FSETH(hMcbsp, SPCR1,CLKSTP,0x2);
		MCBSP_FSETH(hMcbsp, PCR,CLKXP, 0x0);
		MCBSP_FSETH(hMcbsp, PCR,CLKRP, 0x0);

	}
	else if(mode == SPI_MODE_1)
	{
		/*����SPCR1�Ĵ���,SPIʱ��ֹͣģʽ*/
		MCBSP_FSETH(hMcbsp, SPCR1,CLKSTP,0x3);
		MCBSP_FSETH(hMcbsp, PCR,CLKXP, 0x0);
		MCBSP_FSETH(hMcbsp, PCR,CLKRP, 0x1);
	}
	else if(mode == SPI_MODE_2)
	{
		/*����SPCR1�Ĵ���,SPIʱ��ֹͣģʽ*/
		MCBSP_FSETH(hMcbsp, SPCR1,CLKSTP,0x2);
		MCBSP_FSETH(hMcbsp, PCR,CLKXP, 0x1);
		MCBSP_FSETH(hMcbsp, PCR,CLKRP, 0x0);
	}ddddddddddddddddddd
	else if(mode == SPI_MODE_3)
	{
		/*����SPCR1�Ĵ���,SPIʱ��ֹͣģʽ*/
		MCBSP_FSETH(hMcbsp, SPCR1,CLKSTP,0x3);
		MCBSP_FSETH(hMcbsp, PCR,CLKXP, 0x1);
		MCBSP_FSETH(hMcbsp, PCR,CLKRP, 0x1);													-
	}
	MCBSP_FSETH(hMcbsp, PCR,CLKXM, 0x1);
	MCBSP_FSETH(hMcbsp, PCR,FSXM, 0x1);
	MCBSP_FSETH(hMcbsp, PCR,FSXP, 0x1);
	/*����SRGR2��ȷ��ʱ����ԴΪ�ڲ���CPU?
				��ȷ��֡ͬ��Ϊ����Ч��*/
	MCBSP_FSETH(hMcbsp, SRGR2,CLKSM,1);
	/*ʹ�ܲ����ʼĴ���*/
	MCBSP_FSETH(hMcbsp, SPCR2, GRST,1);
	/*�ȴ�����CLKGʱ������*/
	asm(" nop ");
	asm(" nop ");
	/*����SRGR1��ȷ����Ƶ��Ϊ0x20��MCBSP2�Ĳ�����Ϊ100KHz*/
	//   MCBSP_FSETH(hMcbsp, SRGR1, FWID,0x20);
	//   MCBSP_FSETH(hMcbsp, SRGR2,FPER,0x20);
	MCBSP_FSETH(hMcbsp, SRGR1, CLKGDV, (cfgClk - 1));
	/*����FRSTλ*/
	MCBSP_FSETH(hMcbsp, SPCR2,FRST,1);
	/*ʹ��MCBSP0����*/
	MCBSP_FSETH(hMcbsp, SPCR2, XRST,1);
	/*ʹ��MCBSP0����*/
	MCBSP_FSETH(hMcbsp, SPCR1, RRST,1);
	/*�ȴ�����CLKGʱ������*/
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