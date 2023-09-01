
#pragma CODE_SECTION(ALG_init, ".text:init")
#pragma CODE_SECTION(ALG_exit, ".text:exit")
#pragma CODE_SECTION(_ALG_allocMemory, ".text:create")
#pragma CODE_SECTION(_ALG_freeMemory, ".text:create")

#include <std.h>
#include <alg.h>
#include <ialg.h>

#include <stdlib.h>     /* malloc/free declarations */
#include <string.h>     /* memset declaration */


void *memalign54(size_t alignment, size_t size);
void free54(void *ptr);
#define myMemalign  memalign54
#define myFree      free54


Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n);
Void _ALG_freeMemory(IALG_MemRec memTab[], Int n);

/*
 *  ======== ALG_activate ========
 */
Void ALG_activate(ALG_Handle alg)
{
    /* restore all persistant shared memory */
        ;   /* nothing to do since memory allocation never shares any data */
    
    /* do app specific initialization of scratch memory */
    if (alg->fxns->algActivate != NULL) {
        alg->fxns->algActivate(alg);
    }
}

/*
 *  ======== ALG_deactivate ========
 */
Void ALG_deactivate(ALG_Handle alg)
{
    /* do app specific store of persistent data */
    if (alg->fxns->algDeactivate != NULL) {
        alg->fxns->algDeactivate(alg);
    }

    /* save all persistant shared memory */
        ;   /* nothing to do since memory allocation never shares any data */
    
}

/*
 *  ======== ALG_exit ========
 */
Void ALG_exit(Void)
{
}

/*
 *  ======== ALG_init ========
 */
Void ALG_init(Void)
{
}


/*
 *  ======== _ALG_allocMemory ========
 */
Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n)
{
    Int i;
    
    for (i = 0; i < n; i++) {
        memTab[i].base = myMemalign(memTab[i].alignment, memTab[i].size);

        if (memTab[i].base == NULL) {
            _ALG_freeMemory(memTab, i);
            return (FALSE);
        }
        memset(memTab[i].base, 0, memTab[i].size);
    }

    return (TRUE);
}

/*
 *  ======== _ALG_freeMemory ========
 */
Void _ALG_freeMemory(IALG_MemRec memTab[], Int n)
{
    Int i;
    
    for (i = 0; i < n; i++) {
        if (memTab[i].base != NULL) {
            myFree(memTab[i].base);
        }
    }
}

/*
 *  ======== memalign54 ========
 */
void *memalign54(size_t alignment, size_t size)
{
    void     **mallocPtr;
    void     **retPtr;

    /* return if invalid size value */ 
    if (size <= 0) {
       return (0);
    }

    /*
     * If alignment is not a power of two, return what malloc returns. This is
     * how memalign behaves on the c6x.
     */
    if ((alignment & (alignment - 1)) || (alignment <= 1)) {
        if( (mallocPtr = malloc(size + sizeof(mallocPtr))) != NULL ) {
            *mallocPtr = mallocPtr;
            mallocPtr++;
        }
        return ((void *)mallocPtr);
    }

    /* allocate block of memory */
    if ( !(mallocPtr = malloc(alignment + size)) ) { 
        return (0);
    }

    /* Calculate aligned memory address */ 
    retPtr = (void *)(((Uns)mallocPtr + alignment) & ~(alignment - 1));

    /* Set pointer to be used in the free54() fxn */
    retPtr[-1] = mallocPtr;

    /* return aligned memory */
    return ((void *)retPtr);
}

/*
 *  ======== free54 ========
 */
Void free54(void *ptr)
{
    free((void *)((void **)ptr)[-1]);
}
