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


/*********************************************************************
*    iic.h
**********************************************************************/
#ifndef IIC_H
#define IIC_H

/**
 * @param address     - The address of the targetted slave device
 * @param iv          - The DSP clock divider
 * @param clk_low     - The clock divider for the clock low period
 * @param clk_high    - The clock divider for the clock high period
 *
 * @return void
 *
 * \par
 * This function initializes the IIC module to communicate to a particular
 * IIC slave device.  It also sets the module clock and clock high/low
 * periods if needed for a specific device.  The module clock should be
 * set to 12 Mhz.  Using the following formula
 * \f[
 *      12 Mhz = \frac{System Clock}{iv + 1}
 * \f]
 *
 * \par
 * The module clock is then divided by clk_low and clk_high to achieve the
 * desired frequency X
 * \f[
 *      X = \frac{12 Mhz}{clk low + clk high + 2}
 * \f]
 *
 * \par
 */
void IIC_INIT(unsigned int address, unsigned int iv, unsigned int clk_low,
                unsigned int clk_high);

/**
 * @param count       - The number of bytes to be read from device
 * @param buffer      - A pointer to an unpacked buffer where the data will read
 *
 * @return  The value of the first byte of data read in the lower byte of the
 *          return register.  In the upper byte, it returns the read status.
 *          For the upper byte, the status codes are as follows:
 *              <ul>
 *              <li>0 - Success
 *              <li>1 - Failure
 *              <li>2 - Zero read
 *              </ul>
 * \par
 * This function reads values from the IIC bus.  It assumes that the device
 * has already been issued a command to return a value.
 * \verbatim
       16          8 7          0
       +-------------------------+
       |   Status   | First Byte |
       +-------------------------+
   \endverbatim
 */
unsigned int IIC_READ(unsigned int count, unsigned int * pBuffer);

/**
 * @param stop        - Whether or not to issue the stop condition after write
 * @param address     - The address to write to the bus
 * @param count       - The number of bytes to write from the buffer
 * @param buffer      - Pointer to the unpacked buffer to write data from.
 *
 * @return Returns an error code:
 *          <ul>
 *          <li>0 - Success
 *          <li>1 - Failure
 *          <li>2 - Success with no stop
 *          </ul>
 * \par
 * This function writes data onto the IIC bus.
 */
unsigned int IIC_WRITE(unsigned int stop, unsigned int address,
                int count, unsigned int * buffer);

#endif /* IIC_H */
