CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
PROJECT(tools)

LINK_DIRECTORIES(${BUILD_DIR}/lib/memoryManage)
LINK_DIRECTORIES(${BUILD_DIR}/lib/boost)
LINK_DIRECTORIES(${BUILD_DIR}/lib/tradingEngines)

ADD_EXECUTABLE(journal_toolEXE main.cpp)
TARGET_LINK_LIBRARIES(journal_toolEXE journal optionHedgelog paged)
