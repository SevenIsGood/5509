#include	"RTL8019.h"
#include	"string.h"
#include	"ip.h"

#ifndef _CESECT_
#define _CESECT_
#define CESECT1 0x200000
#define CESECT2 0x400000
#define CESECT3 0x600000
#endif

extern int *souraddr;
extern int *deminaddr;
extern	Uint16  RxSuccessFlag;
extern	Uint16	TxEthnetFrameBuffer[1518/2];
extern	Uint16	RxEthnetFrameBuffer[1518/2];
extern	int		ms;

unsigned int test;

const Uint16 MyMAC[] = 
{0xE002,0xA04C,0x7A7E};

const Uint16 MyIP[] =
{0xA8C0,0x0B01};

const Uint16 RemoteIP[] =
{0xA8C0,0x2801};

const Uint16 SubnetMask[] =
{0xFFFF,0x00FF};

const Uint16 GatewayIP[] = 
{0xA8C0,0x0101};

Uint16	TxBufFlag;
Uint16	bnry,curr;
Uint16 UDPUserDataLenth; 
Uint16 IpEdition;                                //IP版本
Uint16 IpServiceType;
Uint16 IpIdent;
Uint16 MyUdpPort;

Uint16 RemoteMAC[3];                             // 48 bit MAC
Uint16 RecdFrameMAC[3];
Uint16 RecdFrameIP[2];
Uint16 RecdIpFrameLenth;
Uint16 TxFrameSize;
Uint16 ARPAnswerSuccessFlag;

void delay(int k)
{
   while(k--);
}

void ddelay(Uint16 kt)
{
	Uint16 jj,kk;
	for(jj=0;jj<65535;jj++)
	{
		kk = kt;
		while(kk>0)
		{
			kk--;
		}
	}
}

Uint16	SwapByte(Uint16	value)
		{
    		Uint16	temp;
    		temp=(value<<8)|(value>>8);
    		return(temp);
		}								

/**********************************************************************
**函数原型：    void  REGconfig()     
**说    明：	设置17个16位数据指针指向8019芯片的寄存器地址
************************************************************************/
void REGconfig()
{
	Reg00 = (int *)CESECT1;
	Reg01 = (int *)CESECT1 + 1;
	Reg02 = (int *)CESECT1 + 2;
	Reg03 = (int *)CESECT1 + 3;
	Reg04 = (int *)CESECT1 + 4;
	Reg05 = (int *)CESECT1 + 5;
	Reg06 = (int *)CESECT1 + 6;
	Reg07 = (int *)CESECT1 + 7;
	Reg08 = (int *)CESECT1 + 8;
	Reg09 = (int *)CESECT1 + 9;
	Reg0a = (int *)CESECT1 + 10;
	Reg0b = (int *)CESECT1 + 11;
	Reg0c = (int *)CESECT1 + 12;
	Reg0d = (int *)CESECT1 + 13;
	Reg0e = (int *)CESECT1 + 14;
	Reg0f = (int *)CESECT1 + 15;
	Reg10 = (int *)CESECT1 + 16;
}

//==============================================================================
/**********************************************************************
**函数原型：    void  page(uchar pagenumber)
**入口参数:   	Uint16	pagenumber: 要切换的页
**出口参数:		无
**返 回 值：	无      
**说    明：	选择页,可选择0,1,2三页,第四页ne000兼容芯片保留 
************************************************************************/
void	page(Uint16 pagenumber)
		{
			Uint16	temp;
			temp = *Reg00;				//command register
			temp = temp & 0x3B ; 			//注意txp位不能要
			pagenumber=pagenumber <<6;
			temp=temp | pagenumber;
			*Reg00=temp;
		}

/**********************************************************************
**函数原型：    void  RST8019()     
**说    明：	硬件复位8019芯片
************************************************************************/
void    RST8019()
		{
			deminaddr = (int *)CESECT2;
			*deminaddr = 0x00fb;
		    deminaddr = (int *)CESECT3;
			*deminaddr = 0x0001;
			delay(30000);
			delay(30000);
			*deminaddr = 0x0000;
			*deminaddr = 0x0002;
			deminaddr = (int *)CESECT2;
			*deminaddr = 0x00ff;
		}

