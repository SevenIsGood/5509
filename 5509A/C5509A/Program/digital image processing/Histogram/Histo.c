
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

unsigned char dbImage[IMAGEWIDTH*IMAGEHEIGHT];
float fHistogram[256];
	
/* 直方图统计实验程序 */
int main()
{
	InitImage(MODEGRAYBAR,dbImage,IMAGEWIDTH,IMAGEHEIGHT);
	Histogram(dbImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram);
	
	InitImage(MODEGRAY,dbImage,IMAGEWIDTH,IMAGEHEIGHT);		//BreakPoint
	Histogram(dbImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram);
	
	InitImage(MODEPHOTO1,dbImage,IMAGEWIDTH,IMAGEHEIGHT);	//BreakPoint
	Histogram(dbImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram);
	
	InitImage(MODEPHOTO2,dbImage,IMAGEWIDTH,IMAGEHEIGHT);	//BreakPoint
	Histogram(dbImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram);
	
	InitImage(MODEPHOTO3,dbImage,IMAGEWIDTH,IMAGEHEIGHT);	//BreakPoint
	Histogram(dbImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram);
	
	InitImage(MODEPHOTO4,dbImage,IMAGEWIDTH,IMAGEHEIGHT);	//BreakPoint
	Histogram(dbImage,IMAGEWIDTH,IMAGEHEIGHT,fHistogram);
	
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


