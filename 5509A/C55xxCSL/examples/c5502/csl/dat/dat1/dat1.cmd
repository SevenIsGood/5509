/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------dat1.cmd---------
 */

MEMORY
{
	VECS:	o = 0x0000		l = 0x100
	DARAM:  o = 0x200		l = 0xfe00
}

SECTIONS
{
	.cinit		>	DARAM
	.text		>	DARAM
	.stack		>	DARAM
	.sysstack	>	DARAM
	.sysmem		>	DARAM
	.data		>	DARAM
	.cio		>	DARAM
	.bss		>	DARAM
	.const		>	DARAM
	.csldata	>	DARAM
	.buffers	>   DARAM	
}

