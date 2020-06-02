#ifndef TRADINGENGINE_DECLEAR_H
#define TRADINGENGINE_DECLEAR_H

#include "../../memoryManage/utils/memoryManage_declare.h"
#include <thread>

typedef boost::shared_ptr<std::thread> ThreadPtr;

#define TE_NAMESPACE_START namespace optionHedge { namespace tradingEngine {
#define TE_NAMESPACE_END }};
#define USING_TE_NAMESPACE using namespace optionHedge::tradingEngine;

#define TICKER_MARKET_DELIMITER '@'

typedef int ValueType;
#define lockClose 1
#define lockYesterdayClose 2
#define lockTodayClose 3
#define yesPos 4
#define todayOpen 5
#define closE 6
#define todayClose 7
#define yesClose 8

typedef int OperationType;
#define Add 1
#define Sub 2

#endif
