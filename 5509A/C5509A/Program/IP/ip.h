#ifndef _IPH_
#define _IPH_
#ifndef _TI_STD_TYPES
#ifndef _TYPE_
#define _TYPE_
typedef	unsigned int		Uint16;
typedef	unsigned long   	Uint32;
#endif
#endif

//以下17个为寄存器指针，指向8019芯片的寄存器
int		*Reg00;
int		*Reg01;
int		*Reg02;
int		*Reg03;
int		*Reg04;
int		*Reg05;
int		*Reg06;
int		*Reg07;
int		*Reg08;
int		*Reg09;
int		*Reg0a;
int		*Reg0b;
int		*Reg0c;
int		*Reg0d;
int		*Reg0e;
int		*Reg0f;
int		*Reg10;


#define MY_TCP_PORT    	     1024
#define MY_UDP_PORT  	     1025

#define ETH_HEADER_START     0
#define IP_HEADER_START      7
#define ARP_HEADER_START     7
#define TCP_HEADER_START     17
#define UDP_HEADER_START     17
#define ICMP_HEADER_START    17
#define USER_DATA_START      27

#define RTL8019_HEADER_SIZE  2 
#define ETH_HEADER_SIZE      7
#define IP_HEADER_SIZE       10
#define TCP_HEADER_SIZE      10
#define UDP_HEADER_SIZE      4
#define ARP_FRAME_SIZE       14
#define ICMP_HEADER_SIZE     2
#define DUMMY_HEADER_SIZE    6
#define MY_MAX_SEG_SIZE      1460

#define Frame_ARP            0x0806
#define Frame_IP             0x0800
#define Ip_Edition           0x4500              //Ip 版本和IP首部长度
#define DEFUALT_TTL          128
#define ICMP_ECHO            8
#define ICMP_ECHO_REPLY      0
//ARP

#define HARDW_ETH            1
#define IP_HLEN_PLEN         0x0604
#define OP_ARP_REQUEST       1
#define OP_ARP_ANSWER        2

#define PROTOCOL_ICMP        1
#define PROTOCOL_TCP         6
#define PROTOCOL_UDP         17

/////TCP define

#define TCP_MAX_RE_TXDNUM    8

#define TCP_CODE_FIN         0x0001
#define TCP_CODE_SYN         0x0002
#define TCP_CODE_RST         0x0004
#define TCP_CODE_PSH         0x0008
#define TCP_CODE_ACK         0x0010
#define TCP_CODE_URG         0x0020

#define 	TCP_STATE_LISTEN        0
#define 	TCP_STATE_SYN_RCVD      1
#define 	TCP_STATE_SYN_SENT      2
#define 	TCP_STATE_ESTABLISHED   3
#define 	TCP_STATE_FIN_WAIT1     4
#define 	TCP_STATE_FIN_WAIT2     5
#define 	TCP_STATE_CLOSING       6
#define 	TCP_STATE_CLOSE_WAIT    7
#define 	TCP_STATE_LAST_ACK      8
#define 	TCP_STATE_CLOSED        9
#define 	TCP_STATE_TIME_WAIT     10
	
extern	void	SendFrame(Uint16	*buf,Uint16	len);
extern	Uint16	SwapWord(Uint16	data);

//prototypes

void TCP_Listen(void);
void TCP_Syn_Rec(void);
void TCP_Syn_Sent(void);
void TCP_Established(void);
void TCP_Close_Wait(void);
void TCP_Last_Ack(void);
void DoNetworkStuff(void);
void TCPActiveTxData(void);
void RTL8019ActiveOpen(void);
void Prepare_ICMP_Answer(void);
void Prepare_TCP_Frame(Uint16 TCPCode);
void CopyFrameFromBE(Uint16 Offset,Uint16 Size);
void ProcessEthBroadcastFrame(void);

#endif

//========================== end ==========================
