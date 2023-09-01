#include <stdio.h>                  // C 语言标准输入输出函数库
#include <math.h>                   // C 数学函数库
#include <csl.h>
#include "DSPF_sp_autocor_cn.h"


#define DATALEN  		(512)

float data_in1[DATALEN] = {0};
float data_in2[DATALEN] = {0};
float data_out[DATALEN] = {0};



static void GenSineData(Uint32 sampleRate, Uint32 freqTONE, Uint32 amplify, float *Data_out, Uint32 NumOfUnit)
{
	int i;
	float pi = 3.1415926;
	float w = 0.0;
	float *pDataOut = 0;
	amplify = (amplify) >> 1 ;
	pDataOut = (float *)Data_out;
	w =2 * freqTONE * pi / sampleRate;
	for(i = 0; i < NumOfUnit; i++)
	{
		pDataOut[i] = ((float)(amplify * sin(w*i)));
	}

}

int main(void)
{
	unsigned int i = 0 , j = 0 ;

	int nr = 256;
	int nx = DATALEN - nr;

	GenSineData(20000, 4000, 1024, data_in1, DATALEN);
	GenSineData(20000, 2000, 128, data_in2, DATALEN);

	for(i = 0; i < DATALEN; i++)
	{
		data_in2[i] = data_in1[i] + data_in2[i];
	}

	DSPF_sp_autocor_cn(data_out, data_in2, nx, nr);

	return 0;
}

