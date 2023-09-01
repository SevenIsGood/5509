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

 Mem = Working membory size in 16-bit elements
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

#if 0
#include <time.h>
clock_t t_overhead, t_start, t_stop;
clock_t t_c = 0;
Uint32 t_c_tot = 0;
Uint32 t_c_avg;
Uint32 t_c_max = 0;
Uint16 num_t_c = 0;
#endif

void minit(void);
size_t free_memory(void); // FL: debug
#ifndef __HUGE_MODEL__
#define PRINT_FREE_MEM ( printf("free memory = %u\n", free_memory()) )
#else
#define PRINT_FREE_MEM ( printf("free memory = %lu\n", free_memory()) )
#endif

/*--------------------------------*/
#ifdef C55x
int main( )
#else
int main( int argc, char** argv )
#endif
{

    //minit();
    //PRINT_FREE_MEM; // FL: debug

    //t_start = clock();
    //t_stop = clock();
    //t_overhead = t_stop - t_start;

    /* TIesr engine function object and error types */
    TIesrEngineSIRECOType tiesr;

    TIesrEngineStatusType tiesrStatus;
    TIesrEngineJACStatusType jacStatus;

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

    //PRINT_FREE_MEM; // FL: debug

    tiesrStatus = tiesr.OpenASR( gramDir, memorySize, tiesrInstance );
    if( tiesrStatus != eTIesrEngineSuccess )
    {
        printf( "Could not open TIesr Instance %d\n", tiesrStatus );
        exit( 1 );
    }

    //PRINT_FREE_MEM; // FL: debug

    /* Set parameters for recognition. There are many other parameters that
    could be adjusted. */
    tiesr.SetTIesrPrune( tiesrInstance, PRUNE_FACTOR );
    tiesr.SetTIesrTransiWeight( tiesrInstance, TRANSI_WEIGHT );
    tiesr.SetTIesrJacRate( tiesrInstance, JAC_RATE );
 
    tiesr.GetTIesrSAD( tiesrInstance, &p1, &p2, &p3, &p4, &p5 );
    tiesr.SetTIesrSAD(tiesrInstance, p1, SAD_SPEECH_DB, p3, SAD_MIN_END_FRAMES, p5 );


    /* Open the JAC adaptation information. If the JAC file does not exist,
    then use an initial set of parameters */
    jacStatus = tiesr.JAC_load( jacFile, tiesrInstance, gramDir, sbcFile ); // FL: remove file I/O
    if( jacStatus != eTIesrEngineJACSuccess )
    {
        printf( "Failed to load JAC %d\n", jacStatus );
        exit( 1 );
    }

    /* Open the audio file */
    aFile = my_fopen( audioFile, "rb" );
    if( !aFile )
    {
        printf( "Failed to open audio file\n" );
        exit( 1 );
    }

    /* Process one or more utterances in the file */
    do
    {
        /* Initialize the TIesr search engine for this utterance */
        //t_start = clock();
        tiesrStatus = tiesr.OpenSearchEngine( tiesrInstance );
        //t_stop = clock();
        //t_c = t_stop - t_start - t_overhead;
        //if (t_c > t_c_max)
        //    t_c_max = t_c;
        if( tiesrStatus != eTIesrEngineSuccess )
        {
            printf( "Failed to initialize tiesr engine instance %d\n", tiesrStatus );
            exit( 1 );
        }

        /* Initialize flag indicating TIesr has not finished
        recognition of an utterance. */
        endReco = FALSE;

        /* Read initial frame */
        numRead = my_fread( frmBuffer, sizeof(short), FRAME_LEN, aFile );
        framecount++;

        /* Process frames until recognition result or end of file */
        while( !endReco && numRead == FRAME_LEN )
        {
            //t_start = clock();
            tiesrStatus = tiesr.CallSearchEngine( frmBuffer, tiesrInstance, 0, TRUE );
            //t_stop = clock();
            //t_c = t_stop - t_start - t_overhead;
            //num_t_c++;
            //t_c_tot += t_c;
            //if (t_c > t_c_max)
            //    t_c_max = t_c;

            if( tiesrStatus != eTIesrEngineSuccess )
            {
                printf( "TIesr search engine failure %d\n", tiesrStatus );
                exit( 1 );
            }

            /* Check if utterance complete */
            //t_start = clock();
            endReco = tiesr.SpeechEnded( tiesrInstance );
            //t_stop = clock();
            //t_c = t_stop - t_start - t_overhead;
            //if (t_c > t_c_max)
            //    t_c_max = t_c;

            /* Read next frame of data if available and needed */
            if( !endReco )
            {
                numRead = my_fread( frmBuffer, sizeof(short), FRAME_LEN, aFile );
                framecount++;
            }
        }

        /* Check if there is a recognition hypothesis (do backtrace) */
        //t_start = clock();
        tiesrStatus = tiesr.CloseSearchEngine( tiesrStatus, tiesrInstance );
        //t_stop = clock();
        //t_c = t_stop - t_start - t_overhead;
        //if (t_c > t_c_max)
        //    t_c_max = t_c;
        if( tiesrStatus != eTIesrEngineSuccess || !endReco )
        {
            printf( "No hypothesis found %d\n", tiesrStatus );
        }

        /* Print the hypothesized words */
        if( tiesrStatus == eTIesrEngineSuccess && endReco )
        {
            /* Update the JAC estimates if there was successful recognition */
            //t_start = clock();
            jacStatus = tiesr.JAC_update( tiesrInstance );
            //t_stop = clock();
            //t_c = t_stop - t_start - t_overhead;
            //num_t_c++;
            //t_c_tot += t_c;
            //if (t_c > t_c_max)
            //    t_c_max = t_c;
            if( jacStatus != eTIesrEngineJACSuccess )
            {
                printf( "Failed to update JAC %d\n", jacStatus );
                //exit( 1 );
            }


            //t_start = clock();
            numWords = tiesr.GetAnswerCount( tiesrInstance, 0 );
            //t_stop = clock();
            //t_c = t_stop - t_start - t_overhead;
            //if (t_c > t_c_max)
            //    t_c_max = t_c;
            printf("Number of words recognized = %d\n", numWords);

            printf( "Recognized: " );
            //t_start = clock();
            //tiesr.GetAnswerWord( word, tiesrInstance, 0 );
            //t_stop = clock();
            //t_c = t_stop - t_start - t_overhead;
            //if (t_c > t_c_max)
            //    t_c_max = t_c;
            for( word = 0; word < numWords; word++ )
            {
                printf( " %s", tiesr.GetAnswerWord( word, tiesrInstance, 0 ) );
            }
            printf( "\n" );

            //t_start = clock();
            //tiesr.GetTotalScore(tiesrInstance);
            //t_stop = clock();
            //t_c = t_stop - t_start - t_overhead;
            //if (t_c > t_c_max)
            //    t_c_max = t_c;
            //t_start = clock();
            //tiesr.GetFrameCount(tiesrInstance);
            //t_stop = clock();
            //t_c = t_stop - t_start - t_overhead;
            //if (t_c > t_c_max)
            //    t_c_max = t_c;
            printf( "Per-frame Score: %d\n",
                (short)( tiesr.GetTotalScore(tiesrInstance)/tiesr.GetFrameCount(tiesrInstance) ) );
            printf( "Confidence Score: %d\n",
                (short)( tiesr.GetConfidenceScore(tiesrInstance) ) );
        }

    } while( multiReco && numRead == FRAME_LEN );

    /* Close audio file */
    my_fclose( aFile );

#if 0 // FL: remove file I/O
    /* Since TIesr will be shut down, save the JAC parameters */
    jacStatus = tiesr.JAC_save( jacFile, tiesrInstance );
    if( jacStatus != eTIesrEngineJACSuccess )
    {
        printf( "Failed to save JAC parameters %d\n", jacStatus );
        exit( 1 );
    }
#endif

    //PRINT_FREE_MEM; // FL: debug

    /* Close the ASR instance */
    tiesr.CloseASR( tiesrInstance );

    //PRINT_FREE_MEM; // FL: debug

    /* Done with TIesr. At this time this function does nothing. */
    TIesrEngineClose( &tiesr );

    //PRINT_FREE_MEM; // FL: debug

    exit( 0 );
}
