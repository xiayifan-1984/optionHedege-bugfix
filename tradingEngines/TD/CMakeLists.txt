CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
PROJECT(td)

SET(TD_SOURCE_FILES  ITDEngine.cpp ITDEngine.h ../base/IEngine.cpp ../base/IEngine.h ../base/TDUserStruct.h ../base/TDUserStruct.cpp ../base/TDPosManage.h ../base/TDPosManage.cpp)

SET(CTP_SOURCE_FILES ${TD_SOURCE_FILES} TDEngineCTP.cpp TDEngineCTP.h TDEngineSpi.h TDEngineSpi.cpp)
ADD_LIBRARY(ctptd SHARED ${CTP_SOURCE_FILES})

TARGET_LINK_LIBRARIES(ctptd journal optionHedgelog ${Boost_LIBRARIES})
INSTALL(TARGETS ctptd DESTINATION "lib/td")
