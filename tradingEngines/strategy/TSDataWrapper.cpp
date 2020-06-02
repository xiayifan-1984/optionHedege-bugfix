#include "TSDataWrapper.h"
#include "TradingStrategyUtil.h"
#include "../../memoryManage/utils/Timer.h"
#include "../base/EngineUtil.hpp"
#include "../../system/systemConfig/sys_messages.h"
#include <sstream>

USING_TE_NAMESPACE

volatile int IDataProcessor::signal_received = -1;

TSDataWrapper::TSDataWrapper(IDataProcessor* processor, TradingStrategyUtil* util) :
	processor(processor), util(util), force_stop(false), auto_sub_mode_enabled(true)
{
	auto rids = util->getRequestIds();
	rid_start = rids.first;
	rid_end = rids.second;
	cur_time = optionHedge::memoryManage::getNanoTime();
//	processor->on_time(cur_time);
}

#define ADD_JOURNAL(func, source) \
    JournalPair jp = func(source); \
    folders.push_back(jp.first); \
    names.push_back(jp.second); \

void TSDataWrapper::add_market_data(short source)
{
	ADD_JOURNAL(getMdJournalPair, source);
}


void TSDataWrapper::add_register_td(short source)
{
	tds[source] = CONNECT_TD_STATUS_ADDED;
	ADD_JOURNAL(getTDJournalPair, source);
}


void TSDataWrapper::pre_run()
{
	/** build up reader now */
	reader = optionHedge::memoryManage::JournalReader::createReaderWithSys(folders, names, optionHedge::memoryManage::getNanoTime(), processor->get_name());
	/** request td login */
	
	for (auto& iter : tds)
	{
		short td_source = iter.first;
		util->td_connect(td_source);
		iter.second = CONNECT_TD_STATUS_REQUESTED;
	}
}

void TSDataWrapper::stop()
{
	force_stop = true;
}

void TSDataWrapper::run()
{
	pre_run();
	optionHedge::memoryManage::FramePtr frame;

	force_stop = false;
	processor->signal_received = -1;

	while (!force_stop && processor->signal_received <= 0)
	{
		frame = reader->getNextFrame();

		if (frame.get() != nullptr)
		{
			cur_time = frame->getNano();
			short msg_type = frame->getMsgType();
			short msg_source = frame->getSource();
			int request_id = frame->getRequestId();

			if (msg_type < 100)
			{
				if (msg_type == MSG_TYPE_SWITCH_TRADING_DAY)
				{
					/*
					for (auto iter : internal_pos)
					{
						if (iter.second.get() != nullptr)
							iter.second->switch_day();
					}
					processor->on_switch_day(cur_time);*/
				}
				else if (msg_type == MSG_TYPE_TRADE_ENGINE_ACK)
				{
					if (tds[msg_source] == CONNECT_TD_STATUS_REQUESTED)
					{
						string content((char*)frame->getData());
						process_td_ack(content, msg_source, cur_time);
					}
				}
			}
			else
			{
				void* data = frame->getData();
				if (request_id == -1)
				{
					util->set_md_nano(cur_time);
					switch (msg_type)
					{
						case MSG_TYPE_MD:
						{
							MarketDataField * md = (MarketDataField *)data;
							last_prices[md->InstrumentID] = md->LastPrice;
							processor->on_market_data(md, msg_source, cur_time);
							break;
						}
					}
					util->set_md_nano(-1);
				}

				if (rid_start <= request_id && request_id < rid_end)
				{
					switch (msg_type)
					{
						case MSG_TYPE_RSP_POS:
						{
							process_rsp_position((RspPositionField *)data,frame->getLastFlag(),request_id,msg_source,cur_time, frame->getErrorId(), frame->getErrorMsg());
							break;
						}
						case MSG_TYPE_RTN_QRY_ORDER:
						{
							processor->on_rsp_qry_order((RtnOrderField*)data, request_id, msg_source, cur_time, frame->getLastFlag(), frame->getErrorId(), frame->getErrorMsg());
							break;
						}
						case MSG_TYPE_RTN_QRY_TRADE:
						{
							processor->on_rsp_qry_trade((RtnTradeField*)data, request_id, msg_source, cur_time, frame->getLastFlag(),frame->getErrorId(), frame->getErrorMsg());
							break;
						}
						case MSG_TYPE_ORDER:
						{
							processor->on_rsp_order((InputOrderField*)data,request_id,msg_source,cur_time,frame->getErrorId(), frame->getErrorMsg());
							break;
						}
						case MSG_TYPE_RTN_ORDER:
						{
							RtnOrderField * rtn_order = (RtnOrderField *)data;
							order_statuses[request_id] = rtn_order->OrderStatus;
							processor->on_rtn_order(rtn_order, request_id, msg_source, cur_time);
							break;
						}
						case MSG_TYPE_RTN_TRADE:
						{
							//internal_pos[msg_source]->update((RtnTradeField *)data);
							processor->on_rtn_trade((RtnTradeField *)data, request_id, msg_source, cur_time);
							break;
						}
						case MSG_TYPE_RTN_INS_STATUS:
						{
							processor->on_rtn_instrument_status((RspInstrumentStatus *)data, request_id, msg_source, cur_time);
							break;
						}
						case MSG_TYPE_RSP_ACCOUNT:
						{
							processor->on_rsp_account((RspAccountField *)data, request_id, msg_source, cur_time, frame->getLastFlag(), frame->getErrorId(), frame->getErrorMsg());
							break;
						}
						case MSG_TYPE_ORDER_ACTION_RSP:
						{
							processor->on_rsp_action((RspActionField*)data, request_id, msg_source, cur_time);
							break;
						}
					}
				}
			}
		}
		else
		{
			cur_time = optionHedge::memoryManage::getNanoTime();
		}
		//processor->on_time(cur_time);
	}
	if (processor->signal_received > 0)
	{
		std::stringstream ss;
		ss << "[DataWrapper] signal received: " << processor->signal_received;
		//processor->debug(ss.str().c_str());
	}

	if (force_stop)
	{
		//processor->debug("[DataWrapper] forced to stop!");
	}
}

