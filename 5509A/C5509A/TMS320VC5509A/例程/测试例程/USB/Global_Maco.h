
/****************************************************************/
/*	�ļ���:		Global_Maco.h 									*/
/* 	����:		����5509Ƭ��usb�ĸ������üĴ�����				*/
/*  �������:	10/20/2004										*/
/****************************************************************/

#ifndef	Global_Maco_h
#define Global_Maco_h

#define	DSCR_DEVICE		1   		/* �����������: Device */
#define	DSCR_CONFIG		2   		/* �����������: Configuration */
#define	DSCR_STRING		3   		/* �����������: String */
#define	DSCR_INTRFC		4   		/* �����������: Interface */
#define	DSCR_ENDPNT		5   		/* �����������: Endpoint */
#define	DSCR_DEVQUAL	6   		/* �����������: Device Qualifier */

#define	DSCR_DEVICE_LEN		18
#define	DSCR_CONFIG_LEN		9
#define	DSCR_INTRFC_LEN		9
#define	DSCR_ENDPNT_LEN		7
#define	DSCR_DEVQUAL_LEN	10

#define DSCR_CONFIG_LEN_LINK	(14*DSCR_ENDPNT_LEN)	+ \
								DSCR_INTRFC_LEN			+ \
								DSCR_CONFIG_LEN

#define	ET_CONTROL		0   		/* �ڵ������: Control */
#define	ET_ISO			1   		/* �ڵ������: Isochronous */
#define	ET_BULK			2   		/* �ڵ������: Bulk */
#define	ET_INT			3   		/* �ڵ������: Interrupt */

#define DESCTBL_LEN DSCR_DEVICE_LEN		 + \
					DSCR_DEVQUAL_LEN	 + \
					DSCR_CONFIG_LEN		 + \
					DSCR_INTRFC_LEN		 + \
					(4*DSCR_ENDPNT_LEN)  + \
					DSCR_CONFIG_LEN      + \
					DSCR_INTRFC_LEN      + \
					(4*DSCR_ENDPNT_LEN)  + \
					2+2+2+(2*7)+2+(2*10)
					
/* ����ģ�����ַ */
#define USBMOD_ADDR		0x5800

/* �ж�ʹ�ܼĴ�����ַ */
#define USBIE_ADDR  	  0x67FD
	#define USB_RSTR  	  0x0080
	#define USB_SUSR	  0x0040
	#define USB_RESR      0x0020
	#define USB_SOF       0x0010
	#define USB_PSOF      0x0008
	#define USB_SETUP     0x0004
	#define USB_STPOW     0x0001

/* ����USB�����Ĵ����� */
#define USBINTF_ADDR    (*(volatile ioport Uint16*)0x67FE)	// �жϱ�־�Ĵ�����ַ
#define USBINTS_ADDR    (*(volatile ioport Uint16*)0x6792)	// �ж�Դ�Ĵ�����ַ 
#define USBIEPIF_ADDR	(*(volatile ioport Uint16*)0x6793)	// ����ڵ��жϱ�־�Ĵ���
#define	USBIEPIE_ADDR	(*(volatile ioport Uint16*)0x679B) 	// ����ڵ��ж�ʹ�ܼĴ���
#define	USBOEPIF_ADDR	(*(volatile ioport Uint16*)0x6794) 	// ����ڵ��жϱ�־�Ĵ��� 
#define	USBOEPIE_ADDR	(*(volatile ioport Uint16*)0x679C)	// ����ڵ��ж�ʹ�ܼĴ���

#define USBOD1CTL_ADDR (*(volatile ioport Uint16*)0x5808) 	// ����ڵ�1DMA���ƼĴ���
#define USBOD2CTL_ADDR (*(volatile ioport Uint16*)0x5810) 	// ����ڵ�2DMA���ƼĴ���
#define USBOD3CTL_ADDR (*(volatile ioport Uint16*)0x5818) 	// ����ڵ�3DMA���ƼĴ���
#define USBOD4CTL_ADDR (*(volatile ioport Uint16*)0x5820) 	// ����ڵ�4DMA���ƼĴ���
#define USBOD5CTL_ADDR (*(volatile ioport Uint16*)0x5828) 	// ����ڵ�5DMA���ƼĴ���
#define USBOD6CTL_ADDR (*(volatile ioport Uint16*)0x5830) 	// ����ڵ�6DMA���ƼĴ���
#define USBOD7CTL_ADDR (*(volatile ioport Uint16*)0x5838) 	// ����ڵ�7DMA���ƼĴ���
	#define DMA_GO		0x01
	#define DMA_STP		0x02
	#define DMA_RLD		0x04
	#define DMA_OVF		0x08
	#define DMA_END		0x10
	#define DMA_CAT		0x20
	#define DMA_SHT		0x40
	#define DMA_EM		0x80	
	
