/*
* audio_utils.c
*
*
* Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "audio_utils.h"

Int gain[5] = { 0, 0, 0, 0, 0};

EQ_Status EQstatus = {sizeof(IEQ_Status), 64, 5, NULL, NULL};

EQ_Params eqlp = {sizeof(IEQ_Params), TX_FRAME_LEN, 5, gain, NULL, NULL};
EQ_Params eqrp = {sizeof(IEQ_Params), TX_FRAME_LEN, 5, gain, NULL, NULL};

IEQ_Handle  eqleft, eqright;
EQ_Params eqLeftParams, eqRightParams;

IALG_MemRec mp3MemTab[MAX_HEAP_SECTIONS];

#pragma DATA_ALIGN(eq_heap_section_1, 4)
#pragma DATA_SECTION(eq_heap_section_1, "MP3_DARAM_HEAP")
#pragma DATA_SECTION(eq_heap_section_2, "MP3_DARAM_HEAP")
#pragma DATA_SECTION(eq_heap_section_3, "MP3_DARAM_HEAP")
#pragma DATA_SECTION(eq_heap_section_4, "MP3_DARAM_HEAP")
#pragma DATA_SECTION(eq_heap_section_5, "MP3_DARAM_HEAP")
#pragma DATA_SECTION(eq_heap_section_6, "MP3_DARAM_HEAP")

unsigned int eq_heap_section_1[10];
unsigned int eq_heap_section_2[NUM_OF_BANDS*20];
unsigned int eq_heap_section_3[6];

unsigned int eq_heap_section_4[10];
unsigned int eq_heap_section_5[NUM_OF_BANDS*20];
unsigned int eq_heap_section_6[6];
unsigned int counter;

const EQ_Coeff EQ_TI_48KHzCoeffs[] = {
	0x3fbc7cee,
	0x80448543,
	0x3ee88459,
	0x8128e0b6,
	0x3bbd91b7,
	0x85516fd2,
	0x306d692a,
	0x9ea2912d,
	0x1901aeb1,
	0x00000000

};

const EQ_Coeff EQ_TI_48KHzDeltaCoeffs[] = {

	0x001491fb,
	0xffeb6e2e,
	0x00522b5d,
	0xffaddfe7,
	0x0119978f,
	0xfee8d12a,
	0x02b2bed6,
	0xfda9cc01,
	0x03785534,
	0x00000000

};

const EQ_Coeff EQ_TI_8KHzCoeffs[] = {
		0x3e6f09e6,
		0x81b4e48e,
		0x39b65e24,
		0x88a054e1,
		0x29d58abc,
		0xb529f908,
		0xfd6958d7,
		0x3d6958d7,
		0x0d29571c,
		0x4d29571c
};

const EQ_Coeff EQ_TI_8KHzDeltaCoeffs[] = {
		0x007389e3,
		0xff8c96f3,
		0x018072f5,
		0xfe86f022,
		0x032320cf,
		0xfdc81a46,
		0x02e44c98,
		0x02e44c98,
		0x29c936fa,
		0x29c936fa

};

const EQ_Coeff EQ_TI_16KHzCoeffs[] = {
	0x3f364afb,
	0x80d2bf1c,
	0x3cc77477,
	0x83d25c8d,
	0x33fc0727,
	0x94d824a3,
	0x1901aeb1,
	0x00000000,
	0xe0d226ed,
	0x00000000
};

const EQ_Coeff EQ_TI_16KHzDeltaCoeffs[] = {
	0x003c1442,
	0xffc3f003,
	0x00ddeecd,
	0xff2322c8,
	0x0257175c,
	0xfdd6830a,
	0x03785534,
	0x00000000,
	0x01b1ecf2,
	0x00000000
};

/*
 *  ======== EQ_create ========
 *  Create an instance of a EQ object.
 */
IEQ_Handle EQ_create(const IEQ_Fxns *dfxns, const EQ_Params *prms, int instance)
{
    IALG_MemRec *memTab;
    Int n;
    ALG_Handle alg;
    IALG_Fxns *fxnsPtr;
    IALG_Fxns *fxns = (IALG_Fxns *)dfxns;
    IALG_Handle p = NULL;
    IALG_Params *params = (IALG_Params *)prms;

    memTab = mp3MemTab;

    n = fxns->algAlloc(params, &fxnsPtr, memTab);
    if (n <= 0) {
        return (NULL);
    }

	if (instance == 0)
	{
	    memTab[0].base = eq_heap_section_1;
	    memTab[1].base = eq_heap_section_2;
	    memTab[2].base = eq_heap_section_3;
	}
	else if (instance == 1)
	{
	    memTab[0].base = eq_heap_section_4;
	    memTab[1].base = eq_heap_section_5;
	    memTab[2].base = eq_heap_section_6;
	}

	for (counter = 0; counter < n; counter++)
    	memset(memTab[counter].base, 0, memTab[counter].size);

    alg = (IALG_Handle)memTab[0].base;
    alg->fxns = fxns;

    if (fxns->algInit(alg, memTab, p, params) == IALG_EOK) {
        return (IEQ_Handle)(alg);
    }

    fxns->algFree(alg, memTab);

	return NULL;
}

/*
 *  ======== EQ_delete ========
 *  Delete a EQ instance object
 */
