#ifndef _DMA_H_
#define _DMA_H_
#ifdef USE_DMA0INTERRUPT
void isrDma0Hook()
#else
void isrDma0Hook(){}
#endif
#endif