/**********************************************************************
**函数原型：    void  CS8019()     
**说    明：	片选8019芯片
************************************************************************/
void    CS8019()
		{
			deminaddr = (int *)CESECT2;
			*deminaddr = 0x00fb;
		    deminaddr = (int *)CESECT3;
			*deminaddr = 0x0000;

			deminaddr = (int *)CESECT2;
			*deminaddr = 0x00ff;
		}

/**********************************************************************
**函数原型：    void  disCS8019()     
**说    明：	取消片选8019芯片
************************************************************************/
void    disCS8019()
		{
			deminaddr = (int *)CESECT2;
			*deminaddr = 0x00fb;
		    deminaddr = (int *)CESECT3;
			*deminaddr = 0x0002;
			deminaddr = (int *)CESECT2;
			*deminaddr = 0x00ff;
		}

/**********************************************************************
**函数原型:     void 	Init8019()
**入口参数:		无
**出口参数:		无
**返 回 值:		无              
**说    明:		对芯片的工作寄存器进行设置,各个寄存器的用法可参考文档和
**				络芯片的数据手册
************************************************************************/
void	Init8019()
		{
			ddelay(100);
/**********************************************************************

1.*Reg00命令寄存器: CR,命令寄存器,地址偏移量00H,为一个字节
  位:  7   6   5   4   3   2   1   0 
名字: PS1 PS0 RD2 RD1 RD0 TXP STA STP 
//============================================
2.
RD2,RD1,RD0: 这3个位代表要执行的功能。
 0   0   1 : 读网卡内存
 0   1   0 : 写网卡内存
 0   1   1 : 发送网卡数据包
 1   *   * : 完成或结束DMA的读写操作
//============================================
3.TXP位置1时发送数据包，发完自动清零
//============================================
4.
STA,STP:	这两个位用来启动命令或停止命令
 1   0 		启动命令
 0   1 		停止命令
//============================================
********************************************************/
			*Reg00=0x21;   //使芯片处于停止模式,这时进行寄存器设置
			ddelay(100);  //延时,确保芯片进入停止模式
			page(0);
			delay(10);
			*Reg0a= 0x00;   //清rbcr0
			delay(1);
			*Reg0b= 0x00;   //清rbcr1
			delay(1);
			*Reg0c= 0xe0;  //RCR,监视模式,不接收数据包
			delay(1);
			*Reg0d= 0xe2;  //TCR,loop back模式
			delay(1);
			*Reg01= 0x4c;   //pstart = 0x4c
			delay(1);
			*Reg02= 0x80;   //pstop = 0x80
			delay(1);
			*Reg03= 0x4c;   //bnry = 4c
			delay(1);
			*Reg04= 0x40;  //TPSR,发送起始页寄存器
			delay(1);
			*Reg07= 0xff;  //清除所有中断标志位，中断状态寄存器
			delay(1);
			*Reg0f= 0x01;  //中断屏蔽寄存器清0，禁止中断
			delay(1);
			*Reg0e= 0xc9;  // 数据配置寄存器，16位dma方式
			page(1);
			delay(10);
			*Reg07= 0x4d;
			delay(1);
			*Reg08= 0x00;
			delay(1);
			*Reg09= 0x00;
			delay(1);
			*Reg0a= 0x00;
			delay(1);
			*Reg0b= 0x00;
			delay(1);
			*Reg0c= 0x00;
			delay(1);
			*Reg0d= 0x00;
			delay(1);
			*Reg0e= 0x00;
			delay(1);
			*Reg0f= 0x00;
			
    		*Reg01=0x02;
    		*Reg02=0xE0;
    		*Reg03=0x4C;
    		*Reg04=0xA0;
    		*Reg05=0x7E;
    		*Reg06=0x7A;
			
			page(3);
			delay(10);
			*Reg01 = 0x30;
			delay(1);
			*Reg04 = 0x80;
			delay(1);
			
			page(0);
			delay(10);
			*Reg0c= 0xce;  //将芯片设置成正常模式,跟外部网络连接
			delay(1);			
			*Reg0d= 0xe0;
			delay(1);
			*Reg00= 0x22;  //启动芯片开始工作
//-----------------------------------
			*Reg07= 0xff;  //清除所有中断标志位
			ddelay(100);
		}


