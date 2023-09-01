/*
 *  ======== src.c ========
 *  SRC  Module - implements all functions and defines all constant
 *  structures common to all SRC  algorithm implementations.
 */

#pragma CODE_SECTION (SRC_init, ".text:init")
#pragma CODE_SECTION (SRC_exit, ".text:exit")

#include <std.h>
#include <alg.h>

#include "src.h"
 
Void	SRC_setInput			(ISRC_Handle handle, Int *in)
{	
	handle->fxns->setInput(handle,in);
}

Void	SRC_setOutput		(ISRC_Handle handle, Int *out)
{
	handle->fxns->setOutput(handle,out);
}

Void	SRC_apply			(ISRC_Handle handle)
{
	handle->fxns->apply(handle);
}
	

Bool	SRC_isInputBufferEmpty	(ISRC_Handle handle)
{
	handle->fxns->isInputBufferEmpty(handle);
}

Bool	SRC_isOutputBufferFull	(ISRC_Handle handle) 
{
	handle->fxns->isOutputBufferFull(handle);
}

Void	SRC_getStatus	(ISRC_Handle handle, ISRC_Status *status)
{
	handle->fxns->getStatus(handle,status);
}
	
Void	SRC_setStatus		(ISRC_Handle handle, const ISRC_Status *status)
{
	handle->fxns->setStatus(handle,status);
}

LgInt	SRC_getInputRate		(ISRC_Handle handle)	
{
	handle->fxns->getInputRate(handle);
}

LgInt	SRC_getOutputRate		(ISRC_Handle handle)	
{
	handle->fxns->getOutputRate(handle);
}

Void	SRC_setInputRate		(ISRC_Handle handle, LgInt inputRate)
{
	handle->fxns->setInputRate(handle,inputRate);
}

Void	SRC_setOutputRate		(ISRC_Handle handle, LgInt outputRate)
{
	handle->fxns->setOutputRate(handle,outputRate);
}	
	

 	
/*
 *  ======== SRC_exit ========
 *  SRC finalization
 */
Void SRC_exit()
 {
 }

/*
 *  ======== SRC_init ========
 *  SRC initialization
 */
Void SRC_init()
 {
 }

