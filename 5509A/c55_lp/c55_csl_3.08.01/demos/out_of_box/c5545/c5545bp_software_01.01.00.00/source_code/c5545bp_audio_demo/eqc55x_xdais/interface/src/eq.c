
/*
 *  ======== eq.c ========
 *  EQ  Module - implements all functions and defines all constant
 *  structures common to all EQ  algorithm implementations.
 */

#pragma CODE_SECTION (EQ_init, ".text:init")
#pragma CODE_SECTION (EQ_exit, ".text:exit")

#include <std.h>
#include <alg.h>

#include <eq.h>
 
 
Void EQ_getStatus (IEQ_Handle handle, IEQ_Status *status)
{
	handle->fxns->getStatus(handle,status);
}


Void EQ_setStatus (IEQ_Handle handle,const IEQ_Status *status)
{
	handle->fxns->setStatus(handle,status);
}


Void	EQ_apply (IEQ_Handle handle, Int *in, Int *out)
{	
	handle->fxns->apply(handle,in,out);
}	


Void EQ_getGains(IEQ_Handle handle, Int *gains) 
{
	handle->fxns->getGains(handle,gains);
}

Void EQ_setGains(IEQ_Handle handle, Int  *gains) 
{
	handle->fxns->setGains(handle,gains);
}


Int EQ_getGain(IEQ_Handle handle, Int band) 
{
	handle->fxns->getGain(handle, band);
 return(0);
}


Void EQ_setGain(IEQ_Handle handle, Int i, Int gain) 
{
	handle->fxns->setGain(handle,i,gain);
}			
	

/*
Void EQ_reset(IEQ_Handle handle) {
	
	handle->fxns->reset(handle);
 }
*/ 
 
/*
 *  ======== Eq_exit ========
 *  Equalizer finalization
 */
Void EQ_exit()
 {
 }

/*
 *  ======== EQ_init ========
 *  Equalizer initialization
 */
Void EQ_init()
 {
 }

