#ifndef _RTL8019A_H
#define _RTL8019A_H

#define PAGE	0x44
#define CR *(unsigned short*)0x200000
#define DMA *(int*)0x200010
/*register declarations for RTL8019AS			*/

/*Page0(PS1=0,PS0=0)	*/
/*RD			*/
typedef struct Page0_read
{
	unsigned short cr;
	unsigned short clda0;
	unsigned short clda1;
	unsigned short bnry;
	unsigned short tsr;
	unsigned short ncr;
	unsigned short fifo;
	unsigned short isr;
	unsigned short crda0;
	unsigned short crda1;
	unsigned short crd0;
	unsigned short crd1;
	unsigned short rsr;
	unsigned short cntr0;
	unsigned short cntr1;
	unsigned short cntr2;
} RTL8019AS_PAGE0_RD,* pRTL8019AS_PAGE0_RD;
/*Page0(PS1=0,PS0=0)	*/
/*WR*/
typedef struct Page0_write
{
	unsigned short cr;
	unsigned short pstart;
	unsigned short pstop;
	unsigned short bnry;
	unsigned short tpsr;
	unsigned short tbcr0;
	unsigned short tbcr1;
	unsigned short isr;
	unsigned short rsar0;
	unsigned short rsar1;
	unsigned short rbcr0;
	unsigned short rbcr1;
	unsigned short rcr;
	unsigned short tcr;
	unsigned short dcr;
	unsigned short immr;     
} RTL8019AS_PAGE0_WR,* pRTL8019AS_PAGE0_WR;
/*Page0(PS1=0,PS0=1)	*/
typedef struct Page1
{
	unsigned short cr;
	unsigned short par0;
	unsigned short par1;
	unsigned short par2;
	unsigned short par3;
	unsigned short par4;
	unsigned short par5;
	unsigned short curr;
	unsigned short mar0;
	unsigned short mar1;
	unsigned short mar2;
	unsigned short mar3;
	unsigned short mar4;
	unsigned short mar5;
	unsigned short mar6;
	unsigned short mar7;
} RTL8019AS_PAGE1,* pRTL8019AS_PAGE1;
/*Page0(PS1=1,PS0=0)	*/
typedef struct Page2
{
	unsigned short cr;
	unsigned short pstart;
	unsigned short pstop;
	unsigned short reserved0;
	unsigned short tpsr;
	unsigned short reserved1[7];
	unsigned short rcr;
	unsigned short dcr;
	unsigned short imr;
} RTL8019AS_PAGE2,* pRTL8019AS_PAGE2;
/*Page0(PS1=1,PS0=1)	*/
typedef struct Page3
{
	unsigned short cr;
	unsigned short page3cr;
	unsigned short bpage;
	unsigned short config0;
	unsigned short config1;
	unsigned short config2;
	unsigned short config3;
	unsigned short test;
	unsigned short csnsav;
	unsigned short hltclk;
	unsigned short reserved;
	unsigned short intr;
	unsigned short pmwp;
	unsigned short config4;
} RTL8019AS_PAGE3,* pRTL8019AS_PAGE3;
#define ARP	    1
#define UDP     2
#define IGMP    3
#define LSS     4

#define BROADCAST     1
#define REC_ARP       2
#define NOD           3

extern unsigned short TxEthnetFrameBuffer[1518/2];
extern unsigned short RxEthnetFrameBuffer[1518/2];
#endif
