#ifndef __HX711_H__
#define __HX711_H__

extern void Delay_1ms(unsigned int ms);
extern float HX711_GetWeight(void);
extern void HX711_Init(unsigned char clkAddr, unsigned char dataAddr);
#endif