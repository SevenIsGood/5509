
/*=======================================================================

 *
 * load.cpp
 *
 * Load model and grammar data.
 *
 * Copyright (C) 2010-2013 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation version 2.1 of the License.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any kind,Gram
 * whether express or implied; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *


 *  Modifications to operate on 16-Bit Plattforms. Add platform specific
 *  load_word_list.
 *  Copyright (C) 2011 Enertex Bayern GmbH - http://www.enertex.de/

 Load models from files. For embedded devices, one may wish to
 change this code to define pointers to the data in memory,
 rather than reading files.

======================================================================*/

/* Headers required by Windows OS */
#if defined (WIN32) || defined (WINCE)
#include <windows.h>

#elif defined (DSPBIOS_BUILD) // FL: use DSP/BIOS heap
#include <std.h>
#include <mem.h>
extern Int SEG0;
#endif

#include "tiesr_config.h"
#include "mfcc_f_user.h"
#include "pack_user.h"
#include "load_user.h"

//char *malloc( int );

#include "my_fileio.h"

size_t free_memory(void); // FL: debug
#ifndef __HUGE_MODEL__
#define PRINT_FREE_MEM ( printf("free memory = %u\n", free_memory()) )
#else
#define PRINT_FREE_MEM ( printf("free memory = %lu\n", free_memory()) )
#endif


/*--------------------------------*/
TIESRENGINECOREAPI_API TIesrEngineStatusType load_scales( char *fname, gmhmm_type *gv )
{
   FILE  *fp;
   size_t nread;
   
   fp = my_fopen(fname, "rb");
   if (!fp)
   {
      PRT_ERR(fprintf(stderr, "can't open %s\n", fname));
      PRT_ERR(exit(0));
      return eTIesrEngineModelFileFail;
   }
   
   
   gv->scale_mu = (short *) malloc( sizeof(short) * gv->n_mfcc * 2 );
   if( gv->scale_mu == NULL )
   {
      my_fclose(fp);
      return eTIesrEngineModelFileFail;
   }
   
   
   nread = my_fread(gv->scale_mu, sizeof(short), gv->n_mfcc*2, fp);
   if( nread != (size_t)gv->n_mfcc*2 )
   {
      free(gv->scale_mu);
      gv->scale_mu = (short*)NULL;
      my_fclose(fp);
      return eTIesrEngineModelFileFail;
   }
   
   
   gv->scale_var =(short *) malloc( sizeof(short) * gv->n_mfcc * 2 );
   if( gv->scale_var == NULL )
   {
      my_fclose(fp);
      return eTIesrEngineModelFileFail;
   }
   
   
   nread = my_fread( gv->scale_var, sizeof(short), gv->n_mfcc*2, fp);
   if( nread != (size_t)gv->n_mfcc*2 )
   {
      free(gv->scale_var);
      gv->scale_var = (short*) NULL;
      my_fclose(fp);
      return eTIesrEngineModelFileFail;
   }
   
   
   fclose( fp );
   return eTIesrEngineSuccess;
}


#ifndef NO_FILE_IO // FL: remove file I/O
/*--------------------------------*/
/*
 ** return 0 if file open fail
 */
TIESRENGINECOREAPI_API unsigned short load_n_mfcc( char *fname )
{
   FILE  *fp;
   unsigned short n_mfcc = 0;
   
   fp = my_fopen(fname, "rb");
   if (!fp)
   {
      PRT_ERR(fprintf(stderr, "can't open %s\n", fname));
      return n_mfcc;
   }
   
   my_fread(&n_mfcc, sizeof(unsigned short), 1, fp);
   my_fclose( fp );
   return n_mfcc;
}
#else
/*--------------------------------*/
/*
 ** return 0 if file open fail
 */
TIESRENGINECOREAPI_API unsigned short load_n_mfcc( Int16 *n_mfcc_data )
{
   unsigned short n_mfcc;
   
   n_mfcc = n_mfcc_data[0];

   return n_mfcc;
}
#endif


//#ifndef OWN_LOAD_FUNCTIONS
// EDIT: By Michael Schuster
// The user has to define his own load functions. 


/*----------------------------------------------------------------*/
/*
 ** unpack mean or variance vectors
 */

TIesrEngineStatusType 
unpacking(short *base, ushort nbr, short n_mfcc,FILE *fp, short scale[])
{
   int i;
   unsigned short tmp_sh[ MAX_DIM_MFCC16 ];
   short *vec;
   size_t nread;
   for (vec = base, i=0; i< nbr; i++, vec += n_mfcc * 2)
   {
      nread = my_fread(tmp_sh, sizeof(short), n_mfcc, fp);
      if( nread != (size_t)n_mfcc )
         return eTIesrEngineModelLoadFail;
      vector_unpacking(tmp_sh, vec, scale, n_mfcc);
   }
   //printf("----------n unpaced: %d\n", nbr);
   return eTIesrEngineSuccess;
}


/* ---------------------------------------------------------------------------
 load mu
 ---------------------------------------------------------------------- */
