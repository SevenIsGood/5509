#include <csl.h>ESP8266
#include <csl_pll.h>
#include <csl_mcbsp.h>
#include <csl_gpio.h>

#include "CH432INC.H"
#include "spi.h"



/**********************************************************************
        定义CH432串口0的寄存器地址
***********************************************************************/

#define CH432_RBR_PORT     REG_RBR_ADDR     /* 假定CH432接收缓冲寄存器0的I/O地址 */
#define CH432_THR_PORT     REG_THR_ADDR     /* 假定CH432发送保持寄存器0的I/O地址 */
#define CH432_IER_PORT     REG_IER_ADDR     /* 假定CH432中断使能寄存器0的I/O地址 */
#define CH432_IIR_PORT     REG_IIR_ADDR     /* 假定CH432中断识别寄存器0的I/O地址 */
#define CH432_FCR_PORT     REG_FCR_ADDR     /* 假定CH432FIFO控制寄存器0的I/O地址 */
#define CH432_LCR_PORT     REG_LCR_ADDR     /* 假定CH432线路控制寄存器0的I/O地址 */
#define CH432_MCR_PORT     REG_MCR_ADDR     /* 假定CH432MODEM控制寄存器0的I/O地址 */
#define CH432_LSR_PORT     REG_LSR_ADDR     /* 假定CH432线路状态寄存器0的I/O地址 */
#define CH432_MSR_PORT     REG_MSR_ADDR     /* 假定CH432MODEM状态寄存器0的I/O地址 */
#define CH432_SCR_PORT     REG_SCR_ADDR     /* 假定CH432用户可定义寄存器0的I/O地址 */
#define CH432_DLL_PORT     REG_DLL_ADDR
#define CH432_DLM_PORT     REG_DLM_ADDR

/**************************************************************************
        定义CH432串口1的寄存器地址
**************************************************************************/

#define CH432_RBR1_PORT    REG_RBR1_ADDR    /* 假定CH432接收缓冲寄存器1的I/O地址 */
#define CH432_THR1_PORT    REG_THR1_ADDR    /* 假定CH432发送保持寄存器1的I/O地址 */
#define CH432_IER1_PORT    REG_IER1_ADDR    /* 假定CH432中断使能寄存器1的I/O地址 */
#define CH432_IIR1_PORT    REG_IIR1_ADDR    /* 假定CH432中断识别寄存器1的I/O地址 */
#define CH432_FCR1_PORT    REG_FCR1_ADDR    /* 假定CH432FIFO控制寄存器1的I/O地址 */
#define CH432_LCR1_PORT    REG_LCR1_ADDR    /* 假定CH432线路控制寄存器1的I/O地址 */
#define CH432_MCR1_PORT    REG_MCR1_ADDR    /* 假定CH432MODEM控制寄存器1的I/O地址 */
#define CH432_LSR1_PORT    REG_LSR1_ADDR    /* 假定CH432线路状态寄存器1的I/O地址 */
#define CH432_MSR1_PORT    REG_MSR1_ADDR    /* 假定CH432MODEM状态寄存器1的I/O地址 */
#define CH432_SCR1_PORT    REG_SCR1_ADDR    /* 假定CH432用户可定义寄存器1的I/O地址 */
#define CH432_DLL1_PORT    REG_DLL1_ADDR
#define CH432_DLM1_PORT    REG_DLM1_ADDR

#define SPI_DEV       0         /* 定义CH432串口0通讯波特率 */
#define CH432_BPS    115200    /* 定义CH432串口0通讯波特率 */
#define FPCLK        1843200   /* 定义内部时钟频率    */
#define CS_GPIO        0x80      /* 定义内部时钟频率    */


int *spi_handle = NULL;

unsigned char SEND_STRING[ ] = { "444444555566666777788889999000009999995555555hello" };
unsigned char SEND_STRING1[ ] = { "33333333wwwwww" };
unsigned char buf[100];    /* 缓冲区保存数据 */
unsigned char rcvbuf[64];    /* 缓冲区保存数据 */S

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


