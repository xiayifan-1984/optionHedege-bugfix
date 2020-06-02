CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
PROJECT(MD)

SET(CTP_SOURCE_FILES MDEngineCTP.cpp MDEngineCTP.h MDEngineSpi.h MDEngineSpi.cpp IMDEngine.cpp IMDEngine.h ../base/IEngine.cpp ../base/IEngine.h)
ADD_LIBRARY(ctpmd SHARED ${CTP_SOURCE_FILES})


TARGET_LINK_LIBRARIES(ctpmd journal optionHedgelog ${Boost_LIBRARIES})
INSTALL(TARGETS ctpmd DESTINATION "lib/md")
