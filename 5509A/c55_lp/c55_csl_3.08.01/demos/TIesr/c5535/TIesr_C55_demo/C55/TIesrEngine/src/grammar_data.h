#ifndef _GRAMMAR_DATA_H_
#define _GRAMMAR_DATA_H_

#include <stdlib.h>
#include <tistdtypes.h>

extern Int16 gWord[];
extern Int16 gNet[];
extern Int16 gHmm[];
extern Int16 gMu[];
extern Int16 gVar[];
extern Int16 gTran[];
extern Int16 gPdf[];
extern Int16 gMixture[];
extern Int16 gGconst[];
extern Int16 gDim[];
extern Int16 gConfig[];

static Int16 *grammar_data[] = {
   &gWord[0],
   &gNet[0],
   &gHmm[0],
   &gMu[0],
   &gVar[0],
   &gTran[0],
   &gPdf[0],
   &gMixture[0],
   &gGconst[0],
   &gDim[0],
   NULL, 
   &gConfig[0],
   NULL
};

#endif
