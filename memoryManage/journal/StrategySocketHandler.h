#ifndef PROJECT_STRATEGYSOCKETHANDLER_H
#define PROJECT_STRATEGYSOCKETHANDLER_H

#include "../utils/memoryManage_declare.h"
#include "PageProvider.h"
#include "IStrategyUtil.h"

MM_NAMESPACE_START

class StrategySocketHandler :public ClientPageProvider, public IStrategyUtil
{
public:
	StrategySocketHandler(const string& strategy_name);
	virtual bool register_strategy(int& rid_start, int& rid_end);
	virtual bool td_connect(short source);
	virtual bool md_subscribe(const vector<string>& tickers, short source, short msg_type);
};

DECLARE_PTR(StrategySocketHandler);

MM_NAMESPACE_END
#endif
