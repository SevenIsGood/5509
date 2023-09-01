#include <csl.h>ESP8266
#include <csl_pll.h>
#include <csl_mcbsp.h>
#include <csl_gpio.h>

#include "CH432INC.H"
#include "spi.h"



/**********************************************************************
        ����CH432����0�ļĴ�����ַ
***********************************************************************/

#define CH432_RBR_PORT     REG_RBR_ADDR     /* �ٶ�CH432���ջ���Ĵ���0��I/O��ַ */
#define CH432_THR_PORT     REG_THR_ADDR     /* �ٶ�CH432���ͱ��ּĴ���0��I/O��ַ */
#define CH432_IER_PORT     REG_IER_ADDR     /* �ٶ�CH432�ж�ʹ�ܼĴ���0��I/O��ַ */
#define CH432_IIR_PORT     REG_IIR_ADDR     /* �ٶ�CH432�ж�ʶ��Ĵ���0��I/O��ַ */
#define CH432_FCR_PORT     REG_FCR_ADDR     /* �ٶ�CH432FIFO���ƼĴ���0��I/O��ַ */
#define CH432_LCR_PORT     REG_LCR_ADDR     /* �ٶ�CH432��·���ƼĴ���0��I/O��ַ */
#define CH432_MCR_PORT     REG_MCR_ADDR     /* �ٶ�CH432MODEM���ƼĴ���0��I/O��ַ */
#define CH432_LSR_PORT     REG_LSR_ADDR     /* �ٶ�CH432��·״̬�Ĵ���0��I/O��ַ */
#define CH432_MSR_PORT     REG_MSR_ADDR     /* �ٶ�CH432MODEM״̬�Ĵ���0��I/O��ַ */
#define CH432_SCR_PORT     REG_SCR_ADDR     /* �ٶ�CH432�û��ɶ���Ĵ���0��I/O��ַ */
#define CH432_DLL_PORT     REG_DLL_ADDR
#define CH432_DLM_PORT     REG_DLM_ADDR

/**************************************************************************
        ����CH432����1�ļĴ�����ַ
**************************************************************************/

#define CH432_RBR1_PORT    REG_RBR1_ADDR    /* �ٶ�CH432���ջ���Ĵ���1��I/O��ַ */
#define CH432_THR1_PORT    REG_THR1_ADDR    /* �ٶ�CH432���ͱ��ּĴ���1��I/O��ַ */
#define CH432_IER1_PORT    REG_IER1_ADDR    /* �ٶ�CH432�ж�ʹ�ܼĴ���1��I/O��ַ */
#define CH432_IIR1_PORT    REG_IIR1_ADDR    /* �ٶ�CH432�ж�ʶ��Ĵ���1��I/O��ַ */
#define CH432_FCR1_PORT    REG_FCR1_ADDR    /* �ٶ�CH432FIFO���ƼĴ���1��I/O��ַ */
#define CH432_LCR1_PORT    REG_LCR1_ADDR    /* �ٶ�CH432��·���ƼĴ���1��I/O��ַ */
#define CH432_MCR1_PORT    REG_MCR1_ADDR    /* �ٶ�CH432MODEM���ƼĴ���1��I/O��ַ */
#define CH432_LSR1_PORT    REG_LSR1_ADDR    /* �ٶ�CH432��·״̬�Ĵ���1��I/O��ַ */
#define CH432_MSR1_PORT    REG_MSR1_ADDR    /* �ٶ�CH432MODEM״̬�Ĵ���1��I/O��ַ */
#define CH432_SCR1_PORT    REG_SCR1_ADDR    /* �ٶ�CH432�û��ɶ���Ĵ���1��I/O��ַ */
#define CH432_DLL1_PORT    REG_DLL1_ADDR
#define CH432_DLM1_PORT    REG_DLM1_ADDR

#define SPI_DEV       0         /* ����CH432����0ͨѶ������ */
#define CH432_BPS    115200    /* ����CH432����0ͨѶ������ */
#define FPCLK        1843200   /* �����ڲ�ʱ��Ƶ��    */
#define CS_GPIO        0x80      /* �����ڲ�ʱ��Ƶ��    */


int *spi_handle = NULL;

unsigned char SEND_STRING[ ] = { "444444555566666777788889999000009999995555555hello" };
unsigned char SEND_STRING1[ ] = { "33333333wwwwww" };
unsigned char buf[100];    /* �������������� */
unsigned char rcvbuf[64];    /* �������������� */S

static void CH432_WriteByte(char data)
{
    if(!spi_handle)
    {
        return;
    }

    SPI_Write(spi_handle, &data, 1);

}

