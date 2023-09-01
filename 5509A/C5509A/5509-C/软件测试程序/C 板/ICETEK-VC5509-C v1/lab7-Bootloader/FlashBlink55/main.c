#include "myapp.h"
int i;
main()
{
  i=0;
  CLK_init();
  EMIF_init();
  INTR_init();
  TIME_init();
  for(;;);
}

void interrupt tint( void )
{
 i++;
 if(i<30)
 {
    LED= 0x55;
 }
 if( (i>30)&&( i<60))
 {
    LED =0x55; 
 }
 if(i == 60 ) i=0;
  

}
void INTR_init( void )
{
	IVPD=0xd0;
	IVPH=0xd0;
	IER0=0x10;
	DBIER0 =0x10;
	IFR0=0xffff;
	asm(" BCLR INTM");

}
void TIME_init(void)
{
    ioport unsigned int *tim0; 
    ioport unsigned int *prd0; 
    ioport unsigned int *tcr0; 
    ioport unsigned int *prsc0;  
	tim0  =  (unsigned int *)0x1000;
	prd0  =  (unsigned int *)0x1001;
	tcr0  =  (unsigned int *)0x1002;
	prsc0 =  (unsigned int *)0x1003;
   *tcr0 = 0x04f0;
   *tim0 = 0;
   *prd0 = 0xffff;
   *prsc0 = 2;
   *tcr0 = 0x00e0;


}