#ifndef NO_FILE_IO // FL: remove file I/O
//#ifndef C55x
short *load_mu( char *fname,  gmhmm_type *gv, fileConfiguration_t*  fileConfig )
{
   FILE *fp;
   unsigned int vec_size = gv->n_mfcc;
   short *mu; /* array of mean vectors  */
   size_t nread;
   
   
   /* TIesr Engine wants mean vectors as interleaved static/dynamic
    features in bytes if BIT8MEAN set.  Otherwise, it wants the
    vectors as non-interleaved shorts.  vec_size is the number of
    shorts to allocate per vector, assuming a short is two bytes. */
   #ifndef BIT8MEAN
   /* TIesr Engine will store means as shorts */
   vec_size *= 2;
   #endif
   
   
   fp = my_fopen(fname, "rb");
   if( !fp )
      return (short*) NULL;
   
   
   /* Read number of mean vectors */
   nread = my_fread(&(gv->n_mu), sizeof(unsigned short), 1, fp);
   if( nread != 1 )
   {
      my_fclose( fp );
      return (short*) NULL;
   }
   
   
   
   /* Allocate space for mean vectors in either short or byte depending
    on the setting of BIT8MEAN */
   mu = (short *) malloc( gv->n_mu * vec_size * sizeof(short) );
   if( !mu )
   {
      my_fclose(fp);
      return (short*) NULL;
   }
   
   
   /* TIesr Engine wants mean vectors as interleaved static/dynamic
    features in bytes if BIT8MEAN set */
   #ifdef BIT8MEAN
   
   if( fileConfig->byteMu )
   {
      /* file mu is already in proper format, read in directly */
      nread = my_fread(mu, sizeof(short), gv->n_mu * vec_size, fp);
      my_fclose( fp );
      if( nread != gv->n_mu*vec_size )
      {
         free( mu );
         return (short*) NULL;
      }
   }
   else
   {
      /* The file mu data is in non-interleaved shorts.  TIesr Engine
       does not now have the code to convert from short to
       interleaved byte format.  It could be put in TIesr Engine.
       See TIesrFlex for the conversion code.  If that is done,
       then a call will be put here to convert. */
      my_fclose( fp );
      free( mu );
      return (short*) NULL;
   }
   
   
   /* TIesr Engine wants mean vectors as non-interleaved short features
    with static features first followed by dynamic features if
    BIT8MEAN is not set. */
   #else
   
   if( fileConfig->byteMu )
   {
      /* Input data is in interleaved static/dyanamic bytes.  Unpack these
       into non-interleaved short vectors */
      TIesrEngineStatusType status;
      status = unpacking(mu, gv->n_mu, gv->n_mfcc, fp, gv->scale_mu);
      my_fclose( fp );
      if( status != eTIesrEngineSuccess )
      {
         free( mu );
         return (short*) NULL;
      }
      
   }
   
   else
   {
      /* Input data is already in non-interleaved short features,
       read directly */
      nread = my_fread(mu, sizeof(short), gv->n_mu * vec_size, fp);
      my_fclose( fp );
      if( nread != gv->n_mu*vec_size )
      {
         free(mu);
         return (short*) NULL;
      } 
   }
   
   #endif
   
   /* The loading was a success, return the mean vector location. */
   return mu;
}

// #endif //C55x

#else
//#ifndef C55x
short *load_mu( short *mu_data,  gmhmm_type *gv, fileConfiguration_t*  fileConfig )
{
   FILE *fp;
   unsigned int vec_size = gv->n_mfcc;
   short *mu; /* array of mean vectors  */
   size_t nread;
   
   
   /* TIesr Engine wants mean vectors as interleaved static/dynamic
    features in bytes if BIT8MEAN set.  Otherwise, it wants the
    vectors as non-interleaved shorts.  vec_size is the number of
    shorts to allocate per vector, assuming a short is two bytes. */
   #ifndef BIT8MEAN
   /* TIesr Engine will store means as shorts */
   vec_size *= 2;
   #endif   
   
   /* Read number of mean vectors */
   gv->n_mu = mu_data[0];
   
#ifndef DSPBIOS_BUILD // FL: use DSP/BIOS heap
   /* Allocate space for mean vectors in either short or byte depending
    on the setting of BIT8MEAN */
   mu = (short *) malloc( gv->n_mu * vec_size * sizeof(short) );
   if( !mu )
   {
      return (short*) NULL;
   }

#else
   /* Allocate space for mean vectors in either short or byte depending
    on the setting of BIT8MEAN */
   mu = (short *)MEM_alloc(SEG0, gv->n_mu * vec_size * sizeof(short), 0);
   if(mu == MEM_ILLEGAL)
   {
      return (short*) NULL;
   }

#endif
   
   /* TIesr Engine wants mean vectors as interleaved static/dynamic
    features in bytes if BIT8MEAN set */
   #ifdef BIT8MEAN
   
   if( fileConfig->byteMu )
   {
      /* file mu is already in proper format, read in directly */
      nread = my_fread(mu, sizeof(short), gv->n_mu * vec_size, fp);
      my_fclose( fp );
      if( nread != gv->n_mu*vec_size )
      {
         free( mu );
         return (short*) NULL;
      }
   }
   else
   {
      /* The file mu data is in non-interleaved shorts.  TIesr Engine
       does not now have the code to convert from short to
       interleaved byte format.  It could be put in TIesr Engine.
       See TIesrFlex for the conversion code.  If that is done,
       then a call will be put here to convert. */
      my_fclose( fp );
      free( mu );
      return (short*) NULL;
   }
   
   
   /* TIesr Engine wants mean vectors as non-interleaved short features
    with static features first followed by dynamic features if
    BIT8MEAN is not set. */
   #else
   
   if( fileConfig->byteMu )
   {
#if 0 // FL:  unsupported
      /* Input data is in interleaved static/dyanamic bytes.  Unpack these
       into non-interleaved short vectors */
      TIesrEngineStatusType status;
      status = unpacking(mu, gv->n_mu, gv->n_mfcc, fp, gv->scale_mu);
      my_fclose( fp );
      if( status != eTIesrEngineSuccess )
      {
         free( mu );
         return (short*) NULL;
      }
#endif

#ifndef DSPBIOS_BUILD // FL: use DSP/BIOS heap
      free( mu );
#else
      MEM_free(SEG0, mu, gv->n_mu * vec_size);
#endif
      return (short*) NULL;
   }
   
   else
   {
      memcpy(mu, &mu_data[1], gv->n_mu * vec_size);
   }
   
   #endif
   
   /* The loading was a success, return the mean vector location. */
   return mu;
}

