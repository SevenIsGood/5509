#ifndef _TIMER_H_
#define _TIMER_H_
#ifdef USE_DMA0INTERRUPT
void isrTimer0Hook(void)
#else
void isrTimer0Hook(void){}
#endif
#endif
