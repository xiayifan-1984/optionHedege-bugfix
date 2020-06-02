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

	rdt_ptr = 14,		//普通指针，结构体指针，(如果指针为null,长度为0， 如果不为null,填结构体)
	rdt_solid = 15,		//0x0F  //字符串，结构体，实体
};




struct  tagRPCHead
{
	unsigned short		rdt:4;
	unsigned short		rlen:12;
};

#pragma pack()








#endif
