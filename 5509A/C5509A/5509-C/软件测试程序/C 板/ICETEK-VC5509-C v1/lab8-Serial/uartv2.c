#define UART_BASE_ADDR			0x400200
#define RBR				*((int *)(UART_BASE_ADDR+0))
#define THR				*((int *)(UART_BASE_ADDR+0))	
#define IER				*((int *)(UART_BASE_ADDR+1))
#define IIR				*((int *)(UART_BASE_ADDR+2))	
#define FCR				*((int *)(UART_BASE_ADDR+2))	
#define LCR				*((int *)(UART_BASE_ADDR+3))
#define MCR				*((int *)(UART_BASE_ADDR+4))
#define LSR				*((int *)(UART_BASE_ADDR+5))
#define MSR				*((int *)(UART_BASE_ADDR+6))
#define SCR				*((int *)(UART_BASE_ADDR+7))
#define DLL				*((int *)(UART_BASE_ADDR+0))
#define DLM				*((int *)(UART_BASE_ADDR+1))
void TMCR_reset( void );
void CLK_init( void );
void EMIF_init(void);
void wait(int nWait);

char cString[17]={ "Hello PC!,Over|" },cReceive,cBuffer[17],cAnswer[16]={"Oh,you say"};
int bReceive,nLen;

main()
{
	unsigned int uWork;
	int i,k;

    TMCR_reset();
    CLK_init();
    EMIF_init();
    bReceive=0;
    LCR = 0x80; 
    DLL = 0x18;    
//    DLL = 0x03;    
	DLM = 0x00;  
    LCR = 0x03;    
    FCR = 0x01;
    MCR = 0x20;
    IER = 0x00;      
   while ( 1 )
	{
		if ( bReceive==0 )
		{
			for ( i=0;i<16;i++ )
			{
				do
				{ 
					uWork=LSR;
				} while ( uWork&0x040 != 0x040 );
				THR=cString[i];
				wait(256);		
			}
		}
		else
		{
			for ( i=0;i<10;i++ )
			{
				do
				{ 
					uWork=LSR;
				} while ( uWork&0x040 != 0x040 );
				THR=cAnswer[i];
				wait(256);		
			}
			do
			{ 
				uWork=LSR;
			} while ( uWork&0x040 != 0x040 );
			THR='\"';
			for ( i=0;i<nLen;i++ )
			{
				do
				{ 
					uWork=LSR;
				} while ( uWork&0x040 != 0x040 );
				THR=cBuffer[i];
				wait(256);		
			}
			do
			{ 
				uWork=LSR;
			} while ( uWork&0x040 != 0x040 );
			THR='\"';
			wait(1024);		
			for ( i=9;i<16;i++ )
			{
				do
				{ 
					uWork=LSR;
				} while ( uWork&0x040 != 0x040 );
				THR=cString[i];
				wait(256);		
			}
		}
		k=0; bReceive=0;
		while ( 1 )
		{
			do
			{
				uWork=LSR;
			} while ( (uWork&1)==0 );
			cReceive=RBR;
			cBuffer[k]=cReceive&0x0ff; 
			if ( cReceive=='.' )
			{
				cBuffer[k+1]='\0';
				nLen=k+1;
				bReceive=1;
				break;
			}
			k++; k%=16;
		}
	} 
//    for(;;)
//    {  
//    	while(LSR & 0x40 == 0);
//	    	THR = 0X55;
//    }        
    
} 
void EMIF_init(void)
{
    
    ioport unsigned int *ce21  =(unsigned int *)0x809;
    *ce21   = 0x1fff;
}
void CLK_init( void )
{
   ioport unsigned int *clkmd;
   clkmd=(unsigned int *)0x1c00;
   *clkmd = 0x2033;
}
void TMCR_reset( void )
{
    ioport unsigned int *TMCR_MGS3=(unsigned int *)0x07FE;   
    ioport unsigned int *TMCR_MM =(unsigned int *)0x07FF;   
    *TMCR_MGS3 =0x510;
    *TMCR_MM   =0x000;


}
void wait(int nWait)
{
	int i,j,k=0;
	for ( i=0;i<nWait;i++ )
		for ( j=0;j<64;j++ )
			k++;
}
