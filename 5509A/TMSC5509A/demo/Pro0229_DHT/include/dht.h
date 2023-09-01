#ifndef __DHT11_H__
#define __DHT11_H__
//设定标志(static unsigned char status)的宏值
#define OK              1
#define ERROR           0

extern void Delay_1ms(unsigned int ms);
//函数的返回值表示读取数据是否成功 OK 表示成功 ERROR 表示失败
extern unsigned char DHT11_ReadTempAndHumi(void);
extern int DHT11_GetTemperature(void);
extern int DHT11_GetHumidity(void);
extern void DHT11_Init(unsigned char ioAddr);


#endif