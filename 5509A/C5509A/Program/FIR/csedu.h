#include "scancode.h"
#include<stdlib.h>
#include<math.h>

#define LCDCMDTURNON 0x3f
#define LCDCMDTURNOFF 0x3e
#define LCDCMDSTARTLINE 0xc0
#define LCDCMDPAGE 0xb8
#define LCDCMDVERADDRESS 0x40


struct struLCDGraph
{
	void *pData;
	unsigned int uDataMode;
	unsigned int uDataLength;
	unsigned int uMaxValue;
	unsigned int uWindowX0,uWindowY0,uWindowX1,uWindowY1;
	int nOriginX,nOriginY;
	unsigned int uLineMode;
	unsigned int bShowXCcoordinate; /* 是否绘制x坐标轴 */
	unsigned int bShowYCcoordinate; /* 是否绘制y坐标轴 */
};

void LCDSetOrigin(int nX,int nY);
void LCDSetScreenBuffer(unsigned int *_pScreenBuffer);
void LCDTurnOn();
void LCDTurnOff();
void LCDCLS();
void _LCDCLS();
void LCDSetDelay(unsigned int nDelay);
void _Delay(unsigned int nTime);
void LCDRefreshScreen();
void LCDPutPixel(int x,int y,unsigned int color);
void LCDGraph(struct struLCDGraph *Gstru);
void LCDWriteBytes(unsigned int *pData,int x,int y,unsigned color);
void LCDPutCString(unsigned int *pData,int x,int y,unsigned int nCharNumber,unsigned color);
void LCDGraph2(struct struLCDGraph *Gstru,struct struLCDGraph *Gstru1);
void LCDDrawGraph(struct struLCDGraph *Gstru);

unsigned int _nOriginX=0,_nOriginY=0;
unsigned int *_pScreen;
unsigned int _nXLength=128,_nYLength=64,_nXWord=64,_nYWord=8;
unsigned int _bRefreshMode=0;
unsigned int _nLCDDelay=0;
unsigned int _nScreenX0=0,_nScreenX1=127,_nScreenY0=0,_nScreenY1=63;

void LCDSetDelay(unsigned int nDelay)
{
	_nLCDDelay=nDelay;
}

void LCDSetOrigin(int nX,int nY)
{
	_nOriginX=nX; _nOriginY=nY;
}

void LCDSetScreenBuffer(unsigned int *_pScreenBuffer)
{
	_pScreen=_pScreenBuffer;
}
 

// functions
void ShowTitle();
void LCDPutString(unsigned int *pData,int x,int y,unsigned int nCharNumber,unsigned color);
void interrupt Timer(void);
void LBD();
void ShiftToLeftAndRight(unsigned int nInit);
void Sound();
//void Delay(unsigned int nDelay);
//void RefreshLEDArray();			// 刷新显示
//void SetLEDArray(int nNumber);		// 修改显示内容
//char ConvertScanToChar(unsigned char cScanCode);	// 将键盘扫描码转换为字符
void Keyboard();
void Led();
void AD();
void FFT();
void DCMotor();
void SAMotor();
//
// Functions
void InitInterrupt(void);			// 初始化中断
void InitCTR();						// 初始化ICETEK-CTR
void CloseCTR();					// 关闭ICETEK-CTR上各设备
void Delay(unsigned int nTime);		// 延时子程序
void Delay1(unsigned int nTime);	// 延时子程序(带LED显示刷新)
unsigned char GetKey();				// 读取键盘输入
void RefreshLEDArray();				// 刷新发光二极管阵列显示
void SetLEDArray(int nNumber);		// 修改发光二极管阵列显示内容
char ConvertScanToChar(unsigned char cScanCode);	// 将键盘扫描码转换成字符
void LCDCMD(unsigned char dbCommand);	// 向液晶显示器发送命令
void LCDWriteLeft(unsigned char dbData);	// 写液晶显示器左半屏
void LCDCLS();							// 清除液晶显示器显示
void TurnOnLCD();						// 打开液晶显示器
// AD  ----------------------------------------------------------
ioport unsigned int *ADCCTL1=(unsigned int *)0x6800;
ioport unsigned int *ADCDATA1=(unsigned int *)0x6801;
ioport unsigned int *ADCCLKDIV1=(unsigned int *)0x6802;
ioport unsigned int *ADCCLKCTL1=(unsigned int *)0x6803;
#define ADCCTL (*ADCCTL1)
#define ADCDATA (*ADCDATA1)
#define ADCCLKDIV (*ADCCLKDIV1)
#define ADCCLKCTL (*ADCCLKCTL1)

