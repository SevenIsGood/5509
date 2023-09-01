/**********************************************************************/
/*File:    descriptors.h											  */
/*Author:  duanlifeng   											  */
/*																	  */
/*描述: Defines USB descriptor table constants						  */
/*																	  */
/*  版权(c) 	2003-		北京合众达电子技术有限责任公司			  */
/**********************************************************************/

#ifndef __DESCRIPTORS_H__
#define	__DESCRIPTORS_H__

#include <stdio.h>
#include <csl.h>
#include <csl_usb.h>
#include <csl_chiphal.h>

/* 声明外部定义的数据结构 */
extern char Device_Description[20];
extern char Device_Qualifier[11];
extern Uint16 Device_Config[6];
extern USB_DataStruct usb_configuration_link;
extern Uint16 String_descriptor_langid[4];
extern Uint16 String_descriptor[12];
extern char String_descriptor1[40];
extern char String_descriptor2[24];
					
#endif

/****************************************************************/
/*	No more													 	*/
/****************************************************************/

