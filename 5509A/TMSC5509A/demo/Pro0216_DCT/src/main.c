/****************************************************************************/
/*                                                                          */
/*              离散余弦变换测试                                            */
/*                                                                          */
/*              2014年07月27日                                              */
/*                                                                          */
/****************************************************************************/
// 离散余弦变换类似于离散傅里叶变换，不过离散余弦变换只适用于实数
// 图像的离散余弦变换是 JPEG 压缩的核心部分

#include <stdio.h>                  // C 语言标准输入输出函数库
#include <math.h>                   // C 数学函数库
#include <csl.h>
#include <csl_pll.h>
#include <csl_chiphal.h>
#include <csl_emif.h>
#include "Dct.h"                    // DSP 图像函数库
#include "Pic.h"
/****************************************************************************/
/*                                                                          */
/*              宏定义                                                      */
/*                                                                          */
/****************************************************************************/
// 离散余弦变换测试

#define N 8
#define IMAGE_WIDTH    128
#define IMAGE_HEIGHT   80

// Q15 (15-bit fixed-point) representation of constants
#define SQRT2_OVER_2  23170 // sqrt(2)/2 * 32768
#define PI_OVER_16    6433   // (pi/16) * 32768
// Q15 (15-bit fixed-point) representation of constants
#define SQRT2_OVER_2 23170 // sqrt(2)/2 * 32768
#define PI 32768            // 3.141592653589793 * 32768

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
EMIF_Config emiffig = {
  0x221, 	//EGCR  : the MEMFREQ = 00,the clock for the memory is equal to cpu frequence
  			//		  the WPE = 0 ,forbiden the writing posting when we debug the EMIF
  			//        the MEMCEN = 1,the memory clock is reflected on the CLKMEM pin
  			//        the NOHOLD = 1,HOLD requests are not recognized by the EMIF
  0xFFFF,	//EMI_RST: any write to this register resets the EMIF state machine
  0x3FFF,	//CE0_1:  CE0 space control register 1
  			//        MTYPE = 011,Synchronous DRAM(SDRAM),16-bit data bus width
  0xFFFF,   //CE0_2:  CE0 space control register 2
  0x00FF,   //CE0_3:  CE0 space control register 3
  			//        TIMEOUT = 0xFF;
  0x1FFF,	//CE1_1:  CE0 space control register 1
            //        Asynchronous, 16Bit
  0xFFFF,	//CE1_2:  CE0 space control register 2
  0x00FF,	//CE1_3:  CE0 space control register 3
  0x1FFF,	//CE2_1:  CE0 space control register 1
            //        Asynchronous, 16Bit
  0xFFFF,	//CE2_2:  CE0 space control register 2
  0x00FF,	//CE2_3:  CE0 space control register 3

  0x7FFF,	//CE3_1:  CE0 space control register 1
  0xFFFF,	//CE3_2:  CE0 space control register 2
  0x00FF,	//CE3_3:  CE0 space control register 3

  0x2911,   //SDC1:   SDRAM control register 1
  			//		  TRC = 8
  			//        SDSIZE = 0;SDWID = 0
  			//        RFEN = 1
  			//        TRCD = 2
  			//        TRP  = 2
  0x0410,	//SDPER : SDRAM period register
  			//		  7ns *4096
  0x07FF,    //SDINIT: SDRAM initialization register
  			//        any write to this register to init the all CE spaces,
  			//        do it after hardware reset or power up the C55x device
  0x0131	//SDC2:	  SDRAM control register 2
  			//        SDACC = 0;
  			//        TMRD = 01;
  			//        TRAS = 0101;
  			//        TACTV2ACTV = 0001;
  };

/*锁相环的设置*/
PLL_Config  myConfig	  = {
	0, 	//IAI: the PLL locks using the same process that was underway
		//before the idle mode was entered
	1, 	//IOB: If the PLL indicates a break in the phase lock,
		//it switches to its bypass mode and restarts the PLL phase-locking
		//sequence
	24, //PLL multiply value; multiply 24 times
	1 	//Divide by 2 PLL divide value; it can be either PLL divide value
		//(when PLL is enabled), or Bypass-mode divide value
		//(PLL in bypass mode, if PLL multiply value is set to 1)
};

