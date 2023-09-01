#include <math.h>

#define FIRNUMBER 25
#define SIGNAL1F 1000
#define SIGNAL2F 4500
#define SAMPLEF  10000
#define PI 3.1415926

float InputWave(void);
float FIR(void);
void SDRAM_init(void);

float fHn[FIRNUMBER]={ 0.0,0.0,0.001,-0.002,-0.002,0.01,-0.009,
                       -0.018,0.049,-0.02,0.11,0.28,0.64,0.28,
                       -0.11,-0.02,0.049,-0.018,-0.009,0.01,
                       -0.002,-0.002,0.001,0.0,0.0
                     };
float fXn[FIRNUMBER]={ 0.0 };
float fInput,fOutput;
float fSignal1,fSignal2;
float fStepSignal1,fStepSignal2;
float f2PI;
int i;
float fIn[256],fOut[256];
int nIn,nOut;

void main(void)
{
	nIn=0; nOut=0;
	f2PI=2*PI;
	fSignal1=0.0;
	fSignal2=PI*0.1;
	fStepSignal1=2*PI/30;
	fStepSignal2=2*PI*1.4;
	while ( 1 )
	{
		fInput=InputWave();
		fIn[nIn]=fInput;
		nIn++; nIn%=256;
		fOutput=FIR();
		fOut[nOut]=fOutput;
		nOut++;
		if ( nOut>=256 )
		{
			nOut=0;		/* break point */
		}
	}
}

float InputWave(void)
{
	for ( i=FIRNUMBER-1;i>0;i-- )
		fXn[i]=fXn[i-1];
	fXn[0]=sin((double)fSignal1)+cos((double)fSignal2)/6.0;
	fSignal1+=fStepSignal1; 
	if ( fSignal1>=f2PI )	fSignal1-=f2PI;
	fSignal2+=fStepSignal2;
	if ( fSignal2>=f2PI )	fSignal2-=f2PI;
	return(fXn[0]);
}

float FIR(void)
{
	float fSum;
	fSum=0;
	for ( i=0;i<FIRNUMBER;i++ )
	{
		fSum+=(fXn[i]*fHn[i]);
	}
	return(fSum);
}


void SDRAM_init(void)
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
