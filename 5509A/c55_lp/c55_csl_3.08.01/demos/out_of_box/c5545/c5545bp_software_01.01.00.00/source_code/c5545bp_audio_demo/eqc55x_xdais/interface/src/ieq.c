
/*
 *  ======== ieq.c ========
 *  Default creation parameters for all implementations of the EQ
 *  module abstract interface. 
 */

#include <std.h>
#include <ieq.h>

/*
 *  ======== IEQ_PARAMS ========
 *  This static initialization defines the default parameters used to
 *  create an instances of a EQ object.
 */
const IEQ_Params IEQ_PARAMS = {
	sizeof(IEQ_PARAMS),
	64,	/* bufferSize */
	5, 0,	/* numBands, dBGains */
	0, 0	/* coeffs, deltaCoeffs */
};


