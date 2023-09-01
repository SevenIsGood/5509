#include"math.h"

#define PI 3.1415926
#define COEFFNUMBER 16
#define INPUTNUMBER 1024

int FIRLMS(int *nx,float *nh,int nError,int nCoeffNumber);

float h[COEFFNUMBER],fU;
int xx[INPUTNUMBER],rr[INPUTNUMBER],wc[INPUTNUMBER];

main()
{
	int i,nLastOutput;
	
	nLastOutput=0;
	fU=0.0005;
	for ( i=0;i<COEFFNUMBER;i++ )	h[i]=0;
	for ( i=0;i<INPUTNUMBER;i++ )
	{
		xx[i]=256*sin(i*2*PI/34);
		rr[i]=wc[i]=0;
	}
	for ( i=COEFFNUMBER+1;i<INPUTNUMBER;i++ )
	{
		nLastOutput=FIRLMS(xx+i,h,nLastOutput-xx[i-1],COEFFNUMBER);	// break point
		rr[i]=nLastOutput;
		wc[i]=rr[i]-xx[i];
	}
	exit(0);
}

int FIRLMS(int *nx,float *nh,int nError,int nCoeffNumber)
{
	int i,r;
	float fWork;
	
	r=0;
	for ( i=0;i<nCoeffNumber;i++ )
	{
		fWork=nx[i]*nError*fU;
		nh[i]+=fWork;
		r+=(nx[i-i]*nh[i]);
	}
	r/=128;
	return r;
}