// McBSP0  ------------------------------------------------------
ioport unsigned int *SPCR20=(unsigned int *)0x2804;
ioport unsigned int *SPCR10=(unsigned int *)0x2805;
ioport unsigned int *PCR_0=(unsigned int *)0x2c12;
#define SPCR1_0 (*SPCR10)
#define SPCR2_0 (*SPCR20)
#define PCR0 (*PCR_0)
// McBSP1  ------------------------------------------------------
ioport unsigned int *SPCR21=(unsigned int *)0x2c04;
ioport unsigned int *PCR_1=(unsigned int *)0x2c12;
#define SPCR2_1 (*SPCR21)
#define PCR1 (*PCR_1)
// McBSP2  ------------------------------------------------------
ioport unsigned int *SPCR12=(unsigned int *)0x3005;
ioport unsigned int *PCR_2=(unsigned int *)0x3012;
#define SPCR1_2 (*SPCR12)
#define PCR2 (*PCR_2)

// ICETEK-VC5509-A -----------------------------------------------
#define LBDS (*((unsigned int *)0x40001))
#define DIPS (*(unsigned int *)0x400002)
// ICETEK-CTR  --------------------------------------------------
#define CTRGR (*(unsigned int *)0x600800) //port8000
#define CTRKEY (*(unsigned int *)0x602800) // port8001
#define CTRCLKEY (*(unsigned int *)0x600801)
#define CTRLA (*(unsigned int *)0x602802)  // port8005
#define CTRLR (*(unsigned int *)0x602803)  // port8007
#define CTRLCDCMDR (*(unsigned int *)0x602800) // port8001
#define CTRLCDCR (*(unsigned int *)0x600801) // port8002
#define CTRLCDLCR (*(unsigned int *)0x602801) // port8003
#define CTRLCDRCR (*(unsigned int *)0x600802) // port8004
// Special values -----------------------------------------------
#define LCDCMDTURNON 0x3f
#define LCDCMDTURNOFF 0x3e
#define LCDCMDSTARTLINE 0xc0
#define LCDCMDPAGE 0xb8
#define LCDCMDVERADDRESS 0x40
//

// for Timer
#define MODEMUSIC 1
#define MODEDCMOTOR 2
#define MODESAMOTOR 3
#define MODEADC 4
#define MODEFFT 5

int uWork0,uWork1,k0,k1,nAD;

// for AD
#define ADCNUMBER 150
int nADBuf1[ADCNUMBER],nADBuf2[ADCNUMBER];

// for SAMotor
#define FREQSA 0x0f00
extern unsigned int pwm[8];//={ 0x86,0x87,0x83,0x8b,0x89,0x8d,0x8c,0x8e };
int nAddStep,nStep;

// for DCMotor
#define PWMDC 0x0200
int uN,nCount,nCount1,nDir;

// for CTR.lib
//struct struLCDGraph struGraph,struGraph1;
//unsigned int nScreenBuffer[1024];
int nGraphBuf1[ADCNUMBER],nGraphBuf2[ADCNUMBER];

// for LCD control
#define LCDDELAY 1
#define LCDCMDTURNON 0x3f
#define LCDCMDTURNOFF 0x3e
#define LCDCMDSTARTLINE 0xc0
#define LCDCMDPAGE 0xb8
#define LCDCMDVERADDRESS 0x40
#define LCDLEFTPAGE 0
#define LCDRIGHTPAGE 1
#define LCDDATABYTE 1
#define LCDDATAWORD 2
#define STRINGREVERSE 1
#define STRINGNOREVERSE 0

// led
unsigned int uLed[12][2]={ {1,0x40},{2,0x40},{4,0x40},
						   {0,0x41},{0,0x42},{0,0x44},
						   {8,0x40},{0x10,0x40},{0x20,0x40},
						   {0,0x48},{0,0x50},{0,0x60},
						 };

// sound
//#define nMusicNumber 40
//extern unsigned int music[nMusicNumber][2];//=
//{
//	{182,480},{151,480},{135,480},{121,480},{135,480},{151,480},{182,480},{0,480},
//	{182,480},{151,480},{135,480},{121,480},{135,480},{151,480},{182,480},{0,480},
//	{182,240},{151,240},{135,240},{121,240},{135,240},{151,240},{182,240},{0,240},
//	{182,240},{151,240},{135,240},{121,240},{135,240},{151,240},{182,240},{0,240},
//	{182,1920},{151,1920},{135,1920},{121,1920},{135,1920},{151,1920},{182,1920},{0,1920} 
//};

