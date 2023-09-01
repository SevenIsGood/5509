/***************************************************************************************************
                   							 HPI_Host.h
****************************************************************************************************/

// #defines
#define SARAM_BYTE_ADDRESS_START(Adr) (Adr<<1)
#define DMA_BYTE_ADDRESS(Adr) (SARAM_BYTE_ADDRESS_START(Adr)+0x80000)
#define HPI_HOST_SARAM_ADR(Adr) (DMA_BYTE_ADDRESS(Adr)>>2)

#define INPUT_PORT 	  (0x00)
#define OUTPUT_PORT   (0xFF)

#define READ  0
#define WRITE 1

#define WITHOUT_INC  0
#define AUTO_INC 	 1

#define HPIC 	0
#define HPIA 	1
#define HPID_A 	2
#define HPID_M 	3

#define HHWIL_LOW		P6OUT&=(~0x4)
#define HHWIL_HIGH		P6OUT|=0x4		

#define HRW_LOW			P6OUT&=(~0x8)
#define HRW_HIGH		P6OUT|=0x8

#define HDS_LOW			P6OUT&=(~0x10)
#define HDS_HIGH		P6OUT|=0x10

#define HCNTL0_LOW		PJOUT&=(~0x8)
#define HCNTL0_HIGH		PJOUT|=0x8

#define HCNTL1_LOW		PJOUT&=(~0x4)
#define HCNTL1_HIGH		PJOUT|=0x4

//HPIC BIT DEFINES
#define HWOB			BIT0
#define DSP_INT			BIT1
#define HINT			BIT2
#define HRDY			BIT3
#define FETCH			BIT4
#define XPIA			BIT5
#define RESET			BIT6
#define HPI_RST			BIT7
#define HWOB_STAT		BIT8
#define DUAL_HPIA		BIT9
#define LB_MODE			BITA
#define HPIA_RW_SEL		BITB
#define NRDY_HPIA_WR	BITC
#define NRDY_HPIA_RD	BITD
#define NRDY_HPID_WR	BITE
//#define NRDY_HPID_RD	BITF // Bit Defenition is not clear from spec


// Address Splitting Union Definition
typedef union __adress_divide
{
	unsigned long int byte_addr;
	unsigned char byte_addr_buffer[4];
}Address_Divide;

// Function declarations
void Initialize_HPI_Pins(void);
void Config_HPI(unsigned char ,unsigned char );
void HPI_Write_Address(unsigned long int ad_value);
unsigned long int HPI_Read_Address(void);
void HPI_WRITE_DATA(unsigned int *Write_Data,unsigned int Buffer_size,unsigned char inc);
void HPID_READ_DATA(unsigned int *Read_Data,unsigned int, unsigned char inc);
