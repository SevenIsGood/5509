/*
 *  Copyright 2001 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */
/* "@(#) XDAS 2.12 04-24-01 (__imports)" */
/*
 *  ======== alg.h ========
 */
#ifndef ALG_
#define ALG_

#include <ialg.h>

/*
 *  ======== ALG_Handle ========
 *  This handle type is used to reference all ALG instance objects
 */
typedef IALG_Handle ALG_Handle;

/*
 *  ======== ALG_activate ========
 *  Restore all shared persistant data associated with algorithm object
 */
extern void ALG_activate(ALG_Handle alg);

/*
 *  ======== ALG_control ========
 *  Control algorithm object
 */
extern int ALG_control(ALG_Handle alg, IALG_Cmd cmd, IALG_Status *sptr);

/*
 *  ======== ALG_create ========
 *  Create algorithm object and initialize its memory
 */
extern ALG_Handle ALG_create(IALG_Fxns *fxns, IALG_Handle p, IALG_Params *prms);

/*
 *  ======== ALG_deactivate ========
 *  Save all shared persistant data associated with algorithm object
 *  to some non-shared persistant memory.
 */
extern void ALG_deactivate(ALG_Handle alg);

/*
 *  ======== ALG_delete ========
 *  Delete algorithm object and release its memory
 */
extern void ALG_delete(ALG_Handle alg);

/*
 *  ======== ALG_exit ========
 *  Module finalization
 */
extern void ALG_exit(void);

/*
 *  ======== ALG_init ========
 *  Module initialization
 */
extern void ALG_init(void);

#endif  /* ALG_ */
