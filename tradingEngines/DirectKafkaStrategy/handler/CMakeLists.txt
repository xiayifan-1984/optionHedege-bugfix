

include_directories(../util)
include_directories(../util2)

include_directories(/usr/local/include)


#查找当前目录下的所有源文件，保存到变量HANDLER_SRCS
aux_source_directory(. HANDLER_SRCS)

#生成链接库
add_library( handler ${HANDLER_SRCS} )

target_link_libraries(handler  util )
target_link_libraries(handler  util2 )
