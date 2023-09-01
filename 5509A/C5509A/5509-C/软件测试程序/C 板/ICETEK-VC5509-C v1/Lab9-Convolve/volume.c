/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DSP/BIOS 4.90.270 06-11-03 (barracuda-m10)" */
/***************************************************************************/
/*                                                                         */
/*     V O L U M E . C                                                     */
/*                                                                         */
/*     Audio gain processing in a main loop                                */
/*                                                                         */
/***************************************************************************/

#include <stdio.h>

#include "volume.h"

/* Global declarations */
int inp1_buffer[BUFSIZE];
int inp2_buffer[BUFSIZE];       /* processing data buffers */
int out1_buffer[BUFSIZE];
int out2_buffer[BUFSIZE];
int out3_buffer[BUFSIZE];
int out4_buffer[BUFSIZE*2];
int size = BUFSIZE;
int ain = MINGAIN;
int zhy=0;
int sk=64;        /*sk代表所开的bufsize的大小,需修改它.输入文件sine.dat为32点,sine11.dat,
                    sin22.dat,sin33.dat,sin44.dat为64点的输入波形.*/
                  /* volume control variable */

int gain = MINGAIN;                      /* volume control variable */
unsigned int processingLoad = BASELOAD;  /* processing routine load value */

static int processing1(int *output1, int *output2);
static int processing2(int *output2, int *output3); 
static int processing3(int *input1,int *output2,int *output4);
static int processing4(int *input2, int *output1);
static void dataIO1(void);
static void dataIO2(void);
struct PARMS str =
{
    2934,
    9432,
    213,
    9432,
    &str
};

/* Functions */
extern void load(unsigned int loadValue);

/*
 * ======== main ========
 */
void main()
{
   int *input1 = &inp1_buffer[0];
    int *input2 = &inp2_buffer[0];
    int *output1 = &out1_buffer[0];
    int *output2 = &out2_buffer[0];
    int *output3 = &out3_buffer[0];
    int *output4 = &out4_buffer[0];
    puts("volume example started\n");

    /* loop forever */
    while(TRUE)
    {       
        /* 
         *  Read input data using a probe-point connected to a host file. 
         *  Write output data to a graph connected through a probe-point.
         */
        dataIO1();
        dataIO2();
        /* apply gain */ 
        processing4(input2,output1);
        processing1(output1, output2); 
        processing2(output2, output3);
        processing3(input1,output2,output4) ;
    }
}

/*
 *  ======== processing ========
 *
 * FUNCTION: apply signal processing transform to input signal.
 *
 * PARAMETERS: address of input and output buffers.
 *
 * RETURN VALUE: TRUE.
 */
static int processing4(int *input2,int *output1)
{   int m=sk;
    for(;m>=0;m--)
   {
        *output1++ = *input2++ * ain;
    }
   for(;(size-m)>0;m++)
   {output1[m]=0;
   }
    load(processingLoad); 
    return(TRUE);
    
    }
 
static int processing1(int *output1,int *output2)
{  
    int m=sk-1;  
   for(;m>0;m--)
   {
        *output2++ = *output1++ * ain;
    }
   
    /* additional processing load */
    load(processingLoad); 
    return(TRUE);
}
static int processing2(int *output2, int *output3)
{   int n=zhy;
   
    size=BUFSIZE; 
    for(;(size-n)>0;n++)
    { *output3++ = output2[n];
    }
    /* for (;n>0;n--)
    { *output3++ = 0;
    }  */
    load(processingLoad);
    return(TRUE);
    }
static int processing3(int *input1,int *output2,int *output4)
{   int m=sk;
    int y=zhy;
    int z,x,w,i,f,g;
    for(;(m-y)>0;)
    {i=y;
     x=0;
     z=0;
     f=y;
     for(;i>=0;i--)
     {g=input1[z]*output2[f];
      x=x+g;
     z++;
     f--;
     }
     *output4++ = x;
     y++;
     }
     m=sk;
     y=sk-1;
     w=m-zhy-1;
    for(;m>0;m--)
    {
     y--;
     i=y;
     z=sk-1;
     x=0;
     f=sk-y;
     for(;i>0;i--,z--,f++)
     {g=input1[z]*output2[f];
     x=x+g;
     
     }
     out4_buffer[w]=x;
     w++;
     }
     load(processingLoad);
     return(TRUE);
     }  
/*
 *  ======== dataIO ========
 *
 * FUNCTION: read input signal and write processed output signal.
 *
 * PARAMETERS: none.
 *
 * RETURN VALUE: none.
 */
static void dataIO1()
{
    /* do data I/O */

    return;
}
static void dataIO2()
{
    /* do data I/O */

    return;
}
