#include <csl.h>
#include <csl_pll.h>
#include <csl_chip.h>
#include <csl_gpio.h>
#include <string.h>
#include "spi.h"


#if 0
#define W25Q16_PAGE_SIZE    256                          //һҳ�Ĵ�С��256�ֽ�
#define W25Q16_SECTOR_SIZE (4*1024)                      //������С���ֽ�
#define W25Q16_BLOCK_SIZE  (16*W25Q16_SECTOR_SIZE)
#define W25Q16_SIZE        (32*W25Q16_BLOCK_SIZE)#define MOSI_H					 (GpioDataRegs.GPASET.bit.GPIO16 = 1)
#define MOSI_L					 (GpioDataRegs.GPACLEAR.bit.GPIO16 = 1)
#define SPICLK_H				 (GpioDataRegs.GPASET.bit.GPIO17 = 1)
#define SPICLK_L				 (GpioDataRegs.GPACLEAR.bit.GPIO17 = 1)
#define FLASH_CS_SET			(GpioDataRegs.AIOSET.bit.AIO12  = 1)
#define FLASH_CS_RESET		    (GpioDataRegs.AIOCLEAR.bit.AIO12  = 1)
#define READ_MISO GpioDataRegs.GPADAT.bit.GPIO6const
Uint8 byDUMMY = 0xff;

