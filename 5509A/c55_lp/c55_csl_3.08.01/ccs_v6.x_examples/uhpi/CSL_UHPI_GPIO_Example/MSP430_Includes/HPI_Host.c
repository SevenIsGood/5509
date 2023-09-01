/***************************************************************************************************
                   							 HPI_Host.c
****************************************************************************************************/

#include <msp430f5529.h>
#include "HPI_Host.h"

#define C5517_UHPI_OUTPUT_TEST
//#define C5517_UHPI_INPUT_TEST

void Initialize_HPI_Pins(void)
{

  // Declaring the pin states and direction
  P6DIR |= 0xDC;  	// P6.2-hhwil, P6.3 DIR set as output for HPI(write),P6.4-HDS1 as output , HBE0,HBE1 as out put
  PJDIR = 0x0;      // PJ.1 set as input (HINT) (pin 73)
  PJDIR |= 0xC;     // PJ.2(HCNTL1) PJ.3(HCNTL0) set as output
  P6OUT&=(~0xC0);   // HBE0,1 = 0

  //Initializing port pin and signals
  Config_HPI(WRITE,HPIA);

}

void Config_HPI(unsigned char RW_Dir,unsigned char REG_Type)
{
	 switch(RW_Dir)
	 {
	  case READ: 
	             P1DIR = INPUT_PORT;		 // P1 = Input port(D0-D7)
			  	 P2DIR = INPUT_PORT;		 // P2 = Input port(D8-D15)
	  			 HRW_HIGH;    	 	 		 // HR/W = 1 for Read. 
	   			 break;
	  case WRITE:
	  			 P1DIR = OUTPUT_PORT;		 // P1 = output port(D0-D7)
	  		     P2DIR = OUTPUT_PORT;		 // P2 = output port(D8-D15)
	  			 HRW_LOW;      		 		 // HR/W = 0 for write. 
	  			 break;
	 }
	 switch(REG_Type)
	 {
	  case HPIC: 
	  			 HCNTL1_LOW; 		// HCNTL1=0
  				 HCNTL0_LOW; 		// HCNTL0=0 
	   			 break;
	  case HPIA: 
	  			 HCNTL1_HIGH; 		//HCNTL1=1
  				 HCNTL0_LOW; 		//HCNTL0=0  
	  			 break;
	  case HPID_A: //Host has R/W access to the HPID register with auto-increment.
	  			 HCNTL1_LOW;		//HCNTL1=0
  				 HCNTL0_HIGH; 		//HCNTL0=1 
	  			 break;	
	  case HPID_M: //Host has R/W access to the HPID register without auto-increment.
	  			 HCNTL1_HIGH;		//HCNTL1=0
  				 HCNTL0_HIGH; 		//HCNTL0=1 
	  			 break;			 
	 }
	 HDS_HIGH;      			    // HDS1 = 1 
 
}
void HPI_Write_Address(unsigned long int ad_value)
{
  int i=0,loop_cnt=0;	
  unsigned char flag =0;
  
  Address_Divide Start_Address;
  
  Config_HPI(WRITE,HPIA);
  Start_Address.byte_addr = HPI_HOST_SARAM_ADR(ad_value);
  
  do
  {
  	if(0 == flag)
  	{
  		HHWIL_LOW; 			 	//HHWIL=0 
  		P1OUT = Start_Address.byte_addr_buffer[2];
  		P2OUT = Start_Address.byte_addr_buffer[3];
  		flag=1;
  	}
  	else
  	{ 
  		HHWIL_HIGH; 			//HHWIL=1
  		P1OUT = Start_Address.byte_addr_buffer[0];
  		P2OUT = Start_Address.byte_addr_buffer[1]; 
  		flag=0;
  	}
  	while((PJIN&0x1)==0x0);  	// Wait for HRDY to go HIGH	
  	HDS_LOW; 					// HDS1 = 0 
  	for(i=0;i<50;i++);
  	HDS_HIGH;    				// HDS1 = 1   	
	//while((PJIN&0x1)==0x0);   // Wait for HRDY to go HIGH	
  	loop_cnt++;
  }while(loop_cnt<2);
}