// for LedArray
extern unsigned char ledbuf[8],ledx[8];
unsigned int ledkey1[11][8]=
{
	{0x00,0x00,0x7C,0x82,0x82,0x82,0x7C,0x00},
	{0x00,0x00,0x00,0x84,0xFE,0x80,0x00,0x00},	//1
	{0x00,0x00,0x84,0xC2,0xA2,0x92,0x8C,0x00},	//2
	{0x00,0x00,0x44,0x92,0x92,0x92,0x6C,0x00},
	{0x00,0x00,0x30,0x28,0x24,0xFE,0x20,0x00},
	{0x00,0x00,0x4E,0x92,0x92,0x92,0x62,0x00},
	{0x00,0x00,0x7C,0x92,0x92,0x92,0x64,0x00},
	{0x00,0x00,0x02,0xC2,0x32,0x0A,0x06,0x00},
	{0x00,0x00,0x6C,0x92,0x92,0x92,0x6C,0x00},
	{0x00,0x00,0x4C,0x92,0x92,0x92,0x7C,0x00},
	{0xc0,0xe0,0x7f,0x03,0x03,0xc3,0xe3,0x7f}
};

// strings---------------------------------------------------------------
unsigned int str1[64]=
{	//瑞泰创新
	0x1042,0x1042,0x0FFE,0x0842,0x0442,0x00,0xFF40,0x15E,
	0x150,0x7FD0,0x15F,0x3F50,0x4150,0x815E,0x7F40,0x00,
	0x420,0x2422,0x222A,0x112A,0x12AA,0x4C6A,0x843F,0x7FAA,
	0x82A,0xC6A,0x12AA,0x212A,0x232A,0x622,0x220,0x00,
	0x40,0x20,0x3FD0,0x404C,0x4043,0x4244,0x4448,0x43D8,
	0x7830,0x10,0x00,0x7FC,0x2000,0x4000,0x3FFF,0x00,
	0x2120,0x1124,0x4D2C,0x8135,0x7FE6,0x534,0x592C,0x2124,
	0x1800,0x7FC,0x24,0x24,0xFFE2,0x22,0x22,0x00
};

unsigned int str2[80]=
{	//教学实验箱
	0x910,0x914,0x4894,0x88D4,0x7eBF,0x594,0x449C,0x4414,
	0x2050,0x20F8,0x130F,0xC08,0x3388,0xC078,0x4008,0x00,
	0x40,0x230,0x210,0x212,0x25C,0x254,0x4250,0x8251,
	0x7F5E,0x2D4,0x250,0x218,0x257,0x232,0x210,0x00,
	0x00,0x8210,0x820C,0x4204,0x424C,0x23B4,0x1294,0xA05,
	0x7F6,0xA04,0x1204,0xE204,0x4214,0x20C,0x204,0x00,
	0x802,0x8FA,0x482,0x2482,0x40FE,0x3F80,0x2240,0x2C60,
	0x2158,0x2E46,0x2048,0x3050,0x2C20,0x2320,0x2020,0x00,
	0x2010,0x1088,0xC87,0x382,0xFFF6,0x28A,0xC82,0x12,
	0xFFE8,0x4927,0x4922,0x4926,0x493A,0xFFE2,0x02,0x00
};
unsigned int str3[24]=
{	//LED
	0x2008,0x3FF8,0x2008,0x2000,0x2000,0x2000,0x3000,0x00,
	0x2008,0x3FF8,0x2088,0x2088,0x23E8,0x2008,0x1810,0x00,
	0x2008,0x3FF8,0x2008,0x2008,0x2008,0x1010,0xFE0,0x00
};
unsigned int str4[48]=
{	//蜂鸣器
	0x1000,0x31F8,0x1108,0x1FFF,0x908,0x1DF8,0x1890,0x1548,
	0x1547,0x152A,0xFF92,0x152A,0x1546,0x15C0,0x1040,0x00,
	0x00,0x3FC,0x204,0xA04,0xBFC,0x800,0xBFC,0xA04,
	0xa16,0xa65,0xa04,0x4a84,0x82FC,0x7e00,0x00,0x00,
	0x240,0x240,0x7E4F,0x4549,0x4549,0x44C9,0x7CCF,0x70,
	0x7CC0,0x44CF,0x4549,0x4559,0x7E69,0x64F,0x200,0x00
};
unsigned int str5[32]=
{	//键盘
	0x140,0x130,0x7FEF,0x2128,0x5128,0x2244,0x1464,0xFDC,
	0x1410,0x2554,0x3FFF,0x4554,0x4554,0x457C,0x4410,0x00,
	0x4420,0x4220,0x41A0,0x7C7C,0x4424,0x4426,0x7C6D,0x45B4,
	0x4424,0x7D24,0x4624,0x45FC,0x7C20,0x4020,0x4020,0x00
};
unsigned int str6[32]=
{	//电机
	0x00,0x00,0xFF8,0x448,0x448,0x448,0x448,0x3FFF,
	0x4448,0x4448,0x4448,0x4448,0x4FF8,0x4000,0x7000,0x00,
	0x408,0x308,0xC8,0xFFFF,0x48,0x4188,0x3008,0x0C00,
	0x3FE,0x02,0x02,0x02,0x3FFE,0x4000,0x7800,0x00
};
/*
unsigned int str7[16]=
{	//DA
	0x2008,0x3FF8,0x2008,0x2008,0x2008,0x1010,0xFE0,0x00,
	0x3f80,0x0840,0x0820,0x0810,0x0820,0x0840,0x3f80,0x0000
}; 
*/
unsigned int str8[96]=
{	//AD
//	0x2000,0x3C00,0x23C0,0x0238,0x02E0,0x2700,0x3800,0x2000,
//	0x2008,0x3FF8,0x2008,0x2008,0x2008,0x1010,0x0FE0,0x0000
// AD+混频+FFT  --  **  宋体, 12  **
// 当前所选字体下一个汉字对应的点阵为:  宽度x高度=88x16,  调整后为: 88x16
0x2000,0x3C00,0x23C0,0x0238,0x02E0,0x2700,0x3800,0x2000,
0x2008,0x3FF8,0x2008,0x2008,0x2008,0x1010,0x0FE0,0x0000,
0x0100,0x0100,0x0100,0x1FF0,0x0100,0x0100,0x0100,0x0000,
0x0408,0x0430,0xFF01,0x00C6,0x0030,0xFF00,0x447E,0x244A,
0x044A,0x004A,0x7FCA,0x844A,0x844A,0x827E,0xE000,0x0000,
0x4420,0x433C,0x2020,0x1020,0x0BBF,0x0424,0x4324,0x4000,
0x27F2,0x181A,0x07D6,0x0812,0x1012,0x27F2,0x6002,0x0000,
0x0100,0x0100,0x0100,0x1FF0,0x0100,0x0100,0x0100,0x0000,
0x2008,0x3FF8,0x2088,0x0088,0x03E8,0x0008,0x0010,0x0000,
0x2008,0x3FF8,0x2088,0x0088,0x03E8,0x0008,0x0010,0x0000,
0x0018,0x0008,0x2008,0x3FF8,0x2008,0x0008,0x0018,0x0000,
0,0,0,0,0,0,0,0
}; 
unsigned int str9[32]=
{	// 直流
	0x4000,0x4004,0x4004,0x7FF4,0x4A94,0x4A94,0x4A9C,0x4A97,
	0x4A94,0x4A94,0x4A94,0x7FF4,0x4004,0x4004,0x4000,0x0000,
	0x0410,0x0460,0xFC01,0x0386,0x4060,0x3004,0x0F44,0x0064,
	0x0055,0x7F4E,0x0044,0x3F64,0x40C4,0x4004,0x7004,0x0000
};
unsigned int str10[32]=
{	// 步进
	0x0000,0x4420,0x4420,0x4220,0x41BC,0x2020,0x2020,0x2020,
	0x17FF,0x1024,0x0824,0x0424,0x0324,0x0220,0x0020,0x0000,
	0x0080,0x4082,0x209C,0x1F88,0x2000,0x5088,0x4C88,0x43FF,
	0x4088,0x4088,0x4088,0x5FFF,0x4088,0x4088,0x4080,0x0000
};

