

#include "MConsume.h"
#include "librdkafka/rdkafka.h"
#include <string.h>

class MConsumeImpl
{
public:
	MConsumeImpl()
	{
		m_rk = nullptr;
		m_conf = nullptr;
		m_topic_conf = nullptr;
		m_topics = nullptr;
		m_pfn = nullptr;
	}
	~MConsumeImpl()
	{

	}

public:
	int				init(const char* brokers, const char* group, pfnConsumeCB pfnCB, void* pthis)
	{
		//rd_kafka_resp_err_t err;
		int iret = 0;
		char errstr[255];
		char tmp[16];
		rd_kafka_conf_res_t  rest;

		m_pfn = pfnCB;
		m_pthis = pthis;

		//[0]
		/* Kafka configuration */
		m_conf = rd_kafka_conf_new();

		//quick termination  
		snprintf(tmp, sizeof(tmp), "%d", 0);
		rd_kafka_conf_set(m_conf, "internal.termination.signal", tmp, NULL, 0);

		//[1]
		/* Topic configuration */
		m_topic_conf = rd_kafka_topic_conf_new();

		//[2]
		/* Set logger */
		rd_kafka_conf_set_log_cb(m_conf, s_logger);

		char szgroup[64] = { 0 };
		if (group)
		{
			strcpy(szgroup, group);
		}
		else
		{
			strcpy(szgroup, "rdkafka_consumer_example");
		}

		rest = rd_kafka_conf_set(m_conf, "group.id", szgroup, errstr, sizeof(errstr));
		if (RD_KAFKA_CONF_OK != rest)
		{
			fprintf(stderr, "%s\n", errstr);
			return (-2);
		}

		/* Callback called on partition assignment changes */
		//rd_kafka_conf_set_rebalance_cb(m_conf, s_rebalance_cb);

		//rd_kafka_conf_set(m_conf, "enable.partition.eof", "true", NULL, 0);

		/* Set default topic config for pattern-matched topics. */
		rd_kafka_conf_set_default_topic_conf(m_conf, m_topic_conf);


		//[3]
		/* Create Kafka handle */
		m_rk = rd_kafka_new(RD_KAFKA_CONSUMER, m_conf, errstr, sizeof(errstr));
		if (!m_rk)
		{
			fprintf(stderr, "Failed to create new consumer: %s\n", errstr);
			return (-3);
		}

		/* Add brokers */
		iret = rd_kafka_brokers_add(m_rk, brokers);
		if (iret == 0)
		{
			fprintf(stderr, "%% No valid brokers specified\n");
			return (-4);
		}

		//[4]
		/* Redirect rd_kafka_poll() to consumer_poll() */
		rd_kafka_poll_set_consumer(m_rk);


		return 1;
	}

	int subscribe(int argc, char* argv[])
	{
		//[5]topics
		m_topics = rd_kafka_topic_partition_list_new(argc);
		int		partition = (-1);
		for (int i = 0; i < argc; i++)
		{
			char* topic = argv[i];

			rd_kafka_topic_partition_list_add(m_topics, topic, partition);
		}

		fprintf(stderr, "%% Subscribing to %d topics\n", m_topics->cnt);
		rd_kafka_resp_err_t err = rd_kafka_subscribe(m_rk, m_topics);
		if (err)
		{
			fprintf(stderr, "Failed to start consuming topics: %s\n", rd_kafka_err2str(err));

			return (-5);
		}
		return 1;
	}

	void			release()
	{
		rd_kafka_resp_err_t  err = rd_kafka_consumer_close(m_rk);

		if (err)
		{
			fprintf(stderr, "%% Failed to close consumer: %s\n", rd_kafka_err2str(err));
		}
		else
		{
			fprintf(stderr, "%% Consumer closed\n");
		}

		rd_kafka_topic_partition_list_destroy(m_topics);

		/* Destroy handle */
		rd_kafka_destroy(m_rk);

		/* Let background threads clean up and terminate cleanly. */
		int run = 5;
		while (run-- > 0 && rd_kafka_wait_destroyed(1000) == -1)
		{
			printf("Waiting for librdkafka to decommission\n");
		}

		if (run <= 0)
		{
			rd_kafka_dump(stdout, m_rk);
		}
	}