void TSDataWrapper::process_rsp_position(const RspPositionField* pos, bool is_last, int request_id, short source, long rcv_time, short errorId, const char* errorMsg)
{
	/*
	PosHandlerPtr& handler = pos_handlers[request_id];
	if (handler.get() == nullptr)
	{
		handler = PosHandler::create(source);
	}
	if (strlen(pos->InstrumentID) > 0)
	{
		handler->add_pos(pos->InstrumentID, pos->PosiDirection, pos->Position, pos->YdPosition, pos->PositionCost);
	}*/
//	if (is_last)
//	{
//		PosHandlerPtr& handler = nullptr;
		processor->on_rsp_position(pos, request_id, source, rcv_time,is_last,errorId,errorMsg);
//		handler.reset();
//	}
}

void TSDataWrapper::process_td_ack(const string& content, short source, long rcv_time)
{
	try
	{
		json j_ack = json::parse(content);
		if (j_ack["name"].get<string>().compare(processor->get_name()) == 0)
		{
			bool ready = j_ack["ok"].get<bool>();
			processor->on_td_login(ready, j_ack, source);
			/*
			if (j_ack.find(PH_FEE_SETUP_KEY) != j_ack.end())
			{
				fee_handlers[source] = FeeHandlerPtr(new FeeHandler(j_ack[PH_FEE_SETUP_KEY]));
			}*/
			if (ready)
			{
//				PosHandlerPtr handler = PosHandler::create(source, content);
//				set_internal_pos(handler, source);
				tds[source] = CONNECT_TD_STATUS_ACK_POS;
//				processor->on_rsp_position(handler, -1, source, cur_time);
			}
			else
			{
				tds[source] = CONNECT_TD_STATUS_ACK_NONE;
//				processor->on_rsp_position(PosHandlerPtr(), -1, source, cur_time);
			}
		}
	}
	catch (...)
	{
		std::stringstream ss;
		ss << "[ERROR] processing td ack: " << content;
	}
}

byte TSDataWrapper::get_td_status(short source) const
{
	auto iter = tds.find(source);
	if (iter == tds.end())
		return CONNECT_TD_STATUS_UNKNOWN;
	else
		return iter->second;
}

vector<int> TSDataWrapper::get_effective_orders() const
{
	vector<int> res;
	for (auto iter : order_statuses)
	{
		char status = iter.second;
		if (status != CHAR_Error && status != CHAR_AllTraded && status != CHAR_Canceled && status != CHAR_PartTradedNotQueueing)
		{
			res.push_back(iter.first);
		}
	}
}
