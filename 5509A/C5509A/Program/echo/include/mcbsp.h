#ifndef _MCBSP_H_
#define _MCBSP_H_
#define USE_MCBSP0
#ifdef USE_MCBSP0
void mcbspRcvHook();
void mcbspXmitHook();
#else
void mcbspRcvHook(){}
void mcbspXmitHook(){}
#endif
#endif
