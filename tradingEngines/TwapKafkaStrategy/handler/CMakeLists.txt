

include_directories(../util)
include_directories(../util2)

include_directories(/usr/local/include)


#把当前目录的cpp文件，加入到变量HANDLER_SRCS
aux_source_directory(. HANDLER_SRCS)

#生成链接库
add_library( handler_twap ${HANDLER_SRCS} )

target_link_libraries(handler_twap  util_twap )
target_link_libraries(handler_twap  util2_twap )
