#include "myapp.h"
#define ST1_55        *(int *)0x03
#define ST3_55        *(int *)0x04
// ����ָʾ�ƼĴ�����ַ�ͼĴ�������
//#define LBDS (*((unsigned int *)0x401000))
//#define LBDS (*((unsigned int *)0x0C01800))
#define LBDS (*((unsigned int *)0x600C00))
//#define LBDS1 (*((unsigned int *)0x0C01000))
#define LBDS1 (*((unsigned int *)0x600400))
#define GPIO_SEL (*((unsigned int *)0x400000))
// �ӳ���ӿ�
void Delay(unsigned int nDelay);	// ��ʱ�ӳ���

main()
{
	unsigned int uLED[10]={0x0db,0x50,0x6b,0x73,0x0f0,0x0b3,0x0bb,0x51,0x0fb,0x0f3};
	int i;

	CLK_init();					// ��ʼ��DSP����ʱ��
	ST3_55 =  ST3_55|0x0040;    //����CE3��ӳ��
	SDRAM_init();				// ��ʼ��EMIF�ӿ�
	
	GPIO_SEL=0;
	
	while ( 1 )
	{
	/*	for ( i=0;i<5;i++ )
		{   
		//   ST1_55^=0x2000;
		    asm(" bset xf");
			LBDS=uLED[i];		// ����˳���Ϳ�����
			LBDS1=uLED[i];
			Delay(100);			// ��ʱ
		}
		for ( i=5;i>=0;i-- )
		{   
       //     ST1_55^=0x2000;		
		    asm(" bclr xf");
			LBDS=uLED[i];		// ����˳���Ϳ�����
			LBDS1=uLED[i];
			Delay(100);			// ��ʱ
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
			LBDS1=0x0ff-uLED[i];		// ����˳���Ϳ�����
			Delay(80);			// ��ʱ
	 	 }
        for ( i=9;i>=0;i-- )
		 {   
			LBDS1=0x0ff-uLED[i];		// ����˳���Ϳ�����
			Delay(80);			// ��ʱ
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
