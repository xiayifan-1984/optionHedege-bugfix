

include_directories(../util)
include_directories(../util2)

include_directories(/usr/local/include)


#���ҵ�ǰĿ¼�µ�����Դ�ļ������浽����HANDLER_SRCS
aux_source_directory(. HANDLER_SRCS)

#�������ӿ�
add_library( handler_park ${HANDLER_SRCS} )

target_link_libraries(handler_park  util_park )
target_link_libraries(handler_park  util2_park )
