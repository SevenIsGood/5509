#include "Image.h"


#define IMAGEWIDTH 80
#define IMAGEHEIGHT 80
#define MODEGRAYBAR 0
#define MODEGRAY 1
#define MODEPHOTO1 2
#define MODEPHOTO2 3
#define MODEPHOTO3 4
#define MODEPHOTO4 5

unsigned char dbImage[IMAGEWIDTH*IMAGEHEIGHT];
unsigned char dbTargetImage[IMAGEWIDTH*IMAGEHEIGHT];



/* 图像锐化实验程序 */
int main()
{
	InitImage(MODEGRAYBAR, dbImage, IMAGEWIDTH, IMAGEHEIGHT);
	Sobel(dbImage, dbTargetImage, IMAGEWIDTH, IMAGEHEIGHT);

	InitImage(MODEGRAY, dbImage, IMAGEWIDTH, IMAGEHEIGHT);		//BreakPoint
	Sobel(dbImage, dbTargetImage, IMAGEWIDTH, IMAGEHEIGHT);

	InitImage(MODEPHOTO1, dbImage, IMAGEWIDTH, IMAGEHEIGHT);	//BreakPoint
	Sobel(dbImage, dbTargetImage, IMAGEWIDTH, IMAGEHEIGHT);

	InitImage(MODEPHOTO2, dbImage, IMAGEWIDTH, IMAGEHEIGHT);	//BreakPoint
	Sobel(dbImage, dbTargetImage, IMAGEWIDTH, IMAGEHEIGHT);

	InitImage(MODEPHOTO3, dbImage, IMAGEWIDTH, IMAGEHEIGHT);	//BreakPoint
	Sobel(dbImage, dbTargetImage, IMAGEWIDTH, IMAGEHEIGHT);

	InitImage(MODEPHOTO4, dbImage, IMAGEWIDTH, IMAGEHEIGHT);	//BreakPoint
	Sobel(dbImage, dbTargetImage, IMAGEWIDTH, IMAGEHEIGHT);

	while (1);													//BreakPoint
}

