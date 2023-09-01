#ifndef __MQ_H__
#define __MQ_H__

extern void Delay_1ms(unsigned int ms);
extern unsigned char MQ_StatusRead(void);
extern void MQ_Init(unsigned char ioAddr);

#endif