#include "myapp.h"
#include "zk16.h"

#define ST1_55        *(int *)0x03
#define ST3_55        *(int *)0x04

#define INT8U unsigned char
#define INT16U unsigned int

// ����ָʾ�ƼĴ�����ַ�ͼĴ�������

#define t6963c (*((unsigned int *)0x601400))
#define t6963d (*((unsigned int *)0x601402))

#define LBDS1 (*((unsigned int *)0x600400))

// �ӳ���ӿ�
void Delay(unsigned int nDelay);	// ��ʱ�ӳ���

/*===================================================================================*/
/*Function: Write command to T6963C  Controler                                       */
/*Input   : com(command)                                                             */
/*Output  : None                                                                     */
/*===================================================================================*/
void Write_Command(unsigned char com)
{
 unsigned int i,j;

  for(i=0;i<1000;i++)
	for(j=0;j<100;j++);

  t6963c=com;

}
/*===================================================================================*/
/*Function:Write data to T6963 Controler                                             */
/*Input   :dat(data)                                                                 */
/*Output  :None                                                                      */
/*===================================================================================*/
void Write_Data(unsigned char dat)
{
  unsigned int i,j;
  for(i=0;i<1000;i++)
	for(j=0;j<100;j++);

   t6963d=dat;
}

/*===================================================================================*/
/*Function:Auto write data to T6963C Controler                                       */
/*Input   :dat(data)                                                                 */
/*Output  :None                                                                      */
/*===================================================================================*/
void Auto_Write(INT8U dat)
{
  unsigned int i,j;
  for(i=0;i<1000;i++)
	for(j=0;j<100;j++);

  t6963d=dat;
}

/*===================================================================================*/
/*Function:Set text area start address                                               */
/*Input   :x  Col position���У�                                                     */
/*         y  Raw position (��)                                                      */
/*Output  :None                                                                      */
/*===================================================================================*/
void Set_Text_Addr(INT8U x,INT8U y)
{
 INT16U temp;

 temp=y*0x20+x;                      /*0x20���ı���ʾ�����                          */ 
 Write_Data(temp&0xff);              /*д��ʾ��ַ��8λ                               */
 Write_Data(temp/256);               /*д��ʾ��ַ��8λ                                */
 Write_Command(0x24);
 }

/*===================================================================================*/
/*Function:Clear screen                                                              */
/*Input   :None                                                                      */
/*Output  :None                                                                      */
/*===================================================================================*/
void Clear_Screen(void)
{
  INT16U i;
  
  Set_Text_Addr(0,0);
  Write_Command(0xb0);
  
  for(i=0;i<0x2000;i++)
  {
    Auto_Write(0x00);
  }
 
  Write_Command(0xb2);
}

/*===================================================================================*/
/*Function:LCD Init                                                                  */
/*Input   :None                                                                      */
/*Output  :None                                                                      */
/*===================================================================================*/
void LCD_Init(void)
{

  Write_Data(0x00);                    /*�����ı���ʾ���׵�ַ                        */
  Write_Data(0x00);
  Write_Command(0x40);
  
  Write_Data(0x20);                    /*�����ı���ʾ�����                          */
  Write_Data(0x00);
  Write_Command(0x41);
  
  Write_Data(0x80);                    /*����ͼ����ʾ���׵�ַ                        */
  Write_Data(0x00);
  Write_Command(0x42);
  
  Write_Data(0x20);                    /*����ͼ����ʾ�����                          */
  Write_Data(0x00);
  Write_Command(0x43);
 

  Write_Command(0xa1);                 /*���ù�����״Ϊ 2��                        */


  Write_Command(0x80);                 /*CG ROM ��ʽ����                             */

  Write_Command(0x98);                 /*��ʾ��ʽ��ͼ�η�ʽ��,�ı���ʽ��             */
  

//  Clear_Screen();                      /*����                                        */
  
}

/*===================================================================================*/
/*Function:��ʾ8*16Ӣ����ĸ                                                          */
/*Input   :                                                                          */
/*          x:��x����ʾ                                                              */
/*          y:��y����ʾ                                                              */
/*Output  : None                                                                     */
/*===================================================================================*/
void Display_English_16(INT8U y,INT8U x,char *str)
{
	INT8U C1,C2,cData;
	INT8U i,j,uLen,uRow,uCol;
	uLen=0;
	i=0;
	uRow=y;
	uCol=x;
	Set_Text_Addr(uCol,uRow);
	while(str[uLen]!=0)                      /*���*str�ַ����ĳ���.                 */
	{
		uLen++;
	}
	while(i<uLen)
	{
		C1=str[i];
		C2=str[i+1];
		if(C1<128)                           /*�Ƿ���ASCII�ַ�                       */       
		{
			for(j=0;j<16;j++)
			{
				Write_Command(0xb0);
				if(C1>=0x20)                 /*�Ƿ��ǿ���ʾ�ַ�                      */
				{
					if(j<0)
					Write_Data(0x00);
					else
					Write_Data(ASC_MSK[(C1-0x20)*16+j]);
				}
				else
				Write_Data(cData);
				Write_Command(0xb2);
				Set_Text_Addr(uCol,uRow+j+1);
			}
			if(C1!=0x08)
			uCol++;
		}
		else
		{
			;
		}
		Set_Text_Addr(uCol,uRow);
		i++;
	}
}

/*===================================================================================*/
void setb(unsigned char d) 
{
	Write_Command(0xf8|d);
}

/*===================================================================================*/
/*����                                                                               */
/*����                               	                                             */
void Line(INT8U y0,INT8U x0,INT8U y1,INT8U x1) 
{
	INT8U  x=x1-x0,y=y1-y0,xx;
	float yy;
	if (x>y) 
     {
	  yy=y0;
	  for (xx=x0;xx<x1;xx++) 
       {
	     yy+=(float)y/x;
	     Set_Text_Addr((xx/8),yy);
		 setb((7-xx&7));
	   }
	 } 
     else 
      {
		yy=x0;
		for (xx=y0;xx<y1;xx++) 
        {
		 yy+=(float)x/y;
		 Set_Text_Addr(yy/8,xx);
		 setb((7-(char)yy&7));
	    }
	  }
}

void Draw_Button(INT8U x1,INT8U y1,INT8U x2,INT8U y2,INT8U style)
{
  Line(x1,y1,x1,y2);
  Line(x1,y2,x2,y2);
  Line(x2,y1,x2,y2+1);
  Line(x1,y1,x2,y1);
  
  if(style==0)
   {  
    Line(x1+1,y2+1,x2+1,y2+1);
    Line(x2+1,y1+1,x2+1,y2+2);
   }
   else if(style==1)
    {
     Line(x1+1,y1+1,x2-1,y1+1);
     Line(x1+1,y1+1,x1+1,y2-1);
    }
}  

void Draw_Bar(void)
{
  INT8U i;
  INT16U j;
//  Draw_Button(50,50,65,119,2);
  Draw_Button(95,80,106,149,2);

  for(i=0;i<66;i++)
  {
    Line(97,82+i,105,82+i);  
    for(j=0;j<1000;j++);
  }
}

main()
{
	CLK_init();					// ��ʼ��DSP����ʱ��
	ST3_55 =  ST3_55|0x0040;    //����CE3��ӳ��
	SDRAM_init();				// ��ʼ��EMIF�ӿ�
	
//	GPIO_SEL=0;

	LCD_Init();
	Display_English_16(48,20,"B");
//	Draw_Bar();

    while(1);
	
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
