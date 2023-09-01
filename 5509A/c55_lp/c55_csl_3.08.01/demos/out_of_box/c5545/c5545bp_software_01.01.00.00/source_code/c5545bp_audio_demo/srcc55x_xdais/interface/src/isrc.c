/*
 *  ======== isrc.c ========
 *  Default creation parameters for all implementations of the SRC
 *  module abstract interface. 
 */

#include <std.h>
#include <isrc.h>

/*
 *  ======== ISRC_PARAMS ========
 *  This static initialization defines the default parameters used to
 *  create an instances of a SRC object.
 */
const ISRC_Params ISRC_PARAMS = {
    sizeof(ISRC_PARAMS) ,  /* Size of this structure */
    512,   ////======
    64,    ////======
    44100, ////====== 
    22050, ////======
    0      ///======
};


