/*----------------------------------------------------------------------
 *
 * my_fileio.h
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

#ifndef _MY_FILEIO_H
#define	_MY_FILEIO_H

#ifdef	__cplusplus
extern "C" {
#endif

// Reading from streams if used with own system implementation
size_t my_fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t my_fwrite(void *ptr, size_t size, size_t nmemb, FILE *stream);
FILE  *my_fopen(const char *path, const char *mode);
Int32  my_fclose(FILE *fp);
Int32  my_fseek(FILE *stream, long offset, int whence);
Int32  my_fsize(FILE* fp);

// You have to implement also this defined in load_user.h
// TIesrEngineStatusType load_word_list( char *fname, gmhmm_type *gv );
#ifdef	__cplusplus
}
#endif

#endif	/* _MY_FILEIO_H */

