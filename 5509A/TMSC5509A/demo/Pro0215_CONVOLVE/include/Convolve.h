#ifndef CONVOLVE_H_
#define CONVOLVE_H_

#include <stdio.h>
#include <csl.h>

void Convolve(
    Uint16 *Input,			//	ԭʼ��������
    Uint16 *Impulse,		//	�����Ӧ
    Uint16 *Output,			//	���������
    Uint16 length			//	������㳤��
);

#endif
