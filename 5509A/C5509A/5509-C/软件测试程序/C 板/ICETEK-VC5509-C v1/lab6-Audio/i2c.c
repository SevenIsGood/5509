#include "5509.h"
#include "util.h"
DSPCLK dspclk;
I2C_Init()
{
    PC55XX_I2C pI2C = (PC55XX_I2C)C55XX_I2C_ADDR;

    // Put I2C controller in reset (bad if clock derivatives change out of reset)
    ClearMask(pI2C -> icmdr, ICMDR_IRS);
    
    // Set prescaler to generate 12MHz clock
    pI2C -> icpsc = dspclk.pllmult;
       
    // Setup clock control registers (100KHz clock out)
    Write(pI2C -> icclkl, 7);  // For 400KHz, use 47 for 100KHz
    Write(pI2C -> icclkh, 7);
        
    // Setup master and slave addresses
    WriteMask(pI2C -> icoar, ICOAR_OADDR, ICOAR_MASK_7);

    // Take I2C controller out of reset, put in master mode
    SetMask(pI2C -> icmdr, ICMDR_IRS | ICMDR_MST);
}

I2C_Disable()
{
    PC55XX_I2C pI2C = (PC55XX_I2C)C55XX_I2C_ADDR;

    // Put I2C controller in reset (bad if clock derivatives change out of reset)
    ClearMask(pI2C -> icmdr, ICMDR_IRS);    
}

void I2C_Write(unsigned short int device, int count, unsigned char *bytedata)
{
    PC55XX_I2C pI2C = (PC55XX_I2C)C55XX_I2C_ADDR;
    int i;
        
    // Set the I2C controller to write a stream of count bytes
    Write(pI2C -> iccnt, count);
    WriteMask(pI2C -> icsar, device, ICSAR_MASK_7);
    WriteMask(pI2C -> icmdr,
        ICMDR_STT | ICMDR_STP | ICMDR_TRX,
        ICMDR_STT | ICMDR_STP | ICMDR_TRX);
        
    // Transmit data
    for (i = 0; i < count ; i++) {
        Write(pI2C -> icdxr, bytedata[i]);
        while(!(pI2C -> icstr & ICSTR_ICXRDY));
    }
}

void I2C_Read(unsigned short int device, int count, unsigned char *bytedata)
{
    PC55XX_I2C pI2C = (PC55XX_I2C)C55XX_I2C_ADDR;
    int i;
       
    // Set the I2C controller to read a stream of count bytes
    Write(pI2C -> iccnt, count);
    WriteMask(pI2C -> icsar, device, ICSAR_MASK_7);
    WriteMask(pI2C -> icmdr,
        ICMDR_STT | ICMDR_STP,
        ICMDR_STT | ICMDR_STP | ICMDR_TRX);
    
    // Receive the data
    for (i = 0; i < count; i++)
    {
        // Wait for receive data to come back    
        while(!(pI2C -> icstr & ICSTR_ICRRDY));

        // Copy the data out
        bytedata[i] = pI2C -> icdrr;
    }
}