	void			runonce()
	{
		rd_kafka_message_t *rkmessage;

		rkmessage = rd_kafka_consumer_poll(m_rk, 1000);
		if (rkmessage)
		{
			s_msg_consume(rkmessage);
			rd_kafka_message_destroy(rkmessage);
		}
	}

protected:
	void		s_msg_consume(rd_kafka_message_t *rkmessage)
	{
		printf("s_msg_consume\n");
		if (rkmessage->err)	//有错误
		{
			if (rkmessage->err == RD_KAFKA_RESP_ERR__PARTITION_EOF)
			{
				fprintf(stderr,
					"Consumer reached end of %s [%d] message queue at offset %" PRId64"\n",
					rd_kafka_topic_name(rkmessage->rkt),
					rkmessage->partition, rkmessage->offset);

				return;
			}

			if (rkmessage->rkt)
			{
				fprintf(stderr, "Consume error for topic [%s] [%d] offset %" PRId64": %s\n",
					rd_kafka_topic_name(rkmessage->rkt),
					rkmessage->partition,
					rkmessage->offset,
					rd_kafka_message_errstr(rkmessage));
			}
			else
			{
				fprintf(stderr, "Consumer error: %s: %s\n",
					rd_kafka_err2str(rkmessage->err),
					rd_kafka_message_errstr(rkmessage));
			}

			if (rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_PARTITION ||
				rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_TOPIC)
			{
				//do nothing
			}

			return;
		}

		//成功返回
		{
			fprintf(stdout, "Message (topic [%s] [%d], offset %" PRId64", %d bytes):\n",
				rd_kafka_topic_name(rkmessage->rkt),
				rkmessage->partition,
				rkmessage->offset, rkmessage->len);
		}

		if (rkmessage->key_len)
		{
			printf("Key: %.*s\n", (int)rkmessage->key_len, (char *)rkmessage->key);
		}

		if (m_pfn)
		{
			m_pfn(m_pthis, rd_kafka_topic_name(rkmessage->rkt), rkmessage->partition, rkmessage->offset, 
						(int)rkmessage->len, (char *)rkmessage->payload);
		}
	}

	static void		s_logger(const rd_kafka_t *rk, int level, const char *fac, const char *buf)
	{
		/*
		struct timeval tv;
		gettimeofday(&tv, NULL);

		fprintf(stdout, "%u.%03u RDKAFKA-%i-%s: %s: %s\n", (int)tv.tv_sec, (int)(tv.tv_usec / 1000), level, fac, rd_kafka_name(rk), buf);

		*/
		printf("RDKAFKA-%d-%s: %s: %s\n", level, fac, rd_kafka_name(rk), buf);
	}

	static void		s_rebalance_cb(rd_kafka_t *rk, rd_kafka_resp_err_t err, rd_kafka_topic_partition_list_t *partitions, void *opaque)
	{
		fprintf(stderr, "Consumer group rebalanced: ");

		if (err == RD_KAFKA_RESP_ERR__ASSIGN_PARTITIONS)
		{
			fprintf(stderr, "assigned:\n");
			//print_partition_list(stderr, partitions);
			rd_kafka_assign(rk, partitions);
		}
		else if (err == RD_KAFKA_RESP_ERR__REVOKE_PARTITIONS)
		{
			fprintf(stderr, "revoked:\n");
			//print_partition_list(stderr, partitions);
			rd_kafka_assign(rk, NULL);
		}
		else
		{
			fprintf(stderr, "failed: %s\n", rd_kafka_err2str(err));
			rd_kafka_assign(rk, NULL);
		}
	}

private:
	rd_kafka_t *						m_rk;
	rd_kafka_conf_t *					m_conf;
	rd_kafka_topic_conf_t *				m_topic_conf;
	rd_kafka_topic_partition_list_t *	m_topics;
	pfnConsumeCB						m_pfn;
	void*								m_pthis;

};

//=================================================================================================================================================================================================
MConsume::MConsume()
{
	m_pImpl = nullptr;
	m_pfn = nullptr;
	m_pthis = nullptr;
}

MConsume::~MConsume()
{
	Release();
}

int				MConsume::Init(const char* brokers, const char* group, pfnConsumeCB pfnCB, void* pthis)
{
	Release();

	m_pfn = pfnCB;
	m_pthis = pthis;
	m_pImpl = new MConsumeImpl();
	return m_pImpl->init(brokers, group, m_pfn, m_pthis);
}

void			MConsume::Release()
{
	if (m_pImpl)
	{
		m_pImpl->release();
		delete m_pImpl;
		m_pImpl = nullptr;
	}
}

void			MConsume::Subscribe(int topicCount, char* argv[])
{
	if (m_pImpl)
	{
		m_pImpl->subscribe(topicCount, argv);
	}
}

void			MConsume::PeriodCall()
{
	if (m_pImpl)
	{
		m_pImpl->runonce();
	}
}



