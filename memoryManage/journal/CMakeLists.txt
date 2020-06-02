CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
PROJECT(journal)

IF(USE_SERVICE)
    ADD_DEFINITIONS(-DUSE_PAGED_SERVICE)
ENDIF(USE_SERVICE)

SET(LIB_UTIL_INCLUDE_FILES ../utils/memoryManage_declare.h ../utils/constants.h ../utils/Timer.h ../utils/json.hpp ../utils/redistool.hpp)
SET(LIB_UTIL_SOURCE_FILES ../utils/Timer.cpp)
SET(LIB_INCLUDE_FILES ../utils/memoryManage_declare.h ../utils/constants.h FrameHeader.h PageHeader.h PageUtil.h IPageProvider.h IStrategyUtil.h IJournalVisitor.h journal.h PageProvider.h JournalHandler.h JournalWriter.h JournalReader.h StrategyUtil.h StrategySocketHandler.h)
SET(LIB_SOURCE_FILES journal.cpp Frame.hpp PageUtil.cpp Page.cpp journal.cpp PageProvider.cpp JournalHandler.cpp JournalWriter.cpp JournalReader.cpp StrategyUtil.cpp)

ADD_LIBRARY(${PROJECT_NAME} SHARED ${LIB_UTIL_INCLUDE_FILES} ${LIB_UTIL_SOURCE_FILES} ${LIB_INCLUDE_FILES} ${LIB_SOURCE_FILES} )

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SOVERSION 1.1 VERSION 1.1)
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)


INSTALL(FILES ${LIB_UTIL_INCLUDE_FILES} DESTINATION "include")

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION "lib/memoryManage")
