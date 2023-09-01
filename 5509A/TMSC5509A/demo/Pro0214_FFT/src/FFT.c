
#include <math.h>

#define PI 3.1415926
#define SAMPLENUMBER 128

void InitForFFT(void);
void MakeWave(void);
void SDRAM_init(void);
void FFT(float dataR[SAMPLENUMBER],float dataI[SAMPLENUMBER]);

int INPUT[SAMPLENUMBER],DATA[SAMPLENUMBER];
float fWaveR[SAMPLENUMBER],fWaveI[SAMPLENUMBER],w[SAMPLENUMBER];
float sin_tab[SAMPLENUMBER],cos_tab[SAMPLENUMBER];

void main(void)
{
	int i;

	SDRAM_init();
	InitForFFT();
	MakeWave();
	for ( i=0;i<SAMPLENUMBER;i++ )
	{
		fWaveR[i]=INPUT[i];
		fWaveI[i]=0.0f;
		w[i]=0.0f;
	}
	FFT(fWaveR,fWaveI);
	for ( i=0;i<SAMPLENUMBER;i++ )
	{
		DATA[i]=w[i];
	}
	while ( 1 );	// break point
}

void FFT(float dataR[SAMPLENUMBER],float dataI[SAMPLENUMBER])
{
	int x0,x1,x2,x3,x4,x5,x6,xx;
	int i,j,k,b,p,L;
	float TR,TI,temp;
	
	/********** following code invert sequence ************/
	for ( i=0;i<SAMPLENUMBER;i++ )
	{
		x0=x1=x2=x3=x4=x5=x6=0;
		x0=i&0x01; x1=(i/2)&0x01; x2=(i/4)&0x01; x3=(i/8)&0x01;x4=(i/16)&0x01; x5=(i/32)&0x01; x6=(i/64)&0x01;
		xx=x0*64+x1*32+x2*16+x3*8+x4*4+x5*2+x6;
		dataI[xx]=dataR[i];
	}
	for ( i=0;i<SAMPLENUMBER;i++ )
	{
		dataR[i]=dataI[i]; dataI[i]=0; 
	}

	/************** following code FFT *******************/
	for ( L=1;L<=7;L++ )
	{ /* for(1) */
		b=1; i=L-1;
		while ( i>0 ) 
		{
			b=b*2; i--;
		} /* b= 2^(L-1) */
		for ( j=0;j<=b-1;j++ ) /* for (2) */
		{
			p=1; i=7-L;
			while ( i>0 ) /* p=pow(2,7-L)*j; */
			{
				p=p*2; i--;
			}
			p=p*j;
			for ( k=j;k<128;k=k+2*b ) /* for (3) */
			{
				TR=dataR[k]; TI=dataI[k]; temp=dataR[k+b];
				dataR[k]=dataR[k]+dataR[k+b]*cos_tab[p]+dataI[k+b]*sin_tab[p];
				dataI[k]=dataI[k]-dataR[k+b]*sin_tab[p]+dataI[k+b]*cos_tab[p];
				dataR[k+b]=TR-dataR[k+b]*cos_tab[p]-dataI[k+b]*sin_tab[p];
				dataI[k+b]=TI+temp*sin_tab[p]-dataI[k+b]*cos_tab[p];
			} /* END for (3) */
		} /* END for (2) */
	} /* END for (1) */
	for ( i=0;i<SAMPLENUMBER/2;i++ )
	{ 
		w[i]=sqrt(dataR[i]*dataR[i]+dataI[i]*dataI[i]);
	}
} /* END FFT */


void InitForFFT(void)
{
	int i;
	
	for ( i=0;i<SAMPLENUMBER;i++ )
	{
		sin_tab[i]=sin(PI*2*i/SAMPLENUMBER);
		cos_tab[i]=cos(PI*2*i/SAMPLENUMBER);
	}
}

void MakeWave(void)
{
	int i;
	
	for ( i=0;i<SAMPLENUMBER;i++ )
	{
		INPUT[i]=sin(PI*2*i/SAMPLENUMBER*3)*1024;
	}
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
