#ifndef _H_REDIS_TOOLS_
#define _H_REDIS_TOOLS_
#include <iostream>
#include <vector>
#include <hiredis/hiredis.h>
#include <map>
#include <string.h>
#include <mutex>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <mutex>


using namespace std;

class RedisTool
{
public:
	RedisTool();
	~RedisTool();
	void init();
	int auth(const std::string& password = "123456");
	int setString(const std::string& key, const std::string& value);
	std::string getString(const std::string& key);
	std::vector<std::string> getKeysWithPattern(const std::string& pattern);

	int hmset(const std::string& hm, const std::string& key, const std::string& val);
	std::string hmget(const std::string& hm, const std::string& key);

	template<typename PairsContainer>
	int setHmWithPattern(const std::string& hm, const PairsContainer& fiels, const std::string pattern = "");

	template<typename PairsContainer>
	PairsContainer getHmWithPattern(const std::string& hm, const std::string& pattern);

	int delkey(const std::string& key);

	int setBits(const std::string& key, const char* bits, int len);
	template<typename T>
	int getBitsAsStruct(const std::string& key, T& value);

	int getBits(const std::string& key, char* pbuf);


private:
	redisContext *m_redis;
};

template<typename PairsContainer>
int RedisTool::setHmWithPattern(const std::string& hm, const PairsContainer& fields, const std::string pattern)
{
	int ret = 1;
	auto pat = pattern;
	std::cout << "size = " << fields.size() << std::endl;
	for (auto& ele : fields)
	{
		auto fieldKey = pat + "_" + ele.first;
		auto tret = hmset(hm, fieldKey, ele.second);
		redisReply* reply;
		{
			redisReply* reply;
			reply = (redisReply*)redisCommand(m_redis, "HSET %s %s %s", hm.c_str(), fieldKey.c_str(), ele.second.c_str());
		}

		if (reply == nullptr)
		{
			redisFree(m_redis);
			m_redis = nullptr;
			printf("ERROR getString: reply = NULL!!!!!!!!!!!! maybe redis server is down");
			tret = -1;
		}
		ret = tret != 1 ? tret : ret;
	}
	return ret;
}

template<typename PairsContainer>
PairsContainer RedisTool::getHmWithPattern(const std::string& hm, const std::string& pattern)
{
	PairsContainer result;
	if (m_redis == nullptr || m_redis->err)
	{
		std::cout << "Redis init Error !!!" << std::endl;
		init();
		return result;
	}

	redisReply* reply;
	reply = (redisReply*)redisCommand(m_redis, "HGETALL %s", hm.c_str());
	if (reply == nullptr || reply->type == REDIS_REPLY_ERROR)
	{
		fprintf(stderr, "redis del failure !\n");
	}
	else if (reply->type == REDIS_REPLY_ARRAY)
	{
		auto len = reply->elements;
		if (len % 2 != 0)
		{
			freeReplyObject(reply);
			return result;
		}
		auto element = reply->element;
		for (int i = 0; i < len; i += 2)
		{
			if (pattern.size() <= 0)
			{
				std::string key(element[i]->str);
				std::string value(element[i + 1]->str);
				result.insert({ key, value });
			}
			else
			{
				std::string tkey(element[i]->str, pattern.size());
				if (tkey == pattern)
				{
					std::string key(element[i]->str + pattern.size() + 1);
					std::string value(element[i + 1]->str);
					result.insert({ key, value });
				}
			}
		}
	}
	freeReplyObject(reply);
	return result;
}

template<typename T>
int RedisTool::getBitsAsStruct(const std::string& key, T& value)
{
	int ret = 1;
	if (m_redis == nullptr || m_redis->err)
	{
		std::cout << "Redis init Error !!!" << std::endl;
		init();
		return -1;
	}
	redisReply* reply = (redisReply*)redisCommand(m_redis, "GET %s", key.c_str());
	if (reply == nullptr)
	{
		redisFree(m_redis);
		m_redis = nullptr;
		printf("ERROR getString: reply = NULL!!!!!!!!!!!! maybe redis server is down");
		ret = -1;
	}
	else
	{
		if (reply->len <= 0)
		{
			ret = -1;
		}
		else
		{
			memcpy(&value, reply->str, reply->len);
		}

	}
	freeReplyObject(reply);
	return ret;
}

inline RedisTool::RedisTool()
{
	m_redis = nullptr;
	init();
}

