
/****************************************************************/
/*	文件名:		Global_Maco.h 									*/
/* 	功能:		声明5509片上usb的各种配置寄存器宏				*/
/*  设计日期:	10/20/2004										*/
/****************************************************************/

#ifndef	Global_Maco_h
#define Global_Maco_h

#define	DSCR_DEVICE		1   		/* 描述表的类型: Device */
#define	DSCR_CONFIG		2   		/* 描述表的类型: Configuration */
#define	DSCR_STRING		3   		/* 描述表的类型: String */
#define	DSCR_INTRFC		4   		/* 描述表的类型: Interface */
#define	DSCR_ENDPNT		5   		/* 描述表的类型: Endpoint */
#define	DSCR_DEVQUAL	6   		/* 描述表的类型: Device Qualifier */

#define	DSCR_DEVICE_LEN		18
#define	DSCR_CONFIG_LEN		9
#define	DSCR_INTRFC_LEN		9
#define	DSCR_ENDPNT_LEN		7
#define	DSCR_DEVQUAL_LEN	10

#define DSCR_CONFIG_LEN_LINK	(14*DSCR_ENDPNT_LEN)	+ \
								DSCR_INTRFC_LEN			+ \
								DSCR_CONFIG_LEN

#define	ET_CONTROL		0   		/* 节点的类型: Control */
#define	ET_ISO			1   		/* 节点的类型: Isochronous */
#define	ET_BULK			2   		/* 节点的类型: Bulk */
#define	ET_INT			3   		/* 节点的类型: Interrupt */

#define DESCTBL_LEN DSCR_DEVICE_LEN		 + \
					DSCR_DEVQUAL_LEN	 + \
					DSCR_CONFIG_LEN		 + \
					DSCR_INTRFC_LEN		 + \
					(4*DSCR_ENDPNT_LEN)  + \
					DSCR_CONFIG_LEN      + \
					DSCR_INTRFC_LEN      + \
					(4*DSCR_ENDPNT_LEN)  + \
					2+2+2+(2*7)+2+(2*10)
					
/* 定义模块基地址 */
#define USBMOD_ADDR		0x5800

/* 中断使能寄存器地址 */
#define USBIE_ADDR  	  0x67FD
	#define USB_RSTR  	  0x0080
	#define USB_SUSR	  0x0040
	#define USB_RESR      0x0020
	#define USB_SOF       0x0010
	#define USB_PSOF      0x0008
	#define USB_SETUP     0x0004
	#define USB_STPOW     0x0001

/* 定义USB操作寄存器宏 */
#define USBINTF_ADDR    (*(volatile ioport Uint16*)0x67FE)	// 中断标志寄存器地址
#define USBINTS_ADDR    (*(volatile ioport Uint16*)0x6792)	// 中断源寄存器地址 
#define USBIEPIF_ADDR	(*(volatile ioport Uint16*)0x6793)	// 输入节点中断标志寄存器
#define	USBIEPIE_ADDR	(*(volatile ioport Uint16*)0x679B) 	// 输入节点中断使能寄存器
#define	USBOEPIF_ADDR	(*(volatile ioport Uint16*)0x6794) 	// 输出节点中断标志寄存器 
#define	USBOEPIE_ADDR	(*(volatile ioport Uint16*)0x679C)	// 输出节点中断使能寄存器

#define USBOD1CTL_ADDR (*(volatile ioport Uint16*)0x5808) 	// 输出节点1DMA控制寄存器
#define USBOD2CTL_ADDR (*(volatile ioport Uint16*)0x5810) 	// 输出节点2DMA控制寄存器
#define USBOD3CTL_ADDR (*(volatile ioport Uint16*)0x5818) 	// 输出节点3DMA控制寄存器
#define USBOD4CTL_ADDR (*(volatile ioport Uint16*)0x5820) 	// 输出节点4DMA控制寄存器
#define USBOD5CTL_ADDR (*(volatile ioport Uint16*)0x5828) 	// 输出节点5DMA控制寄存器
#define USBOD6CTL_ADDR (*(volatile ioport Uint16*)0x5830) 	// 输出节点6DMA控制寄存器
#define USBOD7CTL_ADDR (*(volatile ioport Uint16*)0x5838) 	// 输出节点7DMA控制寄存器
	#define DMA_GO		0x01
	#define DMA_STP		0x02
	#define DMA_RLD		0x04
	#define DMA_OVF		0x08
	#define DMA_END		0x10
	#define DMA_CAT		0x20
	#define DMA_SHT		0x40
	#define DMA_EM		0x80	
	
