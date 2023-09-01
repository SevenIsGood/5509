#define IMAGEWIDTH 80
#define IMAGEHEIGHT 80

extern unsigned char dbImage[IMAGEWIDTH*IMAGEHEIGHT];
extern unsigned char dbTargetImage[IMAGEWIDTH*IMAGEHEIGHT];

int mi,mj;
unsigned int m_nWork;
unsigned char *pImg,*pImg1;

void Reverse(int nWidth,int nHeight)
{
	pImg=dbImage; pImg1=dbTargetImage;
	for ( mj=0;mj<nHeight;mj++ )
		for ( mi=0;mi<nWidth;mi++,pImg++,pImg1++ )
			(*pImg1)=(~(*pImg))&0x0ff;
}


