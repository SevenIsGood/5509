/*
 *  Copyright 2001 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */
/* "@(#) XDAS 2.12 05-21-01 (__imports)" */
/*
 *  ======== ialg.h ========
 *!
 */
#ifndef IALG_
#define IALG_

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------*/
/*    TYPES AND CONSTANTS    */
/*---------------------------*/

#define IALG_DEFMEMRECS 4   /* default number of memory records */
#define IALG_OBJMEMREC  0   /* memory record index of instance object */
#define IALG_SYSCMD     256 /* minimum "system" IALG_Cmd value */

#define IALG_EOK        0   /* successful return status code */
#define IALG_EFAIL      -1  /* unspecified error return status code */

typedef enum IALG_MemAttrs {
    IALG_SCRATCH,           /* scratch memory */
    IALG_PERSIST,           /* persistent memory */
    IALG_WRITEONCE          /* write-once persistent memory */
} IALG_MemAttrs;

#define IALG_MPROG  0x0008  /* program memory space bit */
#define IALG_MXTRN  0x0010  /* external memory space bit */

/*
 *  ======== IALG_MemSpace ========
 */
typedef enum IALG_MemSpace {
    IALG_EPROG =            /* external program memory */
        IALG_MPROG | IALG_MXTRN,

    IALG_IPROG =            /* internal program memory */
        IALG_MPROG,

    IALG_ESDATA =           /* off-chip data memory (accessed sequentially) */
        IALG_MXTRN + 0,

    IALG_EXTERNAL =         /* off-chip data memory (accessed randomly) */
        IALG_MXTRN + 1,
    
    IALG_DARAM0 = 0,        /* dual access on-chip data memory */
    IALG_DARAM1 = 1,        /* block 1, if independant blocks required */

    IALG_SARAM  = 2,        /* single access on-chip data memory */
    IALG_SARAM0 = 2,        /* block 0, equivalent to IALG_SARAM */
    IALG_SARAM1 = 3,        /* block 1, if independant blocks required */

    IALG_DARAM2 = 4,        /* block 2, if a 3rd independent block required */
    IALG_SARAM2 = 5         /* block 2, if a 3rd independent block required */
} IALG_MemSpace;

/*
 *  ======== IALG_isProg ========
 */
#define IALG_isProg(s) (        \
    (((short)(s)) & IALG_MPROG)   \
)

/*
 *  ======== IALG_isOffChip ========
 */
#define IALG_isOffChip(s) (     \
    (((short)(s)) & IALG_MXTRN)   \
)

typedef struct IALG_MemRec {
    int             size;       /* size in MAU of allocation */
    int             alignment;  /* alignment requirement (MAU) */
    IALG_MemSpace   space;      /* allocation space */
    IALG_MemAttrs   attrs;      /* memory attributes */
    void            *base;      /* base address of allocated buf */
} IALG_MemRec;

/*
 *  ======== IALG_Obj ========
 *  Algorithm instance object definition
 *
 *  All XDAS algorithm instance objects *must* have this structure
 *  as their first element.  However, they do not need to initialize
 *  it; initialization of this sub-structure is done by the
 *  "framework".
 */
typedef struct IALG_Obj {
    struct IALG_Fxns *fxns;
} IALG_Obj;

/*
 *  ======== IALG_Handle ========
 *  Handle to an algorithm instance object
 */
typedef struct IALG_Obj *IALG_Handle;

/*
 *  ======== IALG_Params ========
 *  Algorithm instance creation parameters
 *
 *  All XDAS algorithm parameter structures *must* have a this 
 *  as their first element.
 */
typedef struct IALG_Params {
    int size;       /* number of MAU in the structure */
} IALG_Params;

/*
 *  ======== IALG_Status ========
 *  Pointer to algorithm specific status structure
 *
 *  All XDAS algorithm parameter structures *must* have a this 
 *  as their first element.
 */
typedef struct IALG_Status {
    int size;       /* number of MAU in the structure */
} IALG_Status;

/*
 *  ======== IALG_Cmd ========
 *  Algorithm specific command.  This command is used in conjunction
 *  with IALG_Status to get and set algorithm specific attributes
 *  via the algControl method.
 */
typedef unsigned short IALG_Cmd;

/*
 *  ======== IALG_Fxns ========
 *  This structure defines the fields and methods that must be supplied by
 *  all XDAS algorithms.
 *
 *      implementationId  - unique pointer that identifies the module
 *                          implementing this interface.
 *      algActivate()     - notification to the algorithm that its memory
 *                          is "active" and algorithm processing methods
 *                          may be called.  May be NULL; NULL => do nothing.
 *      algAlloc()        - apps call this to query the algorithm about
 *                          its memory requirements. Must be non-NULL.
 *      algControl()      - algorithm specific control operations.  May be
 *                          NULL; NULL => no operations supported.
 *      algDeactivate()   - notification that current instance is about to
 *                          be "deactivated".  May be NULL; NULL => do nothing.
 *      algFree()         - query algorithm for memory to free when removing
 *                          an instance.  Must be non-NULL.
 *      algInit()         - apps call this to allow the algorithm to
 *                          initialize memory requested via algAlloc().  Must
 *                          be non-NULL.
 *      algMoved()        - apps call this whenever an algorithms object or
 *                          any pointer parameters are moved in real-time.
 *                          May be NULL; NULL => object can not be moved.
 *      algNumAlloc()     - query algorithm for number of memory requests.
 *                          May be NULL; NULL => number of mem recs is less
 *                          then IALG_DEFMEMRECS.
 */
typedef struct IALG_Fxns {
    void    *implementationId;
    void    (*algActivate)(IALG_Handle);
    int     (*algAlloc)(const IALG_Params *, struct IALG_Fxns **, IALG_MemRec *);
    int     (*algControl)(IALG_Handle, IALG_Cmd, IALG_Status *);
    void    (*algDeactivate)(IALG_Handle);
    int     (*algFree)(IALG_Handle, IALG_MemRec *);
    int     (*algInit)(IALG_Handle, const IALG_MemRec *, IALG_Handle, const IALG_Params *);
    void    (*algMoved)(IALG_Handle, const IALG_MemRec *, IALG_Handle, const IALG_Params *);
    int     (*algNumAlloc)(void);
} IALG_Fxns;

#ifdef __cplusplus
}
#endif

#endif  /* IALG_ */
