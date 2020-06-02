
#最小版本
cmake_minimum_required (VERSION 3.7)

#工程名称
PROJECT(TwapKafkaStrategy)

#把当前目录的cpp文件，加入到变量DIR_SRCS
aux_source_directory(. DIR_SRCS)

#添加子目录
include_directories(./  ./util ./util2 ./handler)


#[3]添加编译选项
ADD_DEFINITIONS(-std=c++11)

add_subdirectory(util)
add_subdirectory(util2)
add_subdirectory(handler)

#[4]生成EXE
add_executable(${PROJECT_NAME}  ${DIR_SRCS} )


#[5]添加静态lib
target_link_libraries(${PROJECT_NAME}   util_twap util2_twap handler_twap )

target_link_libraries(${PROJECT_NAME}   dl  rdkafka hiredis)

link_directories(${BUILD_DIR}/lib/memoryManage)
link_directories(${BUILD_DIR}/lib/boost)
link_directories(${BUILD_DIR}/lib/tradingEngines)
link_directories(${BUILD_DIR}/lib/md)
target_link_libraries(${PROJECT_NAME} journal paged ctpmd optionHedgelog log4cplus tradingstrategy )

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} )