void SPI_Init(void)
{
	FLASH_CS_SET;
	SPICLK_L;
	MOSI_H;

}
static void SPI_Set_MOSI(Uint8 val)
{
	if(val)
		MOSI_H;
	else
		MOSI_L;

}
/*******************************************************************************
* Function Name  : SPIReadWriteByte
* Description    : SPI��д����
* Input          : val 		��������
* Output         : RevData  ��������
* Return         : None
*******************************************************************************/
static Uint8 SPIReadWriteByte(Uint8 val)
{
	Uint16 i;
	Uint8  RevData = 0;
	//Uint16 wReadDO;
	for(i=0;
	 i<8;
	 i++)
	{
		SPICLK_L;
		SPI_Set_MOSI(val & 0x80);
		DSP28x_usDelay(1);
		val <<= 1;
		SPICLK_H;
		RevData <<= 1;
		//wReadDO = GpioDataRegs.GPADAT.bit.GPIO6;
		RevData |=READ_MISO;
		DSP28x_usDelay(1);
	}
	return RevData;

}
/*******************************************************************************
* Function Name  : W25Q16_ReadWriteDate
* Description    : SPIFLASH ��д����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
Uint8 W25Q16_ReadWriteDate(Uint8 val)
{
	return SPIReadWriteByte(val);

}
/*******************************************************************************
* Function Name  : W25q64_WriteEnable
* Description    : дʹ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void W25q16_WriteEnable(void)
{
	FLASH_CS_RESET;
	W25Q16_ReadWriteDate(0x06);
	FLASH_CS_SET;

}
/*******************************************************************************
* Function Name  : W25Q16_WaitForWriteEnd
* Description    : �ȴ�FLASH�ڲ�ʱ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void W25Q16_WaitForWriteEnd(void)
{
	Uint8 status = 0;
	FLASH_CS_RESET;
	W25Q16_ReadWriteDate(0x05);
	do{
		status = W25Q16_ReadWriteDate(byDUMMY);
	}while((status & 0x01) == 1);
	FLASH_CS_SET;

}
/*******************************************************************************
* Function Name  : W25q16_WriteStatusReg1
* Description    : д״̬�Ĵ���1
* Input          : status:״ֵ̬
* Output         : None
* Return         : None
*******************************************************************************/
void W25q16_WriteStatusReg1(Uint16 status)
{
	W25q16_WriteEnable();
	FLASH_CS_RESET;
	W25Q16_ReadWriteDate(0x01);
	W25Q16_ReadWriteDate(status & 0xFF);
	W25Q16_ReadWriteDate((status>>8) & 0xFF);
	FLASH_CS_SET;
	W25Q16_WaitForWriteEnd();

}
/*******************************************************************************
* Function Name  : W25q16_ReadStatus
* Description    : ��״̬�Ĵ���
* Input          : None
* Output         : None
* Return         : ״̬�Ĵ���ֵ
*******************************************************************************/
Uint16 W25q16_ReadStatus(void)
{
	Uint16 status=0;
	FLASH_CS_RESET;
	W25Q16_ReadWriteDate(0x05);
	status = W25Q16_ReadWriteDate(byDUMMY);
	FLASH_CS_SET;
	FLASH_CS_RESET;
	W25Q16_ReadWriteDate(0x35);
	status |= W25Q16_ReadWriteDate(byDUMMY)<<8;
	FLASH_CS_SET;
	return status;

}/*******************************************************************************
* Function Name  : W25q16_ReadDeviceID
* Description    : ��оƬID
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
Uint16 W25q16_ReadDeviceID(void)
{
	Uint8 DeviceID=0;
	FLASH_CS_RESET;
	W25Q16_ReadWriteDate(0xAB);
	W25Q16_ReadWriteDate(byDUMMY);
	W25Q16_ReadWriteDate(byDUMMY);
	W25Q16_ReadWriteDate(byDUMMY);
	DeviceID = W25Q16_ReadWriteDate(0X00);
	FLASH_CS_SET;
	return DeviceID;

}
/*******************************************************************************
* Function Name  : W25q16_ReadJEDE_ID
* Description    : ��оƬID
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
Uint16 W25q16_ReadJEDE_ID(void)
{
	Uint16 DeviceID=0;
	FLASH_CS_RESET;
	W25Q16_ReadWriteDate(0x9F);
	W25Q16_ReadWriteDate(byDUMMY);

//	W25Q16_ReadWriteDate(byDUMMY);

//	W25Q16_ReadWriteDate(0x00);
	DeviceID = W25Q16_ReadWriteDate(byDUMMY)<<8;
	DeviceID |= W25Q16_ReadWriteDate(byDUMMY);
	FLASH_CS_SET;
	return DeviceID;

}
/*******************************************************************************
* Function Name  : W25q16_Read
* Description    : ������
* Input          : buf:���ݻ�����len:����addr:��ʼ��ַ
* Output         : None
* Return         : None
*******************************************************************************/
void W25q16_Read(Uint8 *buf, Uint32 len, Uint32 addr)
{
	Uint32 i;
	FLASH_CS_RESET;
	W25Q16_ReadWriteDate(0x03);
	W25Q16_ReadWriteDate((addr & 0xFF0000)>>16);
	W25Q16_ReadWriteDate((addr & 0x00FF00)>>8);
	W25Q16_ReadWriteDate(addr & 0xFF);
	for (i=0; i<len; i++)
	{
		buf[i] = W25Q16_ReadWriteDate(byDUMMY);
	}
	FLASH_CS_SET;
}
/*******************************************************************************
* Function Name  : W25q16_4KErase
* Description    : 4KƬ����
* Input          : addr:��ʼ��ַ
* Output         : None
* Return         : None
*******************************************************************************/
void  W25q16_4KErase(Uint32 addr)
{
	W25q16_WriteEnable();
	FLASH_CS_RESET;
	W25Q16_ReadWriteDate(0x20);
	W25Q16_ReadWriteDate((addr & 0xFF0000)>>16);
	W25Q16_ReadWriteDate((addr & 0x00FF00)>>8);
	W25Q16_ReadWriteDate(addr & 0xFF);
	FLASH_CS_SET;
	W25Q16_WaitForWriteEnd();

}
/*******************************************************************************
* Function Name  : W25q16_32KErase
* Description    : 32KƬ����
* Input          : addr:��ʼ��ַ
* Output         : None
* Return         : None
*******************************************************************************/
void  W25q16_32KErase(Uint32 addr)
{
	W25q16_WriteEnable();
	FLASH_CS_RESET;
	W25Q16_ReadWriteDate(0x52);
	W25Q16_ReadWriteDate((addr & 0xFF0000)>>16);
	W25Q16_ReadWriteDate((addr & 0x00FF00)>>8);
	W25Q16_ReadWriteDate(addr & 0xFF);
	FLASH_CS_SET;
	W25Q16_WaitForWriteEnd();

}
/*******************************************************************************
* Function Name  : W25q16_64KErase
* Description    : 64KƬ����
* Input          : addr:��ʼ��ַ
* Output         : None
* Return         : None
* Date           : 2014-10-24
* Author         : ADT LL
*******************************************************************************/
void  W25q16_64KErase(Uint32 addr)
{
	W25q16_WriteEnable();
	FLASH_CS_RESET;
	W25Q16_ReadWriteDate(0xD8);
	W25Q16_ReadWriteDate((addr & 0xFF0000)>>16);
	W25Q16_ReadWriteDate((addr & 0x00FF00)>>8);
	W25Q16_ReadWriteDate(addr & 0xFF);
	FLASH_CS_SET;
	W25Q16_WaitForWriteEnd();

}
/*******************************************************************************
* Function Name  : W25q16_PageProgram
* Description    : ҳд
* Input          : buf:Ҫд�ڵ�����len:���ݳ���add:��ʼ��ַ
* Output         : None
* Return         : ״̬��
*******************************************************************************/
static void W25q16_PageProgram(Uint8 *buf, Uint16 len, Uint32 addr)
{
	//�Ƿ��ж�len ��addr �������⣿����//......W25q16_WriteEnable();
	FLASH_CS_RESET;
	W25Q16_ReadWriteDate(0x02);
	W25Q16_ReadWriteDate((addr & 0xFF0000)>>16);
	W25Q16_ReadWriteDate((addr & 0x00FF00)>>8);
	W25Q16_ReadWriteDate(addr & 0xFF);
	while (len--)
	{
		W25Q16_ReadWriteDate(*buf);
		buf++;
	}
	FLASH_CS_SET;
	W25Q16_WaitForWriteEnd();
}
/*******************************************************************************
* Function Name  : W25q64_Write
* Description    : ���ݴ洢
* Input          : buf:Ҫд�ڵ�����len:���ݳ���add:��ʼ��ַ
* Output         : None
* Return         : None
*******************************************************************************/
void W25q16_Write(Uint8 *buf, Uint16 len, Uint32 addr)
{
	Uint8 pagenum;
	Uint8 addrbyte;
	//��Ͱ�λ��ַaddrbyte = addr%W25Q16_PAGE_SIZE;
	if (len > (W25Q16_PAGE_SIZE - addrbyte))//��ҳ��
	{
		W25q16_PageProgram(buf, W25Q16_PAGE_SIZE - addrbyte, addr);
		//д����ҳaddr += W25Q16_PAGE_SIZE-addrbyte;
		buf += W25Q16_PAGE_SIZE-addrbyte;
		len -= W25Q16_PAGE_SIZE-addrbyte;
		pagenum = len/W25Q16_PAGE_SIZE;
		while (pagenum--){
			W25q16_PageProgram(buf, W25Q16_PAGE_SIZE, addr);
			addr += W25Q16_PAGE_SIZE;
			buf += W25Q16_PAGE_SIZE;
			len -= W25Q16_PAGE_SIZE;
		}
		W25q16_PageProgram(buf, len, addr);
	}
	else
	{
		W25q16_PageProgram(buf, len, addr);
	}
}
#endif

