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
  			Uint16	hard_type;				//Ӳ������		
  			Uint16 	proto_type;				//Э������
  			Uint16 	proto_hard_length;		//Ӳ����Э���ַ����
  			Uint16 	op_code;				//�����ֶ�
  			struct 	mac    send_macaddr;	//���Ͷ���̫����ַ	
  			struct 	ipaddr send_ipaddr;		//���Ͷ�IP��ַ
  			struct 	mac    rec_macaddr;		//���ն���̫����ַ
  			struct 	ipaddr rec_ipaddr;		//���ն�IP��ַ
};

struct Socket_Type{
                   	Uint16   	My_Port; 		 		//�����˿�
		   			Uint16  	Dest_Port;		 		//�Է��˿�
		   			Uint16  	Dest_Ip[2];	 			//�Է�ip
		   			Uint16  	Dest_Mac_Id[3]; 		//�Է�����̫����ַ
		   			Uint32	 	IRS;			 		//��ʼ��˳���
                   	Uint32	 	ISS;			 		//�ҵĳ�ʼ�����к�
                   	Uint32	 	Rcv_Next;		 		//�Է���˳���
		   			Uint32	 	Send_Next;		 		//�ҵ��Ѿ�����˳���
                   	Uint32	 	Sent_UnAck;		 		//�ҵĻ�û��ȷ��˳���
		   									 		//unsigned long dest_ack_number;
		   			Uint16	 	Rcv_Window;		 		//�Է���window��С
                   	Uint16	 	Snd_Window; 		 	//�ҵ�window��С
        	   		Uint16	 	Dest_Max_Seg_Size;		//�Է����ܵ��������ݰ���СMTU
                   	Uint16	 	My_Max_Seg_Size; 		//���ܽ��ܵ��������ݰ���С
		   			Uint32	 	My_Wl1;		//seq
		   			Uint32	 	My_Wl2;		//ack
                   	Uint16	 	State;		//����״̬
		   			Uint16	 	Open;
                  };

//========================== end ==========================
