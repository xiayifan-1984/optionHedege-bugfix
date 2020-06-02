

#查找当前目录下的所有源文件，保存到变量UTIL_SRCS
aux_source_directory(. UTIL_SRCS)

#生成链接库
add_library(util_park ${UTIL_SRCS} )


