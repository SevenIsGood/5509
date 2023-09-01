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

#ifdef LIVE_AUDIO
#include <TIesrFA_User.h>

   static const int CIRCULAR_FRAMES = 10;
   static const int AUDIO_FRAMES = 10;
   static const int READ_RATE = 20000;
   static const int BLOCK = 1;
#endif

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

static const int JAC_RATE = 25;
static const int PRUNE_FACTOR = -8;
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
      /* For testing 8kHz sample grammar */
      "u:\\C55xTIesr\\Gram5WordBE\\",
      "u:\\C55xTIesr\\Gram5WordBE\\test.jac",
      "u:\\C55xTIesr\\Gram5WordBE\\hlrtree.bin.notthere",
      "u:\\C55xTIesr\\test5wdbe.raw",
#endif
  "1"
};

#endif

#ifdef	__cplusplus
}
#endif

#endif	/* _TESTTIESRENGINE_H */

