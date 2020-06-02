#ifndef _MRPCDEF_DEFINE_H
#define _MRPCDEF_DEFINE_H


#pragma pack(1)

enum rpcDataType
{
	rdt_int8,			//bool, char, uchar
	rdt_int16,			//short, ushort
	rdt_int32,			//int, uint
	rdt_int64,			//int64, uint64
	rdt_float,			//float
	rdt_double,			//double

	rdt_ptr = 14,		//��ָͨ�룬�ṹ��ָ�룬(���ָ��Ϊnull,����Ϊ0�� �����Ϊnull,��ṹ��)
	rdt_solid = 15,		//0x0F  //�ַ������ṹ�壬ʵ��
};




struct  tagRPCHead
{
	unsigned short		rdt:4;
	unsigned short		rlen:12;
};

#pragma pack()








#endif
