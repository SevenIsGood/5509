/******************************************************************************
 * FILE PURPOSE: Defines libarary directory name using getLibs
 ******************************************************************************
 * FILE NAME: package.xs
 *
 * DESCRIPTION: This file defines the library directory name for proper build
 *              in case a different directory name for storing library files 
 *              other than "lib" is used. XDC by default assumes that the 
 *              library directory is "lib" is not sepcifically indicated by use
 *              the attributes in a file called package.xs  
 *
 * TABS: NONE
 *
 * Copyright (C) 2012, Texas Instruments, Inc.
 *****************************************************************************/

/* "mangle" program build attrs into an appropriate directory name */
function getLibs (prog)
{
  var suff = prog.build.target.suffix;
  var lib  = "";
  
  lib += "lib/mhm_c.a"  + suff;

  return (lib);
}

/* nothing past this point */