#include "spiflash.h"


#define SPI_DEVICE 	   (0)
#define CS_GPIO        (0x10)      /* �����ڲ�ʱ��Ƶ��    */
//===ȫ�ֱ����Ķ���
W25QXX_HandlerType  g_W25QXXDevice0 = { 0 };
pW25QXX_HandlerType pW25QXXDevice0 = &g_W25QXXDevice0;

//===ͳһ���ͺ���
Uint8(*W25QXX_SEND_CMD)(W25QXX_HandlerType *, Uint8, Uint8 *);



static void W25QXX_Delay_1ms(Uint32 delay)
{
	Uint32 x, y;
	Uint32 msCnt = 144000;
	for(x = ms; x > 0; x--)
	{
		for(y = msCnt; y > 0; y--)
		{
			asm("_nop");
		}
	}
}

static void W25QXX_SPI_CsWrite(Uint8 level)
{
    Uint8 ioData;

    ioData = GPIO_RGET(IODATA);
    ioData = level ? (ioData | CS_GPIO) : (ioData & ~CS_GPIO);

    GPIO_RSET(IODATA, ioData);
}




static Uint8 W25QXX_SPI_HW_Init(W25QXX_HandlerType *W25Qx)
{

    unsigned char ioDir;

    ioDir = GPIO_RGET(IODIR);
    ioDir |= CS_GPIO;
    GPIO_RSET(IODIR, ioDir);

	W25QXX_SPI_CsWrite(1);

	W25Qx->msgSPI = SPI_init(SPI_DEVICE, 16000000, SPI_8BIT, SPI_MODE_0);
    return OK_0;
}

static void W25QXX_WriteByte(W25QXX_HandlerType *W25Qx, Uint8 DataIn)
{
    if(!W25Qx->msgSPI)
    {
        return;
    }

	W25QXX_SPI_CsWrite(0);
    SPI_Write(W25Qx->msgSPI, &DataIn, 1);
}

static void W25QXX_ReadByte(W25QXX_HandlerType *W25Qx, Uint8 Reg, Uint8 *DataOut)
{
    Uint8 dataIn, dataOut;

    if(!W25Qx->msgSPI)
    {
        return;
    }



    SPI_Read(W25Qx->msgSPI, &Reg, DataOut, 1);


}

//
Uint8 W25QXX_SPI_HW_SendCmd(W25QXX_HandlerType *W25Qx, Uint8 cmd, Uint8 *pRVal)
{
	if(pRVal)
    	return W25QXX_ReadByte(W25Qx->msgSPI, cmd, pRVal);
	else
		return W25QXX_WriteByte(W25Qx->msgSPI, cmd);
}



Uint8 W25QXX_SPI_Init(W25QXX_HandlerType *W25Qx)
{


	W25Qx->msgSPI.msgModelIsHW = 1;
	W25QXX_SPI_HW_Init(W25Qx);


	W25QXX_SEND_CMD = W25QXX_SPI_HW_SendCmd;

    W25Qx->msgFuncDelayms = W25QXX_Delay_1ms;


    return OK_0;
}


Uint8 W25QXX_SPI_ReadRegSR1(W25QXX_HandlerType *W25Qx)
{
    Uint8 _return = 0;


    //--���Ͷ�ȡ״̬�Ĵ���������
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_READ_REG_SR1, NULL);
    W25QXX_SEND_CMD(W25Qx, 0xFF, &_return);

    return _return;
}


Uint8 W25QXX_SPI_ReadRegSR2(W25QXX_HandlerType *W25Qx)
{
    Uint8 _return = 0;

    //--���Ͷ�ȡ״̬�Ĵ���������
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_READ_REG_SR2, NULL);
    W25QXX_SEND_CMD(W25Qx, 0xFF, &_return);

    return _return;
}


