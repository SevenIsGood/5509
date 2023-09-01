
/*
 *  ======== SRC_ti.h ========
 *  Interface header for the SRC_TI module.
 */
#ifndef SRC_TI_
#define SRC_TI_

#include <ialg.h>
#include <isrc.h>

/*
 *  ======== SRC_TI_exit ========
 *  Required module finalization function.
 */
extern Void SRC_TI_exit(Void);

/*
 *  ======== SRC_TI_init ========
 *  Required module initialization function.
 */
extern Void SRC_TI_init(Void);

/*
 *  ======== SRC_TI_IALG ========
 *  TI's implementation of SRC's IALG interface
 */
extern IALG_Fxns SRC_TI_IALG; 

/*
 *  ======== SRC_TI_ISRC ========
 *  TI's implementation of SRC's ISRC interface
 */
extern const ISRC_Fxns SRC_TI_ISRC; 

#endif /* SRC_TNI_ */
