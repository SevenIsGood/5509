/* ============================================================================
 * Copyright (c) 2008-2012 Texas Instruments Incorporated.
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/


#include "msp430f5529.h"

  unsigned int HPIC_read(void)
  {
  	unsigned int temp16b;

    //Set P1 and P2 as input
    P1DIR = 0x00;
    P2DIR = 0x00;
    P6OUT = 0x30; //P6.7(HBE0)=0; P6.6(HBE1)=0; P6.5(unused); P6.4(HDS1)=1; P6.3(HPI_HR_NW)=0; P6.2(HHWIL)=0; P6.1(unused); P6.0(MSP430_SPI_SEL)=0->SPI_flash selected

    PJOUT &= (0x3); //HCNTL0=0; HCNTL1=0
    P6OUT &= 0xFB;  //P6.2(HPI_HHWIL)=0;
    P6OUT |= 0x08;  //P6.3(HPI_HR_NW)=1;

    P6OUT &= 0xEF;  //set P6.4(HDS1)=0 to request to read
    while ((PJIN & 0x1) != 0x1); //when HPI_HRDY=1 this indicates HPI is ready; then continue
    P6OUT |= 0x10;  //set P6.4(HDS1)=1

    temp16b = (unsigned int)(P2IN<<8) + (unsigned int)P1IN; //latch read data
    return (temp16b);
  }

  long unsigned int HPIA_read(void)
  {
  	volatile long unsigned int temp32b=0;
    volatile unsigned char temp8b_1=0, temp8b_2=0;
    //Set P1 and P2 as input
    P1DIR = 0x00;
    P2DIR = 0x00;

  	//---L: read low-16bit HPIA
    PJOUT |= (0x4); //HCNTL1=1
    PJOUT &= (0x7); //HCNTL0=0
    P6OUT &= 0xFB;  //P6.2(HPI_HHWIL)=0; Low-16bit word
    P6OUT |= 0x08;  //P6.3(HPI_HR_NW)=1; Read operation

    P6OUT &= 0xEF;  //P6.4(HDS1)=0
    while ((PJIN & 0x1) != 0x1); //detect HPI_HRDY = 1? if it is 1 then continue
    P6OUT |= 0x10;  //Toggle HDS1 to latch the data, P6.4(HDS1)=1;

    temp8b_1 = P1IN;
    temp8b_2 = P2IN;

    temp32b = (((long unsigned int)temp8b_2)<<24) + (((long unsigned int)temp8b_1)<<16);

    //---H: read high-16bit HPIA
    PJOUT |= (0x4); //HCNTL1=1
    PJOUT &= (0x7); //HCNTL0=0
    P6OUT |= 0x04;  //P6.2(HPI_HHWIL)=1; high-16bit word
    P6OUT |= 0x08;  //P6.3(HPI_HR_NW)=1; read operation

    P6OUT &= 0xEF;  //P6.4(HDS1)=0
    while ((PJIN & 0x1) != 0x1); //detect HPI_HRDY = 1? if it is 1 then continue
    P6OUT |= 0x10;  //Toggle HDS1 to latch the data, P6.4(HDS1)=1;

    temp8b_1 = P1IN;
    temp8b_2 = P2IN;

    temp32b = temp32b + (((long unsigned int)temp8b_2)<<8) + (long unsigned int)temp8b_1;

    return(temp32b);
  }

  long unsigned int HPID_read_one(void)
  {
    volatile long unsigned int temp32b=0;
    volatile unsigned char temp8b_1=0, temp8b_2=0;

    //Set P1 and P2 as input
    P1DIR = 0x00;
    P2DIR = 0x00;

  	//---L: read low-16bit HPID
    PJOUT |= (0xC); //HCNTL0=1; HCNTL1=1
    P6OUT &= 0xFB;  //P6.2(HPI_HHWIL)=0; Low-16bit word
    P6OUT |= 0x08;  //P6.3(HPI_HR_NW)=1; Read operation

    P6OUT &= 0xEF;  //P6.4(HDS1)=0
    while ((PJIN & 0x1) != 0x1); //detect HPI_HRDY = 1? if it is 1 then continue
    P6OUT |= 0x10;  //Toggle HDS1 to latch the data, P6.4(HDS1)=1;

    temp8b_1 = P1IN;
    temp8b_2 = P2IN;

    temp32b = (((long unsigned int)temp8b_2)<<8) + (long unsigned int)temp8b_1;

    //---H: read high-16bit HPID
    PJOUT |= (0xC); //HCNTL0=1; HCNTL1=1
    P6OUT |= 0x04;  //P6.2(HPI_HHWIL)=1; high-16bit word
    P6OUT |= 0x08;  //P6.3(HPI_HR_NW)=1; read operation

    P6OUT &= 0xEF;  //P6.4(HDS1)=0
    ///while ((PJIN & 0x1) != 0x1); //detect HPI_HRDY = 1? if it is 1 then continue
    P6OUT |= 0x10;  //Toggle HDS1 to latch the data, P6.4(HDS1)=1;

    temp8b_1 = P1IN;
    temp8b_2 = P2IN;

    temp32b = (((long unsigned int)temp8b_2)<<24) + (((long unsigned int)temp8b_1)<<16) + temp32b;

    return(temp32b);

  }
  ///void HPID_read_auto(void);
  void HPID_read_auto(int trans_length, long unsigned int *HPID_buffer)
  {
    volatile unsigned char temp8b_1=0, temp8b_2=0;
    int i;
    //Set P1 and P2 as input
    P1DIR = 0x00;
    P2DIR = 0x00;

    //--read 32bit data from HPID registers continously
    for (i = 0; i < trans_length; i++){

  	  //---L: read low-16bit HPID
      PJOUT &= (0xB); //HCNTL1=0
      PJOUT |= (0x8); //HCNTL0=1

      P6OUT &= 0xFB;  //P6.2(HPI_HHWIL)=0; Low-16bit word
      P6OUT |= 0x08;  //P6.3(HPI_HR_NW)=1; Read operation

      P6OUT &= 0xEF;  //P6.4(HDS1)=0
      while ((PJIN & 0x1) != 0x1); //detect HPI_HRDY = 1? if it is 1 then continue
      P6OUT |= 0x10;  //Toggle HDS1 to latch the data, P6.4(HDS1)=1;

      temp8b_1 = P1IN;
      temp8b_2 = P2IN;

      HPID_buffer[i] = (((long unsigned int)temp8b_2)<<8) + (long unsigned int)temp8b_1;

      //---H: read high-16bit HPID
      P6OUT |= 0x04;  //P6.2(HPI_HHWIL)=1; high-16bit word

      P6OUT &= 0xEF;  //P6.4(HDS1)=0
      ///while ((PJIN & 0x1) != 0x1); //detect HPI_HRDY = 1? if it is 1 then continue
      P6OUT |= 0x10;  //Toggle HDS1 to latch the data, P6.4(HDS1)=1;

      temp8b_1 = P1IN;
      temp8b_2 = P2IN;

      HPID_buffer[i] = (((long unsigned int)temp8b_2)<<24) + (((long unsigned int)temp8b_1)<<16) + HPID_buffer[i];
    }
  }

  void HPIC_write(unsigned int HPIC_value_16b)
  {
    //Set P1 and P2 as output
    P1DIR = 0xFF; //Set P1.x to output direction
    P2DIR = 0xFF; //Set P2.x to output direction
    P6OUT = 0x30; //P6.7(HBE0)=0; P6.6(HBE1)=0; P6.5(unused); P6.4(HDS1)=1; P6.3(HPI_HR_NW)=0; P6.2(HHWIL)=0; P6.1(unused); P6.0(MSP430_SPI_SEL)=0->SPI_flash selected

    PJOUT &= (0x3); //HCNTL0=0; HCNTL1=0
    while ((PJIN & 0x1) != 0x1); //when HPI_HRDY=1 this indicates HPI is ready; then continue
    P6OUT &= 0xE3;  //P6.4(HDS1)=0; P6.3(HPI_HR_NW)=0; P6.2(HPI_HHWIL)=0;

      P1OUT = (char)HPIC_value_16b;      //HPIC data:0x81
      P2OUT = (char)(HPIC_value_16b>>8); //HPIC data

    P6OUT |= 0x10;  //Toggle HDS1 to latch the data, P6.4(HDS1)=1;
  }

  void HPIA_write(long unsigned int HPIA_value_32b)
  {
  	//Set P1 and P2 as output
    P1DIR = 0xFF; //Set P1.x to output direction
    P2DIR = 0xFF; //Set P2.x to output direction

  	//---L: Write to low-16bit HPIA
    PJOUT |= (0x4); //HCNTL1=1
    PJOUT &= (0x7); //HCNTL0=0
    P6OUT &= 0xFB;  //P6.2(HPI_HHWIL)=0; Low-16bit word
    P6OUT &= 0xF7;  //P6.3(HPI_HR_NW)=0; Write operation
    while ((PJIN & 0x1) != 0x1); //detect HPI_HRDY = 1? if it is 1 then continue
    P6OUT &= 0xEF;  //P6.4(HDS1)=0

      P1OUT = (char)(HPIA_value_32b>>16); //HPIA data [23:16]
      P2OUT = (char)(HPIA_value_32b>>24); //HPIA data [31:24]

      ///P1OUT = (char)HPIA_value_32b;      //HPIA data[7:0]
      ///P2OUT = (char)(HPIA_value_32b>>8); //HPIA data[15:8]

    while ((PJIN & 0x1) != 0x1); //detect HPI_HRDY = 1? if it is 1 then continue
    P6OUT |= 0x10;  //Toggle HDS1 to latch the data, P6.4(HDS1)=1;

    //---H: Write to high-16bit HPIA
    PJOUT |= (0x4); //HCNTL1=1
    PJOUT &= (0x7); //HCNTL0=0
    P6OUT |= 0x04;  //P6.2(HPI_HHWIL)=1; high-16bit word
    P6OUT &= 0xF7;  //P6.3(HPI_HR_NW)=0; write operation
    while ((PJIN & 0x1) != 0x1); //detect HPI_HRDY = 1? if it is 1 then continue
    P6OUT &= 0xEF;  //P6.4(HDS1)=0

      P1OUT = (char)HPIA_value_32b;      //HPIA data[7:0]
      P2OUT = (char)(HPIA_value_32b>>8); //HPIA data[15:8]

      ///P1OUT = (char)(HPIA_value_32b>>16); //HPIA data [23:16]
      ///P2OUT = (char)(HPIA_value_32b>>24); //HPIA data [31:24]

    while ((PJIN & 0x1) != 0x1); //detect HPI_HRDY = 1? if it is 1 then continue
    P6OUT |= 0x10;  //Toggle HDS1 to latch the data, P6.4(HDS1)=1;
  }

  void HPID_write_one(long unsigned int HPID_data)
  {
  	//Set P1 and P2 as output
    P1DIR = 0xFF; //Set P1.x to output direction
    P2DIR = 0xFF; //Set P2.x to output direction

  	//-----write a single 32bit data to HPID registers
    //---Write to Low-16bit HPID
    PJOUT |= (0xC); //HCNTL0=1; HCNTL1=1
    P6OUT &= 0xFB;  //P6.2(HPI_HHWIL)=0; Low-16bit word
    P6OUT &= 0xF7;  //P6.3(HPI_HR_NW)=0; write operation

    while ((PJIN & 0x1) != 0x1); //detect HPI_HRDY = 1? if it is 1 then continue

    P6OUT &= 0xEF;  //P6.4(HDS1)=0

    P1OUT = (char)HPID_data;      //HPID data[7:0]
    P2OUT = (char)(HPID_data>>8); //HPID data[15:8]

    P6OUT |= 0x10;  //Toggle HDS1 to latch the data, P6.4(HDS1)=1;

    //---Write to High-16bit HPID
    ///PJOUT |= (0xC);   //HCNTL0=1; HCNTL1=1
    P6OUT |= 0x04;     //P6.2(HPI_HHWIL)=1; high-16bit word
    ///P6OUT &= 0xF7;     //P6.3(HPI_HR_NW)=0; write operation

    P6OUT &= 0xEF;     //P6.4(HDS1)=0

    P1OUT = (char)(HPID_data>>16); //HPID data [23:16]
    P2OUT = (char)(HPID_data>>24); //HPID data [31:24]

    P6OUT |= 0x10;  //Toggle HDS1 to latch the data, P6.4(HDS1)=1;
  }

  void HPID_write_auto(int trans_length, long unsigned int *HPID_buffer)
  {
  	int i;

  	//Set P1 and P2 as output
    P1DIR = 0xFF; //Set P1.x to output direction
    P2DIR = 0xFF; //Set P2.x to output direction

    //set HPI as auto-incrementing mode in write operation
    PJOUT &= (0xB); //HCNTL1=0
    PJOUT |= (0x8); //HCNTL0=1
    P6OUT &= 0xF7;  //P6.3(HPI_HR_NW)=0; write operation

  	//--write 32bit data to HPID registers continously
    for (i = 0; i < trans_length; i++){

      //---Write to Low-16bit HPID
      ///PJOUT &= (0xB); //HCNTL1=0
      ///PJOUT |= (0x8); //HCNTL0=1
      P6OUT &= 0xFB;  //P6.2(HPI_HHWIL)=0; Low-16bit word
      ///P6OUT &= 0xF7;  //P6.3(HPI_HR_NW)=0; write operation

      while ((PJIN & 0x1) != 0x1); //detect HPI_HRDY = 1? if it is 1 then continue

      P6OUT &= 0xEF;  //P6.4(HDS1)=0

        P1OUT = (char)HPID_buffer[i];      //HPID data[7:0]
        P2OUT = (char)(HPID_buffer[i]>>8); //HPID data[15:8]

      P6OUT |= 0x10;  //Toggle HDS1 to latch the data, P6.4(HDS1)=1;

      //---Write to High-16bit HPID
      ///PJOUT &= (0xB);   //HCNTL1=0
      ///PJOUT |= (0x8);   //HCNTL0=1
      P6OUT |= 0x04;     //P6.2(HPI_HHWIL)=1; high-16bit word
      ///P6OUT &= 0xF7;     //P6.3(HPI_HR_NW)=0; write operation

      P6OUT &= 0xEF;     //P6.4(HDS1)=0

        P1OUT = (char)(HPID_buffer[i]>>16); //HPID data [23:16]
        P2OUT = (char)(HPID_buffer[i]>>24); //HPID data [31:24]

      P6OUT |= 0x10;  //Toggle HDS1 to latch the data, P6.4(HDS1)=1;
    }
  }