Uint16 CalcCheckSum(Uint16 *Start, Uint16 Count, Uint16 IsTCP, Uint16 IsUDP)
{
    Uint32 Sum;
  
    Sum = 0;
    if ((IsTCP)||(IsUDP)) 
    {
        Sum += MyIP[0];
        Sum += MyIP[1];
        Sum += RemoteIP[0];
        Sum += RemoteIP[1];
        Sum += SwapWord(Count*2);
        if(IsTCP)
             Sum += SwapWord(PROTOCOL_TCP);
        else
             Sum += SwapWord(PROTOCOL_UDP);
    }
    while (Count > 0) 
    {
        Sum += *Start;
        Start ++;
        Count --;
    }
    Sum = (Sum & 0xFFFF) + ((Sum >> 16)&0xFFFF);
    if(Sum&0xFFFF0000)
        Sum++;
    return((Uint16)((~Sum)&0xFFFF));
}


//=============================================================================
/**********************************************************************
**函数原型：    void 		SendFrame(Uint16 *TxdNetBuff,Uint16  length)
**入口参数:		Uint16    	*TxdNetBuff	:指向发送缓冲区
**              uint length					:发送数据包的长度
**出口参数:		无
**返 回 值：	无              
**说    明：	发送数据包,以太网底层驱动程序,所有的数据发送都要通过该程序
************************************************************************/
void	SendFrame(Uint16	*TxdNetBuff,Uint16 length)
		{
			Uint16		i,ii;
		   	if(length<46/2)
       			{
          			for(i=length;i<60/2;i++)	TxEthnetFrameBuffer[i]=0;
          			length=60/2;
       			}
			
			page(0);
			delay(10);						//切换至第0页
			length = length <<1;
			TxBufFlag=!TxBufFlag;			//设置两个发缓区,提高发送效率
			if(TxBufFlag)
				{*Reg09=0x40 ;}				//设置发送页地址
			else
				{*Reg09=0x46 ;}				//设置发送页地址
    
			*Reg08=0x00; 					//read page address low
			delay(1);
			*Reg0b = length >>8;				//写发送长度高位
			delay(1);
			*Reg0a = length & 0x00ff;        //写发送长度低位  
			delay(1);
			*Reg00=0x12;						//write dma, page0

			for(i=0;i<length/2;i++)
				{
					*Reg10 = TxEthnetFrameBuffer[i];
   				}
/***************************************/
//以下为终止DMA操作
			*Reg0b=0x00; 
			delay(1);
			*Reg0a=0x00;
			delay(1);
			*Reg00=0x22;			//结束或放弃DMA操作
			delay(1);
/***************************************/

			for(i=0;i<6;i++)	//最多重发6次
				{
					for(ii=0;ii<1000;ii++)
    					{//检查CR寄存器的txp位是否为低,为1说明正在发送,为0说明发完或出错放弃
        					if((*Reg00&0x04)==0)  
        						{ break; }
        				}
					if((*Reg04&0x01)!=0)//表示发送成功,判断发送状态寄存器TSR，决定是否出错
						{break;};
					*Reg00=0x3e;
				}
			delay(1);
			*Reg07=0xff;
			if(TxBufFlag)
				{
  					*Reg04=0x40;   //txd packet start;
  				}
			else
				{
     				*Reg04=0x46;  //txd packet start;
    			}
    		delay(1);	
			*Reg06=length>>8;  //high byte counter
			delay(1);
			*Reg05=length&0xff;//low byte counter
			delay(1);
			*Reg07=0xff;
			delay(1);
			*Reg00=0x3e;       //to sendpacket;
	}

