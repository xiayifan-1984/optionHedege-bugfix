#include  "RpcStream.h"
#include <stdlib.h>
#include <string.h>
//............................................................................................................................................................................................................................
//............................................................................................................................................................................................................................
//............................................................................................................................................................................................................................
//............................................................................................................................................................................................................................
//............................................................................................................................................................................................................................
RpcRead::RpcRead()
{
	m_pbuf =0;
	m_nbuflen =0;
	m_noffset =0;
	m_berror =false;
}

RpcRead::~RpcRead()
{

}

void	RpcRead::BindStream(char* buf, int buflen)
{
	m_pbuf = buf;
	m_nbuflen = buflen;
	m_noffset =0;
	m_berror =false;
}

bool	RpcRead::IsError()
{
	return m_berror;
}

int		RpcRead::GetOffset()
{
	return m_noffset;
}

char	RpcRead::ReadInt8()
{
	void* p = ReadData(0, 0);
	if (!m_berror && p)
	{
		char* t = (char*)p;
		return *t;
	}
	return -1;
}

short	RpcRead::ReadInt16()
{
	void* p = ReadData(0, 0);
	if (!m_berror && p)
	{
		short* t = (short*)p;
		return *t;
	}
	return -1;
}

int		RpcRead::ReadInt32()
{
	void* p = ReadData(0, 0);
	if (!m_berror && p)
	{
		int* t = (int*)p;
		return *t;
	}
	return -1;
}

__int64	RpcRead::ReadInt64()
{
	void* p = ReadData(0, 0);
	if (!m_berror && p)
	{
		__int64* t = (__int64*)p;
		return *t;
	}
	return -1;
}

float	RpcRead::ReadFloat()
{
	void* p = ReadData(0, 0);
	if (!m_berror && p)
	{
		float* t = (float*)p;
		return *t;
	}
	return -1;
}

double	RpcRead::ReadDouble()
{
	void* p = ReadData(0, 0);
	if (!m_berror && p)
	{
		double* t = (double*)p;
		return *t;
	}
	return -1;
}

int		RpcRead::ReadSolid(char* outbuf, int insize)
{
	int outsize = insize;
	void* p = ReadData(outbuf, &outsize);
	if (!m_berror)
	{
		return outsize;
	}
	return 0;
}

void*	RpcRead::ReadSolidPtr()
{
	void* ptr = ReadData(0, 0);
	if (!m_berror)
	{
		return ptr;
	}
	return 0;
}

void*	RpcRead::ReadData(char* outbuf, int* outsize)
{
	int nleftsize = m_nbuflen - m_noffset;
	int headsize = sizeof(tagRPCHead);
	if (nleftsize >headsize)
	{
		char* p = m_pbuf + m_noffset;
		tagRPCHead* pHead = (tagRPCHead*)p;
		int contentlen = pHead->rlen;

		if (nleftsize >= contentlen + headsize)
		{
			if (outsize)
			{
				if (outbuf && *outsize >=contentlen)
				{
					memcpy(outbuf, p+headsize, contentlen );
				}
				*outsize = contentlen;
			}
			
			m_noffset += contentlen + headsize;
			if (contentlen >0)
			{
				return (p+headsize);
			}
			else
			{
				return 0;
			}
		}
	}

	m_berror =true;
	return 0;
}

//............................................................................................................................................................................................................................
//............................................................................................................................................................................................................................
//............................................................................................................................................................................................................................
//............................................................................................................................................................................................................................
//............................................................................................................................................................................................................................
RpcWrite::RpcWrite()
{
	m_pbuf =0;
	m_nbuflen =0;
	m_noffset =0;
	m_berror =false;
}

RpcWrite::RpcWrite(char* buf, int buflen)
{
	m_pbuf = buf;
	m_nbuflen = buflen;
	m_noffset =0;
	m_berror =false;
}

RpcWrite::~RpcWrite()
{

}

void	RpcWrite::Attach(char* buf, int buflen)
{
	m_pbuf = buf;
	m_nbuflen = buflen;
	m_noffset =0;
	m_berror =false;
}

void	RpcWrite::WriteInt8(char cdata)
{
	WriteData(rdt_int8, sizeof(char), &cdata);
}

void	RpcWrite::WriteInt16(short sdata)
{
	WriteData(rdt_int16, sizeof(short), &sdata);
}

void	RpcWrite::WriteInt32(int	ndata)
{
	WriteData(rdt_int32, sizeof(int), &ndata);
}

void	RpcWrite::WriteInt64(__int64 idata)
{
	WriteData(rdt_int64, sizeof(__int64), &idata);
}

void	RpcWrite::WriteFloat(float fdata)
{
	WriteData(rdt_float, sizeof(float), &fdata);
}

void	RpcWrite::WriteDouble(double ddata)
{
	WriteData(rdt_double, sizeof(double), &ddata);
}

//编码方式为{Head|***}
void	RpcWrite::WriteData(int rdt, int sizeoftype, void* data)
{
	if (m_berror)
	{
		return;
	}

	short datasize = 0;
	if (data)
	{
		datasize =sizeoftype;
	}

	tagRPCHead ohead;
	ohead.rdt = rdt;
	ohead.rlen = sizeoftype;

	int headsize = sizeof(tagRPCHead);
	int encodelen = headsize + datasize;

	if (m_pbuf && m_nbuflen - m_noffset >= encodelen)
	{
		char* p = m_pbuf+m_noffset;
		memcpy(p, &ohead, headsize);

		if (data && sizeoftype >0)
		{
			memcpy(p+headsize, data, datasize);
		}
		
		m_noffset += encodelen;
		return;
	}

	m_berror = true;
}

void	RpcWrite::WriteSolidPtr(char* pdata, int ofsize)
{
	if (0 == pdata || ofsize <=0)
	{
		pdata =0;
		ofsize =0;
	}
	WriteData(rdt_ptr, ofsize, pdata);
}

//长度不超过4095 = 0xFFF 
//编码方式为 {Head|***}
void	RpcWrite::WriteSolid(char* pdata, int ofsize)
{
	if (ofsize >= 0xFFF)
	{
		return;
	}

	WriteData(rdt_solid, ofsize, pdata);
}

bool	RpcWrite::IsError()
{
	return m_berror;
}

int		RpcWrite::GetOffset()
{
	return m_noffset;
}







