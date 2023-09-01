/* ============================================================================
 * Copyright (c) 2008-2012 Texas Instruments Incorporated.
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/


/*
 *  AIC3204 Test
 *
 */

#define AIC3204_I2C_ADDR 0x18
#include "evm5515.h"
#include "evm5515_gpio.h"
#include "evm5515_i2c.h"
#include "stdio.h"
#include "csl_general.h"

#if (defined(CHIP_C5517))

extern Int16 aic3204_tone_headphone( );
extern Int16 aic3204_tone_stereo_out( );
extern Int16 aic3204_loop_stereo_in1( );
extern Int16 aic3204_loop_stereo_in2( );
extern Int16 aic3204_loop_mic_in( );
/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _AIC3204_rget( regnum, regval )                                         *
 *                                                                          *
 *      Return value of codec register regnum                               *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 AIC3204_rget(  Uint16 regnum, Uint16* regval )
{
    Int16 retcode = 0;
    Uint8 cmd[2];

    cmd[0] = regnum & 0x007F;       // 7-bit Device Address
    cmd[1] = 0;

    retcode |= EVM5515_I2C_write( AIC3204_I2C_ADDR, cmd, 1 );
    retcode |= EVM5515_I2C_read( AIC3204_I2C_ADDR, cmd, 1 );

    *regval = cmd[0];
    EVM5515_wait( 10 );
    return retcode;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _AIC3204_rset( regnum, regval )                                         *
 *                                                                          *
 *      Set codec register regnum to value regval                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 AIC3204_rset( Uint16 regnum, Uint16 regval )
{
    Uint8 cmd[2];
    cmd[0] = regnum & 0x007F;       // 7-bit Register Address
    cmd[1] = regval;                // 8-bit Register Data

    return EVM5515_I2C_write( AIC3204_I2C_ADDR, cmd, 2 );
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  aic3204_test( )                                                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 aic3204_test( )
{
	/* Configure Parallel Port */
    SYS_EXBUSSEL &= ~0x7000;     // Set parallel port to 000
    //SYS_EXBUSSEL |=  0x1000;   // Configure Parallel Port for I2S2
    /* Configure Serial Port */
	SYS_EXBUSSEL &= ~0x0F00;   //
	SYS_EXBUSSEL |=  0x0B00;   // Serial Port 1 mode 3 (McBSP and GP[5:4]).
	//EVM5515_GPIO_init();
	//EVM5515_GPIO_setDirection(GPIO10, GPIO_OUT);
	//EVM5515_GPIO_setOutput( GPIO10, 1 );    // Take AIC3201 chip out of reset
	EVM5515_I2C_init( );                    // Initialize I2C

    /* Codec tests */
   /* printf( " -> 1 KHz Tone on Headphone.\n" );
    if ( aic3204_tone_headphone( ) )
        return 1;

    EVM5515_wait( 100 );  // Wait
    printf( " -> 1 KHz Tone on Stereo OUT.\n" );
    if ( aic3204_tone_stereo_out( ) )
        return 1;

    EVM5515_wait( 100 );  // Wait
    printf( "<-> Audio Loopback from Stereo IN 2 --> to Stereo OUT\n" );
    if ( aic3204_loop_stereo_in2( ) )
        return 1;

    EVM5515_wait( 100 );  // Wait
    printf( "<-> Microphone --> to HP\n" );
    if ( aic3204_loop_mic_in( ) )
       return 1;
    */

    EVM5515_wait( 100 );  // Wait
    printf( "<-> Audio Loopback from Stereo IN 1 -->CODEC-->C5517(McBSP)-->CODEC--> HP\n" );

    if (aic3204_loop_stereo_in1())
	{
        return (1);
	}

	EVM5515_GPIO_setOutput( GPIO26, 0 );

	return (0);
}

#endif /* #if (defined(CHIP_C5517)) */
