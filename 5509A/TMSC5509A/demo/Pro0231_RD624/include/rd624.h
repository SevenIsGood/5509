#ifndef __RD624_H__
#define __RD624_H__

extern void Delay_1ms(unsigned int ms);
extern unsigned char RD624_StatusRead(void);
extern void RD624_Init(unsigned char ioAddr);

#endif