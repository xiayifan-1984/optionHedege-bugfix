
#最小版本要求
cmake_minimum_required (VERSION 3.7)

#工程名称
PROJECT(ParkedKafkaStrategy)

#所有当前目录的cpp文件，加入到变量DIR_SRCS
aux_source_directory(. DIR_SRCS)

#头文件包含目录
include_directories(./  ./util ./util2 ./handler)

#cpp包含目录
add_subdirectory(util)
add_subdirectory(util2)
add_subdirectory(handler)

#其他定义
ADD_DEFINITIONS(-std=c++11)



#工程的目标是生成EXE
add_executable(${PROJECT_NAME}  ${DIR_SRCS} )


#工程自身的lib
target_link_libraries(${PROJECT_NAME}   util_park util2_park handler_park )

target_link_libraries(${PROJECT_NAME}   dl  rdkafka hiredis)


#工程其他的lib

link_directories(${BUILD_DIR}/lib/memoryManage)
link_directories(${BUILD_DIR}/lib/boost)
link_directories(${BUILD_DIR}/lib/tradingEngines)
link_directories(${BUILD_DIR}/lib/md)
target_link_libraries(${PROJECT_NAME} journal paged ctpmd optionHedgelog log4cplus tradingstrategy )

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} )