#endif

/* ---------------------------------------------------------------------------
 load var
 ---------------------------------------------------------------------- */
#ifndef NO_FILE_IO // FL: remove file I/O
static short *load_var( char *fname, gmhmm_type *gv, fileConfiguration_t *fileConfig )
{
   FILE               *fp;
   short *base_var; /* inverse of variance */
   size_t nread;
   
   /* Open binary inverse variances file or failure */
   fp = my_fopen(fname, "rb");
   if( !fp )
   {
      return (short*) NULL;
   }
   
   
   /* Load number of inverse variance vectors */
   nread = my_fread(&(gv->n_var), sizeof(unsigned short), 1, fp);
   if( nread != 1 )
   {
      my_fclose( fp );
      return (short*) NULL;
   }
   
   
   /* TIesr Engine wants inv variance vectors as interleaved static/dynamic
    features in bytes if BIT8VAR is set.  Otherwise, it wants the
    vectors as non-interleaved shorts.  Allocate space to hold variance
    based on the desired size, assuming short is two bytes. */
   #ifdef BIT8VAR
   base_var = (short *) malloc( gv->n_var * gv->n_mfcc * sizeof(short) );
   #else
   base_var = (short *) malloc( gv->n_var * 2 * gv->n_mfcc * sizeof(short) );
   #endif
   if( ! base_var )
   {
      my_fclose(fp);
      return  (short*) NULL;
   }
   
   /* TIesr Engine wants inv variance vectors as interleaved static/dynamic
    features in bytes if BIT8VAR is set */
   #ifdef BIT8VAR
   
   if( fileConfig->byteVar )
   {
      /* Input file inv variance is already in byte format, read directly. */
      nread = my_fread(base_var, sizeof(short), gv->n_var * gv->n_mfcc, fp);
      my_fclose(fp);
      if( nread != (unsigned int)(gv->n_var*gv->n_mfcc) )
      {
         free(base_var);
         return NULL;
      }
   }
   else
   {
      /* The file inv variance data is in non-interleaved shorts.
       TIesr Engine does not now have the code to convert from short
       to interleaved byte format.  It could be put in TIesr Engine.
       See TIesrFlex for the conversion code.  If that is done, then
       a call will be put here to convert. */
      my_fclose( fp );
      free( base_var );
      return NULL;
   }
   
   
   /* TIesr Engine wants inv variance vectors as non-interleaved short
    features with static features first followed by dynamic features
    if BIT8VAR is not set. */
   #else
   
   if( fileConfig->byteVar )
   {
      /* Input data is in interleaved static/dyanamic bytes.  Unpack these
       into non-interleaved short vectors */
      TIesrEngineStatusType status;
      status = unpacking(base_var, gv->n_var, gv->n_mfcc, fp, gv->scale_var);
      my_fclose( fp );
      if( status != eTIesrEngineSuccess )
      {
         free( base_var );
         return (short*) NULL;
      }
   }
   
   else
   {
      /* Input data is already in non-interleaved short features,
       read directly */
      nread = my_fread(base_var, sizeof(short), gv->n_var * 2 * gv->n_mfcc, fp);
      my_fclose( fp );
      if( nread != (size_t)gv->n_var * 2 * gv->n_mfcc )
      {
         free(base_var);
         return (short*) NULL;
      }
   }
   
   #endif
   
   /* Loading of variance vectors was successful */
   return base_var;
}

#else
static short *load_var( short *var_data, gmhmm_type *gv, fileConfiguration_t *fileConfig )
{
   FILE               *fp;
   short *base_var; /* inverse of variance */
   size_t nread;
      
   gv->n_var = var_data[0];
   
   /* TIesr Engine wants inv variance vectors as interleaved static/dynamic
    features in bytes if BIT8VAR is set.  Otherwise, it wants the
    vectors as non-interleaved shorts.  Allocate space to hold variance
    based on the desired size, assuming short is two bytes. */
   #ifdef BIT8VAR
   base_var = (short *) malloc( gv->n_var * gv->n_mfcc * sizeof(short) );
   if( ! base_var )
   {
      return  (short*) NULL;
   }
   #else
#ifndef DSPBIOS_BUILD // FL: use DSP/BIOS heap
   base_var = (short *) malloc( gv->n_var * 2 * gv->n_mfcc * sizeof(short) );
   if( ! base_var )
   {
      return  (short*) NULL;
   }

#else
   base_var = (short *)MEM_alloc(SEG0, gv->n_var * 2 * gv->n_mfcc * sizeof(short), 0);
   if(base_var == MEM_ILLEGAL)
   {
      return  (short*) NULL;
   }

   #endif

   #endif
   
   /* TIesr Engine wants inv variance vectors as interleaved static/dynamic
    features in bytes if BIT8VAR is set */
   #ifdef BIT8VAR
   
   if( fileConfig->byteVar )
   {
      /* Input file inv variance is already in byte format, read directly. */
      nread = my_fread(base_var, sizeof(short), gv->n_var * gv->n_mfcc, fp);
      my_fclose(fp);
      if( nread != (unsigned int)(gv->n_var*gv->n_mfcc) )
      {
         free(base_var);
         return NULL;
      }
   }
   else
   {
      /* The file inv variance data is in non-interleaved shorts.
       TIesr Engine does not now have the code to convert from short
       to interleaved byte format.  It could be put in TIesr Engine.
       See TIesrFlex for the conversion code.  If that is done, then
       a call will be put here to convert. */
      my_fclose( fp );
      free( base_var );
      return NULL;
   }
   
   
   /* TIesr Engine wants inv variance vectors as non-interleaved short
    features with static features first followed by dynamic features
    if BIT8VAR is not set. */
   #else
   
   if( fileConfig->byteVar )
   {
#if 0 // FL: unsupported
      /* Input data is in interleaved static/dyanamic bytes.  Unpack these
       into non-interleaved short vectors */
      TIesrEngineStatusType status;
      status = unpacking(base_var, gv->n_var, gv->n_mfcc, fp, gv->scale_var);
      my_fclose( fp );
      if( status != eTIesrEngineSuccess )
      {
         free( base_var );
         return (short*) NULL;
      }
#endif

#ifndef DSPBIOS_BUILD // FL: use DSP/BIOS heap
      free( base_var );
#else
      MEM_free(SEG0, base_var, gv->n_var * 2 * gv->n_mfcc);
#endif
      return (short*) NULL;
   }
   
   else
   {
      memcpy(base_var, &var_data[1], gv->n_var * 2 * gv->n_mfcc);
   }
   
   #endif
   
   /* Loading of variance vectors was successful */
   return base_var;
}
#endif



