#ifndef _RPCSTREAM_DEFINE_H
#define _RPCSTREAM_DEFINE_H

#include "./RpcDef.h"

typedef long long __int64;

class RpcRead
{
public:
	RpcRead();
	virtual ~RpcRead();
	void		BindStream(char* buf, int buflen);
	bool		IsError();
	int			GetOffset();
	
public:
	char		ReadInt8();
	short		ReadInt16();
	int			ReadInt32();
	__int64		ReadInt64();
	float		ReadFloat();
	double		ReadDouble();
	int			ReadSolid(char* outbuf, int insize);	//����ʵ�ʳ���
	void*		ReadSolidPtr();			//��ȡָ��(����Ϊnull)
	
protected:
	void*		ReadData(char* outbuf, int* outsize);
	
private:
	char*		m_pbuf;
	int			m_nbuflen;
	int			m_noffset;
	bool		m_berror;
};



class RpcWrite
{
public:
	RpcWrite();
	RpcWrite(char* buf, int buflen);
	virtual ~RpcWrite();
	
public:
	bool		IsError();
	int			GetOffset();
	void		Attach(char* buf, int buflen);
	
public:
	void		WriteInt8(char cdata);
	void		WriteInt16(short sdata);
	void		WriteInt32(int	ndata);
	void		WriteInt64(__int64 idata);
	void		WriteFloat(float fdata);
	void		WriteDouble(double ddata);
	void		WriteSolidPtr(char* pdata, int ofsize);
	void		WriteSolid(char* pdata, int ofsize);	//���Ȳ�����4095 = 0xFFF,(һ���ṹ��ĳ��Ȳ�����0xFFF)
	
protected:
	void		WriteData(int rdt, int sizeoftype, void* data);
	
private:
	char*		m_pbuf;
	int			m_nbuflen;
	int			m_noffset;
	bool		m_berror;
};


#endif
