#include "IMDEngine.h"
#include "../../memoryManage/utils/Timer.h"
#include "../base/EngineUtil.hpp"
#include "../../system/systemConfig/sys_messages.h"
#include <boost/bind.hpp>

USING_TE_NAMESPACE

#define MD_API_PATH OPTIONHEDGE_ROOT_DIR "lib64/api/thostmduserapi_se.so"

IMDEngine::IMDEngine(short source):IEngine(source)
{
	subs_tickers.clear();
	subs_markets.clear();
	history_subs.clear();
	mdapiHandler= dlopen(MD_API_PATH, RTLD_LAZY | RTLD_GLOBAL);
	if (!mdapiHandler)
	{
		std::runtime_error("Can not create mdapiHandler");
	}
}

IMDEngine::~IMDEngine()
{
	dlclose(mdapiHandler);
}

void IMDEngine::set_reader_thread()
{
	reader_thread = ThreadPtr(new std::thread(boost::bind(&IMDEngine::listening,this)));
}

void IMDEngine::init()
{
	reader = optionHedge::memoryManage::JournalReader::createSysReader(name());
	JournalPair l1MdPair = getMdJournalPair(source_id);
	writer = optionHedge::memoryManage::JournalWriter::create(l1MdPair.first, l1MdPair.second);
}

void IMDEngine::listening()
{
	optionHedge::memoryManage::FramePtr frame;
	while (isRunning&&signal_received < 0)
	{
		frame = reader->getNextFrame();
		if (frame.get() != nullptr)
		{
			short msg_type = frame->getMsgType();
			short msg_source = frame->getSource();

			if (msg_source == source_id)
			{
				switch (msg_type)
				{
					case MSG_TYPE_SUBSCRIBE_MARKET_DATA:
					case MSG_TYPE_SUBSCRIBE_L2_MD:
					case MSG_TYPE_SUBSCRIBE_INDEX:
					case MSG_TYPE_SUBSCRIBE_ORDER_TRADE:
					{
						string ticker((char*)(frame->getData()));
						size_t found = ticker.find(TICKER_MARKET_DELIMITER);
						if (found != string::npos)
						{
							subs_tickers.push_back(ticker.substr(1, found));
							subs_markets.push_back(ticker.substr(found + 1));
							OPTIONHEDGE_LOG_DEBUG(logger, "[sub] (ticker)" << ticker.substr(0, found) << " (market)" << ticker.substr(found + 1));
						}
						else
						{
							subs_tickers.push_back(ticker);
							subs_markets.push_back("");
							OPTIONHEDGE_LOG_DEBUG(logger, "[sub] (ticker)" << ticker<<" market(null)");
						}

						SubCountMap& sub_counts = history_subs[msg_type];
						if (sub_counts.find(ticker) == sub_counts.end())
						{
							sub_counts[ticker] = 1;
						}

						else
						{
							sub_counts[ticker]++;
						}

						if (frame->getLastFlag() == 1)
						{
							if (is_logged_in())
							{
								if (msg_type == MSG_TYPE_SUBSCRIBE_MARKET_DATA)
								{
									subscribeMarketData(subs_tickers, subs_markets);
								}
								else if (msg_type == MSG_TYPE_SUBSCRIBE_L2_MD)
								{
									subscribeL2MD(subs_tickers, subs_markets);
								}
								else if (msg_type == MSG_TYPE_SUBSCRIBE_INDEX)
								{
									subscribeIndex(subs_tickers, subs_markets);
								}
								else if (msg_type == MSG_TYPE_SUBSCRIBE_ORDER_TRADE)
								{
									subscribeOrderTrade(subs_tickers, subs_markets);
								}
								subs_tickers.clear();
								subs_markets.clear();
							}
						}
					}
				}
			}

			if (msg_type == MSG_TYPE_MD_ENGINE_OPEN && (msg_source <= 0 || msg_source == source_id))
			{
				on_engine_open();
			}
			else if (msg_type == MSG_TYPE_MD_ENGINE_CLOSE && (msg_source <= 0 || msg_source == source_id))
			{
				on_engine_close();
			}
			else if (msg_type == MSG_TYPE_STRING_COMMAND)
			{
				string cmd((char*)frame->getData());
				on_command(cmd);
			}
		}
	}
	if (IEngine::signal_received >= 0)
	{
		OPTIONHEDGE_LOG_INFO(logger, "[IEngine] signal received: " << IEngine::signal_received);
	}

	if (!isRunning)
	{
		OPTIONHEDGE_LOG_INFO(logger, "[IEngine] forced to stop.");
	}
}

void IMDEngine::pre_run()
{
	subscribeHistorySubs();
}

void IMDEngine::subscribeHistorySubs()
{
	for (auto& iter : history_subs)
	{
		if (iter.second.size() == 0)
		{
			continue;
		}

		short msg_type = iter.first;
		vector<string> tickers, markets;

		for (auto& tickerIter : iter.second)
		{
			const string& ticker = tickerIter.first;
			size_t found = ticker.find(TICKER_MARKET_DELIMITER);
			if (found != string::npos)
			{
				tickers.push_back(ticker.substr(0, found));
				markets.push_back(ticker.substr(found + 1));
			}

			else
			{
				tickers.push_back(ticker);
				markets.push_back("");
			}
		}

		if (msg_type == MSG_TYPE_SUBSCRIBE_MARKET_DATA)
		{
			subscribeMarketData(tickers, markets);
		}
		else if (msg_type == MSG_TYPE_SUBSCRIBE_L2_MD)
		{
			subscribeL2MD(tickers, markets);
		}
		else if (msg_type == MSG_TYPE_SUBSCRIBE_INDEX)
		{
			subscribeIndex(tickers, markets);
		}
		else if (msg_type == MSG_TYPE_SUBSCRIBE_ORDER_TRADE)
		{
			subscribeOrderTrade(tickers, markets);
		}

		OPTIONHEDGE_LOG_INFO(logger, "[sub] history (msg_type)" << msg_type << " (num)" << tickers.size());

	}
}

void IMDEngine::on_market_data(const MarketDataField* data)
{
	if (isRunning)
	{
		writer->write_frame(data, sizeof(MarketDataField), source_id, MSG_TYPE_MD, 1/*islast*/, -1/*invalidRid*/);
		OPTIONHEDGE_LOG_DEBUG_FMT(logger, "%-10s[%7.1f, %4d | %7.1f, %4d]",
			data->InstrumentID,
			data->BidPrice1,
			data->BidVolume1,
			data->AskPrice1,
			data->AskVolume1);
	}
}