/**********************************************************************
**函数原型：    Uint16        Rec_NewPacket()
**入口参数:		无
**出口参数:		无
**返 回 值：	0          没有新数据包
**              1          接收到新数据包
**说    明：	查询是否有新数据包并接收进缓冲区
************************************************************************/
Uint16	RecFrame()
		{
			Uint16	i,ii;
			Uint16	Temp,length;
			union{
					Uint16	total;	
					struct{
							Uint16	high:8;
							Uint16	low:8;
						  }e;
				 }d;
			for(i=0;i<1518/2;i++)	RxEthnetFrameBuffer[i] = 0;				 		  		
			page(0);
			delay(10);
			bnry = *Reg03;				//bnry page have read 读页指针
			bnry = bnry & 0x00FF;
			page(1);
			delay(10);
			curr = *Reg07;				//curr writepoint 8019写页指针
			curr = curr & 0x00FF;
			page(0);
			delay(10);
			if((curr==0))return(0);	//读的过程出错
			bnry++;
			if(bnry>0x7f)	bnry=0x4c;
			if(bnry!=curr)			//此时表示有新的数据包在缓冲区里
    			{					//在任何操作都最好返回page0
     				page(0);
     				delay(10);
     //=======================================
     				*Reg09=bnry;		//读页地址的高字节
     				delay(5);
     				*Reg08=0x00; 	//读页地址的低字节
     				delay(5);
     				*Reg0b=0x00;		//读取字节计数高字节
     				delay(5);
     				*Reg0a=18;       //读取字节计数高字节
     				delay(5);
     				*Reg00=0x0a;     //启动Remote DMA读操作
	 				delay(5);	
     				for(i=0;i<9;i++)	//读取一包的前18个字节:4字节的8019头部,6字节目的地址,6字节原地址,2字节协议
                    	{
                       		RxEthnetFrameBuffer[i]=*Reg10;
                       		delay(1);
                      	}
 //=======================================中止DMA操作
     				*Reg0b=0x00;	//
     				delay(1);
     				*Reg0a=0x00;	//
     				delay(1);
     				*Reg00=0x22;	//结束或放弃DMA操作
     				delay(1);
 //=======================================
					
					d.total = RxEthnetFrameBuffer[0];
					length = RxEthnetFrameBuffer[1];
     				if(((d.e.low & 0x01)==0)||(d.e.high > 0x7f) || (d.e.high < 0x4c)||( length > 1517))
    					{//接收状态错误或下一数据包的起始页地址错误或接收的数据包长度>1500字节
       						page(1);
       						delay(10);
							curr=*Reg07; 	//page1
							page(0);		//切换回page0
							delay(10);
        					bnry = curr -1;
        					if(bnry < 0x4c) 	bnry =0x7f;
        					*Reg03=bnry; 	//write to bnry     
        					delay(1);
        					*Reg07=0xff;
							return(0);
              			}
//=============================================
            
     				else//表示数据包是完好的.读取剩下的数据
     					{
     						Temp = SwapByte(RxEthnetFrameBuffer[8]);		
        					if((Temp == Frame_IP)||(Temp == Frame_ARP))
								{ //协议为IP或ARP才接收
                					*Reg09 = bnry;   				//read page address high
                					delay(1);
                					*Reg08 = 4;      				//read page address low
                					delay(1);
                					*Reg0b = length>>8;  //read count high
                					delay(1);
                					*Reg0a = length & 0xff;//read count low;
                					delay(1);
                					*Reg00=0x0a;                             //read dma
                					delay(1);
						        	length=(length+1)/2;
                					for(ii=2;ii<length+2;ii++)
                						{
                							RxEthnetFrameBuffer[ii]=*Reg10;
                						}
                        			//================终止DMA操作
                        			*Reg0b=0x00;			//read count high  
                        			delay(1); 
                        			*Reg0a=0x00;			//read count low;
                        			delay(1);
                        			*Reg00=0x22;			//结束或放弃DMA
                        			delay(1);
                        			//============================
                        		}
                        		//=========================================
                        	d.total = RxEthnetFrameBuffer[0];
                        	bnry = d.e.high - 1;	
                			if(bnry<0x4c)	bnry=0x7f;
                			page(0);
                			delay(10);
	        				*Reg03=bnry;      //write to bnry
	        				delay(1);
                			*Reg07=0xff;
                			return(1);   //have new packet
               			}
   				}
			else	
				return(0);
		}


