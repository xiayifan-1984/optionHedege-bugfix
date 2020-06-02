CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
PROJECT(paged)

SET(LIB_SOURCE_FILES PageSocketHandler.cpp PageSocketHandler.h PageCommStruct.h PageSocketStruct.h PageEngine.cpp PageEngine.h)

SET(LIB_FILES ${LIB_SOURCE_FILES})
ADD_LIBRARY(${PROJECT_NAME} ${LIB_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} journal optionHedgelog)

INSTALL(FILES PageCommStruct.h PageSocketStruct.h DESTINATION "include")
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION "lib/memoryManage")