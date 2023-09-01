/*
* TIesrBintoASCII.c
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
#include <stdlib.h>
#include <string.h>
char v1[100], v2[100], v3[100], v4[100]   ;
char vv4[100] ,v11[100], v12[100]   ;
FILE  *p_in, *p_out , *p_out2 ; 

/*************************************************************
*
*    Clear_bin function strip out the last 4 characters of a string
*    and converts any upper case to lower case. It also removes the .bin from  the file names 
*
************************************************************/
void clear_bin(char * in, char *out)
{
   int   i , j   ;

   i = (int) strlen(in)  ;
   for (j=0; j<i-4 ; j++)
   {
      *out++ =  tolower( (unsigned char) *in++ ) ;
   }
   in = in + 4  ;
      *out++ =  tolower( (unsigned char) *in++ ) ;
}

/**********************************************************
*   upperCase converts a string to uppercase. The input is any character string and
*   the output is the same string in upper case.
*  
*
**************************************************************/

void upperCase(char * in, char *OUT)
{
   int   i , j   ;
   i = (int) strlen(in)  ;
//   printf("Inside upper, length is  %d \n",i) ;
   for (j=0; j<i+1; j++)
   {
      *OUT++ =  toupper( (unsigned char) *in++ ) ;
   }
}


/********************************************************
*   This function prepares the include file. For each XXX.bin file
*   the code generates two files; gd_XXX.c and gd_XXX.h
*   This routine writes the include file gd_XXX.h
*
*********************************************************/
void writeIncludeFile(FILE *fp, char * name, int size)
{
   char NAME[100]  ;
   char Name[100]  ;
   strcpy (Name, name)   ;  //  
   *Name = toupper ( (unsigned char) *Name) ;
   upperCase (name, NAME) ;
   fprintf(fp, "#ifndef _GD_%s_H_\n",NAME) ;
   fprintf(fp,"#define _GD_%s_H_\n\n",NAME) ;
   fprintf(fp, "#include <tistdtypes.h> \n\n") ; 
   fprintf(fp,"#define  %s_SZ (%d) \n\n", NAME, size) ;
   fprintf(fp, " extern  const ") ;
   fprintf(fp, "   Int16 g%s[%s_SZ] ; \n\n",Name,NAME) ;
   fprintf(fp,"#endif   \n\n") ;
 }


/********************************************************
*   This function prepares the top of the gd_XXX.c file. i
*  For each XXX.bin file  the code generates two files; gd_XXX.c 
*  and gd_XXX.h
*   This routine writes the top of the  file gd_XXX.c
*
*********************************************************/

void writeC_HeaderFile(FILE *fp, char * name)
{
   char NAME[100]  ;
   char Name[100]  ;
   char *p_NAME  ;
   char *p_Name  ;
   p_NAME = &NAME[0] ;
   p_Name = &Name[0] ;

   strcpy (p_Name, name)   ;  //  
   *Name = toupper ( (unsigned char) *Name) ;
   upperCase (name, p_NAME) ;
   fprintf(fp, "#include <tistdtypes.h> \n\n") ; 
   fprintf(fp,"#include \"gd_%s.h\" \n\n",name) ;
   fprintf(fp,"#") ;
   fprintf(fp,"pragma DATA_SECTION (g%s,\".gd_%s\" ) \n ",p_Name,name) ;
   fprintf(fp, "const Int16 g%s [",p_Name)  ;
   fprintf(fp, "%s",p_NAME)  ;
   fprintf(fp, "_SZ]= \n ")  ;
   fprintf(fp,"{  \n") ;  
 }




union u_short {
      short a   ;
      char b[2]  ;
   }  u   ;


int main(int argc, char * argv[])
{
	char a1,b1,c1   ;
	int a,b,c,d,e   ;
	char *p_a, *p_b, *p_c, *p_d  ;
	short x1, x2, x3   ;
	short *p_x1, *p_x2, *p_x3   ;
	long y1, y2   ;
        char xx[100] ,yy[100]  ;
        int sz   ;
	int i,j,k   ;
        char *baseName =   &v1[0]  ;
        char *baseName1 =   &v11[0]  ;
        char *baseName2 =   &v12[0]  ;
        char *inFileName1 = &v2[0] ;
        char *outFileName1 =&v3[0] ;
        char *outFileName2 =&vv4[0]  ;
        int flag = 0  ;
//  Step 1 - get the base name into xx
       strcpy(yy, argv[1]) ;
       clear_bin(yy, xx) ;
        strcpy(baseName, xx) ;
      strcpy (yy,"word") ;
      if ( strcmp (xx,yy ) == 0)
      {
        inFileName1 = strcat (baseName,".lis") ;  
        flag = 1 ;   //    for the word case
      }
      else
      {
        inFileName1 = strcat (baseName,".bin") ;  
      }
//  Step 2 - build the input file (XXX.bin) and open it
	//printf("\ninput file name is  %s  \n", inFileName1) ;

       p_in = fopen(inFileName1,"rb") ;
      if (p_in == NULL)
      {
          printf("unable to open file %s \n", inFileName1) ;
      }

//      Generate two names, the .c file and the .h file

        //baseName = strcpy(baseName, xx) ;

        strcpy( baseName1 , "gd_")  ;
        strcat(baseName1, xx) ;
        outFileName1 = strcat (baseName1,".c") ;  


        strcpy( baseName2 , "gd_")  ;
         strcat(baseName2, xx) ;
        outFileName2 = strcat (baseName2,".h") ;  

//   Open the two output files, gd_XXX.c and gd_XXX.h 

    p_out = fopen(outFileName1,"w") ;
   if (p_out == NULL)
   {
       printf("unable to open file %s \n", outFileName1) ;
       exit(1) ;
   }

    p_out2 = fopen(outFileName2,"w") ;
   if (p_out2 == NULL)
   {
       printf("unable to open file %s \n", outFileName2) ;
       exit(1) ;
   }
//////////////////////////////////////////////////////////////
/*        Build the include file       */
//    find the number of characters in teh input file

     fseek(p_in, 0L, SEEK_END);
     if (flag == 1)
        sz = ftell(p_in);  //    8-bits 
     else
        sz = ftell(p_in)/2;  //  convert to short from 8-bit  
    fseek(p_in, 0L, SEEK_SET);

   writeIncludeFile(p_out2, xx,sz ) ;

   fclose (p_out2)   ;

//  Build the gd_XXX.c file in three steps -
//  First the top part of the file is written
//  Next the values are calculated and change the endians
//  Last write the end of the file



   writeC_HeaderFile(p_out, xx)  ;
   if (flag == 1)
   {
      for (i=0; i<sz-1; i++)
      {
       fread(&u.b[0],1,1,p_in);
       fprintf(p_out,"%d , \n",u.b[0]) ;
      }

       fread(&u.b[0],1,1,p_in);
       fprintf(p_out,"%d  \n",u.b[0]) ;

   }
   else
   {

      for (i=0; i<sz-1; i++)
      {
       fread(&u.a,2,1,p_in);
       a1 = u.b[0]  ;
       u.b[0] = u.b[1] ;
       u.b[1] = a1  ;
       fprintf(p_out,"%d , \n",u.a) ;
      }

       fread(&u.a,2,1,p_in);
       a1 = u.b[0]  ;
       u.b[0] = u.b[1] ;
       u.b[1] = a1  ;
       fprintf(p_out,"%d  \n",u.a) ;
   }


    fprintf(p_out," } ; \n\n") ;

   printf("  DONE processing  %s file \n", inFileName1)  ;

   fclose (p_out)   ;

	return 0;
}


