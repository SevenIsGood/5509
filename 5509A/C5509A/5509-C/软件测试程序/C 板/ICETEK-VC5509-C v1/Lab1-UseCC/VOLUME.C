#include "volume.h"

int inp_buffer[BUF_SIZE];	/* BUF_SIZE的定义见volume.h */
int out_buffer[BUF_SIZE];

int *input;
int *output;

int volume = 1;

struct PARMS str = 
{
	2934,9432,213,9432,&str
};

/***************************************************************************/
/*                                                                         */
/* NAME: read_signals()                                                    */
/*                                                                         */
/* FUNCTION: read input signal.                                            */
/*                                                                         */
/* PARAMETERS: none.                                                       */
/*                                                                         */
/* RETURN VALUE: TRUE.                                                     */
/*                                                                         */
/***************************************************************************/
int read_signals(int *input)
{
	/* read reference signal */
	/* read input signal */
	return(TRUE);
}

/***************************************************************************/
/*                                                                         */
/* NAME: write_buffer()                                                    */
/*                                                                         */
/* FUNCTION: write to the output buffer. Use the volume variable 		   */
/*			 to control the volume.						                   */
/*                                                                         */
/* PARAMETERS: input , output, num.                                        */
/*                                                                         */
/* RETURN VALUE: TRUE.                                                     */
/*                                                                         */
/***************************************************************************/
int write_buffer(int *input,int *output,int count)
{
	while( count--)
	{
		*output++ = (*input++) * volume;
	}
	return(TRUE);
}

/***************************************************************************/
/*                                                                         */
/* NAME: main()                                                            */
/*                                                                         */
/* FUNCTION: Volume Control: Read input buffer							   */
/*           multiply by volume coeficient and write to                    */
/*			 output buffer.												   */
/*                                                                         */
/* PARAMETERS: none.                                                       */
/*                                                                         */
/* RETURN VALUE: none.                                                     */
/*                                                                         */
/***************************************************************************/
main()
{   
	int num = BUF_SIZE;

    while(TRUE)  /* loop forever */
	{               
		input = &inp_buffer[0]; 
		output = &out_buffer[0];
		/* read input signals from PC file */
		read_signals(input);
		
		/* write to output buffer 		*/
		write_buffer(input, output, num);
	}
}
