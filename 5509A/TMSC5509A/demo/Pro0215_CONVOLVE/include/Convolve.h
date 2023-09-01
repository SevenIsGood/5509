#ifndef CONVOLVE_H_
#define CONVOLVE_H_

#include <stdio.h>
#include <csl.h>

void Convolve(
    Uint16 *Input,			//	原始输入数据
    Uint16 *Impulse,		//	冲击响应
    Uint16 *Output,			//	卷积输出结果
    Uint16 length			//	卷积运算长度
);

#endif