Uint16 SwapWord(Uint16 Data)
{
    return((Data>>8)|(Data<<8));
}

void WriteDWord(Uint16 *Address,Uint32 Data)
{
    Uint16 Temp;
    Temp=(Uint16)(Data>>16);
    *Address=SwapWord(Temp);
    Temp=(Uint16)(Data&0x0000FFFF);
    *(Address+1)=SwapWord(Temp);
}

void CopyFrameFromBE(Uint16 Offset,Uint16 Size)
{
    Uint16 i,Temp;
    for(i=0;i<Size;i++)
    {
         Temp=*(RxEthnetFrameBuffer+Offset+i);
         *(RxEthnetFrameBuffer+Offset+i)=SwapWord(Temp);
    }
}

//============================================================
//======================== ARP ================================
void ArpRequest(void)
{
	*(TxEthnetFrameBuffer + ETH_HEADER_START +0) = 0xFFFF;
	*(TxEthnetFrameBuffer + ETH_HEADER_START +1) = 0xFFFF;
	*(TxEthnetFrameBuffer + ETH_HEADER_START +2) = 0xFFFF;
	 
    memcpy((TxEthnetFrameBuffer+ETH_HEADER_START+3),&MyMAC,3);
    *(TxEthnetFrameBuffer+ETH_HEADER_START+6)=SwapWord(Frame_ARP);
//ARP
    *(TxEthnetFrameBuffer+ARP_HEADER_START+0)=SwapWord(HARDW_ETH);
    *(TxEthnetFrameBuffer+ARP_HEADER_START+1)=SwapWord(Frame_IP);
    *(TxEthnetFrameBuffer+ARP_HEADER_START+2)=SwapWord(IP_HLEN_PLEN);
    *(TxEthnetFrameBuffer+ARP_HEADER_START+3)=SwapWord(OP_ARP_REQUEST);
    memcpy((TxEthnetFrameBuffer+ARP_HEADER_START+4),&MyMAC,3);
    memcpy((TxEthnetFrameBuffer+ARP_HEADER_START+7),&MyIP,2);
    *(TxEthnetFrameBuffer + ARP_HEADER_START + 9) = 0;
    *(TxEthnetFrameBuffer + ARP_HEADER_START + 10) = 0;
    *(TxEthnetFrameBuffer + ARP_HEADER_START + 11) = 0;
    
    if(((RemoteIP[0]^MyIP[0])&SubnetMask[0])||((RemoteIP[1]^MyIP[1])&SubnetMask[1]))
         memcpy((TxEthnetFrameBuffer+ARP_HEADER_START+12),&GatewayIP,2);
    else
         memcpy((TxEthnetFrameBuffer+ARP_HEADER_START+12),&RemoteIP,2);
    TxFrameSize=ETH_HEADER_SIZE+ARP_FRAME_SIZE; 
    SendFrame(TxEthnetFrameBuffer,TxFrameSize);
}

void ArpAnswer(void)
{
    memcpy((TxEthnetFrameBuffer+ETH_HEADER_START+0),&RecdFrameMAC,3);
    memcpy((TxEthnetFrameBuffer+ETH_HEADER_START+3),&MyMAC,3);
    *(TxEthnetFrameBuffer+ETH_HEADER_START+6)=SwapWord(Frame_ARP);
//ARP    
    *(TxEthnetFrameBuffer+ARP_HEADER_START+0)=SwapWord(HARDW_ETH);
    *(TxEthnetFrameBuffer+ARP_HEADER_START+1)=SwapWord(Frame_IP);
    *(TxEthnetFrameBuffer+ARP_HEADER_START+2)=SwapWord(IP_HLEN_PLEN);
    *(TxEthnetFrameBuffer+ARP_HEADER_START+3)=SwapWord(OP_ARP_ANSWER);
    memcpy((TxEthnetFrameBuffer+ARP_HEADER_START+4),&MyMAC,3);
    memcpy((TxEthnetFrameBuffer+ARP_HEADER_START+7),&MyIP,2);
    memcpy((TxEthnetFrameBuffer+ARP_HEADER_START+9),&RecdFrameMAC,3);
    memcpy((TxEthnetFrameBuffer+ARP_HEADER_START+12),&RecdFrameIP,2);
    TxFrameSize=ETH_HEADER_SIZE+ARP_FRAME_SIZE;
    SendFrame(TxEthnetFrameBuffer,TxFrameSize);
}

