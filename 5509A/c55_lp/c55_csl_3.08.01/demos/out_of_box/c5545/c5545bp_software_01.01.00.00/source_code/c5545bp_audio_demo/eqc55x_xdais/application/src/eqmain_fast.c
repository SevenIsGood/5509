
#define IBS 64   
#define OBS 64

#define REAL_TIME
/* Number of bands..for the Equalizer..*/
#define  N  5
/* Number of bands..for the Equalizer..*/

#include <stdio.h>
#include <string.h> 
#include "eq_ti.h"
#include "eq.h"
FILE *fIn = NULL;
FILE *fOut = NULL;  

int loutbuff[OBS];
int routbuff[OBS];    

//*ROCK
//Int const gain[N] = { 12, 0};
//Int const gain[N] = {6,6,6,6,6};
//Int const gain[N] = { 6};
//*JAZZ
//Int const gain[N] = { -12, 0, -3, 0, 3};
//*DANCE
//Int const gain[N] = { 12, 0, -6, -6, 0};
//*LATIN
Int const gain[N] = { -12, 3, -3, 3, 0};
//to check 2 band performance


#ifndef REAL_TIME

const EQ_Coeff EQ_TI_DefCoeffs_temp[] = {
  0x3fb687ea, /*xS1Beta*/
  0x804aa9e8, /*xS1BGamma*/
  0x3ed00758, /*xS2Beta*/
  0x81449017, /*xS2BGamma*/
  0x3b608930, /*xS3Beta*/
  0x85df7eca, /*xS3BGamma*/
  0x2f36a614, /*xS4Beta*/
  0xa25d0cac, /*xS4BGamma*/
  0x16597883, /*xS5Beta*/
  0x0bf4e454  /*xS5BGamma*/
   };

const EQ_Coeff EQ_TI_DefDeltaCoeffs_temp[] = {
 0x00165cd3, /*xS1BDelta*/
 0xffe9a362, /*xS1BGDelta*/	 
 0x0059017c, /*xS2BDelta*/
 0xffa70cfe, /*xS2BGDelta*/
 0x012d4aae, /*xS3BDelta*/
 0xfed5c2d9, /*xS3BGDelta*/
 0x02cc7214, /*xS4BDelta*/
 0xfda4c951, /*xS4BGDelta*/
 0x0375432b, /*xS5BDelta*/
 0x007a9486  /*xS5BGDelta*/
 };

EQ_Status EQstatus = {sizeof(IEQ_Status),IBS,N,(long *)EQ_TI_DefCoeffs_temp,(long *)EQ_TI_DefDeltaCoeffs_temp};

#else

EQ_Status EQstatus = {sizeof(IEQ_Status),IBS,N,NULL,NULL};

#endif

EQ_Params eqlp = {sizeof(IEQ_Params),OBS,N,gain,NULL,NULL};
EQ_Params eqrp = {sizeof(IEQ_Params),OBS,N,gain,NULL,NULL};
/*
#pragma DATA_SECTION(eqleftband,"eq_band_align1");

#pragma DATA_SECTION(eqrightband,"eq_band_align2");
*/

const IALG_MemRec *dummy=NULL;
const IALG_MemRec *vdummy=NULL;

//const EQ_TI_Handle eqpdummy=NULL;

/*char *infilename = "d:\\test\\inputfile\\EQtest\\Oops_44khz.pcm"; 
char *outfilename = "d:\\test\\outputfile\\Output_Oops_44khz.pcm";  */

char *infilename = "..\\..\\..\\test\\input\\Oops_44khz.pcm"; 

//char *outfilename = "..\\..\\..\\test\\output\\Output_Oops_44khz_6_6_6_6_6.pcm";  
//char *outfilename = "..\\..\\..\\test\\output\\Output_Oops_44khz_12_0_-6_-6_0.pcm";  
//char *outfilename = "..\\..\\..\\test\\output\\Output_Oops_44khz_-12_0_-3_0_3.pcm";  
char *outfilename = "..\\..\\..\\test\\output\\Output_Oops_44khz_-12_3_-3_3_0.pcm";  


int i=0; 
char cgain[2];
       
int tempin[IBS*4];       
int tempout[OBS*4];
long FrameCnt;
void main()
{ 
//  unsigned char =0;
  
  int flag=1;
//  int filecount;
//  int inner,outer;

  IEQ_Handle  eqleft,eqright;
  EQ_Params eqLeftParams, eqRightParams; ////=====  
//  unsigned char c123=0;
  FrameCnt=0;
      
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
  eqLeftParams = eqlp;
  eqRightParams = eqrp;
  
  EQ_init();
  
  if(((eqleft=(EQ_create(&EQ_TI_IEQ,&eqLeftParams))) != NULL) 
  && ((eqright=(EQ_create(&EQ_TI_IEQ,&eqRightParams))) != NULL))
  {
  
  
#ifdef REAL_TIME
  EQ_setStatus(eqleft, &EQstatus);
  EQ_setStatus(eqright, &EQstatus);
#endif

  while(1)
  {  
     
          flag=fread(tempin,1,IBS*4,fIn);
          if(flag == 0) break;
  
          for(i=0;i<IBS*2;i+=1)
            if(!(i%2))
             {
               loutbuff[i/2]=tempin[2*i+1];
               loutbuff[i/2]=loutbuff[i/2]<<8;
               loutbuff[i/2]+=tempin[2*i];
             }  
            else 
             {
               routbuff[i/2]=tempin[2*i+1];
               routbuff[i/2]=routbuff[i/2]<<8;
               routbuff[i/2]+=tempin[2*i];
             }  
        
 
 	    EQ_setGains(eqleft, (int *)gain);
		EQ_setGains(eqright,(int *)gain);

    
        EQ_apply(eqleft,loutbuff,loutbuff);
   	    EQ_apply(eqright,routbuff,routbuff);
   	    
   	    FrameCnt++;
           
	    for(i=0;i<2*OBS;i++)
        if(!(i%2))
         {
          tempout[2*i]=loutbuff[i/2]&0xff;
          tempout[2*i+1]=(loutbuff[i/2]>>8) & 0xff;
         } 
        else 
         {
          tempout[2*i]=routbuff[i/2]&0xff;
          tempout[2*i+1]=(routbuff[i/2]>>8)&0xff;
         } 
       
         for (i=0; i < 4*OBS; i++)
         	flag=fwrite(&tempout[i],1,1,fOut); 

     }
     
    EQ_delete(eqleft);
    EQ_delete(eqright);     
  
  
  }
  fclose(fIn);
  fclose(fOut);
}