/*
unsigned int str11[32]=
{	// 演示
	0x0410,0xFC21,0x0386,0x0060,0x0000,0x000C,0x8FD4,0x6954,
	0x0955,0x0FF6,0x0954,0x2954,0xCFD4,0x000C,0x0004,0x0000,
	0x1000,0x0820,0x0420,0x0322,0x0022,0x4022,0x8022,0x7FE2,
	0x0022,0x0022,0x0122,0x0222,0x0C22,0x1820,0x0020,0x0000
};
*/

unsigned int str12[32]=
{	// 闪灯
	0x0000,0xFFF8,0x0001,0x0806,0x0800,0x0400,0x0302,0x00F2,
	0x0102,0x0202,0x0C02,0x4002,0x8002,0x7FFE,0x0000,0x0000,
	0x4080,0x3070,0x0C00,0x03FF,0x0240,0x0430,0x0800,0x0004,
	0x0004,0x2004,0x4004,0x3FFC,0x0004,0x0004,0x0004,0x0000
};
/*
#define CCXRTCX 32
#define CCYRTCX 63
#define CCXJXSYX 24
#define CCYJXSYX 47
#define CCXFMQ 0
#define CCYFMQ 31
#define CCXJP 50
#define CCYJP 31
#define CCXLED 84
#define CCYLED 31
#define CCXAD 110
#define CCYAD 31
#define CCXZL 0
#define CCYZL 16
#define CCXDJ1 30
#define CCYDJ1 16
#define CCXBJ 66
#define CCYBJ 16
#define CCXDJ2 96
#define CCYDJ2 16
*/
#define CCXRTCX 0
#define CCYRTCX 63
#define CCXJXSYX 80
#define CCYJXSYX 63
#define CCXSD 0
#define CCYSD 47
#define CCXFMQ 34
#define CCYFMQ 47
#define CCXJP 84
#define CCYJP 47
#define CCXLED 100
#define CCYLED 31
#define CCXAD 0
#define CCYAD 31
#define CCXZL 0
#define CCYZL 16
#define CCXDJ1 30
#define CCYDJ1 16
#define CCXBJ 66
#define CCYBJ 16
#define CCXDJ2 96
#define CCYDJ2 16