static char CH432_ReadByte(char reg)
{
    char dataIn, dataOut;

    if(!spi_handle)
    {
        return;
    }

    SPI_Read(spi_handle, &reg, &dataOut, 1);

}

static void CH432_CsWrite(unsigned char level)
{
    unsigned char  ioData;

    ioData = GPIO_RGET(IODATA);
    ioData = level ? (ioData | CS_GPIO) : (ioData & ~CS_GPIO);

    GPIO_RSET(IODATA, ioData);
}


static unsigned char CH432_ReadData( unsigned char mAddr )    /* ��ָ���Ĵ�����ȡ���� */
{
    unsigned char readBack;

    CH432_CsWrite(0);
    CH432_WriteByte( ( mAddr<<2 ) &0xFD );              /* λ5-2Ϊ��ǰ������ַ��λ1Ϊ0������ */
    readBack = Spi432InByte( );
    CH432_CsWrite(1);

    return readBack;
}

static void CH432_WriteData( unsigned char mAddr, unsigned char mData )    /* ��ָ���Ĵ���д������ */
{
    CH432_CsWrite(0);
    CH432_WriteByte( (mAddr<<2) | 0x02 );                           /* λ5-2Ϊ��ǰ������ַ��λ1Ϊ1д���� */
    CH432_WriteByte( mData );
    CH432_CsWrite(1);
}

static void CH432_WriteBlock( unsigned char mAddr, unsigned char mLen, Punsigned char mBuf )    /* ��ָ����ʼ��ַд�����ݿ� */
{
    while ( mLen -- ) CH432_WriteData( mAddr, *mBuf++ );
}


/*���ڽ��ܷ��Ͳ���*/
void UART_Init()
{
    unsigned short div;
    unsigned char DLL,DLM;
    unsigned char ioDir;

    ioDir = GPIO_RGET(IODIR);
    ioDir |= CS_GPIO;
    GPIO_RSET(IODIR, ioDir);

    CH432_CsWrite(1);

    spi_handle = SPI_Open(SPI_DEV, FPCLK, SPI_8BIT, SPI_MODE_0);


    div = (FPCLK>>4)/CH432_BPS;
    DLM = div>>8;
    DLL = div&0xff;

    CH432_WriteData( CH432_LCR_PORT, BIT_LCR_DLAB );    /* ����DLABΪ1 */
    CH432_WriteData( CH432_DLL_PORT, DLL );             /* ���ò����� */
    CH432_WriteData( CH432_DLM_PORT, DLM );
    CH432_WriteData( CH432_FCR_PORT, BIT_FCR_RECVTG1 | BIT_FCR_RECVTG0 | BIT_FCR_FIFOEN );    /* ����FIFOģʽ��������Ϊ14 */
    CH432_WriteData( CH432_LCR_PORT, BIT_LCR_WORDSZ1
                                  | BIT_LCR_WORDSZ0 );    /* �ֳ�8λ��1λֹͣλ����У�� */
    CH432_WriteData( CH432_IER_PORT, BIT_IER_IEMODEM | BIT_IER_IELINES | BIT_IER_IETHRE | BIT_IER_IERECV );    /* ʹ���ж� */
    CH432_WriteData( CH432_MCR_PORT, BIT_MCR_OUT2 | BIT_MCR_RTS | BIT_MCR_DTR );    /* �����ж����,DTR,RTSΪ1 */
}


void UART_SendByte( unsigned char dat )    /* CH432����0����һ�ֽ��ӳ��� */
{
    while( ( CH432_ReadData( CH432_LSR_PORT ) & BIT_LSR_THRE ) == 0 );    /* �ȴ����ݷ������ */
    CH432_WriteData( CH432_THR_PORT, dat );
}

unsigned char UART_RcvByte()    /* CH432����0����һ�ֽ��ӳ��� */
{
    unsigned char Rcvdat;
    if( !( CH432_ReadData( CH432_LSR_PORT ) & ( BIT_LSR_BREAKINT | BIT_LSR_FRAMEERR | BIT_LSR_PARERR | BIT_LSR_OVERR ) ) )    /*  b1-b4�޴��� */
    {
        while( ( CH432_ReadData( CH432_LSR_PORT ) & BIT_LSR_DATARDY ) == 0 );    /* �ȴ�����׼���� */
        Rcvdat = CH432_ReadData( CH432_RBR_PORT );    /* �ӽ��ջ���Ĵ����������� */
        return( Rcvdat );
    }
    else CH432_ReadData( CH432_RBR_PORT );    /* �д������ */
}

void UART_SendStr( unsigned char *str )    /* CH432����0����һ�ַ����ӳ��� */
{
    while( 1 )
    {
        if( *str == '\0' ) break;
        UART_SendByte( *str++ );
    }
}