#define USBO1CTX_ADDR (*(volatile ioport Uint16*)0x670A)	// 输出节点1缓冲大小寄存器
#define USBO2CTX_ADDR (*(volatile ioport Uint16*)0x6712)	// 输出节点2缓冲大小寄存器
#define USBO3CTX_ADDR (*(volatile ioport Uint16*)0x671A)	// 输出节点3缓冲大小寄存器
#define USBO4CTX_ADDR (*(volatile ioport Uint16*)0x6722)	// 输出节点4缓冲大小寄存器
#define USBO5CTX_ADDR (*(volatile ioport Uint16*)0x672A)	// 输出节点5缓冲大小寄存器
#define USBO6CTX_ADDR (*(volatile ioport Uint16*)0x6732)	// 输出节点6缓冲大小寄存器
#define USBO7CTX_ADDR (*(volatile ioport Uint16*)0x673A)	// 输出节点7缓冲大小寄存器

#define USBO1XBUF_ADDR (*(volatile ioport Uint16*)0x6709)	// 输出节点1x缓冲寄存器基地址
#define USBO2XBUF_ADDR (*(volatile ioport Uint16*)0x6711)	// 输出节点2x缓冲寄存器基地址
#define USBO3XBUF_ADDR (*(volatile ioport Uint16*)0x6719)	// 输出节点3x缓冲寄存器基地址
#define USBO4XBUF_ADDR (*(volatile ioport Uint16*)0x6721)	// 输出节点4x缓冲寄存器基地址
#define USBO5XBUF_ADDR (*(volatile ioport Uint16*)0x6729)	// 输出节点5x缓冲寄存器基地址
#define USBO6XBUF_ADDR (*(volatile ioport Uint16*)0x6731)	// 输出节点6x缓冲寄存器基地址
#define USBO7XBUF_ADDR (*(volatile ioport Uint16*)0x6739)	// 输出节点7x缓冲寄存器基地址

#define USB_CTRL0_WRITE		0x7600		// 控制节点0写数据标志
#define USB_BULK1_WRITE		0x7601		// 输入块节点1向PC机写数据标志
#define USB_BULK2_WRITE		0x7602		// 输入块节点2向PC机写数据标志
#define USB_BULK3_WRITE		0x7603		// 输入块节点3向PC机写数据标志
#define USB_BULK4_WRITE		0x7604		// 输入块节点4向PC机写数据标志
#define USB_BULK5_WRITE		0x7605		// 输入块节点5向PC机写数据标志
#define USB_BULK6_WRITE		0x7606		// 输入块节点6向PC机写数据标志
#define USB_BULK7_WRITE		0x7607		// 输入块节点7向PC机写数据标志

#define USB_CTRL0_READ		0x7700		// 控制节点0读主机传来数据标志
#define USB_BULK1_READ		0x7701		// 输出块节点1读主机传来数据标识
#define USB_BULK2_READ		0x7702		// 输出块节点2读主机传来数据标识
#define USB_BULK3_READ		0x7703		// 输出块节点3读主机传来数据标识
#define USB_BULK4_READ		0x7704		// 输出块节点4读主机传来数据标识
#define USB_BULK5_READ		0x7705		// 输出块节点5读主机传来数据标识
#define USB_BULK6_READ		0x7706		// 输出块节点6读主机传来数据标识
#define USB_BULK7_READ		0x7707		// 输出块节点7读主机传来数据标识

#endif

/****************************************************************/
/*	No more													 	*/
/****************************************************************/
