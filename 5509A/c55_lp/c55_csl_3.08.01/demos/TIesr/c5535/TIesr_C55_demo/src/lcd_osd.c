/*
* lcd_osd.c
*
*
* Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/ 
* 
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
#include <stdio.h>
#include <string.h>
#include "csl_i2c.h"
#include "lcd_osd.h"

#define OSD9616_I2C_ADDR 0x3C    // OSD9616 I2C address

#pragma DATA_SECTION(CharGen_6x8, ".charrom")
Uint16 CharGen_6x8[256*3];

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *      Sends 2 bytes of data to the OSD9616                                *
 *                                                                          *
 * ------------------------------------------------------------------------ */
CSL_Status OSD9616_send(
    Uint16 regAddr, 
    Uint16 regData, 
    pI2cHandle hi2c
)
{
    Uint16 writeCount;
    Uint16 writeBuff[2];
    Uint16 startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));
    CSL_Status status = CSL_SOK;

    if (hi2c != NULL)
    {
        writeCount  =  2;
        /* Initialize the buffer          */
        /* First byte is Register Address */
        /* Second byte is register data   */
        writeBuff[0] = (regAddr & 0x00FF);
        writeBuff[1] = (regData & 0x00FF);

        /* Write the data */
        //status = I2C_write(hi2c, OSD9616_I2C_ADDR, writeCount, writeBuff);
        status = I2C_write(writeBuff, writeCount, OSD9616_I2C_ADDR, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
    }
    else
    {
        status = CSL_ESYS_BADHANDLE;
    }

    return status;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *      Sends multiple bytes of data to the OSD9616                         *
 *                                                                          *
 * ------------------------------------------------------------------------ */
CSL_Status OSD9616_multiSend(
    Uint16 *regData, 
    Uint16 length, 
    pI2cHandle hi2c
)
{
    Uint16 writeBuff[10];
    Uint16 startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));
    Uint16 i;
    CSL_Status status = CSL_SOK;

    if (hi2c != NULL)
    {
        for (i=0; i<length; i++) 
        {
            writeBuff[i] = (regData[i] & 0x00FF);
        }

        /* Write the data */
        //status = I2C_write(hi2c, OSD9616_I2C_ADDR, length, writeBuff);
        status = I2C_write(writeBuff, length, OSD9616_I2C_ADDR, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
    }
    else
    {
        status = CSL_ESYS_BADHANDLE;
    }

    return status;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *      Initializes OSD9616 display                                         *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 oled_init(
    pI2cHandle hi2c
)
{
    Uint16 cmd[10];    // For multibyte commands
    Uint16 startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));
    CSL_Status status;

    /* Initialize LCD power */
    //USBSTK5515_GPIO_setDirection( 12, 1 );  // Output
    //USBSTK5515_GPIO_setOutput( 12, 1 );     // Enable 13V 

    /* Initialize OSD9616 display */
    status = OSD9616_send(0x00, 0x2e, hi2c); // Turn off scrolling 
    if (status != CSL_SOK)
    {
        return status;
    }

    status = OSD9616_send(0x00, 0x00, hi2c); // Set low column address
    if (status != CSL_SOK)
    {
        return status;
    }
    status = OSD9616_send(0x00, 0x10, hi2c); // Set high column address
    if (status != CSL_SOK)
    {
        return status;
    }
    
    status = OSD9616_send(0x00, 0x40, hi2c); // Set start line address
    if (status != CSL_SOK)
    {
        return status;
    }

    cmd[0] = 0x00 & 0x00FF;  // Set contrast control register
    cmd[1] = 0x81;
    cmd[2] = 0x7f;
    //I2C_write(hi2c, OSD9616_I2C_ADDR, 3, cmd);
    status = I2C_write(cmd, 3, OSD9616_I2C_ADDR, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
    if (status != CSL_SOK)
    {
        return status;
    }
    
    status = OSD9616_send(0x00, 0xa1, hi2c); // Set segment re-map 95 to 0
    if (status != CSL_SOK)
    {
        return status;
    }
    status = OSD9616_send(0x00, 0xa6, hi2c); // Set normal display
    if (status != CSL_SOK)
    {
        return status;
    }

    cmd[0] = 0x00 & 0x00FF;  // Set multiplex ratio(1 to 16)
    cmd[1] = 0xa8; 
    cmd[2] = 0x0f;
    //I2C_write(hi2c, OSD9616_I2C_ADDR, 3, cmd);
    status = I2C_write(cmd, 3, OSD9616_I2C_ADDR, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
    if (status != CSL_SOK)
    {
        return status;
    }

    status = OSD9616_send(0x00, 0xd3, hi2c); // Set display offset
    if (status != CSL_SOK)
    {
        return status;
    }
    status = OSD9616_send(0x00, 0x00, hi2c); // Not offset
    if (status != CSL_SOK)
    {
        return status;
    }
    status = OSD9616_send(0x00, 0xd5, hi2c); // Set display clock divide ratio/oscillator frequency
    if (status != CSL_SOK)
    {
        return status;
    }
    status = OSD9616_send(0x00, 0xf0, hi2c); // Set divide ratio
    if (status != CSL_SOK)
    {
        return status;
    }

    cmd[0] = 0x00 & 0x00FF;  // Set pre-charge period
    cmd[1] = 0xd9;
    cmd[2] = 0x22;
    //I2C_write(hi2c, OSD9616_I2C_ADDR, 3, cmd);
    status = I2C_write(cmd, 3, OSD9616_I2C_ADDR, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
    if (status != CSL_SOK)
    {
        return status;
    }

    cmd[0] = 0x00 & 0x00FF;  // Set com pins hardware configuration
    cmd[1] = 0xda;
    cmd[2] = 0x02;
    //I2C_write(hi2c, OSD9616_I2C_ADDR, 3, cmd);
    status = I2C_write(cmd, 3, OSD9616_I2C_ADDR, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
    if (status != CSL_SOK)
    {
        return status;
    }

    status = OSD9616_send(0x00, 0xdb, hi2c); // Set vcomh
    if (status != CSL_SOK)
    {
        return status;
    }
    status = OSD9616_send(0x00, 0x49, hi2c); // 0.83*vref
    if (status != CSL_SOK)
    {
        return status;
    }
    
    cmd[0] = 0x00 & 0x00FF;  //--set DC-DC enable
    cmd[1] = 0x8d;
    cmd[2] = 0x14;
    //I2C_write(hi2c, OSD9616_I2C_ADDR, 3, cmd);
    status = I2C_write(cmd, 3, OSD9616_I2C_ADDR, TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
    if (status != CSL_SOK)
    {
        return status;
    }
    
    status = OSD9616_send(0x00, 0xaf, hi2c); // Turn on oled panel
    if (status != CSL_SOK)
    {
        return status;
    }
    
    status = clear_lcd(hi2c);
    if (status != CSL_SOK)
    {
        return status;
    }
    
#if 0
    /* Set vertical and horizontal scrolling */
    cmd[0] = 0x00;
    cmd[1] = 0x29;  // Vertical and Right Horizontal Scroll
    cmd[2] = 0x00;  // Dummy byte
    cmd[3] = 0x00;  // Define start page address
    cmd[4] = 0x03;  // Set time interval between each scroll step
    cmd[5] = 0x01;  // Define end page address
    cmd[6] = 0x01;  // Vertical scrolling offset
    status = OSD9616_multiSend(cmd, 7, hi2c);
    if (status != CSL_SOK)
    {
        return status;
    }
    status = OSD9616_send(0x00, 0x2f, hi2c);
    if (status != CSL_SOK)
    {
        return status;
    }
    /* Keep first 8 rows from vertical scrolling  */
    cmd[0] = 0x00;
    cmd[1] = 0xa3;  // Set Vertical Scroll Area
    cmd[2] = 0x08;  // Set No. of rows in top fixed area
    cmd[3] = 0x08;  // Set No. of rows in scroll area
    status = OSD9616_multiSend(cmd, 4, hi2c);
    if (status != CSL_SOK)
    {
        return status;
    }
#endif
#if 0
    cmd[0] = 0x00;
    cmd[1] = 0x27;  // Horizontal Scroll
    cmd[2] = 0x00;  // Dummy byte
    cmd[3] = 0x00;  // Define start page address
    cmd[4] = 0x02;  // Set time interval between each scroll step
    cmd[5] = 0x02;  // Define end page address
    cmd[6] = 0x00;  // Dummy
    cmd[7] = 0xFF;  // Dummy
    status = OSD9616_multiSend(cmd, 8, hi2c); 
    if (status != CSL_SOK)
    {
        return status;
    }
    status = OSD9616_send(0x00, 0x2f, hi2c);
    if (status != CSL_SOK)
    {
        return status;
    }
    
#endif
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *      Clears LCD page                                                     *
 *                                                                          *
 * ------------------------------------------------------------------------ */
CSL_Status clear_lcd_page(
    Uint16 pageNum, 
    pI2cHandle hi2c
)
{
    Uint16 i;
    CSL_Status status;

    /* Clear page */ 
    status = OSD9616_send(0x00, 0x00, hi2c);   // Set low column address
    if (status != CSL_SOK)
    {
        return status;
    }
    status = OSD9616_send(0x00, 0x10, hi2c);   // Set high column address
    if (status != CSL_SOK)
    {
        return status;
    }
    status = OSD9616_send(0x00, 0xb0+pageNum, hi2c); // Set page
    if (status != CSL_SOK)
    {
        return status;
    }
    for (i=0; i<128; i++) 
    {
        status = OSD9616_send(0x40, 0x00, hi2c);
        if (status != CSL_SOK)
        {
            return status;
        }
    }

    return status;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *      Clears the LCD                                                      *
 *                                                                          *
 * ------------------------------------------------------------------------ */
CSL_Status clear_lcd(
    pI2cHandle hi2c
)
{
    CSL_Status status;

    /* Clear page 0 */ 
    status = clear_lcd_page(0, hi2c);
    if (status != CSL_SOK)
    {
        return status;
    }

    /* Clear page 1 */ 
    status = clear_lcd_page(1, hi2c);
    if (status != CSL_SOK)
    {
        return status;
    }

    return status;
#if 0
    status = OSD9616_send(0x00, 0x00, hi2c);   // Set low column address
    if (status != CSL_SOK)
    {
        return status;
    }
    status = OSD9616_send(0x00, 0x10, hi2c);   // Set high column address
    if (status != CSL_SOK)
    {
        return status;
    }
    status = OSD9616_send(0x00, 0xb0+0, hi2c); // Set page to page 0
    if (status != CSL_SOK)
    {
        return status;
    }
    for (i=0; i<128; i++) 
    {
        status = OSD9616_send(0x40, 0x00, hi2c);
        if (status != CSL_SOK)
        {
            return status;
        }
    }

    /* Clear page 1 */ 
    status = OSD9616_send(0x00, 0x00, hi2c);   // Set low column address
    if (status != CSL_SOK)
    {
        return status;
    }
    status = OSD9616_send(0x00, 0x10, hi2c);   // Set high column address
    if (status != CSL_SOK)
    {
        return status;
    }
    status = OSD9616_send(0x00, 0xb0+1, hi2c); // Set page to page 1
    if (status != CSL_SOK)
    {
        return status;
    }
    for (i=0; i<128; i++) 
    {
        status = OSD9616_send(0x40, 0x00, hi2c);
        if (status != CSL_SOK)
        {
            return status;
        }
    }

    return status;
#endif
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *      Send 6 bytes representing a Character to LCD                        *
 *                                                                          *
 * ------------------------------------------------------------------------ */
CSL_Status print_char(
    unsigned char a, 
    pI2cHandle hi2c
)
{
    Uint8 data;
    CSL_Status status;

    data = ((CharGen_6x8[a*3])>>8) & 0x00FF;
    status = OSD9616_send(0x40, data, hi2c);
    if (status != CSL_SOK)
    {
        return status;
    }
    data = (CharGen_6x8[a*3]) & 0x00FF;
    status = OSD9616_send(0x40, data, hi2c);
    if (status != CSL_SOK)
    {
        return status;
    }
    data = ((CharGen_6x8[a*3+1])>>8) & 0x00FF;
    status = OSD9616_send(0x40, data, hi2c);
    if (status != CSL_SOK)
    {
        return status;
    }
    data = (CharGen_6x8[a*3+1]) & 0x00FF;
    status = OSD9616_send(0x40, data, hi2c);
    if (status != CSL_SOK)
    {
        return status;
    }
    data = ((CharGen_6x8[a*3+2])>>8) & 0x00FF;
    status = OSD9616_send(0x40, data, hi2c);
    if (status != CSL_SOK)
    {
        return status;
    }
    data = (CharGen_6x8[a*3+2]) & 0x00FF;
    status = OSD9616_send(0x40, data, hi2c);
    if (status != CSL_SOK)
    {
        return status;
    }

    return status;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *      Prints a string to LCD                                              *
 *                                                                          *
 * ------------------------------------------------------------------------ */
CSL_Status print_string(
    char *a, 
    pI2cHandle hi2c
)
{
    Uint16 len;
    Uint16 i;
    CSL_Status status = CSL_SOK;

    len = strlen(a);
    if ((len ==0) | (len>22)) len=16;

    for (i=0; i<len; i++) 
    {
        status = print_char(a[i], hi2c);
        if (status != CSL_SOK)
        {
            return status;
        }
    }

    return status;
}

#if 0
/* ------------------------------------------------------------------------ *
 *                                                                          *
 *      Prints "Texas Instruments"                                          *
 *            "C5535 eZdsp stk" to LCD                                      *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 print_ti(
    pI2cHandle hi2c
)
{
    char ti[23]; 
        
    OSD9616_send(0x00, 0x2e, hi2c); // Turn off scrolling 
    clear_lcd(hi2c);
    sprintf(ti,"                       "); //clear buffer
    /* Write to page 1 */
    OSD9616_send(0x00, 0x00, hi2c);   // Set low column address
    OSD9616_send(0x00, 0x10, hi2c);   // Set high column address
    OSD9616_send(0x00, 0xb0+1, hi2c); // Set page for page 0 to page 5
       
    sprintf(ti,"  Texas Instruments");
    print_string(ti, hi2c);
    /* Write to page 0*/ 
    OSD9616_send(0x00, 0x00, hi2c);   // Set low column address
    OSD9616_send(0x00, 0x10, hi2c);   // Set high column address
    OSD9616_send(0x00, 0xb0+0, hi2c); // Set page for page 0 to page 5
    sprintf(ti," C5535 eZdsp USB stick");
    print_string(ti, hi2c);
    
    OSD9616_send(0x00, 0x2f, hi2c); // Turn on scrolling 
    
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *      Prints "Industry lowest"                                            *
 *            "power dsp C5535" to LCD                                      *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 print_dsp(
    pI2cHandle hi2c
)
{
    char ti[23]; 
    Uint16 i;
    
    OSD9616_send(0x00, 0x2e, hi2c); // Turn off scrolling 
    clear_lcd(hi2c);
    
    for (i=0; i<23; i++) 
    {
        ti[i] = 0x20;      //clear buffer
    }
    //sprintf(ti,"                       "); // clear buffer

    /* Write to page 1 */
    OSD9616_send(0x00, 0x00, hi2c);   // Set low column address
    OSD9616_send(0x00, 0x10, hi2c);   // Set high column address
    OSD9616_send(0x00, 0xb0+1, hi2c); // Set page for page 0 to page 5
       
#if 0
    ti[0] = 0x54; /* T        0x54    84                   */
    ti[1] = 0x49; /* I        0x49    73                   */
    ti[2] = 0x20; /* (space)  0x20    32                   */
    ti[3] = 0x43; /* C        0x43    67                   */
    ti[4] = 0x35; /* 5        0x35    53                   */
    ti[5] = 0x30; /* 0        0x30    48                   */
    ti[6] = 0x30; /* 0        0x30    48                   */
    ti[7] = 0x30; /* 0        0x30    48                   */
    ti[8] = 0x20; /* (space)  0x20    32                   */
    ti[9] = 0x41; /* A        0x41    65                   */
    ti[10]= 0x45; /* E        0x45    69                   */
    ti[11]= 0x52; /* R        0x52    82                   */
    ti[12]= 0x20; /* (space)  0x20    32                   */
    ti[13]= 0x20; /* (space)  0x20    32                   */
    ti[14]= 0x20; /* (space)  0x20    32                   */
    ti[15]= 0x20; /* (space)  0x20    32                   */
#endif
    sprintf(ti,"   TI C5000 DSP");
    print_string(ti, hi2c);
    /* Write to page 0*/ 
    OSD9616_send(0x00, 0x00, hi2c);   // Set low column address
    OSD9616_send(0x00, 0x10, hi2c);   // Set high column address
    OSD9616_send(0x00, 0xb0+0, hi2c); // Set page for page 0 to page 5
#if 0
    for (i=0; i<23; i++) 
    {
        ti[i] = 0x20;      //clear buffer
    }
    ti[0] = 0x20; /* (space)  0x20    32                   */
    ti[1] = 0x20; /* (space)  0x20    32                   */
    ti[2] = 0x20; /* (space)  0x20    32                   */
    ti[3] = 0x44; /* D        0x44    68                   */
    ti[4] = 0x45; /* E        0x45    69                   */
    ti[5] = 0x4D; /* M        0x4D    77                   */
    ti[6] = 0x4F; /* O        0x5F    79                   */
    ti[7] = 0x20; /* (space)  0x20    32                   */
    ti[8] = 0x20; /* (space)  0x20    32                   */
    ti[9] = 0x20; /* (space)  0x20    32                   */
    ti[10]= 0x20; /* (space)  0x20    32                   */
    ti[11]= 0x20; /* (space)  0x20    32                   */
    ti[12]= 0x20; /* (space)  0x20    32                   */
    ti[13]= 0x20; /* (space)  0x20    32                   */
    ti[14]= 0x20; /* (space)  0x20    32                   */
    ti[15]= 0x20; /* (space)  0x20    32                   */      
#endif
    sprintf(ti,"   SPKPHONE DEMO*");
    print_string(ti, hi2c);
    
    OSD9616_send(0x00, 0x2f, hi2c); // Turn on scrolling 

    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *      Prints all characters in ROM table to LCD                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 print_charROM(
    pI2cHandle hi2c
)
{
    char crom[16]; 
    Uint16 i,j;
    
    OSD9616_send(0x00, 0x2e, hi2c); // Turn off scrolling 
    clear_lcd(hi2c);
    
    for (j=0; j<8; j++) 
    {
        /* Write to page 1 */
        OSD9616_send(0x00,  0x00, hi2c);   // Set low column address
        OSD9616_send(0x00,  0x10, hi2c);   // Set high column address
        OSD9616_send(0x00,  0xb0+1, hi2c); // Set page for page 0 to page 5
        for (i=0; i<16; i++) 
        {
            crom[i] = i + j*32;
        }
        print_string(crom, hi2c);
     
        /* Write to page 0*/ 
        OSD9616_send(0x00,  0x00, hi2c);   // Set low column address
        OSD9616_send(0x00,  0x10, hi2c);   // Set high column address
        OSD9616_send(0x00,  0xb0+0, hi2c); // Set page for page 0 to page 5
        for (i=0; i<16; i++) 
        {
            crom[i] = i + 16 + j*32;
        }    
        print_string(crom, hi2c);
    }
    //OSD9616_send(0x00,0x2f, hi2c); // Turn on scrolling 
    return 0;
}

#endif
