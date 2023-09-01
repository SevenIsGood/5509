#include "Convolve.h"


void Convolveok(
     float *Input,			//	ԭʼ��������
     float *Impulse,		//	�����Ӧ
     float *Output,		//	���������
     Uint32 length 			//  ������г���
)
{
	int i,k,p;
	float r;

	p=0;
	for (k=0; k<=length-1; k++)
	{
		Output[k]=0;
		r=0;
		for (i=0; i<=p; i++)
		{
			r=Input[k-i] * Impulse[i];
			Output[k] = Output[k]+r;
		}
		p=p+1;
		if (p>length-1)
			p=length-1;
		else
			p=p;
	 }

	p=length-2;
	for (k=length; k<=length+length-1; k++)
	{
	 	Output[k]=0;
		r=0;
		for (i=0; i<=p; i++)
		{
			r = Input[length-1-i] * Impulse[length-1-p+i];
			Output[k] = Output[k]+r;
		}
	    p=p-1;
	}
	return ;
}
