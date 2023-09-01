#ifndef EQ_
#define EQ_

#include <alg.h>
#include <ialg.h>
#include <ieq.h>

#define EQ_PARAMS IEQ_PARAMS

#if 0
/*
 *  ======== EQ_create ========
 *  Create an instance of a EQ object.
 */
static inline IEQ_Handle EQ_create(const IEQ_Fxns *fxns, const EQ_Params *prms, int instance)
{
    return ((IEQ_Handle)ALG_create((IALG_Fxns *)fxns, NULL, (IALG_Params *)prms));
}

/*
 *  ======== EQ_delete ========
 *  Delete a EQ instance object
 */
static inline Void EQ_delete(IEQ_Handle handle)
{
    ALG_delete((ALG_Handle)handle);
}
#else

/*
 *  ======== EQ_create ========
 *  Create an instance of a EQ object.
 */
IEQ_Handle EQ_create(const IEQ_Fxns *dfxns, const EQ_Params *prms, int instance);

/*
 *  ======== EQ_delete ========
 *  Delete a EQ instance object
 */
Void EQ_delete(IEQ_Handle handle);

#endif


#endif    /* EQ */
