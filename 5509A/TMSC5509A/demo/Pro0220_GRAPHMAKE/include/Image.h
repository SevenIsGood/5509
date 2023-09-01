#ifndef IMAGE_H_
#define IMAGE_H_

void InitImage(unsigned int nMode,unsigned char *pImage,int nWidth,int nHeight);
void Sobel(unsigned char *dbImage, unsigned char *dbTargetImage,int nWidth,int nHeight);

#endif