/*--------------------------------*/
/* Testing only */
/*
 #define TESTIT
 */

#ifdef TESTIT
/*
 ** access to transition probabilities
 */
#define get_pi_gvn_trans(trans) ((short *)(trans+1))
#define nbr_states_gvn_trans(trans_ptr) (*(unsigned short *)(trans_ptr))
#define trans_size(trans_ptr) (nbr_states_gvn_trans(trans_ptr) *  nbr_states_gvn_trans(trans_ptr))

#define get_a_gvn_trans(trans)    (trans+nbr_states_gvn_trans(trans))
#define get_ai_gvn_trans(trans, i) ( get_a_gvn_trans(trans)+(nbr_states_gvn_trans(trans)*(i)))
#define get_aij_gvn_trans(trans, i, j) (get_ai_gvn_trans(trans, i)[j])

#define SCALE 2
#define MINV -32768

void trans_weight(short *trans_pool, unsigned short len)
{
   unsigned short n, nbr_stts, i, j;
   short *ptr;
   int  int_aij;
   
   for (n = 0, ptr = trans_pool; ptr < trans_pool + len; ptr += trans_size(ptr), n++)
   {
      nbr_stts = nbr_states_gvn_trans(ptr);
      printf("[%3d] %d\n", n, nbr_stts);
      /*     for (i=0; i<nbr_stts - 1; i++) printf("%5d ", get_pi_gvn_trans(ptr)[i]);  printf("\n"); */
      for (i=0; i<nbr_stts - 1; i++)
      {
         for (j=0; j<nbr_stts; j++)
         {
            printf("%5d ", get_aij_gvn_trans(ptr, i, j));
            int_aij =  SCALE * get_aij_gvn_trans(ptr, i, j);
            get_aij_gvn_trans(ptr, i, j) =  MAX(MINV, int_aij);
            printf("(%5d) ", get_aij_gvn_trans(ptr, i, j));
         }
         printf("\n");
      }
   }
}

#endif



/* ---------------------------------------------------------------------------
 load obs_scr
 ---------------------------------------------------------------------- */
/* -- used Gaussian level cache is enough. no need for pdf level

 static TIesrEngineStatusType
 load_obs_scr( char *fname, gmhmm_type *gv )
 {
 FILE               *fp;
 unsigned short     len;

 fp = my_fopen(fname, "rb");
 if( !fp )
 return eTIesrEngineModelLoadFail;

 my_fread(&len, sizeof(unsigned short), 1, fp);

 gv->obs_scr = (short *) malloc( len * sizeof(short) );
 if( ! gv->obs_scr )
 {
 fclose(fp);
 return eTIesrEngineModelLoadFail;
 }

 //   my_fread(obs_scr, sizeof(short), len, fp);


 fclose( fp );
 return eTIesrEngineSuccess;
 }
 */

/*--------------------------------*/
/*
 ** generic load
 */
#ifndef NO_FILE_IO // FL: remove file I/O
TIESRENGINECOREAPI_API short *load_model_file(char fname[], unsigned short *len)
{
   FILE *fp;
   short *ptr;
   size_t nread;
   
   
   fp = my_fopen(fname, "rb");
   if( !fp )
      return (short*) NULL;
   
   my_fread(len, sizeof(unsigned short), 1, fp);
   
   ptr = (short *) malloc((*len) * sizeof(short) );
   if( !ptr )
   {
      my_fclose(fp);
      return (short*) NULL;
   }
   
   nread = my_fread(ptr, sizeof(short), *len, fp);
   my_fclose(fp);
   if( nread != *len )
   {
      free(ptr);
      return (short*) NULL;
   }
   
   return ( ptr );
}

#else
TIESRENGINECOREAPI_API short *load_model_file(short *model_data, unsigned short *len)
{
   short *ptr;
   
      
   *len = model_data[0];

#ifndef DSPBIOS_BUILD // FL: use DSP/BIOS heap
   ptr = (short *) malloc((*len) * sizeof(short) );
   if( !ptr )
   {
      return (short*) NULL;
   }

#else
   ptr = (short *)MEM_alloc(SEG0, *len * sizeof(short), 0);
   if(ptr == MEM_ILLEGAL)
   {
      return (short*) NULL;
   }
#endif
   
   memcpy(ptr, &model_data[1], *len);
   
   return ( ptr );
}

#endif


/* ---------------------------------------------------------------------------
 load word list
 ---------------------------------------------------------------------- */
