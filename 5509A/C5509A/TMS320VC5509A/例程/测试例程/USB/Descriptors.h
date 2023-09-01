/**********************************************************************/
/*File:    descriptors.h											  */
/*Author:  duanlifeng   											  */
/*																	  */
/*����: Defines USB descriptor table constants						  */
/*																	  */
/*  ��Ȩ(c) 	2003-		�������ڴ���Ӽ����������ι�˾			  */
/**********************************************************************/

#ifndef __DESCRIPTORS_H__
#define	__DESCRIPTORS_H__

#include <stdio.h>
#include <csl.h>
#include <csl_usb.h>
#include <csl_chiphal.h>

/* �����ⲿ��������ݽṹ */
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

