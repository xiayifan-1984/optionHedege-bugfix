#ifndef PUBSTRUCT_DEFINE_H
#define PUBSTRUCT_DEFINE_H

#pragma pack(1)

//�û���Ϣ
typedef struct
{
	int					Broker;
	char				User[32];
	char				Psw[32];
	char				UserProductInfo[32];
	char				AuthCode[32];
	char				QueryAddr[256];
}tagTradeUserInfo;


typedef struct
{
	int 				broker;
	char				userid[16];
}tagTDUserInfo;

typedef struct
{
	unsigned char  		maintype;			//������
	unsigned char  		childtype;			//������
	unsigned short  	size;				//������ĳ���
}tagComm_FrameHead, MAIN_FRAME_HEAD;


#pragma pack()















#endif