#ifndef OWN_LOAD_FUNCTIONS
TIesrEngineStatusType
load_word_list( char *fname, gmhmm_type *gv )
{
   FILE      *fp;
   short     i;
   int size;
   char *p_char;
   
   if ( gv->trans->n_word == 0)
      return eTIesrEngineSuccess;
   
   fp = my_fopen(fname, "r");
   if( !fp )
      return eTIesrEngineModelLoadFail;
     
   size = my_fsize(fp); /* count */
   
   gv->vocabulary = (char **) malloc( gv->trans->n_word * sizeof(char *) );
   if( ! gv->vocabulary )
   {
      my_fclose(fp);
      return eTIesrEngineModelLoadFail;
   }
   
   p_char = (char *) malloc( size * sizeof(char));
   if( ! p_char )
   {
      free( gv->vocabulary );
      gv->vocabulary =(char **) NULL;
      my_fclose(fp);
      return eTIesrEngineModelLoadFail;
   }
   
   
   my_fread(p_char, sizeof(char), size, fp);
   my_fclose( fp );
   
   for (i = 0; i < gv->trans->n_word; i++)
   {
      gv->vocabulary[ i ] = p_char;
      while (*p_char != '\n') p_char++;
      *p_char = '\0'; /* replace line-feed by string end */
      p_char++;
   }
   
   return eTIesrEngineSuccess;
   
}

#else
#ifndef NO_FILE_IO  // FL: remove file I/O
TIesrEngineStatusType
load_word_list( char *fname, gmhmm_type *gv )
{
   FILE      *fp;
   short     i;
   int size;
   char *p_char;

   if ( gv->trans->n_word == 0)
      return eTIesrEngineSuccess;

   fp = my_fopen(fname, "r");
   if( !fp )
      return eTIesrEngineModelLoadFail;

   size = my_fsize(fp); /* count */

   gv->vocabulary = (char **) malloc( gv->trans->n_word * sizeof(char *) );
   if( ! gv->vocabulary )
   {
      my_fclose(fp);
      return eTIesrEngineModelLoadFail;
   }

   p_char = (char *) malloc( size * sizeof(char));
   if( ! p_char )
   {
      free( gv->vocabulary );
      gv->vocabulary =(char **) NULL;
      my_fclose(fp);
      return eTIesrEngineModelLoadFail;
   }


   fread(p_char, sizeof(char), size, fp);
   my_fclose( fp );

   for (i = 0; i < gv->trans->n_word; i++)
   {
      gv->vocabulary[ i ] = p_char;
      while (*p_char != '\n') p_char++;
      *p_char = '\0'; /* replace line-feed by string end */
      p_char++;
   }

   return eTIesrEngineSuccess;
}

#else
TIesrEngineStatusType
load_word_list( short *word_list, gmhmm_type *gv )
{
   short     i;
   int size;
   char *p_char;

   if ( gv->trans->n_word == 0)
      return eTIesrEngineSuccess;

   size = word_list[0]; // FL: added size as first 16-bit word

#ifndef DSPBIOS_BUILD // FL: use DSP/BIOS heap
   gv->vocabulary = (char **) malloc( gv->trans->n_word * sizeof(char *) );
   if( ! gv->vocabulary )
   {
      return eTIesrEngineModelLoadFail;
   }

   p_char = (char *) malloc( size * sizeof(char));
   if( ! p_char )
   {
      free( gv->vocabulary );
      gv->vocabulary =(char **) NULL;
      return eTIesrEngineModelLoadFail;
   }
#else
   gv->vocabulary = (char **)MEM_alloc(SEG0, gv->trans->n_word*sizeof(char *), 0);
   if(gv->vocabulary == MEM_ILLEGAL )
   {
      return eTIesrEngineModelLoadFail;
   }

   p_char = (char *)MEM_alloc(SEG0, size * sizeof(char), 0);
   if(p_char == MEM_ILLEGAL)
   {
      MEM_free(SEG0, gv->vocabulary, gv->trans->n_word);
      gv->vocabulary =(char **) NULL;
      return eTIesrEngineModelLoadFail;
   }
#endif

   memcpy(p_char, &word_list[1], size);

   for (i = 0; i < gv->trans->n_word; i++)
   {
      gv->vocabulary[ i ] = p_char;
      while (*p_char != '\n') p_char++;
      *p_char = '\0'; /* replace line-feed by string end */
      p_char++;
   }

   return eTIesrEngineSuccess;
}
#endif

#endif

/*--------------------------------*/
TIESRENGINECOREAPI_API void free_scales(gmhmm_type *gv)
{
   if( gv->scale_mu )
      free(  gv->scale_mu );
   if( gv->scale_var )
      free(  gv->scale_var );
}


/*--------------------------------*/
TIESRENGINECOREAPI_API void free_models(gmhmm_type *gv)
{
   if( gv->base_net )
      free( gv->base_net );
   if( gv->base_hmms )
      free( gv->base_hmms );
   if( gv->base_mu )
      free( gv->base_mu );
   //
   if( gv->gauss_scr )
      free( gv->gauss_scr);
   if( gv->base_mu_orig )
      free( gv->base_mu_orig );
   if( gv->base_var )
      free( gv->base_var );
   #ifdef USE_SVA
   if( gv->base_var_orig )
      free( gv->base_var_orig );
   #endif
   if( gv->base_tran )
      free( gv->base_tran );
   //   if( gv->obs_scr )
   //      free( gv->obs_scr );
   if( gv->base_pdf )
      free( gv->base_pdf );
   if( gv->base_mixture )
      free( gv->base_mixture );
   if( gv->base_gconst )
      free( gv->base_gconst );
   
   free_scales(gv);
   
   if (gv->vocabulary)
   {
      if( gv->vocabulary[0] )
         free(gv->vocabulary[0]);
      free(gv->vocabulary);
   }
}