static unsigned char CH432_ReadData( unsigned char mAddr )    /* 从指定寄存器读取数据 */
{
    unsigned char readBack;

    CH432_CsWrite(0);
    CH432_WriteByte( ( mAddr<<2 ) &0xFD );              /* 位5-2为当前操作地址，位1为0读操作 */
    readBack = Spi432InByte( );
    CH432_CsWrite(1);

    return readBack;
}

static void CH432_WriteData( unsigned char mAddr, unsigned char mData )    /* 向指定寄存器写入数据 */
{
    CH432_CsWrite(0);
    CH432_WriteByte( (mAddr<<2) | 0x02 );                           /* 位5-2为当前操作地址，位1为1写操作 */
    CH432_WriteByte( mData );
    CH432_CsWrite(1);
}

static void CH432_WriteBlock( unsigned char mAddr, unsigned char mLen, Punsigned char mBuf )    /* 向指定起始地址写入数据块 */
{
    while ( mLen -- ) CH432_WriteData( mAddr, *mBuf++ );
}


/*串口接受发送操作*/
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

    CH432_WriteData( CH432_LCR_PORT, BIT_LCR_DLAB );    /* 设置DLAB为1 */
    CH432_WriteData( CH432_DLL_PORT, DLL );             /* 设置波特率 */
    CH432_WriteData( CH432_DLM_PORT, DLM );
    CH432_WriteData( CH432_FCR_PORT, BIT_FCR_RECVTG1 | BIT_FCR_RECVTG0 | BIT_FCR_FIFOEN );    /* 设置FIFO模式，触发点为14 */
    CH432_WriteData( CH432_LCR_PORT, BIT_LCR_WORDSZ1
                                  | BIT_LCR_WORDSZ0 );    /* 字长8位，1位停止位、无校验 */
    CH432_WriteData( CH432_IER_PORT, BIT_IER_IEMODEM | BIT_IER_IELINES | BIT_IER_IETHRE | BIT_IER_IERECV );    /* 使能中断 */
    CH432_WriteData( CH432_MCR_PORT, BIT_MCR_OUT2 | BIT_MCR_RTS | BIT_MCR_DTR );    /* 允许中断输出,DTR,RTS为1 */
}


void UART_SendByte( unsigned char dat )    /* CH432串口0发送一字节子程序 */
{
    while( ( CH432_ReadData( CH432_LSR_PORT ) & BIT_LSR_THRE ) == 0 );    /* 等待数据发送完毕 */
    CH432_WriteData( CH432_THR_PORT, dat );
}

unsigned char UART_RcvByte()    /* CH432串口0接收一字节子程序 */
{
    unsigned char Rcvdat;
    if( !( CH432_ReadData( CH432_LSR_PORT ) & ( BIT_LSR_BREAKINT | BIT_LSR_FRAMEERR | BIT_LSR_PARERR | BIT_LSR_OVERR ) ) )    /*  b1-b4无错误 */
    {
        while( ( CH432_ReadData( CH432_LSR_PORT ) & BIT_LSR_DATARDY ) == 0 );    /* 等待数据准备好 */
        Rcvdat = CH432_ReadData( CH432_RBR_PORT );    /* 从接收缓冲寄存器读出数据 */
        return( Rcvdat );
    }
    else CH432_ReadData( CH432_RBR_PORT );    /* 有错误清除 */
}

void UART_SendStr( unsigned char *str )    /* CH432串口0发送一字符串子程序 */
{
    while( 1 )
    {
        if( *str == '\0' ) break;
        UART_SendByte( *str++ );
    }
}



void  UART_Send( unsigned char *Data, unsigned char Num )               /* 禁用FIFO,CH432串口0发送多字节子程序 */
{
    do
    {
        while( ( CH432_ReadData( CH432_LSR_PORT ) & BIT_LSR_THRE ) == 0 );    /* 等待数据发送完毕 */
        CH432_WriteData( CH432_THR_PORT, *Data++ );
    }
    while( --Num );
}


