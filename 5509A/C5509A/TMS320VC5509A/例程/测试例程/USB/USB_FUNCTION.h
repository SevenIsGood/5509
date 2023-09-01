
/****************************************************************/
/*	�ļ���:		USB_FUNCTION.h 									*/
/* 	����:		����5509Ƭ��usb�����úͲ�������					*/
/*	�����		�����											*/
/*  �������:	10/20/2004										*/
/****************************************************************/

#ifndef USB_FUNCTION_h
#define USB_FUNCTION_h

/* �ڵ�����ݽṹ��ʼ�� */
extern USB_SetupStruct USB0_SetupPkt;				// ��ʼ��һ��Setup���ݽṹ

/* ���ƽڵ�0���¼��������� */
extern Uint16 EpIn0Event_mask;

extern Uint16 EpOut0Event_mask; 

extern Uint16 CtrlDataLen;							// ���ƽڵ�0�������򳤶�	
				   	  
extern USB_EpObj 	EndptObjOut0,	EndptObjIn0, 	// ��ʼ������endpoint0 
					EndptObjOut1,	EndptObjIn1, 	// ��ʼ��������ڵ�1-7
					EndptObjOut2,	EndptObjIn2, 
					EndptObjOut3,	EndptObjIn3,
					EndptObjOut4,	EndptObjIn4,
					EndptObjOut5,	EndptObjIn5,
					EndptObjOut6,	EndptObjIn6,
					EndptObjOut7,	EndptObjIn7;
					
/* ����ȫ�ֱ��� */
extern Uint16 usbbuffer[200];
extern Uint16 usbflag;
extern Uint16 usbreceilen;							//	�����ݴ��PC������������,�Ա����			
extern USB_EpHandle hEpObjArray[];

/****************************************************************/
/*	������:		USB_EndpointInit() 								*/
/* 	����:		��5509Ƭ�ϵ�USB�ڵ���г�ʼ��					*/
/*  ����:														*/
/* 				EpIn0EventMark---������ƽڵ��ж��¼��Ļ����	*/
/* 				EpOut0EventMark---������ƽڵ��ж��¼��Ļ����	*/
/****************************************************************/
void USB_EndpointInit(Uint16 EpIn0EventMark, Uint16 EpOut0EventMark);

/****************************************************************/
/*	������:		USB_EnableDInt() 								*/
/* 	����:		ʹ�ܿ�����ڵ�(OUT1-OUT7)�ж�					*/
/*  ����:														*/
/*  			hEp[]---ָ��һ��NULL��ֹ���Ѿ���ʼ���Ľڵ����	*/
/*						�������								*/
/****************************************************************/
extern void USB_EnableDInt(USB_EpHandle hEp[]);

/****************************************************************/
/*	������:		USB_ReceivData() 								*/
/* 	����:		����ָ�����������ڵ������					*/
/*  ����:														*/
/*  			EpNum----USB CSL����Ľڵ��					*/
/*				*DataPtr-ָ��������ݻ�������ָ��				*/
/*				*ByteCnt-ָ��洢���յ������ֽڸ����ı�����ַ	*/
/*	����ֵ:		����ɹ�,����USB_TURE,���򷵻�USB_FALSE			*/
/****************************************************************/
extern USB_Boolean USB_ReceivData(USB_EpNum EpNum, Uint16 *DataPtr, Uint16 *ByteCnt);

/****************************************************************/
/*	������:		USB_InitConfig() 								*/
/* 	����:		��5509Ƭ�ϵ�USB���г�ʼ������					*/
/*  ����:														*/
/*  			hEp[]---ָ��һ��NULL��ֹ���Ѿ���ʼ���Ľڵ����	*/
/*						�������								*/
/* 				EpIn0EventMark----������ƽڵ��ж��¼��Ļ����	*/
/* 				EpOut0EventMark---������ƽڵ��ж��¼��Ļ����	*/
/****************************************************************/
extern void USB_InitConfig(USB_EpHandle hEp[], Uint16 EpIn0EventMark, Uint16 EpOut0EventMark);

/****************************************************************/
/*	������:		USB_DefaultDeal() 								*/
/* 	����:		5509Ƭ�ϵ�USB������ѭ����,���ϵؼ��״̬��Ϣ,	*/
/*				���������Ӧ��ϵͳĬ��״̬����,��ִ����Ӧ�Ķ���	*/
/*  ����:														*/
/* 				��												*/
/****************************************************************/
extern void USB_DefaultDeal(void);

/****************************************************************/
/*	������:		USB_SendData() 									*/
/* 	����:		��ָ�������봫��ڵ㷢������					*/
/*  ����:														*/
/*  			hEp------ָ��һ���Ѿ���ʼ���Ľڵ����ľ��		*/
/*				ByteCnt--�����͵����ֽڸ���						*/
/*				*DataPtr-ָ�����ݻ�������ָ��					*/
/*				Flags----USB���ݴ����־�Ļ����				*/	
/*	����ֵ:		����ɹ�,����USB_TURE,���򷵻�USB_FALSE			*/
/****************************************************************/
extern USB_Boolean USB_SendData(USB_EpHandle hEp, Uint16 ByteCnt, void *DataPtr, USB_IOFLAGS Flags);

/****************************************************************/
/*	������:		USB_SendCtrlData() 								*/
/* 	����:		����ƽڵ�0����ָ�����ȵ�����					*/
/*  ����:														*/
/*				*DataPtr-ָ�������ݻ�������ָ��				*/
/*				ByteCnt-�������ݳ���				        	*/
/*				Flags----USB���ݴ����־�Ļ����				*/
/*	����ֵ:		����ɹ�,����USB_TURE,���򷵻�USB_FALSE			*/
/****************************************************************/
extern USB_Boolean USB_SendCtrlData(Uint16 *DataPtr, Uint16 ByteCnt, USB_IOFLAGS Flags);

/****************************************************************/
/*	������:		USB_ReceivCtrlData() 							*/
/* 	����:		�ӿ��ƽڵ�0����ָ�����ȵ�����					*/
/*  ����:														*/
/*				*DataPtr-ָ��������ݻ�������ָ��				*/
/*				ByteCnt-�������ݳ���				        	*/
/*				Flags----USB���ݴ����־�Ļ����				*/
/*	����ֵ:		����ɹ�,����USB_TURE,���򷵻�USB_FALSE			*/
/****************************************************************/
extern USB_Boolean USB_ReceivCtrlData(Uint16 *DataPtr, Uint16 ByteCnt, USB_IOFLAGS Flags);

#endif

/****************************************************************/
/*	No more													 	*/
/****************************************************************/