/*----------------------------------------------------------------
 load_model_config

 This function reads the configuration of the binary model data
 files.  In this way, the recognizer can adjust for various
 configurations of model data, such as means and/or variances in
 short or byte.

 ----------------------------------------------------------------*/
#ifndef NO_FILE_IO // FL: remove file I/O
TIesrEngineStatusType load_model_config(char fname[],
fileConfiguration_t *fileConfig )
{
   FILE *fp;
   size_t nread;
   
   
   fp = my_fopen(fname, "rb");
   
   
   /* If configuration file is not available, assume the last assumption for
    creating models, that is, the models have byte mean and variance. */
   if( !fp )
   {
      fileConfig->byteMu = 1;
      fileConfig->byteVar = 1;
      
      return eTIesrEngineSuccess;
   }
   
   /* Read configuration parameters into configuration structure */
   nread = my_fread( &fileConfig->byteMu, sizeof(short), 1, fp);
   nread = my_fread( &fileConfig->byteVar, sizeof(short), 1, fp);
   my_fclose(fp);
   
   
   /* Check for enough data to read last parameter */
   if( nread != 1 )
      return eTIesrEngineModelLoadFail;
   
   return eTIesrEngineSuccess;
}

#else
TIesrEngineStatusType load_model_config(short *model_config_data,
fileConfiguration_t *fileConfig )
{
   fileConfig->byteMu = model_config_data[0];
   fileConfig->byteVar = model_config_data[1];
   
   return eTIesrEngineSuccess;
}

#endif

#ifndef NO_FILE_IO // FL: remove file I/O
/* load mean vectors to either base mean for decoding or original mean for adaptation
 @param mod_dir pointer to model directory
 @param gv pointer to gmhmm_type
 @bLoadBaseMu TRUE if load to base mean, FALSE if load to original mean */
TIESRENGINECOREAPI_API
TIesrEngineStatusType load_mean_vec(char * mod_dir, gmhmm_type * gv ,
Boolean bLoadBaseMu )
{
   char fname[ MAX_STR ];
   TIesrEngineStatusType status;
   fileConfiguration_t fileConfig;
   
   /* Read the configuration of input file model data.  For now, if no
    configuration file is available, byte means and variances are
    assumed, since this was the last way we were assuming data was
    written to binary model files. */
   sprintf(fname, "%s%s", mod_dir, file_names[11]);
   status = load_model_config( fname, &fileConfig );
   if( status != eTIesrEngineSuccess )
      return eTIesrEngineModelLoadFail;
   
   sprintf( fname, "%s%s", mod_dir, file_names[3] );
   if (gv->base_mu == NULL && bLoadBaseMu)
   {
      gv->base_mu = load_mu( fname, gv, &fileConfig );
      if( ! gv->base_mu )
         return eTIesrEngineModelLoadFail;
   }
   
   /* load mu again for PMC */
   if (gv->base_mu_orig == NULL && bLoadBaseMu == FALSE)
   {
      gv->base_mu_orig = load_mu( fname, gv, &fileConfig );
      if( ! gv->base_mu_orig )
         return eTIesrEngineModelLoadFail;
   }
   return eTIesrEngineSuccess;
}

#else
/* load mean vectors to either base mean for decoding or original mean for adaptation
 @param mod_dir pointer to model directory
 @param gv pointer to gmhmm_type
 @bLoadBaseMu TRUE if load to base mean, FALSE if load to original mean */
TIESRENGINECOREAPI_API
TIesrEngineStatusType load_mean_vec(gmhmm_type * gv ,
Boolean bLoadBaseMu )
{
   TIesrEngineStatusType status;
   fileConfiguration_t fileConfig;
   
   /* Read the configuration of input file model data.  For now, if no
    configuration file is available, byte means and variances are
    assumed, since this was the last way we were assuming data was
    written to binary model files. */
   status = load_model_config( grammar_data[11], &fileConfig );
   if( status != eTIesrEngineSuccess )
      return eTIesrEngineModelLoadFail;
   
   if (gv->base_mu == NULL && bLoadBaseMu)
   {
      gv->base_mu = load_mu( grammar_data[3], gv, &fileConfig );
      if( ! gv->base_mu )
         return eTIesrEngineModelLoadFail;
   }
   
   /* load mu again for PMC */
   if (gv->base_mu_orig == NULL && bLoadBaseMu == FALSE)
   {
      gv->base_mu_orig = load_mu( grammar_data[3], gv, &fileConfig );
      if( ! gv->base_mu_orig )
         return eTIesrEngineModelLoadFail;
   }
   return eTIesrEngineSuccess;
}
#endif


/*----------------------------------------------------------------*/
/*
 ** return the mfcc dimension of the feature, 0 if mfcc dimension file does not exist
 */
