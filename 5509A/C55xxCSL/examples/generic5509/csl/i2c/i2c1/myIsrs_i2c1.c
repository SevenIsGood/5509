/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------myIsrs_i2c1.c---------
 * Function definitions for ISR/callBack functions
 */
#include <stdio.h>    
#pragma CODE_SECTION (myALIsr,"myisrSeg");
#pragma CODE_SECTION (myNACKIsr,"myisrSeg");
#pragma CODE_SECTION (myARDYIsr,"myisrSeg");
#pragma CODE_SECTION (myRRDYIsr,"myisrSeg");

void myALIsr()
{
   printf("I2C Arbitration Interrupt Occurred\n");
   asm("\tNOP                       ;====> I2C periodic interrupt routine");   
 
}   

void myNACKIsr()
{
   printf("I2C  NACK Interrupt Occurred\n");
   asm("\tNOP                       ;====> I2C periodic interrupt routine");   
 
}

void myARDYIsr()
{
   printf("I2C ARDY Interrupt Occurred\n");
   asm("\tNOP                       ;====> I2C periodic interrupt routine");   
 
}

void myRRDYIsr()
{
   printf("I2C RRDY Interrupt Occurred\n");
   asm("\tNOP                       ;====> I2C periodic interrupt routine");   
 
}

void myXRDYIsr()
{
   printf("I2C XRDY Interrupt Occurred\n");
   asm("\tNOP                       ;====> I2C periodic interrupt routine");   
 
}


