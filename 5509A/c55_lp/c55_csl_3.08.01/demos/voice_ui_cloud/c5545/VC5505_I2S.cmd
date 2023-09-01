///-stack    0x400                 /* PRIMARY STACK SIZE    */
///-sysstack 0x200                 /* SECONDARY STACK SIZE  */
///-heap     0x800                 /* HEAP AREA SIZE        */

///MEMORY
///{
///    MMR     (RW)  : origin = 0000000h length = 0000c0h /* MMRs */
///    VECS    (RX)  : origin = 0ffff00h length = 000100h /* on-chip ROM vectors */
///}

SECTIONS
{
	vectors(NOLOAD)
    vector      : > SARAM    ALIGN = 256
    ///.text       : > SARAM
    ///.switch     : > SARAM
    ///.cinit      : > SARAM
    ///.pinit      : > SARAM

    ///.data       : > SARAM
    ///.bss        : > SARAM
    ///.const      : > SARAM

    ///.stack      : > SARAM
    ///.sysstack   : > SARAM

    ///.sysmem     : > SARAM
    ///.cio        : > SARAM

    data_br_buf  : > DARAM, align=0x400  /* for keyword recog */
    data_fft_buf : > DARAM              /* for keyword recog */

    i2sDmaWriteBufLeft     : > DARAM
    i2sDmaWriteBufRight    : > DARAM
    i2sDmaReadBufLeft      : > DARAM
    i2sDmaReadBufRight     : > DARAM
    i2sDmaReadBufLeft2      : > DARAM
    i2sDmaReadBufRight2     : > DARAM
    i2sDmaReadBufLeft3      : > DARAM
    i2sDmaReadBufRight3     : > DARAM

    .volatileMemBufs       : > SARAM
    .nonVolatileMemBufs    : > SARAM
}

/* C5545 fft addresses */
/* For keyword recog */
_hwafft_br = 0x00fefe9c;
_hwafft_256pts = 0x00ff07a4;
