#ifndef  _MPRODUCE_DEFINE_H
#define  _MPRODUCE_DEFINE_H

class MProduceImpl;
class MProduce
{
public:
	MProduce();
	~MProduce();

public:
	int						Init(const char* brokers);
	void					Release();
	int						Send(const char* topic, char* putf8buf, int nbuflen);
	void					PeriodCall();

protected:
	MProduceImpl*			m_pImp;

};



#endif
