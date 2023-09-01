#include "rtl8019a.h"
#include "MNetWorkPKT.h"
#include "5509.h"
#include "main.h"
unsigned short RxEthnetFrameBuffer[1518/2];
unsigned short TxEthnetFrameBuffer[1518/2];
/**********************************************************************
**函数原型：    unsigned short SwapWord(unsigned short Data)  
**说    明：	高低字节互换（大小端问题）
************************************************************************/		
unsigned short SwapWord(unsigned short Data)
{
    return((Data>>8)|(Data<<8));
}
/**********************************************************************
**函数原型：    void  RST8019()     
**说    明：	硬件复位8019芯片
************************************************************************/
void    rst8019()
{
	int* deminaddr = (int *)CE2ADDR;
	*deminaddr = 0x00fb;
	deminaddr = (int *)CE3ADDR;
	*deminaddr = 0x0001;
	delay(30000);
	delay(30000);
	*deminaddr = 0x0000;
	*deminaddr = 0x0002;
	deminaddr = (int *)CE2ADDR;
	*deminaddr = 0x00ff;
}

/**********************************************************************
**函数原型：    void  CS8019()     
**说    明：	片选8019芯片
************************************************************************/
void    cs8019()
{
	int* deminaddr = (int *)CE2ADDR;
	*deminaddr = 0x00fb;
	deminaddr = (int *)CE3ADDR;
	*deminaddr = 0x0000;

	deminaddr = (int *)CE2ADDR;
	*deminaddr = 0x00ff;
}

