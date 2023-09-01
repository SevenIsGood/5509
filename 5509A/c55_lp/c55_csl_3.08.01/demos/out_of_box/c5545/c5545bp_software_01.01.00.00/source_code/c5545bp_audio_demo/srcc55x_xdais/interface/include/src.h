#ifndef SRC_
#define SRC_

#include <alg.h>
#include <ialg.h>
#include <isrc.h>

#define SRC_PARAMS ISRC_PARAMS

#if 0
/*
 *  ======== SRC_create ========
 *  Create an instance of a SRC object.
 */
static inline ISRC_Handle SRC_create(const ISRC_Fxns *fxns, const SRC_Params *prms)
{
    return ((ISRC_Handle)ALG_create((IALG_Fxns *)fxns, NULL, (IALG_Params *)prms));
}

/*
 *  ======== SRC_delete ========
 *  Delete a SRC instance object
 */
static inline Void SRC_delete(ISRC_Handle handle)
{
    ALG_delete((ALG_Handle)handle);
}

#else

/*
 *  ======== SRC_create ========
 *  Create an instance of a SRC object.
 */
ISRC_Handle SRC_create(const ISRC_Fxns *dfxns, const SRC_Params *prms, int instance);

/*
 *  ======== SRC_delete ========
 *  Delete a SRC instance object
 */
Void SRC_delete(ISRC_Handle handle);

#endif


#endif    /* SRC */