#define USBO1CTX_ADDR (*(volatile ioport Uint16*)0x670A)	// ����ڵ�1�����С�Ĵ���
#define USBO2CTX_ADDR (*(volatile ioport Uint16*)0x6712)	// ����ڵ�2�����С�Ĵ���
#define USBO3CTX_ADDR (*(volatile ioport Uint16*)0x671A)	// ����ڵ�3�����С�Ĵ���
#define USBO4CTX_ADDR (*(volatile ioport Uint16*)0x6722)	// ����ڵ�4�����С�Ĵ���
#define USBO5CTX_ADDR (*(volatile ioport Uint16*)0x672A)	// ����ڵ�5�����С�Ĵ���
#define USBO6CTX_ADDR (*(volatile ioport Uint16*)0x6732)	// ����ڵ�6�����С�Ĵ���
#define USBO7CTX_ADDR (*(volatile ioport Uint16*)0x673A)	// ����ڵ�7�����С�Ĵ���

#define USBO1XBUF_ADDR (*(volatile ioport Uint16*)0x6709)	// ����ڵ�1x����Ĵ�������ַ
#define USBO2XBUF_ADDR (*(volatile ioport Uint16*)0x6711)	// ����ڵ�2x����Ĵ�������ַ
#define USBO3XBUF_ADDR (*(volatile ioport Uint16*)0x6719)	// ����ڵ�3x����Ĵ�������ַ
#define USBO4XBUF_ADDR (*(volatile ioport Uint16*)0x6721)	// ����ڵ�4x����Ĵ�������ַ
#define USBO5XBUF_ADDR (*(volatile ioport Uint16*)0x6729)	// ����ڵ�5x����Ĵ�������ַ
#define USBO6XBUF_ADDR (*(volatile ioport Uint16*)0x6731)	// ����ڵ�6x����Ĵ�������ַ
#define USBO7XBUF_ADDR (*(volatile ioport Uint16*)0x6739)	// ����ڵ�7x����Ĵ�������ַ

#define USB_CTRL0_WRITE		0x7600		// ���ƽڵ�0д���ݱ�־
#define USB_BULK1_WRITE		0x7601		// �����ڵ�1��PC��д���ݱ�־
#define USB_BULK2_WRITE		0x7602		// �����ڵ�2��PC��д���ݱ�־
#define USB_BULK3_WRITE		0x7603		// �����ڵ�3��PC��д���ݱ�־
#define USB_BULK4_WRITE		0x7604		// �����ڵ�4��PC��д���ݱ�־
#define USB_BULK5_WRITE		0x7605		// �����ڵ�5��PC��д���ݱ�־
#define USB_BULK6_WRITE		0x7606		// �����ڵ�6��PC��д���ݱ�־
#define USB_BULK7_WRITE		0x7607		// �����ڵ�7��PC��д���ݱ�־

#define USB_CTRL0_READ		0x7700		// ���ƽڵ�0�������������ݱ�־
#define USB_BULK1_READ		0x7701		// �����ڵ�1�������������ݱ�ʶ
#define USB_BULK2_READ		0x7702		// �����ڵ�2�������������ݱ�ʶ
#define USB_BULK3_READ		0x7703		// �����ڵ�3�������������ݱ�ʶ
#define USB_BULK4_READ		0x7704		// �����ڵ�4�������������ݱ�ʶ
#define USB_BULK5_READ		0x7705		// �����ڵ�5�������������ݱ�ʶ
#define USB_BULK6_READ		0x7706		// �����ڵ�6�������������ݱ�ʶ
#define USB_BULK7_READ		0x7707		// �����ڵ�7�������������ݱ�ʶ

#endif

/****************************************************************/
/*	No more													 	*/
/****************************************************************/