//==============================================================
//===========================ICMP================================

void Process_ICMP_Frame(void)
{
    Uint16 ICMPTypeAndCode;
    
    CopyFrameFromBE(RTL8019_HEADER_SIZE+ICMP_HEADER_START,2);
    ICMPTypeAndCode=*(RxEthnetFrameBuffer+RTL8019_HEADER_SIZE+ICMP_HEADER_START);
    switch(ICMPTypeAndCode>>8)
    {
         case ICMP_ECHO: Prepare_ICMP_Answer();
                         break;
    }
}

void Prepare_ICMP_Answer(void)
{
    Uint16 ICMPDataCount;
    Uint16 CalcCkSum;

    ICMPDataCount=RecdIpFrameLenth-IP_HEADER_SIZE - ICMP_HEADER_SIZE;
//Ethnet
    memcpy((TxEthnetFrameBuffer+ETH_HEADER_START),&RecdFrameMAC,3);
    memcpy((TxEthnetFrameBuffer+ETH_HEADER_START+3),&MyMAC,3);
    *(TxEthnetFrameBuffer+ETH_HEADER_START+6)=SwapWord(Frame_IP);
//IP
    *(TxEthnetFrameBuffer+IP_HEADER_START+0)=SwapWord(Ip_Edition);
    *(TxEthnetFrameBuffer+IP_HEADER_START+1)=SwapWord(RecdIpFrameLenth*2);
    *(TxEthnetFrameBuffer+IP_HEADER_START+2)=SwapWord(0);
    *(TxEthnetFrameBuffer+IP_HEADER_START+3)=SwapWord(0);
    *(TxEthnetFrameBuffer+IP_HEADER_START+4)=SwapWord((DEFUALT_TTL<<8)|PROTOCOL_ICMP);
    *(TxEthnetFrameBuffer+IP_HEADER_START+5)=0;
    memcpy((TxEthnetFrameBuffer+IP_HEADER_START+6),&MyIP,2);
    memcpy((TxEthnetFrameBuffer+IP_HEADER_START+8),&RecdFrameIP,2);

    CalcCkSum=CalcCheckSum((TxEthnetFrameBuffer+IP_HEADER_START),IP_HEADER_SIZE,0,0);
    if(!CalcCkSum)
         CalcCkSum=0xFFFF;
    *(TxEthnetFrameBuffer+IP_HEADER_START+5)=CalcCkSum;
//ICMP    
    *(TxEthnetFrameBuffer+ICMP_HEADER_START+0)=SwapWord(ICMP_ECHO_REPLY<<8);
    *(TxEthnetFrameBuffer+ICMP_HEADER_START+1)=0;
//    CopyFrameFrom8900(ICMP_HEADER_START+2,ICMPDataCount);
    memcpy((TxEthnetFrameBuffer+ICMP_HEADER_START+2),(RxEthnetFrameBuffer+RTL8019_HEADER_SIZE+ICMP_HEADER_START+2),ICMPDataCount);
    CalcCkSum=CalcCheckSum((TxEthnetFrameBuffer+ICMP_HEADER_START),(ICMPDataCount+ICMP_HEADER_SIZE),0,0);
    if(!CalcCkSum)
        CalcCkSum=0xFFFF;
    *(TxEthnetFrameBuffer+ICMP_HEADER_START+1)=CalcCkSum;
    TxFrameSize=ETH_HEADER_SIZE+IP_HEADER_SIZE+ICMP_HEADER_SIZE+ICMPDataCount;
    SendFrame(TxEthnetFrameBuffer,TxFrameSize);
}

/***************UDP*******************/

