CMAKE_MINIMUM_REQUIRED(VERSION 3.7)
PROJECT(optionHedge)



SET(BOOST_INCLUDEDIR "/usr/local/include/boost")
SET(BOOST_LIBRARYDIR "/usr/local/lib")

SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_STATIC_RUNTIME OFF)

FIND_PACKAGE(Boost 1.62.0 COMPONENTS locale date_time filesystem system thread serialization math_tr1 chrono regex program_options REQUIRED)
INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})
INCLUDE_DIRECTORIES(/usr/local/include)

file(STRINGS config.in ConfigContents)
foreach(NameAndValue ${ConfigContents})
    string(REGEX REPLACE "^[ ]+" "" NameAndValue ${NameAndValue})
    string(REGEX MATCH "^[^=]+" Name ${NameAndValue})
    string(REPLACE "${Name}=" "" Value ${NameAndValue})
    set(${Name} "${Value}")
    MESSAGE( STATUS ${Name} " = " ${Value} )
endforeach()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -ggdb -llog4cplus -lpthread -lboost_system -lboost_filesystem -lboost_regex  -lboost_program_options -ldl -lhiredis -L ./ -D_GLIBCXX_USE_CXX11_ABI=1 -O0 -pipe -Wall")
OPTION(BUILD_SHARED_LIBS "turn OFF for .a libs" ON)
OPTION(USE_SERVICE "use memory service" ON)
OPTION(USE_HASH_CODE "use hash code when writing" ON)

ADD_SUBDIRECTORY(system)
ADD_SUBDIRECTORY(memoryManage)
ADD_SUBDIRECTORY(configFile)
ADD_SUBDIRECTORY(tradingEngines)

INSTALL(DIRECTORY "${BOOST_LIBRARYDIR}/" DESTINATION "lib/boost")
