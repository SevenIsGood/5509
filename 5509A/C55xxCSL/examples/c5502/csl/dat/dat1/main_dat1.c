/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_dat1.c---------
 * This examples demonstrates the use of DAT APIs to fill up a buffer, which
 * can be viewed in CCS as an RGB-image (View->Graph->Image).
 *
 * The 'src' buffer is split into 3 parts of BUFF_SIZE elements each for
 * R, G and B color coding resp. Assuming BUFF_SIZE = 100 choose following
 * options to view the image from CCS menu (View->Graph->Image)
 * 		Color Space							:	RGB
 *		Interleaved Data Sources			:	No
 *		Start Address - R Source			:	src
 *		Start Address - G Source			:	src + 100
 *		Start Address - B Source			:	src + 200
 *		Page								:	DATA
 *		Lines Per Display					:	10
 *		Pixels Per Line						:	10
 *		Byte Packing to Fill 32 Bits		:	No
 *		Index Increment						:	1
 *		Image Origin						:	Bottom Left
 *		Uniform Quantization to 256 Colors	:	No
 *		Status Bar Display					: 	On
 *		Cursor Mode							: 	Zoom Cursor
 *		
 * After CCS displays the image, perform following steps:
 * 1)Click on it twice or thrice (since zoom cursor is selected) as suitable
 * 2)Right click and select -> Cursor Mode -> Data Cursor
 * 3)Right click and untick -> Float In Main Window
 * 4)Set break-points at following statements in 'main_dat1.c' (this file)
 *   in the order of their occurance.
 *    a) DAT_fill  b) DAT_copy  c) DAT_fill  d) DAT_copy2D  e) DAT_copy2D
 *    f) DAT_copy2D
 * 5)Adjust the image window size such that complete image is seen
 *
 * On running the program it will halt at above break-points and
 * the image color will change in the following order (1-to-1 mapping with
 * the break-points).
 *    a) Blue with shading  b) Green with shading  c) Yellow with shading
 *    d) Light red (pink) with shading e) First 1/5th area painted red
 *    f) Second 1/5th area painted red
 * and at the end of the program, entire image is painted red
 */
 
#include <stdio.h>

#include <csl.h>
#include <csl_dat.h>

//---------Global constants---------

//Assume that image contains 10x10 = 100 elements
#define BUFF_SIZE		100
#define ROW_SIZE		10
#define ROW_CNT			10

//---------Global data definition---------

//src[0............BUFF_SIZE-1]   = R component of the image
//src[BUFF_SIZE....2*BUFF_SIZE-1] = G component of the image
//src[2*BUFF_SIZE..3*BUFF_SIZE-1] = B component of the image
Uint16 src[BUFF_SIZE * 3];

//hiFillValue to enhance a color and loFillValue to reduce a color
Uint16 hiFillValue = 245;
Uint16 loFillValue = 5;

DAT_Handle myhDat;

//---------Function prototypes---------
void initBuff(Uint16 *in1, Uint16 len);

//---------main routine---------
void main()
{
	Uint16 i, success = TRUE;
	volatile Uint16 j;
	
	//Initialize CSL library
	CSL_init();
	
	//Initialize the image buffer
	initBuff(src, BUFF_SIZE);
	printf("Buffer initialized\n");
	
	//Open DAT channel and verify the handle
	myhDat = DAT_open(DAT_CHA0, DAT_PRI_HIGH, 0);	
	if (myhDat == INV)
	{
		printf("\nERROR : Invalid Handle, DAT_open failed\n");
		printf("\nTEST FAILED\n");
		exit(-1);
	}
	
	//Fill the G component of the buffer with hiFillValue to boost Green color
	DAT_fill(myhDat, (DMA_AdrPtr)(src + BUFF_SIZE), BUFF_SIZE, &hiFillValue);
	DAT_wait(myhDat);	
	printf("Green color enhanced\n");
	
	//Copy G component buffer to R component i.e. hiFillValue is copied 
	//Boosting Red color component
	DAT_copy(myhDat, (DMA_AdrPtr)(src + BUFF_SIZE), 
					(DMA_AdrPtr)(src), BUFF_SIZE);
	DAT_wait(myhDat);	
	printf("Red color enhanced\n");
	
	//Fill the G component of the buffer with loFillValue to reduce Green color
	DAT_fill(myhDat, (DMA_AdrPtr)(src + BUFF_SIZE), BUFF_SIZE, &loFillValue);
	DAT_wait(myhDat);	
	printf("Green color reduced\n");
	
	//---Copy loFillValue from Green color component to Blue color component---
	
	//Fill-up first 1/5th area i.e. only Red is displayed in this area
	DAT_copy2D(myhDat, DAT_1D2D, (DMA_AdrPtr)(src + BUFF_SIZE),
					 (DMA_AdrPtr)(src + 2*BUFF_SIZE), ROW_SIZE, ROW_CNT/5,
					 ROW_SIZE);
	DAT_wait(myhDat);
	printf("Blue color reduced for first 1/5th area\n");
	
	//Fill-up second 1/5th area i.e. only Red is displayed in this area
	DAT_copy2D(myhDat, DAT_2D1D, (DMA_AdrPtr)(src + 6*BUFF_SIZE/5),
					 (DMA_AdrPtr)(src + 11*BUFF_SIZE/5), ROW_SIZE, ROW_CNT/5,
					 ROW_SIZE);
	DAT_wait(myhDat);	
	printf("Blue color reduced for second 1/5th area\n");
	
	//Fill-up the remaining area i.e. entire image is displayed red
	DAT_copy2D(myhDat, DAT_2D2D, (DMA_AdrPtr)(src + 7*BUFF_SIZE/5),
					 (DMA_AdrPtr)(src + 12*BUFF_SIZE/5), ROW_SIZE, 3*ROW_CNT/5,
					 ROW_SIZE);
	DAT_wait(myhDat);	
	printf("Blue color reudced for the remaining area\n");
	
	//Close the DAT channel
	DAT_close(myhDat);
	
	//Verify if DAT transfer copied/filled correct values
	for (i = 0; i < BUFF_SIZE; i++)
	{
		if ((src[i] != hiFillValue) || 
		    (src[i + BUFF_SIZE] != loFillValue) ||
		    (src[i + 2*BUFF_SIZE] != loFillValue)
		   )
		{
			success = FALSE;
			break;
		}
	}
	
	if (success == FALSE) printf("\nTEST FAILED\n");
	else printf("\nTEST PASSED\n");
	
	return;		
}

//Function to initialize the image buffer
void initBuff(Uint16 *in1, Uint16 len)
{
	Uint16 i;
	
	for (i = 0; i < len; i++)
	{
		in1[i] = i;
		in1[len + i] = i + 255/4;
		in1[2 * len + i] = i + 255/2;
	}	
}
