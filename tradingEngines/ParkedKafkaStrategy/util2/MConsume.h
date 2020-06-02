#ifndef _MCONSUME_DEFINE_H
#define _MCONSUME_DEFINE_H

typedef int (*pfnConsumeCB)(void* pthis, const char* topic, int partition, long long offset, int buflen, const char* pbuf);

class MConsumeImpl;
class MConsume
{
public:
	MConsume();
	~MConsume();

public:
	int				Init(const char* brokers, const char* group, pfnConsumeCB pfnCB, void* pthis);
	void			Release();

	void			Subscribe(int topicCount, char* argv[]);

	void			PeriodCall();

protected:
	MConsumeImpl*	m_pImpl;
	pfnConsumeCB	m_pfn;
	void*			m_pthis;
};




#endif


















