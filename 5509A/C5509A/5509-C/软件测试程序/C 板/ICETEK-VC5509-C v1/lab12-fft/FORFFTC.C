/*****************************************************************************/
/*   FORFFTC.C                                                               */
/*   Checks the real FFT code for a 2048-point FFT run on a C3x device.      */
/*   It can be extended to different sizes by changing the global constants  */
/*   defined below.                                                          */
/*   For FFT of size below 1024 points or equal to 1024 points , FFT input   */
/*   and output data can be put in internal RAM , same location. Also the    */
/*   twiddle factor table and FFT code can be put in internal RAM.           */
/*   If FFT size is over 1024 points, please watch the memory allocation.    */
/*                                                                           */
/*   In this example, the FFT input is the sum of three cosine, which makes  */
/*   simpler when we check the FFT output.                                   */
/*****************************************************************************/
#include "values.h"
#include "math.h"
#include "Dsplib.h"
/*===========================================================================*/
/* GLOBAL CONSTANTS                                                          */
/*===========================================================================*/

#define FFT_SIZE             128
#define HALF_FFT_SIZE        64
#define LOG2_SIZE              11
#define BITREV                  1      /*  Bit reverse will be performed   */

/*===========================================================================*/
/* GLOBAL VARIABLES                                                          */
/*===========================================================================*/
int i;
long theta,part;
int clkbitr,clkcore,clktotal;
long FFTBufferIn[FFT_SIZE];
long FFTBufferOut[FFT_SIZE];
long SinTable[HALF_FFT_SIZE];
extern void cifft32_NOSCALE (LDATA *x,  ushort nx);
/*===========================================================================*/
/* MAIN()                                                                    */
/*===========================================================================*/
void main()
{ 
	//asm ("   OR 0800h,ST");    /* cache is on */
	theta = 2*PI/FFT_SIZE;
	for ( i=0;i<HALF_FFT_SIZE;i++ )   /* ²úÉú */
		SinTable[i]=sin(i*theta);

	for ( i=0;i<FFT_SIZE;i++ )        /* fill fft input table */
		FFTBufferIn[i]=FFTBufferOut[i]=cos(100*i*theta)*2/3+cos(64*i*theta)*3/4+cos(32*i*theta);

//	ffft_rl( FFT_SIZE, LOG2_SIZE, FFTBufferIn, FFTBufferOut,SinTable,BITREV);
    cifft32_NOSCALE (FFTBufferIn,  FFT_SIZE);
	for (i=0;i<FFT_SIZE;i++)        /* zero small values for easier check */
    { 
    	if (FFTBufferOut[i] < 0.01)
     	FFTBufferOut[i]=0.0;
    }
	exit(0);
}

