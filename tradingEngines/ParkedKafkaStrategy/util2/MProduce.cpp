
#include "MProduce.h"

#include "librdkafka/rdkafka.h"
#include <string>
#include <map>
using namespace std;

class MProduceImpl
{
public:
	MProduceImpl()
	{
		m_rk = nullptr;
		m_conf = nullptr;
	}

	~MProduceImpl()
	{

	}

	int		init(const char* brokers)
	{
		char errstr[512] = { 0 };       /* librdkafka API error reporting buffer */

										//[1]
		m_conf = rd_kafka_conf_new();

		/* Set bootstrap broker(s) as a comma-separated list of
		* host or host:port (default port 9092).
		* librdkafka will use the bootstrap brokers to acquire the full
		* set of brokers from the cluster. */
		if (rd_kafka_conf_set(m_conf, "bootstrap.servers", brokers, errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
		{
			fprintf(stderr, "%s\n", errstr);
			return (-1);
		}

		/* Set the delivery report callback.
		* This callback will be called once per message to inform
		* the application if delivery succeeded or failed.
		* See dr_msg_cb() above. */
		rd_kafka_conf_set_dr_msg_cb(m_conf, dr_msg_cb);

		//[2]
		/*
		* Create producer instance.
		*
		* NOTE: rd_kafka_new() takes ownership of the conf object
		*       and the application must not reference it again after
		*       this call.
		*/
		m_rk = rd_kafka_new(RD_KAFKA_PRODUCER, m_conf, errstr, sizeof(errstr));
		if (!m_rk)
		{
			fprintf(stderr, "%% Failed to create new producer: %s\n", errstr);
			return (-2);
		}

		return 1;
	}

	void		release()
	{
		/* Wait for final messages to be delivered or fail.
		* rd_kafka_flush() is an abstraction over rd_kafka_poll() which
		* waits for all messages to be delivered. */
		fprintf(stderr, "%% Flushing final messages..\n");
		rd_kafka_flush(m_rk, 10 * 1000 /* wait for max 10 seconds */);

		map<string, rd_kafka_topic_t*>::iterator itb = m_map2rkt.begin();
		while (itb != m_map2rkt.end())
		{
			rd_kafka_topic_t* p = itb->second;

			/* Destroy topic object */
			rd_kafka_topic_destroy(p);

			itb++;
		}
		m_map2rkt.clear();

		/* Destroy the producer instance */
		rd_kafka_destroy(m_rk);

	}

	static  void		dr_msg_cb(rd_kafka_t *rk, const rd_kafka_message_t *rkmessage, void *opaque)
	{
		if (rkmessage->err)
		{
			fprintf(stderr, "%% Message delivery failed: %s\n", rd_kafka_err2str(rkmessage->err));
		}
		else
		{
			fprintf(stderr, "Message delivered (%d bytes, partition %d)\n",
				rkmessage->len, rkmessage->partition);
		}

		/* The rkmessage is destroyed automatically by librdkafka */
	}


	void	do_run()
	{
		/* A producer application should continually serve
		* the delivery report queue by calling rd_kafka_poll()
		* at frequent intervals.
		* Either put the poll call in your main loop, or in a
		* dedicated thread, or call it after every
		* rd_kafka_produce() call.
		* Just make sure that rd_kafka_poll() is still called
		* during periods where you are not producing any messages
		* to make sure previously produced messages have their
		* delivery report callback served (and any other callbacks
		* you register). */

		rd_kafka_poll(m_rk, 0/*non-blocking*/);
	}

	int			produce(const char* topic, char* putf8buf, int nbuflen)
	{
		rd_kafka_topic_t* _rkt = nullptr;

		//[1]
		string strkey = topic;
		map<string, rd_kafka_topic_t*>::iterator itb = m_map2rkt.find(strkey);
		if (itb == m_map2rkt.end())
		{
			_rkt = rd_kafka_topic_new(m_rk, topic, NULL);
			if (!_rkt)
			{
				fprintf(stderr, "%% Failed to create topic object: %s\n", rd_kafka_err2str(rd_kafka_last_error()));
				return (-3);
			}

			m_map2rkt.insert(make_pair(strkey, _rkt));
		}
		else
		{
			_rkt = itb->second;
		}

		//[2]
	retry:
		int iret = rd_kafka_produce(
			/* Topic object */
			_rkt,
			/* Use builtin partitioner to select partition*/
			RD_KAFKA_PARTITION_UA,
			/* Make a copy of the payload. */
			RD_KAFKA_MSG_F_COPY,
			/* Message payload (value) and length */
			putf8buf, nbuflen,
			/* Optional key and its length */
			NULL, 0,
			/* Message opaque, provided in
			* delivery report callback as
			* msg_opaque. */
			NULL);
		if (iret == (-1))
		{
			/**
			* Failed to *enqueue* message for producing.
			*/
			fprintf(stderr,
				"%% Failed to produce to topic %s: %s\n",
				rd_kafka_topic_name(_rkt),
				rd_kafka_err2str(rd_kafka_last_error()));

			/* Poll to handle delivery reports */
			if (rd_kafka_last_error() == RD_KAFKA_RESP_ERR__QUEUE_FULL)
			{
				/* If the internal queue is full, wait for
				* messages to be delivered and then retry.
				* The internal queue represents both
				* messages to be sent and messages that have
				* been sent or failed, awaiting their
				* delivery report callback to be called.
				*
				* The internal queue is limited by the
				* configuration property
				* queue.buffering.max.messages */
				rd_kafka_poll(m_rk, 1000/*block for max 1000ms*/);
				goto retry;
			}
		}
		else
		{
			fprintf(stderr, "Enqueued message (%d bytes) for topic %s\n", nbuflen, rd_kafka_topic_name(_rkt));
		}

		//[3]
		rd_kafka_poll(m_rk, 0/*non-blocking*/);
		return 1;
	}


private:
	rd_kafka_t*							m_rk;
	rd_kafka_conf_t*					m_conf;
	map<string, rd_kafka_topic_t*>		m_map2rkt;
};

//===============================================================================================================================================================================================================
//===============================================================================================================================================================================================================
//===============================================================================================================================================================================================================
//===============================================================================================================================================================================================================
MProduce::MProduce()
{
	m_pImp = nullptr;
}

MProduce::~MProduce()
{
	Release();
}

int						MProduce::Init(const char* brokers)
{
	Release();

	m_pImp = new MProduceImpl();

	return m_pImp->init(brokers);
}

void					MProduce::Release()
{
	if (m_pImp)
	{
		m_pImp->release();
		delete m_pImp;

		m_pImp = nullptr;
	}
}

void					MProduce::PeriodCall()
{
	if (m_pImp)
	{
		m_pImp->do_run();
	}
}

int						MProduce::Send(const char* topic, char* putf8buf, int nbuflen)
{
	if (m_pImp)
	{
		return m_pImp->produce(topic, putf8buf, nbuflen);
	}

	return (-1);
}