// Variables
unsigned char ledbuf[8],ledx[8];
unsigned char ledkey[10][8]=
{
	{0x00,0x00,0x7C,0x82,0x82,0x82,0x7C,0x00},	//0
	{0x00,0x00,0x00,0x84,0xFE,0x80,0x00,0x00},	//1
	{0x00,0x00,0x84,0xC2,0xA2,0x92,0x8C,0x00},	//2
	{0x00,0x00,0x44,0x92,0x92,0x92,0x6C,0x00},
	{0x00,0x00,0x30,0x28,0x24,0xFE,0x20,0x00},
	{0x00,0x00,0x4E,0x92,0x92,0x92,0x62,0x00},
	{0x00,0x00,0x7C,0x92,0x92,0x92,0x64,0x00},
	{0x00,0x00,0x02,0xC2,0x32,0x0A,0x06,0x00},
	{0x00,0x00,0x6C,0x92,0x92,0x92,0x6C,0x00},
	{0x00,0x00,0x4C,0x92,0x92,0x92,0x7C,0x00}
};
#define nMusicNumber 52
unsigned int music[nMusicNumber][2]=
{
	{101,1920},{121,1440},{114,480},{101,1920},{151,1920},
	{151,480},{135,480},{121,480},{101,480},{114,960},{121,960},{135,3840},
	{121,480},{114,480},{101,480},{91,480},{101,960},{101,960},
	{76,1920},{101,1920},{114,960},{121,960},{135,1440},{151,480},{151,1920},{0,1920},
	{101,1920},{121,1440},{114,480},{101,1920},{151,1920},
	{151,480},{135,480},{121,480},{101,480},{114,960},{121,960},{135,3840},
	{121,480},{114,480},{101,480},{91,480},{101,960},{101,960},
	{76,1920},{101,1920},{114,960},{121,960},{135,1440},{151,480},{151,1920},{0,1920}
};
unsigned int pwm[8]={ 0x86,0x87,0x83,0x8b,0x89,0x8d,0x8c,0x8e };
unsigned char dbClearKey;


void CloseCTR()
{
    CTRGR=0; 				 
    CTRLR=0; CTRLR=0x40;
    CTRLR=0x0c0;
	LCDCMD(LCDCMDTURNOFF);	
	dbClearKey=CTRCLKEY;
	LBDS=0;
}

void LCDCMD(unsigned char dbCommand)
{
	CTRLCDCMDR=dbCommand;		
	CTRLCDCR=0;
}

unsigned char GetKey()
{
	unsigned char dbReturn;
	
	dbReturn=CTRKEY;
	dbClearKey=CTRCLKEY;
	return dbReturn;
}

char ConvertScanToChar(unsigned char cScanCode)
{
	char cReturn;
	
	cReturn=0;
	switch ( cScanCode )
	{
		case SCANCODE_0: cReturn='0'; break;
		case SCANCODE_1: cReturn='1'; break;
		case SCANCODE_2: cReturn='2'; break;
		case SCANCODE_3: cReturn='3'; break;
		case SCANCODE_4: cReturn='4'; break;
		case SCANCODE_5: cReturn='5'; break;
		case SCANCODE_6: cReturn='6'; break;
		case SCANCODE_7: cReturn='7'; break;
		case SCANCODE_8: cReturn='8'; break;
		case SCANCODE_9: cReturn='9'; break;
		case SCANCODE_Plus: cReturn='+'; break;
		case SCANCODE_Minus: cReturn='-'; break;
	}
	return cReturn;
}

void InitCTR()
{
	int i;
	
	for ( i=0;i<8;i++ )
	{
		ledbuf[i]=0x0ff;		// 显示为空白
		ledx[i]=(i<<4);	// 生成显示列控制字
	}
    CTRGR=0; CTRGR=0x80; CTRGR=0;
    CTRLR=0; CTRLR=0x40;
    CTRLR=0x0c0;
	LCDCMD(LCDCMDTURNOFF);	
	dbClearKey=CTRCLKEY;
	LBDS=0;
}
void _Delay(unsigned int nDelay)
{
	int jj,kk=0;
		for ( jj=0;jj<12;jj++ )
		{
			kk++;
		}
}