void Process_UDP_Frame(void)
{
    Uint16 UdpLenth;
    
    CopyFrameFromBE(RTL8019_HEADER_SIZE+UDP_HEADER_START,4);
    if(*(RxEthnetFrameBuffer+RTL8019_HEADER_SIZE+UDP_HEADER_START+1)==MY_UDP_PORT)
    {
         UdpLenth=*(RxEthnetFrameBuffer+RTL8019_HEADER_SIZE+UDP_HEADER_START+2)/2;
         UDPUserDataLenth=UdpLenth-UDP_HEADER_SIZE;
    }
}

void Prepare_UDP_Frame(Uint16 RemoteUdpPort,Uint16 TxUDPDataNum)
{

    Uint16 CalcCkSum;
//ETHNET    
    memcpy((TxEthnetFrameBuffer+ETH_HEADER_START+0),&RemoteMAC,3);
    memcpy((TxEthnetFrameBuffer+ETH_HEADER_START+2),&MyMAC,3);
    *(TxEthnetFrameBuffer+ETH_HEADER_START+6)=SwapWord(Frame_IP);
//IP
    *(TxEthnetFrameBuffer+IP_HEADER_START+0)=SwapWord(Ip_Edition);
    *(TxEthnetFrameBuffer+IP_HEADER_START+1)=SwapWord(TxUDPDataNum+IP_HEADER_SIZE+UDP_HEADER_SIZE);
    *(TxEthnetFrameBuffer+IP_HEADER_START+2)=SwapWord(0x0000);
    *(TxEthnetFrameBuffer+IP_HEADER_START+3)=SwapWord(0x0000);
    *(TxEthnetFrameBuffer+IP_HEADER_START+4)=SwapWord((DEFUALT_TTL<<8)|PROTOCOL_UDP);
    *(TxEthnetFrameBuffer+IP_HEADER_START+5)=0;
    memcpy((TxEthnetFrameBuffer+IP_HEADER_START+6),&MyIP,2);
    memcpy((TxEthnetFrameBuffer+IP_HEADER_START+8),&RemoteIP,2);
    CalcCkSum=CalcCheckSum((TxEthnetFrameBuffer+IP_HEADER_START),IP_HEADER_SIZE,0,0);
    if(!CalcCkSum)
         CalcCkSum=0xFFFF;
    *(TxEthnetFrameBuffer+IP_HEADER_START+5)=CalcCkSum;
//UDP                                                  
    *(TxEthnetFrameBuffer+UDP_HEADER_START+0)=SwapWord(MY_UDP_PORT);
    *(TxEthnetFrameBuffer+UDP_HEADER_START+1)=SwapWord(RemoteUdpPort);
    *(TxEthnetFrameBuffer+UDP_HEADER_START+2)=SwapWord(TxUDPDataNum+UDP_HEADER_SIZE);
    *(TxEthnetFrameBuffer+UDP_HEADER_START+3)=0;

    CalcCkSum=CalcCheckSum((TxEthnetFrameBuffer+UDP_HEADER_START),(TxUDPDataNum+UDP_HEADER_SIZE),0,1);
    if(!CalcCkSum)
         CalcCkSum=0xFFFF;
    *(TxEthnetFrameBuffer+UDP_HEADER_START+3)=CalcCkSum;
    TxFrameSize=ETH_HEADER_SIZE+IP_HEADER_SIZE+UDP_HEADER_SIZE+TxUDPDataNum;
    SendFrame(TxEthnetFrameBuffer,TxFrameSize);
}

/***************TCP/IP*****************/

