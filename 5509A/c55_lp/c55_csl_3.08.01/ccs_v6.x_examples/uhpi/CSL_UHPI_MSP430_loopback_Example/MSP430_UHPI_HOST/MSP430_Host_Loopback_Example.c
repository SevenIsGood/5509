#include <msp430f5529.h>
#include <HPI_Host.h>
#include "SetVcoreUp.h"

//#define  WORD_LENGTH_W_MAX 4096  //Testing 4K*2bytes per transfer = 8KB write 
#define WORD_LENGTH_W 128   
#define WORD_LENGTH_R 128
#define BUFFER_SIZE 512

#define SARAM_WORD_ADDRESS_START 0x08000
//#define SARAM_WORD_ADDRESS_START 0x10000
#define SARAM_WORD_ADDRESS_END   0x27000
#define OFFSET 0x1000

unsigned long int Check_Data_integrity(void);

unsigned int Read_Data[BUFFER_SIZE]={0},Write_Data[BUFFER_SIZE]={0};

void main(void)
{
  int i=0;
  unsigned int Data=0;  
  unsigned char flag=0;
  unsigned long int SARAM_Adr=0,Data_Mismatch_cnt[32]={0};
  //unsigned long int Read_Adress=0;
   
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

  Initialize_HPI_Pins();                        // Configure and initialize port pins
  while((PJIN&0x2)==0x2);                  // continuous loop while HINT is not received

  // initialize data to write
  for(i=0;i<BUFFER_SIZE;i++)
  {
  	Write_Data[i]=i;
  	
  }
  SARAM_Adr = SARAM_WORD_ADDRESS_START;

  i=0;
  //do
  {
	  HPI_Write_Address(SARAM_Adr); 		  // Set SARAM Adress to HPIA to set data location to write
	  //Read_Adress=HPI_Read_Address();		  // For Testing, Read back the written Data from HPIA	
	  HPI_WRITE_DATA(Write_Data,WORD_LENGTH_W,AUTO_INC);			  // Write Data to HPID (To SARAM )
	    /******* from the Post-Si test code ******/
	  //for(i=0;i<500;i++);
	  HPI_Write_Address(SARAM_Adr); 		  // Set SARAM Adress to HPIA to set data location to read
	  HPID_READ_DATA(Read_Data,WORD_LENGTH_R,AUTO_INC);			  // Read Data from HPID (From SARAM)
	  Data_Mismatch_cnt[i++]=Check_Data_integrity(); // Check Data integrity
	  SARAM_Adr += OFFSET; 
	  	
  }//while(SARAM_Adr<=SARAM_WORD_ADDRESS_END); 
  
  // Below while loop is to hang the DSP for testing
  // If While(1); is added HPIC acess codes will be optimized
 /* flag=0;
  while(1)
  {
   if(flag==1)
    break;
  }*/
 
  //HPIC access - Set HINt and DSP_INT bits
  Config_HPI(WRITE,HPIC);
  for(i=0;i<500;i++);
  HDS_LOW; 				// HDS1 = 0
  Data = (HINT|DSP_INT);
  P1OUT = (unsigned char)Data;
  P2OUT = 0x00;
  for(i=0;i<50;i++); 
  HDS_HIGH;    			// HDS1 = 1 
  for(i=0;i<500;i++);   
     
  while(1);
 
}

unsigned long int Check_Data_integrity(void)
{
	unsigned int i=0;
	unsigned long int unmatch_cnt=0;
	
	for(i=0;i<WORD_LENGTH_R;i++)
  	{
  	  if(Read_Data[i]!=Write_Data[i]) // Check the data integrity
	  {
	  	unmatch_cnt++;	// Increment unmatched data count
	  }
  	}
  	return(unmatch_cnt);
}  
