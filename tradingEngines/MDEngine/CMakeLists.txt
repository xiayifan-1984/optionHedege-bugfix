CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
PROJECT(MDEngine)

LINK_DIRECTORIES(${BUILD_DIR}/lib/memoryManage)
LINK_DIRECTORIES(${BUILD_DIR}/lib/boost)


ADD_EXECUTABLE(MDEngine main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} journal optionHedgelog paged ctpmd ${Boost_LIBRARIES})