// 输入缓存
#pragma DATA_ALIGN(IMG_In, 8);
#pragma DATA_SECTION(IMG_In, ".image");
unsigned short IMG_In[IMAGE_WIDTH * IMAGE_HEIGHT];
// 输出
#pragma DATA_ALIGN(IMG_Out, 8);
#pragma DATA_SECTION(IMG_Out, ".image");
unsigned short IMG_Out[IMAGE_WIDTH * IMAGE_HEIGHT];

#pragma DATA_ALIGN(dctResult, 8);
#pragma DATA_SECTION(dctResult, ".image");
short dctResult[IMAGE_WIDTH * IMAGE_HEIGHT];

/****************************************************************************/
/*                                                                          */
/*              函数声明                                                    */
/*                                                                          */
/****************************************************************************/
void SDRAM_init(void);
// 读取图片数组
void Read_IMG(const unsigned char *p);

/****************************************************************************/
/*                                                                          */
/*              函数                                                        */
/*                                                                          */
/****************************************************************************/
// 读取图片数组
void Read_IMG(const unsigned char *p)
{
	unsigned short i,j;

	printf("正在读取图像...\n");

	// 复制图像数组
	for( i=0; i<IMAGE_HEIGHT; i++ )
		for( j=0; j<IMAGE_WIDTH; j++ )
		{
			IMG_In[ i*IMAGE_WIDTH+j ] = ((*(p+1))<<8)+(*p);
			p +=2;
		}

	printf("图像读取完成!\n");
}

// Function to fill the cosLookupTable
/*
void fillCosLookupTable() {
    int n,k;
    for (k = 0; k < N; k++) {
        for (n = 0; n < N; n++) {
        	cosLookupTableGen[k][n] = (short)(SQRT2_OVER_2 * cos((2.0 * n + 1) * k * PI / (2.0 * N)));
        }
    }
}
*/
// Function to perform 1D DCT on a block of data
void perform1DDCT(short data[N], short result[N]) {
    short tmp[N];
    int n,k;

    short cosLookupTable[N][N] = {
        {23170, 23170, 23170, 23170, 23170, 23170, 23170, 23170},
        {32138, 27245, 18205, 6393, -6393, -18205, -27245, -32138},
        {30274, 12540, -12540, -30274, -30274, -12540, 12540, 30274},
        {27245, -6393, -32138, -18205, 18205, 32138, 6393, -27245},
        {23170, -23170, -23170, 23170, 23170, -23170, -23170, 23170},
        {18205, -32138, 6393, 27245, -27245, -6393, 32138, -18205},
        {12540, -30274, 30274, -12540, -12540, 30274, -30274, 12540},
        {6393, -18205, 27245, -32138, 32138, -27245, 18205, -6393},
    };

    for (k = 0; k < N; k++) {
        int sum = 0;
        for (n = 0; n < N; n++) {
            sum += (int)data[n] * (int)(cosLookupTable[k][n]);
        }
        result[k] = (short)((sum * SQRT2_OVER_2) >> 15);
    }
}


// Function to perform 1D IDCT on a block of data
void perform1DIDCT(short data[N], short result[N]) {
    short tmp[N];
    int n,k;

    short cosLookupTable[N][N] = {
        {23170, 23170, 23170, 23170, 23170, 23170, 23170, 23170},
        {32138, 27245, 18205, 6393, -6393, -18205, -27245, -32138},
        {30274, 12540, -12540, -30274, -30274, -12540, 12540, 30274},
        {27245, -6393, -32138, -18205, 18205, 32138, 6393, -27245},
        {23170, -23170, -23170, 23170, 23170, -23170, -23170, 23170},
        {18205, -32138, 6393, 27245, -27245, -6393, 32138, -18205},
        {12540, -30274, 30274, -12540, -12540, 30274, -30274, 12540},
        {6393, -18205, 27245, -32138, 32138, -27245, 18205, -6393},
    };

    for (n = 0; n < N; n++) {
        int sum = 0;
        for (k = 0; k < N; k++) {
            short coef = (k == 0) ? SQRT2_OVER_2 : 32768;
            sum += ((int)data[k] * coef * ((2 * n + 1) * k * PI_OVER_16)) >> 15;
        }
        tmp[n] = (short)((sum + 16384) >> 15); // Rounding
    }

    for (n = 0; n < N; n++) {
        int sum = 0;
        for (k = 0; k < N; k++) {
            sum += ((int)tmp[k] * ((2 * n + 1) * k * PI_OVER_16)) >> 15;
        }
        result[n] = (short)((sum + 16384) >> 15); // Rounding
    }
}