inline RedisTool::~RedisTool()
{
	if (m_redis != NULL)
	{
		redisFree(m_redis);
		printf("~RedisTool :: free redis connection !\n");
	}
}

inline void RedisTool::init()
{
	struct  timeval timeout = { 1,500000 };
	char ip[255];

	//strncpy(ip, "127.0.0.1", strlen("127.0.0.1"));
	int port = 6379;
	m_redis = redisConnectWithTimeout("localhost", port, timeout);
	if (m_redis->err) {
		printf("RedisTool : Connection error: %s\n", m_redis->errstr);
		return;
	}
	else
	{
		cout << "init redis tool success " << endl;
		//REDIS_REPLY响应的类型type
		cout << "#define REDIS_REPLY_STRING 1" << endl;
		cout << "#define REDIS_REPLY_ARRAY 2" << endl;
		cout << "#define REDIS_REPLY_INTEGER 3" << endl;
		cout << "#define REDIS_REPLY_NIL 4" << endl;
		cout << "#define REDIS_REPLY_STATUS 5" << endl;
		cout << "#define REDIS_REPLY_ERROR 6" << endl;
	}
	//auth();
}

inline int RedisTool::auth(const string& password)
{
	if (m_redis == NULL || m_redis->err)//int err; /* Error flags, 错误标识，0表示无错误 */
	{
		std::cout << "Redis init Error !!!" << endl;
		init();
		return -1;
	}
	redisReply* reply;
	int result = -1;
	reply = (redisReply*)redisCommand(m_redis, "AUTH %s", password.c_str());
	if (reply)
	{
		if (strcmp(reply->str, "OK") == 0)
		{
			result = 1;
			cout << "authentic is successful! " << endl;
			return result;
		}
	}
	return result;
}

inline int RedisTool::setString(const string& key, const string& value)
{

	if (m_redis == NULL || m_redis->err)//int err; /* Error flags, 错误标识，0表示无错误 */
	{
		std::cout << "Redis init Error !!!" << endl;
		init();
		return -1;
	}
	redisReply *reply;
	{
		reply = (redisReply *)redisCommand(m_redis, "SET %s %s", key.c_str(), value.c_str());
	}

	int result = 0;
	if (reply == nullptr)
	{
		redisFree(m_redis);
		m_redis = nullptr;
		result = -1;
		cout << "set string fail : reply->str = NULL " << endl;
	}
	else if (strcmp(reply->str, "OK") == 0)
	{
		result = 1;
		cout << "set string type = " << reply->type << endl;
	}
	else
	{
		result = -1;
		cout << "set string fail :" << reply->str << endl;
	}
	freeReplyObject(reply);
	return result;
}

inline string RedisTool::getString(const string& key)
{
	string ret;
	if (m_redis == nullptr || m_redis->err)
	{
		std::cout << "Redis init Error !!!" << endl;
		init();
		return ret;
	}
	redisReply *reply;
	reply = (redisReply*)redisCommand(m_redis, "GET %s", key.c_str());
	int result;
	if (reply == nullptr)
	{
		redisFree(m_redis);
		m_redis = nullptr;
		cout << "ERROR getString: reply = NULL!!!!!!!!!!!! maybe redis server is down" << endl;
		return ret;
	}
	else if (reply->len <= 0)
	{
		cout << "Get no info!!!" << endl;
		freeReplyObject(reply);
		return ret;
	}
	else
	{
		stringstream ss;
		ss << reply->str;
		freeReplyObject(reply);
		return ss.str();
	}

}

inline int RedisTool::hmset(const string& hm, const string& key, const string& val)
{
	int ret = 1;
	if (m_redis == nullptr || m_redis->err)
	{
		cout << "Redis init Error !!!" << endl;
		init();
		return -1;
	}

	redisReply* reply;
	{
		reply = (redisReply*)redisCommand(m_redis, "HMSET %s %s %s", hm.c_str(), key.c_str(), val.c_str());
	}

	if (reply == nullptr)
	{
		redisFree(m_redis);
		m_redis = nullptr;
		cout << "ERROR getString: reply = NULL!!!!!!!!!!!! maybe redis server is down" << endl;
		ret = -1;
	}
	else
	{
		if (strncmp(reply->str, "OK", strlen("OK")) == 0)
		{
			ret = 1;
		}
		else
		{
			ret = -1;
		}
	}
	freeReplyObject(reply);
	return ret;
}