void W25QXX_SPI_WriteRegSR1(W25QXX_HandlerType *W25Qx, Uint8 cmd)
{
    //--���Ͷ�ȡ״̬�Ĵ���������
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_WRITE_REG_SR, NULL);
    W25QXX_SEND_CMD(W25Qx, cmd, NULL);
}


void W25QXX_SPI_WriteRegSR2(W25QXX_HandlerType *W25Qx, Uint8 cmd)
{
    Uint8 _return = 0;

    //---��ȡSR1��ֵ
    _return =  W25QXX_SPI_ReadRegSR1(W25Qx,0);

    //--���Ͷ�ȡ״̬�Ĵ���������
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_WRITE_REG_SR, NULL);

    //---дSR1
    W25QXX_SEND_CMD(W25Qx, _return, NULL);

    //---дSR2
    W25QXX_SEND_CMD(W25Qx, cmd, NULL);
}


void W25QXX_SPI_EnableWrite(W25QXX_HandlerType *W25Qx)
{
    //--���Ͷ�ȡ״̬�Ĵ���������
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_ENABLE_WRITE, NULL);
}

void W25QXX_SPI_DisableWrite(W25QXX_HandlerType *W25Qx)
{
    //---���Ͷ�ȡ״̬�Ĵ���������
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_DISABLE_WRITE, NULL);
}

///
//��        ����
//��        �ܣ�  ��ȡоƬID
//�������:
//�������:
//˵        ����
//
Uint16 W25QXX_SPI_ReadID(W25QXX_HandlerType *W25Qx)
{
    Uint8 temp = 0;
    Uint16  _return = 0;

    //--���Ͷ�ȡID����
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_MANUFACT_DEVICE_ID, NULL);
    W25QXX_SEND_CMD(W25Qx, 0x00, NULL);
    W25QXX_SEND_CMD(W25Qx, 0x00, NULL);
    W25QXX_SEND_CMD(W25Qx, 0x00, NULL);

    //---��ȡ�豸��ID��Ϣ��λֵ
    W25QXX_SEND_CMD(W25Qx, 0xFF, &temp);
    _return = temp;
    //---��ȡ�豸��ID��Ϣ��λֵ
    W25QXX_SEND_CMD(W25Qx, 0xFF, &temp);
    _return = (_return << 8) + temp;

    W25Qx->msgChipID = _return;
    switch (W25Qx->msgChipID)
    {
        case 0xEF13:
            //---W25Q80---1MByte
            W25Qx->msgEraseDelayMS = 25000;
            W25Qx->msgEraseSectorDelayMS = 200;
            W25Qx->msgErase32KbBlockDelayMS = 1000;
            W25Qx->msgErase64KbBlockDelayMS = 1500;
            break;
        case 0xEF14:
            //---W25Q16---2Mbyte
            W25Qx->msgEraseDelayMS = 40000;
            W25Qx->msgEraseSectorDelayMS = 200;
            W25Qx->msgErase32KbBlockDelayMS = 1000;
            W25Qx->msgErase64KbBlockDelayMS = 1500;
            break;
        case 0xEF15:
            //---W25Q32---4Mbyte
            W25Qx->msgEraseDelayMS = 80000;
            W25Qx->msgEraseSectorDelayMS = 200;
            W25Qx->msgErase32KbBlockDelayMS = 1000;
            W25Qx->msgErase64KbBlockDelayMS = 1500;
            break;
        case 0xEF16:
            //---W25Q64---8Mbyte
            W25Qx->msgEraseDelayMS = 30000;
            W25Qx->msgEraseSectorDelayMS = 400;
            W25Qx->msgErase32KbBlockDelayMS = 800;
            W25Qx->msgErase64KbBlockDelayMS = 1000;
            break;
        default:
            break;
    }

    return _return;
}

///
//��        ����
//��        �ܣ�
//�������:
//�������:
//˵        ����
//
Uint8 W25QXX_SPI_WaitBusy(W25QXX_HandlerType *W25Qx,Uint32 timeOut)
{
    //---��ȡ״̬�Ĵ���
    Uint8 _return = 0;
	Uint8 timeOut = 0xFFFFFF;
    _return=W25QXX_SPI_ReadRegSR1(W25Qx,0);

    //---��ȡʱ���ǩ
    Uint32 nowTime = 0;
    Uint32 oldTime = 0;
    UINT64_T cnt = 0;


    //---�ȴ�BUSYλ���
    while ((_return & 0x01) == 0x01)
    {
        _return = W25QXX_SPI_ReadRegSR1(W25Qx,0);
		timeOut--;
		if(timeOut--)
		{
			return ERROR_1;
		}
    }

    return OK_0;
}


void W25QXX_SPI_ReadData(W25QXX_HandlerType *W25Qx, Uint32 addr, Uint8 *pVal, Uint16 length)
{
    Uint16 i = 0;

    //----���Ͷ�ȡ����
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_READ_DATA, NULL);

    //----����24bit��ַ
    W25QXX_SEND_CMD(W25Qx, (Uint8)(addr >> 16), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(addr >> 8), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(addr), NULL);

    //---ѭ����ȡ����
    for (i = 0; i < length; i++)
    {
        W25QXX_SEND_CMD(W25Qx, 0xFF, &pVal[i]);
    }

}