void  UART_Send( unsigned char *Data, unsigned char Num )               /* ����FIFO,CH432����0���Ͷ��ֽ��ӳ��� */
{
    do
    {
        while( ( CH432_ReadData( CH432_LSR_PORT ) & BIT_LSR_THRE ) == 0 );    /* �ȴ����ݷ������ */
        CH432_WriteData( CH432_THR_PORT, *Data++ );
    }
    while( --Num );
}


unsigned char UART_Rcv( unsigned char *buf )                           /* ����FIFO,CH432����0���ն��ֽ��ӳ��� */
{
    unsigned char RcvNum = 0;
    if( !( CH432_ReadData( CH432_LSR_PORT ) & ( BIT_LSR_BREAKINT | BIT_LSR_FRAMEERR | BIT_LSR_PARERR | BIT_LSR_OVERR ) ) )    /* b1-b4�޴��� */
    {
        while( ( CH432_ReadData( CH432_LSR_PORT ) & BIT_LSR_DATARDY ) == 0 );    /* �ȴ�����׼���� */
        do
        {
            *buf++ = CH432_ReadData( CH432_RBR_PORT );    /* �ӽ��ջ���Ĵ����������� */
            RcvNum++;
        }
        while( ( CH432_ReadData( CH432_LSR_PORT ) & BIT_LSR_DATARDY ) == 0x01 );
    }
    else CH432_ReadData( CH432_RBR_PORT );
    return( RcvNum );
}


void UART_FifoSend( unsigned char *Data, unsigned char Num )   	 /* ����FIFO,һ�����16�ֽڣ�CH432����0���Ͷ��ֽ��ӳ��� */
{
    while( 1 )
    {
        while( ( CH432_ReadData( CH432_LSR_PORT ) & BIT_LSR_TEMT ) == 0 );    /* �ȴ����ݷ�����ϣ�THR,TSRȫ�� */
        if( Num <= 16 )
        {
            CH432_WriteBlock( CH432_THR_PORT, Num, Data );
            break;
        }
        else
        {
            CH432_WriteBlock( CH432_THR_PORT, 16, Data );
            Num -= 16;
            Data += 16;
        }
    }
}


void UART_Auto_HandFlow0_Ctrl( )    /* ����0�Զ�Ӳ�������� */
{
    CH432_WriteData( CH432_MCR_PORT, BIT_MCR_AFE | BIT_MCR_OUT2 | BIT_MCR_RTS );    /* ����MCR�Ĵ�����AFE��RTSΪ1 */
}

void UART_Idle()    /* ��ѯCH432 INT0�����ӳ��� */
{
    unsigned char InterruptStatus;
    unsigned char ModemStatus;
    unsigned char RcvNum, s;
    while( 1 )
    {
		InterruptStatus=CH432_ReadData( CH432_IIR_PORT ) & ( ~ CH432_IIR_FIFOS_ENABLED );    /* ������1���ж�״̬ */
		if( ( InterruptStatus & 0x01 ) ) continue;    /* û���ж��˳� */
		else
		{
			switch( InterruptStatus )
			{
				case INT_MODEM_CHANGE:
					ModemStatus = CH432_ReadData( CH432_MSR_PORT );
					CH432_WriteData( CH432_THR_PORT, ModemStatus );
					if ( ModemStatus == 0x30)          /* Modem�źŷ������ݱ仯 */
					{
						UART_SendStr( SEND_STRING1 );
					}
					else if ( ModemStatus == 0x11 )    /* Modem �źŽ������ݱ仯 */
					{
						UART_SendStr( SEND_STRING );
					}
					else if ( ModemStatus == 0x22 )    /* Modem �źŽ������ݱ仯 */
					{
						s= CH432_ReadData( CH432_RBR_PORT );
						CH432_WriteData( CH432_THR_PORT, s );
					}
					break;
				case INT_NOINT:    /* û���ж� */
					continue;
				case INT_THR_EMPTY:    /* ���ͱ��ּĴ������ж� */
					CH432_WriteData( CH432_THR_PORT, 0xdd );
					break;
				case INT_RCV_SUCCESS:    /* ���ڽ��տ��������ж� */
					s=UART_RcvByte();
					UART_SendByte( s );
					break;
				case INT_RCV_LINES:        /* ������·״̬�ж� */
					CH432_ReadData( CH432_LSR_PORT);
					break;
				case INT_RCV_OVERTIME:    /* �������ݳ�ʱ�ж� */
					RcvNum = CH432Seril0Rcv( buf );
					UART_SendByte( RcvNum );
					UART_Send( buf, RcvNum );
					break;
				default:    /* �����ܷ������ж� */
					break;
			}

			return;
		}
	}
}