void ProcessEthBroadcastFrame(void)
{
    Uint16 TargetIP[2],FrameLenth;
    
	FrameLenth=5;
    CopyFrameFromBE(RTL8019_HEADER_SIZE+6,FrameLenth);
    memcpy(&RecdFrameMAC,(RxEthnetFrameBuffer+ETH_HEADER_START+RTL8019_HEADER_SIZE+3),3);
    if(*(RxEthnetFrameBuffer+ETH_HEADER_START+RTL8019_HEADER_SIZE+6)==Frame_ARP)
    {
         if(*(RxEthnetFrameBuffer+ARP_HEADER_START+RTL8019_HEADER_SIZE)==HARDW_ETH)
         {
              if(*(RxEthnetFrameBuffer+ARP_HEADER_START+RTL8019_HEADER_SIZE+1)==Frame_IP)
              {
                   if(*(RxEthnetFrameBuffer+ARP_HEADER_START+RTL8019_HEADER_SIZE+2)==IP_HLEN_PLEN)
                   {
                        if(*(RxEthnetFrameBuffer+ARP_HEADER_START+RTL8019_HEADER_SIZE+3)==OP_ARP_REQUEST)
                        {
                             memcpy(&RecdFrameIP,(RxEthnetFrameBuffer+ARP_HEADER_START+RTL8019_HEADER_SIZE+7),2);
                             memcpy(&TargetIP,(RxEthnetFrameBuffer+ARP_HEADER_START+RTL8019_HEADER_SIZE+12),2);
                             if((MyIP[0]==TargetIP[0]) && (MyIP[1]==TargetIP[1]))
                                  ArpAnswer();    
                        }
                   }
              }
         }
    }               
}

void ProcessEthIAFrame(void)
{
    Uint16 FrameType,IpHeaderLenth,ProtocolType;
    
    CopyFrameFromBE(RTL8019_HEADER_SIZE+6,1);    
    memcpy(&RecdFrameMAC,(RxEthnetFrameBuffer+ETH_HEADER_START+RTL8019_HEADER_SIZE+3),3);
    FrameType=*(RxEthnetFrameBuffer+ETH_HEADER_START+RTL8019_HEADER_SIZE+6);
    switch(FrameType)
    {
         case Frame_ARP:
         {
              CopyFrameFromBE(ARP_HEADER_START+RTL8019_HEADER_SIZE+0,4);
              if(*(RxEthnetFrameBuffer+ARP_HEADER_START+RTL8019_HEADER_SIZE+2)==IP_HLEN_PLEN)
              {
                   if(*(RxEthnetFrameBuffer+ARP_HEADER_START+RTL8019_HEADER_SIZE+3)==OP_ARP_ANSWER)
                   {
                        memcpy(&RecdFrameMAC,(RxEthnetFrameBuffer+ARP_HEADER_START+RTL8019_HEADER_SIZE+4),3);
                        memcpy(&RecdFrameIP,(RxEthnetFrameBuffer+ARP_HEADER_START+RTL8019_HEADER_SIZE+7),2);
                        ARPAnswerSuccessFlag=1;
                   }
              }
         }
         break;
         case Frame_IP:
         {
              CopyFrameFromBE(IP_HEADER_START+RTL8019_HEADER_SIZE+0,1);
              IpHeaderLenth=*(RxEthnetFrameBuffer+IP_HEADER_START+RTL8019_HEADER_SIZE);
              if((IpHeaderLenth&0xFF00)==Ip_Edition)
              {
                   IpHeaderLenth=5*(32/8)/2;
                   CopyFrameFromBE(IP_HEADER_START+RTL8019_HEADER_SIZE+1,IpHeaderLenth-1-4);
                   RecdIpFrameLenth=*(RxEthnetFrameBuffer+IP_HEADER_START+RTL8019_HEADER_SIZE+1)/2;
                   ProtocolType=*(RxEthnetFrameBuffer+IP_HEADER_START+RTL8019_HEADER_SIZE+4)&0x00FF;
                   memcpy(&RecdFrameIP,(RxEthnetFrameBuffer+IP_HEADER_START+RTL8019_HEADER_SIZE+6),2);
                   switch(ProtocolType)
                   {
                        case PROTOCOL_ICMP:  Process_ICMP_Frame();
                                             break;
                        case PROTOCOL_TCP : // Process_TCP_Frame();
                                             break;
                        case PROTOCOL_UDP :  Process_UDP_Frame();
                                             break;
                   }
              }
         }
         default: break;
    }
}

void DoNetworkStuff()
{    
    Uint16 Temp;
    page(0);
    Temp=*Reg0c;
    Temp&=0x20;
    if(Temp)
         ProcessEthBroadcastFrame();
    else    
         ProcessEthIAFrame(); 
}


