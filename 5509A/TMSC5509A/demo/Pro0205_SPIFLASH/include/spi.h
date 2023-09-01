#ifndef __SPI_H__
#define __SPI_H__

/*
* mode	CPOL	CPHA
* mode 0	0	0
* mode 1	0	1
* mode 2	1	0
* mode 3	1	1
*/
typedef enum  {
    SPI_MODE_0 = 0,
    SPI_MODE_1 = 1,
    SPI_MODE_2 = 2,
    SPI_MODE_3 = 3
} SPI_CLK_MODE;

typedef  enum{
    SPI_8BIT  = 0,
    SPI_16BIT = 1,
    SPI_24BIT = 2,
    SPI_32BIT = 3
} SPI_BIT_LEN;

extern int * SPI_Open(int DevNum, int clk, SPI_BIT_LEN bitLen, SPI_CLK_MODE mode);
extern int SPI_Write(int *handle, char *buff, int len);
extern int SPI_Read(int *handle, char *inBuff, char *outBuff, int len);
#endif