/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_i2c1.c---------
 * This example is desinged to show how to use the I2C   
 * module's interrupt dispatch function. Because there   
 * are several possible events that could trigger a HW   
 * interrupt to the CPU for I2C, a dispatcher was written
 * to enable read of the I2C status to determine which   
 * event/s triggered the HW interrupt and to call a User 
 * defined function to service any masked I2C event that 
 * needs acknowledgement. This example shows how to build
 * an ISR call back table for use with the I2C dispatcher
 */
#include <csl.h>
#include <csl_i2c.h>
#include <stdio.h>
#include "myIsrs_i2c1.h"

//---------Global data definition---------
Uint16 mask = 1;    
Uint16 enable = 1;    
Uint16 databyte1[2]={0xA,0xB};  
int i=0,x;         

/* Declare and Initialize an I2C call Back Structure */
I2C_IsrAddr addr = {  
        myALIsr,
        myNACKIsr,
        myARDYIsr,
        myRRDYIsr,
        myXRDYIsr
};       

/* Declare and Initialize an I2C initialization structure */
I2C_Setup Init = {

        0,              /* 7 bit address mode */
        0x0020,         /* own address - don't care if master */
        144,            /* clkout value (Mhz)  */
        400,            /* a number between 10 and 400*/
        0,              /* number of bits/byte to be received or transmitted (8)*/
        0,              /* DLB mode on*/
        1               /* FREE mode of operation on*/

};

//---------Function prototypes---------

/* Reference start of interrupt vector table       */
/* This symbol is defined in the file, vectors.s55 */
extern void VECSTART(void);

#pragma CODE_SECTION (taskFunc, "tskSeg");
void taskFunc(void);

//---------main routine---------
void main(void)
{
    /* Initialize CSL library - This is REQUIRED !!! */
    CSL_init();
    
    /* Set IVPD/IVPH to start of interrupt vector table */
    IRQ_setVecs((Uint32)(&VECSTART));
    
    /* Call I2C example task/function */
    taskFunc();
}

void taskFunc(void) {

    /* Initialize the I2C using the iniitalization structure values */
    I2C_setup(&Init); 

    /* Set call back functions for I2C interrupt events */
    I2C_setCallback(&addr);             // hook up the interrupt functions
    
    /* Enable RRDY interrupt */
    I2C_eventEnable(I2C_EVT_RRDY);  
 
    /* Enable all maskable interrupts */
    IRQ_globalEnable();                 // Enable interrupts
    
    /* Write a data byte to I2C */
    x=I2C_write(databyte1,1,1,0x20,1,30000);
   
    if(!x) printf ("\nTEST PASSED\n");
    else printf ("\nTEST FAILED\n");
      
   /* Give some time for interrupt to occur */                                                                   
   for(i=0;i<10000;i++){ 
   };   
}    

