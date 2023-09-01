#include "5509.h"
void emifInit( void )
{      
    ioport unsigned int *ebsr  =(unsigned int *)0x6c00;
    PC55XX_EMIF pC55XX_EMIF  =(PC55XX_EMIF)0x800;
    *ebsr   = 0x0a01;
    pC55XX_EMIF->egcr  	= 0x221;
    pC55XX_EMIF->egcr   = 0x221;

    pC55XX_EMIF->ce0_1  = 0x3FFF;
	pC55XX_EMIF->ce0_2	= 0xFFFF;
	pC55XX_EMIF->ce0_3	= 0x00FF;

 	pC55XX_EMIF->ce1_1  = 0x1FFF;
	pC55XX_EMIF->ce1_2	= 0xFFFF;
	pC55XX_EMIF->ce1_3	= 0x00FF;

	pC55XX_EMIF->ce2_1  = 0x1FFF;
	pC55XX_EMIF->ce2_2	= 0xFFFF;
	pC55XX_EMIF->ce2_3	= 0x00FF;

	pC55XX_EMIF->ce3_1  = 0x1FFF;
	pC55XX_EMIF->ce3_2	= 0xFFFF;
	pC55XX_EMIF->ce3_3	= 0x00FF;

    pC55XX_EMIF->emi_rst= 0xFFFF;

    pC55XX_EMIF->sdc1   = 0x2911;
	pC55XX_EMIF->sdper	= 0x0410;
    pC55XX_EMIF->sdc2   = 0x0131;
    pC55XX_EMIF->sdinit = 0x07FF;
}     