void Delay(unsigned int nDelay)
{
	int ii,jj,kk=0;
	for ( ii=0;ii<nDelay;ii++ )
	{
		for ( jj=0;jj<1024;jj++ )
		{
			kk++;
		}
	}
}

void Delay1(unsigned int nDelay)
{
	int ii,jj,kk=0;
	for ( ii=0;ii<nDelay;ii++ )
	{
		for ( jj=0;jj<64;jj++ )
		{
			RefreshLEDArray();
			kk++;
		}
	}
}

void RefreshLEDArray()
{
	int i;
	for ( i=0;i<8;i++ )
	{
		CTRGR=ledx[i];
		CTRLA=ledbuf[i];
	}
}

void SetLEDArray(int nNumber)
{
	int i;
	for ( i=0;i<8;i++ )
		ledbuf[i]=~ledkey[nNumber][7-i];
}

void LCDWriteLeft(unsigned char dbData)
{
	CTRLCDLCR=dbData;
	CTRLCDCR=0;
}

void LCDWriteRight(unsigned char dbData)
{
	CTRLCDRCR=dbData;
	CTRLCDCR=0;
}

void TurnOnLCD()
{
	LCDCMD(LCDCMDTURNON);
}
void LCDCLS()
{
	int i,j;
	unsigned int *pWork;
	
	CTRLCDCMDR=LCDCMDSTARTLINE; _Delay(_nLCDDelay); CTRLCDCR=0; _Delay(_nLCDDelay);
	pWork=_pScreen;
	for ( i=0;i<_nYWord;i++ )
	{
		CTRLCDCMDR=LCDCMDPAGE+i; _Delay(_nLCDDelay);
		CTRLCDCR=0;
		_Delay(_nLCDDelay);
		CTRLCDCMDR=LCDCMDVERADDRESS; _Delay(_nLCDDelay);
		CTRLCDCR=0;
		_Delay(_nLCDDelay);
		for ( j=0;j<_nXWord;j++,pWork++ )
		{
			CTRLCDLCR=0; _Delay(_nLCDDelay); CTRLCDCR=0;	_Delay(_nLCDDelay);
			(*pWork)=0;
		}
		CTRLCDCMDR=LCDCMDPAGE+i; _Delay(_nLCDDelay);
		CTRLCDCR=0;	_Delay(_nLCDDelay);
		CTRLCDCMDR=LCDCMDVERADDRESS; _Delay(_nLCDDelay);
		CTRLCDCR=0;	_Delay(_nLCDDelay);
		for ( j=0;j<_nXWord;j++,pWork++ )
		{
			CTRLCDRCR=0; _Delay(_nLCDDelay); CTRLCDCR=0;	_Delay(_nLCDDelay);
			(*pWork)=0;
		}
	}
}
void _LCDCLS()
{
	int i,j;
	LCDCMD(LCDCMDSTARTLINE);
	for ( i=0;i<8;i++ )
	{
		LCDCMD(LCDCMDPAGE+i);
		LCDCMD(LCDCMDVERADDRESS);
		for ( j=0;j<64;j++ )
			LCDWriteLeft(0);
		LCDCMD(LCDCMDPAGE+i);
		LCDCMD(LCDCMDVERADDRESS);
		for ( j=0;j<64;j++ )
			LCDWriteRight(0);
	}
}
 
void LCDPutString(unsigned int *pData,int x,int y,unsigned int nCharNumber,unsigned color)
{
	int i,j,l;
	unsigned int k,mcolor;
	
	for ( l=0;l<nCharNumber;l++ )
		for ( i=0;i<8;i++ )
		{
			k=1;
			for ( j=0;j<16;j++,k<<=1 )
			{
				if ( color==2 )	mcolor=2;
				else
				{
					mcolor=( pData[l*8+i]&k )?(1):(0);
					if ( color==0 )	mcolor=1-mcolor;
				}
				LCDPutPixel(x+l*8+i,y-j,mcolor);
			}
		}
	LCDRefreshScreen();
}

void ShowTitle()
{
	LCDPutCString(str1,CCXRTCX,CCYRTCX,4,1);
	LCDPutCString(str2+32,CCXJXSYX,CCYJXSYX,5,1);
	LCDPutCString(str12,CCXSD,CCYSD,2,1);

	LCDPutCString(str4,CCXFMQ,CCYFMQ,3,1);
	LCDPutCString(str5,CCXJP,CCYJP,2,1);
	LCDPutCString(str8,CCXAD,CCYAD,6,1);
//	LCDPutString(str8,CCXAD,CCYAD,2,1);
	LCDPutString(str3,CCXLED,CCYLED,3,1);
	LCDPutCString(str9,CCXZL,CCYZL,2,1);
	LCDPutCString(str6,CCXDJ1,CCYDJ1,2,1);
	LCDPutCString(str10,CCXBJ,CCYBJ,2,1);
	LCDPutCString(str6,CCXDJ2,CCYDJ2,2,1);
}

