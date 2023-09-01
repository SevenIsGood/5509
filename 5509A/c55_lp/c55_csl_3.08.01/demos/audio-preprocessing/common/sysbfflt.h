/* 
  sysbfflt.h: BF filter definitions for all angles of intereset
*/
#ifndef _SYSBFFLT_H
#define _SYSBFFLT_H

///#include <xdc/std.h>                /* required for types.h */
#include <ti/mas/types/types.h>

/*======================================================================
 * Filter angles that are supported
 *======================================================================*/

#define SYS_BF_ANGLE_P0     (0)
#define SYS_BF_ANGLE_P60    (1)
#define SYS_BF_ANGLE_P90    (2)
#define SYS_BF_ANGLE_P120    (3)
#define SYS_BF_ANGLE_P180   (4)
#define SYS_BF_ANGLE_M120    (5)
#define SYS_BF_ANGLE_M90    (6)
#define SYS_BF_ANGLE_M60    (7)
#define SYS_BF_ANGLE_LAST   SYS_BF_ANGLE_M60
#define SYS_BF_ANGLES_MAX   (SYS_BF_ANGLE_LAST+1)

#define SYS_BF_FILTER_LENGTH  24      /* must be even for optimized code */

extern Fract *sysBfFilters[];       /* table of all available filters */
#endif
/* nothing past this point */