unsigned long int HPI_Read_Address(void)
{
  unsigned short flag=0,data=0,loop_cnt=0;
  unsigned int i=0;
  unsigned long int Address=0; 
  
  //HPIA read
  Config_HPI(READ,HPIA);
  do
  {
  	if(0 == flag)
  	{
  		HHWIL_LOW; 			 //HHWIL=0   		
  	}
  	else
  	{ 
  		HHWIL_HIGH; 		 //HHWIL=1  		
  	}
  	while((PJIN&0x1)==0x0);  // Wait for HRDY to go HIGH	
  	HDS_LOW; // HDS1 = 0 
  	for(i=0;i<50;i++);
  	HDS_HIGH;    // HDS1 = 1
  	data = P1IN|(P2IN<<8);    
  	if(0 == flag)
  	{
  		Address = data;
		Address<<=16;		// shift to higher 16 bits
		flag=1;
  	}
  	else
  	{
  		Address|=data;   	// or with lower byte
  		flag=0;
  	}		
  	loop_cnt++;
  }while(loop_cnt<2);

  for(i=0;i<50;i++);   
  Config_HPI(WRITE,HPIA); 	//Reset to HPIA Write
 
  return(Address);  		// Return Address
}

void HPI_WRITE_DATA(unsigned int *Write_Data,unsigned int Buffer_size,unsigned char inc)
{
 unsigned int i=0,looper1=0;
 unsigned char flag=0; 
 	
  //HPID Write  
  if(0 == inc)
  	Config_HPI(WRITE,HPID_M);
  else
    Config_HPI(WRITE,HPID_A);	 
  
  flag=0;
  for(looper1=0;looper1<Buffer_size;looper1++)
  {
	  //wait till HRDY is going high
	  while((PJIN&0x1)!=0x1);                  // continuous loop while HRDY=0
	  
	  if(0==flag)
	  {
		  HHWIL_LOW; 		//HHWIL=0  
		  flag=1;
	  }
	  else
	  {
		  HHWIL_HIGH; 		//HHWIL=1 
		  flag=0;
	  }
	  P1OUT = (unsigned char)(Write_Data[looper1]&0x00FF);
	  P2OUT = (unsigned char)((Write_Data[looper1]&0xFF00)>>8);
	  for(i=0;i<50;i++); 
	  HDS_LOW; 				// HDS1 = 0
	  for(i=0;i<50;i++); 
	  HDS_HIGH;    			// HDS1 = 1 
	  for(i=0;i<50;i++);   
  
  }
}

void HPID_READ_DATA(unsigned int *Read_Data,unsigned int Buffer_size,unsigned char inc)
{
  unsigned int i=0,looper1=0;
  unsigned char flag=0; 
 	
  //HPID Read  
  if(0 == inc)
  	//Config_HPI(WRITE,HPID_M);
	  Config_HPI(READ,HPID_M);
  else
    //Config_HPI(WRITE,HPID_A);
	  Config_HPI(READ,HPID_A);
  flag=0;
  // initialize data
  for(looper1=0;looper1<Buffer_size;looper1++)
  {
  	Read_Data[looper1]=0;
  }  
  for(looper1=0;looper1<Buffer_size;looper1++)
  {
	  if(0==flag)
	  {
	  	HHWIL_LOW; 				//HHWIL=0
		  //HHWIL_HIGH; 			//HHWIL=1
	  	flag=1;
	  }
	  else
	  {
	  	HHWIL_HIGH; 			//HHWIL=1
	  	//HHWIL_LOW; 				//HHWIL=0
	    flag=0;
	  }
	  for(i=0;i<500;i++);   
	  HDS_LOW; 					// HDS1 = 0
	  for(i=0;i<50;i++);
	  HDS_HIGH;    				// HDS1 = 1 
	  Read_Data[looper1] = P1IN|(P2IN<<8);
	  for(i=0;i<50;i++);  
   
  }
  for(i=0;i<50;i++);  
}
