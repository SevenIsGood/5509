#include <stdio.h>
#include <string.h>
#include "src_ti.h"
#include "src.h"

FILE *fIn = NULL;
FILE *fOut = NULL;

#define IBS 32
#define OBS 64

int lbuff[IBS];
int rbuff[IBS];
int loutbuff[OBS];
int routbuff[OBS];


//ISRC_Params lp = {sizeof(SRC_Params),IBS,OBS,8000,8000,0};
//ISRC_Params rp = {sizeof(SRC_Params),IBS,OBS,8000,8000,0};
//ISRC_Params lp = {sizeof(SRC_Params),IBS,OBS,8000,48000,0};
//ISRC_Params rp = {sizeof(SRC_Params),IBS,OBS,8000,48000,0};
//ISRC_Params lp = {sizeof(SRC_Params),IBS,OBS,11025,8000,0};
//ISRC_Params rp = {sizeof(SRC_Params),IBS,OBS,11025,8000,0};
//ISRC_Params lp = {sizeof(SRC_Params),IBS,OBS,11025,44100,0};
//ISRC_Params rp = {sizeof(SRC_Params),IBS,OBS,11025,44100,0};
//ISRC_Params lp = {sizeof(SRC_Params),IBS,OBS,16000,11025,0};
//ISRC_Params rp = {sizeof(SRC_Params),IBS,OBS,16000,11025,0};
//ISRC_Params lp = {sizeof(SRC_Params),IBS,OBS,16000,32000,0};
//ISRC_Params rp = {sizeof(SRC_Params),IBS,OBS,16000,32000,0};
//ISRC_Params lp = {sizeof(SRC_Params),IBS,OBS,22050,16000,0};
//ISRC_Params rp = {sizeof(SRC_Params),IBS,OBS,22050,16000,0};
//ISRC_Params lp = {sizeof(SRC_Params),IBS,OBS,22050,48000,0};
//ISRC_Params rp = {sizeof(SRC_Params),IBS,OBS,22050,48000,0};
//ISRC_Params lp = {sizeof(SRC_Params),IBS,OBS,32000,8000,0};
//ISRC_Params rp = {sizeof(SRC_Params),IBS,OBS,32000,8000,0};
//ISRC_Params lp = {sizeof(SRC_Params),IBS,OBS,32000,48000,0};
//ISRC_Params rp = {sizeof(SRC_Params),IBS,OBS,32000,48000,0};
//ISRC_Params lp = {sizeof(SRC_Params),IBS,OBS,44100,16000,0};
//ISRC_Params rp = {sizeof(SRC_Params),IBS,OBS,44100,16000,0};
//ISRC_Params lp = {sizeof(SRC_Params),IBS,OBS,44100,48000,0};
//ISRC_Params rp = {sizeof(SRC_Params),IBS,OBS,44100,48000,0};
//ISRC_Params lp = {sizeof(SRC_Params),IBS,OBS,48000,22050,0};
//ISRC_Params rp = {sizeof(SRC_Params),IBS,OBS,48000,22050,0};
ISRC_Params lp = {sizeof(SRC_Params),IBS,OBS,48000,48000,0};
ISRC_Params rp = {sizeof(SRC_Params),IBS,OBS,48000,48000,0};


//char *infilename = "..\\..\\..\\test\\input\\sweep_0-4_samp8.pcm";
//char *infilename = "..\\..\\..\\test\\input\\sweep_0-5_samp11.pcm";
//char *infilename = "..\\..\\..\\test\\input\\sweep_0-8_samp16.pcm";
//char *infilename = "..\\..\\..\\test\\input\\sweep_0-11_samp22.pcm";
//char *infilename = "..\\..\\..\\test\\input\\sweep_0-16_samp32.pcm";
//char *infilename = "..\\..\\..\\test\\input\\sweep_0-22_samp44.pcm";
char *infilename = "..\\..\\..\\test\\input\\sweep_0-24_samp48.pcm";

