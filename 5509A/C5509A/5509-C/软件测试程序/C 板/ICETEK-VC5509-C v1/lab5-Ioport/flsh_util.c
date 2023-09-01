#include"myapp.h"
#define FlashMap1BaseAddr 0x200000
void WriteIsOver(void)
{
	unsigned int i;
	unsigned char LastToggleBit,CurrentToggleBit;
	LastToggleBit = (*(volatile unsigned char*)FlashMap1BaseAddr) & 0x40;
	CurrentToggleBit = (*(volatile unsigned char*)FlashMap1BaseAddr) & 0x40;
	for(i = 0; ; i++)
	{
		LastToggleBit = CurrentToggleBit;
		CurrentToggleBit = (*(volatile unsigned char*)FlashMap1BaseAddr) & 0x40;
		if(LastToggleBit == CurrentToggleBit) break;
	}
}


int Read_ID(void)
{
	int CurrentToggleBit,i;
	for(i=0;i<0x10;i++);
	*(volatile unsigned char*)(FlashMap1BaseAddr + 0x555) = 0xaa;
	for(i=0;i<0x10;i++);
	*(volatile unsigned char*)(FlashMap1BaseAddr + 0x2aa) = 0x55;	
	for(i=0;i<0x10;i++);
	*(volatile unsigned char*)(FlashMap1BaseAddr + 0x555) = 0x90;
	for(i=0;i<0x10;i++);
	CurrentToggleBit = (*(volatile unsigned char*)FlashMap1BaseAddr);
	for(i=0;i<0x10;i++);
	return (CurrentToggleBit);
}
void WriteByte(unsigned int Addr,unsigned int Val)
{

	*(volatile unsigned char*)(FlashMap1BaseAddr + 0x555) = 0xaa;
	*(volatile unsigned char*)(FlashMap1BaseAddr + 0x2aa) = 0x55;
	*(volatile unsigned char*)(FlashMap1BaseAddr + 0x555) = 0xa0;
	*(volatile unsigned char*)(FlashMap1BaseAddr + Addr) = Val;
	WriteIsOver();
}
void EraseChip(void)
{
	*(volatile unsigned char*)(FlashMap1BaseAddr + 0x555) = 0xaa;
	*(volatile unsigned char*)(FlashMap1BaseAddr + 0x2aa) = 0x55;
	*(volatile unsigned char*)(FlashMap1BaseAddr + 0x555) = 0x80;
	*(volatile unsigned char*)(FlashMap1BaseAddr + 0x555) = 0xaa;
	*(volatile unsigned char*)(FlashMap1BaseAddr + 0x2aa) = 0x55;
	*(volatile unsigned char*)(FlashMap1BaseAddr + 0x555) = 0x10;
	WriteIsOver();
}

