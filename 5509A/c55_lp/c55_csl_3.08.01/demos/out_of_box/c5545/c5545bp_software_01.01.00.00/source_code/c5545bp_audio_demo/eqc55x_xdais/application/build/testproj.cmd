-w
/***********************************************************/
/*                 Linker command file                     */
/***********************************************************/
MEMORY
{
	SRAMP1:	origin = 28000h length = 08000h
	SRAMP2:	origin = 38000h length = 06000h
/*	PAGE 0: vectors:	origin = 0FF80h length = 00080h*/

	DARAM: 	origin = 02080h	length = 0df80h  
	SRAMP3: origin = 03E000h length = 1500h
}
 
SECTIONS
{
/*	vectors: {}		> vectors	page 0*/
	.text    		> SRAMP1	

	.switch  		> SRAMP2	
	.cinit   		> SRAMP2	
	.const   		> DARAM		
	/*DualMacVars     > DARAM   arnab  */
	.bss            > DARAM
	.sysmem         > DARAM	
	.data			> DARAM		
    
	.stack   		> DARAM		
	.sysstack       > DARAM		
    .cio            > DARAM 	
    data_align1       > SRAMP2,align(256) 
    data_align2       > SRAMP2,align(256) 
	eq_align1       > SRAMP2,align(256) 
    eq_align2       > SRAMP2,align(256) 
	eq_band_align1       > SRAMP2,align(256) 
    eq_band_align2       > SRAMP2,align(256) 
    
}  
