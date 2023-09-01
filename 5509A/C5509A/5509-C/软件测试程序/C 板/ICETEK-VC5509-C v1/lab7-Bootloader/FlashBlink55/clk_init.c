#include"myapp.h"


void CLK_init( void )
{
   ioport unsigned int *clkmd;
   clkmd=(unsigned int *)0x1c00;
   *clkmd =0x2033;// 0x2033;//0x2413;// 144MHz //2613
}
void EMIF_init(void)
{
    /*ioport unsigned int *CLKMD = (unsigned int *)0x1c00; 
    ioport unsigned int *EBSR  = (unsigned int *)0x6c00;
    ioport unsigned int *EGCR  = (unsigned int *)0x800;
    ioport unsigned int *EMIRST= (unsigned int *)0x801;
    ioport unsigned int *EMIBE = (unsigned int *)0x802; 
    ioport unsigned int *CE01  = (unsigned int *)0x803;
    ioport unsigned int *CE02  = (unsigned int *)0x804;
    ioport unsigned int *CE03  = (unsigned int *)0x805;
    ioport unsigned int *CE11  = (unsigned int *)0x806;
    ioport unsigned int *CE12  = (unsigned int *)0x807;
    ioport unsigned int *CE13  = (unsigned int *)0x808;
    ioport unsigned int *CE21  = (unsigned int *)0x809;
    
    ioport unsigned int *CE22  = (unsigned int *)0x80A;
    ioport unsigned int *CE23  = (unsigned int *)0x80B;
    ioport unsigned int *CE31  = (unsigned int *)0x80C;
    ioport unsigned int *CE32  = (unsigned int *)0x80D;
    ioport unsigned int *CE33  = (unsigned int *)0x80E;
    ioport unsigned int *SDC1  = (unsigned int *)0x80F;
    ioport unsigned int *SDPER = (unsigned int *)0x810;
    ioport unsigned int *SDCNT = (unsigned int *)0x811;
    ioport unsigned int *INIT  = (unsigned int *)0x812;
    ioport unsigned int *SDC2  = (unsigned int *)0x813;*/
    ioport unsigned int *CE21  = (unsigned int *)0x809;
   /* *EBSR = 0xa01;
    *EGCR = 0x220;
    *EGCR = 0X220;
    *CE01 =0X3000;
    *CE11 = 0X1fff;
    *CE21 =0x1fff;
    *EMIRST = 0;
    *SDC1 =0X5958;
    *SDC2=0X38F;
    *INIT = 0;*/
    
    *CE21 =0x1fff;
        
                
}
void TMCR_reset( void )
{
    ioport unsigned int *TMCR_MGS3=(unsigned int *)0x07FE;   
    ioport unsigned int *TMCR_MM =(unsigned int *)0x07FF;   
    *TMCR_MGS3 =0x510;
    *TMCR_MM   =0x000;


}
