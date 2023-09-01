/*----------------------------------------------------------------------
 *
 * my_fileio.c
 *
 * File I/O definitions for cases where standard I/O can not be used.
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

 ----------------------------------------------------------------------*/
#include "tiesr_config.h"


#ifndef OWN_LOAD_FUNCTIONS
#include <stdio.h>

size_t my_fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return fread(ptr,size,nmemb,stream);
}
size_t my_fwrite(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return fwrite(ptr,size,nmemb,stream);
}
FILE *my_fopen(const char *path, const char *mode)
{
    return fopen(path,mode);
}
Int32 my_fclose(FILE *fp)
{
    return fclose(fp);
}
Int32 my_fseek(FILE *stream, long offset, int whence)
{
    return fseek(stream, offset, whence);
}


/*--------------------------------*/
//
// MS WCE does not support rewind function
//
Int32 my_fsize(FILE* fp)
{
    unsigned int size=0;
    fseek(fp, 0, 2);
    size=ftell(fp); /* count */
    fseek(fp, 0L, SEEK_SET);
    clearerr(fp);
    return size;
}

#else

#include <stdio.h>

size_t my_fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	char buf[2];
	char *cptr = (char*)ptr;
	size_t nobj, nchar, ncount, nread = 0;

	for(nobj = 0; nobj < nmemb; nobj++ )
	{
		ncount = 0;
		for( nchar = 0; nchar < size; nchar++ )
		{
			ncount += fread(buf, 2, 1, stream );
			*cptr++ = ( (buf[0]<<8) | buf[1]);
		}
		if( ncount == size )
			nread++;
	}
	return nread; 
}

size_t my_fwrite(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	char buf[2];
	char *cptr = (char*)ptr;
	size_t nobj, nchar, ncount, nwrite = 0;
	for( nobj = 0; nobj < nmemb; nobj++ )
	{
		ncount = 0;
		for( nchar = 0; nchar < size; nchar++ )
		{
			buf[0] = ( (*cptr >> 8 ) & 0xff );
			buf[1] = (*cptr++ & 0xff );
			ncount += fwrite( buf, 2, 1, stream );
		}
		if( ncount == size )
			nwrite++;
	}
    return nwrite;
}

FILE *my_fopen(const char *path, const char *mode)
{
    return fopen(path,mode);
}
Int32 my_fclose(FILE *fp)
{
    return fclose(fp);
}
Int32 my_fseek(FILE *stream, long offset, int whence)
{
    return fseek(stream, offset, whence);
}


/*--------------------------------*/
//
// MS WCE does not support rewind function
//
Int32 my_fsize(FILE* fp)
{
    unsigned int size=0;
    fseek(fp, 0, 2);
    size=ftell(fp); /* count */
    fseek(fp, 0L, SEEK_SET);
    clearerr(fp);
    return size;
}

#endif