void LCDRefreshScreen()
{
	int i,j;
	int yl;
	unsigned int *pWork,nWork,nWork1;
//	if ( nRefreshMode==0 )
	{
		yl=_nYWord; pWork=_pScreen;
		CTRLCDCMDR=LCDCMDSTARTLINE; _Delay(_nLCDDelay);
		CTRLCDCR=0; _Delay(_nLCDDelay);
		for ( i=0;i<yl;i++ )
		{
			CTRLCDCMDR=LCDCMDPAGE+i; _Delay(_nLCDDelay);
			CTRLCDCR=0; _Delay(_nLCDDelay);
			CTRLCDCMDR=LCDCMDVERADDRESS; _Delay(_nLCDDelay);
			CTRLCDCR=0; _Delay(_nLCDDelay);
			for ( j=0;j<_nXWord;j++,pWork++ )
			{
				nWork=(*pWork)&0x0ff; nWork1=((*pWork)>>8);
				if ( nWork1==0 )
				{
					CTRLCDLCR=nWork; _Delay(_nLCDDelay);
					CTRLCDCR=0; _Delay(_nLCDDelay);
					(*pWork)=nWork;
				}
			}
		}
		for ( i=0;i<yl;i++ )
		{
			CTRLCDCMDR=LCDCMDPAGE+i; _Delay(_nLCDDelay);
			CTRLCDCR=0; _Delay(_nLCDDelay);
			CTRLCDCMDR=LCDCMDVERADDRESS; _Delay(_nLCDDelay);
			CTRLCDCR=0; _Delay(_nLCDDelay);
			for ( j=0;j<_nXWord;j++,pWork++ )
			{
				nWork=(*pWork)&0x0ff; nWork1=((*pWork)>>8);
				if ( nWork1==0 )
				{
					CTRLCDRCR=nWork; _Delay(_nLCDDelay);
					CTRLCDCR=0; _Delay(_nLCDDelay);
					(*pWork)=nWork;
				}
			}
		}
	}
}

void LCDPutPixel(int x,int y,unsigned int color)
{
	int nX,nY;
	unsigned int *pWork,nWork;
	
	nX=x+_nOriginX; nY=_nYLength-y-1;
	if ( nX<_nScreenX0 || nX>_nScreenX1 )	return;
	if ( nY<_nScreenY0 || nY>_nScreenY1 )	return;
	nWork=( nX<_nXWord )?(nX):(nX+448);
	nWork+=((nY/8)*_nXWord);
	pWork=_pScreen+nWork;
	nWork=(0x80>>(7-nY%8));
	if ( color==2 )
	{
		(*pWork)^=nWork;
	}
	else if ( color==0 )
	{
		nWork=(~nWork)&0x0ff;
		(*pWork)&=nWork;
	}
	else
	{
		(*pWork)|=nWork;
	}
	(*pWork)&=0x0ff;
}

void LCDDrawLine(int x0, int y0, int x1, int y1, unsigned char color)
{
	int x,y;
	int dx,dy,t,error,xd,yd;

	if ( (x0==x1)&&(y0==y1) ) LCDPutPixel(x0,y0,color);
	else	if ( y0==y1 )
	{	if ( x0>x1 )	{ t=x0; x0=x1; x1=t; t=y0; y0=y1; y1=t; }
	 	for ( x=x0;x<=x1;x++ )	LCDPutPixel(x,y0,color);
    } 
    else	if ( x0==x1 )
	{	if ( y0>y1 ) { t=x0; x0=x1; x1=t; t=y0; y0=y1; y1=t; }
	   	for ( y=y0;y<=y1;y++ ) LCDPutPixel(x0,y,color);
	}
	else
	{	xd=abs(x1-x0); yd=abs(y1-y0);
	   	if ( xd>=yd )
	    {	if ( x0>x1 ) { t=x0; x0=x1; x1=t; t=y0; y0=y1; y1=t; }
	       	error=(yd<<1)-(xd); LCDPutPixel(x0,y0,color);
	       	x=x0; y=y0; dy=(y1>y0)?1:(-1);
	       	while ( x<x1 )
		 	{	x++;
		   		if ( error<0 ) error+=(yd<<1); else { error+=((yd-xd)<<1); y+=dy; }
		   		LCDPutPixel(x,y,color);
		 	}
	    }
	  	else
	   	{	if ( y0>y1 ) { t=x0; x0=x1; x1=t; t=y0; y0=y1; y1=t; }
	     	error=(xd<<1)-(yd); LCDPutPixel(x0,y0,color);
	     	x=x0; y=y0; dx=(x1>x0)?1:(-1);
	     	while ( y<y1 )
	      	{	y++;
				if ( error<0 ) error+=(xd<<1); else { error+=((xd-yd)<<1); x+=dx; }
				LCDPutPixel(x,y,color);
	      	}
	   	}
	}
}

