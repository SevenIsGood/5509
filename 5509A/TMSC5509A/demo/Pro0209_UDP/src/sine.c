#include <stdio.h>

#include <csl.h>
#include <csl_pll.h>
#include <csl_chiphal.h>

#define SINE_LEN (2000)

void wait( unsigned int cycles );
void SDRAM_init(void);

Uint16 nSine[SINE_LEN];

/*锁相环的设置*/
PLL_Config  myConfig	  = {
	0, 	//IAI: the PLL locks using the same process that was underway
		//before the idle mode was entered
	1, 	//IOB: If the PLL indicates a break in the phase lock,
		//it switches to its bypass mode and restarts the PLL phase-locking
		//sequence
	24, //PLL multiply value; multiply 24 times
	1 	//Divide by 2 PLL divide value; it can be either PLL divide value
		//(when PLL is enabled), or Bypass-mode divide value
		//(PLL in bypass mode, if PLL multiply value is set to 1)
};

/****************************************************************************/
/*																			*/
/*   产生正弦波函数															 */
/*   sampleRate : 采样率													 */
/*   freqTONE   : 正弦波频率											     */
/*   amplify	: 幅度													    */
/*   Data_out   : 数据输出接口											     */
/*   NumOfUnit  : 正弦波点数												 */
/*																		    */
/****************************************************************************/
static void GenSineData(Uint32 sampleRate, Uint32 freqTONE, Uint32 amplify, Uint16 *Data_out, Uint32 NumOfUnit)
{
	int i;
	float pi = 3.1415926;
	float w = 0.0;
	short *pDataOut = 0;
	amplify = (amplify) >> 1 ;
	pDataOut = (short *)Data_out;
	w =2 * freqTONE * pi / sampleRate;
	for(i = 0; i < NumOfUnit; i++)
	{
		pDataOut[i] = ((short) ((float)(amplify * sin(w*i)) + amplify));
	}

}

main()
{
	int i;
	unsigned int uWork;
	/*初始化CSL库*/
	CSL_init();

	/*设置系统的运行速度为144MHz*/
	PLL_config(&myConfig);

	SDRAM_init();

    GenSineData(200000, 1000, 2048, nSine, SINE_LEN);
	while ( 1 )
	{

	}
}

void SDRAM_init( void )
{
	ioport unsigned int *ebsr  =(unsigned int *)0x6c00;
	ioport unsigned int *egcr  =(unsigned int *)0x800;
	ioport unsigned int *emirst=(unsigned int *)0x801;
	//ioport unsigned int *emibe =(unsigned int *)0x802;
	ioport unsigned int *ce01  =(unsigned int *)0x803;
	//ioport unsigned int *ce02  =(unsigned int *)0x804;
	//ioport unsigned int *ce03  =(unsigned int *)0x805;
	ioport unsigned int *ce11  =(unsigned int *)0x806;
	//ioport unsigned int *ce12  =(unsigned int *)0x807;
	//ioport unsigned int *ce13  =(unsigned int *)0x808;
	ioport unsigned int *ce21  =(unsigned int *)0x809;
	//ioport unsigned int *ce22  =(unsigned int *)0x80A;
	//ioport unsigned int *ce23  =(unsigned int *)0x80B;
	ioport unsigned int *ce31  =(unsigned int *)0x80C;
	//ioport unsigned int *ce32  =(unsigned int *)0x80D;
	//ioport unsigned int *ce33  =(unsigned int *)0x80E;
	ioport unsigned int *sdc1  =(unsigned int *)0x80F;
	//ioport unsigned int *sdper =(unsigned int *)0x810;
	//ioport unsigned int *sdcnt =(unsigned int *)0x811;
	ioport unsigned int *init  =(unsigned int *)0x812;
	ioport unsigned int *sdc2  =(unsigned int *)0x813;
	//*ebsr   = 0x221;//0xa01
	*ebsr   = 0xa01;
	*egcr   = 0x220;
	*egcr   = 0X220;
	*ce01   = 0X3000;
	*ce11   = 0X1fff;
	*ce21   = 0x1fff;
	*ce31   = 0x1fff;
	*emirst = 0;
	*sdc1   = 0X5958;
	*sdc2   = 0X38F;
	*init   = 0;
}


void wait( unsigned int cycles )
{
	int i;
	for ( i = 0 ; i < cycles ; i++ ){ }
}