/**********************************************************************
**函数原型：    void  disCS8019()     
**说    明：	取消片选8019芯片
************************************************************************/
void    dis8019()
{
	int* deminaddr = (int *)CE2ADDR;
	*deminaddr = 0x00fb;
	deminaddr = (int *)CE3ADDR;
	*deminaddr = 0x0002;
	deminaddr = (int *)CE2ADDR;
	*deminaddr = 0x00ff;
}
/**********************************************************************
**函数原型：    void selectPage8019(unsigned short pagenumber)   
**说    明：	选中8019的工作页面
************************************************************************/
void selectPage8019(unsigned short pagenumber)
{
	unsigned short	temp;
	temp = CR;				//command register
	temp = temp & 0x3B ; 			//注意txp位不能要
	pagenumber=pagenumber <<6;
	temp=temp | pagenumber;
	CR=temp;
}
/**********************************************************************
**函数原型：    void init8019(void) 
**说    明：	初始化8019芯片
************************************************************************/
void init8019(void)
{
	pRTL8019AS_PAGE0_WR pPage0Wr;
	pRTL8019AS_PAGE0_RD pPage0Rd;
	pRTL8019AS_PAGE1 pPage1;
	pRTL8019AS_PAGE3 pPage3;
	pPage0Wr=(RTL8019AS_PAGE0_WR* )CE1ADDR;
	pPage0Rd=(RTL8019AS_PAGE0_RD* )CE1ADDR;
	pPage1=(RTL8019AS_PAGE1* )CE1ADDR;
	pPage3=(RTL8019AS_PAGE3* )CE1ADDR;
	CR=0x21;
	ddelay(10);
    selectPage8019(0);
    delay(10);
	pPage0Wr->rbcr0=0x00;
	pPage0Wr->rbcr1=0x00;
	pPage0Wr->rcr=0xe0;
	pPage0Wr->tcr=0xe2;
	pPage0Wr->pstart=0x4c;
	pPage0Wr->pstop=0x80;
	pPage0Rd->bnry=0x4c;
	pPage0Wr->tpsr=0x40;
	pPage0Rd->isr=0xff;
	pPage0Wr->immr=0x01;
	pPage0Wr->dcr=0xc9;
 	selectPage8019(1);
	delay(10);
	pPage1->curr=0x4d;  
	pPage1->mar0=0x00;
    pPage1->mar1=0x00;
    pPage1->mar2=0x00;
    pPage1->mar3=0x00;
    pPage1->mar4=0x00;
    pPage1->mar5=0x00;
    pPage1->mar6=0x00;
    pPage1->mar7=0x00;

    pPage1->par0=broadMac.ucByte2_1;
    pPage1->par1=broadMac.ucByte2_1>>8;
    pPage1->par2=broadMac.ucByte4_3;
    pPage1->par3=broadMac.ucByte4_3>>8;
    pPage1->par4=broadMac.ucByte6_5;
    pPage1->par5=broadMac.ucByte6_5>>8;

    selectPage8019(3);
	delay(10);
    pPage3->page3cr= 0x30;  
	pPage3->config1 = 0x80;
 	selectPage8019(0);
	delay(10);
	pPage0Wr->rcr=0xce;			
	pPage0Wr->tcr= 0xe0;
	pPage0Wr->cr= 0x22;  //启动芯片开始工作
	pPage0Wr->isr=0xff;
}
/**********************************************************************
**函数原型：    void sendFrame8019(unsigned short* framBuffer,unsigned short length)
**说    明：	8019发送一帧数据
************************************************************************/
void sendFrame8019(unsigned short* framBuffer,unsigned short length)
{
	static unsigned short TxBufFlag;
	unsigned short i,ii;
	pRTL8019AS_PAGE0_WR pPage0Wr;
	pRTL8019AS_PAGE0_RD pPage0Rd;
	pPage0Wr=(RTL8019AS_PAGE0_WR* )CE1ADDR;
	pPage0Rd=(RTL8019AS_PAGE0_RD* )CE1ADDR;
	if(length<46/2)
    {
    	length=60/2;
    }
	selectPage8019(0);
	delay(10);						//切换至第0页
	length = length <<1;
	TxBufFlag=!TxBufFlag;			//设置两个发缓区,提高发送效率
	if(TxBufFlag)
		{pPage0Wr->rsar1=0x40 ;}				//设置发送页地址
	else
		{pPage0Wr->rsar1=0x46 ;}				//设置发送页地址
    
	pPage0Wr->rsar0=0x00; 					//read page address low
	pPage0Wr->rbcr1= length >>8;				//写发送长度高位
	pPage0Wr->rbcr0 = length & 0x00ff;        //写发送长度低位  
	pPage0Wr->cr=0x12;						//write dma, page0

	for(i=0;i<length/2;i++)
	{
		DMA = framBuffer[i];
   	}
/***************************************/
//以下为终止DMA操作
	pPage0Wr->rbcr1=0x00; 
	pPage0Wr->rbcr0=0x00;
	pPage0Wr->cr=0x22;			//结束或放弃DMA操作
/***************************************/
	for(i=0;i<6;i++)	//最多重发6次
	{
		for(ii=0;ii<1000;ii++)
    	{//检查CR寄存器的txp位是否为低,为1说明正在发送,为0说明发完或出错放弃
        	if((pPage0Rd->cr&0x04)==0)  
        		{ break; }
        }
		if((pPage0Rd->tsr&0x01)!=0)//表示发送成功,判断发送状态寄存器TSR，决定是否出错
			{break;};
		pPage0Wr->cr=0x3e;
	}
	delay(1);
	pPage0Wr->isr=0xff;
	if(TxBufFlag)
	{
  		pPage0Wr->tpsr=0x40;   //txd packet start;
  	}
	else
	{
     	pPage0Wr->tpsr=0x46;  //txd packet start;
    }	
	pPage0Wr->tbcr1=length>>8;  //high byte counter
	pPage0Wr->tbcr0=length&0xff;//low byte counter
	pPage0Wr->isr=0xff;
	pPage0Wr->cr=0x3e;       //to sendpacket;
}
/**********************************************************************
**函数原型：    unsigned short recFrame()
**说    明：	8019接受一帧数据
************************************************************************/
unsigned short recFrame()
{
	unsigned short	i,ii,bnry,curr;
	unsigned short	Temp,length,temp2;
	union{
			unsigned short	total;	
		struct{
			unsigned short	high:8;
			unsigned short	low:8;
		}e;
	}d;
	pRTL8019AS_PAGE0_WR pPage0Wr;
	pRTL8019AS_PAGE0_RD pPage0Rd;
	pRTL8019AS_PAGE1 pPage1;
	pPage0Wr=(RTL8019AS_PAGE0_WR* )CE1ADDR;
	pPage0Rd=(RTL8019AS_PAGE0_RD* )CE1ADDR;
	pPage1=(RTL8019AS_PAGE1* )CE1ADDR;
	for(i=0;i<1518/2;i++)	
		RxEthnetFrameBuffer[i] = 0;				 		  		
	selectPage8019(0);
	delay(1);
	bnry = pPage0Rd->bnry;				//bnry page have read 读页指针
	bnry = bnry & 0x00FF;
	selectPage8019(1);
	delay(1);
	curr = pPage1->curr;				//curr writepoint 8019写页指针
	curr = curr & 0x00FF;
	selectPage8019(0);
	delay(1);
	if((curr==0))
		return(0);	//读的过程出错
	bnry++;
	if(bnry>0x7f)	
		bnry=0x4c;
	if(bnry!=curr)			//此时表示有新的数据包在缓冲区里
    {					//在任何操作都最好返回page0
     	selectPage8019(0);
     	delay(1);
     //=======================================
     	pPage0Wr->rsar1=bnry;		//读页地址的高字节
     	pPage0Wr->rsar0=0x00; 	//读页地址的低字节
     	pPage0Wr->rbcr1=0x00;		//读取字节计数高字节
     	pPage0Wr->rbcr0=18;       //读取字节计数高字节
     	pPage0Wr->cr=0x0a;     //启动Remote DMA读操作	
     	for(i=0;i<9;i++)	//读取一包的前18个字节:4字节的8019头部,6字节目的地址,6字节原地址,2字节协议
        {
            RxEthnetFrameBuffer[i]=DMA;
        }
 //=======================================中止DMA操作
     	pPage0Wr->rbcr1=0x00;	//
     	pPage0Wr->rbcr0=0x00;	//
     	pPage0Wr->cr=0x22;	//结束或放弃DMA操作
 //=======================================
					
		d.total = RxEthnetFrameBuffer[0];
		length = RxEthnetFrameBuffer[1];
     	if(((d.e.low & 0x01)==0)||(d.e.high > 0x7f) || (d.e.high < 0x4c)||( length > 1517))
    	{//接收状态错误或下一数据包的起始页地址错误或接收的数据包长度>1500字节
       		selectPage8019(1);
       		delay(1);
			curr=pPage1->curr; 	//page1
			selectPage8019(0);		//切换回page0
			delay(10);
        	bnry = curr -1;
        	if(bnry < 0x4c) 	
        		bnry =0x7f;
        	pPage0Wr->bnry=bnry; 	//write to bnry     
        	pPage0Wr->isr=0xff;
			return(0);
        }
//=============================================
            
     	else//表示数据包是完好的.读取剩下的数据
     	{
     		Temp = SwapWord(RxEthnetFrameBuffer[8]);	
        	if((Temp == IP_FRAME)||(Temp == ARP_FRAME))
			{ //协议为IP或ARP才接收
                pPage0Wr->rsar1 = bnry;   				//read page address high
                pPage0Wr->rsar0= 4;      				//read page address low
                pPage0Wr->rbcr1 = length>>8;  //read count high
                pPage0Wr->rbcr0 = length & 0xff;//read count low;
                pPage0Wr->cr=0x0a;                             //read dma
				length=(length+1)/2;
                for(ii=2;ii<length+2;ii++)
                {
                	RxEthnetFrameBuffer[ii]=DMA;
                }
                        			//================终止DMA操作
                pPage0Wr->rbcr1=0x00;			//read count high   
                pPage0Wr->rbcr0=0x00;			//read count low;
                pPage0Wr->cr=0x22;			//结束或放弃DMA
				switch(Temp)
     			{
				case IP_FRAME:
					temp2=RxEthnetFrameBuffer[13]>>8;
					switch(temp2)
					{
					case PROTOCOL_UDP:
						recvUdpFrame(&RxEthnetFrameBuffer[19],length-19);			//udp接受数据
						break;
					default:
						break;
					}
					break;
				case ARP_FRAME:
					recvArpFrame(&RxEthnetFrameBuffer[2]);							//arp接收数据
				default:
					break;
     			}
                        			//============================
            }
                        		//=========================================
            d.total = RxEthnetFrameBuffer[0];
            bnry = d.e.high - 1;	
            if(bnry<0x4c)	
            	bnry=0x7f;
            selectPage8019(0);
            delay(1);
	        pPage0Wr->bnry=bnry;      //write to bnry
            pPage0Wr->isr=0xff;
            return(1);   //have new packet
		}
   	}
	else	
		return(0);
}
/**********************************************************************
**函数原型：    uinterrupt void int8019a()
**说    明：	8019中断
************************************************************************/
interrupt void int8019a()
{
	//recFrame();
	recv8019Flag=1;
}