Void EQ_delete(IEQ_Handle handle)
{
    IALG_MemRec *memTab;
    IALG_Fxns *fxns;
    ALG_Handle alg = (ALG_Handle)handle;

    fxns = alg->fxns;
    memTab = mp3MemTab;

    memTab[0].base = alg;
    fxns->algFree(alg, memTab);
}

Void EQ_init();
Int16 eq_config(Uint8 audSrc)
{
	eqLeftParams  = eqlp;
	eqRightParams = eqrp;

	EQ_init();

	eqleft = EQ_create(&EQ_TI_IEQ, &eqLeftParams, 0);
	if(eqleft == NULL)
	{
		printf("EQ_create Failed\n");
		return 1;
	}

	eqright = EQ_create(&EQ_TI_IEQ, &eqRightParams, 1);
	if(eqright == NULL)
	{
		printf("EQ_create Failed\n");
		return 1;
	}

	if(audSrc == AUDIO_SOURCE_SD)
	{
		EQstatus.coeffs =  (IEQ_Coeff *)EQ_TI_8KHzCoeffs;
		EQstatus.deltaCoeffs = (IEQ_Coeff *)EQ_TI_8KHzDeltaCoeffs;
	}
	else
	{
		EQstatus.coeffs =  (IEQ_Coeff *)EQ_TI_48KHzCoeffs;
		EQstatus.deltaCoeffs = (IEQ_Coeff *)EQ_TI_48KHzDeltaCoeffs;
	}

	EQ_setStatus(eqleft, &EQstatus);
	EQ_setStatus(eqright, &EQstatus);

	return 0;
}

#pragma DATA_ALIGN(src_heap_section_1, 4)
#pragma DATA_SECTION(src_heap_section_1, "MP3_DARAM_HEAP")
#pragma DATA_SECTION(src_heap_section_2, "MP3_DARAM_HEAP")

unsigned int src_heap_section_1[156];
unsigned int src_heap_section_2[156];
/*
 *  ======== SRC_create ========
 *  Create an instance of a SRC object.
 */
ISRC_Handle SRC_create(const ISRC_Fxns *dfxns, const SRC_Params *prms, int instance)
{
    IALG_MemRec *memTab;
    Int n, counter;
    ALG_Handle alg;
    IALG_Fxns *fxnsPtr;
    IALG_Fxns *fxns = (IALG_Fxns *)dfxns;
    IALG_Handle p = NULL;
    IALG_Params *params = (IALG_Params *)prms;

    memTab = mp3MemTab;

    n = fxns->algAlloc(params, &fxnsPtr, memTab);
    if (n <= 0) {
        return (NULL);
    }

	if (instance == 0)
	    memTab[0].base = src_heap_section_1;
	else if (instance == 1)
	    memTab[0].base = src_heap_section_2;

	for (counter = 0; counter < n; counter++)
    	memset(memTab[counter].base, 0, memTab[counter].size);

    alg = (IALG_Handle)memTab[0].base;
    alg->fxns = fxns;

    if (fxns->algInit(alg, memTab, p, params) == IALG_EOK) {
        return (ISRC_Handle)(alg);
    }

    fxns->algFree(alg, memTab);

	return NULL;
}

ISRC_Params lp = {sizeof(SRC_Params),TX_FRAME_LEN,TX_FRAME_LEN,16000,SAMP_RATE_SD,0};
ISRC_Params rp = {sizeof(SRC_Params),TX_FRAME_LEN,TX_FRAME_LEN,16000,SAMP_RATE_SD,0};

ISRC_Handle srcLeft, srcRight;
SRC_Params srcLeftParams, srcRightParams;

int srcOutL[TX_FRAME_LEN];
int srcOutR[TX_FRAME_LEN];
int srcInL[TX_FRAME_LEN];
int srcInR[TX_FRAME_LEN];

/*
 *  ======== SRC_delete ========
 *  Delete a SRC instance object
 */
Void SRC_delete(ISRC_Handle handle)
{
    IALG_MemRec *memTab;
    IALG_Fxns *fxns;
    ALG_Handle alg = (ALG_Handle)handle;

    fxns = alg->fxns;
    memTab = mp3MemTab;

    memTab[0].base = alg;
    fxns->algFree(alg, memTab);
}

Int16 src_config(void)
{
	srcLeftParams  = lp;
	srcRightParams = rp;

  	/* Create an instance of the algorithm */
  	srcLeft = SRC_create(&SRC_TI_ISRC, &srcLeftParams, 0);
  	if(srcLeft == NULL)
  	{
		printf("SRC Create Failed\n");
		return 1;
	}

	//SRC_setInput(srcLeft, srcOutL);

#if 1
  	srcRight = SRC_create(&SRC_TI_ISRC, &srcRightParams, 1);
  	if(srcRight == NULL)
  	{
		printf("SRC Create Failed\n");
	}

	//SRC_setOutput(srcRight, srcOutR);

#endif

	return 0;
}

void src_delete(void)
{
	SRC_delete(srcLeft);
	SRC_delete(srcRight);
}

void src_set_samplerate(Uint32 rate)
{
	SRC_TI_setInputRate(srcLeft, rate);
	SRC_TI_setInputRate(srcRight, rate);
}