// Function to perform DCT on a block of the image
void performDCT(short image[], short dctResult[]) {
    int x, y;
	int i,j;
    for (y = 0; y < IMAGE_HEIGHT; y += N) {
        for (x = 0; x < IMAGE_WIDTH; x += N) {
            // Extract an N x N block from the image
            short block[N * N];
            for (i = 0; i < N; i++) {
                for (j = 0; j < N; j++) {
                    block[i * N + j] = image[(y + i) * IMAGE_WIDTH + x + j];
                }
            }

            // Perform 1D DCT on rows
            short temp[N * N];
            for (i = 0; i < N; i++) {
                perform1DDCT(&block[i * N], &temp[i * N]);
            }

            // Perform 1D DCT on columns
            for (j = 0; j < N; j++) {
                short col[N];
                for (i = 0; i < N; i++) {
                    col[i] = temp[i * N + j];
                }
                perform1DDCT(col, &dctResult[y * IMAGE_WIDTH + x + j]);
            }
        }
    }
}

// Function to perform IDCT and reconstruct the image
void performIDCT(short dctData[], short reconstructedImage[]) {
    int x, y;
	int i,j;
    for (y = 0; y < IMAGE_HEIGHT; y += N) {
        for (x = 0; x < IMAGE_WIDTH; x += N) {
            // Perform 1D IDCT on columns
            short temp[N * N];
            for (j = 0; j < N; j++) {
                short col[N];
                for (i = 0; i < N; i++) {
                    col[i] = dctData[(y + i) * IMAGE_WIDTH + x + j];
                }
                perform1DIDCT(col, &temp[j]);
            }

            // Perform 1D IDCT on rows
            for (i = 0; i < N; i++) {
                short row[N];
                for (j = 0; j < N; j++) {
                    row[j] = temp[i * N + j];
                }
                perform1DIDCT(row, &reconstructedImage[(y + i) * IMAGE_WIDTH + x]);
            }
        }
    }
}

unsigned int datacount = 0;
int databuffer[1000] = {0};
int *souraddr, *deminaddr;


/****************************************************************************/
/*                                                                          */
/*              主函数                                                      */
/*                                                                          */
/****************************************************************************/
int main(void)
{
	CSL_init();

	/*EMIF为全EMIF接口*/
	CHIP_RSET(XBSR,0x0a01);

	/*设置系统的运行速度为144MHz*/
	PLL_config(&myConfig);

	EMIF_config(&emiffig);

	int error = 0;
    datacount = 0;
     /*向SDRAM中写入数据*/
     souraddr = (int *)0x40000;
     deminaddr = (int *)0x41000;
     int i = 0;
     while (souraddr < deminaddr)
     {
             *souraddr++ = datacount;
             datacount++;
             // for (i = 0; i < 100; ++i);
     }

     /*读出SRAM中的数据*/
     souraddr = (int *)0x40000;
     datacount = 0;
     while (souraddr < deminaddr)
     {
             databuffer[datacount++] = *souraddr++;
             if (databuffer[datacount - 1] != (datacount - 1))
                     error++;
             // for (i = 0; i < 100; ++i);
     }
     if (error == 0)
             printf("SDRAM test completed! No Error!");
     else
             printf("SDRAM test failed!");
     while (1)
             ;






	//读取图片数组
	Read_IMG((char *)gImage);



	performDCT(IMG_In, dctResult);

	performIDCT(dctResult, IMG_Out);

	return 0;
}
