CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
PROJECT(tradingstrategy)

SET(STRATEGY_SOURCE_FILES TSDataWrapper.cpp TSDataWrapper.h IStrategy.h IStrategy.cpp TradingStrategyUtil.h TradingStrategyUtil.cpp)
ADD_LIBRARY(${PROJECT_NAME} SHARED ${STRATEGY_SOURCE_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} journal optionHedgelog ${Boost_LIBRARIES})
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION "lib/tradingstrategy")

INSTALL(FILES ../base/TE_DECLEAR.h IDataProcessor.h IStrategy.h TSDataWrapper.h TradingStrategyUtil.h DESTINATION "include")
