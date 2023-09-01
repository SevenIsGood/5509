#ifndef __SR04_H__
#define __SR04_H__

extern void SR04_Trig(void);
extern unsigned int SR04_GetDistance(void);
extern void SR04_Init(unsigned char echoAddr, unsigned char trigAddr);
extern unsigned int SR04_GetDistance(void);
extern unsigned char SR04_IsReady(void);
extern void Delay_1ms(unsigned int ms);
#endif