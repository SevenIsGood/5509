/*=======================================================================

 *
 * TestTIesrEngine.h
 *
 * Header for implementation of program to test TIesrEngine API.
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation version 2.1 of the License.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any kind,
 * whether express or implied; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *

 Include file for building an application to do speech
 recognition using the TIesrEngine APIs This is a simple command line
 app that runs recognition of an utterance and outputs the words
 recognized.

 The macros, structures, and declarations in this header are
 for the convenience in building the application.

======================================================================*/
..........
#ifndef _TESTTIESRENGINE_H
#define	_TESTTIESRENGINE_H

// Windows headers
#if defined (WIN32)
#include <windows.h>
#endif


#ifdef	__cplusplus
extern "C" {
#endif


/* The TIesrEngine API header data needed for an application */

#include <tiesr_engine_api_sireco.h>


/*--------------------------------
  Overall constants
  --------------------------------*/

#ifndef TRUE
static const int TRUE = 1;
#endif

#ifndef FALSE
static const int FALSE = 0;
#endif


/*--------------------------------
 Parameters used within the recognition program to setup the 
 TIesrEngine instance.  A lot more parameters could be set to tune the
 recognizer performance.
 --------------------------------*/

static const int JAC_RATE = 10;
static const int PRUNE_FACTOR = -6;
static const int TRANSI_WEIGHT = 0;

/* SAD end of utterance parameter 15 = 300ms */
static const int SAD_MIN_END_FRAMES = 30;

/* SAD miniumum speech detection value in dB */
static const int SAD_SPEECH_DB = 60;

/*-------------------------------------
  Parameters defining the command line arguments. Needed for the C55x platform
  which does not easily support passing of command line. Modify to match your
  testing files.
--------------------------------------*/
#ifdef C55x
int argc = 7;

char *argv[]= 
{ "TestTIesrEngine",
  "2668",
#ifndef C55x_8KHZ
/* For testing 24kHz sample grammar */
  "u:\\C55xTIesr\\GramCmdListBE\\",
  "u:\\C55xTIesr\\test.jac",
  "u:\\C55xTIesr\\GramCmdListBE\\hlrtree.bin",
  "u:\\C55xTIesr\\record2_be.raw",
#else
#if 0 // 5 word grammar
  /* For testing 8kHz sample grammar */
  "..\\..\\..\\C55xTIesrData\\Gram5WordBE\\",
  "..\\..\\..\\C55xTIesrData\\Gram5WordBE\\test.jac",
  "..\\..\\..\\C55xTIesrData\\Gram5WordBE\\hlrtree.bin",
  "..\\..\\..\\C55xTIesrData\\Test5WordBE\\test5wdbe.raw",
  //"..\\..\\..\\C55xTIesrData\\Test5WordBE\\test5WdBe_2.raw",
  //"..\\..\\..\\C55xTIesrData\\Test5WordBE\\test5WdBe_3.raw",
  //"..\\..\\..\\C55xTIesrData\\Test5WordBE\\test5WdBe_4.raw",
  //"..\\..\\..\\C55xTIesrData\\Test5WordBE\\test5WdBe_5.raw",
  //"..\\..\\..\\C55xTIesrData\\Test5WordBE\\test5WdBe_6.raw",
  //"..\\..\\..\\C55xTIesrData\\Test5WordBE\\test5WdBe_7.raw",
  //"..\\..\\..\\C55xTIesrData\\Test5WordBE\\test5WdBe_8.raw",
#endif
#if 0 // remote control grammar -- 1 pronunciation
  /* For testing 8kHz sample grammar */
  "..\\..\\..\\C55xTIesrData\\GramRc1BE\\",
  "..\\..\\..\\C55xTIesrData\\GramRc1BE\\test.jac",
  "..\\..\\..\\C55xTIesrData\\GramRc1BE\\hlrtree.bin",
  "..\\..\\..\\C55xTIesrData\\TestRcBE\\testRc5WdBe.raw",
#endif
#if 0 // remote control grammar -- 2 pronunciation
  /* For testing 8kHz sample grammar */
  "..\\..\\..\\C55xTIesrData\\GramRc2BE\\",
  "..\\..\\..\\C55xTIesrData\\GramRc2BE\\test.jac",
  "..\\..\\..\\C55xTIesrData\\GramRc2BE\\hlrtree.bin",
  "..\\..\\..\\C55xTIesrData\\TestRcBE\\testRc5WdBe.raw",
#endif
#if 0 // remote control grammar -- 1 pronunciation, modified dictionary
  /* For testing 8kHz sample grammar */
  "..\\..\\..\\C55xTIesrData\\GramRc1BE_newDict\\",
  "..\\..\\..\\C55xTIesrData\\GramRc1BE_newDict\\test.jac",
  "..\\..\\..\\C55xTIesrData\\GramRc1BE_newDict\\hlrtree.bin",
  "..\\..\\..\\C55xTIesrData\\TestRcBE\\testRc5WdBe.raw",
#endif
#if 0 // remote control grammar -- 1 word (Play), modified dictionary
  /* For testing 8kHz sample grammar */
  "..\\..\\..\\C55xTIesrData\\GramRc1WdBE\\",
  "..\\..\\..\\C55xTIesrData\\GramRc1WdBE\\test.jac",
  "..\\..\\..\\C55xTIesrData\\GramRc1WdBE\\hlrtree.bin",
  "..\\..\\..\\C55xTIesrData\\TestRcBE\\testRc5WdBe.raw",
#endif
#if 0 // remote control grammar -- 2 words (Play | Pause), modified dictionary
  /* For testing 8kHz sample grammar */
  "..\\..\\..\\C55xTIesrData\\GramRc2WdBE\\",
  "..\\..\\..\\C55xTIesrData\\GramRc2WdBE\\test.jac",
  "..\\..\\..\\C55xTIesrData\\GramRc2WdBE\\hlrtree.bin",
  "..\\..\\..\\C55xTIesrData\\TestRcBE\\testRc5WdBe.raw",
#endif
#if 0 // remote control grammar -- 3 words (Play | Pause | Record), modified dictionary
  /* For testing 8kHz sample grammar */
  "..\\..\\..\\C55xTIesrData\\GramRc3WdBE\\",
  "..\\..\\..\\C55xTIesrData\\GramRc3WdBE\\test.jac",
  "..\\..\\..\\C55xTIesrData\\GramRc3WdBE\\hlrtree.bin",
  "..\\..\\..\\C55xTIesrData\\TestRcBE\\testRc5WdBe.raw",
#endif
#if 0 // remote control grammar -- 4 words (Play | Pause | Record | Up), modified dictionary
  /* For testing 8kHz sample grammar */
  "..\\..\\..\\C55xTIesrData\\GramRc4WdBE\\",
  "..\\..\\..\\C55xTIesrData\\GramRc4WdBE\\test.jac",
  "..\\..\\..\\C55xTIesrData\\GramRc4WdBE\\hlrtree.bin",
  "..\\..\\..\\C55xTIesrData\\TestRcBE\\testRc5WdBe.raw",
#endif
#if 0 // remote control grammar -- 5 words (Play | Pause | Record | Up | Down), modified dictionary
  /* For testing 8kHz sample grammar */
  "..\\..\\..\\C55xTIesrData\\GramRc5WdBE\\",
  "..\\..\\..\\C55xTIesrData\\GramRc5WdBE\\test.jac",
  "..\\..\\..\\C55xTIesrData\\GramRc5WdBE\\hlrtree.bin",
  "..\\..\\..\\C55xTIesrData\\TestRcBE\\testRc5WdBe.raw",
#endif
#if 0 // remote control grammar -- 6 words (Play | Pause | Record | Channel (Up | Down)), modified dictionary
  /* For testing 8kHz sample grammar */
  "..\\..\\..\\C55xTIesrData\\GramRc6WdBE\\",
  "..\\..\\..\\C55xTIesrData\\GramRc6WdBE\\test.jac",
  "..\\..\\..\\C55xTIesrData\\GramRc6WdBE\\hlrtree.bin",
  "..\\..\\..\\C55xTIesrData\\TestRcBE\\testRc5WdBe.raw",
#endif
#if 0 // 30 word grammar
  /* For testing 8kHz sample grammar */
  "..\\..\\..\\C55xTIesrData\\Gram30WordBE\\",
  "..\\..\\..\\C55xTIesrData\\Gram30WordBE\\test.jac",
  "..\\..\\..\\C55xTIesrData\\Gram30WordBE\\hlrtree.bin",
  "..\\..\\..\\C55xTIesrData\\Test30WordBE\\test30wdbe.raw",
#endif
#if 0 // keyword spotting grammar #1 */
/* For testing 8kHz sample grammar */
  "..\\..\\..\\C55xTIesrData\\GramKWS\\",
  "..\\..\\..\\C55xTIesrData\\GramKWS\\test.jac",
  "..\\..\\..\\C55xTIesrData\\GramKWS\\hlrtree.bin",
  "..\\..\\..\\C55xTIesrData\\TestKWS\\testKWSBe_1.raw",
#endif
#if 1 // keyword spotting grammar #2 */
/* For testing 8kHz sample grammar */
  "..\\..\\..\\C55xTIesrData\\GramKWS2Be\\",
  "..\\..\\..\\C55xTIesrData\\GramKWS2Be\\test.jac",
  "..\\..\\..\\C55xTIesrData\\GramKWS2Be\\hlrtree.bin",
  "..\\..\\..\\C55xTIesrData\\TestKWS\\testKWSBe_1.raw",
#endif
#endif
  "1"
};

#endif

#ifdef	__cplusplus
}
#endif

#endif	/* _TESTTIESRENGINE_H */