TIESRENGINECOREAPI_API
TIesrEngineStatusType load_models(char *mod_dir, gmhmm_type *gv, char bMonoNet,
char * network_file , char* word_list_file)
{
   char fname[ MAX_STR ];
   unsigned short len;
   TIesrEngineStatusType status;
   fileConfiguration_t fileConfig;
   
   
   /* Initialization of byte to short scale vectors */
   gv->scale_mu = (short*) NULL;
   gv->scale_var =(short*)  NULL;
   
   /* read the dimension of mfcc */
#ifndef NO_FILE_IO // FL: remove file I/O 
   sprintf(fname, "%s%s", mod_dir, file_names[9]);
   gv->n_mfcc = load_n_mfcc( fname );
#else
   gv->n_mfcc = load_n_mfcc(grammar_data[9]);
#endif
   if (gv->n_mfcc == 0)
      return  eTIesrEngineModelLoadFail;
   
   //PRINT_FREE_MEM; // FL: debug
   
   /* Read the configuration of input file model data.  For now, if no
    configuration file is available, byte means and variances are
    assumed, since this was the last way we were assuming data was
    written to binary model files. */
#ifndef NO_FILE_IO // FL: remove file I/O
   sprintf(fname, "%s%s", mod_dir, file_names[11]);
   status = load_model_config( fname, &fileConfig );
   if( status != eTIesrEngineSuccess )
      return eTIesrEngineModelLoadFail;
#else
   status = load_model_config( grammar_data[11], &fileConfig );
   if( status != eTIesrEngineSuccess )
      return eTIesrEngineModelLoadFail;
#endif
   
   //PRINT_FREE_MEM; // FL: debug

   /* Read the scaling of mean and variance if mu.bin or var.bin in byte. */
   if( fileConfig.byteMu || fileConfig.byteVar )
   {
#if 0
      sprintf(fname, "%s%s", mod_dir, file_names[10]);
      status = load_scales( fname, gv );
      if( status != eTIesrEngineSuccess )
         return eTIesrEngineModelLoadFail;
#endif
      // FL: currently unsupported
      return eTIesrEngineModelLoadFail;
   }
   else
   {
      gv->scale_mu = (short*) NULL;
      gv->scale_var = (short*) NULL;
   }
   
   //PRINT_FREE_MEM;  // FL: debug

   /*
    ** read top level grammar
    */
   if (network_file)
   {
#if 0
       gv->base_net =  load_model_file(network_file, &len);
      if( ! gv->base_net )
         return eTIesrEngineModelLoadFail;
      gv->trans = (TransType *) gv->base_net;
#endif
      // FL: currently unsupported
      return eTIesrEngineModelLoadFail;
   }else
   {
      if (!bMonoNet)
      {
#ifndef NO_FILE_IO // FL: remove file I/O
         sprintf(fname, "%s%s", mod_dir, file_names[1]);
         gv->base_net =  load_model_file(fname, &len);
#else
         gv->base_net =  load_model_file(grammar_data[1], &len);
#endif
         if( ! gv->base_net )
            return eTIesrEngineModelLoadFail;
         gv->trans = (TransType *) gv->base_net;
      }else
      {
         gv->base_net= (short*) NULL;
         gv->trans = (TransType *)  NULL;
      }
   }
   
   //PRINT_FREE_MEM; // FL: debug

   /* adjust address */
   /* C54 do it here, but cannot do it here for 32 bits CPU */
   
   /*
    ** load word list, after load_net()
    */
   if (word_list_file)
   {
#if 0
       status = load_word_list( word_list_file, gv );
      if( status != eTIesrEngineSuccess )
         return eTIesrEngineModelLoadFail;
#endif
      // FL: unsupported
      return eTIesrEngineModelLoadFail;
   }else if (!bMonoNet)
   {
#ifndef NO_FILE_IO // FL: remove file I/O
      sprintf(fname, "%s%s", mod_dir, file_names[0]);
      status = load_word_list( fname, gv );
#else
      status = load_word_list( grammar_data[0], gv );
#endif
      if( status != eTIesrEngineSuccess )
         return eTIesrEngineModelLoadFail;
   }else
   {
       gv->vocabulary = (char**) NULL;
   }
   
   //PRINT_FREE_MEM; // FL: debug

   /*
    ** read all HMM's used in the top level grammar
    */
   
#ifndef NO_FILE_IO // FL: remove file I/O
   sprintf(fname, "%s%s", mod_dir, file_names[2]);
   gv->base_hmms =  (unsigned short *)load_model_file(fname, &len);
#else
   gv->base_hmms =  (unsigned short *)load_model_file(grammar_data[2], &len);
#endif
   if( ! gv->base_hmms )
      return eTIesrEngineModelLoadFail;
   
   //PRINT_FREE_MEM; // FL: debug
   
   /* load mu, performing conversions as necessary */
#ifndef NO_FILE_IO // FL: remove file I/O
   sprintf( fname, "%s%s", mod_dir, file_names[3] );
   gv->base_mu = load_mu( fname, gv, &fileConfig );
#else
   gv->base_mu = load_mu( grammar_data[3], gv, &fileConfig );
#endif
   if( ! gv->base_mu )
      return eTIesrEngineModelLoadFail;
   
   //PRINT_FREE_MEM; // FL: debug

#ifndef DSPBIOS_BUILD // FL: use DSP/BIOS heap
   // Gaussian cache
   gv->gauss_scr =(short *) calloc( gv->n_mu , sizeof(short) );
   if ( !gv->gauss_scr )
   {
      return eTIesrEngineModelLoadFail;
   }

#else
   // Gaussian cache
   gv->gauss_scr =(short *)MEM_calloc(SEG0, gv->n_mu, 0);
   if (gv->gauss_scr == MEM_ILLEGAL)
   {
      return eTIesrEngineModelLoadFail;
   }

#endif
   
   //PRINT_FREE_MEM; // FL: debug
   
   /* load original mu for PMC */
   #ifdef USE_16BITMEAN_DECOD
   gv->base_mu_orig = (short*) NULL;
   /*
    the following code has been commented. Loading the original mu has
    been a process in cluster-dependent JAC, inside function of
    rj_compensate(gmhmm_type * gv) */
   #else
#ifndef NO_FILE_IO // FL: remove file I/O
   gv->base_mu_orig = load_mu( fname, gv, &fileConfig );
#else
   gv->base_mu_orig = load_mu( grammar_data[3], gv, &fileConfig );
   #endif
   if( ! gv->base_mu_orig )
      return eTIesrEngineModelLoadFail;
   #endif
   
   //PRINT_FREE_MEM; // FL: debug

#ifndef NO_FILE_IO // FL: remove file I/O
   /* load inverse variance vectors  */
   sprintf( fname, "%s%s", mod_dir, file_names[4] );
   gv->base_var = load_var( fname, gv, &fileConfig );
#else
   gv->base_var = load_var( grammar_data[4], gv, &fileConfig );
#endif
   if( ! gv->base_var )
      return eTIesrEngineModelLoadFail;
   
   //PRINT_FREE_MEM; // FL: debug

   #ifdef USE_SVA
   
   /* load var again for SVA */
   gv->base_var_orig = load_var( fname, gv, &fileConfig );
   if( ! gv->base_var_orig )
      return eTIesrEngineModelLoadFail;
   
   #endif
   
   //PRINT_FREE_MEM; // FL: debug
   
   /*
    ** transition matrix
    */
#ifndef NO_FILE_IO // FL: remove file I/O
   sprintf(fname, "%s%s", mod_dir, file_names[5]);
   gv->base_tran = load_model_file(fname, &len);
#else
   gv->base_tran = load_model_file(grammar_data[5], &len);
#endif
   if( ! gv->base_tran )
      return eTIesrEngineModelLoadFail;
   
   #ifdef TESTIT
   trans_weight(gv->base_tran, len);
   #endif
   
   //PRINT_FREE_MEM; // FL: debug

   /*
    ** PDF
    */
#ifndef NO_FILE_IO // FL: remove file I/O
   sprintf(fname, "%s%s", mod_dir, file_names[6]);
   gv->base_pdf = (unsigned short *)load_model_file(fname, &(gv->n_pdf));
#else
   gv->base_pdf = (unsigned short *)load_model_file(grammar_data[6], &(gv->n_pdf));
#endif
   if( ! gv->base_pdf )
      return eTIesrEngineModelLoadFail;

   //PRINT_FREE_MEM; // FL: debug

   /*
    ** observation score buffer, only compute once per pdf per frame
    */
   /* Gaussian cache is better ,and this is redendant
    status = load_obs_scr( fname, gv );
    if( status != eTIesrEngineSuccess )
    return eTIesrEngineModelLoadFail;
    */
   
   /*
    ** mixture
    */
#ifndef NO_FILE_IO // FL: remove file I/O
   sprintf(fname, "%s%s", mod_dir, file_names[7]);
   gv->base_mixture = load_model_file(fname, &len);
#else
   gv->base_mixture = load_model_file(grammar_data[7], &len);
#endif
   if( ! gv->base_mixture )
      return eTIesrEngineModelLoadFail;
   
   //PRINT_FREE_MEM; // FL: debug

   /*
    ** gconst
    */
#ifndef NO_FILE_IO // FL: remove file I/O
   sprintf(fname, "%s%s", mod_dir, file_names[8]);
   gv->base_gconst = load_model_file(fname, &len);
#else
   gv->base_gconst = load_model_file(grammar_data[8], &len);
#endif
   if( ! gv->base_gconst )
      return eTIesrEngineModelLoadFail;
   
   //PRINT_FREE_MEM; // FL: debug

   return eTIesrEngineSuccess;
}

