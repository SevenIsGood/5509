/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_i2c2.c---------
 * This example shows how to use the I2C API to transfer a byte of 
 * data in loopback mode.                                    
 */
 
#include <csl_i2c.h>
#include <stdio.h>

//---------Global data definition---------
int x,y,z;
Uint16 databyte1[1]={0xA};
Uint16 datareceive1[1]={0};

/* Create and initialize an I2C initialization structure */
I2C_Setup Init = {
        0,              /* 7 bit address mode */
        0x0020,         /* own address - don't care if master */
        144,            /* clkout value (Mhz)  */
        400,            /* a number between 10 and 400*/
        0,              /* number of bits/byte to be received or transmitted (8)*/
        1,              /* DLB mode on*/
        1               /* FREE mode of operation on*/
};


//---------Function prototypes---------
void taskFunc(void);

//---------main routine---------
void main(void)
{
    /* Initialize CSL library - This is REQUIRED !!! */
    CSL_init();

    /* Call I2C example task/func */
    taskFunc();
}

void taskFunc(void) {
 
    int err = 1;
    /* Initialize I2C, using parameters in init structure */
    I2C_setup(&Init); 
    
    /* Write a data byte to I2C */
    x=I2C_write(databyte1,1,1,0x20,1,30000);
    
    /* Read data byte from I2C */
    z=I2C_read(datareceive1,1,1,0x20,1,30000,0);
    
    /* Make sure the received byte is same as one sent */
    if (databyte1[0]==datareceive1[0]) {
       err = 0;
    }
    printf ("%s\n",err?"TEST FAILED" : "TEST PASSED");
}
