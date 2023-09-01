#include "5509.h"


void initIIc()
{
	PC55XX_I2C pI2c=(PC55XX_I2C)C55XX_I2C_ADDR;
	pI2c->icmdr=0;
	delay(100);
	pI2c->icsar=0x001a;
	pI2c->icmdr=0x0620;
	pI2c->iccnt=0x0000;
	pI2c->icpsc=0x0007;
	pI2c->icclkl=0x000f;
	pI2c->icclkh=0x000f;
	pI2c->icmdr=pI2c->icmdr|0x4000;
	pI2c->icclkh=0x0064;
	pI2c->icclkl=0x0064;
}
void disableIIc()
{
    PC55XX_I2C pI2C = (PC55XX_I2C)C55XX_I2C_ADDR;

    // Put I2C controller in reset (bad if clock derivatives change out of reset)
    ClearMask(pI2C -> icmdr, ICMDR_IRS);    
}
void writeIIc(unsigned short deviceAddr,unsigned char* dataBuff,unsigned short len)
{
	unsigned short i;
	PC55XX_I2C pI2c=(PC55XX_I2C)C55XX_I2C_ADDR;
	pI2c->iccnt=len;
 	WriteMask(pI2c -> icsar, deviceAddr, ICSAR_MASK_7);
	WriteMask(pI2c -> icmdr,
        ICMDR_STT | ICMDR_STP | ICMDR_TRX,
        ICMDR_STT | ICMDR_STP | ICMDR_TRX);		       
    // Transmit data
    for (i = 0; i < len ; i++) 
    {
        pI2c -> icdxr=dataBuff[i];
        while(!(pI2c -> icstr & ICSTR_ICXRDY));
    }
}
void readIIC(unsigned short int deviceAddr, unsigned char* dataBuff,unsigned short len)
{
    PC55XX_I2C pI2C = (PC55XX_I2C)C55XX_I2C_ADDR;
    int i;
       
    // Set the I2C controller to read a stream of count bytes
   	pI2C -> iccnt=len;
    WriteMask(pI2C -> icsar, deviceAddr, ICSAR_MASK_7);
    WriteMask(pI2C -> icmdr,
        ICMDR_STT | ICMDR_STP,
        ICMDR_STT | ICMDR_STP | ICMDR_TRX);
    
    // Receive the data
    for (i = 0; i < len; i++)
    {
        // Wait for receive data to come back    
        while(!(pI2C -> icstr & ICSTR_ICRRDY));

        // Copy the data out
        dataBuff[i] = pI2C -> icdrr;
    }
}
