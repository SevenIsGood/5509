#ifndef _TI_STD_TYPES
#ifndef _TYPE_
#define _TYPE_
typedef	unsigned int		Uint16;
typedef	unsigned long   	Uint32;
#endif
#endif

#define 	BroadCast     1
#define 	RequestArp    2
#define		AnswerArp	  3
#define 	Nod           4
#define 	ARP	    	  1
#define 	UDP     	  2
#define 	IGMP    	  3
#define 	LSS           4
#define		TCP			  5
#define		ICMP		  6

#define		IP_FRAME	0X0800
#define		ARP_FRAME	0X0806
	

struct	ipaddr
		{
  			Uint16 	addr2_1;
  			Uint16	addr4_3;	
		};
struct 	mac
		{
  			Uint16	addr2_1;
  			Uint16	addr4_3;
  			Uint16 	addr6_5;
		};

struct 	iphdr
		{
  			Uint16  tos_version;
  			Uint16 	tol_len;
  			Uint16 	id;
  			Uint16	frag_off;
  			Uint16 	protocal_ttl;
  			Uint16	chksum;
  			struct 	ipaddr saddr;
  			struct 	ipaddr daddr;
		};
struct 	udphdr
		{
  			Uint16 	sport;
  			Uint16 	dport;
  			Uint16 	length;
  			Uint16 	chksum;
		};

struct 	igmphdr
		{
  			Uint16	type_mrt;
  			Uint16  chksum;
  			struct  ipaddr groupaddr;
		};
struct 	pre_udphdr
		{
  			struct 	ipaddr saddr;
  			struct 	ipaddr daddr;
  			Uint16	protocal_value;
  			Uint16  length;
};
struct 	arp
		{
  			Uint16	hard_type;				//硬件类型		
  			Uint16 	proto_type;				//协议类型
  			Uint16 	proto_hard_length;		//硬件及协议地址长度
  			Uint16 	op_code;				//操作字段
  			struct 	mac    send_macaddr;	//发送端以太网地址	
  			struct 	ipaddr send_ipaddr;		//发送断IP地址
  			struct 	mac    rec_macaddr;		//接收端以太网地址
  			struct 	ipaddr rec_ipaddr;		//接收端IP地址
};

struct Socket_Type{
                   	Uint16   	My_Port; 		 		//本机端口
		   			Uint16  	Dest_Port;		 		//对方端口
		   			Uint16  	Dest_Ip[2];	 			//对方ip
		   			Uint16  	Dest_Mac_Id[3]; 		//对方的以太网地址
		   			Uint32	 	IRS;			 		//初始化顺序号
                   	Uint32	 	ISS;			 		//我的初始化序列号
                   	Uint32	 	Rcv_Next;		 		//对方的顺序号
		   			Uint32	 	Send_Next;		 		//我的已经发送顺序号
                   	Uint32	 	Sent_UnAck;		 		//我的还没有确认顺序号
		   									 		//unsigned long dest_ack_number;
		   			Uint16	 	Rcv_Window;		 		//对方的window大小
                   	Uint16	 	Snd_Window; 		 	//我的window大小
        	   		Uint16	 	Dest_Max_Seg_Size;		//对方接受的最大的数据包大小MTU
                   	Uint16	 	My_Max_Seg_Size; 		//我能接受的最大的数据包大小
		   			Uint32	 	My_Wl1;		//seq
		   			Uint32	 	My_Wl2;		//ack
                   	Uint16	 	State;		//连接状态
		   			Uint16	 	Open;
                  };

//========================== end ==========================
