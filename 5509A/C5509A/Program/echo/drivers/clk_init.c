void CLK_init()
{
   ioport unsigned int *clkmd;
   clkmd=(unsigned int *)0x1c00;
   *clkmd =0x21f3;	// ¾§Õñ12Hz,9Hz=0x21f3;// 144MHz=0x2613
}

void SetDSPPLL(unsigned int uPLL)
{
   ioport unsigned int *clkmd;
   clkmd=(unsigned int *)0x1c00;
   *clkmd =uPLL;
}

void TMCR_reset( void )
{
    ioport unsigned int *TMCR_MGS3=(unsigned int *)0x07FE;   
    ioport unsigned int *TMCR_MM =(unsigned int *)0x07FF;   
    *TMCR_MGS3 =0x510;
    *TMCR_MM   =0x000;


}
