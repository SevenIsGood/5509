//keyboard code
#include "myapp.h"
#define ST1_55        *(int *)0x03
#define ST3_55        *(int *)0x04
// 定义指示灯寄存器地址和寄存器类型
#define LBDS1 (*((unsigned int *)0x600400))
#define keyboard (*((unsigned int *)0x600C00))
#define GPIO_SEL (*((unsigned int *)0x400000))
// 子程序接口
void Delay(unsigned int nDelay);	// 延时子程序

unsigned int time=0;
unsigned int temp_key,temp_key1;
unsigned int keycode[100];

main()
{
//	unsigned int uLED[5]={0,1,2,4,0xFF};	// 控制字，逐位置1: 0001B 0010B 0100B 1000B
    unsigned int uLED[10]={0x0db,0x50,0x6b,0x73,0x0f0,0x0b3,0x0bb,0x51,0x0fb,0x0f3};

	CLK_init();					// 初始化DSP运行时钟
	ST3_55 =  ST3_55|0x0040;    //定义CE3的映射
	SDRAM_init();				// 初始化EMIF接口
	
	GPIO_SEL=0;
	
	while ( 1 )
	{
	  temp_key=keyboard;
	  temp_key=temp_key & 0x00ff;
	  if(0xff!=temp_key)
	  { 
	     temp_key1=temp_key;
	     Delay(20);
         temp_key=keyboard;
	     temp_key=temp_key & 0x00ff;
         if(temp_key == temp_key1)
		 {
		  //time++;
          //keycode[time]=temp_key;
		  switch(temp_key)
		  {
			case 0x0fb:
				LBDS1=0x0ff-uLED[1];
				break;
			case 0x0f7:
					LBDS1=0x0ff-uLED[2];
				break;
			case 0x0ef:
				LBDS1=0x0ff-uLED[3];
				break;
			case 0x0df:
					LBDS1=0x0ff-uLED[4];
				break;
			case 0x0bf:
				LBDS1=0x0ff-uLED[5];
				break;
			case 0x07f:
				LBDS1=0x0ff-uLED[6];
				break;
			case 0x0fd:
				LBDS1=0x0ff-uLED[7];
				break;
			case 0x0fe:
					LBDS1=0x0ff-uLED[8];
				break;
			default:break;
		  }
		  
		 }
	  }
	}
}

void Delay(unsigned int nDelay)
{
	unsigned int ii,jj,kk=0;
	for ( ii=0;ii<nDelay;ii++ )
	{
		for ( jj=0;jj<2048;jj++ )
		{
			kk++;
		}
	}
}
