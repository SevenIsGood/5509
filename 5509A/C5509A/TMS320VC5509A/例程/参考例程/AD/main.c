#include "myapp.h"
#include "csedu.h"
#include "scancode.h"

void InitADC();
void wait( unsigned int cycles );
void EnableAPLL( );

unsigned int nADC0[256],nADC1[256];

main()
{
	int i;
	unsigned int uWork;
	
    EnableAPLL();
	SDRAM_init();
	InitADC();
    PLL_Init(132);
	while ( 1 )
	{
		for ( i=0;i<256;i++ )
		{
			ADCCTL=0x8000;	// 启动AD转换，通道0
			do
			{
				uWork=ADCDATA;
			} while ( uWork&0x8000 );
			nADC0[i]=uWork&0x0fff;
		}
		for ( i=0;i<256;i++ )
		{
			ADCCTL=0x9000;	// 启动AD转换，通道1
			do
			{
				uWork=ADCDATA;
			} while ( uWork&0x8000 );
			nADC1[i]=uWork&0x0fff;
		}
		asm( " nop");		// break point
	}
}

void InitADC()
{
	ADCCLKCTL=0x23; // 4MHz ADCLK
	ADCCLKDIV=0x4f00;
}

void wait( unsigned int cycles )
{
    int i;
    for ( i = 0 ; i < cycles ; i++ ){ }
}

void EnableAPLL( )
{
    /* Enusre DPLL is running */
    *( ioport volatile unsigned short* )0x1f00 = 4;

    wait( 25 );

    *( ioport volatile unsigned short* )0x1f00 = 0;

    // MULITPLY
    *( ioport volatile unsigned short* )0x1f00 = 0x3000;

    // COUNT
    *( ioport volatile unsigned short* )0x1f00 |= 0x4F8;

    wait( 25 );

    //*( ioport volatile unsigned short* )0x1f00 |= 0x800

    // MODE
    *( ioport volatile unsigned short* )0x1f00 |= 2;

    wait( 30000 );

    // APLL Select
    *( ioport volatile unsigned short* )0x1e80 = 1;

    // DELAY
    wait( 60000 );
}
