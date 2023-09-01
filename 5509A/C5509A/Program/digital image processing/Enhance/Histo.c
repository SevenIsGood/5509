
#define IMAGEWIDTH 80
#define IMAGEHEIGHT 80
#define MODEGRAYBAR 0
#define MODEGRAY 1
#define MODEPHOTO1 2
#define MODEPHOTO2 3
#define MODEPHOTO3 4
#define MODEPHOTO4 5

void InitImage(unsigned int nMode,unsigned char *pImage,int nWidth,int nHeight);
void Histogram(unsigned char *pImage,int nWidth,int nHeight,float fHisto[256]);
void Enhance(unsigned char *pImage,unsigned char *pImage1,int nWidth,int nHeight,float fHisto[256],float fHisto1[256]);

unsigned char dbImage[IMAGEWIDTH*IMAGEHEIGHT];
unsigned char dbTargetImage[IMAGEWIDTH*IMAGEHEIGHT];
float fHistogram[256],fHistogram1[256];
unsigned char lut[256];
	
/* 直方图统计实验程序 */
int main()
{
	InitImage(MODEGRAYBAR,dbImage,IMAGEWIDTH,IMAGEHEIGHT);
	Histogram(dbImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram);
	Enhance(dbImage,dbTargetImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram,fHistogram1);
	Histogram(dbTargetImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram1);
	
	InitImage(MODEGRAY,dbImage,IMAGEWIDTH,IMAGEHEIGHT);		//BreakPoint
	Histogram(dbImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram);
	Enhance(dbImage,dbTargetImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram,fHistogram1);
	Histogram(dbTargetImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram1);
	
	InitImage(MODEPHOTO1,dbImage,IMAGEWIDTH,IMAGEHEIGHT);	//BreakPoint
	Histogram(dbImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram);
	Enhance(dbImage,dbTargetImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram,fHistogram1);
	Histogram(dbTargetImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram1);
	
	InitImage(MODEPHOTO2,dbImage,IMAGEWIDTH,IMAGEHEIGHT);	//BreakPoint
	Histogram(dbImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram);
	Enhance(dbImage,dbTargetImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram,fHistogram1);
	Histogram(dbTargetImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram1);
	
	InitImage(MODEPHOTO3,dbImage,IMAGEWIDTH,IMAGEHEIGHT);	//BreakPoint
	Histogram(dbImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram);
	Enhance(dbImage,dbTargetImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram,fHistogram1);
	Histogram(dbTargetImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram1);
	
	InitImage(MODEPHOTO4,dbImage,IMAGEWIDTH,IMAGEHEIGHT);	//BreakPoint
	Histogram(dbImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram);
	Enhance(dbImage,dbTargetImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram,fHistogram1);
	Histogram(dbTargetImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram1);
	
	while (1);						//BreakPoint
}		

void Histogram(unsigned char *pImage,int nWidth,int nHeight,float fHisto[256])
{
   int i,j;
   unsigned int uWork;
   unsigned char *pWork;
   
	for ( i=0;i<256;i++ )	fHisto[i]=0.0f;
	pWork=pImage;
	for ( i=0;i<nHeight;i++ )
	{  
		for ( j=0;j<nWidth;j++,pWork++ )
		{
			uWork=(unsigned int)(*pWork);
			fHisto[uWork]++;
		}
	}
	uWork=nWidth*nHeight;
    for ( i=0;i<256;i++ )
    {
    	fHisto[i]/=uWork;
    	fHisto[i]*=100;
    }
}

void Enhance(unsigned char *pImage,unsigned char *pImage1,int nWidth,int nHeight,float fHisto[256],float fHisto1[256])
{
   int i,j;
  // unsigned int uWork;
   unsigned char *pWork,*pWork1;
   
	for ( i=0;i<256;i++ )
		fHisto1[i]=fHisto[i]/100;
	for ( i=1;i<256;i++ )
		fHisto1[i]+=fHisto1[i-1];
	for ( i=0;i<256;i++ )
		lut[i]=fHisto1[i]*256;
	for ( i=0;i<256;i++ )
		if ( lut[i]>=256 )
			lut[i]=255;
	pWork=pImage; pWork1=pImage1;
	for ( i=0;i<nHeight;i++ )
		for ( j=0;j<nWidth;j++,pWork++,pWork1++ )
			(*pWork1)=lut[(*pWork)];
}

