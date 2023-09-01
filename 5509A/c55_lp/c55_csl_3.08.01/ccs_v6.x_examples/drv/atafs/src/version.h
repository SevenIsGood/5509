/* ============================================================================
 * Copyright (c) 2008-2012 Texas Instruments Incorporated.
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


/*********************************************************
Adds a version string to an object file that can be
read with the "vers" CCS command-line program.

The string is placed in a new section named "VERSION",
so that it can easily be moved in memory or discarded.

usage:
	VERSION(id, rev, tag)
where
	id is a module variable name prefix
	rev is a string, a module revision number
	tag is a string, a revision control tag
*********************************************************/

#ifndef _VERSION
#define _VERSION

/*
VERSION(id, rev, tag)
 Initialize an int array named <id>_version with a character string
 in the following format:
	Id:<id> Rev:<rev> Tag:<tag> Built: <date> <time>

 The rev and tag arguments are constant character strings.

 e.g. if "file.c" contains:
	VERSION(MP3_TI, "1.01", "RELEASE1");

 then "vers version.obj" will produce, e.g.:
        Id:MP3_TI Rev:1.01 Tag:RELEASE1 Built:Dec 18 2001 18:47:34
*/
#ifndef _TMS320C5XX
/* for every platform except C54x */
#define VERSION(id, rev, tag) \
	const char id##_version[] = "@(#)Id:" #id " Rev:" rev " Tag:" tag " Built:" __DATE__ " " __TIME__

#else
/* for C54x only */
#define VERSION(id, rev, tag) \
	extern const char id##_version[];\
	asm(" version_string _" #id "_version, \"Id:" #id " Rev:" rev " Tag:" tag " Built:" __DATE__ " " __TIME__ "\"")

/*
coff_string for C54x
 An assembly macro to initialize a string in DSP memory in a
 format that appears as a byte-wide character string in the
 COFF file on disk.
*/
asm("coff_string .macro str");
asm("  .var i");
asm("  .asg 1,i");
asm("  .loop $symlen(\":str:\")/2");
asm("    .field ':str(i+1,1):',8");
asm("    .field ':str(i,1):',8");
asm("    .eval i+2,i");
asm("  .endloop");
asm("  .if (i=$symlen(\":str:\"))");
asm("    .field 0,8");
asm("    .field ':str(i,1):',8");
asm("  .else");
asm("    .field 0,8");
asm("  .endif");
asm(" .endm");

/*
version_string
 Initialized an array named "name" with the character string "str",
 so that it can be read using the "vers" CCS command-line program.
*/
asm("version_string .macro name,str");
asm("  .sect \".const:VERSION\"");
asm("  .global :name:");
asm(":name::");
asm("  .string \"@(#):str:\"");
asm("  .byte 0");
asm("  coff_string \"@(#):str:\"");
asm("  .text");
asm("  .endm");
#endif

#endif
