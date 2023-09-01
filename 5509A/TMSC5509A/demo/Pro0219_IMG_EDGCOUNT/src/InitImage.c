#include<stdio.h>
#include "Image.h"

#define MODEGRAYBAR 0
#define MODEGRAY 1
#define MODEPHOTO1 2
#define MODEPHOTO2 3
#define MODEPHOTO3 4
#define MODEPHOTO4 5
#define MODEBLOCK 6
#define MODEINCLINE 7
#define MODEFPHOTO 8

#define GRAYBARLEVEL 16

void ReadImage(unsigned char *pImage,char *cFileName,int nWidth,int nHeight);

void InitImage(unsigned int nMode,unsigned char *pImage,int nWidth,int nHeight)
{
	int x,y,nWork,nWork1;
	unsigned char *pWork;
	int nPointx=nWidth/2;
	int nPointy=nHeight/2;
	float fWork;

	switch ( nMode )
	{
		case MODEGRAYBAR:
			pWork=pImage; nWork=256/GRAYBARLEVEL; nWork1=nHeight/GRAYBARLEVEL;
			for ( y=0;y<nHeight;y++ )
			{
				for ( x=0;x<nWidth;x++,pWork++ )
				{
					(*pWork)=(y/nWork1)*nWork;
				}
			}
			break;
		case MODEGRAY:
			pWork=pImage;
			nWork1=nHeight-nPointy; nWork=nWork1*nWork1;
			nWork1=nWidth-nPointx; nWork+=(nWork1*nWork1);
			nWork/=256;
			for ( y=0;y<nHeight;y++ )
			{
				for ( x=0;x<nWidth;x++,pWork++ )
				{
					nWork1=(x-nPointx)*(x-nPointx)+(y-nPointy)*(y-nPointy);
					nWork1=255-nWork1/nWork;
					if ( nWork1<0 )	nWork1=0;
					else if ( nWork1>255 )	nWork1=255;
					(*pWork)=nWork1;
				}
			}
			break;
		case MODEPHOTO1:
			ReadImage(pImage,"..\\DSP.bmp",nWidth,nHeight);
			break;
		case MODEPHOTO2:
			ReadImage(pImage,"..\\1.bmp",nWidth,nHeight);
			break;
		case MODEPHOTO3:
			ReadImage(pImage,"..\\2.bmp",nWidth,nHeight);
			break;
		case MODEPHOTO4:
			ReadImage(pImage,"..\\3.bmp",nWidth,nHeight);
			break;
		case MODEBLOCK:
			pWork=pImage;
			for ( y=0;y<nHeight;y++ )
				for ( x=0;x<nWidth;x++,pWork++ )
					(*pWork)=0;
			pWork=pImage+(nHeight/4-1)*nWidth;
			for ( y=nHeight/4;y<nHeight*3/4;y++,pWork+=nWidth )
				for ( x=nWidth/4;x<nWidth*3/4;x++ )
					pWork[x]=255;
			break;
		case MODEINCLINE:
			pWork=pImage;
			for ( y=0;y<nHeight;y++ )
				for ( x=0;x<nWidth;x++,pWork++ )
					(*pWork)=0;
			pWork=pImage;
			fWork=nWidth/nHeight;
			for ( y=0;y<nHeight/4;y++ )
				for ( x=nWidth/2-fWork*y;x<nWidth/2+fWork*y;x++ )
				{
					pWork[(y+nHeight/4)*nWidth+x]=255;
					pWork[(nHeight*3/4-y-1)*nWidth+x]=255;
				}
			break;
		case MODEFPHOTO:
			ReadImage(pImage,"..\\fourier.bmp",nWidth,nHeight);
			break;
		default:
			break;
	}
}

void ReadImage(unsigned char *pImage,char *cFileName,int nWidth,int nHeight)
{
	int j;
	unsigned char *pWork;
	FILE *fp;

	if ( fp=fopen(cFileName,"rb" ) )
	{
		fseek(fp,1078L,SEEK_SET);
		pWork=pImage+(nHeight-1)*nWidth;
		for ( j=0;j<nHeight;j++,pWork-=nWidth )
			fread(pWork,nWidth,1,fp);
		fclose(fp);
	}
}