#define DATAINTMODE 0
#define DATAUINTMODE 1
#define LINEMODE 0
#define PIXELMODE 1
#define BARMODE 2

void LCDGraph(struct struLCDGraph *Gstru)
{
	LCDDrawGraph(Gstru);
	LCDRefreshScreen();
}

void LCDGraph2(struct struLCDGraph *Gstru,struct struLCDGraph *Gstru1)
{
	LCDDrawGraph(Gstru);
	LCDDrawGraph(Gstru1);
	LCDRefreshScreen();
}

void LCDDrawGraph(struct struLCDGraph *Gstru)
{
	unsigned int *pUint,uWork,sx0,sy0,sx1,sy1,*puWork;
	int *pInt,*pnWork,nWork,nx0,ny0,nx1,ny1;
	int i;
	float fWork,fxSample,fySample;
	
	if ( Gstru->bShowXCcoordinate )	LCDDrawLine(Gstru->nOriginX,Gstru->nOriginY,Gstru->nOriginX,Gstru->uWindowY1,1);
	if ( Gstru->bShowYCcoordinate )	LCDDrawLine(Gstru->nOriginX,Gstru->nOriginY,Gstru->uWindowX1,Gstru->nOriginY,1); 
	sx0=_nScreenX0; sx1=_nScreenX1; sy0=_nScreenY0; sy1=_nScreenY1;
	pInt=(int *)(Gstru->pData); pUint=(unsigned int *)(Gstru->pData);
	fySample=( Gstru->nOriginY==Gstru->uWindowY0 )?((float)(Gstru->uMaxValue)/abs(Gstru->uWindowY1-Gstru->uWindowY0)):((float)(Gstru->uMaxValue)/abs(Gstru->nOriginY-Gstru->uWindowY0));
	fySample=-fySample;
	fxSample=(float)(Gstru->uDataLength)/abs(Gstru->uWindowX1-Gstru->uWindowX0);
	pnWork=pInt; puWork=pUint;
	ny0=Gstru->nOriginY-(*pnWork)/fySample; nx0=Gstru->nOriginX; nx1=nx0+1;
	_nScreenX0=Gstru->uWindowX0; _nScreenX1=Gstru->uWindowX1; _nScreenY0=_nYLength-Gstru->uWindowY1; _nScreenY1=_nYLength-Gstru->uWindowY0;
	for ( i=1;i<Gstru->uWindowX1-Gstru->uWindowX0;i++,nx1++ )
	{
		uWork=i*fxSample;
		switch ( Gstru->uDataMode )
		{
			case DATAINTMODE:
				ny1=Gstru->nOriginY-(*(pnWork+uWork))/fySample;
				break;
			case DATAUINTMODE:
				ny1=Gstru->nOriginY-(*(puWork+uWork))/fySample;
				break;
		}
		switch ( Gstru->uLineMode )
		{
			case LINEMODE:
				LCDDrawLine(nx0,ny0,nx1,ny1,1);
				break;
			case PIXELMODE:
				LCDPutPixel(nx1,ny1,1);
				break;
			case BARMODE:
				LCDDrawLine(nx1,Gstru->nOriginY,nx1,ny1,1);
				break;
		}
		nx0=nx1; ny0=ny1;
	}
	_nScreenX0=sx0; _nScreenX1=sx1; _nScreenY0=sy0; _nScreenY1=sy1;
}
void LCDWriteBytes(unsigned int *pData,int x,int y,unsigned color)
{
	int i,j;
	unsigned int k,mcolor;
	
	for ( i=0;i<8;i++ )
	{
		k=1;
		for ( j=0;j<8;j++,k<<=1 )
		{
			if ( color==2 )	mcolor=2;
			else
			{
				mcolor=( pData[i]&k )?(1):(0);
				if ( color==0 )	mcolor=1-mcolor;
			}
			LCDPutPixel(x+i,y-j,mcolor);
		}
	}
	LCDRefreshScreen();
}

void LCDPutCString(unsigned int *pData,int x,int y,unsigned int nCharNumber,unsigned color)
{
	int i,j,l;
	unsigned int k,mcolor;
	
	for ( l=0;l<nCharNumber;l++ )
		for ( i=0;i<16;i++ )
		{
			k=1;
			for ( j=0;j<16;j++,k<<=1 )
			{
				if ( color==2 )	mcolor=2;
				else
				{
					mcolor=( pData[l*16+i]&k )?(1):(0);
					if ( color==0 )	mcolor=1-mcolor;
				}
				LCDPutPixel(x+l*16+i,y-j,mcolor);
			}
		}
	LCDRefreshScreen();
}


