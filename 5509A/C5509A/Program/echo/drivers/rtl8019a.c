#include "rtl8019a.h"
#include "MNetWorkPKT.h"
#include "5509.h"
#include "main.h"
unsigned short RxEthnetFrameBuffer[1518/2];
unsigned short TxEthnetFrameBuffer[1518/2];
/**********************************************************************
**����ԭ�ͣ�    unsigned short SwapWord(unsigned short Data)  
**˵    ����	�ߵ��ֽڻ�������С�����⣩
************************************************************************/		
unsigned short SwapWord(unsigned short Data)
{
    return((Data>>8)|(Data<<8));
}
/**********************************************************************
**����ԭ�ͣ�    void  RST8019()     
**˵    ����	Ӳ����λ8019оƬ
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
**����ԭ�ͣ�    void  CS8019()     
**˵    ����	Ƭѡ8019оƬ
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
**����ԭ�ͣ�    void  disCS8019()     
**˵    ����	ȡ��Ƭѡ8019оƬ
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
**����ԭ�ͣ�    void selectPage8019(unsigned short pagenumber)   
**˵    ����	ѡ��8019�Ĺ���ҳ��
************************************************************************/
void selectPage8019(unsigned short pagenumber)
{
	unsigned short	temp;
	temp = CR;				//command register
	temp = temp & 0x3B ; 			//ע��txpλ����Ҫ
	pagenumber=pagenumber <<6;
	temp=temp | pagenumber;
	CR=temp;
}
/**********************************************************************
**����ԭ�ͣ�    void init8019(void) 
**˵    ����	��ʼ��8019оƬ
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
	pPage0Wr->cr= 0x22;  //����оƬ��ʼ����
	pPage0Wr->isr=0xff;
}
/**********************************************************************
**����ԭ�ͣ�    void sendFrame8019(unsigned short* framBuffer,unsigned short length)
**˵    ����	8019����һ֡����
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
	delay(10);						//�л�����0ҳ
	length = length <<1;
	TxBufFlag=!TxBufFlag;			//��������������,��߷���Ч��
	if(TxBufFlag)
		{pPage0Wr->rsar1=0x40 ;}				//���÷���ҳ��ַ
	else
		{pPage0Wr->rsar1=0x46 ;}				//���÷���ҳ��ַ
    
	pPage0Wr->rsar0=0x00; 					//read page address low
	pPage0Wr->rbcr1= length >>8;				//д���ͳ��ȸ�λ
	pPage0Wr->rbcr0 = length & 0x00ff;        //д���ͳ��ȵ�λ  
	pPage0Wr->cr=0x12;						//write dma, page0

	for(i=0;i<length/2;i++)
	{
		DMA = framBuffer[i];
   	}
/***************************************/
//����Ϊ��ֹDMA����
	pPage0Wr->rbcr1=0x00; 
	pPage0Wr->rbcr0=0x00;
	pPage0Wr->cr=0x22;			//���������DMA����
/***************************************/
	for(i=0;i<6;i++)	//����ط�6��
	{
		for(ii=0;ii<1000;ii++)
    	{//���CR�Ĵ�����txpλ�Ƿ�Ϊ��,Ϊ1˵�����ڷ���,Ϊ0˵�������������
        	if((pPage0Rd->cr&0x04)==0)  
        		{ break; }
        }
		if((pPage0Rd->tsr&0x01)!=0)//��ʾ���ͳɹ�,�жϷ���״̬�Ĵ���TSR�������Ƿ����
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
**����ԭ�ͣ�    unsigned short recFrame()
**˵    ����	8019����һ֡����
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
	bnry = pPage0Rd->bnry;				//bnry page have read ��ҳָ��
	bnry = bnry & 0x00FF;
	selectPage8019(1);
	delay(1);
	curr = pPage1->curr;				//curr writepoint 8019дҳָ��
	curr = curr & 0x00FF;
	selectPage8019(0);
	delay(1);
	if((curr==0))
		return(0);	//���Ĺ��̳���
	bnry++;
	if(bnry>0x7f)	
		bnry=0x4c;
	if(bnry!=curr)			//��ʱ��ʾ���µ����ݰ��ڻ�������
    {					//���κβ�������÷���page0
     	selectPage8019(0);
     	delay(1);
     //=======================================
     	pPage0Wr->rsar1=bnry;		//��ҳ��ַ�ĸ��ֽ�
     	pPage0Wr->rsar0=0x00; 	//��ҳ��ַ�ĵ��ֽ�
     	pPage0Wr->rbcr1=0x00;		//��ȡ�ֽڼ������ֽ�
     	pPage0Wr->rbcr0=18;       //��ȡ�ֽڼ������ֽ�
     	pPage0Wr->cr=0x0a;     //����Remote DMA������	
     	for(i=0;i<9;i++)	//��ȡһ����ǰ18���ֽ�:4�ֽڵ�8019ͷ��,6�ֽ�Ŀ�ĵ�ַ,6�ֽ�ԭ��ַ,2�ֽ�Э��
        {
            RxEthnetFrameBuffer[i]=DMA;
        }
 //=======================================��ֹDMA����
     	pPage0Wr->rbcr1=0x00;	//
     	pPage0Wr->rbcr0=0x00;	//
     	pPage0Wr->cr=0x22;	//���������DMA����
 //=======================================
					
		d.total = RxEthnetFrameBuffer[0];
		length = RxEthnetFrameBuffer[1];
     	if(((d.e.low & 0x01)==0)||(d.e.high > 0x7f) || (d.e.high < 0x4c)||( length > 1517))
    	{//����״̬�������һ���ݰ�����ʼҳ��ַ�������յ����ݰ�����>1500�ֽ�
       		selectPage8019(1);
       		delay(1);
			curr=pPage1->curr; 	//page1
			selectPage8019(0);		//�л���page0
			delay(10);
        	bnry = curr -1;
        	if(bnry < 0x4c) 	
        		bnry =0x7f;
        	pPage0Wr->bnry=bnry; 	//write to bnry     
        	pPage0Wr->isr=0xff;
			return(0);
        }
//=============================================
            
     	else//��ʾ���ݰ�����õ�.��ȡʣ�µ�����
     	{
     		Temp = SwapWord(RxEthnetFrameBuffer[8]);	
        	if((Temp == IP_FRAME)||(Temp == ARP_FRAME))
			{ //Э��ΪIP��ARP�Ž���
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
                        			//================��ֹDMA����
                pPage0Wr->rbcr1=0x00;			//read count high   
                pPage0Wr->rbcr0=0x00;			//read count low;
                pPage0Wr->cr=0x22;			//���������DMA
				switch(Temp)
     			{
				case IP_FRAME:
					temp2=RxEthnetFrameBuffer[13]>>8;
					switch(temp2)
					{
					case PROTOCOL_UDP:
						recvUdpFrame(&RxEthnetFrameBuffer[19],length-19);			//udp��������
						break;
					default:
						break;
					}
					break;
				case ARP_FRAME:
					recvArpFrame(&RxEthnetFrameBuffer[2]);							//arp��������
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
**����ԭ�ͣ�    uinterrupt void int8019a()
**˵    ����	8019�ж�
************************************************************************/
interrupt void int8019a()
{
	//recFrame();
	recv8019Flag=1;
}