//char *outfilename = "..\\..\\..\\test\\output\\sweep_0-4_samp8to8.pcm";
//char *outfilename = "..\\..\\..\\test\\output\\sweep_0-4_samp8to48.pcm";
//char *outfilename = "..\\..\\..\\test\\output\\sweep_0-5_samp11to8.pcm";
//char *outfilename = "..\\..\\..\\test\\output\\sweep_0-5_samp11to44.pcm";
//char *outfilename = "..\\..\\..\\test\\output\\sweep_0-8_samp16to11.pcm";
//char *outfilename = "..\\..\\..\\test\\output\\sweep_0-8_samp16to32.pcm";
//char *outfilename = "..\\..\\..\\test\\output\\sweep_0-11_samp22to16.pcm";
//char *outfilename = "..\\..\\..\\test\\output\\sweep_0-11_samp22to48.pcm";
//char *outfilename = "..\\..\\..\\test\\output\\sweep_0-16_samp32to8.pcm";
//char *outfilename = "..\\..\\..\\test\\output\\sweep_0-16_samp32to48.pcm";
//char *outfilename = "..\\..\\..\\test\\output\\sweep_0-22_samp44to16.pcm";
//char *outfilename = "..\\..\\..\\test\\output\\sweep_0-22_samp44to48.pcm";
//char *outfilename = "..\\..\\..\\test\\output\\sweep_0-24_samp48to22.pcm";
char *outfilename = "..\\..\\..\\test\\output\\sweep_0-24_samp48to48.pcm";

int rate;

int i=0;

int tempin[IBS*4];
int tempout[OBS*4];
//void apply_both();
void main()
{


  int flag=1;

  ISRC_Handle left,right;
  SRC_Params srcLeftParams, srcRightParams; ////======


  		fOut=fopen(outfilename,"wb");
  		if(fOut == NULL)
  		{
  			return;
  		}
  		fIn=fopen(infilename,"rb");
  		if(fIn == NULL)
  		{
  			return;
  		}

  		for(i=0;i<IBS;i++)
 	    {
    		lbuff[i]=0;
    		rbuff[i]=0;
  		}
  		for(i=0;i<OBS;i++)
  		{
    		loutbuff[i]=0;
    		routbuff[i]=0;
   		}

  	    for(i=0;i<IBS*2;i++)
  		{
    		tempin[i]=0;

  		}
  	   for(i=0;i<OBS*2;i++)
  	   {
    		tempout[i]=0;

       }


 		 //lp.inputRate = rp.inputRate = inputRate ;
 		 //lp.outputRate = rp.outputRate = outputRate;

  		srcLeftParams = lp;
  		srcRightParams = rp;
  		/*create an instance of the algorithm */
  		if(((left=(SRC_create(&SRC_TI_ISRC,&srcLeftParams))) != NULL)
  		&& ((right=(SRC_create(&SRC_TI_ISRC,&srcRightParams))) != NULL))
  		{

   		while(1)
    	{

     	SRC_setOutput(left,loutbuff);
     	SRC_setOutput(right,routbuff);
     	while (!SRC_isOutputBufferFull(left))
     	{
        	if (SRC_isInputBufferEmpty(left))
        	{
          		flag=fread(tempin,1,IBS*4,fIn);
          		if(flag == 0) break;

          		for(i=0;i<IBS*2;i+=1)
            	if(!(i%2))
            	{
               		lbuff[i/2]=tempin[2*i+1];
               		lbuff[i/2]=lbuff[i/2]<<8;
               		lbuff[i/2]+=tempin[2*i];
            	}
            	else
            	{
               		rbuff[i/2]=tempin[2*i+1];
               		rbuff[i/2]=rbuff[i/2]<<8;
               		rbuff[i/2]+=tempin[2*i];
             	}

               SRC_setInput(left,lbuff);
               SRC_setInput(right,rbuff);
        	}

       		SRC_apply(left);
       		SRC_apply(right);

    	}

    	if(flag == 0) break;
		for(i=0;i<2*OBS;i++)
      	if(!(i%2))
      	{

       		tempout[2*i]=loutbuff[i/2]&0xff;
       		tempout[2*i+1]=(loutbuff[i/2]>>8) & 0xff;

      	}
      	else
      	{
       		tempout[2*i]=routbuff[i/2]&0xff;
       		tempout[2*i+1]=(routbuff[i/2]>>8) & 0xff;
      	}

      	flag=fwrite(tempout,1,OBS*4,fOut);

     	}
     	SRC_delete(left);
        SRC_delete(right);
    	}
   		fclose(fIn);
   		fclose(fOut);


}