///
//��        ����
//��        �ܣ�
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_ReadFast(W25QXX_HandlerType *W25Qx, Uint32 addr, Uint8 *pVal, Uint16 length)
{
    Uint16 i = 0;
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    //--ʹ��Ƭѡ
    GPIO_OUT_0(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //----���Ͷ�ȡ����
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_READ_FAST, NULL);

    //----����24bit��ַ
    W25QXX_SEND_CMD(W25Qx, (Uint8)(addr >> 16), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(addr >> 8), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(addr), NULL);

    //---����α�ֽ�
    W25QXX_SEND_CMD(W25Qx, 0xFF, NULL);

    //---ѭ����ȡ����
    for (i = 0; i < length; i++)
    {
        W25QXX_SEND_CMD(W25Qx, 0xFF, &pVal[i]);
    }

    //---ȡ��Ƭѡ
    GPIO_OUT_1(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}

///
//��        ����
//��        �ܣ� дҳ����
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_WritePage(W25QXX_HandlerType *W25Qx, Uint32 addr, Uint8 *pVal, Uint16 length)
{
    Uint8 pageIndex = 0;
    Uint16 i = 0;
    Uint32 pageAdddr = 0;
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    //---�ж�Ҳ���ݵĸ����������256���ֽ�
    if (length > W25QXX_PAGE_BYTE_SIZE)
    {
        length = W25QXX_PAGE_BYTE_SIZE;
    }

    //---У��ҳ��ַ
    pageAdddr = (addr & W25QXX_PAGE_NUM_MASK);

    //---У��ҳ���
    pageIndex = (Uint8)(addr & W25QXX_PAGE_BYTE_MASK);

    //---����WEL
    W25QXX_SPI_EnableWrite(W25Qx, 0);

    //--ʹ��Ƭѡ
    GPIO_OUT_0(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //----����дҳ����
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_PAGE_PROGRAM, NULL);

    //----����24bit��ַ
    W25QXX_SEND_CMD(W25Qx, (Uint8)(pageAdddr >> 16), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(pageAdddr >> 8), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(pageAdddr), NULL);

    //---ѭ��д������
    for (i = pageIndex; i < length; i++)
    {
        W25QXX_SEND_CMD(W25Qx, pVal[i - pageIndex], NULL);
    }

    //---ȡ��Ƭѡ
    GPIO_OUT_1(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---�ȴ�д�����
    W25QXX_SPI_WaitBusy(W25Qx,100, 0);
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}
///
//��        ����
//��        �ܣ� ��������оƬ
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_EraseChip(W25QXX_HandlerType *W25Qx)
{
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    //---����WEL
    W25QXX_SPI_EnableWrite(W25Qx, 0);

    //---�ȴ�æ����
    W25QXX_SPI_WaitBusy(W25Qx,100, 0);
    GPIO_OUT_0(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---����Ƭ��������
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_ERASE_CHIP, NULL);
    GPIO_OUT_1(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---�ȴ���������
    W25QXX_SPI_WaitBusy(W25Qx,W25Qx->msgEraseDelayMS, 0);
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}

///
//��        ����
//��        �ܣ� ��������
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_EraseSuspend(W25QXX_HandlerType* W25Qx)
{
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    //---����WEL
    W25QXX_SPI_EnableWrite(W25Qx,0);

    //---�ȴ�æ����
    W25QXX_SPI_WaitBusy(W25Qx, 100, 0);
    GPIO_OUT_0(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---����Ƭ��������
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_ERASE_SUSPEND, NULL);
    GPIO_OUT_1(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---�ȴ���������
    W25QXX_SPI_WaitBusy(W25Qx, 100, 0);
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}

///
//��        ����
//��        �ܣ� �����ָ�
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_EraseResume(W25QXX_HandlerType* W25Qx)
{
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    //---����WEL
    W25QXX_SPI_EnableWrite(W25Qx, 0);

    //---�ȴ�æ����
    W25QXX_SPI_WaitBusy(W25Qx, 100, 0);
    GPIO_OUT_0(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---����Ƭ��������
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_ERASE_RESUME, NULL);
    GPIO_OUT_1(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---�ȴ���������
    W25QXX_SPI_WaitBusy(W25Qx, 100, 0);
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}

///
//��        ����
//��        �ܣ� ����ָ������
//�������: sectorNum---��������ţ�����0��511����ͬ���������ĸ�����ͬ
//�������:
//˵        ���������ĵ�ַ��0��511����
//
void W25QXX_SPI_EraseSector(W25QXX_HandlerType *W25Qx, Uint32 sectorNum)
{
    //---����������ַ
    sectorNum *= W25QXX_SECTOR_BYTE_SIZE;

    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    //---����WEL
    W25QXX_SPI_EnableWrite(W25Qx, 0);

    //---�ȴ�æ����
    W25QXX_SPI_WaitBusy(W25Qx,100, 0);
    GPIO_OUT_0(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //--���Ͷ�ȡ״̬�Ĵ���������
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_ERASE_SECTOR, NULL);

    //----����24bit��ַ
    W25QXX_SEND_CMD(W25Qx, (Uint8)(sectorNum >> 16), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(sectorNum >> 8), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(sectorNum), NULL);
    GPIO_OUT_1(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---�ȴ���������
    W25QXX_SPI_WaitBusy(W25Qx, W25Qx->msgEraseSectorDelayMS, 0);
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}

///
//��        ����
//��        �ܣ� ����ָ������
//�������: sectorNum---��������ţ�����0��511����ͬ���������ĸ�����ͬ
//�������:
//˵        ���������ĵ�ַ��0��511����
//
void W25QXX_SPI_EraseAddrSector(W25QXX_HandlerType* W25Qx, Uint32 sectorAddr)
{
    //---�����������
    sectorAddr /= W25QXX_SECTOR_BYTE_SIZE;
    W25QXX_SPI_EraseSector(W25Qx, sectorAddr,isAutoInit);
}

///
//��        ����
//��        �ܣ������飬32KB
//�������: block32KbNum---���������ţ�����0��63
//�������:
//˵        ����
//
void W25QXX_SPI_EraseBlock32KB(W25QXX_HandlerType *W25Qx, Uint32 block32KbNum)
{
    //---����32Kb��С��ĵ�ַ
    block32KbNum *= W25QXX_BLOCK_32KB_BYTE_SIZE;
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    //---����WEL
    W25QXX_SPI_EnableWrite(W25Qx, 0);

    //---�ȴ�æ����
    W25QXX_SPI_WaitBusy(W25Qx,100, 0);
    GPIO_OUT_0(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //--���Ͷ�ȡ״̬�Ĵ���������
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_ERASE_BLOCK_32KB, NULL);

    //----����24bit��ַ
    W25QXX_SEND_CMD(W25Qx, (Uint8)(block32KbNum >> 16), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(block32KbNum >> 8), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(block32KbNum), NULL);
    GPIO_OUT_1(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---�ȴ���������
    W25QXX_SPI_WaitBusy(W25Qx, W25Qx->msgErase32KbBlockDelayMS, 0);
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}

///
//��        ����
//��        �ܣ������飬32KB
//�������: block32KbNum---���������ţ�����0��63
//�������:
//˵        ����
//
void W25QXX_SPI_EraseAddrBlock32KB(W25QXX_HandlerType* W25Qx, Uint32 block32KbAddr)
{
    //---����32Kb��С������
    block32KbAddr /= W25QXX_BLOCK_32KB_BYTE_SIZE;
    W25QXX_SPI_EraseBlock32KB(W25Qx, block32KbAddr,isAutoInit);
}

///
//��        ����
//��        �ܣ������飬64KB
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_EraseBlock64KB(W25QXX_HandlerType *W25Qx, Uint32 blockNum)
{
    //---����64Kb��С��ĵ�ַ
    blockNum *= W25QXX_BLOCK_64KB_BYTE_SIZE;
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    //---����WEL
    W25QXX_SPI_EnableWrite(W25Qx, 0);

    //---�ȴ�æ����
    W25QXX_SPI_WaitBusy(W25Qx,100, 0);
    GPIO_OUT_0(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //--���Ͷ�ȡ״̬�Ĵ���������
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_ERASE_BLOCK_64KB, NULL);

    //----����24bit��ַ
    W25QXX_SEND_CMD(W25Qx, (Uint8)(blockNum >> 16), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(blockNum >> 8), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(blockNum), NULL);
    GPIO_OUT_1(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---�ȴ���������
    W25QXX_SPI_WaitBusy(W25Qx, W25Qx->msgErase64KbBlockDelayMS, 0);
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}

///
//��        ����
//��        �ܣ������飬64KB
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_EraseAddrBlock64KB(W25QXX_HandlerType* W25Qx, Uint32 blockAddr)
{
    //---����64Kb��С������
    blockAddr /= W25QXX_BLOCK_64KB_BYTE_SIZE;
    W25QXX_SPI_EraseBlock64KB( W25Qx, blockAddr,isAutoInit);
}

///
//��        ����
//��        �ܣ� �������ģʽ
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_PowerDown(W25QXX_HandlerType *W25Qx)
{
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }

    GPIO_OUT_0(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---���͵�������
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_POWER_DOWN, NULL);
    GPIO_OUT_1(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---TDP
    W25Qx->msgSPI.msgFuncDelayus(3);
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}

///
//��        ����
//��        �ܣ� ����
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_WakeUp(W25QXX_HandlerType *W25Qx)
{
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    GPIO_OUT_0(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---���ͻ�������
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_RELEASE_POWER_DOWN, NULL);
    GPIO_OUT_1(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---TRES1
    W25Qx->msgSPI.msgFuncDelayus(3);
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}

///
//��        ����
//��        �ܣ���ȡ���к�
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_ReadUniqueIDNumber(W25QXX_HandlerType *W25Qx, Uint8 *pVal)
{
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    GPIO_OUT_0(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //--���Ͷ�ȡID����
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_READ_UNIQUE_ID, NULL);
    W25QXX_SEND_CMD(W25Qx, 0x00, NULL);
    W25QXX_SEND_CMD(W25Qx, 0x00, NULL);
    W25QXX_SEND_CMD(W25Qx, 0x00, NULL);
    W25QXX_SEND_CMD(W25Qx, 0x00, NULL);

    //---��ȡ64Bit��UniqueSerialNumber
    W25QXX_SEND_CMD(W25Qx, 0xFF, &pVal[0]);
    W25QXX_SEND_CMD(W25Qx, 0xFF, &pVal[1]);
    W25QXX_SEND_CMD(W25Qx, 0xFF, &pVal[2]);
    W25QXX_SEND_CMD(W25Qx, 0xFF, &pVal[3]);
    W25QXX_SEND_CMD(W25Qx, 0xFF, &pVal[4]);
    W25QXX_SEND_CMD(W25Qx, 0xFF, &pVal[5]);
    W25QXX_SEND_CMD(W25Qx, 0xFF, &pVal[6]);
    W25QXX_SEND_CMD(W25Qx, 0xFF, &pVal[7]);
    GPIO_OUT_1(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}
///
//��        ����
//��        �ܣ�
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_ReadJEDECID(W25QXX_HandlerType *W25Qx, Uint8 *pVal)
{
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    GPIO_OUT_0(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //--���Ͷ�ȡID����
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_JEDEC_ID, NULL);
    W25QXX_SEND_CMD(W25Qx, 0xFF, &pVal[0]);
    W25QXX_SEND_CMD(W25Qx, 0xFF, &pVal[1]);
    W25QXX_SEND_CMD(W25Qx, 0xFF, &pVal[2]);
    GPIO_OUT_1(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}

///
//��        ����
//��        �ܣ��������ܼĴ���
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_EraseSecurityReg(W25QXX_HandlerType *W25Qx, Uint32 regAddr)
{
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    //---����WEL
    W25QXX_SPI_EnableWrite(W25Qx,0);

    //---�ȴ�æ����
    W25QXX_SPI_WaitBusy(W25Qx,100,0);
    GPIO_OUT_0(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //--���Ͷ�ȡ״̬�Ĵ���������
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_ERASE_SECURITY_REG, NULL);

    //----����24bit��ַ
    W25QXX_SEND_CMD(W25Qx, (Uint8)(regAddr >> 16), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(regAddr >> 8), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(regAddr), NULL);
    GPIO_OUT_1(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---�ȴ���������
    W25QXX_SPI_WaitBusy(W25Qx,100,0);
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}

///
//��        ����
//��        �ܣ���̼��ܼĴ���
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_ProgramSecurityReg(W25QXX_HandlerType *W25Qx, Uint32 regAddr, Uint8 *pVal, Uint16 length)
{
    Uint16 i = 0;

    //---�ж�Ҳ���ݵĸ����������256���ֽ�
    if (length > W25QXX_PAGE_BYTE_SIZE)
    {
        length = W25QXX_PAGE_BYTE_SIZE;
    }
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    //---����WEL
    W25QXX_SPI_EnableWrite(W25Qx,0);

    //--ʹ��Ƭѡ
    GPIO_OUT_0(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //----����дҳ����
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_PROGRAM_SECURITY_REG, NULL);

    //----����24bit��ַ
    W25QXX_SEND_CMD(W25Qx, (Uint8)(regAddr >> 16), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(regAddr >> 8), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(regAddr), NULL);

    //---ѭ��д������
    for (i = 0; i < length; i++)
    {
        W25QXX_SEND_CMD(W25Qx, pVal[i], NULL);
    }

    //---ȡ��Ƭѡ
    GPIO_OUT_1(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---�ȴ�д�����
    W25QXX_SPI_WaitBusy(W25Qx,100,0);
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}

///
//��        ����
//��        �ܣ���ȡ���ܼĴ���
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_ReadSecurityReg(W25QXX_HandlerType *W25Qx, Uint32 regAddr, Uint8 *pVal, Uint16 length)
{
    Uint16 i = 0;
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    //--ʹ��Ƭѡ
    GPIO_OUT_0(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //----���Ͷ�ȡ����
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_READ_SECURITY_REG, NULL);

    //----����24bit��ַ
    W25QXX_SEND_CMD(W25Qx, (Uint8)(regAddr >> 16), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(regAddr >> 8), NULL);
    W25QXX_SEND_CMD(W25Qx, (Uint8)(regAddr), NULL);

    //---ѭ����ȡ����
    for (i = 0; i < length; i++)
    {
        W25QXX_SEND_CMD(W25Qx, 0xFF, &pVal[i]);
    }

    //---ȡ��Ƭѡ
    GPIO_OUT_1(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}

///
//��        ����
//��        �ܣ� ����
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_EnableReset(W25QXX_HandlerType *W25Qx)
{
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    GPIO_OUT_0(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---����ʹ�ܸ�λ����
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_ENABLE_RESET, NULL);
    GPIO_OUT_1(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}

///
//��        ����
//��        �ܣ� ����
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_Reset(W25QXX_HandlerType *W25Qx)
{
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    GPIO_OUT_0(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);

    //---����ʦ���ܹ���λ����
    W25QXX_SEND_CMD(W25Qx, W25QXX_CMD_RESET, NULL);
    GPIO_OUT_1(W25Qx->msgSPI.msgCS.msgGPIOPort, W25Qx->msgSPI.msgCS.msgGPIOBit);
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}

///
//��        ����
//��        �ܣ�ֻд�벻У�������Ƿ����д��Flash
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_WriteNoCheck(W25QXX_HandlerType *W25Qx, Uint32 addr, Uint8* pVal, Uint16 length)
{
    //---����ҳ���ֽ���
    Uint16 byteNum = W25QXX_PAGE_BYTE_SIZE - (Uint8)(addr&W25QXX_PAGE_BYTE_MASK);

    if (length <= byteNum)
    {
        //---������256���ֽ�
        byteNum = length;
    }
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    //---ѭ��д������
    while (1)
    {
        W25QXX_SPI_WritePage(W25Qx, addr, pVal, byteNum,0);
        if (length == byteNum)
        {
            //---д�������
            break;
        }
        else
        {
            pVal += byteNum;
            addr += byteNum;

            //---��ȥ�Ѿ�д���˵��ֽ���
            length -= byteNum;
            if (length > W25QXX_PAGE_BYTE_SIZE)
            {
                //---һ�ο���д��256���ֽ�
                byteNum = W25QXX_PAGE_BYTE_SIZE;
            }
            else
            {
                //---����256���ֽ���
                byteNum = length;
            }
        }
    };
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}

///
//��        ����
//��        �ܣ�
//�������:
//�������:
//˵        ����
//
void W25QXX_SPI_WriteAndCheck(W25QXX_HandlerType *W25Qx, Uint32 addr, Uint8* pVal, Uint16 length)
{
    Uint32 sectorAddr = 0;
    Uint16 sectorOffset = 0;
    Uint16 sectorRemainSize = 0;
    Uint16 i = 0;

    //---������ַ
    sectorAddr = addr & W25QXX_SECTOR_NUM_MASK;

    //---�������ڵ�ƫ��
    sectorOffset = addr & W25QXX_SECTOR_BYTE_MASK;

    //---����ʣ��ռ��С
    sectorRemainSize = W25QXX_SECTOR_BYTE_SIZE - sectorOffset;

    if (length <= sectorRemainSize)
    {
        //---������4096���ֽ�
        sectorRemainSize = length;
    }
    if (isAutoInit)
    {
        W25QXX_SPI_AutoInit(W25Qx);
    }
    //---ѭ��д������
    while (1)
    {
        //---������������������
        W25QXX_SPI_ReadData(W25Qx, sectorAddr , W25Qx->msgBuffer, W25QXX_SECTOR_BYTE_SIZE,0);

        //---У������
        for (i = 0; i < sectorRemainSize; i++)
        {
            if (W25Qx->msgBuffer[sectorOffset + i] != 0XFF)
            {
                //---��Ҫ����
                break;
            }
        }

        //---�ж��Ƿ���Ҫ����
        if (i < sectorRemainSize)
        {
            //---�����������
            W25QXX_SPI_EraseAddrSector(W25Qx, sectorAddr,0);

            //---����
            for (i = 0; i < sectorRemainSize; i++)
            {
                W25Qx->msgBuffer[i + sectorOffset] = pVal[i];
            }

            //---д����������
            W25QXX_SPI_WriteNoCheck(W25Qx, sectorAddr, W25Qx->msgBuffer, W25QXX_SECTOR_BYTE_SIZE,0);
        }
        else
        {
            //---д�Ѿ������˵�,ֱ��д������ʣ������.
            W25QXX_SPI_WriteNoCheck(W25Qx, addr, pVal, sectorRemainSize,0);
        }

        //---�ж��Ƿ�д�����
        if (length == sectorRemainSize)
        {
            //---д�������
            break;
        }
        else
        {
            //---д��δ����
            //---������ַƫ��
            sectorAddr+= W25QXX_SECTOR_BYTE_SIZE;

            //---ƫ��λ��Ϊ0
            sectorOffset = 0;

            //---ָ��ƫ��
            pVal += sectorRemainSize;

            //---д��ַƫ��
            addr += sectorRemainSize;

            //---�ֽ����ݼ�
            length -= sectorRemainSize;

            //---�ж���һ�������Ƿ��ܹ�д��
            if (length >= W25QXX_SECTOR_BYTE_SIZE)
            {
                //---��һ����������д����
                sectorRemainSize = W25QXX_SECTOR_BYTE_SIZE;
            }
            else
            {
                //---��һ����������д����
                sectorRemainSize = length;
            }
        }
    };
    if (isAutoInit)
    {
        W25QXX_SPI_AutoDeInit(W25Qx);
    }
}
��������������������������������
��Ȩ����������ΪCSDN������Haiguozhe����ԭ�����£���ѭCC 4.0 BY-SA��ȨЭ�飬ת���븽��ԭ�ĳ������Ӽ���������
ԭ�����ӣ�https://blog.csdn.net/haiguozhe/article/details/97620302