/*-----------------------------------------------------------
 mem_alloc

 Manage allocation of a memory area that is viewed as an
 array of short data.  This function has the capability to
 align data pointers within the memory to long, int, or short
 boundaries.  It assumes sizes of short, int, and long are
 powers of two.
 ------------------------------------------------------------*/
pMemory mem_alloc( short * base_mem, ushort * mem_count,
   ushort size_in_short, unsigned int max_mem_usage,
   unsigned short aAlign, char *mesg )
{
#ifdef C55x 
    printf("\n DON'T USE THIS FUNCTION, IT'S BUGGY ON THE C55x\n");
    exit(0);
    return 0l;
#else
   unsigned int varLen;
   unsigned int shrtsPerVar;
   unsigned int shrtOffset;
   
   short *tmp =  base_mem + (*mem_count);
   
   
   /* Checks to ensure alignment of pointer is correct */
   if( aAlign == LONGALIGN || aAlign == INTALIGN )
   {
      varLen = (aAlign == LONGALIGN ) ?  sizeof(long) : sizeof(int);
      shrtsPerVar = varLen >> (SHORTLEN >> 1);
      shrtOffset = *mem_count & (shrtsPerVar - 1);

      /* Not aligned, align it to next short */
      if( shrtOffset )
      {
         tmp += shrtsPerVar - shrtOffset;
         *mem_count = (unsigned short)(tmp - base_mem);
      }
      
      /* Ensure request for exact number of shorts to fill all positions */
      shrtOffset = size_in_short & (shrtsPerVar-1);
      if( shrtOffset )
      {
         size_in_short += (shrtsPerVar - shrtOffset );
      }
   }
   
   (*mem_count) += size_in_short;
   PRT(printf("%5d words, %5d total/%d, in \"%s\"\n", size_in_short,
   *mem_count, max_mem_usage, mesg));
   
   if ((unsigned int) *mem_count > max_mem_usage)
   {
      PRT_ERR(printf("%5d words, %5d total/%d, in \"%s\"\n", size_in_short,
      *mem_count, max_mem_usage, mesg));
      PRT_ERR(fprintf(stderr, "maximum size capacity exceeded at \"%s\"\n", mesg));
      return (pMemory) 0;
   }
   
   return (pMemory)tmp;
#endif
}