unsigned char UART_Rcv( unsigned char *buf )                           /* 禁用FIFO,CH432串口0接收多字节子程序 */
{
    unsigned char RcvNum = 0;
    if( !( CH432_ReadData( CH432_LSR_PORT ) & ( BIT_LSR_BREAKINT | BIT_LSR_FRAMEERR | BIT_LSR_PARERR | BIT_LSR_OVERR ) ) )    /* b1-b4无错误 */
    {
        while( ( CH432_ReadData( CH432_LSR_PORT ) & BIT_LSR_DATARDY ) == 0 );    /* 等待数据准备好 */
        do
        {
            *buf++ = CH432_ReadData( CH432_RBR_PORT );    /* 从接收缓冲寄存器读出数据 */
            RcvNum++;
        }
        while( ( CH432_ReadData( CH432_LSR_PORT ) & BIT_LSR_DATARDY ) == 0x01 );
    }
    else CH432_ReadData( CH432_RBR_PORT );
    return( RcvNum );
}


void UART_FifoSend( unsigned char *Data, unsigned char Num )   	 /* 启用FIFO,一次最多16字节，CH432串口0发送多字节子程序 */
{
    while( 1 )
    {
        while( ( CH432_ReadData( CH432_LSR_PORT ) & BIT_LSR_TEMT ) == 0 );    /* 等待数据发送完毕，THR,TSR全空 */
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


void UART_Auto_HandFlow0_Ctrl( )    /* 串口0自动硬件流控制 */
{
    CH432_WriteData( CH432_MCR_PORT, BIT_MCR_AFE | BIT_MCR_OUT2 | BIT_MCR_RTS );    /* 设置MCR寄存器的AFE和RTS为1 */
}

void UART_Idle()    /* 查询CH432 INT0引脚子程序 */
{
    unsigned char InterruptStatus;
    unsigned char ModemStatus;
    unsigned char RcvNum, s;
    while( 1 )
    {
		InterruptStatus=CH432_ReadData( CH432_IIR_PORT ) & ( ~ CH432_IIR_FIFOS_ENABLED );    /* 读串口1的中断状态 */
		if( ( InterruptStatus & 0x01 ) ) continue;    /* 没有中断退出 */
		else
		{
			switch( InterruptStatus )
			{
				case INT_MODEM_CHANGE:
					ModemStatus = CH432_ReadData( CH432_MSR_PORT );
					CH432_WriteData( CH432_THR_PORT, ModemStatus );
					if ( ModemStatus == 0x30)          /* Modem信号发送数据变化 */
					{
						UART_SendStr( SEND_STRING1 );
					}
					else if ( ModemStatus == 0x11 )    /* Modem 信号接收数据变化 */
					{
						UART_SendStr( SEND_STRING );
					}
					else if ( ModemStatus == 0x22 )    /* Modem 信号接收数据变化 */
					{
						s= CH432_ReadData( CH432_RBR_PORT );
						CH432_WriteData( CH432_THR_PORT, s );
					}
					break;
				case INT_NOINT:    /* 没有中断 */
					continue;
				case INT_THR_EMPTY:    /* 发送保持寄存器空中断 */
					CH432_WriteData( CH432_THR_PORT, 0xdd );
					break;
				case INT_RCV_SUCCESS:    /* 串口接收可用数据中断 */
					s=UART_RcvByte();
					UART_SendByte( s );
					break;
				case INT_RCV_LINES:        /* 接收线路状态中断 */
					CH432_ReadData( CH432_LSR_PORT);
					break;
				case INT_RCV_OVERTIME:    /* 接收数据超时中断 */
					RcvNum = CH432Seril0Rcv( buf );
					UART_SendByte( RcvNum );
					UART_Send( buf, RcvNum );
					break;
				default:    /* 不可能发生的中断 */
					break;
			}

			return;
		}
	}
}