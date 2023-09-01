/*=======================================================================

 *
 * TestTIesrEngine.c
 *
 * Program to test TIesrEngine APIs by recognizing data contained in a file
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

 
 TestTIesrEngine Mem GramDir JacFile HLRFile AudioFile MultiReco

 Mem = Working memory size in 16-bit elements
 GramDir = Grammar and model directory where models created by TIesrFlex exist
 JACFile = JAC file location where dynamic noise/channel adaptation data will be placed
 HLRFile = HLR phonetic tree file location
 AudioFile =  File name containing raw audio as input to be recognized.
 MultiReco = Flag indicating to perform multiple recognition on file to EOF

======================================================================*/

/* C run-time usage */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


// Include the app macros, constants and structures
#include "TestTIesrEngine.h"
#include "gmhmm_type.h"
#include "my_fileio.h"

/*--------------------------------*/
#ifdef C55x
int main( )
#else
int main( int argc, char** argv )
#endif
{

  /* TIesr engine function object and error types */
  TIesrEngineSIRECOType tiesr;

  TIesrEngineStatusType tiesrStatus;
  TIesrEngineJACStatusType jacStatus;

#ifdef LIVE_AUDIO
  TIesrFA_Error_t faError;
  TIesrFA_t faInstance;
  unsigned int framesQueued;
  int isfile;
#endif

  /* Argument values */
  long memorySize;
  TIesr_t tiesrInstance;
  char *gramDir;
  char *jacFile;
  char *sbcFile;
  char *audioFile;
  int  multiReco;

  /* Frame input from file */
  short frmBuffer[FRAME_LEN];
  FILE *aFile;
  size_t numRead;
  unsigned int framecount = 0;

  /* TIesr end of processing */
  short endReco;

  /* TIesr recognition result processing */
  short numWords;
  short word;

  /* For reading SAD parameters */
  short p1, p2, p3, p4, p5;


#if defined (DEBUG)
  mtrace( );
#endif



  /* Extract of command line arguments */
  if( argc != 6 && argc != 7  )
  {
    printf( "Invalid argument count\n" );
    return 1;
  }

  memorySize = atol( argv[1] );
  gramDir = argv[2];
  jacFile = argv[3];
  sbcFile = argv[4];
  audioFile = argv[5];

  if( argc == 7 )
  {
    multiReco = atoi( argv[6]);
  }
  else
  {
    multiReco = 0;
  }



  /* Initialize the TIesr engine function object */
  TIesrEngineOpen( &tiesr );


  /* Open an instance of the TIesr engine, loading the grammar */
  tiesrInstance = (TIesr_t)malloc( memorySize * sizeof(short) );
  if( !tiesrInstance )
  {
    printf( "Could not allocate memory\n" );
    exit( 1 );
  }

  tiesrStatus = tiesr.OpenASR( gramDir, memorySize, tiesrInstance );
  if( tiesrStatus != eTIesrEngineSuccess )
  {
    printf( "Could not open TIesr Instance %d\n", tiesrStatus );
    exit( 1 );
  }


  /* Set parameters for recognition. There are many other parameters that
   could be adjusted. */
/*
  tiesr.SetTIesrPrune( tiesrInstance, PRUNE_FACTOR );
  tiesr.SetTIesrTransiWeight( tiesrInstance, TRANSI_WEIGHT );
  tiesr.SetTIesrJacRate( tiesrInstance, JAC_RATE );
*/

  tiesr.SetTIesrPrune( tiesrInstance, -6 );
  tiesr.SetTIesrTransiWeight( tiesrInstance, 0 );
  tiesr.SetTIesrJacRate( tiesrInstance, 10 );
  
  tiesr.GetTIesrSAD( tiesrInstance, &p1, &p2, &p3, &p4, &p5 );
/*
  tiesr.SetTIesrSAD(tiesrInstance, p1, SAD_SPEECH_DB, p3, SAD_MIN_END_FRAMES, p5 );
*/
  tiesr.SetTIesrSAD(tiesrInstance, p1, 44, p3, SAD_MIN_END_FRAMES, p5 );

  /* Open the JAC adaptation information. If the JAC file does not exist,
   then use an initial set of parameters */
  jacStatus = tiesr.JAC_load( jacFile, tiesrInstance, gramDir, sbcFile );
  if( jacStatus != eTIesrEngineJACSuccess )
  {
    printf( "Failed to load JAC %d\n", jacStatus );
    exit( 1 );
   }


#ifdef LIVE_AUDIO

   faError = TIesrFA_init( &faInstance,
      SAM_FREQ,
      TIesrFALinear,
      FRAME_LEN,
      CIRCULAR_FRAMES,
      AUDIO_FRAMES,
      READ_RATE,
      audioFile,
      20 );

   if( faError != TIesrFAErrNone )
   {
      printf( "Failed to init audio channel: %d\n", faError );
      exit( 1 );
   }

   faError = TIesrFA_open( &faInstance );
   if( faError != TIesrFAErrNone )
   {
      printf( "Failed to open audio channel: %d\n", faError );
      exit( 1 );
   }

   aFile = fopen( audioFile, "rb" );
   isfile = ( aFile != NULL ) ? 1:0;
   if( aFile != NULL )
   {
      fclose( aFile );
   }

   faError = TIesrFA_start( &faInstance );
   if( faError != TIesrFAErrNone )
   {
      printf( "Failed to start audio channel: %d\n", faError );
      exit( 1 );
   }

#else


  /* Open the audio file */
  aFile = my_fopen( audioFile, "rb" );
  if( !aFile )
  {
    printf( "Failed to open audio file\n" );
    exit( 1 );
  }
  
  // For testing only, advance to proper location in file
/*
   {
      unsigned int uStart = 0;
      for (; uStart < 120; uStart++ )
         fread( frmBuffer, sizeof ( short), FRAME_LEN, aFile );
   }
*/
#endif


  /* Process one or more utterances in the file */
  do
  {

    /* Initialize the TIesr search engine for this utterance */
    tiesrStatus = tiesr.OpenSearchEngine( tiesrInstance );
    if( tiesrStatus != eTIesrEngineSuccess )
    {
      printf( "Failed to initialize tiesr engine instance %d\n", tiesrStatus );
      exit( 1 );
    }

    /* Initialize flag indicating TIesr has not finished
     recognition of an utterance. */
    endReco = FALSE;

#ifdef LIVE_AUDIO


    faError = TIesrFA_getframe( &faInstance, frmBuffer, BLOCK, &framesQueued );
    if( faError != TIesrFAErrNone )
    {
       printf( "Failed audio channel get frame: %d\n", faError );
       exit( 1 );
    }
    numRead = FRAME_LEN;
#else

    /* Read initial frame */
    numRead = my_fread( frmBuffer, sizeof(short), FRAME_LEN, aFile );
#endif

    framecount++;


    /* Process frames until recognition result or end of file */
    while( !endReco && numRead == FRAME_LEN )
    {
      tiesrStatus = tiesr.CallSearchEngine( frmBuffer, tiesrInstance, 0, TRUE );

      if( tiesrStatus != eTIesrEngineSuccess )
      {
        printf( "TIesr search engine failure %d\n", tiesrStatus );
        exit( 1 );
      }

      /* Check if utterance complete */
      endReco = tiesr.SpeechEnded( tiesrInstance );

      /* Read next frame of data if available and needed */
      if( !endReco )
      {
#ifdef LIVE_AUDIO
         faError = TIesrFA_getframe( &faInstance, frmBuffer, BLOCK, &framesQueued );
         if( faError != TIesrFAErrNone )
         {
            if( isfile )
            {
               numRead = 0;
            }
            else
            {
               printf( "Failed audio channel get frame: %d\n", faError );
               exit( 1 );
            }
         }
         else
         {
            numRead = FRAME_LEN;
         }
#else
         numRead = my_fread( frmBuffer, sizeof(short), FRAME_LEN, aFile );
#endif
         framecount++;
      }
    }

    /* Check if there is a recognition hypothesis (do backtrace) */
    tiesrStatus = tiesr.CloseSearchEngine( tiesrStatus, tiesrInstance );
    if( tiesrStatus != eTIesrEngineSuccess || !endReco )
    {
      printf( "No hypothesis found %d\n", tiesrStatus );
    }


    /* Print the hypothesized words */
    if( tiesrStatus == eTIesrEngineSuccess && endReco )
    {
      printf( "Recognized: " );

      numWords = tiesr.GetAnswerCount( tiesrInstance, 0 );
      for( word = 0; word < numWords; word++ )
      {
        printf( " %s", tiesr.GetAnswerWord( word, tiesrInstance, 0 ) );
      }
      printf( "\n" );

      printf( "Per-frame Score: %d\n",
              (short)( tiesr.GetTotalScore(tiesrInstance)/tiesr.GetFrameCount(tiesrInstance) ) );

      /* Update the JAC estimates if there was successful recognition */
/*
      jacStatus = tiesr.JAC_update( tiesrInstance );
      if( jacStatus != eTIesrEngineJACSuccess )
      {
        printf( "Failed to update JAC %d\n", jacStatus );
        exit( 1 );
      }
*/
    }

  }
  while( multiReco && numRead == FRAME_LEN );

#ifdef LIVE_AUDIO
   TIesrFA_stop( &faInstance );
   TIesrFA_close( &faInstance );
   TIesrFA_destroy( &faInstance );
#else
   /* Close audio file */
   my_fclose( aFile );
#endif
   
  /* Since TIesr will be shut down, save the JAC parameters */
  jacStatus = tiesr.JAC_save( jacFile, tiesrInstance );
  if( jacStatus != eTIesrEngineJACSuccess )
  {
    printf( "Failed to save JAC parameters %d\n", jacStatus );
    exit( 1 );
  }


  /* Close the ASR instance */
  tiesr.CloseASR( tiesrInstance );

  /* Done with TIesr. At this time this function does nothing. */
  TIesrEngineClose( &tiesr );

  exit( 0 );
}
