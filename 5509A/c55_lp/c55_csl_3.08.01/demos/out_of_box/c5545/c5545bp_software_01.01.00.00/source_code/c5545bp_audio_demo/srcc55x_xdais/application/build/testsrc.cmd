-w
/***********************************************************/
/*                 Linker command file                     */
/***********************************************************/
MEMORY
{
	PAGE 0: SRAMP1:	origin = 28000h length = 08000h
	PAGE 0: SRAMP2:	origin = 38000h length = 06000h
/*	PAGE 0: vectors:	origin = 0FF80h length = 00080h*/

	PAGE 0: DARAM: 	origin = 02080h	length = 0df80h
}
 
SECTIONS
{
/*	vectors: {}		> vectors	page 0*/
	.text    		> SRAMP1	PAGE 0

	.switch  		> SRAMP2	PAGE 0
	.cinit   		> SRAMP2	PAGE 0

	.const   		> DARAM		PAGE 0
	.bss     		> DARAM		PAGE 0 
	.sysmem         > DARAM 	PAGE 0
	.data			> DARAM		PAGE 0
	.stack   		> DARAM		PAGE 0    
    .cio            > DARAM 	PAGE 0
    data_align1       > SRAMP2,align(256) PAGE 0
    data_align2       > SRAMP2,align(256) PAGE 0
	eq_align1       > SRAMP2,align(256) PAGE 0
    eq_align2       > SRAMP2,align(256) PAGE 0
	eq_band_align1       > SRAMP2,align(256) PAGE 0
    eq_band_align2       > SRAMP2,align(256) PAGE 0
	
}  
