#define IMAGEWIDTH 80
#define IMAGEHEIGHT 80

extern unsigned char dbImage[IMAGEWIDTH*IMAGEHEIGHT];
extern unsigned char dbTargetImage[IMAGEWIDTH*IMAGEHEIGHT];

int mi,mj,m_nWork1,m_nWork2;
unsigned int m_nWork,*pWork;
unsigned char *pImg1,*pImg2,*pImg3,*pImg;
unsigned int x1,x2,x3,x4,x5,x6,x7,x8,x9;

void Laplace(int nWidth,int nHeight)
{
	int i;
	
	pImg=dbTargetImage;
	for ( i=0;i<IMAGEWIDTH;i++,pImg++ )
		(*pImg)=0;
	(*pImg)=0;
	pImg1=dbImage;
	pImg2=pImg1+IMAGEWIDTH;
	pImg3=pImg2+IMAGEWIDTH;
	for ( i=2;i<nHeight;i++ )
	{
		pImg++;
		x1=(*pImg1); pImg1++; x2=(*pImg1); pImg1++;
		x4=(*pImg2); pImg2++; x5=(*pImg2); pImg2++;
		x7=(*pImg3); pImg3++; x8=(*pImg3); pImg3++;
		for ( mi=2;mi<nWidth;mi++,pImg++,pImg1++,pImg2++,pImg3++ )
		{
			x3=(*pImg1); x6=(*pImg2); x9=(*pImg3);
			m_nWork1=x5<<2; m_nWork1+=x5;
			m_nWork2=x2+x4+x6+x8;
//			m_nWork1=x5<<3; m_nWork1+=x5;
//			m_nWork2=x1+x2+x3+x4+x6+x7+x8+x9;
			m_nWork1-=m_nWork2;
			if ( m_nWork1>255 )	m_nWork1=255;
			else if ( m_nWork1<0 )	m_nWork1=0;
			(*pImg)=m_nWork1;
			x1=x2; x2=x3;
			x4=x5; x5=x6;
			x7=x8; x8=x9;
		}
		(*pImg)=0; pImg++;
	}
}


