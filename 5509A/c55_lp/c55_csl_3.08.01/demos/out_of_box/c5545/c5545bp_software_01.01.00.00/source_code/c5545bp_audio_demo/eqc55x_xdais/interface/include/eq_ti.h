

/*
 *  ======== EQ_ti.h ========
 *  Interface header for the EQ_TI module.
 */
#ifndef EQ_TI_
#define EQ_TI_

#include <ialg.h>
#include <ieq.h>

/*
 *  ======== EQ_TI_exit ========
 *  Required module finalization function.
 */
extern Void EQ_TI_exit(Void);

/*
 *  ======== EQ_TI_init ========
 *  Required module initialization function.
 */
extern Void EQ_TI_init(Void);

/*
 *  ======== EQ_TI_IALG ========
 *  TI's implementation of EQ's IALG interface
 */
extern IALG_Fxns EQ_TI_IALG; 

/*
 *  ======== EQ_TI_IEQ ========
 *  TI's implementation of EQ's IEQ interface
 */
extern const IEQ_Fxns EQ_TI_IEQ; 

#endif /*EQ_TI_ */
