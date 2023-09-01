#include <msp430f5529.h>
#include <HPI_Host.h>
#include <stdio.h>
#include "SetVcoreUp.h"

#define BUFFER_SIZE 4

//#define C5517_UHPI_OUTPUT_TEST
#define C5517_UHPI_INPUT_TEST

volatile unsigned int Status_Buffer[BUFFER_SIZE]={0,0,0,0};
//unsigned int P6_Status;
void Init_Port_pins(void);

void main(void)
{
  unsigned int i=0;
  
  WDTCTL = WDTPW+WDTHOLD;             // Stop WDT
  
  //Clock configurations

 //P1DIR |= BIT0;                            // ACLK set out to pins
 //P1SEL |= BIT0;                          
 //P2DIR |= BIT2;                            // SMCLK set out to pins
 //P2SEL |= BIT2;                            
// P7DIR |= BIT7;                            // MCLK set out to pins
// P7SEL |= BIT7;          
 
  //P5SEL |= BIT4+BIT5;                       // Port select XT1
  //P5SEL |= BIT2+BIT3;                       // Port select XT2
 /// P2DIR |= BIT3;                            // SMCLK set out to pins
  //P2SEL |= BIT3;                        
  
  /******* from the Post-Si test code ******
  P7DIR |= BIT7;                            // MCLK set out to pins
    P7SEL |= BIT7;

    */// Increase Vcore setting to level3 to support fsystem=25MHz
    // NOTE: Change core voltage one level at a time..
    SetVcoreUp (0x01);
    SetVcoreUp (0x02);
    SetVcoreUp (0x03);
    /******* from the Post-Si test code ******/


  UCSCTL3 |= SELREF_2;                      // Set DCO FLL reference = REFO
    /******* from the Post-Si test code ******/
    //UCSCTL3 = SELREF_2;


  UCSCTL4 |= SELA_2;                        // Set ACLK = REFO
  
  __bis_SR_register(SCG0);                  // Disable the FLL control loop
  UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
  //UCSCTL1 = DCORSEL_5;                      // Select DCO range 24MHz operation
  //UCSCTL2 = FLLD_1 + 374;                   // Set DCO Multiplier for 12MHz
                                            // (N + 1) * FLLRef = Fdco
                                            // (374 + 1) * 32768 = 12MHz
  	  	  	  	  	  	  	  	  	  	  	  // Set FLL Div = fDCOCLK/2

  /******* from the Post-Si test code ******/
  UCSCTL1 = DCORSEL_7;                      // Select DCO range 50MHz operation
  UCSCTL2 = FLLD_1 + 762;                   // Set DCO Multiplier for 25MHz
                                            // (N + 1) * FLLRef = Fdco
                                            // (762 + 1) * 32768 = 25MHz
                                            // Set FLL Div = fDCOCLK/2

  __bic_SR_register(SCG0);                  // Enable the FLL control loop

  // Worst-case settling time for the DCO when the DCO range bits have been
  // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
  // UG for optimization.
  // 32 x 32 x 12 MHz / 32,768 Hz = 375000 = MCLK cycles for DCO to settle


  //__delay_cycles(375000);
  /******* from the Post-Si test code ******/
  __delay_cycles(782000);

	
  // Loop until XT1,XT2 & DCO fault flag is cleared
  do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
                                            // Clear XT2,XT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag

  Init_Port_pins();
  i=0;
  while(1)
  {
  	#ifdef C5517_UHPI_OUTPUT_TEST
	   
	    //if((P1IN&0xFF)!=0xFF)
	    {
	    	Status_Buffer[0]=P1IN;  // Copy the pin states of P1IN(Lower Bytes of ADD/DATA Lines) 
	    		    }	 
	    //if((P2IN&0xFF)!=0xFF)
	    {
	    	Status_Buffer[1]=P2IN; // Copy the pin states of P2IN(Higher Bytes of ADD/DATA Lines) 
	    		    }	
	    //if((PJIN&0x03)!=0x03)
	    {
	    	Status_Buffer[2]=PJIN&0x0f;  // Copy the pin states of PJIN(Lower 4 bits:
	    	                             // HRDY, HINT pin, HCNTL0, and HCNTL1)
	    }	  	   
	     printf("PJIN = %x \nP6IN = %x\nP1IN = %x\nP2IN = %x\n",PJIN,P6IN,P1IN,P2IN);

		Status_Buffer[3]=P6IN&0xdc;  // Copy the pin states of P6IN(bits 7:6,4:2 -
									 // HBE0, HBE1, HDS1, HR_NW, HWIL)
	   for(i=0;i<30000;i++);
	#endif   
	#ifdef C5517_UHPI_INPUT_TEST
	   P6OUT = 0xFF;			// Set control lines HHWIL,HDS1,HBE0 and HBE1 to HIGH
	   PJOUT = 0xFF;			// Set control lines HCNTL0 and HCNTL1 to HIGH
	   for(i=0;i<5000;++i);
	   HRW_LOW;                 // Set HR/W to low to enable direction of MUX from MSP to C5517
	   P1OUT = 0xFF;			// Set all ADDR/DATA(Lower Bytes) lines to HIGH
	   P2OUT = 0xFF;			// Set all ADDR/DATA(Higher Bytes) lines to HIGH
	   for(i=0;i<5000;++i);
       P1OUT = 0x00;			// Set control lines HHWIL,HDS1,HBE0 and HBE1 to LOW
	   P2OUT = 0x00;			// Set control lines HCNTL0 and HCNTL1 to LOW
	   P6OUT = 0x00;			// Set all ADDR/DATA(Lower Bytes) lines to LOW
	   PJOUT = 0x00;			// Set all ADDR/DATA(Higher Bytes) lines to LOW
	   for(i=0;i<5000;++i);
	#endif   
  }
  
   
}

void Init_Port_pins(void)
{
// Declaring the pin states and direction
#ifdef C5517_UHPI_OUTPUT_TEST
// Make all pins are input
  P6DIR &= (~0xDC);  	// P6.2-hhwil, P6.3 DIR set as output for HPI(write),P6.4-HDS1 as output , HBE0,HBE1 as out put
  PJDIR &= (~0x0F);      // PJ.1 set as input (HINT) (pin 73)
  //PJDIR |= 0xC;     // PJ.2(HCNTL1) PJ.3(HCNTL0) set as output 
  //P6OUT&=(~0xC0);   // HBE0,1 = 0
  
  P1DIR = INPUT_PORT;		 // P1 = Input port(D0-D7)
  P2DIR = INPUT_PORT;		 // P2 = Input port(D8-D15)
#endif   
#ifdef C5517_UHPI_INPUT_TEST
// Make all pins are input
  P6DIR |= 0xDC;  	// P6.2-hhwil, P6.3 DIR set as output for HPI(write),P6.4-HDS1 as output , HBE0,HBE1 as out put
  PJDIR |= 0x0F;      // PJ.1 set as input (HINT) (pin 73)
  //PJDIR |= 0xC;     // PJ.2(HCNTL1) PJ.3(HCNTL0) set as output 
  //P6OUT&=(~0xC0);   // HBE0,1 = 0
  
  P1DIR = OUTPUT_PORT;		 // P1 = Input port(D0-D7)
  P2DIR = OUTPUT_PORT;		 // P2 = Input port(D8-D15)
#endif

}