inline string RedisTool::hmget(const string& hm, const string& key)
{
	string result;
	if (m_redis == nullptr || m_redis->err)
	{
		cout << "Redis init Error !!!" << endl;
		init();
		return result;
	}
	redisReply* reply;
	reply = (redisReply*)redisCommand(m_redis, "HGET %s %s", hm.c_str(), key.c_str());
	if (reply == nullptr)
	{
		redisFree(m_redis);
		m_redis = nullptr;
		cout << "ERROR getString: reply = NULL!!!!!!!!!!!! maybe redis server is down" << endl;
		return result;
	}
	else
	{
		cout << "hm result str " << reply->str << " , size " << reply->len << endl;
		if (reply->len > 0 && reply->str != nullptr)
		{
			if (reply->type != REDIS_REPLY_NIL)
			{
				result = string(reply->str, reply->str + reply->len);
			}
		}
		freeReplyObject(reply);
		return result;        /* code */
	}

}

inline int RedisTool::delkey(const string& key)
{
	int ret = 1;
	if (m_redis == nullptr || m_redis->err)
	{
		cout << "Redis init Error !!!" << endl;
		init();
		return -1;
	}

	redisReply* reply;
	{
		reply = (redisReply*)redisCommand(m_redis, "DEL %s", key.c_str());
	}
	if (reply == NULL || reply->type == REDIS_REPLY_ERROR)
	{
		ret = -1;
		fprintf(stderr, "redis del failure !\n");
	}
	freeReplyObject(reply);
	return ret;
}

inline vector<string> RedisTool::getKeysWithPattern(const std::string& pattern)
{
	vector<string> ret;
	if (m_redis == nullptr || m_redis->err)
	{
		cout << "Redis init Error !!!" << endl;
		init();
		return ret;
	}
	redisReply* reply;
	reply = (redisReply*)redisCommand(m_redis, "KEYS %s", pattern.c_str());
	if (reply == nullptr || reply->type == REDIS_REPLY_ERROR)
	{
		fprintf(stderr, "redis del failure !\n");
	}
	else
	{
		if (reply->type == REDIS_REPLY_ARRAY)
		{
			auto len = reply->elements;
			auto element = reply->element;
			for (int i = 0; i < len; ++i)
			{
				string key(element[i]->str);
				ret.push_back(key);
			}
		}
	}
	return ret;
}

inline int RedisTool::setBits(const string& key, const char* bits, int len)
{
	int ret = 1;
	if (m_redis == nullptr || m_redis->err)
	{
		cout << "Redis init Error !!!" << endl;
		init();
		return -1;
	}
	const char *argv[3];
	size_t arglen[3];

	argv[0] = "set";
	arglen[0] = strlen("set");

	argv[1] = key.c_str();
	arglen[1] = key.size();

	printf("setbits %s, %d\n", argv[1], arglen[1]);

	argv[2] = bits;
	arglen[2] = len;

	redisReply* reply = (redisReply *)redisCommandArgv(m_redis, 3, argv, arglen);

	if (reply == nullptr)
	{
		redisFree(m_redis);
		m_redis = nullptr;
		cout << "ERROR getString: reply = NULL!!!!!!!!!!!! maybe redis server is down" << endl;
		ret = -1;
	}
	else
	{
		if (strncmp(reply->str, "OK", strlen("OK")) == 0)
		{
			ret = 1;
		}
		ret = -1;
	}
	freeReplyObject(reply);
	return ret;
}

inline int RedisTool::getBits(const string& key, char* pbuf)
{
	int ret = 0;
	if (m_redis == nullptr || m_redis->err)
	{
		std::cout << "Redis init Error !!!" << std::endl;
		init();
		return -1;
	}
	redisReply* reply = (redisReply*)redisCommand(m_redis, "GET %s", key.c_str());
	if (reply == nullptr)
	{
		redisFree(m_redis);
		m_redis = nullptr;
		printf("ERROR getString: reply = NULL!!!!!!!!!!!! maybe redis server is down");
		ret = -1;
	}
	else
	{
		if (!pbuf)
		{
			return reply->len;
		}
		if (reply->len <= 0)
		{
			ret = -1;
		}
		else
		{
			if (!pbuf)
			{
				return reply->len;
			}
			else
			{
				memcpy(pbuf, reply->str, reply->len);
			}

		}

	}
	freeReplyObject(reply);
	return ret;
}

#endif
