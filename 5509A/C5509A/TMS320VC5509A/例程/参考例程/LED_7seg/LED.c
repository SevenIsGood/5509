#include "myapp.h"
#define ST1_55        *(int *)0x03
#define ST3_55        *(int *)0x04
// 定义指示灯寄存器地址和寄存器类型
//#define LBDS (*((unsigned int *)0x401000))
//#define LBDS (*((unsigned int *)0x0C01800))
#define LBDS (*((unsigned int *)0x600C00))
//#define LBDS1 (*((unsigned int *)0x0C01000))
#define LBDS1 (*((unsigned int *)0x600400))
#define GPIO_SEL (*((unsigned int *)0x400000))
// 子程序接口
void Delay(unsigned int nDelay);	// 延时子程序

main()
{
	unsigned int uLED[10]={0x0db,0x50,0x6b,0x73,0x0f0,0x0b3,0x0bb,0x51,0x0fb,0x0f3};
	int i;

	CLK_init();					// 初始化DSP运行时钟
	ST3_55 =  ST3_55|0x0040;    //定义CE3的映射
	SDRAM_init();				// 初始化EMIF接口
	
	GPIO_SEL=0;
	
	while ( 1 )
	{
	/*	for ( i=0;i<5;i++ )
		{   
		//   ST1_55^=0x2000;
		    asm(" bset xf");
			LBDS=uLED[i];		// 正向顺序送控制字
			LBDS1=uLED[i];
			Delay(100);			// 延时
		}
		for ( i=5;i>=0;i-- )
		{   
       //     ST1_55^=0x2000;		
		    asm(" bclr xf");
			LBDS=uLED[i];		// 反向顺序送控制字
			LBDS1=uLED[i];
			Delay(100);			// 延时
		}*/


			for(i=3;i>0;i--)
		{
		  LBDS1=0x0fff;
		  Delay(100);
		  LBDS1=0;
		  Delay(100);
		}
	
	//	while(1)
	//	LBDS1=uLED[0];	

	 	 for ( i=0;i<10;i++ )
		 {   
			LBDS1=0x0ff-uLED[i];		// 正向顺序送控制字
			Delay(80);			// 延时
	 	 }
        for ( i=9;i>=0;i-- )
		 {   
			LBDS1=0x0ff-uLED[i];		// 正向顺序送控制字
			Delay(80);			// 延时
	 	 }
	
	
	}
}

void Delay(unsigned int nDelay)
{
	int ii,jj,kk=0;
	for ( ii=0;ii<nDelay;ii++ )
	{
		for ( jj=0;jj<2048;jj++ )
		{
			kk++;
		}
	}
}
