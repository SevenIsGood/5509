#include "5509.h"
void CLK_init( void );
void main()
{
    CLK_init();
    AIC23_Init();
    for(;;)
        AIC23_Mixer();

}
void CLK_init( void )
{
   ioport unsigned int *clkmd;
   clkmd=(unsigned int *)0x1c00;
   *clkmd = 0x2413;// 144MHz //2613
}
